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
  MIXSRC216_LAST_LOGICAL_SWITCH = MIXSRC216_FIRST_LOGICAL_SWITCH+NUM_LOGICAL_SWITCH-1,

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
  int16_t trim[4];
  int8_t swtch;       // swtch of phase[0] is not used
  char name[LEN_FLIGHT_MODE_NAME];
  uint8_t fadeIn;
  uint8_t fadeOut;
  ROTARY_ENCODER_ARRAY;
  gvar_t gvars[5];
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

PACK(typedef struct {
  ModelHeader header;
  TimerData_v215 timers[2];
  uint8_t   protocol:3;
  uint8_t   thrTrim:1;            // Enable Throttle Trim
  int8_t    ppmNCH:4;
  uint8_t   trimInc:3;            // Trim Increments
  uint8_t   disableThrottleWarning:1;
  uint8_t   pulsePol:1;
  uint8_t   extendedLimits:1;
  uint8_t   extendedTrims:1;
  uint8_t   throttleReversed:1;
  int8_t    ppmDelay;
  BeepANACenter beepANACenter;        // 1<<0->A1.. 1<<6->A7
  MixData_v215   mixData[MAX_MIXERS];
  LimitData_v215 limitData[NUM_CHNOUT];
  ExpoData_v215  expoData[32];

  int16_t   curves[16];
  int8_t    points[NUM_POINTS];

  LogicalSwitchData_v215 logicalSw[NUM_LOGICAL_SWITCH];
  CustomFunctionData_v215 customFn[32];
  SwashRingData swashR;
  FlightModeData_v215 flightModeData[MAX_FLIGHT_MODES];

  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5ms increments
  uint8_t   thrTraceSrc;

  swarnstate_t switchWarningState;

  char gvar_names[5][LEN_GVAR_NAME];

  FrSkyData_v215 frsky;

  MODELDATA_EXTRA

}) ModelData_v215;

#if defined(PCBTARANIS)
#define MODELDATA_EXTRA_216 \
  uint8_t externalModule; \
  uint8_t trainerMode; \
  ModuleData moduleData[NUM_MODULES+1]; \
  char curveNames[MAX_CURVES][6]; \
  ScriptData_v216 scriptsData[MAX_SCRIPTS]; \
  char inputNames[MAX_INPUTS][LEN_INPUT_NAME]; \
  uint8_t nPotsToWarn; \
  int8_t potPosition[NUM_POTS]; \
  uint8_t spare[2];
#elif defined(PCBSKY9X)
#define MODELDATA_EXTRA_216 \
  uint8_t externalModule; \
  ModuleData moduleData[NUM_MODULES+1]; \
  uint8_t nPotsToWarn; \
  int8_t potPosition[NUM_POTS]; \
  uint8_t rxBattAlarms[2];
#endif

PACK(typedef struct {
  ModelHeader header;
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

  LogicalSwitchData_v216 logicalSw[NUM_LOGICAL_SWITCH];
  CustomFunctionData customFn[NUM_CFN];
  SwashRingData swashR;
  FlightModeData flightModeData[MAX_FLIGHT_MODES];

  AVR_FIELD(int8_t ppmFrameLength)     // 0=22.5ms  (10ms-30ms) 0.5ms increments
  uint8_t   thrTraceSrc;

  swarnstate_t switchWarningState;
  uint8_t switchWarningEnable;

  global_gvar_t gvars[MAX_GVARS];

  FrSkyData_v216 frsky;

  MODELDATA_EXTRA_216

}) ModelData_v216;

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
  int8_t    vBatCalib;
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
  int8_t    currentCalib;
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

void ConvertGeneralSettings_216_to_217(EEGeneral &settings)
{
  settings.version = 217;
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

  if (swtch >= SWSRC_SI0)
    swtch += 12;

  return swtch;
}
#else
int ConvertSwitch_216_to_217(int swtch)
{
  return swtch;
}
#endif

