/*
 * Author - Bertrand Songis <bsongis@gmail.com>
 * 
 * Based on th9x -> http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef eeprom_interface_h
#define eeprom_interface_h

#include <inttypes.h>
#include <string.h>
#include <QString>
#include <QStringList>
#include <QList>
#include <QtXml>
#include <iostream>

#if __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#else
#include "../winbuild/winbuild.h"
#endif

#define EESIZE_STOCK          2048
#define EESIZE_M128           4096
#define EESIZE_GRUVIN9X       4096
#define EESIZE_TARANIS        (32*1024)
#define EESIZE_TARANIS_REV4a  (64*1024)
#define EESIZE_SKY9X          (128*4096)
#define EESIZE_RLC_MAX        EESIZE_TARANIS_REV4a

template<class t> t LIMIT(t mi, t x, t ma) { return std::min(std::max(mi, x), ma); }

enum BoardEnum {
  BOARD_STOCK,
  BOARD_M128,
  BOARD_GRUVIN9X,
  BOARD_SKY9X,
  BOARD_TARANIS,
  BOARD_TARANIS_REV4a
};
#define IS_STOCK(board)       (board==BOARD_STOCK || board==BOARD_M128)
#define IS_ARM(board)       (board==BOARD_SKY9X || board==BOARD_TARANIS  || board==BOARD_TARANIS_REV4a)
#define IS_TARANIS(board)       (board==BOARD_TARANIS  || board==BOARD_TARANIS_REV4a)

const uint8_t modn12x3[4][4]= {
  {1, 2, 3, 4},
  {1, 3, 2, 4},
  {4, 2, 3, 1},
  {4, 3, 2, 1} };

#define C9XMAX_MODELS             60
#define C9X_MAX_PHASES            9
#define C9X_MAX_MIXERS            64
#define C9X_MAX_EXPOS             32
#define C9X_MAX_CURVES            16
#define MAX_POINTS                17
#define C9X_MAX_GVARS                 9
#define NUM_SAFETY_CHNOUT         16
#define C9X_NUM_CHNOUT            32 // number of real output channels
#define C9X_NUM_CSW               32 // number of custom switches
#define C9X_MAX_CUSTOM_FUNCTIONS  32 // number of functions assigned to switches
#define C9X_NUM_MODULES           2

#define STK_RUD  1
#define STK_ELE  2
#define STK_THR  3
#define STK_AIL  4
#define STK_P1   5
#define STK_P2   6
#define STK_P3   7

#define DSW_THR   1 //DSW_SA ^
#define DSW_RUD   2 //DSW_SA -
#define DSW_ELE   3 //DSW_SA _
#define DSW_ID0   4  //DSW_SB ^
#define DSW_ID1   5  //DSW_SB -
#define DSW_ID2   6 //DSW_SB _
#define DSW_AIL   7 //DSW_SC ^
#define DSW_GEA   8  //DSW_SC -
#define DSW_TRN   9 //DSW_SC _
#define DSW_SD0 10
#define DSW_SD1 11
#define DSW_SD2 12
#define DSW_SE0 13
#define DSW_SE1 14
#define DSW_SE2 15
#define DSW_SF0 16
#define DSW_SF1 17
#define DSW_SG0 18
#define DSW_SG1 19
#define DSW_SG2 20

const uint8_t chout_ar[] = { //First number is 0..23 -> template setup,  Second is relevant channel out
1,2,3,4 , 1,2,4,3 , 1,3,2,4 , 1,3,4,2 , 1,4,2,3 , 1,4,3,2,
2,1,3,4 , 2,1,4,3 , 2,3,1,4 , 2,3,4,1 , 2,4,1,3 , 2,4,3,1,
3,1,2,4 , 3,1,4,2 , 3,2,1,4 , 3,2,4,1 , 3,4,1,2 , 3,4,2,1,
4,1,2,3 , 4,1,3,2 , 4,2,1,3 , 4,2,3,1 , 4,3,1,2 , 4,3,2,1    }; // TODO delete it?

// TODO remove this enum!
enum EnumKeys {
  KEY_MENU,
  KEY_EXIT,
#if defined(PCBACT)
  KEY_CLR,
  KEY_PAGE,
  KEY_PLUS,  /* Fake, used for rotary encoder */
  KEY_MINUS, /* Fake, used for rotary encoder */
