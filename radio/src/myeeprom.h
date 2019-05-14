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

#ifndef _MYEEPROM_H_
#define _MYEEPROM_H_

#include "datastructs.h"
#include "bitfield.h"

#define EEPROM_VER             219
#define FIRST_CONV_EEPROM_VER  216

#define GET_MODULE_PPM_POLARITY(idx)             g_model.moduleData[idx].ppm.pulsePol
#define GET_TRAINER_PPM_POLARITY()               g_model.trainerData.pulsePol
#define GET_SBUS_POLARITY(idx)                   g_model.moduleData[idx].sbus.noninverted
#define GET_MODULE_PPM_DELAY(idx)                (g_model.moduleData[idx].ppm.delay * 50 + 300)
#define GET_TRAINER_PPM_DELAY()                  (g_model.trainerData.delay * 50 + 300)
#define SET_DEFAULT_PPM_FRAME_LENGTH(idx)        g_model.moduleData[idx].ppm.frameLength = 4 * max((int8_t)0, g_model.moduleData[idx].channelsCount)

#if defined(PCBHORUS)
  #define IS_TRAINER_EXTERNAL_MODULE()    false
  #define HAS_WIRELESS_TRAINER_HARDWARE() (g_eeGeneral.serial2Mode==UART_MODE_SBUS_TRAINER)
#elif defined(PCBTARANIS)
  #define IS_TRAINER_EXTERNAL_MODULE()      (g_model.trainerData.mode == TRAINER_MODE_MASTER_SBUS_EXTERNAL_MODULE || g_model.trainerData.mode == TRAINER_MODE_MASTER_CPPM_EXTERNAL_MODULE)
  #define HAS_WIRELESS_TRAINER_HARDWARE()   (g_eeGeneral.serial2Mode==UART_MODE_SBUS_TRAINER)
#else
  #define IS_TRAINER_EXTERNAL_MODULE()    false
#endif

#define IS_PLAY_FUNC(func)             ((func) >= FUNC_PLAY_SOUND && func <= FUNC_PLAY_VALUE)

#if defined(GVARS)
  #define IS_ADJUST_GV_FUNC(func)      ((func) == FUNC_ADJUST_GVAR)
#else
  #define IS_ADJUST_GV_FUNC(func)      (0)
#endif

#if defined(HAPTIC)
  #define IS_HAPTIC_FUNC(func)         ((func) == FUNC_HAPTIC)
#else
  #define IS_HAPTIC_FUNC(func)         (0)
#endif

#define HAS_ENABLE_PARAM(func)         ((func) < FUNC_FIRST_WITHOUT_ENABLE)
#define HAS_REPEAT_PARAM(func)         (IS_PLAY_FUNC(func) || IS_HAPTIC_FUNC(func))

#define CFN_EMPTY(p)                   (!(p)->swtch)
#define CFN_SWITCH(p)                  ((p)->swtch)
#define CFN_FUNC(p)                    ((p)->func)
#define CFN_ACTIVE(p)                  ((p)->active)
#define CFN_CH_INDEX(p)                ((p)->all.param)
#define CFN_GVAR_INDEX(p)              ((p)->all.param)
#define CFN_TIMER_INDEX(p)             ((p)->all.param)
#define CFN_PLAY_REPEAT(p)             ((p)->active)
#define CFN_PLAY_REPEAT_MUL            1
#define CFN_PLAY_REPEAT_NOSTART        0xFF
#define CFN_GVAR_MODE(p)               ((p)->all.mode)
#define CFN_PARAM(p)                   ((p)->all.val)
#define CFN_RESET(p)                   ((p)->active=0, (p)->clear.val1=0, (p)->clear.val2=0)
#define CFN_GVAR_CST_MIN               -GVAR_MAX
#define CFN_GVAR_CST_MAX               GVAR_MAX
#define MODEL_GVAR_MIN(idx)            (CFN_GVAR_CST_MIN + g_model.gvars[idx].min)
#define MODEL_GVAR_MAX(idx)            (CFN_GVAR_CST_MAX - g_model.gvars[idx].max)

#if defined(PCBTARANIS) || defined(PCBHORUS)
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
  #define SWITCH_CONFIG(x)            (BF_GET<uint32_t>(g_eeGeneral.switchConfig, 2*(x), 2))
  #define SWITCH_EXISTS(x)            (SWITCH_CONFIG(x) != SWITCH_NONE)
  #define IS_CONFIG_3POS(x)           (SWITCH_CONFIG(x) == SWITCH_3POS)
  #define IS_CONFIG_TOGGLE(x)         (SWITCH_CONFIG(x) == SWITCH_TOGGLE)
  #define SWITCH_WARNING_ALLOWED(x)   (SWITCH_EXISTS(x) && !IS_CONFIG_TOGGLE(x))