int ConvertSource_216_to_217(int source)
{
#if defined(PCBTARANIS)
  // SI to SN switches added
  if (source >= MIXSRC_SI)
    source += 6;
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

void ConvertModel_215_to_216(ModelData &model)
{
  // Virtual inputs added instead of Expo/DR
  // LUA scripts added
  // GVARS: now 9 GVARS, popup param added
  // Curves: structure changed, 32 curves
  // Limits: min and max with PREC1
  // Custom Functions: play repeat * 5
  // Logical Switches: better precision for x when A comes from telemetry
  // Main View: altitude in top bar
  // Mixes: GVARS in weight moved from 512 to 4096 and -512 to -4096, because GVARS may be used in limits [-1250:1250]
  // Switches: two 6-pos pots added, REa added to Sky9x

  assert(sizeof(ModelData_v215) <= sizeof(ModelData));

  ModelData_v215 oldModel;
  memcpy(&oldModel, &model, sizeof(oldModel));
  ModelData_v216 &newModel = *(ModelData_v216*)&model;
  memset(&newModel, 0, sizeof(ModelData_v216));

  char name[LEN_MODEL_NAME+1];
  zchar2str(name, oldModel.header.name, LEN_MODEL_NAME);
  TRACE("Model %s conversion from v215 to v216", name);

  memcpy(&newModel.header, &oldModel.header, sizeof(newModel.header));
  for (uint8_t i=0; i<2; i++) {
    TimerData_v216 & timer = newModel.timers[i];
    if (oldModel.timers[i].mode >= TMRMODE_COUNT)
      timer.mode = TMRMODE_COUNT + ConvertSwitch_215_to_216(oldModel.timers[i].mode - TMRMODE_COUNT + 1) - 1;
    else
      timer.mode = oldModel.timers[i].mode;
    timer.start = oldModel.timers[i].start;
    timer.minuteBeep = oldModel.timers[i].minuteBeep;
    timer.persistent = oldModel.timers[i].persistent;
    timer.countdownBeep = oldModel.timers[i].countdownBeep;
    timer.value = oldModel.timers[i].value;
  }
  newModel.thrTrim = oldModel.thrTrim;
  newModel.trimInc = oldModel.trimInc - 2;
  newModel.disableThrottleWarning = oldModel.disableThrottleWarning;
  newModel.extendedLimits = oldModel.extendedLimits;
  newModel.extendedTrims = oldModel.extendedTrims;
  newModel.throttleReversed = oldModel.throttleReversed;

#if defined(PCBTARANIS)
  newModel.beepANACenter = (oldModel.beepANACenter & 0x3f) | ((oldModel.beepANACenter & 0xc0) << 1);
#else
  newModel.beepANACenter = oldModel.beepANACenter;
#endif

  for (uint8_t i=0; i<64; i++) {
    MixData_v216 * mix = &newModel.mixData[i];
    MixData_v215 * oldMix = &oldModel.mixData[i];
#if defined(PCBTARANIS)
    mix->destCh = oldMix->destCh;
    mix->flightModes = oldMix->flightModes;
    mix->mltpx = oldMix->mltpx;
    if (oldMix->carryTrim == TRIM_OFF) mix->carryTrim = TRIM_OFF;
    mix->weight = ConvertGVAR_215_to_216(oldMix->weight);
    mix->swtch = ConvertSwitch_215_to_216(oldMix->swtch);
    if (oldMix->curveMode==0/*differential*/) {
      mix->curve.type = CURVE_REF_DIFF;
      mix->curve.value = oldMix->curveParam;
    }
    else if (oldMix->curveParam <= 6) {
      mix->curve.type = CURVE_REF_FUNC;
      mix->curve.value = oldMix->curveParam;
    }
    else {
      mix->curve.type = CURVE_REF_CUSTOM;
      mix->curve.value = oldMix->curveParam - 6;
    }
    mix->mixWarn = oldMix->mixWarn;
    mix->delayUp = oldMix->delayUp;
    mix->delayDown = oldMix->delayDown;
    mix->speedUp = oldMix->speedUp;
    mix->speedDown = oldMix->speedDown;
    mix->srcRaw = oldMix->srcRaw;
    if (mix->srcRaw > 4 || oldMix->noExpo)
      mix->srcRaw = ConvertSource_215_to_216(mix->srcRaw);
    mix->offset = ConvertGVAR_215_to_216(oldMix->offset);
    memcpy(mix->name, oldMix->name, LEN_EXPOMIX_NAME);
    if (!GV_IS_GV_VALUE(mix->weight, 500, 500) && !GV_IS_GV_VALUE(mix->offset, 500, 500)) {
      mix->offset = divRoundClosest(mix->offset * mix->weight, 100);
    }
#else
    mix->destCh = oldMix->destCh;
    mix->mixWarn = oldMix->mixWarn;
    mix->flightModes = oldMix->flightModes;
    mix->curveMode = oldMix->curveMode;
    mix->noExpo = oldMix->noExpo;
    mix->carryTrim = oldMix->carryTrim;
    mix->mltpx = oldMix->mltpx;
    mix->weight = oldMix->weight;
    mix->swtch = ConvertSwitch_215_to_216(oldMix->swtch);
    mix->curveParam = oldMix->curveParam;
    mix->delayUp = oldMix->delayUp;
    mix->delayDown = oldMix->delayDown;
    mix->speedUp = oldMix->speedUp;
    mix->speedDown = oldMix->speedDown;
    mix->srcRaw = ConvertSource_215_to_216(oldMix->srcRaw);
    mix->offset = oldMix->offset;
    memcpy(mix->name, oldMix->name, LEN_EXPOMIX_NAME);
#endif
  }
  for (uint8_t i=0; i<32; i++) {
#if defined(PCBTARANIS)
    newModel.limitData[i].min = 10 * oldModel.limitData[i].min;
    newModel.limitData[i].max = 10 * oldModel.limitData[i].max;
#else
    newModel.limitData[i].min = oldModel.limitData[i].min;
    newModel.limitData[i].max = oldModel.limitData[i].max;
#endif
    newModel.limitData[i].ppmCenter = oldModel.limitData[i].ppmCenter;
    newModel.limitData[i].offset = oldModel.limitData[i].offset;
    newModel.limitData[i].symetrical = oldModel.limitData[i].symetrical;
    newModel.limitData[i].revert = oldModel.limitData[i].revert;
#if defined(PCBTARANIS)
    memcpy(newModel.limitData[i].name, oldModel.limitData[i].name, LEN_CHANNEL_NAME);
#endif
  }
#if defined(PCBTARANIS)
  int indexes[NUM_STICKS] = { 0, 0, 0, 0 };
#endif
  for (uint8_t i=0; i<32; i++) {
    ExpoData_v216 * expo = &newModel.expoData[i];
    ExpoData_v215 * oldExpo = &oldModel.expoData[i];
    if (oldExpo->mode) {
#if defined(PCBTARANIS)
      uint8_t chn = oldExpo->chn;
      if (!oldExpo->swtch && !oldExpo->flightModes) {
        indexes[chn] = -1;
      }
      else if (indexes[chn] != -1) {
        indexes[chn] = i+1;
      }
      for (uint8_t j=chn+1; j<NUM_STICKS; j++) {
        indexes[j] = i+1;
      }
      expo->srcRaw = MIXSRC216_Rud+chn;
      expo->chn = chn;
      expo->mode = oldExpo->mode;
      expo->swtch = ConvertSwitch_215_to_216(oldExpo->swtch);
      expo->flightModes = oldExpo->flightModes;
      expo->weight = oldExpo->weight;
      memcpy(expo->name, oldExpo->name, LEN_EXPOMIX_NAME);
      if (oldExpo->curveMode==0/*expo*/) {
        if (oldExpo->curveParam) {
          expo->curve.type = CURVE_REF_EXPO;
          expo->curve.value = oldExpo->curveParam;
        }
      }
      else if (oldExpo->curveParam <= 6) {
        expo->curve.type = CURVE_REF_FUNC;
        expo->curve.value = oldExpo->curveParam;
      }
      else {
        expo->curve.type = CURVE_REF_CUSTOM;
        expo->curve.value = oldExpo->curveParam - 6;
      }
#else
      expo->mode = oldExpo->mode;
      expo->chn = oldExpo->chn;
      expo->curveMode = oldExpo->curveMode;
      expo->swtch = oldExpo->swtch;
      expo->flightModes = oldExpo->flightModes;
      expo->weight = oldExpo->weight;
      memcpy(expo->name, oldExpo->name, LEN_EXPOMIX_NAME);
      expo->curveParam = oldExpo->curveParam;
#endif
    }
    else {
      break;
    }
  }
#if defined(PCBTARANIS)
  for (int i=NUM_STICKS-1; i>=0; i--) {
    int idx = indexes[i];
    if (idx >= 0) {
      ExpoData_v216 * expo = &newModel.expoData[idx];
      memmove(expo+1, expo, (MAX_EXPOS-(idx+1))*sizeof(ExpoData_v216));
      memclear(expo, sizeof(ExpoData_v216));
      expo->srcRaw = MIXSRC216_Rud + i;
      expo->chn = i;
      expo->weight = 100;
      expo->curve.type = CURVE_REF_EXPO;
      expo->mode = 3;
    }
    for (int c=0; c<4; c++) {
      newModel.inputNames[i][c] = char2idx(STR_VSRCRAW[1+STR_VSRCRAW[0]*(i+1)+c]);
    }
  }
#endif
  for (uint8_t i=0; i<16; i++) {
#if defined(PCBTARANIS)
    int8_t *cur = &oldModel.points[i==0 ? 0 : 5*i+oldModel.curves[i-1]];
    int8_t *nxt = &oldModel.points[5+5*i+oldModel.curves[i]];
    uint8_t size = nxt - cur;
    if ((size & 1) == 0) {
      newModel.curves[i].type = CURVE_TYPE_CUSTOM;
      newModel.curves[i].points = (size / 2) - 4;
    }
    else {
      newModel.curves[i].points = size-5;
    }
#else
    newModel.curves[i] = oldModel.curves[i];
#endif
  }
  for (uint16_t i=0; i<512; i++) {
    newModel.points[i] = oldModel.points[i];
  }
  for (uint8_t i=0; i<32; i++) {
    LogicalSwitchData_v216 & sw = newModel.logicalSw[i];
    sw.func = oldModel.logicalSw[i].func;
    if (sw.func >= LS_FUNC_VEQUAL) sw.func += 1;
    if (sw.func >= LS_FUNC_RANGE) sw.func += 1;
    if (sw.func >= LS_FUNC_EDGE) sw.func += 1;
    sw.v1 = oldModel.logicalSw[i].v1;
    sw.v2 = oldModel.logicalSw[i].v2;
    sw.delay = oldModel.logicalSw[i].delay * 5;
    sw.duration = oldModel.logicalSw[i].duration * 5;
    sw.andsw = ConvertSwitch_215_to_216(oldModel.logicalSw[i].andsw);
    uint8_t cstate = lswFamily(sw.func);
    if (cstate == LS_FAMILY_BOOL) {
      sw.v1 = ConvertSwitch_215_to_216(sw.v1);
      sw.v2 = ConvertSwitch_215_to_216(sw.v2);
    }
    else if (cstate == LS_FAMILY_OFS || cstate == LS_FAMILY_COMP || cstate == LS_FAMILY_DIFF) {
      sw.v1 = ConvertSource_215_to_216(sw.v1);
      if (cstate == LS_FAMILY_OFS || cstate == LS_FAMILY_DIFF) {
        if ((uint8_t)sw.v1 >= MIXSRC216_FIRST_TELEM) {
          switch ((uint8_t)sw.v1) {
            case MIXSRC216_FIRST_TELEM + TELEM216_TIMER1-1:
            case MIXSRC216_FIRST_TELEM + TELEM216_TIMER2-1:
              sw.v2 = (sw.v2+128) * 3;
              break;
            case MIXSRC216_FIRST_TELEM + TELEM216_ALT-1:
            case MIXSRC216_FIRST_TELEM + TELEM216_GPSALT-1:
            case MIXSRC216_FIRST_TELEM + TELEM216_MIN_ALT-1:
            case MIXSRC216_FIRST_TELEM + TELEM216_MAX_ALT-1:
              sw.v2 = (sw.v2+128) * 8 - 500;
              break;
            case MIXSRC216_FIRST_TELEM + TELEM216_RPM-1:
            case MIXSRC216_FIRST_TELEM + TELEM216_MAX_RPM-1:
              sw.v2 = (sw.v2+128) * 50;
              break;
            case MIXSRC216_FIRST_TELEM + TELEM216_T1-1:
            case MIXSRC216_FIRST_TELEM + TELEM216_T2-1:
            case MIXSRC216_FIRST_TELEM + TELEM216_MAX_T1-1:
            case MIXSRC216_FIRST_TELEM + TELEM216_MAX_T2-1:
              sw.v2 = (sw.v2+128) + 30;
              break;
            case MIXSRC216_FIRST_TELEM + TELEM216_CELL-1:
            case MIXSRC216_FIRST_TELEM + TELEM216_HDG-1:
              sw.v2 = (sw.v2+128) * 2;
              break;
            case MIXSRC216_FIRST_TELEM + TELEM216_DIST-1:
            case MIXSRC216_FIRST_TELEM + TELEM216_MAX_DIST-1:
              sw.v2 = (sw.v2+128) * 8;
              break;
            case MIXSRC216_FIRST_TELEM + TELEM216_CURRENT-1:
            case MIXSRC216_FIRST_TELEM + TELEM216_POWER-1:
              sw.v2 = (sw.v2+128) * 5;
              break;
            case MIXSRC216_FIRST_TELEM + TELEM216_CONSUMPTION-1:
              sw.v2 = (sw.v2+128) * 20;
              break;
            default:
              sw.v2 += 128;
              break;
          }
        }
      }
      else if (cstate == LS_FAMILY_COMP) {
        sw.v2 = ConvertSource_215_to_216(sw.v2);
      }
    }
  }
  for (uint8_t i=0; i<32; i++) {
    CustomFunctionData & fn = newModel.customFn[i];
    fn.swtch = ConvertSwitch_215_to_216(oldModel.customFn[i].swtch);
    fn.func = oldModel.customFn[i].func;
    if (fn.func <= 15) {
      fn.all.param = fn.func;
      fn.func = FUNC_OVERRIDE_CHANNEL;
    }
    else if (fn.func <= 20) {
      fn.all.param = fn.func - 16;
      fn.func = FUNC_TRAINER;
    }
    else if (fn.func == 21) {
      fn.func = FUNC_INSTANT_TRIM;
    }
    else if (fn.func == 22) {
      fn.func = FUNC_PLAY_SOUND;
    }
#if defined(PCBSKY9X)
    else if (fn.func == 23) {
      fn.func = FUNC_HAPTIC;
    }
#endif
    else if (fn.func == 23+IS_PCBSKY9X) {
      fn.func = FUNC_RESET;
    }
    else if (fn.func == 24+IS_PCBSKY9X) {
      fn.func = FUNC_VARIO;
    }
    else if (fn.func == 25+IS_PCBSKY9X) {
      fn.func = FUNC_PLAY_TRACK;
    }
    else if (fn.func == 26+IS_PCBSKY9X) {
      fn.func = FUNC_PLAY_VALUE;
    }
    else if (fn.func == 27+IS_PCBSKY9X) {
      fn.func = FUNC_LOGS;
    }
    else if (fn.func == 28+IS_PCBSKY9X) {
      fn.func = FUNC_VOLUME;
    }
    else if (fn.func == 29+IS_PCBSKY9X) {
      fn.func = FUNC_BACKLIGHT;
    }
    else if (fn.func == 30+IS_PCBSKY9X) {
      fn.func = FUNC_BACKGND_MUSIC;
    }
    else if (fn.func == 31+IS_PCBSKY9X) {
      fn.func = FUNC_BACKGND_MUSIC_PAUSE;
    }
    else {
      fn.all.param = fn.func - 32 - IS_PCBSKY9X;
      fn.all.mode = oldModel.customFn[i].mode;
      fn.func = FUNC_ADJUST_GVAR;
    }

    fn.active = oldModel.customFn[i].active;
    if (HAS_REPEAT_PARAM(fn.func)) {
      fn.active *= 5;
    }

    if (fn.func == FUNC_PLAY_TRACK || fn.func == FUNC_BACKGND_MUSIC) {
      memcpy(fn.play.name, oldModel.customFn[i].param.name, 8);
    }
    else {
      fn.all.val = oldModel.customFn[i].param.composite.val;
    }
    if (fn.func == FUNC_PLAY_VALUE || fn.func == FUNC_VOLUME || (IS_ADJUST_GV_FUNC(fn.func) && fn.all.mode == FUNC_ADJUST_GVAR_SOURCE)) {
#if defined(PCBTARANIS)
      fn.all.val = ConvertSource_215_to_216(fn.all.val, true);
#endif
    }
  }

  newModel.swashR = oldModel.swashR;
  newModel.swashR.collectiveSource = ConvertSource_215_to_216(newModel.swashR.collectiveSource);

  for (uint8_t i=0; i<9; i++) {
    if (i==0 || oldModel.flightModeData[i].swtch) {
      memcpy(&newModel.flightModeData[i], &oldModel.flightModeData[i], sizeof(oldModel.flightModeData[i])); // the last 4 gvars will remain blank
      newModel.flightModeData[i].swtch = ConvertSwitch_215_to_216(oldModel.flightModeData[i].swtch);
#if defined(PCBTARANIS)
      for (uint8_t t=0; t<4; t++) {
        int trim = oldModel.flightModeData[i].trim[t];
        if (trim > 500) {
          trim -= 501;
          if (trim >= i)
            trim += 1;
          newModel.flightModeData[i].trim[t].mode = 2*trim;
          newModel.flightModeData[i].trim[t].value = 0;
        }
        else {
          newModel.flightModeData[i].trim[t].mode = 2*i;
          newModel.flightModeData[i].trim[t].value = trim;
        }
      }
#endif
    }
  }
  newModel.thrTraceSrc = oldModel.thrTraceSrc;
#if defined(PCBTARANIS)
  // S3 added
  if (newModel.thrTraceSrc >= THROTTLE_SOURCE_S3)
    newModel.thrTraceSrc += 1;
#endif
  newModel.switchWarningState = oldModel.switchWarningState >> 1;
  newModel.switchWarningEnable = (oldModel.switchWarningState & 0x01) ? 0xFF : 0;
  for (uint8_t i=0; i<5; i++) {
    memcpy(newModel.gvars[i].name, oldModel.gvar_names[i], LEN_GVAR_NAME);
  }

  memcpy(&newModel.frsky, &oldModel.frsky, 2*sizeof(FrSkyChannelData));
  // gap for A3-A4
  memcpy(((uint8_t *)&newModel.frsky) + 4*sizeof(FrSkyChannelData), ((uint8_t *)&oldModel.frsky) + 2*sizeof(FrSkyChannelData), sizeof(oldModel.frsky) - 2*sizeof(FrSkyChannelData));
  // A3 and A4 introduced
  if (newModel.frsky.voltsSource >= FRSKY_VOLTS_SOURCE_A3)
    newModel.frsky.voltsSource += 2;
  if (newModel.frsky.currentSource >= FRSKY_CURRENT_SOURCE_A3)
    newModel.frsky.currentSource += 2;

  for (int i=0; i<3; i++) {
    if (newModel.frsky.screensType & (1<<i)) {
      // gauges
      for (int j=0; j<4; j++) {
        // uint8_t & source = newModel.frsky.screens[i].bars[j].source;
        // source = ConvertTelemetrySource_215_to_216(source);
      }
    }
    else {
      // numbers
      for (int j=0; j<4; j++) {
        for (int k=0; k<NUM_LINE_ITEMS; k++) {
          // uint8_t & source = newModel.frsky.screens[i].lines[j].sources[k];
          // source = ConvertTelemetrySource_215_to_216(source);
        }
      }
    }
  }

#if defined(PCBTARANIS)
  newModel.externalModule = oldModel.externalModule;
  newModel.trainerMode = oldModel.trainerMode;
  memcpy(newModel.curveNames, oldModel.curveNames, sizeof(newModel.curveNames));
#endif

  memcpy(newModel.moduleData, oldModel.moduleData, sizeof(newModel.moduleData));
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
  ModelData &newModel = model;
  memset(&newModel, 0, sizeof(ModelData));

  char name[LEN_MODEL_NAME+1];
  zchar2str(name, oldModel.header.name, LEN_MODEL_NAME);
  TRACE("Model %s conversion from v216 to v217", name);

  memcpy(&newModel.header, &oldModel.header, sizeof(newModel.header));
  for (uint8_t i=0; i<2; i++) {
    newModel.timers[i].mode = oldModel.timers[i].mode;
    newModel.timers[i].start = oldModel.timers[i].start;
    newModel.timers[i].countdownBeep = oldModel.timers[i].countdownBeep;
    newModel.timers[i].minuteBeep = oldModel.timers[i].minuteBeep;
    newModel.timers[i].persistent = oldModel.timers[i].persistent;
    newModel.timers[i].value = oldModel.timers[i].value;
  }
  newModel.telemetryProtocol = oldModel.telemetryProtocol;
  newModel.thrTrim = oldModel.thrTrim;
  newModel.trimInc = oldModel.trimInc - 2;
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
  for (int i=0; i<NUM_LOGICAL_SWITCH; i++) {
    newModel.logicalSw[i].func = oldModel.logicalSw[i].func;
    newModel.logicalSw[i].v1 = oldModel.logicalSw[i].v1;
    newModel.logicalSw[i].v2 = oldModel.logicalSw[i].v2;
    newModel.logicalSw[i].v3 = oldModel.logicalSw[i].v3;
    newModel.logicalSw[i].delay = oldModel.logicalSw[i].delay;
    newModel.logicalSw[i].duration = oldModel.logicalSw[i].duration;
    newModel.logicalSw[i].andsw = oldModel.logicalSw[i].andsw;
    uint8_t cstate = lswFamily(newModel.logicalSw[i].func);
    if (cstate == LS_FAMILY_OFS || cstate == LS_FAMILY_COMP || cstate == LS_FAMILY_DIFF) {
      newModel.logicalSw[i].v1 = ConvertSource_216_to_217((uint8_t)newModel.logicalSw[i].v1);
      if (cstate == LS_FAMILY_COMP) {
        newModel.logicalSw[i].v2 = ConvertSource_216_to_217((uint8_t)newModel.logicalSw[i].v2);
      }
    }
  }
  for (int i=0; i<NUM_CFN; i++) {
    CustomFunctionData & fn = newModel.customFn[i];
    fn = oldModel.customFn[i];
    if (fn.func == FUNC_PLAY_VALUE || fn.func == FUNC_VOLUME || (IS_ADJUST_GV_FUNC(fn.func) && fn.all.mode == FUNC_ADJUST_GVAR_SOURCE)) {
      fn.all.val = ConvertSource_216_to_217(fn.all.val);
    }
  }

  newModel.swashR = oldModel.swashR;
  newModel.swashR.collectiveSource = ConvertSource_216_to_217(newModel.swashR.collectiveSource);

  memcpy(newModel.flightModeData, oldModel.flightModeData, sizeof(newModel.flightModeData));
  newModel.thrTraceSrc = oldModel.thrTraceSrc;
  newModel.switchWarningState = oldModel.switchWarningState;
  newModel.switchWarningEnable = oldModel.switchWarningEnable;
  memcpy(newModel.gvars, oldModel.gvars, sizeof(newModel.gvars));

  memcpy(&newModel.frsky.rssiAlarms, &oldModel.frsky.rssiAlarms, sizeof(newModel.frsky.rssiAlarms));
  newModel.externalModule = oldModel.externalModule;
  memcpy(newModel.moduleData, oldModel.moduleData, sizeof(newModel.moduleData));
#if defined(PCBTARANIS)
  newModel.trainerMode = oldModel.trainerMode;
  // TODO memcpy(newModel.scriptsData, oldModel.scriptsData, sizeof(newModel.scriptsData));
  memcpy(newModel.curveNames, oldModel.curveNames, sizeof(newModel.curveNames));
  memcpy(newModel.inputNames, oldModel.inputNames, sizeof(newModel.inputNames));
#endif
  newModel.nPotsToWarn = oldModel.nPotsToWarn;
  memcpy(newModel.potPosition, oldModel.potPosition, sizeof(newModel.potPosition));
}

void ConvertModel(int id, int version)
{
  loadModel(id);

  if (version == 215) {
    version = 216;
    ConvertModel_215_to_216(g_model);
  }

  if (version == 216) {
    version = 217;
    ConvertModel_216_to_217(g_model);
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

  if (g_eeGeneral.version == 215) {
    msg = PSTR("EEprom Data v215");
  }
  else if (g_eeGeneral.version == 216) {
    msg = PSTR("EEprom Data v216");
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
  s_eeDirtyMsk = EE_GENERAL;
  eeCheck(true);

#if LCD_W >= 212
  lcd_rect(60, 6*FH+4, 132, 3);
#else
  lcd_rect(10, 6*FH+4, 102, 3);
#endif

  // Models conversion
  for (uint8_t id=0; id<MAX_MODELS; id++) {
#if LCD_W >= 212
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
