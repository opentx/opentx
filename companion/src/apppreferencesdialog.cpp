#include "apppreferencesdialog.h"
#include "ui_apppreferencesdialog.h"
#include "mainwindow.h"
#include "helpers.h"
#ifdef JOYSTICKS
#include "joystick.h"
#include "joystickdialog.h"
#endif
#include <QDesktopServices>
#include <QtGui>

appPreferencesDialog::appPreferencesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::appPreferencesDialog)
{
  ui->setupUi(this);
  setWindowIcon(CompanionIcon("apppreferences.png"));
  initSettings();
  connect(this, SIGNAL(accepted()), this, SLOT(writeValues()));
#ifndef JOYSTICKS
  ui->joystickCB->hide();
  ui->joystickCB->setDisabled(true);
  ui->joystickcalButton->hide();
  ui->joystickChkB->hide();
  ui->label_11->hide();
#endif
  resize(0,0);
}

appPreferencesDialog::~appPreferencesDialog()
{
  delete ui;
}

void appPreferencesDialog::writeValues()
{
  QSettings settings;
  settings.setValue("startup_check_companion", ui->startupCheck_companion9x->isChecked());
  settings.setValue("startup_check_fw", ui->startupCheck_fw->isChecked());
  settings.setValue("wizardEnable", ui->wizardEnable_ChkB->isChecked());
  settings.setValue("show_splash", ui->showSplash->isChecked());
  settings.setValue("simuSW", ui->simuSW->isChecked());
  settings.setValue("history_size", ui->historySize->value());
  settings.setValue("backLight", ui->backLightColor->currentIndex());
  settings.setValue("libraryPath", ui->libraryPath->text());
  settings.setValue("gePath", ui->ge_lineedit->text());
  settings.setValue("embedded_splashes", ui->splashincludeCB->currentIndex());
  settings.setValue("backupEnable", ui->backupEnable->isChecked());

  if (ui->joystickChkB ->isChecked() && ui->joystickCB->isEnabled()) {
    settings.setValue("js_support", ui->joystickChkB ->isChecked());  
    settings.setValue("js_ctrl", ui->joystickCB ->currentIndex());
  }
  else {
    settings.remove("js_support");
    settings.remove("js_ctrl");
  }

  settings.setValue("default_channel_order", ui->channelorderCB->currentIndex());
  settings.setValue("default_mode", ui->stickmodeCB->currentIndex());
  settings.setValue("rename_firmware_files", ui->renameFirmware->isChecked());
  settings.setValue("burnFirmware", ui->burnFirmware->isChecked());
  settings.setValue("profileId", ui->profileIndexLE->text());
  settings.setValue("Name", ui->profileNameLE->text());
  settings.setValue("sdPath", ui->sdPath->text());
  settings.setValue("SplashFileName", ui->SplashFileName->text());
  if (!ui->SplashFileName->text().isEmpty())
    settings.setValue("SplashImage", "");
  settings.setValue("firmware", ui->firmwareLE->text());
  
  saveProfile();
}

void appPreferencesDialog::on_snapshotPathButton_clicked()
{
  QSettings settings;
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your snapshot folder"), settings.value("snapshotPath").toString());
  if (!fileName.isEmpty()) {
    settings.setValue("snapshotpath", fileName);
    settings.setValue("snapshot_to_clipboard", false);
    ui->snapshotPath->setText(fileName);
  }
}

