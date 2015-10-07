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

#ifndef myeeprom_h
#define myeeprom_h

#include <inttypes.h>

#if defined(EXPORT)
  #define LUA_EXPORT(...)                     LEXP(__VA_ARGS__)
  #define LUA_EXPORT_MULTIPLE(...)            LEXP_MULTIPLE(__VA_ARGS__)
  #define LUA_EXPORT_EXTRA(...)               LEXP_EXTRA(__VA_ARGS__)
#else 
  #define LUA_EXPORT(...)
  #define LUA_EXPORT_MULTIPLE(...)
  #define LUA_EXPORT_EXTRA(...)
#endif 

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

#if defined(CPUARM)
  #define EEPROM_VER             218
  #define FIRST_CONV_EEPROM_VER  216
#elif defined(CPUM2560) || defined(CPUM2561)
  #define EEPROM_VER             217
  #define FIRST_CONV_EEPROM_VER  EEPROM_VER
#elif defined(CPUM128)
  #define EEPROM_VER             217
#else
  #define EEPROM_VER             216
#endif

#ifndef PACK
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#if defined(CPUARM)
  #define ARM_FIELD(x) x;
  #define AVR_FIELD(x)
#else
  #define ARM_FIELD(x)
  #define AVR_FIELD(x) x;
#endif

#if defined(PCBSTD)
  #define N_PCBSTD_FIELD(x)
#else
  #define N_PCBSTD_FIELD(x) x;
#endif

#if defined(PCBTARANIS)
  #define N_TARANIS_FIELD(x)
  #define TARANIS_FIELD(x) x;
#else
  #define N_TARANIS_FIELD(x) x;
  #define TARANIS_FIELD(x)
#endif

#if defined(PCBTARANIS) && defined(REV9E)
  #define TARANIS_REV9E_FIELD(x) x;
#else
  #define TARANIS_REV9E_FIELD(x)
#endif

#define NUM_STICKS             4

#if defined(PCBHORUS)
  #define MAX_MODELS           60
  #define NUM_CHNOUT           32 // number of real output channels CH1-CH32
  #define MAX_FLIGHT_MODES     9
  #define MAX_MIXERS           64
  #define MAX_EXPOS            64
  #define NUM_LOGICAL_SWITCH   32 // number of custom switches
  #define NUM_CFN              64 // number of functions assigned to switches
  #define MAX_SCRIPTS          7
  #define MAX_INPUTS           32
  #define NUM_TRAINER          16
  #define NUM_POTS             3
  #define NUM_XPOTS            0
  #define MAX_SENSORS          32
#elif defined(PCBFLAMENCO)
  #define MAX_MODELS           60
  #define NUM_CHNOUT           32 // number of real output channels CH1-CH32
  #define MAX_FLIGHT_MODES     9
  #define MAX_MIXERS           64
  #define MAX_EXPOS            64
  #define NUM_LOGICAL_SWITCH   32 // number of custom switches
  #define NUM_CFN              64 // number of functions assigned to switches
  #define MAX_SCRIPTS          7
  #define MAX_INPUTS           32
  #define NUM_TRAINER          16
  #define NUM_POTS             3
  #define NUM_XPOTS            0
  #define MAX_SENSORS          32
#elif defined(PCBTARANIS)
  #define MAX_MODELS           60
  #define NUM_CHNOUT           32 // number of real output channels CH1-CH32
  #define MAX_FLIGHT_MODES     9
  #define MAX_MIXERS           64
  #define MAX_EXPOS            64
  #define NUM_LOGICAL_SWITCH   64 // number of logical switches
  #define NUM_CFN              64 // number of functions assigned to switches
  #define MAX_SCRIPTS          7
  #define MAX_INPUTS           32
  #define NUM_TRAINER          16
  #if defined(REV9E)
    #define NUM_POTS           8
    #define NUM_XPOTS          4
  #else
    #define NUM_POTS           5
    #define NUM_XPOTS          3
  #endif
  #define MAX_SENSORS          32
#elif defined(PCBSKY9X)
  #define MAX_MODELS           60
  #define NUM_CHNOUT           32 // number of real output channels CH1-CH32
  #define MAX_FLIGHT_MODES     9
  #define MAX_MIXERS           64
  #define MAX_EXPOS            32
  #define NUM_LOGICAL_SWITCH   64 // number of custom switches
  #define NUM_CFN              64 // number of functions assigned to switches
  #define NUM_TRAINER          16
  #define NUM_POTS             3
  #define NUM_XPOTS            0
  #define MAX_SENSORS          32
#elif defined(CPUM2560) || defined(CPUM2561)
  #define MAX_MODELS           30
  #define NUM_CHNOUT           16 // number of real output channels CH1-CH16
  #define MAX_FLIGHT_MODES     6
  #define MAX_MIXERS           32
  #define MAX_EXPOS            16
  #define NUM_LOGICAL_SWITCH   12 // number of custom switches
  #define NUM_CFN              24 // number of functions assigned to switches
  #define NUM_TRAINER          8
  #define NUM_POTS             3
  #define NUM_XPOTS            0
  #define MAX_SENSORS          0
#elif defined(CPUM128)
  #define MAX_MODELS           30
  #define NUM_CHNOUT           16 // number of real output channels CH1-CH16
  #define MAX_FLIGHT_MODES     5
  #define MAX_MIXERS           32
  #define MAX_EXPOS            14
  #define NUM_LOGICAL_SWITCH   12 // number of custom switches
  #define NUM_CFN              24 // number of functions assigned to switches
  #define NUM_TRAINER          8
  #define NUM_POTS             3
  #define NUM_XPOTS            0
  #define MAX_SENSORS          0
#else
  #define MAX_MODELS           16
  #define NUM_CHNOUT           16 // number of real output channels CH1-CH16
  #define MAX_FLIGHT_MODES     5
  #define MAX_MIXERS           32
  #define MAX_EXPOS            14
  #define NUM_LOGICAL_SWITCH   12 // number of custom switches
  #define NUM_CFN              16 // number of functions assigned to switches
  #define NUM_TRAINER          8
  #define NUM_POTS             3
  #define NUM_XPOTS            0
  #define MAX_SENSORS          0
#endif

#if defined(CPUARM)
  #define MAX_TIMERS           3
#else
  #define MAX_TIMERS           2
#endif

#define NUM_CYC                3
#define NUM_CAL_PPM            4

enum CurveType {
  CURVE_TYPE_STANDARD,
  CURVE_TYPE_CUSTOM,
  CURVE_TYPE_LAST = CURVE_TYPE_CUSTOM
};

#if defined(XCURVES)
PACK(typedef struct {
  uint8_t type:3;
  uint8_t smooth:1;
  uint8_t spare:4;
  int8_t  points;
}) CurveInfo;
#else
struct CurveInfo {
  int8_t * crv;
  uint8_t points;
  bool custom;
};
extern CurveInfo curveInfo(uint8_t idx);
#endif

#if defined(PCBHORUS)
  #define LEN_MODEL_NAME       12
  #define LEN_TIMER_NAME       8
  #define LEN_FLIGHT_MODE_NAME 10
  #define LEN_EXPOMIX_NAME     6
  #define LEN_CHANNEL_NAME     6
  #define LEN_INPUT_NAME       4
  #define MAX_CURVES           32
  #define NUM_POINTS           512
  #define CURVDATA             CurveInfo
#elif defined(PCBFLAMENCO)
  #define LEN_MODEL_NAME       12
  #define LEN_TIMER_NAME       8
  #define LEN_FLIGHT_MODE_NAME 10
  #define LEN_EXPOMIX_NAME     6
  #define LEN_CHANNEL_NAME     6
  #define LEN_INPUT_NAME       4
  #define MAX_CURVES           32
  #define NUM_POINTS           512
  #define CURVDATA             CurveInfo
#elif defined(PCBTARANIS)
  #define LEN_MODEL_NAME       12
  #define LEN_TIMER_NAME       8
  #define LEN_FLIGHT_MODE_NAME 10
  #define LEN_BITMAP_NAME      10
  #define LEN_EXPOMIX_NAME     8
  #define LEN_CHANNEL_NAME     6
  #define LEN_INPUT_NAME       4
  #define MAX_CURVES           32
  #define NUM_POINTS           512
  #define CURVDATA             CurveInfo
#elif defined(CPUARM)
  #define LEN_MODEL_NAME       10
  #define LEN_TIMER_NAME       3
  #define LEN_FLIGHT_MODE_NAME 6
  #define LEN_EXPOMIX_NAME     6
  #define MAX_CURVES           16
  #define NUM_POINTS           512
  #define CURVDATA             int16_t
#else
  #define LEN_MODEL_NAME       10
  #define LEN_FLIGHT_MODE_NAME 6
  #define MAX_CURVES           8
  #define NUM_POINTS           (112-MAX_CURVES)
  #define CURVDATA             int8_t
#endif

#if defined(PCBTARANIS) || defined(PCBSKY9X)
  #define NUM_MODULES          2
#else
  #define NUM_MODULES          1
#endif

typedef int16_t gvar_t;

#if defined(PCBTARANIS)
  typedef uint16_t source_t;
#else
  typedef uint8_t source_t;
#endif

#if !defined(PCBSTD)
  #define LEN_GVAR_NAME 6
  #define GVAR_MAX      1024
  #define GVAR_LIMIT    500
  PACK(typedef struct {
    char    name[LEN_GVAR_NAME];
    uint8_t popup:1;
    uint8_t spare:7;
  }) global_gvar_t;
#endif

#define RESERVE_RANGE_FOR_GVARS 10
// even we do not spend space in EEPROM for 10 GVARS, we reserve the space inside the range of values, like offset, weight, etc.

#if defined(PCBSTD) && defined(GVARS)
  #define MAX_GVARS 5
  #define MODEL_GVARS_DATA gvar_t gvars[MAX_GVARS];
  #define PHASE_GVARS_DATA
  #define GVAR_VALUE(x, p) g_model.gvars[x]
#elif defined(PCBSTD)
  #define MAX_GVARS 0
  #define MODEL_GVARS_DATA
  #define PHASE_GVARS_DATA
#else
  #if defined(CPUARM)
    #define MAX_GVARS 9
  #else
    #define MAX_GVARS 5
  #endif
  #define MODEL_GVARS_DATA global_gvar_t gvars[MAX_GVARS];
  #define PHASE_GVARS_DATA gvar_t gvars[MAX_GVARS]
  #define GVAR_VALUE(x, p) g_model.flightModeData[p].gvars[x]
#endif

PACK(typedef struct {
  uint8_t srcChn:6; // 0-7 = ch1-8
  uint8_t mode:2;   // off,add-mode,subst-mode
  int8_t  studWeight;
}) TrainerMix;

