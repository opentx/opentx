/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
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

#ifndef myeeprom_h
#define myeeprom_h

#include <inttypes.h>

#define WARN_THR_BIT  0x01
#define WARN_BEP_BIT  0x80
#define WARN_SW_BIT   0x02
#define WARN_MEM_BIT  0x04
#define WARN_BVAL_BIT 0x38

#define WARN_THR     (!(g_eeGeneral.warnOpts & WARN_THR_BIT))
#define WARN_BEP     (!(g_eeGeneral.warnOpts & WARN_BEP_BIT))
#define WARN_SW      (!(g_eeGeneral.warnOpts & WARN_SW_BIT))
#define WARN_MEM     (!(g_eeGeneral.warnOpts & WARN_MEM_BIT))
#define BEEP_VAL     ( (g_eeGeneral.warnOpts & WARN_BVAL_BIT) >>3 )

#if defined(PCBX9D)
#define EEPROM_VER       214
#elif defined(PCBSKY9X)
#define EEPROM_VER       214
#elif defined(PCBGRUVIN9X)
#define EEPROM_VER       213
#else
#define EEPROM_VER       213
#endif

#ifndef PACK
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif


typedef int16_t gvar_t;

#if !defined(CPUM64)
typedef char gvar_name_t[6];
#define GVAR_MAX  1024
#endif

#define RESERVE_RANGE_FOR_GVARS 10
// even we do not spend space in EEPROM for 10 GVARS, we reserve the space inside the range of values, like offset, weight, etc.
#if defined(CPUM64) && defined(GVARS)
  #define MAX_GVARS 5
  #define MODEL_GVARS_DATA gvar_t gvars[MAX_GVARS];
  #define PHASE_GVARS_DATA
  #define GVAR_VALUE(x, p) g_model.gvars[x]
#elif defined(CPUM64)
  #define MAX_GVARS 0
  #define MODEL_GVARS_DATA
  #define PHASE_GVARS_DATA
#elif defined(GVARS)
  #define MAX_GVARS 5
  #define MODEL_GVARS_DATA gvar_name_t gvarsNames[MAX_GVARS];
  #define PHASE_GVARS_DATA gvar_t gvars[MAX_GVARS]
  #define GVAR_VALUE(x, p) g_model.phaseData[p].gvars[x]
#else
  #define MAX_GVARS 0
  #define MODEL_GVARS_DATA gvar_name_t gvarsNames[5];
  #define PHASE_GVARS_DATA gvar_t gvars[5]
#endif

PACK(typedef struct t_TrainerMix {
  uint8_t srcChn:6; // 0-7 = ch1-8
  uint8_t mode:2;   // off,add-mode,subst-mode
  int8_t  studWeight;
}) TrainerMix; //

PACK(typedef struct t_TrainerData {
  int16_t        calib[4];
  TrainerMix     mix[4];
}) TrainerData;

PACK(typedef struct t_FrSkyRSSIAlarm {
  int8_t    level:2;
  int8_t    value:6;
}) FrSkyRSSIAlarm;

#if defined(PCBX9D) || defined(PCBACT)
enum MainViews {
  VIEW_TIMERS,
  VIEW_INPUTS,
  VIEW_SWITCHES,
  VIEW_COUNT
};
#else
enum MainViews {
  VIEW_OUTPUTS_VALUES,
  VIEW_OUTPUTS_BARS,
  VIEW_INPUTS,
  VIEW_TIMER2,
  VIEW_COUNT
};
#endif

enum BeeperMode {
  e_mode_quiet = -2,
  e_mode_alarms,
  e_mode_nokeys,
  e_mode_all
};

#if defined(CPUARM)
#define EXTRA_GENERAL_FIELDS \
  uint8_t  backlightBright; \
  int8_t   currentCalib; \
  int8_t   temperatureWarn; \
  uint8_t  mAhWarn; \
  uint16_t mAhUsed; \
  uint32_t globalTimer; \
  int8_t   temperatureCalib; \
  uint8_t  btBaudrate; \
  uint8_t  optrexDisplay; \
  uint8_t  sticksGain;
#else
#define EXTRA_GENERAL_FIELDS
#endif

#if defined(PCBX9D) || defined(PCBACT)
#define MODELDATA_EXTRA   char bitmap[10];
#define LIMITDATA_EXTRA   char name[6];
#define swstate_t         uint16_t
#elif defined(PCBSKY9X)
#define MODELDATA_EXTRA   uint8_t ppmSCH; int8_t ppm2SCH; int8_t ppm2NCH;
#define LIMITDATA_EXTRA
#define swstate_t         uint8_t
#else
#define MODELDATA_EXTRA
#define LIMITDATA_EXTRA
#define swstate_t         uint8_t
#endif

enum BacklightMode {
  e_backlight_mode_off = 0,
  e_backlight_mode_keys,
  e_backlight_mode_sticks,
  e_backlight_mode_both,
  e_backlight_mode_on
};

#if defined(FSPLASH) || defined(XSPLASH)
#define SPLASH_MODE uint8_t splashMode:3
#else
#define SPLASH_MODE uint8_t splashMode:1; uint8_t spare4:2
#endif

