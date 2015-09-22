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
 * opentx is based on code named
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

#include "opentx.h"

enum Mix216Sources {
  MIXSRC216_NONE,

#if defined(PCBTARANIS)
  MIXSRC216_FIRST_INPUT,
  MIXSRC216_LAST_INPUT = MIXSRC216_FIRST_INPUT+MAX_INPUTS-1,

  MIXSRC216_FIRST_LUA,
  MIXSRC216_LAST_LUA = MIXSRC216_FIRST_LUA+(MAX_SCRIPTS*MAX_SCRIPT_OUTPUTS)-1,
#endif

  MIXSRC216_Rud,
  MIXSRC216_Ele,
  MIXSRC216_Thr,
  MIXSRC216_Ail,

  MIXSRC216_FIRST_POT,
#if defined(PCBTARANIS)
  MIXSRC216_POT1 = MIXSRC216_FIRST_POT,
  MIXSRC216_POT2,
  MIXSRC216_POT3,
  MIXSRC216_SLIDER1,
  MIXSRC216_SLIDER2,
  MIXSRC216_LAST_POT = MIXSRC216_SLIDER2,
#else
  MIXSRC216_P1 = MIXSRC216_FIRST_POT,
  MIXSRC216_P2,
  MIXSRC216_P3,
  MIXSRC216_LAST_POT = MIXSRC216_P3,
#endif

#if defined(PCBSKY9X)
  MIXSRC216_REa,
  MIXSRC216_LAST_ROTARY_ENCODER = MIXSRC216_REa,
#endif

  MIXSRC216_MAX,

  MIXSRC216_CYC1,
  MIXSRC216_CYC2,
  MIXSRC216_CYC3,

  MIXSRC216_TrimRud,
  MIXSRC216_TrimEle,
  MIXSRC216_TrimThr,
  MIXSRC216_TrimAil,

  MIXSRC216_FIRST_SWITCH,

#if defined(PCBTARANIS)
  MIXSRC216_SA = MIXSRC216_FIRST_SWITCH,
  MIXSRC216_SB,
  MIXSRC216_SC,
  MIXSRC216_SD,
  MIXSRC216_SE,
  MIXSRC216_SF,
  MIXSRC216_SG,
  MIXSRC216_SH,
#else
  MIXSRC216_3POS = MIXSRC216_FIRST_SWITCH,
  MIXSRC216_THR,
  MIXSRC216_RUD,
  MIXSRC216_ELE,
  MIXSRC216_AIL,
  MIXSRC216_GEA,
  MIXSRC216_TRN,
#endif
  MIXSRC216_FIRST_LOGICAL_SWITCH,
  MIXSRC216_SW1 = MIXSRC216_FIRST_LOGICAL_SWITCH,
  MIXSRC216_SW9 = MIXSRC216_SW1 + 8,
  MIXSRC216_SWA,
  MIXSRC216_SWB,
  MIXSRC216_SWC,
  MIXSRC216_LAST_LOGICAL_SWITCH = MIXSRC216_FIRST_LOGICAL_SWITCH+32-1,

  MIXSRC216_FIRST_TRAINER,
  MIXSRC216_LAST_TRAINER = MIXSRC216_FIRST_TRAINER+NUM_TRAINER-1,

  MIXSRC216_FIRST_CH,
  MIXSRC216_CH1 = MIXSRC216_FIRST_CH,
  MIXSRC216_CH2,
  MIXSRC216_CH3,
  MIXSRC216_CH4,
  MIXSRC216_CH5,
  MIXSRC216_CH6,
  MIXSRC216_CH7,
  MIXSRC216_CH8,
  MIXSRC216_CH9,
  MIXSRC216_CH10,
  MIXSRC216_CH11,
  MIXSRC216_CH12,
  MIXSRC216_CH13,
  MIXSRC216_CH14,
  MIXSRC216_CH15,
  MIXSRC216_CH16,
  MIXSRC216_LAST_CH = MIXSRC216_CH1+NUM_CHNOUT-1,

  MIXSRC216_GVAR1,
  MIXSRC216_LAST_GVAR = MIXSRC216_GVAR1+MAX_GVARS-1,

  MIXSRC216_FIRST_TELEM,
};

enum Telemetry216Source {
  TELEM216_NONE,
  TELEM216_TX_VOLTAGE,
  TELEM216_TX_TIME,
  TELEM216_RESERVE1,
  TELEM216_RESERVE2,
  TELEM216_RESERVE3,
  TELEM216_RESERVE4,
  TELEM216_RESERVE5,
  TELEM216_TIMER1,
  TELEM216_TIMER2,
  TELEM216_SWR,
  TELEM216_RSSI_TX,
  TELEM216_RSSI_RX,
  TELEM216_RESERVE0,
  TELEM216_A1,
  TELEM216_A2,
  TELEM216_A3,
  TELEM216_A4,
  TELEM216_ALT,
  TELEM216_RPM,
  TELEM216_FUEL,
  TELEM216_T1,
  TELEM216_T2,
  TELEM216_SPEED,
  TELEM216_DIST,
  TELEM216_GPSALT,
  TELEM216_CELL,
  TELEM216_CELLS_SUM,
  TELEM216_VFAS,
  TELEM216_CURRENT,
  TELEM216_CONSUMPTION,
  TELEM216_POWER,
  TELEM216_ACCx,
  TELEM216_ACCy,
  TELEM216_ACCz,
  TELEM216_HDG,
  TELEM216_VSPEED,
  TELEM216_ASPEED,
  TELEM216_DTE,
  TELEM216_RESERVE6,
  TELEM216_RESERVE7,
  TELEM216_RESERVE8,
  TELEM216_RESERVE9,
  TELEM216_RESERVE10,
  TELEM216_MIN_A1,
  TELEM216_MIN_A2,
  TELEM216_MIN_A3,
  TELEM216_MIN_A4,
  TELEM216_MIN_ALT,
  TELEM216_MAX_ALT,
  TELEM216_MAX_RPM,
  TELEM216_MAX_T1,
  TELEM216_MAX_T2,
  TELEM216_MAX_SPEED,
  TELEM216_MAX_DIST,
  TELEM216_MAX_ASPEED,
  TELEM216_MIN_CELL,
  TELEM216_MIN_CELLS_SUM,
  TELEM216_MIN_VFAS,
  TELEM216_MAX_CURRENT,
  TELEM216_MAX_POWER,
  TELEM216_RESERVE11,
  TELEM216_RESERVE12,
  TELEM216_RESERVE13,
  TELEM216_RESERVE14,
  TELEM216_RESERVE15,
  TELEM216_ACC,
  TELEM216_GPS_TIME,
};

