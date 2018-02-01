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

#ifndef _CHANNELS_H_
#define _CHANNELS_H_

#include "helpers.h"
#include "modeledit.h"

#include <QtCore>

class GVarGroup;

class LimitsGroup
{
  Q_DECLARE_TR_FUNCTIONS(LimitsGroup)

  public:
    LimitsGroup(Firmware * firmware, TableLayout * tableLayout, int row, int col, int & value, const ModelData & model, int min, int max, int deflt, ModelPanel * panel=NULL);
    ~LimitsGroup();

    void setValue(int val);
    void updateMinMax(int max);

  protected:
    Firmware *firmware;
    QDoubleSpinBox *spinbox;
    GVarGroup *gvarGroup;
    int &value;
    double displayStep;
};

class Channels : public ModelPanel
{
    Q_OBJECT

  public:
    Channels(QWidget * parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware);
    ~Channels();

  public slots:
    void refreshExtendedLimits();

  private slots:
    void symlimitsEdited();
    void nameEdited();
    void invEdited();
    void curveEdited();
    void ppmcenterEdited();
    void update();
    void updateLine(int index);
    void chnDelete();
    void chnCopy();
    void chnPaste();
    void chnCut();
    void chn_customContextMenuRequested(QPoint pos);

  private:
    QLineEdit *name[CPN_MAX_CHNOUT];
    LimitsGroup *chnOffset[CPN_MAX_CHNOUT];
    LimitsGroup *chnMin[CPN_MAX_CHNOUT];
    LimitsGroup *chnMax[CPN_MAX_CHNOUT];
    QComboBox *invCB[CPN_MAX_CHNOUT];
    QComboBox *curveCB[CPN_MAX_CHNOUT];
    QSpinBox *centerSB[CPN_MAX_CHNOUT];
    QCheckBox *symlimitsChk[CPN_MAX_CHNOUT];
    int selectedChannel;
};

#endif // _CHANNELS_H_
