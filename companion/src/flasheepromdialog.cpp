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

#include "flasheepromdialog.h"
#include "ui_flasheepromdialog.h"
#include "eeprominterface.h"
#include "helpers.h"
#include "firmwareinterface.h"
#include "hexinterface.h"
#include "appdata.h"
#include "progressdialog.h"
#include "radiointerface.h"
#include "storage_eeprom.h"
#include "splashlibrarydialog.h"

FlashEEpromDialog::FlashEEpromDialog(QWidget *parent, const QString &filename):
QDialog(parent),
ui(new Ui::FlashEEpromDialog),
eepromFilename(filename),
radioData(new RadioData())
{
  ui->setupUi(this);
  ui->profileLabel->setText(tr("Current profile: %1").arg(g.profile[g.id()].name()));
  if (!filename.isEmpty()) {
    ui->eepromFilename->hide();
    ui->eepromLoad->hide();
  }
  QString backupPath = g.profile[g.id()].pBackupDir();
  if (backupPath.isEmpty()) {
    backupPath=g.backupDir();
    ui->backupBeforeWrite->setChecked(g.enableBackup() || g.profile[g.id()].penableBackup());
  }
  if (backupPath.isEmpty() || !QDir(backupPath).exists()) {
    ui->backupBeforeWrite->setEnabled(false);
  }
  updateUI();
}

FlashEEpromDialog::~FlashEEpromDialog()
{
  delete ui;
}

void FlashEEpromDialog::updateUI()
{
  // ui->burnButton->setEnabled(true);
  ui->eepromFilename->setText(eepromFilename);
  if (getEEpromVersion(eepromFilename) >= 0) {
    ui->profileLabel->show();
    ui->patchCalibration->show();
    ui->patchHardwareSettings->show();
    QString name = g.profile[g.id()].name();
    QString calib = g.profile[g.id()].stickPotCalib();
    QString trainercalib = g.profile[g.id()].trainerCalib();
    QString DisplaySet = g.profile[g.id()].display();
    QString BeeperSet = g.profile[g.id()].beeper();
    QString HapticSet = g.profile[g.id()].haptic();
    QString SpeakerSet = g.profile[g.id()].speaker();
    if (!name.isEmpty()) {
      ui->profileLabel->show();
      ui->patchCalibration->show();
      ui->patchHardwareSettings->show();
      // TODO I hardcode the number of pots here, should be dependant on the board?
      if (!((calib.length()==(CPN_MAX_STICKS+3)*12) && (trainercalib.length()==16))) {
        ui->patchCalibration->setDisabled(true);
      }
      if (!((DisplaySet.length()==6) && (BeeperSet.length()==4) && (HapticSet.length()==6) && (SpeakerSet.length()==6))) {
        ui->patchHardwareSettings->setDisabled(true);
      }
    }
    else {
      ui->profileLabel->hide();
    }
    ui->burnButton->setEnabled(true);
  }
  else {
    ui->profileLabel->hide();
    ui->patchCalibration->hide();
    ui->patchHardwareSettings->hide();
  }
  QTimer::singleShot(0, this, SLOT(shrink()));
}

void FlashEEpromDialog::on_eepromLoad_clicked()
{
  QString filename = QFileDialog::getOpenFileName(this, tr("Choose Radio Backup file"), g.eepromDir(), tr(EXTERNAL_EEPROM_FILES_FILTER));
  if (!filename.isEmpty()) {
    eepromFilename = filename;
    updateUI();
  }
}

int FlashEEpromDialog::getEEpromVersion(const QString &filename)
{
  int result = -1;
  int eeprom_size = 0;

  if (filename.isEmpty()) {
    return -1;
  }

  QFile file(filename);
  if (!file.exists()) {
    QMessageBox::warning(this, tr("Error"), tr("Unable to find file %1!").arg(filename));
    return -1;
  }

  QByteArray eeprom(EESIZE_MAX, 0);
  int fileType = getFileType(filename);

#if 0
  if (fileType==FILE_TYPE_XML) {
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QMessageBox::critical(this, tr("Error"),tr("Error opening file %1:\n%2.").arg(filename).arg(file.errorString()));
      return -1;
    }
    QTextStream inputStream(&file);
    XmlInterface(inputStream).load(testData);
  }
  else