void appPreferencesDialog::initSettings()
{
  QSettings settings;
  ui->snapshotClipboardCKB->setChecked(settings.value("snapshot_to_clipboard", false).toBool());
  ui->burnFirmware->setChecked(settings.value("burnFirmware", true).toBool());
  
  QString Path=settings.value("snapshotPath", "").toString();
  if (QDir(Path).exists()) {
    ui->snapshotPath->setText(Path);
    ui->snapshotPath->setReadOnly(true);
  }
  if (ui->snapshotClipboardCKB->isChecked()) {
    ui->snapshotPath->setDisabled(true);
    ui->snapshotPathButton->setDisabled(true);
  }
  ui->startupCheck_companion9x->setChecked(settings.value("startup_check_companion", true).toBool());
  ui->startupCheck_fw->setChecked(settings.value("startup_check_fw", true).toBool());
  ui->wizardEnable_ChkB->setChecked(settings.value("wizardEnable", true).toBool());
  ui->showSplash->setChecked(settings.value("show_splash", true).toBool());
  ui->historySize->setValue(settings.value("history_size", 10).toInt());
  ui->backLightColor->setCurrentIndex(settings.value("backLight", 0).toInt());
  ui->simuSW->setChecked(settings.value("simuSW", false).toBool());

  Path=settings.value("libraryPath", "").toString();
  if (QDir(Path).exists()) {
    ui->libraryPath->setText(Path);
  }
  Path=settings.value("gePath", "").toString();
  if (QFile(Path).exists()) {
    ui->ge_lineedit->setText(Path);
  }  
  Path=settings.value("backupPath", "").toString();
  if (!Path.isEmpty()) {
    if (QDir(Path).exists()) {
      ui->backupPath->setText(Path);
      ui->backupEnable->setEnabled(true);
      ui->backupEnable->setChecked(settings.value("backupEnable", true).toBool());
    } else {
      ui->backupEnable->setDisabled(true);
    }
  } else {
      ui->backupEnable->setDisabled(true);
  }
  ui->splashincludeCB->setCurrentIndex(settings.value("embedded_splashes", 0).toInt());

#ifdef JOYSTICKS
  ui->joystickChkB->setChecked(settings.value("js_support", false).toBool());
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
    ui->joystickCB->setCurrentIndex(settings.value("js_ctrl", 0).toInt());
  }
  else {
    ui->joystickCB->clear();
    ui->joystickCB->setDisabled(true);
    ui->joystickcalButton->setDisabled(true);
  }
#endif  
//  Profile Tab Inits  
  ui->channelorderCB->setCurrentIndex(settings.value("default_channel_order", 0).toInt());
  ui->stickmodeCB->setCurrentIndex(settings.value("default_mode", 1).toInt());
  ui->renameFirmware->setChecked(settings.value("rename_firmware_files", false).toBool());
  Path=settings.value("sdPath", "").toString();
  if (QDir(Path).exists()) {
    ui->sdPath->setText(Path);
  }
  ui->profileIndexLE->setText(settings.value("profileId", "").toString());
  ui->profileNameLE->setText(settings.value("Name", "").toString());

  QString fileName=settings.value("SplashFileName","").toString();
  if (!fileName.isEmpty()) {
    QFile file(fileName);
    if (file.exists()){ 
      ui->SplashFileName->setText(fileName);
      displayImage( fileName );
    }
  }
  ui->firmwareLE->setText(settings.value("firmware","").toString());
}

void appPreferencesDialog::on_libraryPathButton_clicked()
{
  QSettings settings;
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your library folder"), settings.value("libraryPath").toString());
  if (!fileName.isEmpty()) {
    settings.setValue("libraryPath", fileName);
    ui->libraryPath->setText(fileName);
  }
}

void appPreferencesDialog::on_snapshotClipboardCKB_clicked()
{
  QSettings settings;
  if (ui->snapshotClipboardCKB->isChecked()) {
    ui->snapshotPath->setDisabled(true);
    ui->snapshotPathButton->setDisabled(true);
    settings.setValue("snapshot_to_clipboard", true);
  } else {
    ui->snapshotPath->setEnabled(true);
    ui->snapshotPath->setReadOnly(true);
    ui->snapshotPathButton->setEnabled(true);
    settings.setValue("snapshot_to_clipboard", false);
  }
}

void appPreferencesDialog::on_backupPathButton_clicked()
{
  QSettings settings;
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your Models and Settings backup folder"), settings.value("backupPath").toString());
  if (!fileName.isEmpty()) {
    settings.setValue("backupPath", fileName);
    ui->backupPath->setText(fileName);
  }
  ui->backupEnable->setEnabled(true);
}

void appPreferencesDialog::on_ge_pathButton_clicked()
{
  QSettings settings;
  QString fileName = QFileDialog::getOpenFileName(this, tr("Select Google Earth executable"),ui->ge_lineedit->text());
  if (!fileName.isEmpty()) {
    ui->ge_lineedit->setText(fileName);
  }
}
 