#elif defined(PCBX9D)
  KEY_ENTER,
  KEY_PAGE,
  KEY_PLUS,
  KEY_MINUS,
#else
  KEY_DOWN,
  KEY_UP,
  KEY_RIGHT,
  KEY_LEFT,
#endif
};

enum CSFunction {
  CS_FN_OFF,
  CS_FN_VPOS,
  CS_FN_VNEG,
  CS_FN_APOS,
  CS_FN_ANEG,
  CS_FN_AND,
  CS_FN_OR,
  CS_FN_XOR,
  CS_FN_EQUAL,
  CS_FN_NEQUAL,
  CS_FN_GREATER,
  CS_FN_LESS,
  CS_FN_EGREATER,
  CS_FN_ELESS,
  CS_FN_DPOS,
  CS_FN_DAPOS,
  CS_FN_VEQUAL, // added at the end to avoid everything renumbered
  CS_FN_TIM,
  CS_FN_MAXF
};

enum CSFunctionFamily {
  CS_FAMILY_VOFS,
  CS_FAMILY_VBOOL,
  CS_FAMILY_VCOMP,
  CS_FAMILY_TIMERS
};

inline CSFunctionFamily getCSFunctionFamily(int fn)
{
  if (fn==CS_FN_TIM) {
    return (CS_FAMILY_TIMERS);
  } else {
    return ((fn<CS_FN_AND || fn>CS_FN_ELESS) ? CS_FAMILY_VOFS : (fn<CS_FN_EQUAL ? CS_FAMILY_VBOOL : CS_FAMILY_VCOMP));
  }
}

#define CHAR_FOR_NAMES " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-."
#define CHAR_FOR_NAMES_REGEX "[ A-Za-z0-9_.-,]*"

enum HeliSwashTypes {
 HELI_SWASH_TYPE_NONE=0,
 HELI_SWASH_TYPE_120,
 HELI_SWASH_TYPE_120X,
 HELI_SWASH_TYPE_140,
 HELI_SWASH_TYPE_90
};

#define NUM_STICKS          4
#define BOARD_9X_NUM_POTS   3
#define BOARD_X9D_NUM_POTS  4
#define C9X_NUM_POTS        4
#define NUM_CAL_PPM         4
#define NUM_PPM             8
#define NUM_CYC             3
#define C9X_NUM_SWITCHES          10
#define C9X_NUM_KEYS          6
#define C9X_MAX_TIMERS      2

enum TelemetrySource {
  TELEMETRY_SOURCE_TX_BATT,
  TELEMETRY_SOURCE_TIMER1,
  TELEMETRY_SOURCE_TIMER2,
  TELEMETRY_SOURCE_RSSI_TX,
  TELEMETRY_SOURCE_RSSI_RX,
  TELEMETRY_SOURCE_A1,
  TELEMETRY_SOURCE_A2,
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
  TELEMETRY_SOURCE_A1_MIN,
  TELEMETRY_SOURCE_A2_MIN,
  TELEMETRY_SOURCE_ALT_MIN,
  TELEMETRY_SOURCE_ALT_MAX,
  TELEMETRY_SOURCE_RPM_MAX,
  TELEMETRY_SOURCE_T1_MAX,
  TELEMETRY_SOURCE_T2_MAX,
  TELEMETRY_SOURCE_SPEED_MAX,
  TELEMETRY_SOURCE_DIST_MAX,
  TELEMETRY_SOURCE_CURRENT_MAX,
  TELEMETRY_SOURCE_POWER_MAX,
  TELEMETRY_SOURCE_ACC,
  TELEMETRY_SOURCE_GPS_TIME,
  TELEMETRY_SOURCES_STATUS_COUNT,
  TELEMETRY_SOURCES_DISPLAY_COUNT = TELEMETRY_SOURCES_STATUS_COUNT-2,

  TELEMETRY_SOURCES_COUNT = TELEMETRY_SOURCE_VERTICAL_SPEED,
};

#define TM_HASTELEMETRY     0x01
#define TM_HASOFFSET        0x02
#define TM_HASWSHH          0x04

enum RawSourceType {
  SOURCE_TYPE_NONE,
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
  SOURCE_TYPE_TELEMETRY,
  MAX_SOURCE_TYPE
};
class ModelData;

