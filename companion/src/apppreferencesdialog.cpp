#include "apppreferencesdialog.h"
#include "ui_apppreferencesdialog.h"
#include "mainwindow.h"
#include "appdata.h"
#include "helpers.h"
#include "flashinterface.h"
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
  glob.startup_check_companion(ui->startupCheck_companion9x->isChecked());
  glob.startup_check_fw(ui->startupCheck_fw->isChecked());
  glob.wizardEnable(ui->wizardEnable_ChkB->isChecked());
  glob.show_splash(ui->showSplash->isChecked());
  glob.simuSW(ui->simuSW->isChecked());
  glob.history_size(ui->historySize->value());
  glob.backLight(ui->backLightColor->currentIndex());
  glob.libraryPath(ui->libraryPath->text());
  glob.gePath(ui->ge_lineedit->text());
  glob.embedded_splashes(ui->splashincludeCB->currentIndex());
  glob.backupEnable(ui->backupEnable->isChecked());

  if (ui->joystickChkB ->isChecked() && ui->joystickCB->isEnabled()) {
    glob.js_support(ui->joystickChkB ->isChecked());  
    glob.js_ctrl(ui->joystickCB ->currentIndex());
  }
  else {
    glob.js_support(false);
    glob.js_ctrl(0);
  }

  glob.profileId(ui->profileIndexLE->text().toInt());

  glob.pro[glob.profileId()].default_channel_order(ui->channelorderCB->currentIndex());
  glob.pro[glob.profileId()].default_mode(ui->stickmodeCB->currentIndex());
  glob.pro[glob.profileId()].rename_firmware_files(ui->renameFirmware->isChecked());
  glob.pro[glob.profileId()].burnFirmware(ui->burnFirmware->isChecked());
  glob.pro[glob.profileId()].Name(ui->profileNameLE->text());
  glob.pro[glob.profileId()].sdPath(ui->sdPath->text());
  glob.pro[glob.profileId()].SplashFileName(ui->SplashFileName->text());
  glob.pro[glob.profileId()].firmware(ui->firmwareLE->text());
  
  saveProfile();
}

void appPreferencesDialog::on_snapshotPathButton_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your snapshot folder"), glob.snapshotpath());
  if (!fileName.isEmpty()) {
    glob.snapshotpath(fileName);
    glob.snapshot_to_clipboard(false);
    ui->snapshotPath->setText(fileName);
  }
}

void appPreferencesDialog::initSettings()
{
  ui->snapshotClipboardCKB->setChecked(glob.snapshot_to_clipboard());
  ui->burnFirmware->setChecked(glob.pro[glob.profileId()].burnFirmware());
  ui->snapshotPath->setText(glob.snapshotpath());
  ui->snapshotPath->setReadOnly(true);
  if (ui->snapshotClipboardCKB->isChecked())
  {
    ui->snapshotPath->setDisabled(true);
    ui->snapshotPathButton->setDisabled(true);
  }
  ui->startupCheck_companion9x->setChecked(glob.startup_check_companion());
  ui->startupCheck_fw->setChecked(glob.startup_check_fw());
  ui->wizardEnable_ChkB->setChecked(glob.wizardEnable());
  ui->showSplash->setChecked(glob.show_splash());
  ui->historySize->setValue(glob.history_size());
  ui->backLightColor->setCurrentIndex(glob.backLight());
  ui->simuSW->setChecked(glob.simuSW());
  ui->libraryPath->setText(glob.libraryPath());
  ui->ge_lineedit->setText(glob.gePath());

  if (!glob.backupPath().isEmpty()) {
    if (QDir(glob.backupPath()).exists()) {
      ui->backupPath->setText(glob.backupPath());
      ui->backupEnable->setEnabled(true);
      ui->backupEnable->setChecked(glob.backupEnable());
    } else {
      ui->backupEnable->setDisabled(true);
    }
  } else {
      ui->backupEnable->setDisabled(true);
  }
  ui->splashincludeCB->setCurrentIndex(glob.embedded_splashes());

#ifdef JOYSTICKS
  ui->joystickChkB->setChecked(glob.js_support());
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
    ui->joystickCB->setCurrentIndex(glob.js_ctrl());
  }
  else {
    ui->joystickCB->clear();
    ui->joystickCB->setDisabled(true);
    ui->joystickcalButton->setDisabled(true);
  }
#endif  
//  Profile Tab Inits  
  ui->channelorderCB->setCurrentIndex(glob.pro[glob.profileId()].default_channel_order());
  ui->stickmodeCB->setCurrentIndex(glob.pro[glob.profileId()].default_mode());
  ui->renameFirmware->setChecked(glob.pro[glob.profileId()].rename_firmware_files());
  ui->sdPath->setText(glob.pro[glob.profileId()].sdPath());
  ui->profileIndexLE->setText(QString("%1").arg(glob.profileId()));
  ui->profileNameLE->setText(glob.pro[glob.profileId()].Name());
  ui->firmwareLE->setText(glob.pro[glob.profileId()].firmware());
  ui->SplashFileName->setText(glob.pro[glob.profileId()].SplashFileName());
  displayImage( glob.pro[glob.profileId()].SplashFileName() );
}

