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

#include "hardware.h"
#include "ui_hardware.h"

void HardwarePanel::setupSwitchConfig(int index, QLabel *label, AutoLineEdit *name, AutoComboBox *type, bool threePos = true)
{
  if (IS_STM32(firmware->getBoard()) && index < firmware->getCapability(Switches)) {
    type->addItem(tr("None"), GeneralSettings::SWITCH_NONE);
    type->addItem(tr("2 Positions Toggle"), GeneralSettings::SWITCH_TOGGLE);
    type->addItem(tr("2 Positions"), GeneralSettings::SWITCH_2POS);
    if (threePos)
      type->addItem(tr("3 Positions"), GeneralSettings::SWITCH_3POS);
    name->setField(generalSettings.switchName[index], 3, this);
    type->setField(generalSettings.switchConfig[index], this);
  }
  else {
    label->hide();
    name->hide();
    type->hide();
  }
}

void HardwarePanel::setupPotConfig(int index, QLabel *label, AutoLineEdit *name, AutoComboBox *type)
{
  if (IS_STM32(firmware->getBoard()) && index < firmware->getCapability(Pots)) {
    label->setText(RawSource(SOURCE_TYPE_STICK, CPN_MAX_STICKS+index).toString());
    type->addItem(tr("None"), GeneralSettings::POT_NONE);
    type->addItem(tr("Pot with detent"), GeneralSettings::POT_WITH_DETENT);
    type->addItem(tr("Multipos switch"), GeneralSettings::POT_MULTIPOS_SWITCH);
    type->addItem(tr("Pot without detent"), GeneralSettings::POT_WITHOUT_DETENT);
    name->setField(generalSettings.potName[index], 3, this);
    type->setField(generalSettings.potConfig[index], this);
  }
  else {
    label->hide();
    name->hide();
    type->hide();
  }
}

void HardwarePanel::setupSliderConfig(int index, QLabel *label, AutoLineEdit *name, AutoComboBox *type)
{
  if (IS_STM32(firmware->getBoard()) && index < firmware->getCapability(Sliders)) {
    label->setText(RawSource(SOURCE_TYPE_STICK, CPN_MAX_STICKS+firmware->getCapability(Pots)+index).toString());
    type->addItem(tr("None"), GeneralSettings::SLIDER_NONE);
    type->addItem(tr("Slider with detent"), GeneralSettings::SLIDER_WITH_DETENT);
    name->setField(generalSettings.sliderName[index], 3, this);
    type->setField(generalSettings.sliderConfig[index], this);
  }
  else {
    label->hide();
    name->hide();
    type->hide();
  }
}