#if defined(PCBTARANIS)
PACK(typedef struct {
  uint8_t  mode;         // 0=end, 1=pos, 2=neg, 3=both
  uint8_t  chn;
  int8_t   swtch;
  uint16_t flightModes;
  int8_t   weight;
  uint8_t  curveMode;
  char     name[LEN_EXPOMIX_NAME];
  uint8_t  spare[2];
  int8_t   curveParam;
}) ExpoData_v215;
PACK(typedef struct {
  uint8_t  srcRaw;
  uint16_t scale;
  uint8_t  chn;
  int8_t   swtch;
  uint16_t flightModes;
  int8_t   weight;
  int8_t   carryTrim:6;
  uint8_t  mode:2;
  char     name[LEN_EXPOMIX_NAME];
  int8_t   offset;
  CurveRef curve;
  uint8_t  spare;
}) ExpoData_v216;
PACK(typedef struct {
  uint32_t srcRaw:10;
  uint32_t scale:14;
  uint32_t chn:8;
  int8_t   swtch;
  uint16_t flightModes;
  int8_t   weight;
  int8_t   carryTrim:6;
  uint8_t  mode:2;
  char     name[LEN_EXPOMIX_NAME];
  int8_t   offset;
  CurveRef curve;
}) ExpoData_v217;
#else
PACK(typedef struct {
  uint8_t  mode;         // 0=end, 1=pos, 2=neg, 3=both
  uint8_t  chn;
  int8_t   swtch;
  uint16_t flightModes;
  int8_t   weight;
  uint8_t  curveMode;
  char     name[LEN_EXPOMIX_NAME];
  int8_t   curveParam;
}) ExpoData_v215;
PACK(typedef struct {
  uint8_t  mode:2;         // 0=end, 1=pos, 2=neg, 3=both
  uint8_t  chn:4;
  uint8_t  curveMode:2;
  int8_t   swtch;
  uint16_t flightModes;
  int8_t   weight;
  char     name[LEN_EXPOMIX_NAME];
  int8_t   curveParam;
}) ExpoData_v216;
typedef ExpoData_v216 ExpoData_v217;
#endif

#if defined(PCBTARANIS)
  #define LIMITDATA_V215_EXTRA  char name[LEN_CHANNEL_NAME];
#else
  #define LIMITDATA_V215_EXTRA
#endif

PACK(typedef struct {
  int8_t  min;
  int8_t  max;
  int8_t  ppmCenter;
  int16_t offset:14;
  uint16_t symetrical:1;
  uint16_t revert:1;
  LIMITDATA_V215_EXTRA
}) LimitData_v215;

#if defined(PCBTARANIS)
PACK(typedef struct {
  int16_t min;
  int16_t max;
  int8_t  ppmCenter;
  int16_t offset:14;
  uint16_t symetrical:1;
  uint16_t revert:1;
  char name[LEN_CHANNEL_NAME];
  int8_t curve;
}) LimitData_v216;
#else
#define LimitData_v216 LimitData
#endif

#if defined(PCBTARANIS)
PACK(typedef struct {
  uint8_t  destCh;
  uint16_t flightModes;
  uint8_t  curveMode:1;       // O=curve, 1=differential
  uint8_t  noExpo:1;
  int8_t   carryTrim:3;
  uint8_t  mltpx:2;           // multiplex method: 0 means +=, 1 means *=, 2 means :=
  uint8_t  spare1:1;
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
  uint8_t  spare2[2];
}) MixData_v215;
PACK(typedef struct {
  uint8_t  destCh;
  uint16_t flightModes;
  uint8_t  mltpx:2;         // multiplex method: 0 means +=, 1 means *=, 2 means :=
  uint8_t  carryTrim:1;
  uint8_t  spare1:5;
  int16_t  weight;
  int8_t   swtch;
  CurveRef curve;
  uint8_t  mixWarn:4;       // mixer warning
  uint8_t  spare2:4;
  uint8_t  delayUp;
  uint8_t  delayDown;
  uint8_t  speedUp;
  uint8_t  speedDown;
  uint8_t  srcRaw;
  int16_t  offset;
  char     name[LEN_EXPOMIX_NAME];
  uint8_t  spare3;
}) MixData_v216;
PACK(typedef struct {
  uint8_t  destCh;
  uint16_t flightModes:9;
  uint16_t mltpx:2;         // multiplex method: 0 means +=, 1 means *=, 2 means :=
  uint16_t carryTrim:1;
  uint16_t mixWarn:4;       // mixer warning
  int16_t  weight;
  uint32_t srcRaw:10;
  int32_t  offset:14;
  int32_t  swtch:8;
  CurveRef curve;
  uint8_t  delayUp;
  uint8_t  delayDown;
  uint8_t  speedUp;
  uint8_t  speedDown;
  char     name[LEN_EXPOMIX_NAME];
}) MixData_v217;
#else
PACK(typedef struct {
  uint8_t  destCh;
  uint16_t flightModes;
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
}) MixData_v215;
PACK(typedef struct {
  uint8_t  destCh:5;
  uint8_t  mixWarn:3;         // mixer warning
  uint16_t flightModes;
  uint8_t  curveMode:1;
  uint8_t  noExpo:1;
  int8_t   carryTrim:3;
  uint8_t  mltpx:2;           // multiplex method: 0 means +=, 1 means *=, 2 means :=
  uint8_t  spare:1;
  int16_t  weight;
  int8_t   swtch;
  int8_t   curveParam;
  uint8_t  delayUp;
  uint8_t  delayDown;
  uint8_t  speedUp;
  uint8_t  speedDown;
  uint8_t  srcRaw;
  int16_t  offset;
  char     name[LEN_EXPOMIX_NAME];
}) MixData_v216;
typedef MixData MixData_v217;
#endif

PACK(typedef struct {
  int8_t    mode;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint16_t  start:12;
  uint16_t  countdownBeep:1;
  uint16_t  minuteBeep:1;
  uint16_t  persistent:1;
  uint16_t  spare:1;
  uint16_t  value;
}) TimerData_v215;

PACK(typedef struct {
  int8_t   mode;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint16_t start;
  uint8_t  countdownBeep:2;
  uint8_t  minuteBeep:1;
  uint8_t  persistent:2;
  uint8_t  spare:3;
  uint16_t value;
}) TimerData_v216;

PACK(typedef struct {
  int32_t  mode:8;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint32_t start:24;
  int32_t  value:24;
  uint32_t countdownBeep:2;
  uint32_t minuteBeep:1;
  uint32_t persistent:2;
  uint32_t spare:3;
  char     name[LEN_TIMER_NAME];
}) TimerData_v217;

PACK(typedef struct {
  int16_t trim[4];
  int8_t swtch;       // swtch of phase[0] is not used
  char name[LEN_FLIGHT_MODE_NAME];
  uint8_t fadeIn;
  uint8_t fadeOut;
  int16_t rotaryEncoders[1];
  gvar_t gvars[9];
}) FlightModeData_v215;

PACK(typedef struct {
  int16_t v1;
  int16_t v2;
  uint8_t func;
  uint8_t delay;
  uint8_t duration;
  int8_t  andsw;
}) LogicalSwitchData_v215;

PACK(typedef struct { // Logical Switches data
  int8_t  v1;
  int16_t v2;
  int16_t v3;
  uint8_t func;
  uint8_t delay;
  uint8_t duration;
  int8_t  andsw;
}) LogicalSwitchData_v216;

