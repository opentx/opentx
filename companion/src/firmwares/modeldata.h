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

#ifndef MODELDATA_H
#define MODELDATA_H

#include "constants.h"
#include "customfunctiondata.h"
#include "gvardata.h"
#include "io_data.h"
#include "logicalswitchdata.h"
#include "moduledata.h"
#include "sensordata.h"
#include "telem_data.h"

#include <QtCore>

class GeneralSettings;
class RadioDataConversionState;

#define CHAR_FOR_NAMES " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-."
#define CHAR_FOR_NAMES_REGEX "[ A-Za-z0-9_.-,]*"

class RSSIAlarmData {
  public:
    RSSIAlarmData() { clear(); }
    unsigned int level[2];  // AVR Only
    int warning;
    int critical;
    bool disabled;
    void clear() {
      this->level[0] = 2;
      this->level[1] = 3;
      this->warning = 45;
      this->critical = 42;
      this->disabled = false;
    }
};

#define TIMER_NAME_LEN 8

class TimerData {
  Q_DECLARE_TR_FUNCTIONS(TimerData)

  public:
    enum CountDownMode {
      COUNTDOWN_SILENT,
      COUNTDOWN_BEEPS,
      COUNTDOWN_VOICE,
      COUNTDOWN_HAPTIC
    };
    TimerData() { clear(); }
    RawSwitch    mode;
    char         name[TIMER_NAME_LEN+1];
    bool         minuteBeep;
    unsigned int countdownBeep;
    unsigned int val;
    unsigned int persistent;
    int          pvalue;
    void clear() { memset(this, 0, sizeof(TimerData)); mode = RawSwitch(SWITCH_TYPE_TIMER_MODE, 0); }
    void convert(RadioDataConversionState & cstate);
};

#define CPN_MAX_SCRIPTS       9
#define CPN_MAX_SCRIPT_INPUTS 10
class ScriptData {
  public:
    ScriptData() { clear(); }
    char    filename[10+1];
    char    name[10+1];
    int     inputs[CPN_MAX_SCRIPT_INPUTS];
    void clear() { memset(this, 0, sizeof(ScriptData)); }
};

/*
 * TODO ...
 */
#if 0
class CustomScreenOptionData {
  public:

};

class CustomScreenZoneData {
  public:
    char widgetName[10+1];
    WidgetOptionData widgetOptions[5];
};

class CustomScreenData {
  public:
    CustomScreenData();

    char layoutName[10+1];
    CustomScreenZoneData zones[];
    CustomScreenOptionData options[];
};
#else
typedef char CustomScreenData[610+1];
typedef char TopbarData[216+1];
#endif

enum TrainerMode {
  TRAINER_MODE_MASTER_TRAINER_JACK,
  TRAINER_MODE_SLAVE,
  TRAINER_MODE_MASTER_SBUS_EXTERNAL_MODULE,
  TRAINER_MODE_MASTER_CPPM_EXTERNAL_MODULE,
  TRAINER_MODE_MASTER_BATTERY_COMPARTMENT,
};

class ModelData {
  Q_DECLARE_TR_FUNCTIONS(ModelData)

  public:
    ModelData();
    ModelData(const ModelData & src);
    ModelData & operator = (const ModelData & src);

    void convert(RadioDataConversionState & cstate);

    ExpoData * insertInput(const int idx);
    void removeInput(const int idx, bool clearName = true);

    bool isInputValid(const unsigned int idx) const;
    bool hasExpos(uint8_t inputIdx) const;
    bool hasMixes(uint8_t output) const;

    QVector<const ExpoData *> expos(int input) const;
    QVector<const MixData *> mixes(int channel) const;

    bool      used;
    int       category;
    char      name[15+1];
    char      filename[16+1];
    int       modelIndex;      // Companion only, temporary index position managed by data model.

    TimerData timers[CPN_MAX_TIMERS];
    bool      noGlobalFunctions;
    bool      thrTrim;            // Enable Throttle Trim
    int       trimInc;            // Trim Increments
    unsigned int trimsDisplay;
    bool      disableThrottleWarning;

    unsigned int beepANACenter;      // 1<<0->A1.. 1<<6->A7

    bool      extendedLimits; // TODO xml
    bool      extendedTrims;
    bool      throttleReversed;
    FlightModeData flightModeData[CPN_MAX_FLIGHT_MODES];
    MixData   mixData[CPN_MAX_MIXERS];
    LimitData limitData[CPN_MAX_CHNOUT];

    char      inputNames[CPN_MAX_INPUTS][4+1];
    ExpoData  expoData[CPN_MAX_EXPOS];

    CurveData curves[CPN_MAX_CURVES];
    LogicalSwitchData  logicalSw[CPN_MAX_LOGICAL_SWITCHES];
    CustomFunctionData customFn[CPN_MAX_SPECIAL_FUNCTIONS];
    SwashRingData swashRingData;
    unsigned int thrTraceSrc;
    uint64_t switchWarningStates;
    unsigned int switchWarningEnable;
    unsigned int potsWarningMode;
    bool potsWarningEnabled[CPN_MAX_POTS];
    int          potPosition[CPN_MAX_POTS];
    bool         displayChecklist;
    GVarData gvarData[CPN_MAX_GVARS];
    MavlinkData mavlink;
    unsigned int telemetryProtocol;
    FrSkyData frsky;
    RSSIAlarmData rssiAlarms;

    char bitmap[10+1];

    unsigned int trainerMode;  // TrainerMode

    ModuleData moduleData[CPN_MAX_MODULES+1/*trainer*/];

    ScriptData scriptData[CPN_MAX_SCRIPTS];

    SensorData sensorData[CPN_MAX_SENSORS];

    unsigned int toplcdTimer;

    CustomScreenData customScreenData[5];

    TopbarData topbarData;

    void clear();
    bool isEmpty() const;
    void setDefaultInputs(const GeneralSettings & settings);
    void setDefaultMixes(const GeneralSettings & settings);
    void setDefaultValues(unsigned int id, const GeneralSettings & settings);

    int getTrimValue(int phaseIdx, int trimIdx);
    void setTrimValue(int phaseIdx, int trimIdx, int value);

    bool isGVarLinked(int phaseIdx, int gvarIdx);
    int getGVarFieldValue(int phaseIdx, int gvarIdx);
    float getGVarFieldValuePrec(int phaseIdx, int gvarIdx);

    ModelData removeGlobalVars();

    void clearMixes();
    void clearInputs();

    int getChannelsMax(bool forceExtendedLimits=false) const;

    bool isAvailable(const RawSwitch & swtch) const;

  protected:
    void removeGlobalVar(int & var);
};

#endif // MODELDATA_H
