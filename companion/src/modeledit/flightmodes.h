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

#ifndef _FLIGHTMODES_H_
#define _FLIGHTMODES_H_

#include "modeledit.h"
#include "eeprominterface.h"

namespace Ui {
  class FlightMode;
}

class FlightModePanel : public ModelPanel
{
    Q_OBJECT

  public:
    FlightModePanel(QWidget *parent, ModelData &model, int modeIdx, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~FlightModePanel();

    virtual void update();

  signals:
    void nameModified();

  private slots:
    void phaseName_editingFinished();
    void phaseSwitch_currentIndexChanged(int index);
    void phaseFadeIn_editingFinished();
    void phaseFadeOut_editingFinished();
    void phaseTrimUse_currentIndexChanged(int index);
    void phaseTrim_valueChanged();
    void phaseTrimSlider_valueChanged();
    void GVName_editingFinished();
    void phaseGVValue_editingFinished();
    void phaseGVUse_currentIndexChanged(int index);
    void phaseGVPopupToggled(bool checked);
    void phaseREValue_editingFinished();
    void phaseREUse_currentIndexChanged(int index);
    void name_customContextMenuRequested(const QPoint & pos);
    void fmClear();

  private:
    Ui::FlightMode *ui;
    int phaseIdx;
    FlightModeData & phase;
    int reCount;
    int gvCount;
    QVector<QLabel *> trimsLabel;
    QLineEdit * gvNames[CPN_MAX_GVARS];
    QSpinBox * gvValues[CPN_MAX_GVARS];
    QCheckBox * gvPopups[CPN_MAX_GVARS];
    QComboBox * gvUse[CPN_MAX_GVARS];
    QSpinBox * reValues[CPN_MAX_ENCODERS];
    QComboBox * reUse[CPN_MAX_ENCODERS];
    QVector<QComboBox *> trimsUse;
    QVector<QSpinBox *> trimsValue;
    QVector<QSlider *> trimsSlider;

    void trimUpdate(unsigned int trim);

};

class FlightModesPanel : public ModelPanel
{
    Q_OBJECT

  public:
    FlightModesPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~FlightModesPanel();

    virtual void update();

  private slots:
    void onPhaseModified();
    void onPhaseNameChanged();
    void on_tabWidget_currentChanged(int index);

  private:
    int modesCount;
    QTabWidget *tabWidget;
    QString getTabName(int index);
    QVector<FlightModePanel *> panels;

};

#endif // _FLIGHTMODES_H_

