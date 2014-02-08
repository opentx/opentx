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
  setWindowIcon(CompanionIcon("preferences.png"));
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

  MainWindow * mw = (MainWindow *)this->parent();
  mw->unloadProfile();
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

