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

#ifndef _EEPROMINTERFACE_H_
#define _EEPROMINTERFACE_H_

#include <string.h>
#include <QString>
#include <QStringList>
#include <QList>
#include <QtXml> // This should be removed from here, and remove Xml dependency from all libs which don't need it.
#include <QComboBox>
#include <iostream>
#include <bitset>
#include "constants.h"
#include "../../radio/src/definitions.h"
#include "simulatorinterface.h"

#define EESIZE_STOCK           2048
#define EESIZE_M128            4096
#define EESIZE_GRUVIN9X        4096
#define EESIZE_TARANIS         (32*1024)
#define EESIZE_SKY9X           (128*4096)
#define EESIZE_9XRPRO          (128*4096)
#define EESIZE_MAX             EESIZE_9XRPRO

#define FSIZE_STOCK            (64*1024)
#define FSIZE_M128             (128*1024)
#define FSIZE_GRUVIN9X         (256*1024)
#define FSIZE_TARANIS          (512*1024)
#define FSIZE_SKY9X            (256*1024)
#define FSIZE_9XRPRO           (512*1024)
#define FSIZE_HORUS            (2048*1024)
#define FSIZE_MAX              FSIZE_HORUS

#define IS_9X(board)           (board==BOARD_STOCK || board==BOARD_M128)
#define IS_STOCK(board)        (board==BOARD_STOCK)
#define IS_2560(board)         (board==BOARD_GRUVIN9X || board==BOARD_MEGA2560)
#define IS_SKY9X(board)        (board==BOARD_SKY9X || board==BOARD_9XRPRO || board==BOARD_AR9X)
#define IS_9XRPRO(board)       (board==BOARD_9XRPRO)
#define IS_TARANIS(board)      (board==BOARD_TARANIS_X9D  || board==BOARD_TARANIS_X9DP || board==BOARD_TARANIS_X9E || board==BOARD_TARANIS_X7)
#define IS_TARANIS_PLUS(board) (board==BOARD_TARANIS_X9DP || board==BOARD_TARANIS_X9E)
#define IS_TARANIS_X9E(board)  (board==BOARD_TARANIS_X9E)
#define IS_HORUS(board)        (board==BOARD_HORUS)
#define IS_FLAMENCO(board)     (board==BOARD_FLAMENCO)
#define IS_STM32(board)        (IS_TARANIS(board) || IS_HORUS(board) || IS_FLAMENCO(board))
#define IS_ARM(board)          (IS_STM32(board) || IS_SKY9X(board))
#define HAS_LARGE_LCD(board)   (IS_HORUS(board) || (IS_TARANIS(board) && board != BOARD_TARANIS_X7))

const uint8_t modn12x3[4][4]= {
  {1, 2, 3, 4},
  {1, 3, 2, 4},
  {4, 2, 3, 1},
  {4, 3, 2, 1} };

enum Switches {
  SWITCH_NONE,

  SWITCH_THR = SWITCH_NONE+1,
  SWITCH_RUD,
  SWITCH_ELE,
  SWITCH_ID0,
  SWITCH_ID1,
  SWITCH_ID2,
  SWITCH_AIL,
  SWITCH_GEA,
  SWITCH_TRN,

  SWITCH_SA0 = SWITCH_NONE+1,
  SWITCH_SA1,
  SWITCH_SA2,
  SWITCH_SB0,
  SWITCH_SB1,
  SWITCH_SB2,
  SWITCH_SC0,
  SWITCH_SC1,
  SWITCH_SC2,
  SWITCH_SD0,
  SWITCH_SD1,
  SWITCH_SD2,
  SWITCH_SE0,
  SWITCH_SE1,
  SWITCH_SE2,
  SWITCH_SF0,
  SWITCH_SF1,
  SWITCH_SG0,
  SWITCH_SG1,
  SWITCH_SG2,
  SWITCH_SH0,
  SWITCH_SH2,
  SWITCH_SI0,
  SWITCH_SI2,
  SWITCH_SJ0,
  SWITCH_SJ2,
  SWITCH_SK0,
  SWITCH_SK2,

};

enum TimerModes {
  TMRMODE_NONE,
  TMRMODE_ABS,
  TMRMODE_THR,
  TMRMODE_THR_REL,
  TMRMODE_THR_TRG,
  TMRMODE_FIRST_SWITCH
};

enum FailsafeModes {
  FAILSAFE_NOT_SET,
  FAILSAFE_HOLD,
  FAILSAFE_CUSTOM,
  FAILSAFE_NOPULSES,
  FAILSAFE_RECEIVER,
  FAILSAFE_LAST = FAILSAFE_RECEIVER
};

#define TRIM_LH_L  0
#define TRIM_LH_R  1
#define TRIM_LV_DN 2
#define TRIM_LV_UP 3
#define TRIM_RV_DN 4
#define TRIM_RV_UP 5
#define TRIM_RH_L  6
#define TRIM_RH_R  7
#define TRIM_T5_DN 8
#define TRIM_T5_UP 9
#define TRIM_T6_DN 10
#define TRIM_T6_UP 11
#define TRIM_NONE  12

#define CHAR_FOR_NAMES " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-."
#define CHAR_FOR_NAMES_REGEX "[ A-Za-z0-9_.-,]*"

enum HeliSwashTypes {
 HELI_SWASH_TYPE_NONE=0,
 HELI_SWASH_TYPE_120,
 HELI_SWASH_TYPE_120X,
 HELI_SWASH_TYPE_140,
 HELI_SWASH_TYPE_90
};

class ModelData;
class GeneralSettings;