PACK(typedef struct { // Logical Switches data
  uint16_t func:6;
  int16_t  v1:10;
  int16_t  v2;
  int16_t  v3;
  uint8_t  delay;
  uint8_t  duration;
  int8_t   andsw;
}) LogicalSwitchData_v217;

#if defined(PCBTARANIS)
PACK(typedef struct {
  int8_t  swtch;
  uint8_t func;
  PACK(union {
    char name[10];
    struct {
      int16_t val;
      int16_t ext1;
      int16_t ext2;
    } composite;
  }) param;
  uint8_t mode:2;
  uint8_t active:6;
}) CustomFunctionData_v215;
PACK(typedef struct {
  int8_t  swtch;
  uint8_t func;
  PACK(union {
    PACK(struct {
      char name[8];
    }) play;

    PACK(struct {
      int16_t val;
      uint8_t mode;
      uint8_t param;
      int32_t spare2;
    }) all;

    PACK(struct {
      int32_t val1;
      int32_t val2;
    }) clear;
  });
  uint8_t active;
}) CustomFunctionData_v216;
#else
PACK(typedef struct {
  int8_t  swtch;
  uint8_t func;
  PACK(union {
    char name[6];
    struct {
      int16_t val;
      int16_t ext1;
      int16_t ext2;
    } composite;
  }) param;
  uint8_t mode:2;
  uint8_t active:6;
}) CustomFunctionData_v215;
PACK(typedef struct {
  int8_t  swtch;
  uint8_t func;
  PACK(union {
    PACK(struct {
      char name[6];
    }) play;

    PACK(struct {
      int16_t val;
      uint8_t mode;
      uint8_t param;
      int16_t spare2;
    }) all;

    PACK(struct {
      int32_t val1;
      int16_t val2;
    }) clear;
  });
  uint8_t active;
}) CustomFunctionData_v216;
#endif

PACK(typedef struct {
  uint8_t    source;
  uint8_t    barMin;           // minimum for bar display
  uint8_t    barMax;           // ditto for max display (would usually = ratio)
}) FrSkyBarData_v215;

PACK(typedef struct {
  uint8_t    sources[NUM_LINE_ITEMS];
}) FrSkyLineData_v215;

typedef union {
  FrSkyBarData_v215  bars[4];
  FrSkyLineData_v215 lines[4];
} FrSkyScreenData_v215;

PACK(typedef struct {
  FrSkyChannelData channels[2];
  uint8_t usrProto; // Protocol in FrSky user data, 0=None, 1=FrSky hub, 2=WS HowHigh, 3=Halcyon
  uint8_t voltsSource;
  uint8_t blades;   // How many blades for RPMs, 0=2 blades, 1=3 blades
  uint8_t currentSource;
  uint8_t screensType;
  FrSkyScreenData_v215 screens[3];
  uint8_t varioSource;
  int8_t  varioCenterMax;
  int8_t  varioCenterMin;
  int8_t  varioMin;
  int8_t  varioMax;
  FrSkyRSSIAlarm rssiAlarms[2];
}) FrSkyData_v215;

PACK(typedef struct {
  FrSkyChannelData channels[4];
  uint8_t usrProto; // Protocol in FrSky user data, 0=None, 1=FrSky hub, 2=WS HowHigh, 3=Halcyon
  uint8_t voltsSource:7;
  uint8_t altitudeDisplayed:1;
  int8_t blades;    // How many blades for RPMs, 0=2 blades
  uint8_t currentSource;
  uint8_t screensType; // 2bits per screen (None/Gauges/Numbers/Script)
  FrSkyScreenData_v215 screens[3];
  uint8_t varioSource;
  int8_t  varioCenterMax;
  int8_t  varioCenterMin;
  int8_t  varioMin;
  int8_t  varioMax;
  FrSkyRSSIAlarm rssiAlarms[2];
  uint16_t mAhPersistent:1;
  uint16_t storedMah:15;
  int8_t   fasOffset;
}) FrSkyData_v216;

PACK(typedef struct {
  char    file[10];
  char    name[10];
  int8_t  inputs[10];
}) ScriptData_v216;

PACK(typedef struct { // Swash Ring data
  uint8_t   invertELE:1;
  uint8_t   invertAIL:1;
  uint8_t   invertCOL:1;
  uint8_t   type:5;
  uint8_t   collectiveSource;
  uint8_t   value;
}) SwashRingData_v215;

PACK(typedef struct {
  int8_t  rfProtocol;
  uint8_t channelsStart;
  int8_t  channelsCount; // 0=8 channels
  uint8_t failsafeMode;
  int16_t failsafeChannels[NUM_CHNOUT];
  int8_t  ppmDelay;
  int8_t  ppmFrameLength;
  uint8_t ppmPulsePol;
}) ModuleData_v216;

#if defined(PCBTARANIS)
#define MODELDATA_EXTRA_216 \
  uint8_t externalModule; \
  uint8_t trainerMode; \
  ModuleData_v216 moduleData[NUM_MODULES+1]; \
  char curveNames[MAX_CURVES][6]; \
  ScriptData_v216 scriptsData[MAX_SCRIPTS]; \
  char inputNames[MAX_INPUTS][LEN_INPUT_NAME]; \
  uint8_t nPotsToWarn; \
  int8_t potPosition[NUM_POTS]; \
  uint8_t spare[2];
#elif defined(PCBSKY9X)
#define MODELDATA_EXTRA_216 \
  uint8_t externalModule; \
  ModuleData_v216 moduleData[NUM_MODULES+1]; \
  uint8_t nPotsToWarn; \
  int8_t potPosition[NUM_POTS]; \
  uint8_t rxBattAlarms[2];
#endif

#if defined(PCBTARANIS)
PACK(typedef struct {
  char name[LEN_MODEL_NAME];
  uint8_t modelId;
  char bitmap[LEN_BITMAP_NAME];
}) ModelHeader_v216;
#else
PACK(typedef struct {
  char      name[LEN_MODEL_NAME];
  uint8_t   modelId;
}) ModelHeader_v216;
#endif

PACK(typedef struct {
  ModelHeader_v216 header;
  TimerData_v216 timers[2];
  AVR_FIELD(uint8_t   protocol:3)
  ARM_FIELD(uint8_t   telemetryProtocol:3)
  uint8_t   thrTrim:1;            // Enable Throttle Trim
  AVR_FIELD(int8_t    ppmNCH:4)
  ARM_FIELD(int8_t    spare2:4)
  int8_t    trimInc:3;            // Trim Increments
  uint8_t   disableThrottleWarning:1;
  ARM_FIELD(uint8_t displayChecklist:1)
  AVR_FIELD(uint8_t pulsePol:1)
  uint8_t   extendedLimits:1;
  uint8_t   extendedTrims:1;
  uint8_t   throttleReversed:1;
  AVR_FIELD(int8_t ppmDelay)
  BeepANACenter beepANACenter;        // 1<<0->A1.. 1<<6->A7
  MixData_v216 mixData[MAX_MIXERS];
  LimitData_v216 limitData[NUM_CHNOUT];
  ExpoData_v216  expoData[MAX_EXPOS];

  CURVDATA  curves[MAX_CURVES];
  int8_t    points[NUM_POINTS];

  LogicalSwitchData_v216 logicalSw[32];
  CustomFunctionData_v216 customFn[NUM_CFN];
  SwashRingData_v215 swashR;
  FlightModeData_v215 flightModeData[MAX_FLIGHT_MODES];

  uint8_t   thrTraceSrc;

  uint16_t switchWarningState;
  uint8_t  switchWarningEnable;

  global_gvar_t gvars[MAX_GVARS];

  FrSkyData_v216 frsky;

  MODELDATA_EXTRA_216

}) ModelData_v216;