class RawSource {
  public:
    RawSource():
      type(SOURCE_TYPE_NONE),
      index(0)
    {
    }

    RawSource(int value):
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

    QString toString();
    
    int getDecimals(const ModelData & Model);
    double getMin(const ModelData & Model);
    double getMax(const ModelData & Model);
    double getStep(const ModelData & Model);
    double getOffset(const ModelData & Model);
    int getRawOffset(const ModelData & Model);
    
    bool operator== ( const RawSource& other) {
      return (this->type == other.type) && (this->index == other.index);
    }

    RawSourceType type;
    int index;
};

enum RawSwitchType {
  SWITCH_TYPE_NONE,
  SWITCH_TYPE_SWITCH,
  SWITCH_TYPE_VIRTUAL,
  SWITCH_TYPE_MOMENT_SWITCH,
  SWITCH_TYPE_MOMENT_VIRTUAL,
  SWITCH_TYPE_ON,
  SWITCH_TYPE_OFF,
  SWITCH_TYPE_ONM,
  SWITCH_TYPE_TRN,
  SWITCH_TYPE_REA,
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

    QString toString();

    bool operator== ( const RawSwitch& other) {
      return (this->type == other.type) && (this->index == other.index);
    }

    bool operator!= ( const RawSwitch& other) {
      return (this->type != other.type) || (this->index != other.index);
    }

    RawSwitchType type;
    int index;
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

enum BeeperMode {
  e_quiet = -2,
  e_alarms_only = -1,
  e_no_keys = 0,
  e_all = 1
};

class GeneralSettings {
  public:
    GeneralSettings();
    unsigned int version;
    unsigned int variant;
    int   calibMid[NUM_STICKS+C9X_NUM_POTS];
    int   calibSpanNeg[NUM_STICKS+C9X_NUM_POTS];
    int   calibSpanPos[NUM_STICKS+C9X_NUM_POTS];
    unsigned int  currModel; // 0..15
    unsigned int   contrast;
    unsigned int   vBatWarn;
    int    vBatCalib;
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
    int    timezone;
    bool      optrexDisplay;
    unsigned int    inactivityTimer;
    bool      throttleReversed;
    bool      minuteBeep;
    bool      preBeep;
    bool      flashBeep;
    bool      disablePotScroll;
    bool      frskyinternalalarm;
    bool      disableBG;
    unsigned int  splashMode;
    uint8_t   filterInput; // TODO enum
    unsigned int  backlightDelay;
    bool   blightinv;
    bool   stickScroll;
    unsigned int   templateSetup;  //RETA order according to chout_ar array // TODO enum
    int    PPM_Multiplier;
    int    hapticLength;
    unsigned int   reNavigation;
    bool      hideNameOnSplash;
    bool      enablePpmsim;
    unsigned int   speakerPitch;
    unsigned int   hapticStrength;
    unsigned int   speakerMode;
    unsigned int   lightOnStickMove; /* er9x / ersky9x only */
    char      ownerName[10+1];
    unsigned int   switchWarningStates;
    int    beeperLength;
    unsigned int    gpsFormat;
    int     speakerVolume;
    unsigned int   backlightBright;
    int    currentCalib;
    int    temperatureCalib;
    int    temperatureWarn;
    unsigned int mAhWarn;
    unsigned int btBaudrate;
    unsigned int sticksGain;
    unsigned int rotarySteps;
    unsigned int countryCode;
    unsigned int imperial;
    bool crosstrim;
    char ttsLanguage[2+1];
    int beepVolume;
    int wavVolume;
    int varioVolume;
    int backgroundVolume;
    unsigned int mavbaud;
    unsigned int switchUnlockStates;
};

class ExpoData {
  public:
    ExpoData() { clear(); }
    unsigned int mode;         // 0=end, 1=pos, 2=neg, 3=both
    unsigned int chn;
    RawSwitch swtch;
    unsigned int phases;        // -5=!FP4, 0=normal, 5=FP4
    int  weight;
    int  expo;
    unsigned int curveMode;
    int  curveParam;
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
    CurveData() { clear(5); }
    bool custom;         // 0=end, 1=pos, 2=neg, 3=both
    uint8_t count;
    CurvePoint points[MAX_POINTS];
    char  name[6+1];
    void clear(int count) { memset(this, 0, sizeof(CurveData)); this->count = count; }
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
    void clear() { memset(this, 0, sizeof(LimitData)); min = -100; max = +100; }
};

enum MltpxValue {
  MLTPX_ADD=0,
  MLTPX_MUL=1,
  MLTPX_REP=2
};


class MixData {
  public:
    MixData() { clear(); }
    unsigned int destCh;            //        1..C9X_NUM_CHNOUT
    RawSource srcRaw;
    unsigned int srcVariant;
    int     weight;
    int     differential;
    RawSwitch swtch;
    int     curve;             //0=symmetrisch
    unsigned int delayUp;
    unsigned int delayDown;
    unsigned int speedUp;           // Servogeschwindigkeit aus Tabelle (10ms Cycle)
    unsigned int speedDown;         // 0 nichts
    int  carryTrim;
    bool noExpo;
    MltpxValue mltpx;          // multiplex method 0=+ 1=* 2=replace
    unsigned int mixWarn;           // mixer warning
    unsigned int enableFmTrim;
    unsigned int phases;             // -5=!FP4, 0=normal, 5=FP4
    unsigned int lateOffset;
    int    sOffset;
    char   name[10+1];