enum TelemetrySource {
  TELEMETRY_SOURCE_TX_BATT,
  TELEMETRY_SOURCE_TX_TIME,
  TELEMETRY_SOURCE_TIMER1,
  TELEMETRY_SOURCE_TIMER2,
  TELEMETRY_SOURCE_TIMER3,
  TELEMETRY_SOURCE_SWR,
  TELEMETRY_SOURCE_RSSI_TX,
  TELEMETRY_SOURCE_RSSI_RX,
  TELEMETRY_SOURCE_A1,
  TELEMETRY_SOURCE_A2,
  TELEMETRY_SOURCE_A3,
  TELEMETRY_SOURCE_A4,
  TELEMETRY_SOURCE_ALT,
  TELEMETRY_SOURCE_RPM,
  TELEMETRY_SOURCE_FUEL,
  TELEMETRY_SOURCE_T1,
  TELEMETRY_SOURCE_T2,
  TELEMETRY_SOURCE_SPEED,
  TELEMETRY_SOURCE_DIST,
  TELEMETRY_SOURCE_GPS_ALT,
  TELEMETRY_SOURCE_CELL,
  TELEMETRY_SOURCE_CELLS_SUM,
  TELEMETRY_SOURCE_VFAS,
  TELEMETRY_SOURCE_CURRENT,
  TELEMETRY_SOURCE_CONSUMPTION,
  TELEMETRY_SOURCE_POWER,
  TELEMETRY_SOURCE_ACCX,
  TELEMETRY_SOURCE_ACCY,
  TELEMETRY_SOURCE_ACCZ,
  TELEMETRY_SOURCE_HDG,
  TELEMETRY_SOURCE_VERTICAL_SPEED,
  TELEMETRY_SOURCE_ASPEED,
  TELEMETRY_SOURCE_DTE,
  TELEMETRY_SOURCE_A1_MIN,
  TELEMETRY_SOURCE_A2_MIN,
  TELEMETRY_SOURCE_A3_MIN,
  TELEMETRY_SOURCE_A4_MIN,
  TELEMETRY_SOURCE_ALT_MIN,
  TELEMETRY_SOURCE_ALT_MAX,
  TELEMETRY_SOURCE_RPM_MAX,
  TELEMETRY_SOURCE_T1_MAX,
  TELEMETRY_SOURCE_T2_MAX,
  TELEMETRY_SOURCE_SPEED_MAX,
  TELEMETRY_SOURCE_DIST_MAX,
  TELEMETRY_SOURCE_ASPEED_MAX,
  TELEMETRY_SOURCE_CELL_MIN,
  TELEMETRY_SOURCE_CELLS_MIN,
  TELEMETRY_SOURCE_VFAS_MIN,
  TELEMETRY_SOURCE_CURRENT_MAX,
  TELEMETRY_SOURCE_POWER_MAX,
  TELEMETRY_SOURCE_ACC,
  TELEMETRY_SOURCE_GPS_TIME,
  TELEMETRY_SOURCES_STATUS_COUNT = TELEMETRY_SOURCE_GPS_TIME+1,
  TELEMETRY_SOURCES_DISPLAY_COUNT = TELEMETRY_SOURCE_POWER_MAX+1,
  TELEMETRY_SOURCES_COUNT = TELEMETRY_SOURCE_POWER_MAX+1,
  TELEMETRY_SOURCE_RESERVE = -1
};

#define TM_HASTELEMETRY     0x01
#define TM_HASOFFSET        0x02
#define TM_HASWSHH          0x04

enum RawSourceType {
  SOURCE_TYPE_NONE,
  SOURCE_TYPE_VIRTUAL_INPUT,
  SOURCE_TYPE_LUA_OUTPUT,
  SOURCE_TYPE_STICK, // and POTS
  SOURCE_TYPE_ROTARY_ENCODER,
  SOURCE_TYPE_TRIM,
  SOURCE_TYPE_MAX,
  SOURCE_TYPE_SWITCH,
  SOURCE_TYPE_CUSTOM_SWITCH,
  SOURCE_TYPE_CYC,
  SOURCE_TYPE_PPM,
  SOURCE_TYPE_CH,
  SOURCE_TYPE_GVAR,
  SOURCE_TYPE_SPECIAL,
  SOURCE_TYPE_TELEMETRY,
  MAX_SOURCE_TYPE
};

QString RotaryEncoderString(int index);

class RawSourceRange
{
  public:
    RawSourceRange():
      decimals(0),
      min(0.0),
      max(0.0),
      step(1.0),
      offset(0.0)
    {
    }

    float getValue(int value);

    int decimals;
    double min;
    double max;
    double step;
    double offset;
    QString unit;
};

#define RANGE_SINGLE_PRECISION    1
#define RANGE_DELTA_FUNCTION      2
#define RANGE_DELTA_ABS_FUNCTION  4

class RawSource {
  public:
    RawSource():
      type(SOURCE_TYPE_NONE),
      index(0)
    {
    }

    explicit RawSource(int value):
      type(RawSourceType(abs(value)/65536)),
      index(value >= 0 ? abs(value)%65536 : -(abs(value)%65536))
    {
    }

    RawSource(RawSourceType type, int index=0):
      type(type),
      index(index)
    {
    }

    inline const int toValue() const
    {
      return index >= 0 ? (type * 65536 + index) : -(type * 65536 - index);
    }

    QString toString(const ModelData * model = NULL) const;

    RawSourceRange getRange(const ModelData * model, const GeneralSettings & settings, unsigned int flags=0) const;

    bool operator == ( const RawSource & other) {
      return (this->type == other.type) && (this->index == other.index);
    }

    bool operator != ( const RawSource & other) {
      return (this->type != other.type) || (this->index != other.index);
    }

    bool isTimeBased() const;
    bool isPot() const;
    bool isSlider() const;

    RawSourceType type;
    int index;
};