#endif
  if (fileType==FILE_TYPE_HEX || fileType==FILE_TYPE_EEPE) {
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"),tr("Error opening file %1:\n%2.").arg(filename).arg(file.errorString()));
        return -1;
    }
    QDomDocument doc(ER9X_EEPROM_FILE_TYPE);
    bool xmlOK = doc.setContent(&file);
    if (xmlOK) {
      RadioData * radioData = new RadioData();
      std::bitset<NUM_ERRORS> errors((unsigned long long)LoadEepromXml(*radioData, doc));
      if (!errors.test(ALL_OK)) {
        QMessageBox::warning(this, tr("Error"), tr("Invalid Models and Settings File %1").arg(filename));
      }
      else {
        result = radioData->generalSettings.version;
      }
      delete radioData;
      return result;
    }
    file.reset();

    QTextStream inputStream(&file);
    if (fileType==FILE_TYPE_EEPE) {  // read EEPE file header
      QString hline = inputStream.readLine();
      if (hline!=EEPE_EEPROM_FILE_HEADER) {
        QMessageBox::warning(this, tr("Error"), tr("Invalid Models and Settings File %1").arg(filename));
        return -1;
      }
    }
    eeprom_size = HexInterface(inputStream).load((uint8_t *)eeprom.data(), EESIZE_MAX);
  }
  else if (fileType==FILE_TYPE_BIN) { //read binary
    eeprom_size = file.size();
    if (!file.open(QFile::ReadOnly)) {  //reading binary file   - TODO HEX support
      QMessageBox::warning(this, tr("Error"), tr("Error opening file %1:\n%2.").arg(filename).arg(file.errorString()));
      return -1;
    }
    int len = file.read(eeprom.data(), eeprom_size);
    if (len != eeprom_size) {
      QMessageBox::warning(this, tr("Error"), tr("Error reading file %1:\n%2.").arg(filename).arg(file.errorString()));
      return -1;
    }
  }

  RadioData * radioData = new RadioData();
  std::bitset<NUM_ERRORS> errors((unsigned long long)LoadEeprom(*radioData, (const uint8_t *)eeprom.data(), eeprom_size));
  if (eeprom_size == 0 || !errors.test(ALL_OK)) {
    QMessageBox::warning(this, tr("Error"), tr("Invalid Models and Settings file %1").arg(filename));
  }
  else {
    result = radioData->generalSettings.version;
  }
  delete radioData;
  return result;
}

bool FlashEEpromDialog::patchCalibration()
{
  QString calib = g.profile[g.id()].stickPotCalib();
  QString trainercalib = g.profile[g.id()].trainerCalib();
  int potsnum=GetCurrentFirmware()->getCapability(Pots);
  int8_t txVoltageCalibration=(int8_t) g.profile[g.id()].txVoltageCalibration();
  int8_t txCurrentCalibration=(int8_t) g.profile[g.id()].txCurrentCalibration();
  int8_t PPM_Multiplier=(int8_t) g.profile[g.id()].ppmMultiplier();

  if ((calib.length()==(CPN_MAX_STICKS+potsnum)*12) && (trainercalib.length()==16)) {
    QString Byte;
    int16_t byte16;
    bool ok;
    for (int i=0; i<(CPN_MAX_STICKS+potsnum); i++) {
      Byte=calib.mid(i*12,4);
      byte16=(int16_t)Byte.toInt(&ok,16);
      if (ok)
        radioData->generalSettings.calibMid[i]=byte16;
      Byte=calib.mid(4+i*12,4);
      byte16=(int16_t)Byte.toInt(&ok,16);
      if (ok)
        radioData->generalSettings.calibSpanNeg[i]=byte16;
      Byte=calib.mid(8+i*12,4);
      byte16=(int16_t)Byte.toInt(&ok,16);
      if (ok)
        radioData->generalSettings.calibSpanPos[i]=byte16;
    }
    for (int i=0; i<4; i++) {
      Byte = trainercalib.mid(i*4,4);
      byte16 = (int16_t)Byte.toInt(&ok,16);
      if (ok) {
        radioData->generalSettings.trainer.calib[i] = byte16;
      }
    }
    radioData->generalSettings.txCurrentCalibration = txCurrentCalibration;
    radioData->generalSettings.txVoltageCalibration = txVoltageCalibration;
    radioData->generalSettings.PPM_Multiplier = PPM_Multiplier;
    return true;
  }
  else {
    QMessageBox::critical(this, tr("Warning"), tr("Wrong radio calibration data in profile, Settings not patched"));
    return false;
  }
}