void appPreferencesDialog::on_libraryPathButton_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your library folder"), glob.libraryPath());
  if (!fileName.isEmpty()) {
    glob.libraryPath(fileName);
    ui->libraryPath->setText(fileName);
  }
}

void appPreferencesDialog::on_snapshotClipboardCKB_clicked()
{
  if (ui->snapshotClipboardCKB->isChecked()) {
    ui->snapshotPath->setDisabled(true);
    ui->snapshotPathButton->setDisabled(true);
    glob.snapshot_to_clipboard(true);
  } else {
    ui->snapshotPath->setEnabled(true);
    ui->snapshotPath->setReadOnly(true);
    ui->snapshotPathButton->setEnabled(true);
    glob.snapshot_to_clipboard(false);
  }
}

void appPreferencesDialog::on_backupPathButton_clicked()
{
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select your Models and Settings backup folder"), glob.backupPath());
  if (!fileName.isEmpty()) {
    glob.backupPath(fileName);
    ui->backupPath->setText(fileName);
  }
  ui->backupEnable->setEnabled(true);
}

void appPreferencesDialog::on_ge_pathButton_clicked()
{
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
  QString fileName = QFileDialog::getExistingDirectory(this,tr("Select the folder replicating your SD structure"), glob.pro[glob.profileId()].sdPath());
  if (!fileName.isEmpty()) {
    ui->sdPath->setText(fileName);
  }
}

void appPreferencesDialog::saveProfile()
{
  // The profile name may NEVER be empty
  if (ui->profileNameLE->text().isEmpty())
    ui->profileNameLE->setText("----");

  glob.pro[glob.profileId()].Name( ui->profileNameLE->text() );
  glob.pro[glob.profileId()].default_channel_order( ui->channelorderCB->currentIndex());
  glob.pro[glob.profileId()].default_mode( ui->stickmodeCB->currentIndex());
  glob.pro[glob.profileId()].burnFirmware( ui->burnFirmware->isChecked());
  glob.pro[glob.profileId()].rename_firmware_files( ui->renameFirmware->isChecked());
  glob.pro[glob.profileId()].sdPath( ui->sdPath->text());
  glob.pro[glob.profileId()].SplashFileName( ui->SplashFileName->text());
  glob.pro[glob.profileId()].firmware( ui->firmwareLE->text());
}

void appPreferencesDialog::on_removeProfileButton_clicked()
{
  if ( glob.profileId() == 0 )
     QMessageBox::information(this, tr("Not possible to remove profile"), tr("The default profile can not be removed."));
  else
  {
    glob.pro[glob.profileId()].remove();
    glob.profileId( 0 );
    initSettings();
  }
}

bool appPreferencesDialog::displayImage( QString fileName )
{
  // Start by clearing the pixmap
  ui->imageLabel->setPixmap(QPixmap());

  QImage image(fileName);
  if (image.isNull()) 
    return false;

  // Use the firmware name to determine splash width
  int width = SPLASH_WIDTH;
  if (glob.pro[glob.profileId()].firmware().contains("taranis"))
    width = SPLASHX9D_WIDTH;

  ui->imageLabel->setPixmap(QPixmap::fromImage(image.scaled(width, SPLASH_HEIGHT)));
  if (width==SPLASHX9D_WIDTH) {
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
  if (width == SPLASH_WIDTH)
      ui->imageLabel->setFixedSize(SPLASH_WIDTH, SPLASH_HEIGHT);
  else
     ui->imageLabel->setFixedSize(SPLASHX9D_WIDTH, SPLASHX9D_HEIGHT);

  return true;
}

void appPreferencesDialog::on_SplashSelect_clicked()
{
  QString supportedImageFormats;
  for (int formatIndex = 0; formatIndex < QImageReader::supportedImageFormats().count(); formatIndex++) {
    supportedImageFormats += QLatin1String(" *.") + QImageReader::supportedImageFormats()[formatIndex];
  }

  QString fileName = QFileDialog::getOpenFileName(this,
          tr("Open Image to load"), glob.lastImagesDir(), tr("Images (%1)").arg(supportedImageFormats));

  if (!fileName.isEmpty()){
    glob.lastImagesDir(QFileInfo(fileName).dir().absolutePath());
   
    displayImage(fileName);
    ui->SplashFileName->setText(fileName);
  }
}

void appPreferencesDialog::on_clearImageButton_clicked() {
  ui->imageLabel->clear();
  ui->SplashFileName->clear();
}



