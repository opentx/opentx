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

#include "trainer.h"
#include "ui_trainer.h"

TrainerPanel::TrainerPanel(QWidget * parent, GeneralSettings & generalSettings, Firmware * firmware):
  GeneralPanel(parent, generalSettings, firmware),
  ui(new Ui::Trainer)
{
  ui->setupUi(this);


  ui->trnMode_1->setCurrentIndex(generalSettings.trainer.mix[0].mode);
  ui->trnChn_1->setCurrentIndex(generalSettings.trainer.mix[0].src);
  ui->trnWeight_1->setValue(generalSettings.trainer.mix[0].weight);
  ui->trnMode_2->setCurrentIndex(generalSettings.trainer.mix[1].mode);
  ui->trnChn_2->setCurrentIndex(generalSettings.trainer.mix[1].src);
  ui->trnWeight_2->setValue(generalSettings.trainer.mix[1].weight);
  ui->trnMode_3->setCurrentIndex(generalSettings.trainer.mix[2].mode);
  ui->trnChn_3->setCurrentIndex(generalSettings.trainer.mix[2].src);
  ui->trnWeight_3->setValue(generalSettings.trainer.mix[2].weight);
  ui->trnMode_4->setCurrentIndex(generalSettings.trainer.mix[3].mode);
  ui->trnChn_4->setCurrentIndex(generalSettings.trainer.mix[3].src);
  ui->trnWeight_4->setValue(generalSettings.trainer.mix[3].weight);

}

TrainerPanel::~TrainerPanel()
{
  delete ui;
}

void TrainerPanel::on_trnMode_1_currentIndexChanged(int index)
{
  generalSettings.trainer.mix[0].mode = index;
  emit modified();
}

void TrainerPanel::on_trnChn_1_currentIndexChanged(int index)
{
  generalSettings.trainer.mix[0].src = index;
  emit modified();
}

void TrainerPanel::on_trnWeight_1_editingFinished()
{
  generalSettings.trainer.mix[0].weight = ui->trnWeight_1->value();
  emit modified();
}

void TrainerPanel::on_trnMode_2_currentIndexChanged(int index)
{
  generalSettings.trainer.mix[1].mode = index;
  emit modified();
}

void TrainerPanel::on_trnChn_2_currentIndexChanged(int index)
{
  generalSettings.trainer.mix[1].src = index;
  emit modified();
}

void TrainerPanel::on_trnWeight_2_editingFinished()
{
  generalSettings.trainer.mix[1].weight = ui->trnWeight_2->value();
  emit modified();
}

void TrainerPanel::on_trnMode_3_currentIndexChanged(int index)
{
  generalSettings.trainer.mix[2].mode = index;
  emit modified();
}

void TrainerPanel::on_trnChn_3_currentIndexChanged(int index)
{
  generalSettings.trainer.mix[2].src = index;
  emit modified();
}

void TrainerPanel::on_trnWeight_3_editingFinished()
{
  generalSettings.trainer.mix[2].weight = ui->trnWeight_3->value();
  emit modified();
}

void TrainerPanel::on_trnMode_4_currentIndexChanged(int index)
{
  generalSettings.trainer.mix[3].mode = index;
  emit modified();
}

void TrainerPanel::on_trnChn_4_currentIndexChanged(int index)
{
  generalSettings.trainer.mix[3].src = index;
  emit modified();
}

void TrainerPanel::on_trnWeight_4_editingFinished()
{
  generalSettings.trainer.mix[3].weight = ui->trnWeight_4->value();
  emit modified();
}