PACK(typedef struct {
  int16_t        calib[4];
  TrainerMix     mix[4];
}) TrainerData;

PACK(typedef struct {
  int8_t    level:2;
  int8_t    value:6;
}) FrSkyRSSIAlarm;

#if defined(PCBFLAMENCO) || defined(PCBHORUS)
enum MainViews {
  VIEW_BLANK,
  VIEW_TIMERS_ALTITUDE,
  VIEW_CHANNELS,
  VIEW_TELEM1,
  VIEW_TELEM2,
  VIEW_TELEM3,
  VIEW_TELEM4,
  VIEW_COUNT
};
#elif defined(PCBTARANIS)
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
  #define EXTRA_GENERAL_FIELDS_ARM \
  uint8_t  backlightBright; \
  int8_t   txCurrentCalibration; \
  int8_t   temperatureWarn; \
  uint8_t  mAhWarn; \
  uint16_t mAhUsed; \
  uint32_t globalTimer; \
  int8_t   temperatureCalib; \
  uint8_t  btBaudrate; \
  uint8_t  optrexDisplay; \
  uint8_t  sticksGain; \
  uint8_t  rotarySteps; \
  uint8_t  countryCode; \
  uint8_t  imperial; \
  char     ttsLanguage[2]; \
  int8_t   beepVolume; \
  int8_t   wavVolume; \
  int8_t   varioVolume; \
  int8_t   varioPitch; \
  int8_t   varioRange; \
  int8_t   varioRepeat; \
  int8_t   backgroundVolume;
#endif

#if defined(PCBTARANIS)
  #if defined(REV9E)
    #define swconfig_t        uint64_t
    #define swarnstate_t      uint64_t
    #define swarnenable_t     uint32_t
  #else
    #define swconfig_t        uint16_t
    #define swarnstate_t      uint16_t
    #define swarnenable_t     uint8_t
  #endif
#else
  #define swarnstate_t        uint8_t
  #define swarnenable_t       uint8_t
#endif

enum UartModes {
#if defined(CLI) || defined(DEBUG)
  UART_MODE_DEBUG,
#else
  UART_MODE_NONE,
#endif
  UART_MODE_TELEMETRY_MIRROR,
  UART_MODE_TELEMETRY,
  UART_MODE_SBUS_TRAINER,
  // UART_MODE_CPPM_TRAINER,
  UART_MODE_COUNT,
  UART_MODE_MAX = UART_MODE_COUNT-1
};

#if defined(PCBHORUS)
  #define LEN_SWITCH_NAME              3
  #define LEN_ANA_NAME                 3
  #define EXTRA_GENERAL_FIELDS \
    EXTRA_GENERAL_FIELDS_ARM \
    uint8_t  serial2Mode:6; \
    uint8_t  spare:2; \
    CustomFunctionData customFn[NUM_CFN]; \
    uint32_t switchConfig; \
    uint8_t  potsType; /*two bits for every pot*/\
    char switchNames[NUM_SWITCHES][LEN_SWITCH_NAME]; \
    char anaNames[NUM_STICKS+NUM_POTS][LEN_ANA_NAME];
#elif defined(PCBFLAMENCO)
  #define LEN_SWITCH_NAME              3
  #define LEN_ANA_NAME                 3
  #define EXTRA_GENERAL_FIELDS \
    EXTRA_GENERAL_FIELDS_ARM \
    uint8_t  serial2Mode:6; \
    uint8_t  spare:2; \
    CustomFunctionData customFn[NUM_CFN]; \
    uint32_t switchConfig; \
    uint8_t  potsType; /*two bits for every pot*/\
    char switchNames[NUM_SWITCHES][LEN_SWITCH_NAME]; \
    char anaNames[NUM_STICKS+NUM_POTS][LEN_ANA_NAME];
#elif defined(PCBTARANIS)
  #define LEN_SWITCH_NAME              3
  #define LEN_ANA_NAME                 3
  #define LEN_BLUETOOTH_NAME           10
  #define HAS_WIRELESS_TRAINER_HARDWARE() (g_eeGeneral.serial2Mode==UART_MODE_SBUS_TRAINER/* || g_eeGeneral.serial2Mode==UART_MODE_CPPM_TRAINER*/)

  #if defined(REV9E)
    #define BLUETOOTH_FIELDS \
      uint8_t bluetoothEnable; \
      char bluetoothName[LEN_BLUETOOTH_NAME];
  #else
    #define BLUETOOTH_FIELDS
  #endif
  #define EXTRA_GENERAL_FIELDS \
    EXTRA_GENERAL_FIELDS_ARM \
    uint8_t  serial2Mode:6; \
    uint8_t  slidersConfig:2; \
    uint8_t  potsConfig; /*two bits for every pot*/\
    uint8_t  backlightColor; \
    swarnstate_t switchUnlockStates; \
    CustomFunctionData customFn[NUM_CFN]; \
    swconfig_t switchConfig; \
    char switchNames[NUM_SWITCHES][LEN_SWITCH_NAME]; \
    char anaNames[NUM_STICKS+NUM_POTS][LEN_ANA_NAME]; \
    BLUETOOTH_FIELDS
#elif defined(CPUARM)
  #define EXTRA_GENERAL_FIELDS \
    EXTRA_GENERAL_FIELDS_ARM \
    CustomFunctionData customFn[NUM_CFN];
#elif defined(PXX)
  #define EXTRA_GENERAL_FIELDS uint8_t  countryCode;
#else
  #define EXTRA_GENERAL_FIELDS
#endif

#define FAILSAFE_CHANNEL_HOLD    2000
#define FAILSAFE_CHANNEL_NOPULSE 2001

PACK(typedef struct {
  uint8_t type:4;
  int8_t  rfProtocol:4;
  uint8_t channelsStart;
  int8_t  channelsCount; // 0=8 channels
  uint8_t failsafeMode;
  int16_t failsafeChannels[NUM_CHNOUT];
  int8_t  ppmDelay:6;
  uint8_t ppmPulsePol:1;
  uint8_t ppmOutputType:1;     // false = open drain, true = push pull
  int8_t  ppmFrameLength;
}) ModuleData;

#define SET_DEFAULT_PPM_FRAME_LENGTH(idx) g_model.moduleData[idx].ppmFrameLength = 4 * max((int8_t)0, g_model.moduleData[idx].channelsCount)

#define LEN_SCRIPT_FILENAME    8
#define LEN_SCRIPT_NAME        8
#define MAX_SCRIPT_INPUTS      8
#define MAX_SCRIPT_OUTPUTS     6
PACK(typedef struct {
  char    file[LEN_SCRIPT_FILENAME];
  char    name[LEN_SCRIPT_NAME];
  int8_t  inputs[MAX_SCRIPT_INPUTS];
}) ScriptData;

enum PotsWarnMode {
  POTS_WARN_OFF,
  POTS_WARN_MANUAL,
  POTS_WARN_AUTO
};

#if defined(PCBHORUS)
  enum ModuleIndex {
    EXTERNAL_MODULE,
    TRAINER_MODULE,
  };
  enum TrainerMode {
    TRAINER_MODE_MASTER,
    TRAINER_MODE_SLAVE
  };
  #define MODELDATA_BITMAP  uint8_t bitmap;
  #define MODELDATA_EXTRA   uint8_t externalModule:3; uint8_t trainerMode:3; uint8_t potsWarnMode:2; ModuleData moduleData[NUM_MODULES+1]; char curveNames[MAX_CURVES][6]; ScriptData scriptsData[MAX_SCRIPTS]; char inputNames[MAX_INPUTS][LEN_INPUT_NAME]; uint8_t potsWarnEnabled; int8_t potsWarnPosition[NUM_POTS];
#elif defined(PCBFLAMENCO)
  enum ModuleIndex {
    EXTERNAL_MODULE,
    TRAINER_MODULE,
  };
  enum TrainerMode {
    TRAINER_MODE_MASTER,
    TRAINER_MODE_SLAVE
  };
  #define MODELDATA_BITMAP  uint8_t bitmap;
  #define MODELDATA_EXTRA   uint8_t externalModule:3; uint8_t trainerMode:3; uint8_t potsWarnMode:2; ModuleData moduleData[NUM_MODULES+1]; char curveNames[MAX_CURVES][6]; ScriptData scriptsData[MAX_SCRIPTS]; char inputNames[MAX_INPUTS][LEN_INPUT_NAME]; uint8_t potsWarnEnabled; int8_t potsWarnPosition[NUM_POTS];
#elif defined(PCBTARANIS)
  enum ModuleIndex {
    INTERNAL_MODULE,
    EXTERNAL_MODULE,
    TRAINER_MODULE
  };
  enum TrainerMode {
    TRAINER_MODE_MASTER,
    TRAINER_MODE_SLAVE,
    TRAINER_MODE_MASTER_SBUS_EXTERNAL_MODULE,
    TRAINER_MODE_MASTER_CPPM_EXTERNAL_MODULE,
    TRAINER_MODE_MASTER_BATTERY_COMPARTMENT,
  };
  #define IS_TRAINER_EXTERNAL_MODULE() (g_model.trainerMode == TRAINER_MODE_MASTER_SBUS_EXTERNAL_MODULE || g_model.trainerMode == TRAINER_MODE_MASTER_CPPM_EXTERNAL_MODULE)
  #define MODELDATA_BITMAP  char bitmap[LEN_BITMAP_NAME];
  #define MODELDATA_EXTRA   uint8_t spare:3; uint8_t trainerMode:3; uint8_t potsWarnMode:2; ModuleData moduleData[NUM_MODULES+1]; char curveNames[MAX_CURVES][6]; ScriptData scriptsData[MAX_SCRIPTS]; char inputNames[MAX_INPUTS][LEN_INPUT_NAME]; uint8_t potsWarnEnabled; int8_t potsWarnPosition[NUM_POTS];
#elif defined(PCBSKY9X)
  enum ModuleIndex {
    EXTERNAL_MODULE,
    EXTRA_MODULE,
    TRAINER_MODULE
  };
  #define MODELDATA_BITMAP
  #define MODELDATA_EXTRA   uint8_t spare:6; uint8_t potsWarnMode:2; ModuleData moduleData[NUM_MODULES+1]; uint8_t potsWarnEnabled; int8_t potsWarnPosition[NUM_POTS]; uint8_t rxBattAlarms[2];
#else
  #define MODELDATA_BITMAP
  #define MODELDATA_EXTRA   
#endif

enum BacklightMode {
  e_backlight_mode_off  = 0,
  e_backlight_mode_keys = 1,
  e_backlight_mode_sticks = 2,
  e_backlight_mode_all = e_backlight_mode_keys+e_backlight_mode_sticks,
  e_backlight_mode_on
};