PACK(typedef struct {
  ModelHeader header;
  TimerData_v217 timers[MAX_TIMERS];
  ARM_FIELD(uint8_t   telemetryProtocol:3)
  uint8_t   thrTrim:1;            // Enable Throttle Trim
  ARM_FIELD(uint8_t   noGlobalFunctions:1)
  ARM_FIELD(uint8_t   displayTrims:2)
  ARM_FIELD(uint8_t   ignoreSensorIds:1)
  int8_t    trimInc:3;            // Trim Increments
  uint8_t   disableThrottleWarning:1;
  ARM_FIELD(uint8_t displayChecklist:1)
  uint8_t   extendedLimits:1;
  uint8_t   extendedTrims:1;
  uint8_t   throttleReversed:1;
  BeepANACenter beepANACenter;
  MixData_v217 mixData[MAX_MIXERS];
  LimitData limitData[NUM_CHNOUT];
  ExpoData_v217  expoData[MAX_EXPOS];

  CURVDATA  curves[MAX_CURVES];
  int8_t    points[NUM_POINTS];

  LogicalSwitchData_v217 logicalSw[32];
  CustomFunctionData_v216 customFn[NUM_CFN];
  SwashRingData swashR;
  FlightModeData_v215 flightModeData[MAX_FLIGHT_MODES];

  uint8_t thrTraceSrc;

  swarnstate_t  switchWarningState;
  swarnenable_t switchWarningEnable;

  MODEL_GVARS_DATA

  TELEMETRY_DATA

  MODELDATA_EXTRA

  TelemetrySensor telemetrySensors[MAX_SENSORS];

  TARANIS_REV9E_FIELD(uint8_t topLcdTimer)
}) ModelData_v217;

#if defined(PCBTARANIS)
  #define NUM_POTS_215 4
#else
  #define NUM_POTS_215 3
#endif

PACK(typedef struct {
  uint8_t   version;
  uint16_t  variant;
  int16_t   calibMid[NUM_STICKS+NUM_POTS_215];
  int16_t   calibSpanNeg[NUM_STICKS+NUM_POTS_215];
  int16_t   calibSpanPos[NUM_STICKS+NUM_POTS_215];
  uint16_t  chkSum;
  int8_t    currModel;
  uint8_t   contrast;
  uint8_t   vBatWarn;
  int8_t    txVoltageCalibration;
  int8_t    backlightMode;
  TrainerData trainer;
  uint8_t   view;            // index of view in main screen
  int8_t    buzzerMode:2;    // -2=quiet, -1=only alarms, 0=no keys, 1=all
  uint8_t   fai:1;
  int8_t    beepMode:2;      // -2=quiet, -1=only alarms, 0=no keys, 1=all
  uint8_t   alarmsFlash:1;
  uint8_t   disableMemoryWarning:1;
  uint8_t   disableAlarmWarning:1;
  uint8_t   stickMode:2;
  int8_t    timezone:5;
  uint8_t   spare1:1;
  uint8_t   inactivityTimer;
  uint8_t   mavbaud:3;
  SPLASH_MODE; /* 3bits */
  int8_t    hapticMode:2;    // -2=quiet, -1=only alarms, 0=no keys, 1=all
  uint8_t   blOffBright:4;
  uint8_t   blOnBright:4;
  uint8_t   lightAutoOff;
  uint8_t   templateSetup;   // RETA order for receiver channels
  int8_t    PPM_Multiplier;
  int8_t    hapticLength;
  uint8_t   reNavigation;    // not used on STOCK board
  int8_t    beepLength:3;
  uint8_t   hapticStrength:3;
  uint8_t   gpsFormat:1;
  uint8_t   unexpectedShutdown:1;
  uint8_t   speakerPitch;
  int8_t    speakerVolume;
  int8_t    vBatMin;
  int8_t    vBatMax;
  uint8_t   backlightBright;
  int8_t    txCurrentCalibration;
  int8_t    temperatureWarn;
  uint8_t   mAhWarn;
  uint16_t  mAhUsed;
  uint32_t  globalTimer;
  int8_t    temperatureCalib;
  uint8_t   btBaudrate;
  uint8_t   optrexDisplay;
  uint8_t   sticksGain;
  uint8_t   rotarySteps;
  uint8_t   countryCode;
  uint8_t   imperial;
  char      ttsLanguage[2];
  int8_t    beepVolume;
  int8_t    wavVolume;
  int8_t    varioVolume;
  int8_t    backgroundVolume;
}) GeneralSettings_v215;

void ConvertGeneralSettings_215_to_216(EEGeneral &settings)
{
  GeneralSettings_v215 oldSettings;
  memcpy(&oldSettings, &settings, sizeof(oldSettings));

  settings.version = 216;
  for (int i=0, j=0; i<NUM_STICKS+NUM_POTS; i++) {
    settings.calib[i].mid = oldSettings.calibMid[j];
    settings.calib[i].spanNeg = oldSettings.calibSpanNeg[j];
    settings.calib[i].spanPos = oldSettings.calibSpanPos[j];
#if defined(PCBTARANIS)
    if (i==POT3) continue;
#endif
    j++;
  }
  settings.chkSum = evalChkSum();

  memcpy(&settings.currModel, &oldSettings.currModel, sizeof(GeneralSettings_v215)-offsetof(GeneralSettings_v215, currModel));

  settings.hapticStrength = 0; // Haptic strength reset
}

void ConvertGeneralSettings_216_to_217(EEGeneral & settings)
{
  settings.version = 217;
#if defined(PCBTARANIS)
  settings.potsConfig = 0x05; // S1 and S2 = pots with detent
  settings.switchConfig = 0x00007bff; // 6x3POS, 1x2POS, 1xTOGGLE
#endif
}

void ConvertGeneralSettings_217_to_218(EEGeneral & settings)
{
  settings.version = 218;
}

int ConvertTelemetrySource_215_to_216(int source)
{
  // TELEM_TX_TIME and 5 spare added
  if (source >= TELEM216_TX_TIME)
    source += 6;
  // TELEM_RSSI_TX added
  if (source >= TELEM216_RSSI_TX)
    source += 1;
  // Reserve added
  if (source >= TELEM216_RESERVE0)
    source += 1;
  // A3 and A4 added
  if (source >= TELEM216_A3)
    source += 2;
  // ASpd and dTE added + 5 reserve
  if (source >= TELEM216_ASPEED)
    source += 7;
  // A3 and A4 MIN added
  if (source >= TELEM216_MIN_A3)
    source += 2;
  // ASpd+ Cel- Cels- and Vfas- added
  if (source >= TELEM216_MAX_ASPEED)
    source += 4;
  // 5 reserve added
  if (source >= TELEM216_RESERVE6)
    source += 5;

  return source;
}