    void clear() { memset(this, 0, sizeof(MixData)); }
};

class CustomSwData { // Custom Switches data
  public:
    CustomSwData() { clear(); }
    int  val1; //input
    int  val2; //offset
    unsigned int func;
    unsigned int delay;
    unsigned int duration;
    unsigned int andsw;
    void clear() { memset(this, 0, sizeof(CustomSwData)); }
};

class SafetySwData { // Custom Switches data
  public:
    SafetySwData() { clear(); }
    RawSwitch  swtch;
    int        val;

    void clear() { memset(this, 0, sizeof(SafetySwData)); }
};

enum AssignFunc {
  FuncSafetyCh1 = 0,
  FuncSafetyCh16 = FuncSafetyCh1+15,
  FuncTrainer,
  FuncTrainerRUD,
  FuncTrainerELE,
  FuncTrainerTHR,
  FuncTrainerAIL,
  FuncInstantTrim,
  FuncPlaySound,
  FuncPlayHaptic,
  FuncReset,
  FuncVario,
  FuncPlayPrompt,
  FuncPlayBoth,
  FuncPlayValue,
  FuncLogs,
  FuncVolume,
  FuncBacklight,
  FuncBackgroundMusic,
  FuncBackgroundMusicPause,
  FuncAdjustGV1,
  FuncAdjustGV2,
  FuncAdjustGV3,
  FuncAdjustGV4,
  FuncAdjustGV5,
  FuncCount
};

class FuncSwData { // Function Switches data
  public:
    FuncSwData() { clear(); }
    RawSwitch    swtch;
    AssignFunc   func;
    int param;
    char paramarm[10];
    unsigned int enabled; // TODO perhaps not any more the right name
    unsigned int adjustMode;
    unsigned int repeatParam;
    void clear() { memset(this, 0, sizeof(FuncSwData)); }
};

class PhaseData {
  public:
    PhaseData() { clear(); }
    int trimRef[NUM_STICKS]; //
    int trim[NUM_STICKS];
    RawSwitch swtch;
    char name[10+1];
    unsigned int fadeIn;
    unsigned int fadeOut;
    int rotaryEncoders[2];
    int gvars[5];
    void clear() { memset(this, 0, sizeof(PhaseData)); for (int i=0; i<NUM_STICKS; i++) trimRef[i] = -1; }
};

class SwashRingData { // Swash Ring data
  public:
    SwashRingData() { clear(); }
    bool      invertELE;
    bool      invertAIL;
    bool      invertCOL;
    unsigned int  type;
    RawSource collectiveSource;
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

class FrSkyErAlarmData {
  public:
    FrSkyErAlarmData() { clear(); }
    uint8_t frskyAlarmType ;
    uint8_t frskyAlarmLimit ;
    uint8_t frskyAlarmSound ;
    void clear() { memset(this, 0, sizeof(FrSkyErAlarmData)); }
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

    void clear() { memset(this, 0, sizeof(FrSkyChannelData)); }
};

struct FrSkyBarData {
  unsigned int   source;
  unsigned int   barMin;           // minimum for bar display
  unsigned int   barMax;           // ditto for max display (would usually = ratio)
};

struct FrSkyLineData {
  unsigned int     source[3];
};

class FrSkyScreenData {
  public:
    FrSkyScreenData() { clear(); }