#define ALTERNATE_VIEW 0x10
PACK(typedef struct t_EEGeneral {
  uint8_t   version;
  uint16_t  variant;
  int16_t   calibMid[NUM_STICKS+NUM_POTS];
  int16_t   calibSpanNeg[NUM_STICKS+NUM_POTS];
  int16_t   calibSpanPos[NUM_STICKS+NUM_POTS];
  uint16_t  chkSum;
  int8_t    currModel;
  uint8_t   contrast;
  uint8_t   vBatWarn;
  int8_t    vBatCalib;
  int8_t    backlightMode;
  TrainerData trainer;
  uint8_t   view;      //index of subview in main scrren
  int8_t    spare1:3;
  int8_t    beeperMode:2;
  uint8_t   flashBeep:1;
  uint8_t   disableMemoryWarning:1;
  uint8_t   disableAlarmWarning:1;
  uint8_t   stickMode:2;
  int8_t    timezone:5;
  uint8_t   spare2:1;
  uint8_t   inactivityTimer;
  uint8_t   throttleReversed:1;
  uint8_t   spare3:2;
  SPLASH_MODE; /* 3bits */
  int8_t    hapticMode:2;    // -2=quiet, -1=only alarms, 0=no keys, 1=all
  uint8_t   blOffBright:4;
  uint8_t   blOnBright:4;
  uint8_t   lightAutoOff;
  uint8_t   templateSetup;  //RETA order according to chout_ar array 
  int8_t    PPM_Multiplier;
  int8_t    hapticLength;
  uint8_t   reNavigation; // not used on STOCK board
  int8_t    beeperLength:3;
  uint8_t   hapticStrength:3;
  uint8_t   gpsFormat:1;
  uint8_t   unexpectedShutdown:1;
  uint8_t   speakerPitch;
  int8_t    speakerVolume;
  int8_t    vBatMin;
  int8_t    vBatMax;

  EXTRA_GENERAL_FIELDS;

}) EEGeneral;

#if defined(PCBX9D) || defined(PCBACT)
#define LEN_MODEL_NAME     12
#define LEN_EXPOMIX_NAME   10
#define LEN_FP_NAME        10
#elif defined(PCBSKY9X)
#define LEN_MODEL_NAME     10
#define LEN_EXPOMIX_NAME   6
#define LEN_FP_NAME        6
#else
#define LEN_MODEL_NAME     10
#define LEN_FP_NAME        6
#endif

#if defined(CPUARM)
PACK(typedef struct t_ExpoData {
  uint8_t mode;         // 0=end, 1=pos, 2=neg, 3=both
  uint8_t chn;
  int8_t  swtch;
  uint16_t phases;
  int8_t  weight;
  uint8_t curveMode;
  char    name[LEN_EXPOMIX_NAME];
  int8_t  curveParam;
}) ExpoData;
#else
PACK(typedef struct t_ExpoData {
  uint8_t mode:2;         // 0=end, 1=pos, 2=neg, 3=both
  int8_t  swtch:6;
  uint8_t chn:2;
  uint8_t phases:5;
  uint8_t curveMode:1;
  uint8_t weight;         // we have one bit spare here :)
  int8_t  curveParam;
}) ExpoData;
#endif

PACK(typedef struct t_LimitData {
  int8_t  min;
  int8_t  max;
  int8_t  ppmCenter;
  int16_t offset:14;
  uint16_t symetrical:1;
  uint16_t revert:1;

  LIMITDATA_EXTRA

}) LimitData;


#define TRIM_OFF    (1)
#define TRIM_ON     (0)
#define TRIM_RUD    (-1)
#define TRIM_ELE    (-2)
#define TRIM_THR    (-3)
#define TRIM_AIL    (-4)

#define MLTPX_ADD   0
#define MLTPX_MUL   1
#define MLTPX_REP   2

#define MODE_DIFFERENTIAL  0
#define MODE_EXPO          0
#define MODE_CURVE         1

#if defined(CPUARM)
#define DELAY_STEP  10
#define SLOW_STEP   10
#define DELAY_MAX   (25*DELAY_STEP) /* 25 seconds */
#define SLOW_MAX    (25*SLOW_STEP)  /* 25 seconds */
PACK(typedef struct t_MixData {
  uint8_t  destCh;
  uint16_t phases;
  uint8_t  curveMode:1;       // O=curve, 1=differential
  uint8_t  noExpo:1;
  int8_t   carryTrim:3;
  uint8_t  mltpx:2;           // multiplex method: 0 means +=, 1 means *=, 2 means :=
  uint8_t  spare:1;
  int16_t  weight;
  int8_t   swtch;
  int8_t   curveParam;
  uint8_t  mixWarn:4;         // mixer warning
  uint8_t  srcVariant:4;
  uint8_t  delayUp;
  uint8_t  delayDown;
  uint8_t  speedUp;
  uint8_t  speedDown;
  uint8_t  srcRaw;
  int16_t  offset;
  char     name[LEN_EXPOMIX_NAME];
}) MixData;