#else
  #define IS_CONFIG_3POS(x)           IS_3POS(x)
  #define IS_CONFIG_TOGGLE(x)         IS_TOGGLE(x)
  #define switchInfo(x)               ((x) >= 3 ? (x)-2 : 0)
  #define SWITCH_EXISTS(x)            true
#endif

#define ALTERNATE_VIEW                0x10

#if defined(PCBHORUS)
  #include "layout.h"
  #include "theme.h"
  #include "topbar.h"
#else
  #define THEME_DATA
#endif

#define SWITCHES_DELAY()            uint8_t(15+g_eeGeneral.switchesDelay)
#define SWITCHES_DELAY_NONE         (-15)
#define HAPTIC_STRENGTH()           (3+g_eeGeneral.hapticStrength)

enum CurveRefType {
  CURVE_REF_DIFF,
  CURVE_REF_EXPO,
  CURVE_REF_FUNC,
  CURVE_REF_CUSTOM
};

#define MIN_EXPO_WEIGHT         -100
#define EXPO_VALID(ed)          ((ed)->mode)
#define EXPO_MODE_ENABLE(ed, v) (((v)<0 && ((ed)->mode&1)) || ((v)>=0 && ((ed)->mode&2)))

#define limit_min_max_t     int16_t
#define LIMIT_EXT_PERCENT   150
#define LIMIT_EXT_MAX       (LIMIT_EXT_PERCENT*10)
#define PPM_CENTER_MAX      500
#define LIMIT_MAX(lim)      (GV_IS_GV_VALUE(lim->max, -GV_RANGELARGE, GV_RANGELARGE) ? GET_GVAR_PREC1(lim->max, -LIMIT_EXT_MAX, LIMIT_EXT_MAX, mixerCurrentFlightMode) : lim->max+1000)
#define LIMIT_MIN(lim)      (GV_IS_GV_VALUE(lim->min, -GV_RANGELARGE, GV_RANGELARGE) ? GET_GVAR_PREC1(lim->min, -LIMIT_EXT_MAX, LIMIT_EXT_MAX, mixerCurrentFlightMode) : lim->min-1000)
#define LIMIT_OFS(lim)      (GV_IS_GV_VALUE(lim->offset, -1000, 1000) ? GET_GVAR_PREC1(lim->offset, -1000, 1000, mixerCurrentFlightMode) : lim->offset)
#define LIMIT_MAX_RESX(lim) calc1000toRESX(LIMIT_MAX(lim))
#define LIMIT_MIN_RESX(lim) calc1000toRESX(LIMIT_MIN(lim))
#define LIMIT_OFS_RESX(lim) calc1000toRESX(LIMIT_OFS(lim))

#define TRIM_OFF    (1)
#define TRIM_ON     (0)
#define TRIM_RUD    (-1)
#define TRIM_ELE    (-2)
#define TRIM_THR    (-3)
#define TRIM_AIL    (-4)
#if defined(PCBHORUS)
  #define TRIM_T5   (-5)
  #define TRIM_T6   (-6)
  #define TRIM_LAST TRIM_T6
#else
  #define TRIM_LAST TRIM_AIL
#endif

#define MLTPX_ADD   0
#define MLTPX_MUL   1
#define MLTPX_REP   2

#define GV1_SMALL       128
#define GV1_LARGE       1024
#define GV_RANGE_WEIGHT 500
#define GV_RANGE_OFFSET 500
#define DELAY_STEP      10
#define SLOW_STEP       10
#define DELAY_MAX       (25*DELAY_STEP) /* 25 seconds */
#define SLOW_MAX        (25*SLOW_STEP)  /* 25 seconds */

#define MD_WEIGHT(md) (md->weight)
#define MD_WEIGHT_TO_UNION(md, var) var.word = md->weight
#define MD_UNION_TO_WEIGHT(var, md) md->weight = var.word

#define MD_OFFSET(md) (md->offset)
#define MD_OFFSET_TO_UNION(md, var) var.word = md->offset
#define MD_UNION_TO_OFFSET(var, md) md->offset = var.word