    typedef union {
      FrSkyBarData bars[4];
      FrSkyLineData lines[4];
    } FrSkyScreenBody;

    unsigned int type;
    FrSkyScreenBody body;

    void clear() { memset(this, 0, sizeof(FrSkyScreenData)); }
};

class FrSkyData {
  public:
    FrSkyData() { clear(); }
    FrSkyChannelData channels[2];
    unsigned int usrProto;
    unsigned int imperial;
    unsigned int blades;
    unsigned int voltsSource;
    bool altitudeDisplayed;
    unsigned int currentSource;
    unsigned int FrSkyGpsAlt;
    FrSkyScreenData screens[3];
    FrSkyRSSIAlarm rssiAlarms[2];
    unsigned int varioSource;
    int varioMin;
    int varioCenterMin;    // if increment in 0.2m/s = 3.0m/s max
    int varioCenterMax;
    int varioMax;

    void clear() { memset(this, 0, sizeof(FrSkyData)); rssiAlarms[0].clear(2, 45); rssiAlarms[1].clear(3, 42); varioSource = 2/*VARIO*/; }
};

class MavlinkData {
  public:
    MavlinkData() { clear();}
    unsigned int rc_rssi_scale;
    unsigned int pc_rssi_en;
    void clear() { memset(this, 0, sizeof(MavlinkData)); }
};

enum TimerMode {
  TMRMODE_OFF=0,
  TMRMODE_ABS,
  TMRMODE_THs,
  TMRMODE_THp,
  TMRMODE_THt,
  TMRMODE_FIRST_SWITCH,
  TMRMODE_FIRST_MOMENT_SWITCH = TMRMODE_FIRST_SWITCH+64,
  TMRMODE_FIRST_CHPERC = TMRMODE_FIRST_MOMENT_SWITCH+64,
  
  TMRMODE_FIRST_NEG_SWITCH=-TMRMODE_FIRST_SWITCH,
  TMRMODE_FIRST_NEG_MOMENT_SWITCH=-TMRMODE_FIRST_MOMENT_SWITCH,
   /* sw/!sw, !m_sw/!m_sw */
};

class TimerData {
  public:
    TimerData() { clear(); }
    TimerMode mode;   // timer trigger source -> off, abs, THs, TH%, THt, sw/!sw, !m_sw/!m_sw
    int8_t    modeB;
    bool minuteBeep;
    bool countdownBeep;
    bool      dir;    // 0=>Count Down, 1=>Count Up
    unsigned int val;
    bool      persistent;
    int pvalue;
    void clear() { memset(this, 0, sizeof(TimerData)); }
};

enum Protocol {
  OFF,
  PPM,
  SILV_A,
  SILV_B,
  SILV_C,
  CTP1009,
  LP45,
  DSM2,
  DSMX,
  PPM16,
  PPMSIM,
  PXX_XJT_X16,
  PXX_XJT_D8,
  PXX_XJT_LR12,
  PXX_DJT,
  PROTO_LAST
};

class ModuleData {
  public:
    ModuleData() { clear(); }
    int          protocol;
    unsigned int channelsStart;
    int          channelsCount; // 0=8 channels
    unsigned int failsafeMode;
    int          failsafeChannels[C9X_NUM_CHNOUT];
    int          ppmDelay;
    bool         ppmPulsePol;           // false = positive
    int          ppmFrameLength;
    void clear() { memset(this, 0, sizeof(ModuleData)); }
};

class ModelData {
  public:
    ModelData();
    bool      used;
    char      name[12+1];
    uint8_t   modelVoice;
    TimerData timers[2];
    bool      thrTrim;            // Enable Throttle Trim
    bool      thrExpo;            // Enable Throttle Expo
    unsigned int trimInc;            // Trim Increments
    bool      disableThrottleWarning;

    unsigned int beepANACenter;      // 1<<0->A1.. 1<<6->A7