#define MD_WEIGHT(md) (md->weight)
#define MD_WEIGHT_TO_UNION(md, var) var.word = md->weight
#define MD_UNION_TO_WEIGHT(var, md) md->weight = var.word
// #define MD_SETWEIGHT(md, val) md->weight = val

PACK( union u_int8int16_t {
  struct {
    int8_t  lo;
    uint8_t hi;
  } bytes_t;
  int16_t word;
});

#define MD_OFFSET(md) (md->offset)
#define MD_OFFSET_TO_UNION(md, var) var.word = md->offset
#define MD_UNION_TO_OFFSET(var, md) md->offset = var.word
// #define MD_SETOFFSET(md, val) md->offset = val

#else
#define DELAY_STEP  2
#define SLOW_STEP   2
#define DELAY_MAX   15 /* 7.5 seconds */
#define SLOW_MAX    15 /* 7.5 seconds */
PACK(typedef struct t_MixData {
  uint8_t destCh:4;          // 0, 1..NUM_CHNOUT
  uint8_t curveMode:1;       // O=curve, 1=differential
  uint8_t noExpo:1;
  uint8_t weightMode:1;
  uint8_t offsetMode:1;
  int8_t  weight;
  int8_t  swtch:6;
  uint8_t mltpx:2;           // multiplex method: 0 means +=, 1 means *=, 2 means :=
  uint8_t phases:5;
  int8_t  carryTrim:3;
  uint8_t srcRaw:6;
  uint8_t mixWarn:2;         // mixer warning
  uint8_t delayUp:4;
  uint8_t delayDown:4;
  uint8_t speedUp:4;
  uint8_t speedDown:4;
  int8_t  curveParam;
  int8_t  offset;
}) MixData;

PACK( union u_gvarint_t {
  struct {
    int8_t lo;
	uint8_t hi;
  } bytes_t;
  int16_t word;
	
  u_gvarint_t(int8_t l, uint8_t h) {bytes_t.lo=l; bytes_t.hi=h?255:0;} // hi bit is negativ sign

private:
  // prevent unwanted constructors, also saves program
  u_gvarint_t() {}
  u_gvarint_t(const u_gvarint_t&) {}
}); 
#define MD_WEIGHT(md) (u_gvarint_t(md->weight,md->weightMode).word)
  
PACK( union u_int8int16_t {
  struct {
    int8_t  lo;
    uint8_t hi;
  } bytes_t;
  int16_t word;
});

#define MD_WEIGHT_TO_UNION(md, var) var.bytes_t.lo=md->weight; var.bytes_t.hi=md->weightMode?255:0
#define MD_UNION_TO_WEIGHT(var, md) md->weight=var.bytes_t.lo; if (var.word<0) md->weightMode=1; else md->weightMode=0
// #define MD_SETWEIGHT(md, val) md->weight=val; if (val<0) md->weightMode=1; else md->weightMode=0 

#define MD_OFFSET(md) (u_gvarint_t(md->offset,md->offsetMode).word)
#define MD_OFFSET_TO_UNION(md, var) var.bytes_t.lo=md->offset; var.bytes_t.hi=md->offsetMode?255:0
#define MD_UNION_TO_OFFSET(var, md) md->offset=var.bytes_t.lo; if (var.word<0) md->offsetMode=1; else md->offsetMode=0 /* set negative sign */
// #define MD_SETOFFSET(md, val) md->offset=val; if (val<0) md->offsetMode=1; else md->offsetMode=0

#endif

#if defined(CPUARM)
#define MAX_CSW_DURATION 120 /*60s*/
#define MAX_CSW_DELAY    120 /*60s*/
PACK(typedef struct t_CustomSwData { // Custom Switches data
  int8_t  v1; //input
  int8_t  v2; //offset
  uint8_t func;
  uint8_t delay;
  uint8_t duration;
  uint8_t andsw;  
}) CustomSwData;
#else
PACK(typedef struct t_CustomSwData { // Custom Switches data
  int8_t  v1; //input
  int8_t  v2; //offset
  uint8_t func:4;
  uint8_t andsw:4;
}) CustomSwData;
#endif

enum Functions {
#if defined(CPUARM)
  FUNC_SAFETY_CH1,
  FUNC_SAFETY_CH16=FUNC_SAFETY_CH1+15,
#else
  FUNC_SAFETY_GROUP1,
  FUNC_SAFETY_GROUP2,
  FUNC_SAFETY_GROUP3,
  FUNC_SAFETY_GROUP4,
#endif
  FUNC_TRAINER,
  FUNC_TRAINER_RUD,
  FUNC_TRAINER_ELE,
  FUNC_TRAINER_THR,
  FUNC_TRAINER_AIL,
  FUNC_INSTANT_TRIM,
  FUNC_PLAY_SOUND,
  FUNC_HAPTIC,
  FUNC_RESET,
  FUNC_VARIO,
  FUNC_PLAY_TRACK,
#if !defined(CPUARM)
  FUNC_PLAY_BOTH,
#endif
  FUNC_PLAY_VALUE,
#if !defined(PCBSTD)
  FUNC_LOGS,
#endif
#if defined(CPUARM)
  FUNC_VOLUME,
#endif
  FUNC_BACKLIGHT,
#if defined(CPUARM)
  FUNC_BACKGND_MUSIC,
  FUNC_BACKGND_MUSIC_PAUSE,
#endif
#if defined(GVARS)
  FUNC_ADJUST_GV1,
  FUNC_ADJUST_GVLAST = (FUNC_ADJUST_GV1 + (MAX_GVARS-1)),
#endif
#if defined(DEBUG)
  FUNC_TEST, // should remain the last before MAX as not added in companion9x
#endif
  FUNC_MAX
};