bool FlashEEpromDialog::patchHardwareSettings()
{
  QString DisplaySet = g.profile[g.id()].display();
  QString BeeperSet = g.profile[g.id()].beeper();
  QString HapticSet = g.profile[g.id()].haptic();
  QString SpeakerSet = g.profile[g.id()].speaker();
  uint8_t GSStickMode=(uint8_t) g.profile[g.id()].gsStickMode();
  uint8_t vBatWarn=(uint8_t) g.profile[g.id()].vBatWarn();

  if ((DisplaySet.length()==6) && (BeeperSet.length()==4) && (HapticSet.length()==6) && (SpeakerSet.length()==6)) {
    radioData->generalSettings.vBatWarn = vBatWarn;
    radioData->generalSettings.stickMode = GSStickMode;
    uint8_t byte8u;
    int8_t byte8;
    bool ok;
    byte8=(int8_t)DisplaySet.mid(0,2).toInt(&ok,16);
    if (ok) radioData->generalSettings.optrexDisplay=(byte8==1 ? true : false);
    byte8u=(uint8_t)DisplaySet.mid(2,2).toUInt(&ok,16);
    if (ok) radioData->generalSettings.contrast=byte8u;
    byte8u=(uint8_t)DisplaySet.mid(4,2).toUInt(&ok,16);
    if (ok) radioData->generalSettings.backlightBright=byte8u;
    byte8u=(uint8_t)BeeperSet.mid(0,2).toUInt(&ok,16);
    if (ok) radioData->generalSettings.beeperMode=(GeneralSettings::BeeperMode)byte8u;
    byte8=(int8_t)BeeperSet.mid(2,2).toInt(&ok,16);
    if (ok) radioData->generalSettings.beeperLength=byte8;
    byte8u=(uint8_t)HapticSet.mid(0,2).toUInt(&ok,16);
    if (ok) radioData->generalSettings.hapticMode=(GeneralSettings::BeeperMode)byte8u;
    byte8u=(uint8_t)HapticSet.mid(2,2).toUInt(&ok,16);
    if (ok) radioData->generalSettings.hapticStrength=byte8u;
    byte8=(int8_t)HapticSet.mid(4,2).toInt(&ok,16);
    if (ok) radioData->generalSettings.hapticLength=byte8;
    byte8u=(uint8_t)SpeakerSet.mid(0,2).toUInt(&ok,16);
    if (ok) radioData->generalSettings.speakerMode=byte8u;
    byte8u=(uint8_t)SpeakerSet.mid(2,2).toUInt(&ok,16);
    if (ok) radioData->generalSettings.speakerPitch=byte8u;
    byte8u=(uint8_t)SpeakerSet.mid(4,2).toUInt(&ok,16);
    if (ok) radioData->generalSettings.speakerVolume=byte8u;
    return true;
  }
  else {
    QMessageBox::critical(this, tr("Warning"), tr("Wrong radio setting data in profile, Settings not patched"));
    return false;
  }
}