    bool      extendedLimits; // TODO xml
    bool      extendedTrims;
    bool      throttleReversed;
    PhaseData phaseData[C9X_MAX_PHASES];
    MixData   mixData[C9X_MAX_MIXERS];
    LimitData limitData[C9X_NUM_CHNOUT];
    ExpoData  expoData[C9X_MAX_EXPOS];
    CurveData curves[C9X_MAX_CURVES];
    CustomSwData  customSw[C9X_NUM_CSW];
    FuncSwData    funcSw[C9X_MAX_CUSTOM_FUNCTIONS];
    SafetySwData  safetySw[C9X_NUM_CHNOUT];
    SwashRingData swashRingData;
    unsigned int  thrTraceSrc;
    int8_t   traineron;  // 0 disable trainer, 1 allow trainer
    int8_t   t2throttle;  // Start timer2 using throttle
    unsigned int   modelId;
    unsigned int switchWarningStates;
    char     gvars_names[C9X_MAX_GVARS][6+1];
    uint8_t  gvsource[5];
    uint8_t  bt_telemetry;
    uint8_t  numVoice;
    MavlinkData mavlink;
    /* FrSky */
    FrSkyData frsky;
    FrSkyErAlarmData frskyalarms[8];
    uint8_t customdisplay[6];

    char bitmap[10+1];

    unsigned int trainerMode;

    ModuleData moduleData[C9X_NUM_MODULES+1/*trainer*/];

    void clear();
    bool isempty();
    void setDefault(uint8_t id);
    unsigned int getTrimFlightPhase(uint8_t idx, int8_t phase);

    ModelData removeGlobalVars();

  protected:
    void removeGlobalVar(int & var);
};

class RadioData {
  public:   
    GeneralSettings generalSettings;
    ModelData models[C9XMAX_MODELS];    
};

enum Capability {
 OwnerName,
 FlightPhases,
 FlightPhasesAreNamed,
 FlightPhasesHaveFades,
 SimulatorType,
 Mixes,
 MixesWithoutExpo,
 Timers,
 TimerTriggerB,
 TimeDivisions,
 minuteBeep,
 countdownBeep,
 CustomFunctions,
 VoicesAsNumbers,
 VoicesMaxLength,
 ModelVoice,
 MultiLangVoice,
 ModelImage,
 InstantTrimSW,
 Pots,
 Switches,
 SwitchesPositions,
 CustomSwitches,
 CustomAndSwitches,
 HasNegAndSwitches,
 CustomSwitchesExt,
 RotaryEncoders,
 Outputs,
 ExtraChannels,
 ExtraInputs,
 ExtraTrims,
 ExtendedTrims,
 HasNegCurves,
 HasInputFilter,
 HasExpoCurves,
 ExpoIsCurve,
 ExpoCurve5,
 ExpoCurve9,
 CustomCurves,
 NumCurves3,
 NumCurves5,
 NumCurves9,
 NumCurves,
 NumCurvePoints,
 OffsetWeight,
 Simulation,
 SoundMod,
 SoundPitch,
 MaxVolume,
 Beeperlen,
 EepromBackup,
 Haptic,
 HapticLength,
 HapticMode,
 HasBlInvert,
 HasBeeper,
 BandgapMeasure,
 PotScrolling,
 TrainerSwitch,
 ModelTrainerEnable,
 Timer2ThrTrig,
 HasTTrace,
 HasExpoNames,
 HasMixerNames,
 HasChNames,
 HasCvNames,
 HasFuncRepeat,
 HasPxxCountry,
 HasPPMStart,
 HasGeneralUnits,
 HasFAIMode,
 NoTimerDirs,
 NoThrExpo,
 OptrexDisplay,
 PPMExtCtrl,
 PPMFrameLength,
 MixFmTrim,
 gsSwitchMask,
 pmSwitchMask,
 BLonStickMove,
 DSM2Indexes,
 Telemetry,
 TelemetryUnits,
 TelemetryBlades,
 TelemetryBars,
 Gvars,
 GvarsInCS,
 GvarsAreNamed,
 GvarsFlightPhases,
 GvarsHaveSources,
 GvarsAsSources,
 GvarsAsWeight,
 GvarsNum,
 GvarsOfsNum,
 NoTelemetryProtocol,
 TelemetryCSFields,
 TelemetryColsCSFields,
 TelemetryRSSIModel,
 TelemetryAlarm,
 TelemetryInternalAlarm,
 TelemetryTimeshift,
 TelemetryMaxMultiplier,
 HasAltitudeSel,
 HasVario,
 HasVarioSink,
 HasVariants,
 HasFailsafe,
 HasPPMSim,
 HasCrossTrims,
 HasStickScroll,
 HasFixOffset,
 HasSoundMixer,
 NumModules,
 FSSwitch,
 DiffMixers,
 PPMCenter,
 SYMLimits,
 HasCurrentCalibration,
 HasVolume,
 HasBrightness,
 HasContrast,
 PerModelTimers,
 PerModelThrottleWarning,
 PerModelThrottleInvert,
 SlowScale,
 SlowRange,
 PermTimers,
 HasSDLogs,
 CSFunc,
 LCDWidth,
 GetThrSwitch,
};

enum UseContext {
  DefaultContext,
  TimerContext,
  FlightPhaseContext,
  MixerContext,
  ExpoContext,
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