enum ResetFunctionParam {
  FUNC_RESET_TIMER1,
  FUNC_RESET_TIMER2,
  FUNC_RESET_ALL,
#if defined(FRSKY)
  FUNC_RESET_TELEMETRY,
#endif
#if ROTARY_ENCODERS > 0
  FUNC_RESET_ROTENC1,
#endif
#if ROTARY_ENCODERS > 1
  FUNC_RESET_ROTENC2,
#endif
  FUNC_RESET_PARAMS_COUNT
};

#if defined(CPUARM)
#if defined(PCBX9D)
 #define LEN_CFN_NAME 10
#else
 #define LEN_CFN_NAME 6
#endif
PACK(typedef struct t_CustomFnData { // Function Switches data
  int8_t  swtch;
  uint8_t func;
  PACK(union {
    char name[LEN_CFN_NAME];
    struct {
      uint32_t val;
    } composite;
  }) param;
  uint8_t mode:2;
  uint8_t active:6;
}) CustomFnData;
#define CFN_FUNC(p)        ((p)->func)
#define CFN_ACTIVE(p)      ((p)->active)
#define CFN_CH_NUMBER(p)   (CFN_FUNC(p))
#define CFN_PLAY_REPEAT(p) ((p)->active)
#define CFN_GVAR_MODE(p)   ((p)->mode)
#define CFN_PARAM(p)       ((p)->param.composite.val)
#define CFN_RESET_PARAM(p) memset(&(p)->param, 0, sizeof((p)->param))
#else
PACK(typedef struct t_CustomFnData { // Function Switches data
  int8_t  swtch; // input
  union {
    struct {
      uint8_t param:3;
      uint8_t func:5;
    } func_param;

    struct {
      uint8_t active:1;
      uint8_t param:2;
      uint8_t func:5;
    } func_param_enable;

    struct {
      uint8_t active:1;
      uint8_t func:7;
    } func_safety;
  } internal;
  uint8_t param;
}) CustomFnData;
#define CFN_FUNC(p)        ((p)->internal.func_param.func)
#define CFN_ACTIVE(p)      ((p)->internal.func_param_enable.active)
#define CFN_CH_NUMBER(p)   ((p)->internal.func_safety.func)
#define CFN_PLAY_REPEAT(p) ((p)->internal.func_param.param)
#define CFN_GVAR_MODE(p)   ((p)->internal.func_param_enable.param)
#define CFN_PARAM(p)       ((p)->param)
#define CFN_RESET_PARAM(p) CFN_PARAM(p) = 0
#endif

enum TelemetryUnit {
  UNIT_VOLTS,
  UNIT_AMPS,
  UNIT_METERS_PER_SECOND,
  UNIT_RAW,
  UNIT_KMH,
  UNIT_METERS,
  UNIT_DEGREES,
  UNIT_PERCENT,
  UNIT_MILLIAMPS,
  UNIT_A1A2_MAX = UNIT_MILLIAMPS,
  UNIT_MAH,
  UNIT_WATTS,
  UNIT_DBM,
  UNIT_FEET,
  UNIT_KTS,
  UNIT_HOURS,
  UNIT_MINUTES,
  UNIT_SECONDS,
  UNIT_RPMS,
  UNIT_G,
};

#if defined(CPUARM)
PACK(typedef struct t_FrSkyChannelData {
  uint8_t   ratio;              // 0.0 means not used, 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  int16_t   offset:12;
  uint16_t  type:4;             // channel unit (0=volts, ...)
  uint8_t   alarms_value[2];    // 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   alarms_level:4;
  uint8_t   alarms_greater:2;   // 0=LT(<), 1=GT(>)
  uint8_t   spare:2;
  uint8_t   multiplier;         // 0=no multiplier, 1=*2 multiplier
}) FrSkyChannelData;
#else
PACK(typedef struct t_FrSkyChannelData {
  uint8_t   ratio;              // 0.0 means not used, 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  int16_t   offset:12;
  uint16_t  type:4;             // channel unit (0=volts, ...)
  uint8_t   alarms_value[2];    // 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc. 
  uint8_t   alarms_level:4;
  uint8_t   alarms_greater:2;   // 0=LT(<), 1=GT(>)
  uint8_t   multiplier:2;       // 0=no multiplier, 1=*2 multiplier
}) FrSkyChannelData;
#endif