enum LogicalSwitchesFunctions {
  LS_FUNC_NONE,
  LS_FUNC_VEQUAL, // v==offset
  LS_FUNC_VALMOSTEQUAL, // v~=offset
  LS_FUNC_VPOS,   // v>offset
  LS_FUNC_VNEG,   // v<offset
  LS_FUNC_RANGE,
  LS_FUNC_APOS,   // |v|>offset
  LS_FUNC_ANEG,   // |v|<offset
  LS_FUNC_AND,
  LS_FUNC_OR,
  LS_FUNC_XOR,
  LS_FUNC_EDGE,
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

#define MAX_LS_DURATION 250 /*25s*/
#define MAX_LS_DELAY    250 /*25s*/
#define MAX_LS_ANDSW    SWSRC_LAST

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

enum VarioSource {
  VARIO_SOURCE_VSPEED,
  VARIO_SOURCE_A1,
  VARIO_SOURCE_A2,
  VARIO_SOURCE_DTE,
  VARIO_SOURCE_COUNT,
  VARIO_SOURCE_LAST = VARIO_SOURCE_COUNT-1
};

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
  FRSKY_CURRENT_SOURCE_A3,
  FRSKY_CURRENT_SOURCE_A4,
  FRSKY_CURRENT_SOURCE_FAS,
  FRSKY_CURRENT_SOURCE_LAST=FRSKY_CURRENT_SOURCE_FAS
};

enum FrskyVoltsSource {
  FRSKY_VOLTS_SOURCE_A1,
  FRSKY_VOLTS_SOURCE_A2,
  FRSKY_VOLTS_SOURCE_A3,
  FRSKY_VOLTS_SOURCE_A4,
  FRSKY_VOLTS_SOURCE_FAS,
  FRSKY_VOLTS_SOURCE_CELLS,
  FRSKY_VOLTS_SOURCE_LAST=FRSKY_VOLTS_SOURCE_CELLS
};

enum SwashType {
  SWASH_TYPE_NONE,
  SWASH_TYPE_120,
  SWASH_TYPE_120X,
  SWASH_TYPE_140,
  SWASH_TYPE_90,
  SWASH_TYPE_MAX = SWASH_TYPE_90
};

#define TRIM_EXTENDED_MAX 500
#define TRIM_EXTENDED_MIN (-TRIM_EXTENDED_MAX)
#define TRIM_MAX 125
#define TRIM_MIN (-TRIM_MAX)

#define ROTARY_ENCODER_MAX  1024

#define TRIMS_ARRAY_SIZE  8
#define TRIM_MODE_NONE  0x1F  // 0b11111

#define IS_MANUAL_RESET_TIMER(idx)     (g_model.timers[idx].persistent == 2)

#if !defined(PCBSKY9X)
#define TIMER_COUNTDOWN_START(x)       (g_model.timers[x].countdownStart > 0 ? 5 : 10 - g_model.timers[x].countdownStart * 10)
#else
#define TIMER_COUNTDOWN_START(x)       10
#endif

enum XJTRFProtocols {
  RF_PROTO_OFF = -1,
  RF_PROTO_X16,
  RF_PROTO_D8,
  RF_PROTO_LR12,
  RF_PROTO_LAST = RF_PROTO_LR12
};

enum R9MSubTypes
{
  MODULE_SUBTYPE_R9M_FCC,
  MODULE_SUBTYPE_R9M_EU,
  MODULE_SUBTYPE_R9M_EUPLUS,
  MODULE_SUBTYPE_R9M_AUPLUS,
  MODULE_SUBTYPE_R9M_LAST=MODULE_SUBTYPE_R9M_AUPLUS
};

enum R9MRegion
{
  MODULE_R9M_REGION_FCC,
  MODULE_R9M_REGION_EU,
  MODULE_R9M_REGION_FLEX,
  MODULE_R9M_REGION_LAST=MODULE_R9M_REGION_FLEX
};

enum R9MFrenquency
{
  MODULE_R9M_FREQ_868MHZ,
  MODULE_R9M_FREQ_915MHZ,
  MODULE_R9M_FREQ_LAST=MODULE_R9M_FREQ_915MHZ
};

enum MultiModuleRFProtocols {
  MM_RF_PROTO_CUSTOM = -1,
  MM_RF_PROTO_FIRST = MM_RF_PROTO_CUSTOM,
  MM_RF_PROTO_FLYSKY=0,
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
  MM_RF_PROTO_FS_AFHDS2A,
  MM_RF_PROTO_Q2X2,
  MM_RF_PROTO_WK_2X01,
  MM_RF_PROTO_Q303,
  MM_RF_PROTO_GW008,
  MM_RF_PROTO_DM002,
  MM_RF_PROTO_CABELL,
  MM_RF_PROTO_ESKY150,
  MM_RF_PROTO_H83D,
  MM_RF_PROTO_CORONA,
  MM_RF_PROTO_CFLIE,
  MM_RF_PROTO_HITEC,
  MM_RF_PROTO_WFLY,
  MM_RF_PROTO_BUGS,
  MM_RF_PROTO_BUGS_MINI,
  MM_RF_PROTO_TRAXXAS,
  MM_RF_PROTO_NCC1701,
  MM_RF_PROTO_E01X,
  MM_RF_PROTO_V911S,
  MM_RF_PROTO_GD00X,
  MM_RF_PROTO_LAST = MM_RF_PROTO_GD00X
};

