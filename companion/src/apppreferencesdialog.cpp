#include "apppreferencesdialog.h"
#include "ui_apppreferencesdialog.h"
#include "mainwindow.h"
#include "appdata.h"
#include "helpers.h"
#include "firmwareinterface.h"
#ifdef JOYSTICKS
#include "joystick.h"
#include "joystickdialog.h"
#endif
#include <QDesktopServices>
#include <QtGui>

AppPreferencesDialog::AppPreferencesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AppPreferencesDialog)
{
  ui->setupUi(this);
  updateLock=false;
  setWindowIcon(CompanionIcon("apppreferences.png"));

  initSettings();
  connect(ui->downloadVerCB, SIGNAL(currentIndexChanged(int)), this, SLOT(baseFirmwareChanged()));
  connect(this, SIGNAL(accepted()), this, SLOT(writeValues()));

#ifndef JOYSTICKS
  ui->joystickCB->hide();
  ui->joystickCB->setDisabled(true);
  ui->joystickcalButton->hide();
  ui->joystickChkB->hide();
  ui->label_11->hide();
#endif

  shrink();
}

AppPreferencesDialog::~AppPreferencesDialog()
{
  delete ui;
}

void AppPreferencesDialog::writeValues()
{
  g.useCompanionNightlyBuilds(ui->useCompanionNightlyBuilds->isChecked());
  g.autoCheckApp(ui->autoCheckCompanion->isChecked());
  g.useFirmwareNightlyBuilds(ui->useFirmwareNightlyBuilds->isChecked());
  g.autoCheckFw(ui->autoCheckFirmware->isChecked());
  g.showSplash(ui->showSplash->isChecked());
  g.simuSW(ui->simuSW->isChecked());
  g.useWizard(ui->modelWizard_CB->isChecked());
  g.historySize(ui->historySize->value());
  g.backLight(ui->backLightColor->currentIndex());
  g.profile[g.id()].volumeGain(round(ui->volumeGain->value() * 10.0));
  g.libDir(ui->libraryPath->text());
  g.gePath(ui->ge_lineedit->text());
  g.embedSplashes(ui->splashincludeCB->currentIndex());
  g.enableBackup(ui->backupEnable->isChecked());

  if (ui->joystickChkB ->isChecked() && ui->joystickCB->isEnabled()) {
    g.jsSupport(ui->joystickChkB ->isChecked());  
    g.jsCtrl(ui->joystickCB ->currentIndex());
  }
  else {
    g.jsSupport(false);
    g.jsCtrl(0);
  }
  g.profile[g.id()].channelOrder(ui->channelorderCB->currentIndex());
  g.profile[g.id()].defaultMode(ui->stickmodeCB->currentIndex());
  g.profile[g.id()].renameFwFiles(ui->renameFirmware->isChecked());
  g.profile[g.id()].burnFirmware(ui->burnFirmware->isChecked());
  g.profile[g.id()].sdPath(ui->sdPath->text());
  g.profile[g.id()].pBackupDir(ui->profilebackupPath->text());
  g.profile[g.id()].penableBackup(ui->pbackupEnable->isChecked());
  g.profile[g.id()].splashFile(ui->SplashFileName->text());

  // The profile name may NEVER be empty
  if (ui->profileNameLE->text().isEmpty())
    g.profile[g.id()].name(tr("My Radio"));
  else
    g.profile[g.id()].name(ui->profileNameLE->text());

  // If a new fw type has been choosen, several things need to reset
  current_firmware_variant = getFirmwareVariant();
  QString id = current_firmware_variant->getId();
  if (g.profile[g.id()].fwType() != id) {
    g.profile[g.id()].fwName("");
    g.profile[g.id()].initFwVariables();
    g.profile[g.id()].fwType(id);
  }
}

void AppPreferencesDialog::on_snapshotPathButton_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your snapshot folder"), g.snapshotDir());
  if (!fileName.isEmpty()) {
    g.snapshotDir(fileName);
    g.snapToClpbrd(false);
    ui->snapshotPath->setText(fileName);
  }
}