enum RawSwitchType {
  SWITCH_TYPE_NONE,
  SWITCH_TYPE_SWITCH,
  SWITCH_TYPE_VIRTUAL,
  SWITCH_TYPE_MULTIPOS_POT,
  SWITCH_TYPE_TRIM,
  SWITCH_TYPE_ROTARY_ENCODER,
  SWITCH_TYPE_ON,
  SWITCH_TYPE_OFF,
  SWITCH_TYPE_ONE,
  SWITCH_TYPE_FLIGHT_MODE,
  SWITCH_TYPE_TIMER_MODE
};

class RawSwitch {
  public:
    RawSwitch():
      type(SWITCH_TYPE_NONE),
      index(0)
    {
    }

    explicit RawSwitch(int value):
      type(RawSwitchType(abs(value)/256)),
      index(value >= 0 ? abs(value)%256 : -(abs(value)%256))
    {
    }

    RawSwitch(RawSwitchType type, int index=0):
      type(type),
      index(index)
    {
    }

    inline const int toValue() const
    {
      return index >= 0 ? (type * 256 + index) : -(type * 256 - index);
    }

    QString toString() const;

    bool operator== ( const RawSwitch& other) {
      return (this->type == other.type) && (this->index == other.index);
    }

    bool operator!= ( const RawSwitch& other) {
      return (this->type != other.type) || (this->index != other.index);
    }

    RawSwitchType type;
    int index;
};

class CurveReference {
  public:
    enum CurveRefType {
      CURVE_REF_DIFF,
      CURVE_REF_EXPO,
      CURVE_REF_FUNC,
      CURVE_REF_CUSTOM
    };

    CurveReference() { clear(); }

    CurveReference(CurveRefType type, int value):
      type(type),
      value(value)
    {
    }

    void clear() { memset(this, 0, sizeof(CurveReference)); }

    CurveRefType type;
    int value;

    QString toString() const;
};

enum InputMode {
  INPUT_MODE_NONE,
  INPUT_MODE_POS,
  INPUT_MODE_NEG,
  INPUT_MODE_BOTH
};

class ExpoData {
  public:
    ExpoData() { clear(); }
    RawSource srcRaw;
    unsigned int scale;
    unsigned int mode;
    unsigned int chn;
    RawSwitch swtch;
    unsigned int flightModes;        // -5=!FP4, 0=normal, 5=FP4
    int  weight;
    int offset;
    CurveReference curve;
    int carryTrim;
    char name[10+1];
    void clear() { memset(this, 0, sizeof(ExpoData)); }
};

class CurvePoint {
  public:
    int8_t x;
    int8_t y;
};

class CurveData {
  public:
    enum CurveType {
      CURVE_TYPE_STANDARD,
      CURVE_TYPE_CUSTOM,
      CURVE_TYPE_LAST = CURVE_TYPE_CUSTOM
    };

    CurveData();
    void clear(int count);
    bool isEmpty() const;
    CurveType type;
    bool smooth;
    int  count;
    CurvePoint points[CPN_MAX_POINTS];
    char name[6+1];
};

class LimitData {
  public:
    LimitData() { clear(); }
    int   min;
    int   max;
    bool  revert;
    int   offset;
    int   ppmCenter;
    bool  symetrical;
    char  name[6+1];
    CurveReference curve;
    QString minToString() const;
    QString maxToString() const;
    QString offsetToString() const;
    QString revertToString() const;
    void clear();
};

enum MltpxValue {
  MLTPX_ADD=0,
  MLTPX_MUL=1,
  MLTPX_REP=2
};

#define MIXDATA_NAME_LEN  10

class MixData {
  public:
    MixData() { clear(); }
    unsigned int destCh;            //        1..CPN_MAX_CHNOUT
    RawSource srcRaw;
    int     weight;
    RawSwitch swtch;
    CurveReference     curve;             //0=symmetrisch
    unsigned int delayUp;
    unsigned int delayDown;
    unsigned int speedUp;           // Servogeschwindigkeit aus Tabelle (10ms Cycle)
    unsigned int speedDown;         // 0 nichts
    int  carryTrim;
    bool noExpo;
    MltpxValue mltpx;          // multiplex method 0=+ 1=* 2=replace
    unsigned int mixWarn;           // mixer warning
    unsigned int flightModes;             // -5=!FP4, 0=normal, 5=FP4
    int    sOffset;
    char   name[MIXDATA_NAME_LEN+1];

    void clear() { memset(this, 0, sizeof(MixData)); }
};

enum CSFunction {
  LS_FN_OFF,
  LS_FN_VPOS,
  LS_FN_VNEG,
  LS_FN_APOS,
  LS_FN_ANEG,
  LS_FN_AND,
  LS_FN_OR,
  LS_FN_XOR,
  LS_FN_EQUAL,
  LS_FN_NEQUAL,
  LS_FN_GREATER,
  LS_FN_LESS,
  LS_FN_EGREATER,
  LS_FN_ELESS,
  LS_FN_DPOS,
  LS_FN_DAPOS,
  LS_FN_VEQUAL, // added at the end to avoid everything renumbered
  LS_FN_VALMOSTEQUAL,
  LS_FN_TIMER,
  LS_FN_STICKY,
  LS_FN_EDGE,
  // later ... LS_FN_RANGE,
  LS_FN_MAX
};

enum CSFunctionFamily {
  LS_FAMILY_VOFS,
  LS_FAMILY_VBOOL,
  LS_FAMILY_VCOMP,
  LS_FAMILY_TIMER,
  LS_FAMILY_STICKY,
  LS_FAMILY_EDGE,
};

class LogicalSwitchData { // Logical Switches data
  public:
    LogicalSwitchData(unsigned int func=0)
    {
      clear();
      this->func = func;
    }
    unsigned int func;
    int val1;
    int val2;
    int val3;
    unsigned int delay;
    unsigned int duration;
    int andsw;
    void clear() { memset(this, 0, sizeof(LogicalSwitchData)); }
    CSFunctionFamily getFunctionFamily() const;
    unsigned int getRangeFlags() const;
    QString funcToString() const;
};