int ConvertTelemetrySource_216_to_217(int source)
{
  // TELEM_TIMER3 added
//  if (source >= TELEM_TIMER3)
  //  source += 1;

  return source;
}

#if defined(PCBTARANIS)
int ConvertSource_215_to_216(int source, bool insertZero=false)
{
  if (insertZero)
    source += 1;
  // Virtual Inputs and Lua Outputs added
  if (source > 0)
    source += MAX_INPUTS + MAX_SCRIPTS*MAX_SCRIPT_OUTPUTS;
  // S3 added
  if (source > MIXSRC216_POT2)
    source += 1;
  // PPM9-PPM16 added
  if (source > MIXSRC216_FIRST_TRAINER+7)
    source += 8;
  // 4 GVARS added
  if (source > MIXSRC216_GVAR1+4)
    source += 4;
  // Telemetry conversions
  if (source >= MIXSRC216_FIRST_TELEM)
    source = MIXSRC216_FIRST_TELEM + ConvertTelemetrySource_215_to_216(source-MIXSRC216_FIRST_TELEM+1) - 1;

  return source;
}

int ConvertSwitch_215_to_216(int swtch)
{
  if (swtch < 0)
    return -ConvertSwitch_215_to_216(-swtch);
  else if (swtch <= SWSRC_LAST_SWITCH)
    return swtch;
  else if (swtch > SWSRC_LAST_SWITCH + 32 + 1) {
    swtch -= (SWSRC_LAST_SWITCH + 32 + 1);
    if (swtch > SWSRC_ON)
      swtch = 0;
    return swtch;
  }
  else {
    swtch += (2*4) + (3*6); // 4 trims and 2 * 6-pos added as switches
    return swtch;
  }
}
#else
int ConvertSource_215_to_216(int source, bool insertZero=false)
{
  if (insertZero)
    source += 1;
  // PPM9-PPM16 added
  if (source > MIXSRC216_FIRST_TRAINER+7)
    source += 8;
  // 4 GVARS added
  if (source > MIXSRC216_GVAR1+4)
    source += 4;
  // Telemetry conversions
  if (source >= MIXSRC216_FIRST_TELEM)
    source = MIXSRC216_FIRST_TELEM + ConvertTelemetrySource_215_to_216(source-MIXSRC216_FIRST_TELEM+1) - 1;

  return source;
}

int ConvertSwitch_215_to_216(int swtch)
{
  if (swtch < 0)
    return -ConvertSwitch_215_to_216(-swtch);
  else if (swtch <= SWSRC_LAST_SWITCH)
    return swtch;
  else {
    swtch += (2*4) + 1; // 4 trims and REa added
    if (swtch > SWSRC_ON)
      swtch -= (9+32+1);
    if (swtch > SWSRC_ON)
      swtch = SWSRC_ON;
    return swtch;
  }
}
#endif

#if defined(PCBTARANIS)
int ConvertSwitch_216_to_217(int swtch)
{
  if (swtch < 0)
    return -ConvertSwitch_216_to_217(-swtch);

  if (swtch > SWSRC_SF0)
    swtch += 1;

  if (swtch > SWSRC_SH0)
    swtch += 1;

  return swtch;
}
#else
int ConvertSwitch_216_to_217(int swtch)
{
  return swtch;
}
#endif

int ConvertSwitch_217_to_218(int swtch)
{
  // 32 additional logical switches

  if (swtch < 0)
    return -ConvertSwitch_217_to_218(-swtch);

  if (swtch >= SWSRC_FIRST_LOGICAL_SWITCH+32)
    return swtch+32;

  return swtch;
}

int ConvertSource_216_to_217(int source)
{
#if defined(PCBTARANIS) && defined(REV9E)
  // SI to SR switches added
  if (source >= MIXSRC_SI)
    source += 10;
#endif
  // Telemetry conversions
  if (source >= MIXSRC_FIRST_TELEM)
    source = 0;

  return source;
}

int16_t ConvertGVAR_215_to_216(int16_t var)
{
  if (var <= -508)
    return var + 512 - 4096;
  else if (var >= 507)
    return var - 512 + 4096;
  else
    return var;
}

int ConvertGVar_216_to_217(int value)
{
  if (value < -4096 + 9)
    value += 4096 - 1024;
  else if (value > 4095 - 9)
    value -= 4095 - 1023;
  return value;
}