void AppPreferencesDialog::initSettings()
{
  ui->snapshotClipboardCKB->setChecked(g.snapToClpbrd());
  ui->burnFirmware->setChecked(g.profile[g.id()].burnFirmware());
  ui->snapshotPath->setText(g.snapshotDir());
  ui->snapshotPath->setReadOnly(true);
  if (ui->snapshotClipboardCKB->isChecked()) {
    ui->snapshotPath->setDisabled(true);
    ui->snapshotPathButton->setDisabled(true);
  }
  ui->useCompanionNightlyBuilds->setChecked(g.useCompanionNightlyBuilds());
  ui->autoCheckCompanion->setChecked(g.autoCheckApp());
  ui->useFirmwareNightlyBuilds->setChecked(g.useFirmwareNightlyBuilds());
  ui->autoCheckFirmware->setChecked(g.autoCheckFw());
  ui->showSplash->setChecked(g.showSplash());
  ui->historySize->setValue(g.historySize());
  ui->backLightColor->setCurrentIndex(g.backLight());
  ui->volumeGain->setValue(g.profile[g.id()].volumeGain() / 10.0);

  if (IS_TARANIS(GetCurrentFirmware()->getBoard())) {
    ui->backLightColor->setEnabled(false);
  }

  ui->simuSW->setChecked(g.simuSW());
  ui->modelWizard_CB->setChecked(g.useWizard());
  ui->libraryPath->setText(g.libDir());
  ui->ge_lineedit->setText(g.gePath());

  if (!g.backupDir().isEmpty()) {
    if (QDir(g.backupDir()).exists()) {
      ui->backupPath->setText(g.backupDir());
      ui->backupEnable->setEnabled(true);
      ui->backupEnable->setChecked(g.enableBackup());
    } else {
      ui->backupEnable->setDisabled(true);
    }
  } else {
      ui->backupEnable->setDisabled(true);
  }
  ui->splashincludeCB->setCurrentIndex(g.embedSplashes());

#ifdef JOYSTICKS
  ui->joystickChkB->setChecked(g.jsSupport());
  if (ui->joystickChkB->isChecked()) {
    QStringList joystickNames;
    joystickNames << tr("No joysticks found");
    joystick = new Joystick(0,false,0,0);
    ui->joystickcalButton->setDisabled(true);
    ui->joystickCB->setDisabled(true);

    if ( joystick ) {
      if ( joystick->joystickNames.count() > 0 ) {
        joystickNames = joystick->joystickNames;
        ui->joystickCB->setEnabled(true);
        ui->joystickcalButton->setEnabled(true);
      }
      joystick->close();
    }
    ui->joystickCB->clear();
    ui->joystickCB->insertItems(0, joystickNames);
    ui->joystickCB->setCurrentIndex(g.jsCtrl());
  }
  else {
    ui->joystickCB->clear();
    ui->joystickCB->setDisabled(true);
    ui->joystickcalButton->setDisabled(true);
  }
#endif  
//  Profile Tab Inits  
  ui->channelorderCB->setCurrentIndex(g.profile[g.id()].channelOrder());
  ui->stickmodeCB->setCurrentIndex(g.profile[g.id()].defaultMode());
  ui->renameFirmware->setChecked(g.profile[g.id()].renameFwFiles());
  ui->sdPath->setText(g.profile[g.id()].sdPath());
  if (!g.profile[g.id()].pBackupDir().isEmpty()) {
    if (QDir(g.profile[g.id()].pBackupDir()).exists()) {
      ui->profilebackupPath->setText(g.profile[g.id()].pBackupDir());
      ui->pbackupEnable->setEnabled(true);
      ui->pbackupEnable->setChecked(g.profile[g.id()].penableBackup());
    } else {
      ui->pbackupEnable->setDisabled(true);
    }
  } else {
      ui->pbackupEnable->setDisabled(true);
  }

  ui->profileNameLE->setText(g.profile[g.id()].name());
  ui->SplashFileName->setText(g.profile[g.id()].splashFile());

  displayImage( g.profile[g.id()].splashFile() );

  QString hwSettings;
  if (g.profile[g.id()].stickPotCalib() == "" ) {
    hwSettings = tr("EMPTY: No radio settings stored in profile");
  }
  else  {
    QString str = g.profile[g.id()].timeStamp();
    if (str.isEmpty())
      hwSettings = tr("AVAILABLE: Radio settings of unknown age");
    else
      hwSettings = tr("AVAILABLE: Radio settings stored %1").arg(str);
  }
  ui->lblGeneralSettings->setText(hwSettings);

  Firmware * current_firmware = GetCurrentFirmware();

  foreach(Firmware * firmware, firmwares) {
    ui->downloadVerCB->addItem(firmware->getName(), firmware->getId());
    if (current_firmware->getFirmwareBase() == firmware) {
      ui->downloadVerCB->setCurrentIndex(ui->downloadVerCB->count() - 1);
    }
  }
  
  baseFirmwareChanged();
}

void AppPreferencesDialog::on_libraryPathButton_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your library folder"), g.libDir());
  if (!fileName.isEmpty()) {
    g.libDir(fileName);
    ui->libraryPath->setText(fileName);
  }
}

