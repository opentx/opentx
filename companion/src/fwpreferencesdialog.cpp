/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "fwpreferencesdialog.h"
#include "ui_fwpreferencesdialog.h"
#include "mainwindow.h"
#include "helpers.h"
#include "appdata.h"

const char * const OPENTX_SDCARD_DOWNLOAD_URL[] = {
  "https://downloads.open-tx.org/2.2/release/sdcard/",
  "https://downloads.open-tx.org/2.2/rc/sdcard/",
  "https://downloads.open-tx.org/2.2/nightlies/sdcard/"
};

FirmwarePreferencesDialog::FirmwarePreferencesDialog(QWidget * parent) :
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
  int version = g.fwRev.get(g.profile[g.id()].fwType());
  if (version > 0) {
    ui->lastRevisionLbl->setText(index2version(version));
  }
}

void FirmwarePreferencesDialog::on_checkFWUpdates_clicked()
{
  MainWindow * mw = qobject_cast<MainWindow *>(this->parent());
  mw->checkForFirmwareUpdate();
  initSettings();
}

void FirmwarePreferencesDialog::on_fw_dnld_clicked()
{
  MainWindow * mw = qobject_cast<MainWindow *>(this->parent());
  mw->dowloadLastFirmwareUpdate();
  initSettings();
}

void FirmwarePreferencesDialog::on_sd_dnld_clicked()
{
  QString url = OPENTX_SDCARD_DOWNLOAD_URL[g.boundedOpenTxBranch()];
  QString fwType = g.profile[g.id()].fwType();
  QStringList list = fwType.split("-");
  QString firmware = QString("%1-%2").arg(list[0]).arg(list[1]);
  if (g.boundedOpenTxBranch() != BRANCH_NIGHTLY_UNSTABLE) {
    url.append(QString("%1/").arg(firmware));
  }
  QDesktopServices::openUrl(url);
}