    virtual const char * getName() = 0;

    inline BoardEnum getBoard() { return board; }

    virtual bool load(RadioData &radioData, uint8_t *eeprom, int size) = 0;

    virtual bool loadBackup(RadioData &radioData, uint8_t *eeprom, int esize, int index) = 0;
    
    virtual bool loadxml(RadioData &radioData, QDomDocument &doc) = 0;

    virtual int save(uint8_t *eeprom, RadioData &radioData, uint32_t variant=0, uint8_t version=0) = 0;

    virtual int getSize(ModelData &) = 0;
    
    virtual int getSize(GeneralSettings &) = 0;
    
    virtual int getCapability(const Capability) = 0;
    
    virtual int isAvailable(Protocol proto, int port=0) = 0;

    virtual bool isAvailable(const RawSwitch & swtch, UseContext context) { return true; }

    virtual bool isAvailable(const RawSource & source, UseContext context) { return true; }

    virtual SimulatorInterface * getSimulator() = 0;

    virtual const int getEEpromSize() = 0;

    virtual const int getMaxModels() = 0;

  protected:

    BoardEnum board;

  private:

    EEPROMInterface();

};

extern QString EEPROMWarnings;

/* EEPROM string conversion functions */
void setEEPROMString(char *dst, const char *src, int size);
void getEEPROMString(char *dst, const char *src, int size);

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

inline void applyStickModeToModel(ModelData &model, unsigned int mode)
{
  ModelData model_copy = model;

  // trims
  for (int p=0; p<C9X_MAX_PHASES; p++) {
    for (int i=0; i<NUM_STICKS/2; i++) {
      int converted_stick = applyStickMode(i+1, mode) - 1;
      int tmp = model.phaseData[p].trim[i];
      model.phaseData[p].trim[i] = model.phaseData[p].trim[converted_stick];
      model.phaseData[p].trim[converted_stick] = tmp;
      tmp = model.phaseData[p].trimRef[i];
      model.phaseData[p].trimRef[i] = model.phaseData[p].trimRef[converted_stick];
      model.phaseData[p].trimRef[converted_stick] = tmp;
    }
  }

  // expos
  for (unsigned int i=0; i<sizeof(model.expoData) / sizeof(model.expoData[1]); i++) {
    if (model.expoData[i].mode)
      model_copy.expoData[i].chn = applyStickMode(model.expoData[i].chn+1, mode) - 1;
  }
  int index=0;
  for (unsigned int i=0; i<NUM_STICKS; i++) {
    for (unsigned int e=0; e<sizeof(model.expoData) / sizeof(model.expoData[1]); e++) {
      if (model_copy.expoData[e].mode && model_copy.expoData[e].chn == i)
        model.expoData[index++] = model_copy.expoData[e];
    }
  }

  // mixers
  for (int i=0; i<C9X_MAX_MIXERS; i++) {
    if (model.mixData[i].srcRaw.type == SOURCE_TYPE_STICK) {
      model.mixData[i].srcRaw.index = applyStickMode(model.mixData[i].srcRaw.index + 1, mode) - 1;
    }
  }

  // virtual switches
  for (int i=0; i<C9X_NUM_CSW; i++) {
    RawSource source;
    switch (getCSFunctionFamily(model.customSw[i].func)) {
      case CS_FAMILY_VCOMP:
        source = RawSource(model.customSw[i].val2);
        if (source.type == SOURCE_TYPE_STICK)
          source.index = applyStickMode(source.index + 1, mode) - 1;
        model.customSw[i].val2 = source.toValue();
        // no break
      case CS_FAMILY_VOFS:
        source = RawSource(model.customSw[i].val1);
        if (source.type == SOURCE_TYPE_STICK)
          source.index = applyStickMode(source.index + 1, mode) - 1;
        model.customSw[i].val1 = source.toValue();
        break;
      default:
        break;
    }
  }

  // heli
  if (model.swashRingData.collectiveSource.type == SOURCE_TYPE_STICK)
    model.swashRingData.collectiveSource.index = applyStickMode(model.swashRingData.collectiveSource.index + 1, mode) - 1;
}

void RegisterFirmwares();

bool LoadBackup(RadioData &radioData, uint8_t *eeprom, int esize, int index);
bool LoadEeprom(RadioData &radioData, uint8_t *eeprom, int size);
bool LoadEepromXml(RadioData &radioData, QDomDocument &doc);

struct Option {
  const char * name;
  QString tooltip;
  uint32_t variant;
};

class FirmwareInfo {
  public:
    FirmwareInfo():
      parent(NULL),
      id(QString::null),
      eepromInterface(NULL),
      voice(false),
      variantBase(0)
    {
    }