void AppPreferencesDialog::on_snapshotClipboardCKB_clicked()
{
  if (ui->snapshotClipboardCKB->isChecked()) {
    ui->snapshotPath->setDisabled(true);
    ui->snapshotPathButton->setDisabled(true);
    g.snapToClpbrd(true);
  }
  else {
    ui->snapshotPath->setEnabled(true);
    ui->snapshotPath->setReadOnly(true);
    ui->snapshotPathButton->setEnabled(true);
    g.snapToClpbrd(false);
  }
}

void AppPreferencesDialog::on_backupPathButton_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your Models and Settings backup folder"), g.backupDir());
  if (!fileName.isEmpty()) {
    g.backupDir(fileName);
    ui->backupPath->setText(fileName);
    ui->backupEnable->setEnabled(true);
  }
}

void AppPreferencesDialog::on_ProfilebackupPathButton_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your Models and Settings backup folder"), g.backupDir());
  if (!fileName.isEmpty()) {
    ui->profilebackupPath->setText(fileName);
    ui->pbackupEnable->setEnabled(true);
  }
}

void AppPreferencesDialog::on_ge_pathButton_clicked()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Select Google Earth executable"),ui->ge_lineedit->text());
  if (!fileName.isEmpty()) {
    ui->ge_lineedit->setText(fileName);
  }
}
 
#ifdef JOYSTICKS
void AppPreferencesDialog::on_joystickChkB_clicked() {
  if (ui->joystickChkB->isChecked()) {
    QStringList joystickNames;
    joystickNames << tr("No joysticks found");
    joystick = new Joystick(0,false,0,0);
    ui->joystickcalButton->setDisabled(true);
    ui->joystickCB->setDisabled(true);

    if ( joystick ) {
      if ( joystick->joystickNames.count() > 0 ) {
        joystickNames = joystick->joystickNames;
        ui->joystickCB->setEnabled(true);
        ui->joystickcalButton->setEnabled(true);
      }
      joystick->close();
    }
    ui->joystickCB->clear();
    ui->joystickCB->insertItems(0, joystickNames);
  }
  else {
    ui->joystickCB->clear();
    ui->joystickCB->setDisabled(true);
    ui->joystickcalButton->setDisabled(true);
  }
}

void AppPreferencesDialog::on_joystickcalButton_clicked() {
   joystickDialog * jd=new joystickDialog(this, ui->joystickCB->currentIndex());
   jd->exec();
}
#endif

// ******** Profile tab functions

void AppPreferencesDialog::on_sdPathButton_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select the folder replicating your SD structure"), g.profile[g.id()].sdPath());
  if (!fileName.isEmpty()) {
    ui->sdPath->setText(fileName);
  }
}

void AppPreferencesDialog::on_removeProfileButton_clicked()
{
  if ( g.id() == 0 ) {
     QMessageBox::information(this, tr("Not possible to remove profile"), tr("The default profile can not be removed."));
  }
  else {
    g.profile[g.id()].remove();
    g.id( 0 );
    initSettings();
  }
}


bool AppPreferencesDialog::displayImage( QString fileName )
{
  // Start by clearing the pixmap
  ui->imageLabel->setPixmap(QPixmap());

  QImage image(fileName);
  if (image.isNull()) 
    return false;

  // Use the firmware name to determine splash width
  int width = SPLASH_WIDTH;
  if (g.profile[g.id()].fwType().contains("taranis"))
    width = SPLASHX9D_WIDTH;
  
  ui->imageLabel->setPixmap( makePixMap( image, g.profile[g.id()].fwType()));
  ui->imageLabel->setFixedSize(width, SPLASH_HEIGHT);
  return true;
}

void AppPreferencesDialog::on_SplashSelect_clicked()
{
  QString supportedImageFormats;
  for (int formatIndex = 0; formatIndex < QImageReader::supportedImageFormats().count(); formatIndex++) {
    supportedImageFormats += QLatin1String(" *.") + QImageReader::supportedImageFormats()[formatIndex];
  }

  QString fileName = QFileDialog::getOpenFileName(this,
          tr("Open Image to load"), g.imagesDir(), tr("Images (%1)").arg(supportedImageFormats));

  if (!fileName.isEmpty()){
    g.imagesDir(QFileInfo(fileName).dir().absolutePath());
   
    displayImage(fileName);
    ui->SplashFileName->setText(fileName);
  }
}

void AppPreferencesDialog::on_clearImageButton_clicked()
{
  ui->imageLabel->clear();
  ui->SplashFileName->clear();
}


