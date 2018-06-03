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

#ifndef TELEMETRYDATA_H
#define TELEMETRYDATA_H

#include "rawsource.h"

#include <QtCore>

class FrSkyAlarmData {
  Q_DECLARE_TR_FUNCTIONS(FrSkyAlarmData)

  public:
    FrSkyAlarmData() { clear(); }
    unsigned int   level;               // 0=none, 1=Yellow, 2=Orange, 3=Red
    unsigned int   greater;             // 0=LT(<), 1=GT(>)
    unsigned int value;               // 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.

    void clear() { memset(this, 0, sizeof(FrSkyAlarmData)); }

    static QString alarmLevelName(int alarm)
    {
      switch (alarm) {
        case 1:
          return tr("Yellow");
        case 2:
          return tr("Orange");
        case 3:
          return tr("Red");
        default:
          return "----";
      }
    }
};

class FrSkyChannelData {
  Q_DECLARE_TR_FUNCTIONS(FrSkyChannelData)

  public:
    FrSkyChannelData() { clear(); }
    unsigned int ratio;                // 0.0 means not used, 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
    unsigned int type;                 // future use: 0=volts, 1=ml...
    int   offset;
    unsigned int multiplier;
    FrSkyAlarmData alarms[2];

    float getRatio() const;
    RawSourceRange getRange() const;
    void clear() { memset(this, 0, sizeof(FrSkyChannelData)); }

    static QString unitString(int units)
    {
      switch(units) {
        case 1:
          return QCoreApplication::translate("FrSkyChannelData", "---");
        default:
          return QCoreApplication::translate("FrSkyChannelData", "V");
      }
    }
};

struct FrSkyBarData {
  RawSource source;
  int barMin;           // minimum for bar display
  int barMax;           // ditto for max display (would usually = ratio)
};

struct FrSkyLineData {
  RawSource source[3];
};

struct TelemetryScriptData {
  char filename[8+1];
};

enum TelemetryScreenEnum {
  TELEMETRY_SCREEN_NONE,
  TELEMETRY_SCREEN_NUMBERS,
  TELEMETRY_SCREEN_BARS,
  TELEMETRY_SCREEN_SCRIPT
};

class FrSkyScreenData {
  public:
    FrSkyScreenData() { clear(); }

    typedef struct {
      FrSkyBarData bars[4];
      FrSkyLineData lines[4];
      TelemetryScriptData script;
    } FrSkyScreenBody;

    unsigned int type;
    FrSkyScreenBody body;

    void clear();
};

enum TelemetryVarioSources {
  TELEMETRY_VARIO_SOURCE_ALTI,
  TELEMETRY_VARIO_SOURCE_ALTI_PLUS,
  TELEMETRY_VARIO_SOURCE_VSPEED,
  TELEMETRY_VARIO_SOURCE_A1,
  TELEMETRY_VARIO_SOURCE_A2,
  TELEMETRY_VARIO_SOURCE_DTE,
};

enum TelemetryVoltsSources {
  TELEMETRY_VOLTS_SOURCE_A1,
  TELEMETRY_VOLTS_SOURCE_A2,
  TELEMETRY_VOLTS_SOURCE_A3,
  TELEMETRY_VOLTS_SOURCE_A4,
  TELEMETRY_VOLTS_SOURCE_FAS,
  TELEMETRY_VOLTS_SOURCE_CELLS
};

enum TelemetryCurrentSources {
  TELEMETRY_CURRENT_SOURCE_NONE,
  TELEMETRY_CURRENT_SOURCE_A1,
  TELEMETRY_CURRENT_SOURCE_A2,
  TELEMETRY_CURRENT_SOURCE_A3,
  TELEMETRY_CURRENT_SOURCE_A4,
  TELEMETRY_CURRENT_SOURCE_FAS
};

class FrSkyData {
  public:
    FrSkyData() { clear(); }
    FrSkyChannelData channels[4];
    unsigned int usrProto;
    int blades;
    unsigned int voltsSource;
    unsigned int altitudeSource;
    unsigned int currentSource;
    FrSkyScreenData screens[4];
    unsigned int varioSource;
    bool varioCenterSilent;
    int varioMin;
    int varioCenterMin;    // if increment in 0.2m/s = 3.0m/s max
    int varioCenterMax;
    int varioMax;
    bool mAhPersistent;
    unsigned int storedMah;
    int fasOffset;
    bool ignoreSensorIds;

    void clear();
};

class MavlinkData {
  public:
    MavlinkData() { clear();}
    unsigned int rc_rssi_scale;
    unsigned int pc_rssi_en;
    void clear() { memset(this, 0, sizeof(MavlinkData)); }
};

#endif // TELEMETRYDATA_H
