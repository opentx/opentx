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

#ifndef _TELEMETRY_H_
#define _TELEMETRY_H_

#include "modeledit.h"
#include "eeprominterface.h"

class AutoComboBox;
class TimerEdit;

namespace Ui {
  class TelemetryAnalog;
  class TelemetryCustomScreen;
  class TelemetrySensor;
  class Telemetry;
}

class TelemetryAnalog: public ModelPanel
{
    Q_OBJECT

    friend class TelemetryPanel;

  public:
    TelemetryAnalog(QWidget *parent, FrSkyChannelData & analog, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~TelemetryAnalog();

  signals:
    void modified();

  private slots:
    void on_UnitCB_currentIndexChanged(int index);
    void on_RatioSB_editingFinished();
    void on_RatioSB_valueChanged();
    void on_CalibSB_editingFinished();
    void on_alarm1LevelCB_currentIndexChanged(int index);
    void on_alarm1GreaterCB_currentIndexChanged(int index);
    void on_alarm1ValueSB_editingFinished();
    void on_alarm2LevelCB_currentIndexChanged(int index);
    void on_alarm2GreaterCB_currentIndexChanged(int index);
    void on_alarm2ValueSB_editingFinished();

  private:
    Ui::TelemetryAnalog *ui;
    FrSkyChannelData & analog;
    bool lock;

    void update();
};

class TelemetryCustomScreen: public ModelPanel
{
    Q_OBJECT

  public:
    TelemetryCustomScreen(QWidget *parent, ModelData & model, FrSkyScreenData & screen, GeneralSettings & generalSettings, Firmware * firmware);
    ~TelemetryCustomScreen();
    void update();

  private slots:
    void on_screenType_currentIndexChanged(int index);
    void scriptNameEdited();
    void customFieldChanged(int index);
    void barSourceChanged(int index);
    void barMinChanged(double value);
    void barMaxChanged(double value);
    void barTimeChanged();

  protected:
    void populateTelemetrySourceCB(QComboBox * b, RawSource & source, bool last=false);

  private:
    void updateBar(int line);
    Ui::TelemetryCustomScreen * ui;
    FrSkyScreenData & screen;
    QComboBox * fieldsCB[4][3];
    QComboBox * barsCB[4];
    QDoubleSpinBox * minSB[4];
    QDoubleSpinBox * maxSB[4];
    TimerEdit * minTime[4];
    TimerEdit * maxTime[4];
};

class TelemetrySensorPanel: public ModelPanel
{
    Q_OBJECT

  public:
    TelemetrySensorPanel(QWidget *parent, SensorData & sensor, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware);
    ~TelemetrySensorPanel();
    void update();

  signals:
    void dataModified();

  protected slots:
    void on_name_editingFinished();
    void on_type_currentIndexChanged(int index);
    void on_formula_currentIndexChanged(int index);
    void on_unit_currentIndexChanged(int index);
    void on_prec_valueChanged(double value);

  protected:
    void updateSourcesComboBox(AutoComboBox * cb, bool negative);

  private:
    Ui::TelemetrySensor * ui;
    SensorData & sensor;
    bool lock;
};

class TelemetryPanel : public ModelPanel
{
    Q_OBJECT

  public:
    TelemetryPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~TelemetryPanel();
    virtual void update();

  private slots:
    void on_telemetryProtocol_currentIndexChanged(int index);
    void onModified();
    void on_frskyProtoCB_currentIndexChanged(int index);
    void on_bladesCount_editingFinished();
    void on_rssiAlarmWarningCB_currentIndexChanged(int index);
    void on_rssiAlarmCriticalCB_currentIndexChanged(int index);
    void on_rssiAlarmWarningSB_editingFinished();
    void on_rssiAlarmCriticalSB_editingFinished();
    void on_varioLimitMin_DSB_editingFinished();
    void on_varioLimitMax_DSB_editingFinished();
    void on_varioLimitCenterMin_DSB_editingFinished();
    void on_varioLimitCenterMax_DSB_editingFinished();
    void on_fasOffset_DSB_editingFinished();
    void on_mahCount_SB_editingFinished();
    void on_mahCount_ChkB_toggled(bool checked);

  private:
    Ui::Telemetry *ui;
    TelemetryAnalog * analogs[4];
    TelemetryCustomScreen * telemetryCustomScreens[4];
    TelemetrySensorPanel * sensorPanels[CPN_MAX_SENSORS];

    void setup();
    void telBarUpdate();
    void populateVoltsSource();
    void populateCurrentSource();
    void populateVarioSource();
};

#endif // _TELEMETRY_H_