#if defined(FSPLASH)
  #define SPLASH_MODE uint8_t splashMode:3
#elif defined(PCBTARANIS)
  #define SPLASH_MODE int8_t splashMode:3
#else
  #define SPLASH_MODE uint8_t splashMode:1; uint8_t splashSpare:2
#endif

#define XPOTS_MULTIPOS_COUNT 6

#if defined(PCBTARANIS) || defined(PCBFLAMENCO) || defined(PCBHORUS)
PACK(typedef struct {
  uint8_t count;
  uint8_t steps[XPOTS_MULTIPOS_COUNT-1];
}) StepsCalibData;
#endif

PACK(typedef struct {
  int16_t mid;
  int16_t spanNeg;
  int16_t spanPos;
}) CalibData;


enum Functions {
  // first the functions which need a checkbox
  FUNC_OVERRIDE_CHANNEL,
  FUNC_TRAINER,
  FUNC_INSTANT_TRIM,
  FUNC_RESET,
#if defined(CPUARM)
  FUNC_SET_TIMER,
#endif
  FUNC_ADJUST_GVAR,
#if defined(CPUARM)
  FUNC_VOLUME,
  FUNC_SET_FAILSAFE,
  FUNC_RANGECHECK,
  FUNC_BIND,
#endif
  // then the other functions
  FUNC_FIRST_WITHOUT_ENABLE,
  FUNC_PLAY_SOUND = FUNC_FIRST_WITHOUT_ENABLE,
  FUNC_PLAY_TRACK,
#if !defined(CPUARM)
  FUNC_PLAY_BOTH,
#endif
  FUNC_PLAY_VALUE,
#if defined(CPUARM)
  FUNC_RESERVE4,
  FUNC_PLAY_SCRIPT,
  FUNC_RESERVE5,
  FUNC_BACKGND_MUSIC,
  FUNC_BACKGND_MUSIC_PAUSE,
#endif
  FUNC_VARIO,
  FUNC_HAPTIC,
#if !defined(PCBSTD)
  FUNC_LOGS,
#endif
  FUNC_BACKLIGHT,
#if defined(PCBTARANIS)
  FUNC_SCREENSHOT,
#endif
#if defined(DEBUG)
  FUNC_TEST, // should remain the last before MAX as not added in companion9x
#endif
  FUNC_MAX
};

#if defined(OVERRIDE_CHANNEL_FUNCTION)
  #define HAS_ENABLE_PARAM(func)    ((func) < FUNC_FIRST_WITHOUT_ENABLE)
#else
  #define HAS_ENABLE_PARAM(func)    ((func) < FUNC_FIRST_WITHOUT_ENABLE && (func) != FUNC_OVERRIDE_CHANNEL)
#endif

#if defined(VOICE)
  #define IS_PLAY_FUNC(func)      ((func) >= FUNC_PLAY_SOUND && func <= FUNC_PLAY_VALUE)
#else
  #define IS_PLAY_FUNC(func)      ((func) == FUNC_PLAY_SOUND)
#endif

#if defined(CPUARM)
  #define IS_PLAY_BOTH_FUNC(func) (0)
  #define IS_VOLUME_FUNC(func)    ((func) == FUNC_VOLUME)
#else
  #define IS_PLAY_BOTH_FUNC(func) ((func) == FUNC_PLAY_BOTH)
  #define IS_VOLUME_FUNC(func)    (0)
#endif

#if defined(GVARS)
  #define IS_ADJUST_GV_FUNC(func) ((func) == FUNC_ADJUST_GVAR)
#else
  #define IS_ADJUST_GV_FUNC(func) (0)
#endif

#if defined(HAPTIC)
  #define IS_HAPTIC_FUNC(func)    ((func) == FUNC_HAPTIC)
#else
  #define IS_HAPTIC_FUNC(func)    (0)
#endif

#define HAS_REPEAT_PARAM(func)    (IS_PLAY_FUNC(func) || IS_HAPTIC_FUNC(func))

enum ResetFunctionParam {
  FUNC_RESET_TIMER1,
  FUNC_RESET_TIMER2,
#if defined(CPUARM)
  FUNC_RESET_TIMER3,
#endif
  FUNC_RESET_FLIGHT,
#if defined(FRSKY)
  FUNC_RESET_TELEMETRY,
#endif
#if ROTARY_ENCODERS > 0
  FUNC_RESET_ROTENC1,
#endif
#if ROTARY_ENCODERS > 1
  FUNC_RESET_ROTENC2,
#endif
#if defined(CPUARM)
  FUNC_RESET_PARAM_FIRST_TELEM,
  FUNC_RESET_PARAM_LAST_TELEM = FUNC_RESET_PARAM_FIRST_TELEM + MAX_SENSORS,
#endif
  FUNC_RESET_PARAMS_COUNT,
  FUNC_RESET_PARAM_LAST = FUNC_RESET_PARAMS_COUNT-1,
};

enum AdjustGvarFunctionParam {
  FUNC_ADJUST_GVAR_CONSTANT,
  FUNC_ADJUST_GVAR_SOURCE,
  FUNC_ADJUST_GVAR_GVAR,
  FUNC_ADJUST_GVAR_INC,
};

#if defined(CPUARM)
#if defined(PCBTARANIS)
  #define LEN_CFN_NAME 8
  #define CFN_SPARE_TYPE int32_t
#else
  #define LEN_CFN_NAME 6
  #define CFN_SPARE_TYPE int16_t
#endif
PACK(typedef struct {
  int16_t  swtch:9;
  uint16_t func:7;
  PACK(union {
    PACK(struct {
      char name[LEN_CFN_NAME];
    }) play;

    PACK(struct {
      int16_t val;
      uint8_t mode;
      uint8_t param;
      CFN_SPARE_TYPE spare2;
    }) all;

    PACK(struct {
      int32_t val1;
      CFN_SPARE_TYPE val2;
    }) clear;
  });
  uint8_t active;
}) CustomFunctionData;
#define CFN_EMPTY(p)            (!(p)->swtch)
#define CFN_SWITCH(p)           ((p)->swtch)
#define CFN_FUNC(p)             ((p)->func)
#define CFN_ACTIVE(p)           ((p)->active)
#define CFN_CH_INDEX(p)         ((p)->all.param)
#define CFN_GVAR_INDEX(p)       ((p)->all.param)
#define CFN_TIMER_INDEX(p)      ((p)->all.param)
#define CFN_PLAY_REPEAT(p)      ((p)->active)
#define CFN_PLAY_REPEAT_MUL     1
#define CFN_PLAY_REPEAT_NOSTART 0xFF
#define CFN_GVAR_MODE(p)        ((p)->all.mode)
#define CFN_PARAM(p)            ((p)->all.val)
#define CFN_RESET(p)            ((p)->active=0, (p)->clear.val1=0, (p)->clear.val2=0)
#define CFN_GVAR_CST_MAX        GVAR_LIMIT
#elif defined(CPUM2560)
PACK(typedef struct {
  int8_t  swtch;
  uint8_t func;
  uint8_t mode:2;
  uint8_t param:4;
  uint8_t active:1;
  uint8_t spare:1;
  uint8_t value;
}) CustomFunctionData;
#define CFN_SWITCH(p)       ((p)->swtch)
#define CFN_FUNC(p)         ((p)->func)
#define CFN_ACTIVE(p)       ((p)->active)
#define CFN_CH_INDEX(p)     ((p)->param)
#define CFN_TIMER_INDEX(p)  ((p)->param)
#define CFN_GVAR_INDEX(p)   ((p)->param)
#define CFN_PLAY_REPEAT(p)  ((p)->param)
#define CFN_PLAY_REPEAT_MUL 10
#define CFN_GVAR_MODE(p)    ((p)->mode)
#define CFN_PARAM(p)        ((p)->value)
#define CFN_RESET(p)        ((p)->active = 0, CFN_PARAM(p) = 0)
#define CFN_GVAR_CST_MAX    125
#else
PACK(typedef struct {
  PACK(union {
    PACK(struct {
      int16_t   swtch:6;
      uint16_t  func:4;
      uint16_t  mode:2;
      uint16_t  param:3;
      uint16_t  active:1;
    }) gvar;

    PACK(struct {
      int16_t   swtch:6;
      uint16_t  func:4;
      uint16_t  param:4;
      uint16_t  spare:1;
      uint16_t  active:1;
    }) all;
  });
  uint8_t value;
}) CustomFunctionData;
#define CFN_SWITCH(p)       ((p)->all.swtch)
#define CFN_FUNC(p)         ((p)->all.func)
#define CFN_ACTIVE(p)       ((p)->all.active)
#define CFN_CH_INDEX(p)     ((p)->all.param)
#define CFN_TIMER_INDEX(p)  ((p)->all.param)
#define CFN_GVAR_INDEX(p)   ((p)->gvar.param)
#define CFN_PLAY_REPEAT(p)  ((p)->all.param)
#define CFN_PLAY_REPEAT_MUL 10
#define CFN_GVAR_MODE(p)    ((p)->gvar.mode)
#define CFN_PARAM(p)        ((p)->value)
#define CFN_RESET(p)        ((p)->all.active = 0, CFN_PARAM(p) = 0)
#define CFN_GVAR_CST_MAX    125
#endif

#if defined(PCBTARANIS) || defined(PCBFLAMENCO) || defined(PCBHORUS)
  enum SwitchConfig {
    SWITCH_NONE,
    SWITCH_TOGGLE,
    SWITCH_2POS,
    SWITCH_3POS,
  };
  enum PotConfig {
    POT_NONE,
    POT_WITH_DETENT,
    POT_MULTIPOS_SWITCH,
    POT_WITHOUT_DETENT
  };
  enum SliderConfig {
    SLIDER_NONE,
    SLIDER_WITH_DETENT,
  };
  #define SWITCH_CONFIG(x)            ((g_eeGeneral.switchConfig >> (2*(x))) & 0x03)
  #define SWITCH_EXISTS(x)            (SWITCH_CONFIG(x) != SWITCH_NONE)
  #define IS_3POS(x)                  (SWITCH_CONFIG(x) == SWITCH_3POS)
  #define IS_TOGGLE(x)                (SWITCH_CONFIG(x) == SWITCH_TOGGLE)
  #define IS_3POS_MIDDLE(x)           ((x) == 1)
  #define SWITCH_WARNING_ALLOWED(x)   (SWITCH_EXISTS(x) && !IS_TOGGLE(x))
#endif