enum TelemetrySource {
  TELEM_NONE,
  TELEM_TX_VOLTAGE,
  TELEM_TM1,
  TELEM_TM2,
  TELEM_RSSI_TX,
  TELEM_RSSI_RX,
  TELEM_A1,
  TELEM_A2,
  TELEM_ALT,
  TELEM_RPM,
  TELEM_FUEL,
  TELEM_T1,
  TELEM_T2,
  TELEM_SPEED,
  TELEM_DIST,
  TELEM_GPSALT,
  TELEM_CELL,
  TELEM_CELLS_SUM,
  TELEM_VFAS,
  TELEM_CURRENT,
  TELEM_CONSUMPTION,
  TELEM_POWER,
  TELEM_ACCx,
  TELEM_ACCy,
  TELEM_ACCz,
  TELEM_HDG,
  TELEM_VSPD,
  TELEM_MIN_A1,
  TELEM_MIN_A2,
  TELEM_MIN_ALT,
  TELEM_MAX_ALT,
  TELEM_MAX_RPM,
  TELEM_MAX_T1,
  TELEM_MAX_T2,
  TELEM_MAX_SPEED,
  TELEM_MAX_DIST,
  TELEM_MAX_CURRENT,
  TELEM_ACC,
  TELEM_GPS_TIME,
  TELEM_CSW_MAX = TELEM_POWER,
  TELEM_NOUSR_MAX = TELEM_A2,
  TELEM_DISPLAY_MAX = TELEM_MAX_CURRENT,
  TELEM_STATUS_MAX = TELEM_GPS_TIME
};

enum VarioSource {
  VARIO_SOURCE_FIRST = 0,
  VARIO_SOURCE_ALTI = VARIO_SOURCE_FIRST,
  VARIO_SOURCE_ALTI_PLUS,
  VARIO_SOURCE_VARIO,
  VARIO_SOURCE_A1,
  VARIO_SOURCE_A2,
  VARIO_SOURCE_LAST = VARIO_SOURCE_A2
};

#if defined(FRSKY_HUB)
  #define NUM_TELEMETRY      TELEM_CSW_MAX
#elif defined(WS_HOW_HIGH)
  #define NUM_TELEMETRY      TELEM_ALT
#elif defined(FRSKY)
  #define NUM_TELEMETRY      TELEM_A2
#elif defined(MAVLINK)
  #define NUM_TELEMETRY      4
#else
  #define NUM_TELEMETRY      TELEM_TM2
#endif

PACK(typedef struct t_FrSkyBarData {
  uint8_t    source;
  uint8_t    barMin;           // minimum for bar display
  uint8_t    barMax;           // ditto for max display (would usually = ratio)
}) FrSkyBarData;

#if defined(PCBX9D)
  #define NUM_LINE_ITEMS 3
#else
  #define NUM_LINE_ITEMS 2
#endif
PACK(typedef struct t_FrSkyLineData {
  uint8_t    sources[NUM_LINE_ITEMS];
}) FrSkyLineData;

typedef union t_FrSkyScreenData {
  FrSkyBarData  bars[4];
  FrSkyLineData lines[4];
} FrSkyScreenData;

enum FrskyUsrProtocols {
  USR_PROTO_NONE,
  USR_PROTO_FRSKY,
  USR_PROTO_WS_HOW_HIGH,
  USR_PROTO_LAST = USR_PROTO_WS_HOW_HIGH,
};

enum FrskySource {
  FRSKY_SOURCE_NONE,
  FRSKY_SOURCE_A1,
  FRSKY_SOURCE_A2,
  FRSKY_SOURCE_FAS,
  FRSKY_SOURCE_CELLS,
};

#if defined(CPUARM)
#define MAX_FRSKY_SCREENS 3
PACK(typedef struct t_FrSkyData {
  FrSkyChannelData channels[2];
  uint8_t usrProto; // Protocol in FrSky user data, 0=None, 1=FrSky hub, 2=WS HowHigh, 3=Halcyon
  uint8_t voltsSource;
  uint8_t blades;   // How many blades for RPMs, 0=2 blades, 1=3 blades
  uint8_t currentSource;
  uint8_t screensType;
  FrSkyScreenData screens[MAX_FRSKY_SCREENS];
  uint8_t varioSource;
  int8_t  varioCenterMax;
  int8_t  varioCenterMin;
  int8_t  varioMin;
  int8_t  varioMax;
  FrSkyRSSIAlarm rssiAlarms[2];
}) FrSkyData;
#else
#define MAX_FRSKY_SCREENS 2
PACK(typedef struct t_FrSkyData {
  FrSkyChannelData channels[2];
  uint8_t usrProto:2; // Protocol in FrSky user data, 0=None, 1=FrSky hub, 2=WS HowHigh, 3=Halcyon
  uint8_t blades:2;   // How many blades for RPMs, 0=2 blades, 1=3 blades
  uint8_t screensType:2;
  uint8_t voltsSource:2;
  int8_t  varioMin:4;
  int8_t  varioMax:4;
  FrSkyRSSIAlarm rssiAlarms[2];
  FrSkyScreenData screens[MAX_FRSKY_SCREENS];
  uint8_t varioSource:3;
  int8_t  varioCenterMin:5;
  uint8_t currentSource:3;
  int8_t  varioCenterMax:5;
}) FrSkyData;
#endif

