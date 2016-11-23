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

#ifndef _LOGICALSWITCHES_H_
#define _LOGICALSWITCHES_H_

#include "modeledit.h"
#include "eeprominterface.h"

class LogicalSwitchesPanel : public ModelPanel
{
    Q_OBJECT

  public:
    LogicalSwitchesPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~LogicalSwitchesPanel();

    virtual void update();

  private slots:
    void edited();
    void v1Edited(int value);
    void v2Edited(int value);
    void andEdited(int value);
    void durationEdited(double duration);
    void delayEdited(double delay);
    void csw_customContextMenuRequested(QPoint pos);
    void cswDelete();
    void cswCopy();
    void cswPaste();
    void cswCut();

  private:
    QComboBox * csw[CPN_MAX_CSW];
    QDoubleSpinBox  * cswitchValue[CPN_MAX_CSW];
    QDoubleSpinBox  * cswitchOffset[CPN_MAX_CSW];
    QDoubleSpinBox  * cswitchOffset2[CPN_MAX_CSW];
    QTimeEdit  * cswitchTOffset[CPN_MAX_CSW];
    QComboBox * cswitchAnd[CPN_MAX_CSW];
    QDoubleSpinBox  * cswitchDuration[CPN_MAX_CSW];
    QDoubleSpinBox  * cswitchDelay[CPN_MAX_CSW];
    QComboBox * cswitchSource1[CPN_MAX_CSW];
    QComboBox * cswitchSource2[CPN_MAX_CSW];
    void setSwitchWidgetVisibility(int i);
    int selectedSwitch;

    void populateCSWCB(QComboBox *b, int value);
    void populateAndSwitchCB(QComboBox *b, const RawSwitch & value);
    void updateLine(int index);
    void updateTimerParam(QDoubleSpinBox *sb, int timer, double minimum=0);

};

#endif // _LOGICALSWITCHES_H_