#define ALTERNATE_VIEW 0x10
PACK(typedef struct {
  uint8_t   version;
  uint16_t  variant;
  CalibData calib[NUM_STICKS+NUM_POTS];
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
  uint8_t   adjustRTC:1;
  uint8_t   inactivityTimer;
  uint8_t   mavbaud:3;
  SPLASH_MODE; /* 3bits */
  int8_t    hapticMode:2;    // -2=quiet, -1=only alarms, 0=no keys, 1=all
  AVR_FIELD(uint8_t blOffBright:4)
  AVR_FIELD(uint8_t blOnBright:4)
  ARM_FIELD(int8_t switchesDelay)
  uint8_t   lightAutoOff;
  uint8_t   templateSetup;   // RETA order for receiver channels
  int8_t    PPM_Multiplier;
  int8_t    hapticLength;
  N_PCBSTD_FIELD( uint8_t   reNavigation)
  N_TARANIS_FIELD(uint8_t   stickReverse)
  int8_t    beepLength:3;
  int8_t    hapticStrength:3;
  uint8_t   gpsFormat:1;
  uint8_t   unexpectedShutdown:1;
  uint8_t   speakerPitch;
  int8_t    speakerVolume;
  int8_t    vBatMin;
  int8_t    vBatMax;

  EXTRA_GENERAL_FIELDS

}) EEGeneral;

#define SWITCHES_DELAY()            uint8_t(15+g_eeGeneral.switchesDelay)
#define SWITCHES_DELAY_NONE         (-15)
#define HAPTIC_STRENGTH()           (3+g_eeGeneral.hapticStrength)

enum CurveRefType {
  CURVE_REF_DIFF,
  CURVE_REF_EXPO,
  CURVE_REF_FUNC,
  CURVE_REF_CUSTOM
};

PACK(typedef struct {
  uint8_t type;
  int8_t  value;
}) CurveRef;

#if !defined(XCURVES)
  #define MODE_DIFFERENTIAL  0
  #define MODE_EXPO          0
  #define MODE_CURVE         1
#endif

#if defined(PCBTARANIS) || defined(PCBFLAMENCO) || defined(PCBHORUS)
PACK(typedef struct {
  uint16_t mode:2;
  uint16_t scale:14;
  uint16_t srcRaw:10;
  int16_t  carryTrim:6;
  uint32_t chn:5;
  int32_t  swtch:9;
  uint32_t flightModes:9;
  int32_t  weight:8;
  int32_t  spare:1;
  char     name[LEN_EXPOMIX_NAME];
  int8_t   offset;
  CurveRef curve;
}) ExpoData;
#define MIN_EXPO_WEIGHT         -100
#define EXPO_VALID(ed)          ((ed)->mode)
#define EXPO_MODE_ENABLE(ed, v) (((v)<0 && ((ed)->mode&1)) || ((v)>=0 && ((ed)->mode&2)))
#elif defined(CPUARM)
PACK(typedef struct {
  uint16_t mode:2;         // 0=end, 1=pos, 2=neg, 3=both
  uint16_t chn:3;
  uint16_t curveMode:2;
  uint16_t flightModes:9;
  int8_t   swtch;
  int8_t   weight;
  char     name[LEN_EXPOMIX_NAME];
  int8_t   curveParam;
}) ExpoData;
#define MIN_EXPO_WEIGHT         0
#define EXPO_VALID(ed)          ((ed)->mode)
#define EXPO_MODE_ENABLE(ed, v) (((v)<0 && ((ed)->mode&1)) || ((v)>=0 && ((ed)->mode&2)))
#elif defined(CPUM2560) || defined(CPUM2561)
PACK(typedef struct {
  uint8_t mode:2;         // 0=end, 1=pos, 2=neg, 3=both
  uint8_t chn:2;
  uint8_t curveMode:1;
  uint8_t spare:3;
  uint8_t flightModes;
  int8_t  swtch;
  uint8_t weight;
  int8_t  curveParam;
}) ExpoData;
#define MIN_EXPO_WEIGHT         0
#define EXPO_VALID(ed)          ((ed)->mode)
#define EXPO_MODE_ENABLE(ed, v) (((v)<0 && ((ed)->mode&1)) || ((v)>=0 && ((ed)->mode&2)))
#else
PACK(typedef struct {
  uint8_t mode:2;         // 0=end, 1=pos, 2=neg, 3=both
  int8_t  swtch:6;
  uint8_t chn:2;
  uint8_t flightModes:5;
  uint8_t curveMode:1;
  uint8_t weight;         // One spare bit here (used for GVARS)
  int8_t  curveParam;
}) ExpoData;
#define MIN_EXPO_WEIGHT         0
#define EXPO_VALID(ed)          ((ed)->mode)
#define EXPO_MODE_ENABLE(ed, v) (((v)<0 && ((ed)->mode&1)) || ((v)>=0 && ((ed)->mode&2)))
#endif

#if defined(PCBTARANIS) || defined(PCBFLAMENCO) || defined(PCBHORUS)
  #define limit_min_max_t     int16_t
  #define LIMIT_EXT_PERCENT   150
  #define LIMIT_EXT_MAX       (LIMIT_EXT_PERCENT*10)
  #define PPM_CENTER_MAX      500
  #define LIMIT_MAX(lim)      (GV_IS_GV_VALUE(lim->max, -GV_RANGELARGE, GV_RANGELARGE) ? GET_GVAR(lim->max, -LIMIT_EXT_MAX, LIMIT_EXT_MAX, mixerCurrentFlightMode)*10 : lim->max+1000)
  #define LIMIT_MIN(lim)      (GV_IS_GV_VALUE(lim->min, -GV_RANGELARGE, GV_RANGELARGE) ? GET_GVAR(lim->min, -LIMIT_EXT_MAX, LIMIT_EXT_MAX, mixerCurrentFlightMode)*10 : lim->min-1000)
  #define LIMIT_OFS(lim)      (GV_IS_GV_VALUE(lim->offset, -1000, 1000) ? GET_GVAR(lim->offset, -1000, 1000, mixerCurrentFlightMode)*10 : lim->offset)
  #define LIMIT_MAX_RESX(lim) calc1000toRESX(LIMIT_MAX(lim))
  #define LIMIT_MIN_RESX(lim) calc1000toRESX(LIMIT_MIN(lim))
  #define LIMIT_OFS_RESX(lim) calc1000toRESX(LIMIT_OFS(lim))
#else
  #define limit_min_max_t     int8_t
  #define LIMIT_EXT_PERCENT   125
  #define LIMIT_EXT_MAX       LIMIT_EXT_PERCENT
  #define PPM_CENTER_MAX      125
  #define LIMIT_MAX(lim)      (lim->max+100)
  #define LIMIT_MIN(lim)      (lim->min-100)
  #define LIMIT_OFS(lim)      (lim->offset)
  #define LIMIT_MAX_RESX(lim) calc100toRESX(LIMIT_MAX(lim))
  #define LIMIT_MIN_RESX(lim) calc100toRESX(LIMIT_MIN(lim))
  #define LIMIT_OFS_RESX(lim) calc1000toRESX(LIMIT_OFS(lim))
#endif

#if defined(PCBTARANIS) || defined(PCBFLAMENCO) || defined(PCBHORUS)
PACK(typedef struct {
  int32_t min:11;
  int32_t max:11;
  int32_t ppmCenter:10;
  int16_t offset:11;
  uint16_t symetrical:1;
  uint16_t revert:1;
  uint16_t spare:3;
  int8_t curve;
  char name[LEN_CHANNEL_NAME];
}) LimitData;
#else
PACK(typedef struct {
  int8_t min;
  int8_t max;
  int8_t  ppmCenter;
  int16_t offset:14;
  uint16_t symetrical:1;
  uint16_t revert:1;
}) LimitData;
#endif

#define TRIM_OFF    (1)
#define TRIM_ON     (0)
#define TRIM_RUD    (-1)
#define TRIM_ELE    (-2)
#define TRIM_THR    (-3)
#define TRIM_AIL    (-4)

#define MLTPX_ADD   0
#define MLTPX_MUL   1
#define MLTPX_REP   2

#if defined(CPUARM)
#define GV1_SMALL       128
#define GV1_LARGE       1024
#define GV_RANGE_WEIGHT 500
#define GV_RANGE_OFFSET 500
#define DELAY_STEP      10
#define SLOW_STEP       10
#define DELAY_MAX       (25*DELAY_STEP) /* 25 seconds */
#define SLOW_MAX        (25*SLOW_STEP)  /* 25 seconds */
#if defined(PCBTARANIS) || defined(PCBFLAMENCO) || defined(PCBHORUS)
PACK(typedef struct {
  int16_t  weight:11;       // GV1=-1024, -GV1=1023
  uint16_t destCh:5;
  uint16_t srcRaw:10;       // srcRaw=0 means not used
  uint16_t carryTrim:1;
  uint16_t mixWarn:2;       // mixer warning
  uint16_t mltpx:2;         // multiplex method: 0 means +=, 1 means *=, 2 means :=
  uint16_t spare:1;
  int32_t  offset:14;
  int32_t  swtch:9;
  uint32_t flightModes:9;
  CurveRef curve;
  uint8_t  delayUp;
  uint8_t  delayDown;
  uint8_t  speedUp;
  uint8_t  speedDown;
  char     name[LEN_EXPOMIX_NAME];
}) MixData;
#else
PACK(typedef struct {
  uint8_t  destCh:5;
  uint8_t  mixWarn:3;         // mixer warning
  uint16_t flightModes:9;
  uint16_t curveMode:1;
  uint16_t noExpo:1;
  int16_t  carryTrim:3;
  uint16_t mltpx:2;           // multiplex method: 0 means +=, 1 means *=, 2 means :=
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
}) MixData;
#endif
#define MD_WEIGHT(md) (md->weight)
#define MD_WEIGHT_TO_UNION(md, var) var.word = md->weight
#define MD_UNION_TO_WEIGHT(var, md) md->weight = var.word

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

// highest bit used for small values in mix 128 --> 8 bit is enough
#define GV1_SMALL  128
// highest bit used for large values in mix 256 --> 9 bits is used (8 bits + 1 extra bit from weightMode/offsetMode)
#define GV1_LARGE  256

#define DELAY_STEP  2
#define SLOW_STEP   2
#define DELAY_MAX   15 /* 7.5 seconds */
#define SLOW_MAX    15 /* 7.5 seconds */