#ifdef MAVLINK
#define ROTARY_TYPE_OFF     0
#define ROTARY_TYPE_PPM     1
#define ROTARY_TYPE_MAVLINK 2
#define NUM_ROTARY_SW       8
#define MAX_MODES_VAL       16
PACK(typedef struct t_RotarySwChannelData {
  uint8_t typeRotary:2;   // see defines ROTARY_TYPE
  uint8_t numMode:6;      // num mode
}) RotarySwChannelData;

PACK(typedef struct t_MavlinkData {
  RotarySwChannelData rotarySw[NUM_ROTARY_SW];
  int8_t   modesVal[MAX_MODES_VAL];
}) MavlinkData;
#endif

PACK(typedef struct t_SwashRingData { // Swash Ring data
  uint8_t   invertELE:1;
  uint8_t   invertAIL:1;
  uint8_t   invertCOL:1;
  uint8_t   type:5;  
  uint8_t   collectiveSource;
  uint8_t   value;
}) SwashRingData;

#define TRIM_EXTENDED_MAX 500
#define TRIM_EXTENDED_MIN (-TRIM_EXTENDED_MAX)
#define TRIM_MAX 125
#define TRIM_MIN (-TRIM_MAX)

#define ROTARY_ENCODER_MAX  1024

#if defined(PCBACT)
#define NUM_ROTARY_ENCODERS 0
#define ROTARY_ENCODER_ARRAY_EXTRA
#define ROTARY_ENCODER_ARRAY int16_t rotaryEncoders[1];
#elif defined(PCBX9D)
#define NUM_ROTARY_ENCODERS 0
#define NUM_ROTARY_ENCODERS_EXTRA 0
#define ROTARY_ENCODER_ARRAY_EXTRA
#define ROTARY_ENCODER_ARRAY int16_t rotaryEncoders[1];
#elif defined(PCBSKY9X)
#define NUM_ROTARY_ENCODERS_EXTRA 0
#define NUM_ROTARY_ENCODERS 1
#define ROTARY_ENCODER_ARRAY int16_t rotaryEncoders[1];
#define ROTARY_ENCODER_ARRAY_EXTRA
#elif defined(PCBGRUVIN9X) && ROTARY_ENCODERS > 2
#define NUM_ROTARY_ENCODERS_EXTRA 2
#define NUM_ROTARY_ENCODERS (2+NUM_ROTARY_ENCODERS_EXTRA)
#define ROTARY_ENCODER_ARRAY int16_t rotaryEncoders[2];
#define ROTARY_ENCODER_ARRAY_EXTRA int16_t rotaryEncodersExtra[MAX_PHASES][NUM_ROTARY_ENCODERS_EXTRA];
#elif defined(PCBGRUVIN9X) && ROTARY_ENCODERS <= 2
#define NUM_ROTARY_ENCODERS_EXTRA 0
#define NUM_ROTARY_ENCODERS 2
#define ROTARY_ENCODER_ARRAY int16_t rotaryEncoders[2];
#define ROTARY_ENCODER_ARRAY_EXTRA
#else
#define NUM_ROTARY_ENCODERS_EXTRA 0
#define NUM_ROTARY_ENCODERS 0
#define ROTARY_ENCODER_ARRAY
#define ROTARY_ENCODER_ARRAY_EXTRA
#endif

#if defined(CPUM64)
#define TRIM_ARRAY int8_t trim[4]; int8_t trim_ext:8
#else
#define TRIM_ARRAY int16_t trim[4]
#endif

#if defined(CPUARM)
PACK(typedef struct t_PhaseData {
  TRIM_ARRAY;
  int8_t swtch;       // swtch of phase[0] is not used
  char name[LEN_FP_NAME];
  uint8_t fadeIn;
  uint8_t fadeOut;
  ROTARY_ENCODER_ARRAY;
  PHASE_GVARS_DATA;
}) PhaseData;
#else
PACK(typedef struct t_PhaseData {
  TRIM_ARRAY;
  int8_t swtch;       // swtch of phase[0] is not used
  char name[LEN_FP_NAME];
  uint8_t fadeIn:4;
  uint8_t fadeOut:4;
  ROTARY_ENCODER_ARRAY;
  PHASE_GVARS_DATA;
}) PhaseData;
#endif

#if defined(CPUARM)
  #define MAX_MODELS 60
  #define NUM_CHNOUT 32 // number of real output channels CH1-CH32
  #define MAX_PHASES 9
  #define MAX_MIXERS 64
  #define MAX_EXPOS  32
  #define NUM_CSW    32 // number of custom switches
  #define NUM_CFN    32 // number of functions assigned to switches
#elif defined(PCBGRUVIN9X) || defined(CPUM128)
  #define MAX_MODELS 30
  #define NUM_CHNOUT 16 // number of real output channels CH1-CH16
  #define MAX_PHASES 6
  #define MAX_MIXERS 32
  #define MAX_EXPOS  16
  #define NUM_CSW    15 // number of custom switches
  #define NUM_CFN    24 // number of functions assigned to switches
#else
  #define MAX_MODELS 16
  #define NUM_CHNOUT 16 // number of real output channels CH1-CH16
  #define MAX_PHASES 5
  #define MAX_MIXERS 32
  #define MAX_EXPOS  14
  #define NUM_CSW    12 // number of custom switches
  #define NUM_CFN    16 // number of functions assigned to switches