#ifdef JOYSTICKS
void appPreferencesDialog::on_joystickChkB_clicked() {
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

void appPreferencesDialog::on_joystickcalButton_clicked() {
   joystickDialog * jd=new joystickDialog(this, ui->joystickCB->currentIndex());
   jd->exec();
}
#endif

// ******** Profile tab functions

void appPreferencesDialog::on_sdPathButton_clicked()
{
  QSettings settings;
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select the folder replicating your SD structure"), settings.value("sdPath").toString());
  if (!fileName.isEmpty()) {
    ui->sdPath->setText(fileName);
  }
}

void appPreferencesDialog::saveProfile()
{
  QSettings settings;

  QString profile=QString("profile") + settings.value("profileId").toString();
  QString name=ui->profileNameLE->text();
  if (name.isEmpty()) {
    name = profile;
    ui->profileNameLE->setText(name);
  }
  settings.beginGroup("Profiles");
  settings.beginGroup(profile);
  settings.setValue("Name",name);
  settings.setValue("default_channel_order", ui->channelorderCB->currentIndex());
  settings.setValue("default_mode", ui->stickmodeCB->currentIndex());
  settings.setValue("burnFirmware", ui->burnFirmware->isChecked());
  settings.setValue("rename_firmware_files", ui->renameFirmware->isChecked());
  settings.setValue("sdPath", ui->sdPath->text());
  settings.setValue("SplashFileName", ui->SplashFileName->text());
  settings.setValue("firmware", ui->firmwareLE->text());
  settings.endGroup();
  settings.endGroup();
}

void appPreferencesDialog::loadProfileString(QString profile, QString label)
{
  QSettings settings;
  QString value;

  settings.beginGroup("Profiles");
  settings.beginGroup(profile);
  value = settings.value(label).toString();
  settings.endGroup();
  settings.endGroup();

  settings.setValue( label, value ); 
}

void appPreferencesDialog::loadProfile()
{
  QSettings settings;
  QString profile=QString("profile") + settings.value("profileId").toString();

  loadProfileString( profile, "Name" );
  loadProfileString( profile, "default_channel_order" );
  loadProfileString( profile, "default_mode" );
  loadProfileString( profile, "burnFirmware" );
  loadProfileString( profile, "rename_firmware_files" );
  loadProfileString( profile, "sdPath" );
  loadProfileString( profile, "SplashFileName" );
  loadProfileString( profile, "firmware" );
}

void appPreferencesDialog::on_removeProfileButton_clicked()
{
  QSettings settings;
  QString profileId = settings.value("profileId").toString(); 
  if ( profileId == "1" )
     QMessageBox::information(this, tr("Not possible to remove profile"), tr("The default profile can not be removed."));
  else
  {
    QString profile=QString("profile") + profileId;
    settings.beginGroup("Profiles");
    settings.remove(profile);
    settings.endGroup();
    settings.setValue("profileId", "1");

    loadProfile();
    initSettings();
  }
}

bool appPreferencesDialog::displayImage( QString fileName )
{
  QImage image(fileName);
  if (image.isNull()) 
  {
    QMessageBox::critical(this, tr("Error"), tr("Cannot load %1.").arg(fileName));
    return false;
  }

  int width=ui->imageLabel->width();
  ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(width, 64)));
  if (width==212) {
    image=image.convertToFormat(QImage::Format_RGB32);
    QRgb col;
    int gray, height = image.height();
    for (int i = 0; i < width; ++i) {
      for (int j = 0; j < height; ++j) {
        col = image.pixel(i, j);
        gray = qGray(col);
        image.setPixel(i, j, qRgb(gray, gray, gray));
      }
    }      
    ui->imageLabel->setPixmap(QPixmap::fromImage(image));
  } 
  else {
    ui->imageLabel->setPixmap(QPixmap::fromImage(image.convertToFormat(QImage::Format_Mono)));
  }
  return true;
}

void appPreferencesDialog::on_SplashSelect_clicked()
{
  QString supportedImageFormats;
  for (int formatIndex = 0; formatIndex < QImageReader::supportedImageFormats().count(); formatIndex++) {
    supportedImageFormats += QLatin1String(" *.") + QImageReader::supportedImageFormats()[formatIndex];
  }

  QSettings settings;
  QString fileName = QFileDialog::getOpenFileName(this,
          tr("Open Image to load"), settings.value("lastImagesDir").toString(), tr("Images (%1)").arg(supportedImageFormats));

  if (!fileName.isEmpty()) {
    settings.setValue("lastImagesDir", QFileInfo(fileName).dir().absolutePath());
    
    if (displayImage(fileName))
      ui->SplashFileName->setText(fileName);
  }
}

void appPreferencesDialog::on_clearImageButton_clicked() {
  ui->imageLabel->clear();
  ui->SplashFileName->clear();
}



