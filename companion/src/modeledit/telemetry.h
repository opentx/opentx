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

constexpr char MIMETYPE_TELE_SENSOR[] = "application/x-companion-tele-sensor";

class AutoComboBox;
class RawSourceFilterItemModel;
class TimerEdit;

namespace Ui {
  class TelemetryCustomScreen;
  class TelemetrySensor;
  class Telemetry;
}

class TelemetryCustomScreen: public ModelPanel
{
    Q_OBJECT

  public:
    TelemetryCustomScreen(QWidget *parent, ModelData & model, FrSkyScreenData & screen, GeneralSettings & generalSettings, Firmware * firmware, RawSourceFilterItemModel * srcModel);
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
    TelemetrySensorPanel(QWidget *parent, SensorData & sensor, int sensorIndex, int sensorCapability, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware);
    ~TelemetrySensorPanel();
    void update();

  signals:
    void dataModified();
    void clearAllSensors();
    void insertSensor(int index);
    void deleteSensor(int index);
    void moveUpSensor(int index);
    void moveDownSensor(int index);


  protected slots:
    void on_name_editingFinished();
    void on_type_currentIndexChanged(int index);
    void on_formula_currentIndexChanged(int index);
    void on_unit_currentIndexChanged(int index);
    void on_prec_valueChanged();
    void on_customContextMenuRequested(QPoint pos);
    bool hasClipboardData(QByteArray * data = nullptr) const;
    void cmCopy();
    void cmCut();
    void cmPaste();
    void cmClear(bool prompt = true);
    void cmClearAll();
    void cmInsert();
    void cmDelete();
    void cmMoveUp();
    void cmMoveDown();

  protected:
    void updateSourcesComboBox(AutoComboBox * cb, bool negative);

  private:
    Ui::TelemetrySensor * ui;
    SensorData & sensor;
    bool lock = false;
    int sensorIndex = 0;
    int selectedIndex = 0;
    int sensorCapability;
    bool insertAllowed() const;
    bool moveDownAllowed() const;
    bool moveUpAllowed() const;
};

class TelemetryPanel : public ModelPanel
{
    Q_OBJECT

  public:
    TelemetryPanel(QWidget *parent, ModelData & model, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~TelemetryPanel();
    virtual void update();

  signals:
    void updated();

  private slots:
    void on_telemetryProtocol_currentIndexChanged(int index);
    void onModified();
    void on_frskyProtoCB_currentIndexChanged(int index);
    void on_bladesCount_editingFinished();
    void on_rssiAlarmWarningSB_editingFinished();
    void on_rssiAlarmCriticalSB_editingFinished();
    void on_varioLimitMin_DSB_editingFinished();
    void on_varioLimitMax_DSB_editingFinished();
    void on_varioLimitCenterMin_DSB_editingFinished();
    void on_varioLimitCenterMax_DSB_editingFinished();
    void on_fasOffset_DSB_editingFinished();
    void on_mahCount_SB_editingFinished();
    void on_mahCount_ChkB_toggled(bool checked);
    void on_clearAllSensors();
    void on_insertSensor(int index);
    void on_deleteSensor(int index);
    void on_moveUpSensor(int index);
    void on_moveDownSensor(int index);

  private:
    Ui::Telemetry *ui;
    TelemetryCustomScreen * telemetryCustomScreens[4];
    TelemetrySensorPanel * sensorPanels[CPN_MAX_SENSORS];
    int sensorCapability;

    void setup();
    void telBarUpdate();
    void populateVoltsSource();
    void populateCurrentSource();
    void populateVarioSource();
    void swapData(int idx1, int idx2);
};

#endif // _TELEMETRY_H_