#endif

#define MAX_TIMERS 2

#if defined(CPUARM)
  #define MAX_CURVES 16
  #define NUM_POINTS 512
  #define CURVTYPE   int16_t
#else
  #define MAX_CURVES 8
  #define NUM_POINTS (112-MAX_CURVES)
  #define CURVTYPE   int8_t
#endif

#define MAX_SWITCH (MAX_PSWITCH+NUM_CSW)

enum SwitchSources {
  SWSRC_NONE = 0,

#if defined(PCBX9D)
  SWSRC_SA0,
  SWSRC_SA1,
  SWSRC_SA2,
  SWSRC_SB0,
  SWSRC_SB1,
  SWSRC_SB2,
  SWSRC_SC0,
  SWSRC_SC1,
  SWSRC_SC2,
  SWSRC_SD0,
  SWSRC_SD1,
  SWSRC_SD2,
  SWSRC_SE0,
  SWSRC_SE1,
  SWSRC_SE2,
  SWSRC_SF0,
  SWSRC_SF2,
  SWSRC_SG0,
  SWSRC_SG1,
  SWSRC_SG2,
  SWSRC_SH0,
  SWSRC_SH2,
#else
  SWSRC_ID0,
  SWSRC_ID1,
  SWSRC_ID2,
#if defined(EXTRA_3POS)
  SWSRC_ID3,
  SWSRC_ID4,
  SWSRC_ID5,
#endif

  SWSRC_THR,
  SWSRC_RUD,
  SWSRC_ELE,
  SWSRC_AIL,
  SWSRC_GEA,
  SWSRC_TRN,
#endif

  SWSRC_SW1,
  SWSRC_SW2,
  SWSRC_SW3,
  SWSRC_SW4,
  SWSRC_SW5,
  SWSRC_SW6,
  SWSRC_SW7,
  SWSRC_SW8,
  SWSRC_SW9,
  SWSRC_SWA,
  SWSRC_SWB,
  SWSRC_SWC,
  SWSRC_LAST_CSW = SWSRC_SW1+NUM_CSW-1,

  SWSRC_ON,

  SWSRC_FIRST_MOMENT_SWITCH,
  SWSRC_LAST_MOMENT_SWITCH = SWSRC_FIRST_MOMENT_SWITCH+SWSRC_ON-1,

#if ROTARY_ENCODERS > 0
  SWSRC_FIRST_ROTENC_SWITCH,
  SWSRC_LAST_ROTENC_SWITCH = SWSRC_FIRST_ROTENC_SWITCH+(2*ROTARY_ENCODERS)-1,
#endif

#if defined(CPUARM)
  SWSRC_FIRST_SHORT_SWITCH,
  SWSRC_LAST_SHORT_SWITCH = SWSRC_FIRST_SHORT_SWITCH + MAX_PSWITCH - 1,
  SWSRC_FIRST_LONG_SWITCH,
  SWSRC_LAST_LONG_SWITCH = SWSRC_FIRST_LONG_SWITCH + MAX_PSWITCH - 1,
#endif

  SWSRC_COUNT,
  SWSRC_FIRST = -SWSRC_LAST_MOMENT_SWITCH,
  SWSRC_LAST = SWSRC_COUNT-1,

  SWSRC_OFF = -SWSRC_ON
};

enum MixSources {
  MIXSRC_NONE,
  MIXSRC_Rud,
  MIXSRC_Ele,
  MIXSRC_Thr,
  MIXSRC_Ail,

  MIXSRC_Pot1,
#if defined(PCBX9D)
  MIXSRC_S1 = MIXSRC_Pot1,
  MIXSRC_S2,
  MIXSRC_S3,
  MIXSRC_S4,
#else
  MIXSRC_P1 = MIXSRC_Pot1,
  MIXSRC_P2,
  #if !defined(EXTRA_3POS)
    MIXSRC_P3,
  #endif
#endif

#if defined(PCBSKY9X)
  MIXSRC_REa,
#elif defined(PCBGRUVIN9X)
  MIXSRC_REa,
  MIXSRC_REb,
  #if ROTARY_ENCODERS > 2
    MIXSRC_REc,
    MIXSRC_REd,
  #endif
#endif

  MIXSRC_MAX,

  MIXSRC_CYC1,
  MIXSRC_CYC2,
  MIXSRC_CYC3,

  MIXSRC_TrimRud,
  MIXSRC_TrimEle,
  MIXSRC_TrimThr,
  MIXSRC_TrimAil,

#if defined(PCBX9D)
  MIXSRC_SA,
  MIXSRC_SB,
  MIXSRC_SC,
  MIXSRC_SD,
  MIXSRC_SE,
  MIXSRC_SF,
  MIXSRC_SG,
  MIXSRC_SH,
#else
  MIXSRC_3POS,
  #if defined(EXTRA_3POS)
    MIXSRC_3POS2,
  #endif
  MIXSRC_THR,
  MIXSRC_RUD,
  MIXSRC_ELE,
  MIXSRC_AIL,
  MIXSRC_GEA,
  MIXSRC_TRN,
#endif
  MIXSRC_SW1,
  MIXSRC_SW9 = MIXSRC_SW1 + 8,
  MIXSRC_SWA,
  MIXSRC_SWB,
  MIXSRC_SWC,
  MIXSRC_LAST_CSW = MIXSRC_SW1+NUM_CSW-1, // TODO same naming convention