enum MMDSM2Subtypes {
  MM_RF_DSM2_SUBTYPE_DSM2_22,
  MM_RF_DSM2_SUBTYPE_DSM2_11,
  MM_RF_DSM2_SUBTYPE_DSMX_22,
  MM_RF_DSM2_SUBTYPE_DSMX_11,
  MM_RF_DSM2_SUBTYPE_AUTO
};

enum MMRFrskySubtypes {
  MM_RF_FRSKY_SUBTYPE_D16,
  MM_RF_FRSKY_SUBTYPE_D8,
  MM_RF_FRSKY_SUBTYPE_D16_8CH,
  MM_RF_FRSKY_SUBTYPE_V8,
  MM_RF_FRSKY_SUBTYPE_D16_LBT,
  MM_RF_FRSKY_SUBTYPE_D16_LBT_8CH
};

#define HAS_RF_PROTOCOL_FAILSAFE(rf)   ((rf) == RF_PROTO_X16)
#define HAS_RF_PROTOCOL_MODELINDEX(rf) (((rf) == RF_PROTO_X16) || ((rf) == RF_PROTO_LR12))

enum DSM2Protocols {
  DSM2_PROTO_LP45,
  DSM2_PROTO_DSM2,
  DSM2_PROTO_DSMX,
};

enum ModuleTypes {
  MODULE_TYPE_NONE = 0,
  MODULE_TYPE_PPM,
  MODULE_TYPE_PXX_XJT,
  MODULE_TYPE_ACCESS_ISRM,
  MODULE_TYPE_DSM2,
  MODULE_TYPE_CROSSFIRE,
  MODULE_TYPE_MULTIMODULE,
  MODULE_TYPE_PXX_R9M,
  MODULE_TYPE_ACCESS_R9M,
  MODULE_TYPE_PXX_R9M_LITE,
  MODULE_TYPE_ACCESS_R9M_LITE,
  MODULE_TYPE_ACCESS_R9M_LITE_PRO,
  MODULE_TYPE_SBUS,
  MODULE_TYPE_MAX = MODULE_TYPE_SBUS,
  MODULE_TYPE_COUNT
};

enum AntennaTypes {
  XJT_INTERNAL_ANTENNA,
  XJT_EXTERNAL_ANTENNA
};

enum FailsafeModes {
  FAILSAFE_NOT_SET,
  FAILSAFE_HOLD,
  FAILSAFE_CUSTOM,
  FAILSAFE_NOPULSES,
  FAILSAFE_RECEIVER,
  FAILSAFE_LAST = FAILSAFE_RECEIVER
};

enum ThrottleSources {
  THROTTLE_SOURCE_THR,
  THROTTLE_SOURCE_FIRST_POT,
#if defined(PCBX9E)
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

enum TelemetryProtocols
{
  PROTOCOL_TELEMETRY_FIRST,
  PROTOCOL_TELEMETRY_FRSKY_SPORT = PROTOCOL_TELEMETRY_FIRST,
  PROTOCOL_TELEMETRY_FRSKY_D,
  PROTOCOL_TELEMETRY_FRSKY_D_SECONDARY,
  PROTOCOL_TELEMETRY_CROSSFIRE,
  PROTOCOL_TELEMETRY_SPEKTRUM,
  PROTOCOL_TELEMETRY_FLYSKY_IBUS,
  PROTOCOL_TELEMETRY_MULTIMODULE,
  PROTOCOL_TELEMETRY_LAST=PROTOCOL_TELEMETRY_MULTIMODULE
};

enum DisplayTrims
{
  DISPLAY_TRIMS_NEVER,
  DISPLAY_TRIMS_CHANGE,
  DISPLAY_TRIMS_ALWAYS
};

extern RadioData g_eeGeneral;
extern ModelData g_model;

PACK(union u_int8int16_t {
  struct {
    int8_t  lo;
    uint8_t hi;
  } bytes_t;
  int16_t word;
});

constexpr uint8_t EE_GENERAL = 0x01;
constexpr uint8_t EE_MODEL = 0x02;

#endif // _MYEEPROM_H_
