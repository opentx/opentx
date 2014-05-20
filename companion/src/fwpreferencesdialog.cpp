#include "fwpreferencesdialog.h"
#include "ui_fwpreferencesdialog.h"
#include "mainwindow.h"
#include "eeprominterface.h"
#include "helpers.h"
#include "appdata.h"
#include <QDesktopServices>
#include <QtGui>

FirmwarePreferencesDialog::FirmwarePreferencesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::FirmwarePreferencesDialog)
{
  ui->setupUi(this);
  setWindowIcon(CompanionIcon("fwpreferences.png"));
  initSettings();
}

FirmwarePreferencesDialog::~FirmwarePreferencesDialog()
{
  delete ui;
}

void FirmwarePreferencesDialog::initSettings()
{
  ui->fwTypeLbl->setText(g.profile[g.id()].fwType());
  QString revision = g.fwRev.getString(g.profile[g.id()].fwType());
  if (!revision.isEmpty()) {
    ui->lastRevisionLbl->setText(revision);
  }
}

void FirmwarePreferencesDialog::on_checkFWUpdates_clicked()
{
  MainWindow * mw = (MainWindow *)this->parent();
  QString fwType = g.profile[g.id()].fwType();
  mw->checkForUpdates(true, fwType);
  initSettings();
}

void FirmwarePreferencesDialog::on_fw_dnld_clicked()
{
  MainWindow * mw = (MainWindow *)this->parent();
  mw->downloadLatestFW(current_firmware_variant);
  initSettings();
}

void FirmwarePreferencesDialog::on_voice_dnld_clicked()
{
  QString url = "http://voices-20.open-tx.org/";
  QString fwType = g.profile[g.id()].fwType();
  QStringList list = fwType.split("-");
  QString firmware = QString("%1-%2").arg(list[0]).arg(list[1]);
  url.append(QString("%1/%2/").arg(firmware).arg(ui->voiceCombo->currentText()));
  QDesktopServices::openUrl(url);
}