  MIXSRC_PPM1,
  MIXSRC_LAST_PPM = MIXSRC_PPM1 + 7,

  MIXSRC_CH1,
  MIXSRC_CH2,
  MIXSRC_CH3,
  MIXSRC_CH4,
  MIXSRC_CH5,
  MIXSRC_CH6,
  MIXSRC_CH7,
  MIXSRC_CH8,
  MIXSRC_CH9,
  MIXSRC_CH10,
  MIXSRC_CH11,
  MIXSRC_CH12,
  MIXSRC_CH13,
  MIXSRC_CH14,
  MIXSRC_CH15,
  MIXSRC_CH16,
  MIXSRC_LAST_CH = MIXSRC_CH1+NUM_CHNOUT-1,

  MIXSRC_GVAR1,
  MIXSRC_LAST_GVAR = MIXSRC_GVAR1+MAX_GVARS-1,

  MIXSRC_FIRST_TELEM,
  MIXSRC_LAST_TELEM = MIXSRC_FIRST_TELEM+NUM_TELEMETRY-1,
};

#define MIN_POINTS 3
#define MAX_POINTS 17

#define TMRMODE_NONE     0
#define TMRMODE_ABS      1
#define TMRMODE_THR      2
#define TMRMODE_THR_REL  3
#define TMRMODE_THR_TRG  4

#if defined(PCBGRUVIN9X) || defined(CPUARM)
PACK(typedef struct t_TimerData {
  int8_t    mode;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint16_t  start:12;
  uint16_t  countdownBeep:1;
  uint16_t  minuteBeep:1;
  uint16_t  persistent:1;
  uint16_t  spare:1;
  uint16_t  value;
}) TimerData;
#else
PACK(typedef struct t_TimerData {
  int8_t    mode;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint16_t  start:12;
  uint16_t  countdownBeep:1;
  uint16_t  minuteBeep:1;
  uint16_t  spare:2;
}) TimerData;
#endif

enum Protocols {
  PROTO_PPM,
#if !defined(CPUARM)
  PROTO_PPM16,
  PROTO_PPMSIM,
#endif
#if defined(PXX) || defined(DSM2) || defined(IRPROTOS)
  PROTO_PXX,
#endif
#if defined(DSM2) || defined(IRPROTOS)
  PROTO_DSM2_LP45,
  PROTO_DSM2_DSM2,
  PROTO_DSM2_DSMX,
#endif
#if defined(IRPROTOS)
  // we will need 4 bytes for proto :(
  PROTO_SILV,
  PROTO_TRAC09,
  PROTO_PICZ,
  PROTO_SWIFT,
#endif
  PROTO_MAX,
  PROTO_NONE
};

#if defined(MAVLINK)
#define TELEMETRY_DATA MavlinkData mavlink;
#elif defined(FRSKY) || !defined(CPUM64)
#define TELEMETRY_DATA FrSkyData frsky;
#else
#define TELEMETRY_DATA
#endif

#if defined(PCBGRUVIN9X) || defined(CPUARM)
#define BeepANACenter uint16_t
#else
#define BeepANACenter uint8_t
#endif

PACK(typedef struct t_ModelData {
  char      name[LEN_MODEL_NAME]; // must be first for eeLoadModelName
  uint8_t   modelId;
  TimerData timers[MAX_TIMERS];
  uint8_t   protocol:3;
  uint8_t   thrTrim:1;            // Enable Throttle Trim
  int8_t    ppmNCH:4;
  uint8_t   trimInc:3;            // Trim Increments
  uint8_t   disableThrottleWarning:1;
  uint8_t   pulsePol:1;
  uint8_t   extendedLimits:1;
  uint8_t   extendedTrims:1;
  uint8_t   spare1:1;
  int8_t    ppmDelay;
  BeepANACenter   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  MixData   mixData[MAX_MIXERS];
  LimitData limitData[NUM_CHNOUT];
  ExpoData  expoData[MAX_EXPOS];
  
  CURVTYPE  curves[MAX_CURVES];
  int8_t    points[NUM_POINTS];
  
  CustomSwData customSw[NUM_CSW];
  CustomFnData  funcSw[NUM_CFN];
  SwashRingData swashR;
  PhaseData phaseData[MAX_PHASES];

  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5ms increments
  uint8_t   thrTraceSrc;
  
  swstate_t switchWarningStates;

  MODEL_GVARS_DATA

  TELEMETRY_DATA

  ROTARY_ENCODER_ARRAY_EXTRA

  MODELDATA_EXTRA

}) ModelData;

extern EEGeneral g_eeGeneral;
extern ModelData g_model;

#define TOTAL_EEPROM_USAGE (sizeof(ModelData)*MAX_MODELS + sizeof(EEGeneral))

#endif
/*eof*/