HardwarePanel::HardwarePanel(QWidget * parent, GeneralSettings & generalSettings, Firmware * firmware):
  GeneralPanel(parent, generalSettings, firmware),
  ui(new Ui::Hardware)
{
  ui->setupUi(this);

  if (IS_STM32(firmware->getBoard())) {
    ui->rudName->setField(generalSettings.stickName[0], 3, this);
    ui->eleName->setField(generalSettings.stickName[1], 3, this);
    ui->thrName->setField(generalSettings.stickName[2], 3, this);
    ui->ailName->setField(generalSettings.stickName[3], 3, this);
  }
  else {
    ui->rudLabel->hide();
    ui->rudName->hide();
    ui->eleLabel->hide();
    ui->eleName->hide();
    ui->thrLabel->hide();
    ui->thrName->hide();
    ui->ailLabel->hide();
    ui->ailName->hide();
  }

  setupPotConfig(0, ui->pot1Label, ui->pot1Name, ui->pot1Type);
  setupPotConfig(1, ui->pot2Label, ui->pot2Name, ui->pot2Type);
  setupPotConfig(2, ui->pot3Label, ui->pot3Name, ui->pot3Type);
  setupPotConfig(3, ui->pot4Label, ui->pot4Name, ui->pot4Type);

  setupSliderConfig(0, ui->lsLabel, ui->lsName, ui->lsType);
  setupSliderConfig(1, ui->rsLabel, ui->rsName, ui->rsType);
  setupSliderConfig(2, ui->ls2Label, ui->ls2Name, ui->ls2Type);
  setupSliderConfig(3, ui->rs2Label, ui->rs2Name, ui->rs2Type);

  setupSwitchConfig(0, ui->saLabel, ui->saName, ui->saType);
  setupSwitchConfig(1, ui->sbLabel, ui->sbName, ui->sbType);
  setupSwitchConfig(2, ui->scLabel, ui->scName, ui->scType);
  setupSwitchConfig(3, ui->sdLabel, ui->sdName, ui->sdType);
  setupSwitchConfig(4, ui->seLabel, ui->seName, ui->seType);
  setupSwitchConfig(5, ui->sfLabel, ui->sfName, ui->sfType, false);   //switch does not support 3POS
  setupSwitchConfig(6, ui->sgLabel, ui->sgName, ui->sgType);
  setupSwitchConfig(7, ui->shLabel, ui->shName, ui->shType, false);   //switch does not support 3POS
  setupSwitchConfig(8, ui->siLabel, ui->siName, ui->siType);
  setupSwitchConfig(9, ui->sjLabel, ui->sjName, ui->sjType);
  setupSwitchConfig(10, ui->skLabel, ui->skName, ui->skType);
  setupSwitchConfig(11, ui->slLabel, ui->slName, ui->slType);
  setupSwitchConfig(12, ui->smLabel, ui->smName, ui->smType);
  setupSwitchConfig(13, ui->snLabel, ui->snName, ui->snType);
  setupSwitchConfig(14, ui->soLabel, ui->soName, ui->soType);
  setupSwitchConfig(15, ui->spLabel, ui->spName, ui->spType);
  setupSwitchConfig(16, ui->sqLabel, ui->sqName, ui->sqType);
  setupSwitchConfig(17, ui->srLabel, ui->srName, ui->srType);

  if (IS_TARANIS(firmware->getBoard())) {
    ui->serialPortMode->setCurrentIndex(generalSettings.hw_uartMode);
  }
  else {
    ui->serialPortMode->hide();
    ui->serialPortLabel->hide();
  }

  if (!IS_SKY9X(firmware->getBoard())) {
    ui->txCurrentCalibration->hide();
    ui->txCurrentCalibrationLabel->hide();
  }

  if (IS_TARANIS_X9E(firmware->getBoard())) {
    ui->bluetoothEnable->setChecked(generalSettings.bluetoothEnable);
    ui->bluetoothName->setField(generalSettings.bluetoothName, 10, this);
  }
  else {
    ui->bluetoothLabel->hide();
    ui->bluetoothEnable->hide();
    ui->bluetoothName->hide();
  }

  disableMouseScrolling();

  setValues();
}

HardwarePanel::~HardwarePanel()
{
  delete ui;
}

void HardwarePanel::on_PPM_MultiplierDSB_editingFinished()
{
  generalSettings.PPM_Multiplier = (int)(ui->PPM_MultiplierDSB->value()*10)-10;
  emit modified();
}

void HardwarePanel::on_PPM1_editingFinished()
{
  generalSettings.trainer.calib[0] = ui->PPM1->value();
  emit modified();
}

void HardwarePanel::on_PPM2_editingFinished()
{
  generalSettings.trainer.calib[1] = ui->PPM2->value();
  emit modified();
}

void HardwarePanel::on_PPM3_editingFinished()
{
  generalSettings.trainer.calib[2] = ui->PPM3->value();
  emit modified();
}

void HardwarePanel::on_PPM4_editingFinished()
{
  generalSettings.trainer.calib[3] = ui->PPM4->value();
  emit modified();
}


void HardwarePanel::on_txCurrentHardware_editingFinished()
{
  generalSettings.txCurrentCalibration = ui->txCurrentCalibration->value();
  emit modified();
}

void HardwarePanel::on_bluetoothEnable_stateChanged(int)
{
  generalSettings.bluetoothEnable = ui->bluetoothEnable->isChecked();
  emit modified();
}

void HardwarePanel::setValues()
{
  ui->txVoltageCalibration->setValue((double)generalSettings.txVoltageCalibration/10);
  ui->txCurrentCalibration->setValue((double)generalSettings.txCurrentCalibration);

  ui->PPM1->setValue(generalSettings.trainer.calib[0]);
  ui->PPM2->setValue(generalSettings.trainer.calib[1]);
  ui->PPM3->setValue(generalSettings.trainer.calib[2]);
  ui->PPM4->setValue(generalSettings.trainer.calib[3]);
  ui->PPM_MultiplierDSB->setValue((qreal)(generalSettings.PPM_Multiplier+10)/10);
}

void HardwarePanel::on_txVoltageHardware_editingFinished()
{
  generalSettings.txVoltageCalibration = ui->txVoltageCalibration->value()*10;
  emit modified();
}

void HardwarePanel::on_serialPortMode_currentIndexChanged(int index)
{
  generalSettings.hw_uartMode = index;
  emit modified();
}
