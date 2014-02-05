#include "apppreferencesdialog.h"
#include "ui_apppreferencesdialog.h"
#include "mainwindow.h"
#include "helpers.h"
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
//  resize(0,0);
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
  settings.setValue("history_size", ui->historySize->value());
  settings.setValue("gePath", ui->ge_lineedit->text());
  settings.setValue("backupEnable", ui->backupEnable->isChecked());
  MainWindow * mw = (MainWindow *)this->parent();
  mw->unloadProfile();
}

void appPreferencesDialog::initSettings()
{
  QSettings settings;
  ui->startupCheck_companion9x->setChecked(settings.value("startup_check_companion", true).toBool());
  ui->startupCheck_fw->setChecked(settings.value("startup_check_fw", true).toBool());
  ui->wizardEnable_ChkB->setChecked(settings.value("wizardEnable", true).toBool());
  ui->showSplash->setChecked(settings.value("show_splash", true).toBool());
  ui->historySize->setValue(settings.value("history_size", 10).toInt());

  QString Path=settings.value("gePath", "").toString();
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
 