enum AssignFunc {
  FuncOverrideCH1 = 0,
  FuncOverrideCH32 = FuncOverrideCH1+CPN_MAX_CHNOUT-1,
  FuncTrainer,
  FuncTrainerRUD,
  FuncTrainerELE,
  FuncTrainerTHR,
  FuncTrainerAIL,
  FuncInstantTrim,
  FuncPlaySound,
  FuncPlayHaptic,
  FuncReset,
  FuncSetTimer1,
  FuncSetTimer2,
  FuncSetTimer3,
  FuncVario,
  FuncPlayPrompt,
  FuncPlayBoth,
  FuncPlayValue,
  FuncPlayScript,
  FuncLogs,
  FuncVolume,
  FuncBacklight,
  FuncScreenshot,
  FuncBackgroundMusic,
  FuncBackgroundMusicPause,
  FuncAdjustGV1,
  FuncAdjustGVLast = FuncAdjustGV1+CPN_MAX_GVARS-1,
  FuncSetFailsafeInternalModule,
  FuncSetFailsafeExternalModule,
  FuncRangeCheckInternalModule,
  FuncRangeCheckExternalModule,
  FuncBindInternalModule,
  FuncBindExternalModule,
  FuncCount,
  FuncReserve = -1
};

enum GVarAdjustModes
{
  FUNC_ADJUST_GVAR_CONSTANT,
  FUNC_ADJUST_GVAR_SOURCE,
  FUNC_ADJUST_GVAR_GVAR,
  FUNC_ADJUST_GVAR_INCDEC
};

class CustomFunctionData { // Function Switches data
  public:
    CustomFunctionData(AssignFunc func=FuncOverrideCH1) { clear(); this->func = func; }
    RawSwitch    swtch;
    AssignFunc   func;
    int param;
    char paramarm[10];
    unsigned int enabled; // TODO perhaps not any more the right name
    unsigned int adjustMode;
    int repeatParam;
    void clear();
    QString funcToString() const;
    QString paramToString(const ModelData * model) const;
    QString repeatToString() const;
    QString enabledToString() const;

    static void populateResetParams(const ModelData * model, QComboBox * b, unsigned int value);
    static void populatePlaySoundParams(QStringList & qs);
    static void populateHapticParams(QStringList & qs);

};

class FlightModeData {
  public:
    FlightModeData() { clear(0); }
    int trimMode[CPN_MAX_STICKS+CPN_MAX_AUX_TRIMS];
    int trimRef[CPN_MAX_STICKS+CPN_MAX_AUX_TRIMS];
    int trim[CPN_MAX_STICKS+CPN_MAX_AUX_TRIMS];
    RawSwitch swtch;
    char name[10+1];
    unsigned int fadeIn;
    unsigned int fadeOut;
    int rotaryEncoders[CPN_MAX_ENCODERS];
    int gvars[CPN_MAX_GVARS];
    void clear(const int phase);
};

class SwashRingData { // Swash Ring data
  public:
    SwashRingData() { clear(); }
    int elevatorWeight;
    int aileronWeight;
    int collectiveWeight;
    unsigned int  type;
    RawSource collectiveSource;
    RawSource aileronSource;
    RawSource elevatorSource;
    unsigned int  value;
    void clear() { memset(this, 0, sizeof(SwashRingData)); }
};

class FrSkyAlarmData {
  public:
    FrSkyAlarmData() { clear(); }
    unsigned int   level;               // 0=none, 1=Yellow, 2=Orange, 3=Red
    unsigned int   greater;             // 0=LT(<), 1=GT(>)
    unsigned int value;               // 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.

    void clear() { memset(this, 0, sizeof(FrSkyAlarmData)); }
};

class FrSkyRSSIAlarm {
  public:
    FrSkyRSSIAlarm() { clear(0, 50); }
    unsigned int level;
    int value;
    void clear(unsigned int level, int value) { this->level = level; this->value = value;}
};

class FrSkyChannelData {
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
};