void FlashEEpromDialog::on_burnButton_clicked()
{
  // patch the eeprom if needed
  bool patch = false;
  if (ui->patchCalibration->isChecked()) {
    patch |= patchCalibration();
  }
  if (ui->patchHardwareSettings->isChecked()) {
    patch |= patchHardwareSettings();
  }
  QString filename = eepromFilename;
  if (patch) {
    QString filename = generateProcessUniqueTempFileName("temp.bin");
    QFile file(filename);
    uint8_t *eeprom = (uint8_t*)malloc(GetEepromInterface()->getEEpromSize());
    int eeprom_size = GetEepromInterface()->save(eeprom, *radioData, 0, GetCurrentFirmware()->getVariantNumber());
    if (!eeprom_size) {
      QMessageBox::warning(this, tr("Error"), tr("Cannot write file %1:\n%2.").arg(filename).arg(file.errorString()));
      return;
    }
    if (!file.open(QIODevice::WriteOnly)) {
      QMessageBox::warning(this, tr("Error"), tr("Cannot write file %1:\n%2.").arg(filename).arg(file.errorString()));
      return;
    }
    QTextStream outputStream(&file);
    long result = file.write((char*)eeprom, eeprom_size);
    if (result != eeprom_size) {
      QMessageBox::warning(this, tr("Error"), tr("Error writing file %1:\n%2.").arg(filename).arg(file.errorString()));
      return;
    }
  }

  close();

  ProgressDialog progressDialog(this, tr("Write Models and Settings to Radio"), CompanionIcon("write_eeprom.png"));

  // backup previous EEPROM if requested
  QString backupFilename;
  QString backupPath;
  if (ui->backupBeforeWrite->isChecked()) {
    backupPath = g.profile[g.id()].pBackupDir();
    if (backupPath.isEmpty()) {
      backupPath=g.backupDir();
    }
    backupFilename = backupPath + "/backup-" + QDateTime().currentDateTime().toString("yyyy-MM-dd-HHmmss") + ".bin";
  }
  else if (ui->checkFirmwareCompatibility->isChecked()) {
    backupFilename = generateProcessUniqueTempFileName("eeprom.bin");
  }
  if (!backupFilename.isEmpty()) {
    if (!readEeprom(backupFilename, progressDialog.progress())) {
      return;
    }
  }

  // check EEPROM compatibility if requested
  if (ui->checkFirmwareCompatibility->isChecked()) {
    int eepromVersion = getEEpromVersion(filename);
    QString firmwareFilename = generateProcessUniqueTempFileName("flash.bin");
    if (!readFirmware(firmwareFilename, progressDialog.progress()))
      return;
    QString compatEEprom = generateProcessUniqueTempFileName("compat.bin");
    if (convertEEprom(filename, compatEEprom, firmwareFilename)) {
      int compatVersion = getEEpromVersion(compatEEprom);
      if ((compatVersion / 100) != (eepromVersion / 100)) {
        QMessageBox::warning(this, tr("Warning"), tr("The radio firmware belongs to another product family, check file and preferences!"));
        return;
      }
      else if (compatVersion < eepromVersion) {
        QMessageBox::warning(this, tr("Warning"), tr("The radio firmware is outdated, please upgrade!"));
        return;
      }
      filename = compatEEprom;
    }
    else if (QMessageBox::question(this, "Error", tr("Cannot check Models and Settings compatibility! Continue anyway?"), QMessageBox::Yes|QMessageBox::No) == QMessageBox::No) {
      return;
    }
    qunlink(firmwareFilename);
  }

  // and write...
  writeEeprom(filename, progressDialog.progress());

  progressDialog.exec();
}

void FlashEEpromDialog::on_cancelButton_clicked()
{
  close();
}

void FlashEEpromDialog::shrink()
{
  resize(0, 0);
}