void ConvertModel_216_to_217(ModelData &model)
{
  // Timer3 added
  // 32bits Timers
  // MixData reduction
  // PPM center range
  // Telemetry custom screens

  assert(sizeof(ModelData_v216) <= sizeof(ModelData));

  ModelData_v216 oldModel;
  memcpy(&oldModel, &model, sizeof(oldModel));
  ModelData_v217 & newModel = (ModelData_v217 &)model;
  memset(&newModel, 0, sizeof(ModelData_v217));

  char name[LEN_MODEL_NAME+1];
  zchar2str(name, oldModel.header.name, LEN_MODEL_NAME);
  TRACE("Model %s conversion from v216 to v217", name);

  newModel.header.modelId[0] = oldModel.header.modelId;
  memcpy(newModel.header.name, oldModel.header.name, LEN_MODEL_NAME);
#if defined(PCBTARANIS)
  memcpy(newModel.header.bitmap, oldModel.header.bitmap, LEN_BITMAP_NAME);
#endif

  for (uint8_t i=0; i<2; i++) {
    TimerData_v217 & timer = newModel.timers[i];
    if (oldModel.timers[i].mode >= TMRMODE_COUNT)
      timer.mode = TMRMODE_COUNT + ConvertSwitch_216_to_217(oldModel.timers[i].mode - TMRMODE_COUNT + 1) - 1;
    else
      timer.mode = ConvertSwitch_216_to_217(oldModel.timers[i].mode);
    timer.start = oldModel.timers[i].start;
    timer.countdownBeep = oldModel.timers[i].countdownBeep;
    timer.minuteBeep = oldModel.timers[i].minuteBeep;
    timer.persistent = oldModel.timers[i].persistent;
    timer.value = oldModel.timers[i].value;
  }
  newModel.telemetryProtocol = oldModel.telemetryProtocol;
  newModel.thrTrim = oldModel.thrTrim;
  newModel.trimInc = oldModel.trimInc;
  newModel.disableThrottleWarning = oldModel.disableThrottleWarning;
  newModel.displayChecklist = oldModel.displayChecklist;
  newModel.extendedLimits = oldModel.extendedLimits;
  newModel.extendedTrims = oldModel.extendedTrims;
  newModel.throttleReversed = oldModel.throttleReversed;
  newModel.beepANACenter = oldModel.beepANACenter;
  for (int i=0; i<MAX_MIXERS; i++) {
    newModel.mixData[i].destCh = oldModel.mixData[i].destCh;
    newModel.mixData[i].flightModes = oldModel.mixData[i].flightModes;
    newModel.mixData[i].mltpx = oldModel.mixData[i].mltpx;
    newModel.mixData[i].carryTrim = oldModel.mixData[i].carryTrim;
    newModel.mixData[i].mixWarn = oldModel.mixData[i].mixWarn;
    newModel.mixData[i].weight = ConvertGVar_216_to_217(oldModel.mixData[i].weight);
    newModel.mixData[i].swtch = ConvertSwitch_216_to_217(oldModel.mixData[i].swtch);
#if defined(PCBTARANIS)
    newModel.mixData[i].curve = oldModel.mixData[i].curve;
#else
    newModel.mixData[i].curveMode = oldModel.mixData[i].curveMode;
    newModel.mixData[i].noExpo = oldModel.mixData[i].noExpo;
    newModel.mixData[i].curveParam = oldModel.mixData[i].curveParam;
#endif
    newModel.mixData[i].delayUp = oldModel.mixData[i].delayUp;
    newModel.mixData[i].delayDown = oldModel.mixData[i].delayDown;
    newModel.mixData[i].speedUp = oldModel.mixData[i].speedUp;
    newModel.mixData[i].speedDown = oldModel.mixData[i].speedDown;
    newModel.mixData[i].srcRaw = ConvertSource_216_to_217(oldModel.mixData[i].srcRaw);
    newModel.mixData[i].offset = ConvertGVar_216_to_217(oldModel.mixData[i].offset);
    memcpy(newModel.mixData[i].name, oldModel.mixData[i].name, sizeof(newModel.mixData[i].name));
  }
  for (int i=0; i<NUM_CHNOUT; i++) {
#if defined(PCBTARANIS)
    newModel.limitData[i].min = ConvertGVar_216_to_217(oldModel.limitData[i].min);
    newModel.limitData[i].max = ConvertGVar_216_to_217(oldModel.limitData[i].max);
    newModel.limitData[i].offset = ConvertGVar_216_to_217(oldModel.limitData[i].offset);
    newModel.limitData[i].ppmCenter = oldModel.limitData[i].ppmCenter;
    newModel.limitData[i].symetrical = oldModel.limitData[i].symetrical;
    newModel.limitData[i].revert = oldModel.limitData[i].revert;
    newModel.limitData[i].curve = oldModel.limitData[i].curve;
    memcpy(newModel.limitData[i].name, oldModel.limitData[i].name, sizeof(newModel.limitData[i].name));
#else
    newModel.limitData[i] = oldModel.limitData[i];
#endif
  }
  for (int i=0; i<MAX_EXPOS; i++) {
#if defined(PCBTARANIS)
    newModel.expoData[i].srcRaw = ConvertSource_216_to_217(oldModel.expoData[i].srcRaw);
    newModel.expoData[i].scale = oldModel.expoData[i].scale;
    newModel.expoData[i].carryTrim = oldModel.expoData[i].carryTrim;
    newModel.expoData[i].curve = oldModel.expoData[i].curve;
    newModel.expoData[i].offset = oldModel.expoData[i].offset;
#else
    newModel.expoData[i].curveMode = oldModel.expoData[i].curveMode;
    newModel.expoData[i].curveParam = oldModel.expoData[i].curveParam;
#endif
    newModel.expoData[i].chn = oldModel.expoData[i].chn;
    newModel.expoData[i].swtch = ConvertSwitch_216_to_217(oldModel.expoData[i].swtch);
    newModel.expoData[i].flightModes = oldModel.expoData[i].flightModes;
    newModel.expoData[i].weight = oldModel.expoData[i].weight;
    newModel.expoData[i].mode = oldModel.expoData[i].mode;
    memcpy(newModel.expoData[i].name, oldModel.expoData[i].name, sizeof(newModel.expoData[i].name));
  }
  memcpy(newModel.curves, oldModel.curves, sizeof(newModel.curves));
  memcpy(newModel.points, oldModel.points, sizeof(newModel.points));
  for (int i=0; i<32; i++) {
    LogicalSwitchData_v217 & sw = newModel.logicalSw[i];
    sw.func = oldModel.logicalSw[i].func;
    sw.v1 = oldModel.logicalSw[i].v1;
    sw.v2 = oldModel.logicalSw[i].v2;
    sw.v3 = oldModel.logicalSw[i].v3;
    sw.delay = oldModel.logicalSw[i].delay;
    sw.duration = oldModel.logicalSw[i].duration;
    sw.andsw = oldModel.logicalSw[i].andsw;
    uint8_t cstate = lswFamily(sw.func);
    if (cstate == LS_FAMILY_OFS || cstate == LS_FAMILY_COMP || cstate == LS_FAMILY_DIFF) {
      sw.v1 = ConvertSource_216_to_217((uint8_t)sw.v1);
      if (cstate == LS_FAMILY_COMP) {
        sw.v2 = ConvertSource_216_to_217((uint8_t)sw.v2);
      }
    }
    else if (cstate == LS_FAMILY_BOOL || cstate == LS_FAMILY_STICKY) {
      sw.v1 = ConvertSwitch_216_to_217(sw.v1);
      sw.v2 = ConvertSwitch_216_to_217(sw.v2);
    }
    else if (cstate == LS_FAMILY_EDGE) {
      sw.v1 = ConvertSwitch_216_to_217(sw.v1);
    }
    sw.andsw = ConvertSwitch_216_to_217(sw.andsw);
  }
  for (int i=0; i<NUM_CFN; i++) {
    CustomFunctionData_v216 & fn = newModel.customFn[i];
    fn = oldModel.customFn[i];
    fn.swtch = ConvertSwitch_216_to_217(fn.swtch);
    if (fn.func == FUNC_PLAY_VALUE || fn.func == FUNC_VOLUME || (IS_ADJUST_GV_FUNC(fn.func) && fn.all.mode == FUNC_ADJUST_GVAR_SOURCE)) {
      fn.all.val = ConvertSource_216_to_217(fn.all.val);
    }
  }

  newModel.swashR.collectiveSource = ConvertSource_216_to_217(newModel.swashR.collectiveSource);
  // TODO other fields

  for (int i=0; i<MAX_FLIGHT_MODES; i++) {
    newModel.flightModeData[i] = oldModel.flightModeData[i];
    newModel.flightModeData[i].swtch = ConvertSwitch_216_to_217(oldModel.flightModeData[i].swtch);
  }

  newModel.thrTraceSrc = oldModel.thrTraceSrc;
  newModel.switchWarningState = oldModel.switchWarningState;
  newModel.switchWarningEnable = oldModel.switchWarningEnable;
  memcpy(newModel.gvars, oldModel.gvars, sizeof(newModel.gvars));

  memcpy(&newModel.frsky.rssiAlarms, &oldModel.frsky.rssiAlarms, sizeof(newModel.frsky.rssiAlarms));

  for (int i=0; i<NUM_MODULES+1; i++) {
    newModel.moduleData[i].type = 0;
    newModel.moduleData[i].rfProtocol = oldModel.moduleData[i].rfProtocol;
    newModel.moduleData[i].channelsStart = oldModel.moduleData[i].channelsStart;
    newModel.moduleData[i].channelsCount = oldModel.moduleData[i].channelsCount;
    newModel.moduleData[i].failsafeMode = oldModel.moduleData[i].failsafeMode + 1;
    for (int j=0; j<NUM_CHNOUT; j++) {
      newModel.moduleData[i].failsafeChannels[j] = oldModel.moduleData[i].failsafeChannels[j];
    }
    newModel.moduleData[i].ppmDelay = oldModel.moduleData[i].ppmDelay;
    newModel.moduleData[i].ppmFrameLength = oldModel.moduleData[i].ppmFrameLength;
    newModel.moduleData[i].ppmPulsePol = oldModel.moduleData[i].ppmPulsePol;
  }

#if defined(PCBTARANIS)
  newModel.moduleData[INTERNAL_MODULE].type = MODULE_TYPE_XJT;
#endif
  newModel.moduleData[EXTERNAL_MODULE].type = oldModel.externalModule;

#if defined(PCBTARANIS)
  newModel.trainerMode = oldModel.trainerMode;
  // TODO memcpy(newModel.scriptsData, oldModel.scriptsData, sizeof(newModel.scriptsData));
  memcpy(newModel.curveNames, oldModel.curveNames, sizeof(newModel.curveNames));
  memcpy(newModel.inputNames, oldModel.inputNames, sizeof(newModel.inputNames));
#endif
  newModel.potsWarnMode = oldModel.nPotsToWarn >> 6;
  newModel.potsWarnEnabled = oldModel.nPotsToWarn & 0x1f;
  memcpy(newModel.potsWarnPosition, oldModel.potPosition, sizeof(newModel.potsWarnPosition));
}