struct FrSkyBarData {
  RawSource source;
  unsigned int barMin;           // minimum for bar display
  unsigned int barMax;           // ditto for max display (would usually = ratio)
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

enum UartModes {
  UART_MODE_NONE,
  UART_MODE_TELEMETRY_MIRROR,
  UART_MODE_TELEMETRY,
  UART_MODE_SBUS_TRAINER,
  UART_MODE_DEBUG
};

enum TrainerMode {
  TRAINER_MODE_MASTER_TRAINER_JACK,
  TRAINER_MODE_SLAVE,
  TRAINER_MODE_MASTER_SBUS_EXTERNAL_MODULE,
  TRAINER_MODE_MASTER_CPPM_EXTERNAL_MODULE,
  TRAINER_MODE_MASTER_BATTERY_COMPARTMENT,
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
    FrSkyRSSIAlarm rssiAlarms[2];
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

#define TIMER_NAME_LEN 8

class TimerData {
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
};

enum PulsesProtocol {
  PULSES_OFF,
  PULSES_PPM,
  PULSES_SILV_A,
  PULSES_SILV_B,
  PULSES_SILV_C,
  PULSES_CTP1009,
  PULSES_LP45,
  PULSES_DSM2,
  PULSES_DSMX,
  PULSES_PPM16,
  PULSES_PPMSIM,
  PULSES_PXX_XJT_X16,
  PULSES_PXX_XJT_D8,
  PULSES_PXX_XJT_LR12,
  PULSES_PXX_DJT,
  PULSES_CROSSFIRE,
  PULSES_MULTIMODULE,
  PULSES_PROTOCOL_LAST
};

enum MultiModuleRFProtocols {
  MM_RF_PROTO_FLYSKY=0,
  MM_RF_PROTO_FIRST=MM_RF_PROTO_FLYSKY,
  MM_RF_PROTO_HUBSAN,
  MM_RF_PROTO_FRSKY,
  MM_RF_PROTO_HISKY,
  MM_RF_PROTO_V2X2,
  MM_RF_PROTO_DSM2,
  MM_RF_PROTO_DEVO,
  MM_RF_PROTO_YD717,
  MM_RF_PROTO_KN,
  MM_RF_PROTO_SYMAX,
  MM_RF_PROTO_SLT,
  MM_RF_PROTO_CX10,
  MM_RF_PROTO_CG023,
  MM_RF_PROTO_BAYANG,
  MM_RF_PROTO_ESky,
  MM_RF_PROTO_MT99XX,
  MM_RF_PROTO_MJXQ,
  MM_RF_PROTO_SHENQI,
  MM_RF_PROTO_FY326,
  MM_RF_PROTO_SFHSS,
  MM_RF_PROTO_J6PRO,
  MM_RF_PROTO_FQ777,
  MM_RF_PROTO_ASSAN,
  MM_RF_PROTO_HONTAI,
  MM_RF_PROTO_OLRS,
  MM_RF_PROTO_AFHDS2A,
  MM_RF_PROTO_Q2X2,
  MM_RF_PROTO_LAST= MM_RF_PROTO_Q2X2
};

unsigned int getNumSubtypes(MultiModuleRFProtocols type);

enum TrainerProtocol {
  TRAINER_MASTER_JACK,
  TRAINER_SLAVE_JACK,
  TRAINER_MASTER_SBUS_MODULE,
  TRAINER_MASTER_CPPM_MODULE,
  TRAINER_MASTER_SBUS_BATT_COMPARTMENT
};

class ModuleData {
  public:
    ModuleData() { clear(); }
    unsigned int modelId;
    int          protocol;
    unsigned int subType;
    bool         invertedSerial;
    unsigned int channelsStart;
    int          channelsCount; // 0=8 channels
    unsigned int failsafeMode;
    int          failsafeChannels[CPN_MAX_CHNOUT];


    struct {
        int delay;
        bool pulsePol;           // false = positive
        bool outputType;         // false = open drain, true = push pull
        int frameLength;
    } ppm;

    struct {
      unsigned int rfProtocol;
      bool autoBindMode;
      bool lowPowerMode;
      bool customProto;
      int optionValue;
   } multi;



    void clear() { memset(this, 0, sizeof(ModuleData)); }
    QString polarityToString() const { return ppm.pulsePol ? QObject::tr("Positive") : QObject::tr("Negative"); } // TODO ModelPrinter
};

#define CPN_MAX_SCRIPTS       7
#define CPN_MAX_SCRIPT_INPUTS 10
class ScriptData {
  public:
    ScriptData() { clear(); }
    char    filename[10+1];
    char    name[10+1];
    int     inputs[CPN_MAX_SCRIPT_INPUTS];
    void clear() { memset(this, 0, sizeof(ScriptData)); }
};

#define CPN_MAX_SENSORS       32
class SensorData {

  public:

    enum
    {
      TELEM_TYPE_CUSTOM,
      TELEM_TYPE_CALCULATED
    };

    enum
    {
      TELEM_FORMULA_ADD,
      TELEM_FORMULA_AVERAGE,
      TELEM_FORMULA_MIN,
      TELEM_FORMULA_MAX,
      TELEM_FORMULA_MULTIPLY,
      TELEM_FORMULA_TOTALIZE,
      TELEM_FORMULA_CELL,
      TELEM_FORMULA_CONSUMPTION,
      TELEM_FORMULA_DIST,
      TELEM_FORMULA_LAST = TELEM_FORMULA_DIST
    };

    enum {
      TELEM_CELL_INDEX_LOWEST,
      TELEM_CELL_INDEX_1,
      TELEM_CELL_INDEX_2,
      TELEM_CELL_INDEX_3,
      TELEM_CELL_INDEX_4,
      TELEM_CELL_INDEX_5,
      TELEM_CELL_INDEX_6,
      TELEM_CELL_INDEX_HIGHEST,
      TELEM_CELL_INDEX_DELTA,
    };

    enum
    {
      UNIT_RAW,
      UNIT_VOLTS,
      UNIT_AMPS,
      UNIT_MILLIAMPS,
      UNIT_KTS,
      UNIT_METERS_PER_SECOND,
      UNIT_FEET_PER_SECOND,
      UNIT_KMH,
      UNIT_MPH,
      UNIT_METERS,
      UNIT_FEET,
      UNIT_CELSIUS,
      UNIT_FAHRENHEIT,
      UNIT_PERCENT,
      UNIT_MAH,
      UNIT_WATTS,
      UNIT_MILLIWATTS,
      UNIT_DB,
      UNIT_RPMS,
      UNIT_G,
      UNIT_DEGREE,
      UNIT_RADIANS,
      UNIT_MILLILITERS,
      UNIT_FLOZ,
      UNIT_HOURS,
      UNIT_MINUTES,
      UNIT_SECONDS,
      // FrSky format used for these fields, could be another format in the future
      UNIT_FIRST_VIRTUAL,
      UNIT_CELLS = UNIT_FIRST_VIRTUAL,
      UNIT_DATETIME,
      UNIT_GPS,
      UNIT_GPS_LONGITUDE,
      UNIT_GPS_LATITUDE,
      UNIT_GPS_LONGITUDE_EW,
      UNIT_GPS_LATITUDE_NS,
      UNIT_DATETIME_YEAR,
      UNIT_DATETIME_DAY_MONTH,
      UNIT_DATETIME_HOUR_MIN,
      UNIT_DATETIME_SEC
    };

    SensorData() { clear(); }
    unsigned int type; // custom / formula
    unsigned int id;
    unsigned int subid;
    unsigned int instance;
    unsigned int persistentValue;
    unsigned int formula;
    char label[4+1];
    unsigned int unit;
    unsigned int prec;
    bool autoOffset;
    bool filter;
    bool logs;
    bool persistent;
    bool onlyPositive;

