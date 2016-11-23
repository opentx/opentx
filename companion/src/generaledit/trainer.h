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

#ifndef _TRAINER_H_
#define _TRAINER_H_

#include "generaledit.h"
#include "eeprominterface.h"

namespace Ui {
  class Trainer;
}

class TrainerPanel : public GeneralPanel
{
    Q_OBJECT

  public:
    TrainerPanel(QWidget *parent, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~TrainerPanel();

  private slots:
    void on_trnMode_1_currentIndexChanged(int index);
    void on_trnChn_1_currentIndexChanged(int index);
    void on_trnWeight_1_editingFinished();
    void on_trnMode_2_currentIndexChanged(int index);
    void on_trnChn_2_currentIndexChanged(int index);
    void on_trnWeight_2_editingFinished();
    void on_trnMode_3_currentIndexChanged(int index);
    void on_trnChn_3_currentIndexChanged(int index);
    void on_trnWeight_3_editingFinished();
    void on_trnMode_4_currentIndexChanged(int index);
    void on_trnChn_4_currentIndexChanged(int index);
    void on_trnWeight_4_editingFinished();

  private:
    Ui::Trainer *ui;

    void setValues();
};

#endif // _TRAINER_H_