    virtual ~FirmwareInfo()
    {
    }

    FirmwareInfo(const QString & id, const QString & name, EEPROMInterface * eepromInterface, const QString & url = QString(), const QString & stamp = QString(), const QString & rnurl = QString(), bool voice = false):
      parent(NULL),
      id(id),
      name(name),
      eepromInterface(eepromInterface),
      url(url),
      stamp(stamp),
      rnurl(rnurl),
      voice(voice),
      variantBase(0)
    {
    }

    FirmwareInfo(const QString & id, EEPROMInterface * eepromInterface, const QString & url, const QString & stamp = QString(), const QString & rnurl = QString(), bool voice=false):
      parent(NULL),
      id(id),
      name(QString::null),
      eepromInterface(eepromInterface),
      url(url),
      stamp(stamp),
      rnurl(rnurl),
      voice(voice),
      variantBase(0)
    {
    }

    void setVariantBase(unsigned int variant) {
      this->variantBase = variant;
    }

    unsigned int getVariant(const QString & id);

    virtual void addLanguage(const char *lang);

    virtual void addTTSLanguage(const char *lang);

    virtual void addOption(const char *option, QString tooltip="", uint32_t variant=0);

    virtual void addOptions(Option options[]);

    QStringList get_options() {
      if (parent)
        return id.mid(parent->id.length()+1).split("-", QString::SkipEmptyParts);
      else
        return QStringList();
    }

    int saveEEPROM(uint8_t *eeprom, RadioData &radioData, uint32_t variant=0, unsigned int version=0) {
      return eepromInterface->save(eeprom, radioData, variant, version);
    }

    virtual QString getUrl(const QString &fwId) {
      if (url.contains("%1"))
        return url.arg(fwId);
      else
        return url;
    }

    virtual QString getRnUrl(const QString &fwId) {
      if (rnurl.contains("%1"))
        return rnurl.arg(fwId);
      else
        return rnurl;
    }

    
    QList<const char *> languages;
    QList<const char *> ttslanguages;
    QList< QList<Option> > opts;
    FirmwareInfo *parent;
    QString id;
    QString name;
    EEPROMInterface * eepromInterface;
    QString url;
    QString stamp;
    QString rnurl;
    bool voice;
    unsigned int variantBase;
};

struct FirmwareVariant {
  QString id;
  FirmwareInfo *firmware;
  unsigned int variant;
};

extern QList<FirmwareInfo *> firmwares;
extern FirmwareVariant default_firmware_variant;
extern FirmwareVariant current_firmware_variant;

FirmwareVariant GetFirmwareVariant(QString id);

inline FirmwareInfo * GetFirmware(QString id)
{
  return GetFirmwareVariant(id).firmware;
}

inline FirmwareInfo * GetCurrentFirmware()
{
  return current_firmware_variant.firmware;
}

inline EEPROMInterface * GetEepromInterface()
{
  return GetCurrentFirmware()->eepromInterface;
}

inline unsigned int GetCurrentFirmwareVariant()
{
  return current_firmware_variant.variant;
}

#define CHECK_IN_ARRAY(T, index) ((unsigned int)index < (unsigned int)(sizeof(T)/sizeof(T[0])) ? T[(unsigned int)index] : "???")

#endif