    // for custom sensors
    unsigned int ratio;
    int offset;

    // for consumption
    unsigned int amps;

    // for cell
    unsigned int source;
    unsigned int index;

    // for calculations
    int sources[4];

    // for GPS dist
    unsigned int gps;
    unsigned int alt;

    bool isAvailable() const { return strlen(label) > 0; }
    void updateUnit();
    QString unitString() const;
    void clear() { memset(this, 0, sizeof(SensorData)); }
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

class ModelData {
  public:
    ModelData();
    ModelData(const ModelData & src);
    ModelData & operator = (const ModelData & src);

    ExpoData * insertInput(const int idx);
    void removeInput(const int idx);

    bool isInputValid(const unsigned int idx) const;
    bool hasExpos(uint8_t inputIdx) const;
    bool hasMixes(uint8_t output) const;

    QVector<const ExpoData *> expos(int input) const;
    QVector<const MixData *> mixes(int channel) const;

    bool      used;
    int       category;
    char      name[15+1];
    char      filename[16+1];
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
    LogicalSwitchData  logicalSw[CPN_MAX_CSW];
    CustomFunctionData customFn[CPN_MAX_CUSTOM_FUNCTIONS];
    SwashRingData swashRingData;
    unsigned int thrTraceSrc;
    uint64_t switchWarningStates;
    unsigned int switchWarningEnable;
    unsigned int potsWarningMode;
    bool potsWarningEnabled[CPN_MAX_POTS];
    int          potPosition[CPN_MAX_POTS];
    bool         displayChecklist;
    // TODO structure
    char     gvars_names[CPN_MAX_GVARS][6+1];
    bool     gvars_popups[CPN_MAX_GVARS];
    MavlinkData mavlink;
    unsigned int telemetryProtocol;
    FrSkyData frsky;

    char bitmap[10+1];

    unsigned int trainerMode;

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

    ModelData removeGlobalVars();

    void clearMixes();
    void clearInputs();

    int getChannelsMax(bool forceExtendedLimits=false) const;

  protected:
    void removeGlobalVar(int & var);
};


class TrainerMix {
  public:
    TrainerMix() { clear(); }
    unsigned int src; // 0-7 = ch1-8
    RawSwitch swtch;
    int weight;
    unsigned int mode;   // off, add-mode, subst-mode
    void clear() { memset(this, 0, sizeof(TrainerMix)); }
};

class TrainerData {
  public:
    TrainerData() { clear(); }
    int         calib[4];
    TrainerMix  mix[4];
    void clear() { memset(this, 0, sizeof(TrainerData)); }
};

class GeneralSettings {
  public:

    enum BeeperMode {
      BEEPER_QUIET = -2,
      BEEPER_ALARMS_ONLY = -1,
      BEEPER_NOKEYS = 0,
      BEEPER_ALL = 1
    };

    enum PotConfig {
      POT_NONE,
      POT_WITH_DETENT,
      POT_MULTIPOS_SWITCH,
      POT_WITHOUT_DETENT
    };

    enum SliderConfig {
      SLIDER_NONE,
      SLIDER_WITH_DETENT
    };

    GeneralSettings();

    int getDefaultStick(unsigned int channel) const;
    RawSource getDefaultSource(unsigned int channel) const;
    int getDefaultChannel(unsigned int stick) const;

    unsigned int version;
    unsigned int variant;
    int   calibMid[CPN_MAX_STICKS+CPN_MAX_POTS+CPN_MAX_MOUSE_ANALOGS];
    int   calibSpanNeg[CPN_MAX_STICKS+CPN_MAX_POTS+CPN_MAX_MOUSE_ANALOGS];
    int   calibSpanPos[CPN_MAX_STICKS+CPN_MAX_POTS+CPN_MAX_MOUSE_ANALOGS];
    unsigned int  currModelIndex;
    char currModelFilename[16+1];
    unsigned int   contrast;
    unsigned int   vBatWarn;
    int    txVoltageCalibration;
    int    txCurrentCalibration;
    int    vBatMin;
    int    vBatMax;
    int   backlightMode;
    TrainerData trainer;
    unsigned int   view;    // main screen view // TODO enum
    bool      disableThrottleWarning;
    bool      fai;
    int       switchWarning; // -1=down, 0=off, 1=up
    bool      disableMemoryWarning;
    BeeperMode beeperMode;
    bool      disableAlarmWarning;
    bool      enableTelemetryAlarm;
    BeeperMode hapticMode;
    unsigned int   stickMode; // TODO enum
    int       timezone;
    bool      adjustRTC;
    bool      optrexDisplay;
    unsigned int    inactivityTimer;
    bool      minuteBeep;
    bool      preBeep;
    bool      flashBeep;
    unsigned int  splashMode;
    int splashDuration;
    unsigned int  backlightDelay;
    unsigned int   templateSetup;  //RETA order according to chout_ar array
    int    PPM_Multiplier;
    int    hapticLength;
    unsigned int   reNavigation;
    unsigned int stickReverse;
    unsigned int   speakerPitch;
    int   hapticStrength;
    unsigned int   speakerMode;
    char      ownerName[10+1];
    unsigned int   switchWarningStates;
    int    beeperLength;
    unsigned int    gpsFormat;
    int     speakerVolume;
    unsigned int   backlightBright;
    unsigned int   backlightOffBright;
    int switchesDelay;
    int    temperatureCalib;
    int    temperatureWarn;
    unsigned int mAhWarn;
    unsigned int mAhUsed;
    unsigned int globalTimer;
    bool bluetoothEnable;
    char bluetoothName[10+1];
    unsigned int btBaudrate;
    unsigned int sticksGain;
    unsigned int rotarySteps;
    unsigned int countryCode;
    bool jitterFilter;
    unsigned int imperial;
    char ttsLanguage[2+1];
    int beepVolume;
    int wavVolume;
    int varioVolume;
    int varioPitch;
    int varioRange;
    int varioRepeat;
    int backgroundVolume;
    unsigned int mavbaud;
    unsigned int switchUnlockStates;
    unsigned int hw_uartMode;
    unsigned int backlightColor;
    CustomFunctionData customFn[CPN_MAX_CUSTOM_FUNCTIONS];
    char switchName[18][3+1];
    unsigned int switchConfig[18];
    char stickName[4][3+1];
    char potName[4][3+1];
    unsigned int potConfig[4];
    char sliderName[4][3+1];
    unsigned int sliderConfig[4];