void ConvertModel_217_to_218(ModelData & model)
{
  assert(sizeof(ModelData_v217) <= sizeof(ModelData));

  ModelData_v217 oldModel;
  memcpy(&oldModel, &model, sizeof(oldModel));
  ModelData & newModel = model;
  memset(&newModel, 0, sizeof(ModelData));

  char name[LEN_MODEL_NAME+1];
  zchar2str(name, oldModel.header.name, LEN_MODEL_NAME);
  TRACE("Model %s conversion from v217 to v218", name);

  newModel.header = oldModel.header;
  for (uint8_t i=0; i<MAX_TIMERS; i++) {
    if (oldModel.timers[i].mode >= TMRMODE_COUNT)
      newModel.timers[i].mode = TMRMODE_COUNT + ConvertSwitch_217_to_218(oldModel.timers[i].mode - TMRMODE_COUNT + 1) - 1;
    else
      newModel.timers[i].mode = ConvertSwitch_217_to_218(oldModel.timers[i].mode);
    if (oldModel.timers[i].mode)
      TRACE("timer mode %d => %d", oldModel.timers[i].mode, newModel.timers[i].mode);
    newModel.timers[i].start = oldModel.timers[i].start;
    newModel.timers[i].value = oldModel.timers[i].value;
    newModel.timers[i].countdownBeep = oldModel.timers[i].countdownBeep;
    newModel.timers[i].minuteBeep = oldModel.timers[i].minuteBeep;
    newModel.timers[i].persistent = oldModel.timers[i].persistent;
    memcpy(newModel.timers[i].name, oldModel.timers[i].name, sizeof(newModel.timers[i].name));
  }
  newModel.telemetryProtocol = oldModel.telemetryProtocol;
  newModel.thrTrim = oldModel.thrTrim;
  newModel.noGlobalFunctions = oldModel.noGlobalFunctions;
  newModel.displayTrims = oldModel.displayTrims;
  newModel.ignoreSensorIds = oldModel.ignoreSensorIds;
  newModel.trimInc = oldModel.trimInc;
  newModel.disableThrottleWarning = oldModel.disableThrottleWarning;
  newModel.displayChecklist = oldModel.displayChecklist;
  newModel.extendedLimits = oldModel.extendedLimits;
  newModel.extendedTrims = oldModel.extendedTrims;
  newModel.throttleReversed = oldModel.throttleReversed;
  newModel.beepANACenter = oldModel.beepANACenter;
  for (int i=0; i<MAX_MIXERS; i++) {
    newModel.mixData[i].destCh = oldModel.mixData[i].destCh;
    newModel.mixData[i].flightModes = oldModel.mixData[i].flightModes;
    newModel.mixData[i].mltpx = oldModel.mixData[i].mltpx;
    newModel.mixData[i].carryTrim = oldModel.mixData[i].carryTrim;
    newModel.mixData[i].mixWarn = oldModel.mixData[i].mixWarn;
    newModel.mixData[i].weight = oldModel.mixData[i].weight;
    newModel.mixData[i].swtch = ConvertSwitch_217_to_218(oldModel.mixData[i].swtch);
#if defined(PCBTARANIS)
    newModel.mixData[i].curve = oldModel.mixData[i].curve;
#else
    newModel.mixData[i].curveMode = oldModel.mixData[i].curveMode;
    newModel.mixData[i].noExpo = oldModel.mixData[i].noExpo;
    newModel.mixData[i].curveParam = oldModel.mixData[i].curveParam;
#endif
    newModel.mixData[i].delayUp = oldModel.mixData[i].delayUp;
    newModel.mixData[i].delayDown = oldModel.mixData[i].delayDown;
    newModel.mixData[i].speedUp = oldModel.mixData[i].speedUp;
    newModel.mixData[i].speedDown = oldModel.mixData[i].speedDown;
    newModel.mixData[i].srcRaw = oldModel.mixData[i].srcRaw;
    newModel.mixData[i].offset = oldModel.mixData[i].offset;
    memcpy(newModel.mixData[i].name, oldModel.mixData[i].name, sizeof(newModel.mixData[i].name));
  }
  for (int i=0; i<NUM_CHNOUT; i++) {
    newModel.limitData[i] = oldModel.limitData[i];
#if defined(PCBTARANIS)
    if (newModel.moduleData[INTERNAL_MODULE].type == MODULE_TYPE_XJT || newModel.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_XJT) {
      newModel.limitData[i].ppmCenter = (oldModel.limitData[i].ppmCenter * 612) / 1024;
    }
#endif
  }
  for (int i=0; i<MAX_EXPOS; i++) {
#if defined(PCBTARANIS)
    newModel.expoData[i].srcRaw = oldModel.expoData[i].srcRaw;
    newModel.expoData[i].scale = oldModel.expoData[i].scale;
    newModel.expoData[i].carryTrim = oldModel.expoData[i].carryTrim;
    newModel.expoData[i].curve = oldModel.expoData[i].curve;
    newModel.expoData[i].offset = oldModel.expoData[i].offset;
#else
    newModel.expoData[i].curveMode = oldModel.expoData[i].curveMode;
    newModel.expoData[i].curveParam = oldModel.expoData[i].curveParam;
#endif
    newModel.expoData[i].chn = oldModel.expoData[i].chn;
    newModel.expoData[i].swtch = ConvertSwitch_217_to_218(oldModel.expoData[i].swtch);
    newModel.expoData[i].flightModes = oldModel.expoData[i].flightModes;
    newModel.expoData[i].weight = oldModel.expoData[i].weight;
    newModel.expoData[i].mode = oldModel.expoData[i].mode;
    memcpy(newModel.expoData[i].name, oldModel.expoData[i].name, sizeof(newModel.expoData[i].name));
  }
  memcpy(newModel.curves, oldModel.curves, sizeof(newModel.curves));
  memcpy(newModel.points, oldModel.points, sizeof(newModel.points));
  for (int i=0; i<32; i++) {
    newModel.logicalSw[i].func = oldModel.logicalSw[i].func;
    newModel.logicalSw[i].v1 = oldModel.logicalSw[i].v1;
    newModel.logicalSw[i].v3 = oldModel.logicalSw[i].v3;
    newModel.logicalSw[i].andsw = ConvertSwitch_217_to_218(oldModel.logicalSw[i].andsw);
    newModel.logicalSw[i].v2 = oldModel.logicalSw[i].v2;
    newModel.logicalSw[i].delay = oldModel.logicalSw[i].delay;
    newModel.logicalSw[i].duration = oldModel.logicalSw[i].duration;
  }
  for (int i=0; i<NUM_CFN; i++) {
    memcpy(&newModel.customFn[i], &oldModel.customFn[i], sizeof(CustomFunctionData));
    newModel.customFn[i].swtch = ConvertSwitch_217_to_218(oldModel.customFn[i].swtch);
    newModel.customFn[i].func = oldModel.customFn[i].func;
  }
  newModel.swashR = oldModel.swashR;
  for (int i=0; i<MAX_FLIGHT_MODES; i++) {
    memcpy(newModel.flightModeData[i].trim, oldModel.flightModeData[i].trim, sizeof(newModel.flightModeData[i].trim));
    memcpy(newModel.flightModeData[i].name, oldModel.flightModeData[i].name, sizeof(newModel.flightModeData[i].name));
    newModel.flightModeData[i].swtch = ConvertSwitch_217_to_218(oldModel.flightModeData[i].swtch);
    newModel.flightModeData[i].fadeIn = oldModel.flightModeData[i].fadeIn;
    newModel.flightModeData[i].fadeOut = oldModel.flightModeData[i].fadeOut;
#if defined(PCBSKY9X)
    memcpy(newModel.flightModeData[i].rotaryEncoders, oldModel.flightModeData[i].rotaryEncoders, sizeof(newModel.flightModeData[i].rotaryEncoders));
#endif
    memcpy(newModel.flightModeData[i].gvars, oldModel.flightModeData[i].gvars, sizeof(newModel.flightModeData[i].gvars));
  }
  newModel.thrTraceSrc = oldModel.thrTraceSrc;
  newModel.switchWarningState = oldModel.switchWarningState;
  newModel.switchWarningEnable = oldModel.switchWarningEnable;
  memcpy(newModel.gvars, oldModel.gvars, sizeof(newModel.gvars));
  newModel.frsky = oldModel.frsky;
  for (int i=0; i<NUM_MODULES+1; i++) {
    newModel.moduleData[i] = oldModel.moduleData[i];
  }
#if defined(PCBTARANIS)
  newModel.trainerMode = oldModel.trainerMode;
  memcpy(newModel.scriptsData, oldModel.scriptsData, sizeof(newModel.scriptsData));
  memcpy(newModel.curveNames, oldModel.curveNames, sizeof(newModel.curveNames));
  memcpy(newModel.inputNames, oldModel.inputNames, sizeof(newModel.inputNames));
#endif
  newModel.potsWarnMode = oldModel.potsWarnMode;
  newModel.potsWarnEnabled = oldModel.potsWarnEnabled;
  memcpy(newModel.potsWarnPosition, oldModel.potsWarnPosition, sizeof(newModel.potsWarnPosition));
  for (int i=0; i<MAX_SENSORS; i++) {
    newModel.telemetrySensors[i] = oldModel.telemetrySensors[i];
  }
#if defined(PCBTARANIS) && defined(REV9E)
  newModel.topLcdTimer = oldModel.topLcdTimer;
#endif
}