void AppPreferencesDialog::showVoice(bool show)
{
  ui->voiceLabel->setVisible(show);
  ui->voiceCombo->setVisible(show);
  QTimer::singleShot(0, this, SLOT(shrink()));
}

void AppPreferencesDialog::baseFirmwareChanged()
{
  QVariant selected_firmware = ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex());

  foreach(Firmware * firmware, firmwares) {
    if (firmware->getId() == selected_firmware) {
      populateFirmwareOptions(firmware);
      break;
    }
  }
}

Firmware * AppPreferencesDialog::getFirmwareVariant()
{
  QVariant selected_firmware = ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex());

  foreach(Firmware * firmware, firmwares) {
    QString id = firmware->getId();
    if (id == selected_firmware) {
      foreach(QCheckBox *cb, optionsCheckBoxes) {
        if (cb->isChecked()) {
          id += QString("-") + cb->text();
        }
      }

      if (voice && voice->isChecked()) {
        id += QString("-tts") + ui->voiceCombo->currentText();
      }

      if (ui->langCombo->count()) {
        id += QString("-") + ui->langCombo->currentText();
      }

      return GetFirmware(id);
    }
  }

  // Should never occur...
  return default_firmware_variant;
}

void AppPreferencesDialog::firmwareOptionChanged(bool state)
{
  QCheckBox *cb = qobject_cast<QCheckBox*>(sender());
  if (cb == voice) {
    showVoice(voice->isChecked());
  }
  Firmware * firmware=NULL;
  if (cb && state) {
    QVariant selected_firmware = ui->downloadVerCB->itemData(ui->downloadVerCB->currentIndex());
    foreach(firmware, firmwares) {
      if (firmware->getId() == selected_firmware) {
        foreach(QList<Option> opts, firmware->opts) {
          foreach(Option opt, opts) {
            if (cb->text() == opt.name) {
              foreach(Option other, opts) {
                if (other.name != opt.name) {
                  foreach(QCheckBox *ocb, optionsCheckBoxes) {
                    if (ocb->text() == other.name) {
                      ocb->setChecked(false);
                    }
                  }
                }
              }
              return;
            }
          }
        }
      }
    }
  } 
}

void AppPreferencesDialog::populateFirmwareOptions(const Firmware * firmware)
{
  const Firmware * parent = firmware->getFirmwareBase();

  updateLock = true;

  QString id = current_firmware_variant->getId();
  ui->langCombo->clear();
  foreach(const char *lang, parent->languages) {
    ui->langCombo->addItem(lang);
    if (id.endsWith(lang)) {
      ui->langCombo->setCurrentIndex(ui->langCombo->count() - 1);
    }
  }

  voice = NULL; // we will search for a voice checkbox

  int index = 0;
  QWidget * prevFocus = ui->voiceCombo;
  foreach(QList<Option> opts, parent->opts) {
    foreach(Option opt, opts) {
      if (index >= optionsCheckBoxes.size()) {
        QCheckBox * checkbox = new QCheckBox(ui->profileTab);
        ui->optionsLayout->addWidget(checkbox, optionsCheckBoxes.count()/5, optionsCheckBoxes.count()%5, 1, 1);
        optionsCheckBoxes.push_back(checkbox);
        connect(checkbox, SIGNAL(toggled(bool)), this, SLOT(firmwareOptionChanged(bool)));
        if (prevFocus) {
          QWidget::setTabOrder(prevFocus, checkbox);
        }
      }

      QCheckBox *cb = optionsCheckBoxes.at(index++);
      if (cb) {
        cb->show();
        cb->setText(opt.name);
        cb->setToolTip(opt.tooltip);
        cb->setCheckState(id.contains(opt.name) ? Qt::Checked : Qt::Unchecked);
        if (opt.name == QString("voice")) {
          voice = cb;
        }
        prevFocus = cb;
      }
    }
  }

  for (; index<optionsCheckBoxes.size(); index++) {
    QCheckBox *cb = optionsCheckBoxes.at(index);
    cb->hide();
    cb->setCheckState(Qt::Unchecked);
  }

  ui->voiceCombo->clear();
  foreach(const char *lang, parent->ttslanguages) {
    ui->voiceCombo->addItem(lang);
    if (id.contains(QString("-tts%1").arg(lang))) {
      ui->voiceCombo->setCurrentIndex(ui->voiceCombo->count() - 1);
    }
  }

  showVoice(voice && voice->isChecked());

  updateLock = false;
  QTimer::singleShot(0, this, SLOT(shrink()));
}

void AppPreferencesDialog::shrink()
{
  resize(0,0);
}