#if defined(CPUM2560) || defined(CPUM2561)
PACK(typedef struct {
  uint8_t destCh:4;          // 0, 1..NUM_CHNOUT
  uint8_t curveMode:1;       // O=curve, 1=differential
  uint8_t noExpo:1;
  uint8_t weightMode:1;
  uint8_t offsetMode:1;
  uint8_t srcRaw;
  int8_t  weight;
  int8_t  swtch;
  uint8_t flightModes;
  uint8_t mltpx:2;           // multiplex method: 0 means +=, 1 means *=, 2 means :=
  int8_t  carryTrim:3;
  uint8_t mixWarn:2;         // mixer warning
  uint8_t spare:1;
  uint8_t delayUp:4;
  uint8_t delayDown:4;
  uint8_t speedUp:4;
  uint8_t speedDown:4;
  int8_t  curveParam;
  int8_t  offset;
}) MixData;
#else
PACK(typedef struct {
  uint8_t destCh:4;          // 0, 1..NUM_CHNOUT
  uint8_t curveMode:1;       // O=curve, 1=differential
  uint8_t noExpo:1;
  uint8_t weightMode:1;
  uint8_t offsetMode:1;
  int8_t  weight;
  int8_t  swtch:6;
  uint8_t mltpx:2;           // multiplex method: 0 means +=, 1 means *=, 2 means :=
  uint8_t flightModes:5;
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
#endif
PACK(union u_gvarint_t {
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
  
PACK(union u_int8int16_t {
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

enum LogicalSwitchesFunctions {
  LS_FUNC_NONE,
#if defined(CPUARM)
  LS_FUNC_VEQUAL, // v==offset
#endif
  LS_FUNC_VALMOSTEQUAL, // v~=offset
  LS_FUNC_VPOS,   // v>offset
  LS_FUNC_VNEG,   // v<offset
#if defined(CPUARM)
  LS_FUNC_RANGE,
#endif
  LS_FUNC_APOS,   // |v|>offset
  LS_FUNC_ANEG,   // |v|<offset
  LS_FUNC_AND,
  LS_FUNC_OR,
  LS_FUNC_XOR,
#if defined(CPUARM)
  LS_FUNC_EDGE,
#endif
  LS_FUNC_EQUAL,
  LS_FUNC_GREATER,
  LS_FUNC_LESS,
  LS_FUNC_DIFFEGREATER,
  LS_FUNC_ADIFFEGREATER,
  LS_FUNC_TIMER,
  LS_FUNC_STICKY,
  LS_FUNC_COUNT,
  LS_FUNC_MAX = LS_FUNC_COUNT-1
};

#if defined(CPUARM)
#define MAX_LS_DURATION 250 /*25s*/
#define MAX_LS_DELAY    250 /*25s*/
#define MAX_LS_ANDSW    SWSRC_LAST
typedef int16_t ls_telemetry_value_t;
PACK(typedef struct { // Logical Switches data
  uint8_t  func;
  int32_t  v1:10;  
  int32_t  v3:10;
  int32_t  andsw:9;      // TODO rename to xswtch
  uint32_t andswtype:1;  // TODO rename to xswtchType (AND / OR)
  uint32_t spare:2;      // anything else needed?
  int16_t  v2;
  uint8_t  delay;
  uint8_t  duration;
}) LogicalSwitchData;
#else
typedef uint8_t ls_telemetry_value_t;
#define MAX_LS_ANDSW    15
PACK(typedef struct { // Logical Switches data
  int8_t  v1; //input
  int8_t  v2; //offset
  uint8_t func:4;
  uint8_t andsw:4;
}) LogicalSwitchData;
#endif

#if defined(CPUARM)
enum TelemetryUnit {
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
  UNIT_DB,
  UNIT_RPMS,
  UNIT_G,
  UNIT_DEGREE,
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
#define UNIT_MAX UNIT_FLOZ
#define UNIT_DIST UNIT_METERS
#define UNIT_TEMPERATURE UNIT_CELSIUS
#define UNIT_SPEED UNIT_KMH
#else
enum TelemetryUnit {
  UNIT_VOLTS,
  UNIT_AMPS,
  UNIT_METERS_PER_SECOND,
  UNIT_RAW,
  UNIT_SPEED,
  UNIT_DIST,
  UNIT_TEMPERATURE,
  UNIT_PERCENT,
  UNIT_MILLIAMPS,
  UNIT_A1A2_MAX = UNIT_MILLIAMPS,
  UNIT_MAH,
  UNIT_WATTS,
  UNIT_DB,
  UNIT_FEET,
  UNIT_KTS,
  UNIT_HOURS,
  UNIT_MINUTES,
  UNIT_SECONDS,
  UNIT_RPMS,
  UNIT_G,
  UNIT_HDG,
};
#endif

#if defined(CPUARM)
PACK(typedef struct {
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
PACK(typedef struct {
  uint8_t   ratio;              // 0.0 means not used, 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  int16_t   offset:12;
  uint16_t  type:4;             // channel unit (0=volts, ...)
  uint8_t   alarms_value[2];    // 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc. 
  uint8_t   alarms_level:4;
  uint8_t   alarms_greater:2;   // 0=LT(<), 1=GT(>)
  uint8_t   multiplier:2;       // 0=no multiplier, 1=*2 multiplier
}) FrSkyChannelData;
#endif

#if defined(CPUARM)
#define TELEM_LABEL_LEN           4
//#define TELEM_FLAG_TIMEOUT      0x01
#define TELEM_FLAG_LOG            0x02
//#define TELEM_FLAG_PERSISTENT   0x04
//#define TELEM_FLAG_SCALE        0x08
#define TELEM_FLAG_AUTO_OFFSET    0x10
#define TELEM_FLAG_FILTER         0x20
#define TELEM_FLAG_LOSS_ALARM     0x40

enum TelemetrySensorType
{
  TELEM_TYPE_CUSTOM,
  TELEM_TYPE_CALCULATED
};

enum TelemetrySensorFormula
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

PACK(typedef struct {
  union {
    uint16_t id;                   // data identifier, for FrSky we can reuse existing ones. Source unit is derived from type.
    uint16_t persistentValue;
  };
  union {
    uint8_t instance;              // instance ID to allow handling multiple instances of same value type, for FrSky can be the physical ID of the sensor
    uint8_t formula;
  };
  char     label[TELEM_LABEL_LEN]; // user defined label
  uint8_t  type:1;                 // 0=custom / 1=calculated
  uint8_t  unit:5;                 // user can choose what unit to display each value in
  uint8_t  prec:2;
  uint8_t  autoOffset:1;
  uint8_t  filter:1;
  uint8_t  logs:1;
  uint8_t  persistent:1;
  uint8_t  onlyPositive:1;
  uint8_t  spare:3;
  union {
    PACK(struct {
      uint16_t ratio;
      int16_t  offset;
    }) custom;
    PACK(struct {
      uint8_t source;
      uint8_t index;
      uint16_t spare;
    }) cell;
    PACK(struct {
      int8_t sources[4];
    }) calc;
    PACK(struct {
      uint8_t source;
      uint8_t spare[3];
    }) consumption;
    PACK(struct {
      uint8_t gps;
      uint8_t alt;
      uint16_t spare;
    }) dist;
    uint32_t param;
  };
  void init(const char *label, uint8_t unit=UNIT_RAW, uint8_t prec=0);
  void init(uint16_t id);
  bool isAvailable();
  int32_t getValue(int32_t value, uint8_t unit, uint8_t prec) const;
  bool isConfigurable();
  bool isPrecConfigurable();
}) TelemetrySensor;
#endif

#if !defined(CPUARM)
enum TelemetrySource {
  TELEM_NONE,
  TELEM_TX_VOLTAGE,
  TELEM_TIMER1,
  TELEM_TIMER2,
  TELEM_TIMER_MAX=TELEM_TIMER2,
  TELEM_RSSI_TX,
  TELEM_RSSI_RX,
  TELEM_A_FIRST,
  TELEM_A1=TELEM_A_FIRST,
  TELEM_A2,
  TELEM_A_LAST=TELEM_A2,
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
  TELEM_VSPEED,
  TELEM_ASPEED,
  TELEM_DTE,
  TELEM_MIN_A_FIRST,
  TELEM_MIN_A1=TELEM_MIN_A_FIRST,
  TELEM_MIN_A2,
  TELEM_MIN_A_LAST=TELEM_MIN_A2,
  TELEM_MIN_ALT,
  TELEM_MAX_ALT,
  TELEM_MAX_RPM,
  TELEM_MAX_T1,
  TELEM_MAX_T2,
  TELEM_MAX_SPEED,
  TELEM_MAX_DIST,
  TELEM_MAX_ASPEED,
  TELEM_MIN_CELL,
  TELEM_MIN_CELLS_SUM,
  TELEM_MIN_VFAS,
  TELEM_MAX_CURRENT,
  TELEM_MAX_POWER,
  TELEM_ACC,
  TELEM_GPS_TIME,
  TELEM_CSW_MAX = TELEM_MAX_POWER,
  TELEM_NOUSR_MAX = TELEM_A2,
#if defined(FRSKY)
  TELEM_DISPLAY_MAX = TELEM_MAX_POWER,
#else
  TELEM_DISPLAY_MAX = TELEM_TIMER2, // because used also in PlayValue
#endif
  TELEM_STATUS_MAX = TELEM_GPS_TIME,
  TELEM_FIRST_STREAMED_VALUE = TELEM_RSSI_TX,
};
#endif

enum VarioSource {
#if !defined(FRSKY_SPORT)
  VARIO_SOURCE_ALTI,
  VARIO_SOURCE_ALTI_PLUS,
#endif
  VARIO_SOURCE_VSPEED,
  VARIO_SOURCE_A1,
  VARIO_SOURCE_A2,
#if defined(FRSKY_SPORT)
  VARIO_SOURCE_DTE,
#endif
  VARIO_SOURCE_COUNT,
  VARIO_SOURCE_LAST = VARIO_SOURCE_COUNT-1
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
  #define NUM_TELEMETRY      TELEM_TIMER2
#endif

PACK(typedef struct {
  source_t source;
  ls_telemetry_value_t barMin;           // minimum for bar display
  ls_telemetry_value_t barMax;           // ditto for max display (would usually = ratio)
}) FrSkyBarData;

#if defined(PCBTARANIS) || defined(PCBFLAMENCO) || defined(PCBHORUS)
  #define NUM_LINE_ITEMS 3
#else
  #define NUM_LINE_ITEMS 2
#endif

PACK(typedef struct {
  source_t sources[NUM_LINE_ITEMS];
}) FrSkyLineData;

#if defined(PCBTARANIS) || defined(PCBFLAMENCO) || defined(PCBHORUS)
#define MAX_TELEM_SCRIPT_INPUTS  8
PACK(typedef struct {
  char    file[LEN_SCRIPT_FILENAME];
  int16_t inputs[MAX_TELEM_SCRIPT_INPUTS];
}) TelemetryScriptData;
#endif

typedef union {
  FrSkyBarData  bars[4];
  FrSkyLineData lines[4];
#if defined(PCBTARANIS) || defined(PCBFLAMENCO) || defined(PCBHORUS)
  TelemetryScriptData script;
#endif
} FrSkyScreenData;

enum FrskyUsrProtocols {
  USR_PROTO_NONE,
  USR_PROTO_FRSKY,
  USR_PROTO_WS_HOW_HIGH,
  USR_PROTO_LAST = USR_PROTO_WS_HOW_HIGH,
};

enum FrskyCurrentSource {
  FRSKY_CURRENT_SOURCE_NONE,
  FRSKY_CURRENT_SOURCE_A1,
  FRSKY_CURRENT_SOURCE_A2,
#if defined(CPUARM)
  FRSKY_CURRENT_SOURCE_A3,
  FRSKY_CURRENT_SOURCE_A4,
#endif
  FRSKY_CURRENT_SOURCE_FAS,
  FRSKY_CURRENT_SOURCE_LAST=FRSKY_CURRENT_SOURCE_FAS
};

enum FrskyVoltsSource {
  FRSKY_VOLTS_SOURCE_A1,
  FRSKY_VOLTS_SOURCE_A2,
#if defined(CPUARM)
  FRSKY_VOLTS_SOURCE_A3,
  FRSKY_VOLTS_SOURCE_A4,
#endif
  FRSKY_VOLTS_SOURCE_FAS,
  FRSKY_VOLTS_SOURCE_CELLS,
  FRSKY_VOLTS_SOURCE_LAST=FRSKY_VOLTS_SOURCE_CELLS
};

#if defined(CPUARM)
enum TelemetryScreenType {
  TELEMETRY_SCREEN_TYPE_NONE,
  TELEMETRY_SCREEN_TYPE_VALUES,
  TELEMETRY_SCREEN_TYPE_GAUGES,
#if defined(LUA)
  TELEMETRY_SCREEN_TYPE_SCRIPT,
  TELEMETRY_SCREEN_TYPE_MAX = TELEMETRY_SCREEN_TYPE_SCRIPT
#else
  TELEMETRY_SCREEN_TYPE_MAX = TELEMETRY_SCREEN_TYPE_GAUGES
#endif
};
#define MAX_TELEMETRY_SCREENS 4
#define TELEMETRY_SCREEN_TYPE(screenIndex) TelemetryScreenType((g_model.frsky.screensType >> (2*(screenIndex))) & 0x03)
#define IS_BARS_SCREEN(screenIndex)        (TELEMETRY_SCREEN_TYPE(screenIndex) == TELEMETRY_SCREEN_TYPE_GAUGES)
PACK(typedef struct {
  uint8_t voltsSource;
  uint8_t altitudeSource;
  uint8_t screensType; // 2bits per screen (None/Gauges/Numbers/Script)
  FrSkyScreenData screens[MAX_TELEMETRY_SCREENS];
  uint8_t varioSource:7;
  uint8_t varioCenterSilent:1;
  int8_t  varioCenterMax;
  int8_t  varioCenterMin;
  int8_t  varioMin;
  int8_t  varioMax;
  FrSkyRSSIAlarm rssiAlarms[2];
}) FrSkyData;
#define MIN_BLADES -1   // 1 blade
#define MAX_BLADES 126  // 128 blades
#else
#define MAX_FRSKY_A_CHANNELS 2
#define MAX_TELEMETRY_SCREENS 2
#define IS_BARS_SCREEN(screenIndex) (g_model.frsky.screensType & (1<<(screenIndex)))
PACK(typedef struct {
  FrSkyChannelData channels[MAX_FRSKY_A_CHANNELS];
  uint8_t usrProto:2; // Protocol in FrSky user data, 0=None, 1=FrSky hub, 2=WS HowHigh, 3=Halcyon
  uint8_t blades:2;   // How many blades for RPMs, 0=2 blades
  uint8_t screensType:2;
  uint8_t voltsSource:2;
  int8_t  varioMin:4;
  int8_t  varioMax:4;
  FrSkyRSSIAlarm rssiAlarms[2];
  FrSkyScreenData screens[MAX_TELEMETRY_SCREENS];
  uint8_t varioSource:3;
  int8_t  varioCenterMin:5;
  uint8_t currentSource:3;
  int8_t  varioCenterMax:5;
  int8_t  fasOffset;
}) FrSkyData;
#define MIN_BLADES 0 // 2 blades
#define MAX_BLADES 3 // 5 blades
#endif

#if defined(MAVLINK)
PACK(typedef struct t_MavlinkData {
  uint8_t rc_rssi_scale:4;
  uint8_t pc_rssi_en:1;
  uint8_t spare1:3;
  uint8_t spare2[3];
}) MavlinkData;
#endif

enum SwashType {
  SWASH_TYPE_NONE,
  SWASH_TYPE_120,
  SWASH_TYPE_120X,
  SWASH_TYPE_140,
  SWASH_TYPE_90,
  SWASH_TYPE_MAX = SWASH_TYPE_90
};

#if defined(VIRTUALINPUTS)
PACK(typedef struct {
  uint8_t   type;
  uint8_t   value;
  uint8_t   collectiveSource;
  uint8_t   aileronSource;
  uint8_t   elevatorSource;
  int8_t    collectiveWeight;
  int8_t    aileronWeight;
  int8_t    elevatorWeight;
}) SwashRingData;
#else
PACK(typedef struct {
  uint8_t   invertELE:1;
  uint8_t   invertAIL:1;
  uint8_t   invertCOL:1;
  uint8_t   type:5;  
  uint8_t   collectiveSource;
  uint8_t   value;
}) SwashRingData;
#endif

#define TRIM_EXTENDED_MAX 500
#define TRIM_EXTENDED_MIN (-TRIM_EXTENDED_MAX)
#define TRIM_MAX 125
#define TRIM_MIN (-TRIM_MAX)

#define ROTARY_ENCODER_MAX  1024

#if defined(PCBTARANIS)
  #define NUM_ROTARY_ENCODERS 0
  #define ROTARY_ENCODER_ARRAY
#elif defined(REVX)
  #define NUM_ROTARY_ENCODERS 0
  #define ROTARY_ENCODER_ARRAY int16_t rotaryEncoders[1];
#elif defined(PCBSKY9X)
  #define NUM_ROTARY_ENCODERS 1
  #define ROTARY_ENCODER_ARRAY int16_t rotaryEncoders[1];
#elif defined(CPUM2560)
  #define NUM_ROTARY_ENCODERS 2
  #define ROTARY_ENCODER_ARRAY int16_t rotaryEncoders[2];
#else
  #define NUM_ROTARY_ENCODERS 0
  #define ROTARY_ENCODER_ARRAY
#endif

#if defined(PCBSTD)
  #define TRIMS_ARRAY       int8_t trim[4]; int8_t trim_ext:8
  #define TRIMS_ARRAY_SIZE  5
  #define trim_t            int16_t
#else
  #if defined(PCBTARANIS) || defined(PCBFLAMENCO) || defined(PCBHORUS)
    PACK(typedef struct {
      int16_t  value:11;
      uint16_t mode:5;
    }) trim_t;
    #define TRIM_MODE_NONE  0x1F  // 0b11111
  #else
    #define trim_t          int16_t
  #endif
  #define TRIMS_ARRAY       trim_t trim[4]
  #define TRIMS_ARRAY_SIZE  8
#endif

#if defined(CPUARM)
PACK(typedef struct {
  TRIMS_ARRAY;
  char name[LEN_FLIGHT_MODE_NAME];
  int16_t  swtch:9;       // swtch of phase[0] is not used
  int16_t  spare:7;
  uint8_t  fadeIn;
  uint8_t  fadeOut;
  ROTARY_ENCODER_ARRAY;
  PHASE_GVARS_DATA;
}) FlightModeData;
#else
PACK(typedef struct {
  TRIMS_ARRAY;
  int8_t swtch;       // swtch of phase[0] is not used
  char name[LEN_FLIGHT_MODE_NAME];
  uint8_t fadeIn:4;
  uint8_t fadeOut:4;
  ROTARY_ENCODER_ARRAY;
  PHASE_GVARS_DATA;
}) FlightModeData;
#endif

enum SwitchSources {
  SWSRC_NONE = 0,

  SWSRC_FIRST_SWITCH,

#if defined(PCBFLAMENCO)
  SWSRC_SA0 = SWSRC_FIRST_SWITCH,
  SWSRC_SA1,
  SWSRC_SA2,
  SWSRC_SB0,
  SWSRC_SB2,
  SWSRC_SC0,
  SWSRC_SC1,
  SWSRC_SC2,
  SWSRC_SC3,
  SWSRC_SC4,
  SWSRC_SC5,
  SWSRC_SE0,
  SWSRC_SE2,
  SWSRC_SF0,
  SWSRC_SF1,
  SWSRC_SF2,
  SWSRC_LAST_SWITCH = SWSRC_SF2,
#elif defined(PCBTARANIS)
  SWSRC_SA0 = SWSRC_FIRST_SWITCH,
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
  SWSRC_SF1,
  SWSRC_SF2,
  SWSRC_SG0,
  SWSRC_SG1,
  SWSRC_SG2,
  SWSRC_SH0,
  SWSRC_SH1,
  SWSRC_SH2,
  SWSRC_TRAINER = SWSRC_SH2,
#if defined(REV9E)
  SWSRC_SI0,
  SWSRC_SI1,
  SWSRC_SI2,
  SWSRC_SJ0,
  SWSRC_SJ1,
  SWSRC_SJ2,
  SWSRC_SK0,
  SWSRC_SK1,
  SWSRC_SK2,
  SWSRC_SL0,
  SWSRC_SL1,
  SWSRC_SL2,
  SWSRC_SM0,
  SWSRC_SM1,
  SWSRC_SM2,
  SWSRC_SN0,
  SWSRC_SN1,
  SWSRC_SN2,
  SWSRC_SO0,
  SWSRC_SO1,
  SWSRC_SO2,
  SWSRC_SP0,
  SWSRC_SP1,
  SWSRC_SP2,
  SWSRC_SQ0,
  SWSRC_SQ1,
  SWSRC_SQ2,
  SWSRC_SR0,
  SWSRC_SR1,
  SWSRC_SR2,
  SWSRC_LAST_SWITCH = SWSRC_SR2,
#else
  SWSRC_LAST_SWITCH = SWSRC_SH2,
#endif
#else
  SWSRC_ID0 = SWSRC_FIRST_SWITCH,
  SWSRC_ID1,
  SWSRC_ID2,
  SWSRC_THR,
  SWSRC_RUD,
  SWSRC_ELE,
  SWSRC_AIL,
  SWSRC_GEA,
  SWSRC_TRN,
  SWSRC_TRAINER = SWSRC_TRN,
  SWSRC_LAST_SWITCH = SWSRC_TRN,
#endif

#if NUM_XPOTS > 0
  SWSRC_FIRST_MULTIPOS_SWITCH,
  SWSRC_LAST_MULTIPOS_SWITCH = SWSRC_FIRST_MULTIPOS_SWITCH + (NUM_XPOTS*XPOTS_MULTIPOS_COUNT) - 1,
#endif

  SWSRC_FIRST_TRIM,
  SWSRC_TrimRudLeft = SWSRC_FIRST_TRIM,
  SWSRC_TrimRudRight,
  SWSRC_TrimEleDown,
  SWSRC_TrimEleUp,
  SWSRC_TrimThrDown,
  SWSRC_TrimThrUp,
  SWSRC_TrimAilLeft,
  SWSRC_TrimAilRight,
  SWSRC_LAST_TRIM = SWSRC_TrimAilRight,

#if defined(PCBSKY9X)
  SWSRC_REa,
#elif defined(CPUM2560)
  SWSRC_REa,
  SWSRC_REb,
#endif

  SWSRC_FIRST_LOGICAL_SWITCH,
  SWSRC_SW1 = SWSRC_FIRST_LOGICAL_SWITCH,
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
  SWSRC_LAST_LOGICAL_SWITCH = SWSRC_FIRST_LOGICAL_SWITCH+NUM_LOGICAL_SWITCH-1,

  SWSRC_ON,
  SWSRC_ONE,

#if defined(CPUARM)
  SWSRC_FIRST_FLIGHT_MODE,
  SWSRC_LAST_FLIGHT_MODE = SWSRC_FIRST_FLIGHT_MODE+MAX_FLIGHT_MODES-1,

  SWSRC_TELEMETRY_STREAMING,
  SWSRC_FIRST_SENSOR,
  SWSRC_LAST_SENSOR = SWSRC_FIRST_SENSOR+MAX_SENSORS-1,
#endif

  SWSRC_COUNT,

  SWSRC_OFF = -SWSRC_ON,

  SWSRC_LAST = SWSRC_COUNT-1,
  SWSRC_FIRST = -SWSRC_LAST,

#if defined(CPUARM)
  SWSRC_LAST_IN_LOGICAL_SWITCHES = SWSRC_COUNT-1,
  SWSRC_LAST_IN_MIXES = SWSRC_COUNT-1,
#else
  SWSRC_LAST_IN_LOGICAL_SWITCHES = SWSRC_LAST_LOGICAL_SWITCH,
  SWSRC_LAST_IN_MIXES = SWSRC_LAST_LOGICAL_SWITCH,
#endif

  SWSRC_FIRST_IN_LOGICAL_SWITCHES = -SWSRC_LAST_IN_LOGICAL_SWITCHES,
  SWSRC_FIRST_IN_MIXES = -SWSRC_LAST_IN_MIXES,

#if defined(CPUARM)
  SWSRC_INVERT = SWSRC_COUNT+1,
#endif
};

enum MixSources {
  MIXSRC_NONE,

#if defined(VIRTUALINPUTS)
  MIXSRC_FIRST_INPUT,             LUA_EXPORT_MULTIPLE("input", "Input [I%d]", MAX_INPUTS)
  MIXSRC_LAST_INPUT = MIXSRC_FIRST_INPUT+MAX_INPUTS-1,

  MIXSRC_FIRST_LUA,
  MIXSRC_LAST_LUA = MIXSRC_FIRST_LUA+(MAX_SCRIPTS*MAX_SCRIPT_OUTPUTS)-1,
#endif

  MIXSRC_FIRST_STICK,
  MIXSRC_Rud = MIXSRC_FIRST_STICK,      LUA_EXPORT("rud", "Rudder")
  MIXSRC_Ele,                           LUA_EXPORT("ele", "Elevator")
  MIXSRC_Thr,                           LUA_EXPORT("thr", "Throttle")
  MIXSRC_Ail,                           LUA_EXPORT("ail", "Aileron")

  MIXSRC_FIRST_POT,
#if defined(PCBFLAMENCO)
  MIXSRC_POT1 = MIXSRC_FIRST_POT,       LUA_EXPORT("sd", "Potentiometer D")
  MIXSRC_SLIDER1,                       LUA_EXPORT("ls", "Left slider")
  MIXSRC_SLIDER2,                       LUA_EXPORT("rs", "Right slider")
  MIXSRC_LAST_POT = MIXSRC_SLIDER2,
#elif defined(PCBTARANIS)
  MIXSRC_POT1 = MIXSRC_FIRST_POT,       LUA_EXPORT("s1", "Potentiometer 1")
  MIXSRC_POT2,                          LUA_EXPORT("s2", "Potentiometer 2")
  MIXSRC_POT3,                          LUA_EXPORT("s3", "Potentiometer 3")
  #if defined(REV9E)
    MIXSRC_POT4,                        LUA_EXPORT("s4", "Potentiometer 4")
  #endif
  MIXSRC_FIRST_SLIDER,
  MIXSRC_SLIDER1 = MIXSRC_FIRST_SLIDER, LUA_EXPORT("ls", "Left slider")
  MIXSRC_SLIDER2,                       LUA_EXPORT("rs", "Right slider")
  #if defined(REV9E)
    MIXSRC_SLIDER3,                     LUA_EXPORT("ls2", "Left center slider")
    MIXSRC_SLIDER4,                     LUA_EXPORT("rs2", "Right center slider")
    MIXSRC_LAST_POT = MIXSRC_SLIDER4,
  #else
    MIXSRC_LAST_POT = MIXSRC_SLIDER2,
  #endif
#else
  MIXSRC_P1 = MIXSRC_FIRST_POT,
  MIXSRC_P2,
    MIXSRC_P3,
    MIXSRC_LAST_POT = MIXSRC_P3,
  #endif

#if defined(PCBSKY9X)
  MIXSRC_REa,
  MIXSRC_LAST_ROTARY_ENCODER = MIXSRC_REa,
#elif defined(CPUM2560)
  MIXSRC_REa,
  MIXSRC_REb,
  #if ROTARY_ENCODERS > 2
    MIXSRC_REc,
    MIXSRC_REd,
    MIXSRC_LAST_ROTARY_ENCODER = MIXSRC_REd,
  #else
    MIXSRC_LAST_ROTARY_ENCODER = MIXSRC_REb,
  #endif
#endif

  MIXSRC_MAX,

  MIXSRC_FIRST_HELI,
  MIXSRC_CYC1 = MIXSRC_FIRST_HELI,   LUA_EXPORT("cyc1", "Cyclic 1")
  MIXSRC_CYC2,                       LUA_EXPORT("cyc2", "Cyclic 2")
  MIXSRC_CYC3,                       LUA_EXPORT("cyc3", "Cyclic 3")

  MIXSRC_FIRST_TRIM,
  MIXSRC_TrimRud = MIXSRC_FIRST_TRIM,  LUA_EXPORT("trim-rud", "Rudder trim")
  MIXSRC_TrimEle,                      LUA_EXPORT("trim-ele", "Elevator trim")
  MIXSRC_TrimThr,                      LUA_EXPORT("trim-thr", "Throttle trim")
  MIXSRC_TrimAil,                      LUA_EXPORT("trim-ail", "Aileron trim")
  MIXSRC_LAST_TRIM = MIXSRC_TrimAil,

  MIXSRC_FIRST_SWITCH,

#if defined(PCBFLAMENCO)
  MIXSRC_SA = MIXSRC_FIRST_SWITCH,  LUA_EXPORT("sa", "Switch A")
  MIXSRC_SB,                        LUA_EXPORT("sb", "Switch B")
  MIXSRC_SC,                        LUA_EXPORT("sc", "Switch C")
  MIXSRC_SE,                        LUA_EXPORT("se", "Switch E")
  MIXSRC_SF,                        LUA_EXPORT("sf", "Switch F")
#elif defined(PCBTARANIS) || defined(PCBHORUS)
  MIXSRC_SA = MIXSRC_FIRST_SWITCH,  LUA_EXPORT("sa", "Switch A")
  MIXSRC_SB,                        LUA_EXPORT("sb", "Switch B")
  MIXSRC_SC,                        LUA_EXPORT("sc", "Switch C")
  MIXSRC_SD,                        LUA_EXPORT("sd", "Switch D")
  MIXSRC_SE,                        LUA_EXPORT("se", "Switch E")
  MIXSRC_SF,                        LUA_EXPORT("sf", "Switch F")
  MIXSRC_SG,                        LUA_EXPORT("sg", "Switch G")
  MIXSRC_SH,                        LUA_EXPORT("sh", "Switch H")
#if defined(REV9E)
  MIXSRC_SI,                        LUA_EXPORT("si", "Switch I")
  MIXSRC_SJ,                        LUA_EXPORT("sj", "Switch J")
  MIXSRC_SK,                        LUA_EXPORT("sk", "Switch K")
  MIXSRC_SL,                        LUA_EXPORT("sl", "Switch L")
  MIXSRC_SM,                        LUA_EXPORT("sm", "Switch M")
  MIXSRC_SN,                        LUA_EXPORT("sn", "Switch N")
  MIXSRC_SO,                        LUA_EXPORT("so", "Switch O")
  MIXSRC_SP,                        LUA_EXPORT("sp", "Switch P")
  MIXSRC_SQ,                        LUA_EXPORT("sq", "Switch Q")
  MIXSRC_SR,                        LUA_EXPORT("sr", "Switch R")
  MIXSRC_LAST_SWITCH = MIXSRC_SR,
#else
  MIXSRC_LAST_SWITCH = MIXSRC_SH,
#endif
#else
  MIXSRC_3POS = MIXSRC_FIRST_SWITCH,
  MIXSRC_THR,
  MIXSRC_RUD,
  MIXSRC_ELE,
  MIXSRC_AIL,
  MIXSRC_GEA,
  MIXSRC_TRN,
  MIXSRC_LAST_SWITCH = MIXSRC_TRN,
#endif
  MIXSRC_FIRST_LOGICAL_SWITCH,
  MIXSRC_SW1 = MIXSRC_FIRST_LOGICAL_SWITCH, LUA_EXPORT_MULTIPLE("ls", "Logical switch L%d", NUM_LOGICAL_SWITCH)
  MIXSRC_SW9 = MIXSRC_SW1 + 8,
  MIXSRC_SWA,
  MIXSRC_SWB,
  MIXSRC_SWC,
  MIXSRC_LAST_LOGICAL_SWITCH = MIXSRC_FIRST_LOGICAL_SWITCH+NUM_LOGICAL_SWITCH-1,

  MIXSRC_FIRST_TRAINER,                     LUA_EXPORT_MULTIPLE("trn", "Trainer input %d", NUM_TRAINER)
  MIXSRC_LAST_TRAINER = MIXSRC_FIRST_TRAINER+NUM_TRAINER-1,

  MIXSRC_FIRST_CH,
  MIXSRC_CH1 = MIXSRC_FIRST_CH,             LUA_EXPORT_MULTIPLE("ch", "Channel CH%d", NUM_CHNOUT)
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

  MIXSRC_FIRST_GVAR,
  MIXSRC_GVAR1 = MIXSRC_FIRST_GVAR,         LUA_EXPORT_MULTIPLE("gvar", "Global variable %d", MAX_GVARS)
  MIXSRC_LAST_GVAR = MIXSRC_FIRST_GVAR+MAX_GVARS-1,

#if defined(CPUARM)
  MIXSRC_TX_VOLTAGE,                        LUA_EXPORT("tx-voltage", "Transmitter battery voltage [volts]")
  MIXSRC_TX_TIME,                           LUA_EXPORT("clock", "RTC clock [minutes from midnight]")
  MIXSRC_RESERVE1,
  MIXSRC_RESERVE2,
  MIXSRC_RESERVE3,
  MIXSRC_RESERVE4,
  MIXSRC_RESERVE5,
  MIXSRC_FIRST_TIMER,
  MIXSRC_TIMER1 = MIXSRC_FIRST_TIMER,       LUA_EXPORT("timer1", "Timer 1 value [seconds]")
  MIXSRC_TIMER2,                            LUA_EXPORT("timer2", "Timer 2 value [seconds]")
  MIXSRC_TIMER3,                            LUA_EXPORT("timer3", "Timer 3 value [seconds]")
  MIXSRC_LAST_TIMER = MIXSRC_TIMER3,
#endif

  MIXSRC_FIRST_TELEM,
#if defined(CPUARM)
  MIXSRC_LAST_TELEM = MIXSRC_FIRST_TELEM+3*MAX_SENSORS-1
#else
  MIXSRC_LAST_TELEM = MIXSRC_FIRST_TELEM+NUM_TELEMETRY-1
#endif
};

#define MIXSRC_FIRST   (MIXSRC_NONE+1)
#define MIXSRC_LAST    MIXSRC_LAST_CH
#define INPUTSRC_FIRST MIXSRC_Rud
#define INPUTSRC_LAST  MIXSRC_LAST_TELEM

#define MIN_POINTS 3
#define MAX_POINTS 17

enum TimerModes {
  TMRMODE_NONE,
  TMRMODE_ABS,
  TMRMODE_THR,
  TMRMODE_THR_REL,
  TMRMODE_THR_TRG,
  TMRMODE_COUNT
};

enum CountDownModes {
  COUNTDOWN_SILENT,
  COUNTDOWN_BEEPS,
  COUNTDOWN_VOICE,
#if defined(CPUARM) && defined(HAPTIC)
  COUNTDOWN_HAPTIC,
#endif
  COUNTDOWN_COUNT
};

#if defined(CPUARM)
PACK(typedef struct {
  int32_t  mode:9;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint32_t start:23;
  int32_t  value:24;
  uint32_t countdownBeep:2;
  uint32_t minuteBeep:1;
  uint32_t persistent:2;
  uint32_t spare:3;
  char     name[LEN_TIMER_NAME];
}) TimerData;
#define IS_MANUAL_RESET_TIMER(idx) (g_model.timers[idx].persistent == 2)
#elif defined(CPUM2560)
PACK(typedef struct {
  int8_t   mode;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint16_t start;
  uint8_t  countdownBeep:2;
  uint8_t  minuteBeep:1;
  uint8_t  persistent:2;
  uint8_t  spare:3;
  uint16_t value;
}) TimerData;
#define IS_MANUAL_RESET_TIMER(idx) (g_model.timers[idx].persistent == 2)
#else
PACK(typedef struct {
  int8_t    mode;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint16_t  start:12;
  uint16_t  countdownBeep:1;
  uint16_t  minuteBeep:1;
  uint16_t  spare:2;
}) TimerData;
#define IS_MANUAL_RESET_TIMER(idx) 0
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

enum RFProtocols {
  RF_PROTO_OFF = -1,
  RF_PROTO_X16,
#if defined(MODULE_D16_EU_ONLY_SUPPORT)
  RF_PROTO_LAST = RF_PROTO_X16
#else
  RF_PROTO_D8,
  RF_PROTO_LR12,
  RF_PROTO_LAST = RF_PROTO_LR12
#endif
};

#if defined(MODULE_D16_EU_ONLY_SUPPORT)
  #define HAS_RF_PROTOCOL_FAILSAFE(protocol) ((protocol) == RF_PROTO_X16)
#else
  #define HAS_RF_PROTOCOL_FAILSAFE(protocol) ((protocol) == RF_PROTO_X16 || (protocol) == RF_PROTO_LR12)
#endif

enum DSM2Protocols {
  DSM2_PROTO_LP45,
  DSM2_PROTO_DSM2,
  DSM2_PROTO_DSMX,
};

enum ModuleTypes {
  MODULE_TYPE_NONE = 0,
  MODULE_TYPE_PPM,
  MODULE_TYPE_XJT,
#if defined(DSM2)
  MODULE_TYPE_DSM2,
#endif
  MODULE_TYPE_COUNT
};

#define IS_PULSES_EXTERNAL_MODULE() (g_model.moduleData[EXTERNAL_MODULE].type != MODULE_TYPE_NONE)

enum FailsafeModes {
  FAILSAFE_NOT_SET,
  FAILSAFE_HOLD,
  FAILSAFE_CUSTOM,
  FAILSAFE_NOPULSES,
  FAILSAFE_RECEIVER,
  FAILSAFE_LAST = FAILSAFE_RECEIVER
};

#if defined(MAVLINK)
  #define TELEMETRY_DATA MavlinkData mavlink;
#elif defined(FRSKY) || !defined(PCBSTD)
  #define TELEMETRY_DATA FrSkyData frsky;
#else
  #define TELEMETRY_DATA
#endif

#if defined(CPUARM) || defined(CPUM2560)
  #define BeepANACenter uint16_t
#else
  #define BeepANACenter uint8_t
#endif

PACK(typedef struct {
  char      name[LEN_MODEL_NAME]; // must be first for eeLoadModelName
  uint8_t   modelId[NUM_MODULES];
  MODELDATA_BITMAP
}) ModelHeader;

enum ThrottleSources {
  THROTTLE_SOURCE_THR,
  THROTTLE_SOURCE_FIRST_POT,
#if defined(PCBFLAMENCO)
  THROTTLE_SOURCE_SD,
  THROTTLE_SOURCE_LS,
  THROTTLE_SOURCE_RS,
#elif defined(PCBTARANIS) && defined(REV9E)
  THROTTLE_SOURCE_F1 = THROTTLE_SOURCE_FIRST_POT,
  THROTTLE_SOURCE_F2,
  THROTTLE_SOURCE_F3,
  THROTTLE_SOURCE_F4,
  THROTTLE_SOURCE_S1,
  THROTTLE_SOURCE_S2,
  THROTTLE_SOURCE_LS,
  THROTTLE_SOURCE_RS,
#elif defined(PCBTARANIS)
  THROTTLE_SOURCE_S1 = THROTTLE_SOURCE_FIRST_POT,
  THROTTLE_SOURCE_S2,
  THROTTLE_SOURCE_S3,
  THROTTLE_SOURCE_LS,
  THROTTLE_SOURCE_RS,
#else
  THROTTLE_SOURCE_P1 = THROTTLE_SOURCE_FIRST_POT,
  THROTTLE_SOURCE_P2,
  THROTTLE_SOURCE_P3,
#endif
  THROTTLE_SOURCE_CH1,
};

enum TelemetryType
{
  PROTOCOL_TELEMETRY_FIRST,
  PROTOCOL_FRSKY_SPORT = PROTOCOL_TELEMETRY_FIRST,
  PROTOCOL_FRSKY_D,
  PROTOCOL_FRSKY_D_SECONDARY
};

enum DisplayTrims
{
  DISPLAY_TRIMS_NEVER,
  DISPLAY_TRIMS_CHANGE,
  DISPLAY_TRIMS_ALWAYS
};

PACK(typedef struct {
  ModelHeader header;
  TimerData timers[MAX_TIMERS];
  AVR_FIELD(uint8_t   protocol:3)
  ARM_FIELD(uint8_t   telemetryProtocol:3)
  uint8_t   thrTrim:1;            // Enable Throttle Trim
  AVR_FIELD(int8_t    ppmNCH:4)
  ARM_FIELD(uint8_t   noGlobalFunctions:1)
  ARM_FIELD(uint8_t   displayTrims:2)
  ARM_FIELD(uint8_t   ignoreSensorIds:1)
  int8_t    trimInc:3;            // Trim Increments
  uint8_t   disableThrottleWarning:1;
  ARM_FIELD(uint8_t displayChecklist:1)
  AVR_FIELD(uint8_t pulsePol:1)
  uint8_t   extendedLimits:1;
  uint8_t   extendedTrims:1;
  uint8_t   throttleReversed:1;
  AVR_FIELD(int8_t ppmDelay)
  BeepANACenter beepANACenter;
  MixData   mixData[MAX_MIXERS];
  LimitData limitData[NUM_CHNOUT];
  ExpoData  expoData[MAX_EXPOS];
  
  CURVDATA  curves[MAX_CURVES];
  int8_t    points[NUM_POINTS];
  
  LogicalSwitchData logicalSw[NUM_LOGICAL_SWITCH];
  CustomFunctionData customFn[NUM_CFN];
  SwashRingData swashR;
  FlightModeData flightModeData[MAX_FLIGHT_MODES];

  AVR_FIELD(int8_t ppmFrameLength)     // 0=22.5ms  (10ms-30ms) 0.5ms increments

  uint8_t thrTraceSrc;

  swarnstate_t  switchWarningState;
  swarnenable_t switchWarningEnable;

  MODEL_GVARS_DATA

  TELEMETRY_DATA

  MODELDATA_EXTRA

  ARM_FIELD(TelemetrySensor telemetrySensors[MAX_SENSORS])
  
  TARANIS_REV9E_FIELD(uint8_t topLcdTimer)
}) ModelData;

extern EEGeneral g_eeGeneral;
extern ModelData g_model;

#define TOTAL_EEPROM_USAGE (sizeof(ModelData)*MAX_MODELS + sizeof(EEGeneral))

#endif