    char themeName[8+1];
    typedef uint8_t ThemeOptionData[8+1];
    ThemeOptionData themeOptionValue[5];

    struct SwitchInfo {
      SwitchInfo(unsigned int index, unsigned int position):
        index(index),
        position(position)
      {
      }
      unsigned int index;
      unsigned int position;
    };

    static SwitchInfo switchInfoFromSwitchPositionTaranis(unsigned int index);
    bool switchPositionAllowedTaranis(int index) const;
    bool switchSourceAllowedTaranis(int index) const;
    bool isPotAvailable(int index) const;
    bool isSliderAvailable(int index) const;
};

class CategoryData {
  public:
    char name[15+1];
};

class RadioData {
  public:
    RadioData();
    
    GeneralSettings generalSettings;
    std::vector<CategoryData> categories;
    std::vector<ModelData> models;
    
    void setCurrentModel(unsigned int index)
    {
      generalSettings.currModelIndex = index;
      strcpy(generalSettings.currModelFilename, models[index].filename);
    }

    QString getNextModelFilename()
    {
      char filename[sizeof(ModelData::filename)];
      int index = 0;
      bool found = true;
      while (found) {
        sprintf(filename, "model%d.bin", ++index);
        found = false;
        for (unsigned int i=0; i<models.size(); i++) {
          if (strcmp(filename, models[i].filename) == 0) {
            found = true;
            break;
          }
        }
      }
      return filename;
    }
};

enum Capability {
  Models,
  ModelName,
  FlightModes,
  FlightModesName,
  FlightModesHaveFades,
  Imperial,
  Mixes,
  Timers,
  TimersName,
  CustomFunctions,
  SafetyChannelCustomFunction,
  VoicesAsNumbers,
  VoicesMaxLength,
  MultiLangVoice,
  ModelImage,
  Pots,
  Sliders,
  Switches,
  SwitchesPositions,
  LogicalSwitches,
  CustomAndSwitches,
  HasNegAndSwitches,
  LogicalSwitchesExt,
  RotaryEncoders,
  Outputs,
  ChannelsName,
  ExtraInputs,
  ExtendedTrims,
  NumCurves,
  NumCurvePoints,
  OffsetWeight,
  Simulation,
  SoundMod,
  SoundPitch,
  MaxVolume,
  Haptic,
  HasBeeper,
  ModelTrainerEnable,
  HasExpoNames,
  HasNoExpo,
  HasMixerNames,
  HasCvNames,
  HasPxxCountry,
  HasPPMStart,
  HasGeneralUnits,
  HasFAIMode,
  OptrexDisplay,
  PPMExtCtrl,
  PPMFrameLength,
  Telemetry,
  TelemetryUnits,
  TelemetryBars,
  Heli,
  Gvars,
  GvarsInCS,
  GvarsAreNamed,
  GvarsFlightModes,
  GvarsName,
  NoTelemetryProtocol,
  TelemetryCustomScreens,
  TelemetryCustomScreensFieldsPerLine,
  TelemetryMaxMultiplier,
  HasVario,
  HasVarioSink,
  HasFailsafe,
  HasSoundMixer,
  NumModules,
  PPMCenter,
  PPMUnitMicroseconds,
  SYMLimits,
  HastxCurrentCalibration,
  HasVolume,
  HasBrightness,
  PerModelTimers,
  SlowScale,
  SlowRange,
  PermTimers,
  HasSDLogs,
  CSFunc,
  LcdWidth,
  LcdHeight,
  LcdDepth,
  GetThrSwitch,
  HasDisplayText,
  HasTopLcd,
  GlobalFunctions,
  VirtualInputs,
  TrainerInputs,
  RtcTime,
  SportTelemetry,
  LuaScripts,
  LuaInputsPerScript,
  LuaOutputsPerScript,
  LimitsPer1000,
  EnhancedCurves,
  HasFasOffset,
  HasMahPersistent,
  MultiposPots,
  MultiposPotsPositions,
  SimulatorVariant,
  MavlinkTelemetry,
  HasInputDiff,
  HasMixerExpo,
  MixersMonitor,
  HasBatMeterRange,
  DangerousFunctions,
};

class SimulatorInterface;
class EEPROMInterface
{
  public:

    EEPROMInterface(BoardEnum board):
      board(board)
    {
    }

    virtual ~EEPROMInterface() {}

    inline BoardEnum getBoard() { return board; }

    virtual unsigned long load(RadioData &radioData, const uint8_t * eeprom, int size) = 0;

    virtual unsigned long loadBackup(RadioData & radioData, const uint8_t * eeprom, int esize, int index) = 0;

    virtual unsigned long loadxml(RadioData & radioData, QDomDocument &doc) = 0;

    virtual int save(uint8_t * eeprom, RadioData & radioData, uint8_t version=0, uint32_t variant=0) = 0;

    virtual int getSize(const ModelData &) = 0;

    virtual int getSize(const GeneralSettings &) = 0;

    virtual const int getEEpromSize() = 0;
    
    virtual int loadFile(RadioData & radioData, const QString & filename) = 0;

