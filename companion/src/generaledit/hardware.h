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

#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#include "generaledit.h"

namespace Ui {
  class Hardware;
}

class QLabel;
class AutoLineEdit;
class AutoComboBox;

class HardwarePanel : public GeneralPanel
{
    Q_OBJECT

  public:
    HardwarePanel(QWidget * parent, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~HardwarePanel();

  private slots:
    void on_PPM1_editingFinished();
    void on_PPM2_editingFinished();
    void on_PPM3_editingFinished();
    void on_PPM4_editingFinished();
    void on_PPM_MultiplierDSB_editingFinished();

    void on_txCurrentCalibration_editingFinished();
    void on_txVoltageCalibration_editingFinished();
    void on_filterEnable_stateChanged();

    void on_serialPortMode_currentIndexChanged(int index);

  protected:
    void setupPotType(int index, QLabel * label, AutoLineEdit * name, AutoComboBox * type);
    void setupSliderType(int index, QLabel * label, AutoLineEdit * name, AutoComboBox * type);
    void setupSwitchType(int index, QLabel * label, AutoLineEdit * name, AutoComboBox * type, bool threePos = true);

  private:
    Ui::Hardware *ui;

    void setValues();
};

#endif // _HARDWARE_H_