void ConvertModel(int id, int version)
{
  loadModel(id);

  if (version == 216) {
    version = 217;
    ConvertModel_216_to_217(g_model);
  }
  if (version == 217) {
    version = 218;
    ConvertModel_217_to_218(g_model);
  }

  uint8_t currModel = g_eeGeneral.currModel;
  g_eeGeneral.currModel = id;
  s_eeDirtyMsk = EE_MODEL;
  eeCheck(true);
  g_eeGeneral.currModel = currModel;
}

bool eeConvert()
{
  const char *msg = NULL;

  if (g_eeGeneral.version == 216) {
    msg = PSTR("EEprom Data v216");
  }
  else if (g_eeGeneral.version == 217) {
    msg = PSTR("EEprom Data v217");
  }
  else {
    return false;
  }

  int conversionVersionStart = g_eeGeneral.version;

  // Information to the user and wait for key press
  g_eeGeneral.optrexDisplay = 0;
  g_eeGeneral.backlightMode = e_backlight_mode_on;
  g_eeGeneral.backlightBright = 0;
  g_eeGeneral.contrast = 25;
  ALERT(STR_EEPROMWARN, msg, AU_BAD_EEPROM);

  // Message
  MESSAGE(STR_EEPROMWARN, STR_EEPROM_CONVERTING, NULL, AU_EEPROM_FORMATTING); // TODO translations

  // General Settings conversion
  loadGeneralSettings();
  int version = conversionVersionStart;
  if (version == 215) {
    version = 216;
    ConvertGeneralSettings_215_to_216(g_eeGeneral);
  }
  if (version == 216) {
    version = 217;
    ConvertGeneralSettings_216_to_217(g_eeGeneral);
  }
  if (version == 217) {
    version = 218;
    ConvertGeneralSettings_217_to_218(g_eeGeneral);
  }
  s_eeDirtyMsk = EE_GENERAL;
  eeCheck(true);

#if defined(COLORLCD)
#elif LCD_W >= 212
  lcd_rect(60, 6*FH+4, 132, 3);
#else
  lcd_rect(10, 6*FH+4, 102, 3);
#endif

  // Models conversion
  for (uint8_t id=0; id<MAX_MODELS; id++) {
#if defined(COLORLCD)
#elif LCD_W >= 212
    lcd_hline(61, 6*FH+5, 10+id*2, FORCE);
#else
    lcd_hline(11, 6*FH+5, 10+(id*3)/2, FORCE);
#endif
    lcdRefresh();
    if (eeModelExists(id)) {
      ConvertModel(id, conversionVersionStart);
    }

  }

  return true;
}