    virtual int saveFile(const RadioData & radioData, const QString & filename) = 0;

  protected:

    BoardEnum board;

  private:

    EEPROMInterface();

};

extern std::list<QString> EEPROMWarnings;

/* EEPROM string conversion functions */
void setEEPROMString(char *dst, const char *src, int size);
void getEEPROMString(char *dst, const char *src, int size);

float ValToTim(int value);
int TimToVal(float value);

QString getSignedStr(int value);
QString getGVarString(int16_t val, bool sign=false);

inline int applyStickMode(int stick, unsigned int mode)
{
  if (mode == 0 || mode > 4) {
    std::cerr << "Incorrect stick mode" << mode;
    return stick;
  }

  const unsigned int stickModes[]= {
      1, 2, 3, 4,
      1, 3, 2, 4,
      4, 2, 3, 1,
      4, 3, 2, 1 };

  if (stick >= 1 && stick <= 4)
    return stickModes[(mode-1)*4 + stick - 1];
  else
    return stick;
}

void registerEEpromInterfaces();
void unregisterEEpromInterfaces();
void registerOpenTxFirmwares();
void unregisterOpenTxFirmwares();

enum EepromLoadErrors {
  ALL_OK,
  UNKNOWN_ERROR,
  UNSUPPORTED_NEWER_VERSION,
  WRONG_SIZE,
  WRONG_FILE_SYSTEM,
  NOT_OPENTX,
  NOT_TH9X,
  NOT_GRUVIN9X,
  NOT_ERSKY9X,
  NOT_ER9X,
  UNKNOWN_BOARD,
  WRONG_BOARD,
  BACKUP_NOT_SUPPORTED,

  HAS_WARNINGS,
  OLD_VERSION,
  WARNING_WRONG_FIRMWARE,

  NUM_ERRORS
};

void ShowEepromErrors(QWidget *parent, const QString &title, const QString &mainMessage, unsigned long errorsFound);
void ShowEepromWarnings(QWidget *parent, const QString &title, unsigned long errorsFound);


struct Option {
  const char * name;
  QString tooltip;
  uint32_t variant;
};

class Firmware {

  public:
    Firmware(const QString & id, const QString & name, const BoardEnum board, EEPROMInterface * eepromInterface):
      id(id),
      name(name),
      board(board),
      eepromInterface(eepromInterface),
      variantBase(0),
      base(NULL)
    {
    }

    Firmware(Firmware * base, const QString & id, const QString & name, const BoardEnum board, EEPROMInterface * eepromInterface):
      id(id),
      name(name),
      board(board),
      eepromInterface(eepromInterface),
      variantBase(0),
      base(base)
    {
    }

    virtual ~Firmware()
    {
      delete eepromInterface;
    }

    inline const Firmware * getFirmwareBase() const
    {
      return base ? base : this;
    }

    // TODO needed?
    inline void setVariantBase(unsigned int variant)
    {
      variantBase = variant;
    }

    virtual Firmware * getFirmwareVariant(const QString & id) { return NULL; }

    unsigned int getVariantNumber();

    virtual void addLanguage(const char *lang);

    virtual void addTTSLanguage(const char *lang);

    virtual void addOption(const char *option, QString tooltip="", uint32_t variant=0);

    virtual void addOptions(Option options[]);

    inline int saveEEPROM(uint8_t * eeprom, RadioData & radioData, uint8_t version=0, uint32_t variant=0)
    {
      return eepromInterface->save(eeprom, radioData, version, variant);
    }

    virtual QString getStampUrl() = 0;

    virtual QString getReleaseNotesUrl() = 0;

    virtual QString getFirmwareUrl() = 0;

    inline BoardEnum getBoard() const
    {
      return board;
    }

    inline QString getName() const
    {
      return name;
    }

    inline QString getId() const
    {
      return id;
    }

    inline EEPROMInterface * getEepromInterface()
    {
      return eepromInterface;
    }

    virtual int getCapability(Capability) = 0;

    enum SwitchType {
      SWITCH_NONE,
      SWITCH_TOGGLE,
      SWITCH_2POS,
      SWITCH_3POS
    };

    struct Switch {
      SwitchType type;
      const char * name;
    };

    virtual Switch getSwitch(unsigned int index) = 0;

    virtual QString getAnalogInputName(unsigned int index) = 0;

    virtual QTime getMaxTimerStart() = 0;

    virtual bool isTelemetrySourceAvailable(int source) = 0;

    virtual int isAvailable(PulsesProtocol proto, int port=0) = 0;

    const int getFlashSize();

  public:
    QList<const char *> languages;
    QList<const char *> ttslanguages;
    QList< QList<Option> > opts;

  protected:
    QString id;
    QString name;
    BoardEnum board;
    EEPROMInterface * eepromInterface;
    unsigned int variantBase;
    Firmware * base;

  private:
    Firmware();

};

extern QList<Firmware *> firmwares;
extern Firmware * default_firmware_variant;
extern Firmware * current_firmware_variant;

Firmware * GetFirmware(QString id);

inline Firmware * GetCurrentFirmware()
{
  return current_firmware_variant;
}

SimulatorInterface *getCurrentFirmwareSimulator();

inline EEPROMInterface * GetEepromInterface()
{
  return GetCurrentFirmware()->getEepromInterface();
}

inline int divRoundClosest(const int n, const int d)
{
  return ((n < 0) ^ (d < 0)) ? ((n - d/2)/d) : ((n + d/2)/d);
}

#define CHECK_IN_ARRAY(T, index) ((unsigned int)index < (unsigned int)(sizeof(T)/sizeof(T[0])) ? T[(unsigned int)index] : "???")

SimulatorInterface * GetCurrentFirmwareSimulator();

extern QList<EEPROMInterface *> eepromInterfaces;

#endif // _EEPROMINTERFACE_H_
