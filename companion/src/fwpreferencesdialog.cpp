#include "fwpreferencesdialog.h"
#include "ui_fwpreferencesdialog.h"
#include "mainwindow.h"
#include "eeprominterface.h"
#include "helpers.h"
#include "appdata.h"
#include <QDesktopServices>
#include <QtGui>

fwPreferencesDialog::fwPreferencesDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::fwPreferencesDialog)
{
  ui->setupUi(this);
  setWindowIcon(CompanionIcon("fwpreferences.png"));
  initSettings();
}

fwPreferencesDialog::~fwPreferencesDialog()
{
  delete ui;
}

void fwPreferencesDialog::initSettings()
{
  ui->fwTypeLbl->setText(g.profile[g.id()].fwType());
  int revision = g.fwRev.get(g.profile[g.id()].fwType());
  if (revision > 0)
    ui->lastRevisionLbl->setText(QString("%1").arg(revision));
}

void fwPreferencesDialog::on_checkFWUpdates_clicked()
{
  MainWindow * mw = (MainWindow *)this->parent();
  QString fwType = g.profile[g.id()].fwType();
  mw->checkForUpdates(true, fwType);
  initSettings();
}

void fwPreferencesDialog::on_fw_dnld_clicked()
{
  MainWindow * mw = (MainWindow *)this->parent();
  mw->downloadLatestFW(current_firmware_variant.firmware, g.profile[g.id()].fwType());
  initSettings();
}

void fwPreferencesDialog::on_voice_dnld_clicked()
{
  QString url="http://fw.opentx.it/voices/";
  QString fwType = g.profile[g.id()].fwType();
  QStringList list = fwType.split("-");
  QString firmware = QString("%1-%2").arg(list[0]).arg(list[1]);
  url.append(QString("%1/%2/").arg(firmware).arg(ui->voiceCombo->currentText()));
  QDesktopServices::openUrl(url);
}


