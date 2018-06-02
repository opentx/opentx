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

#define GET_PPM_POLARITY(idx)             g_model.moduleData[idx].ppm.pulsePol
#define GET_SBUS_POLARITY(idx)            g_model.moduleData[idx].sbus.noninverted
#define GET_PPM_DELAY(idx)                (g_model.moduleData[idx].ppm.delay * 50 + 300)
#define SET_DEFAULT_PPM_FRAME_LENGTH(idx) g_model.moduleData[idx].ppm.frameLength = 4 * max((int8_t)0, g_model.moduleData[idx].channelsCount)

#if defined(PCBHORUS)
  #define IS_TRAINER_EXTERNAL_MODULE()    false
  #define HAS_WIRELESS_TRAINER_HARDWARE() (g_eeGeneral.serial2Mode==UART_MODE_SBUS_TRAINER)
#elif defined(PCBTARANIS)
  #define IS_TRAINER_EXTERNAL_MODULE()    (g_model.trainerMode == TRAINER_MODE_MASTER_SBUS_EXTERNAL_MODULE || g_model.trainerMode == TRAINER_MODE_MASTER_CPPM_EXTERNAL_MODULE)
  #define HAS_WIRELESS_TRAINER_HARDWARE() (g_eeGeneral.serial2Mode==UART_MODE_SBUS_TRAINER)
#else
  #define IS_TRAINER_EXTERNAL_MODULE()    false
#endif

#if defined(VOICE)
  #define IS_PLAY_FUNC(func)           ((func) >= FUNC_PLAY_SOUND && func <= FUNC_PLAY_VALUE)
#else
  #define IS_PLAY_FUNC(func)           ((func) == FUNC_PLAY_SOUND)
#endif

#if defined(CPUARM)
  #define IS_PLAY_BOTH_FUNC(func)      (0)
  #define IS_VOLUME_FUNC(func)         ((func) == FUNC_VOLUME)
#else
  #define IS_PLAY_BOTH_FUNC(func)      ((func) == FUNC_PLAY_BOTH)
  #define IS_VOLUME_FUNC(func)         (0)
#endif

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

#if defined(CPUARM)
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
#elif defined(CPUM2560)
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
  #define SWITCH_CONFIG(x)            ((g_eeGeneral.switchConfig >> (2*(x))) & 0x03)
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

#if !defined(CPUARM)
  #define MODE_DIFFERENTIAL  0
  #define MODE_EXPO          0
  #define MODE_CURVE         1
#endif

#if defined(CPUARM)
#define MIN_EXPO_WEIGHT         -100
#define EXPO_VALID(ed)          ((ed)->mode)
#define EXPO_MODE_ENABLE(ed, v) (((v)<0 && ((ed)->mode&1)) || ((v)>=0 && ((ed)->mode&2)))
#elif defined(CPUM2560) || defined(CPUM2561)
#define MIN_EXPO_WEIGHT         0
#define EXPO_VALID(ed)          ((ed)->mode)
#define EXPO_MODE_ENABLE(ed, v) (((v)<0 && ((ed)->mode&1)) || ((v)>=0 && ((ed)->mode&2)))
#else
#define MIN_EXPO_WEIGHT         0
#define EXPO_VALID(ed)          ((ed)->mode)
#define EXPO_MODE_ENABLE(ed, v) (((v)<0 && ((ed)->mode&1)) || ((v)>=0 && ((ed)->mode&2)))
#endif

#if defined(CPUARM)
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

#if defined(CPUARM)
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
#else
#define MAX_LS_ANDSW    15
#endif

#if defined(CPUARM)
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
#endif

enum VarioSource {
#if !defined(TELEMETRY_FRSKY_SPORT)
  VARIO_SOURCE_ALTI,
  VARIO_SOURCE_ALTI_PLUS,
#endif
  VARIO_SOURCE_VSPEED,
  VARIO_SOURCE_A1,
  VARIO_SOURCE_A2,
#if defined(TELEMETRY_FRSKY_SPORT)
  VARIO_SOURCE_DTE,
#endif
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

#if defined(PCBSTD)
  #define TRIMS_ARRAY_SIZE  5
#else
  #define TRIMS_ARRAY_SIZE  8
  #if defined(CPUARM)
    #define TRIM_MODE_NONE  0x1F  // 0b11111
  #endif
#endif

#if defined(CPUARM)
#define IS_MANUAL_RESET_TIMER(idx)     (g_model.timers[idx].persistent == 2)
#elif defined(CPUM2560)
#define IS_MANUAL_RESET_TIMER(idx)     (g_model.timers[idx].persistent == 2)
#else
#define IS_MANUAL_RESET_TIMER(idx)     0
#endif

#if defined(CPUARM) && !defined(PCBSKY9X)
#define TIMER_COUNTDOWN_START(x)       (g_model.timers[x].countdownStart > 0 ? 5 : 10 - g_model.timers[x].countdownStart * 10)
#else
#define TIMER_COUNTDOWN_START(x)       10
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
#if defined(CPUARM)
  PROTO_CROSSFIRE,
#endif
#if defined(IRPROTOS)
  // only used on AVR
  // we will need 4 bits for proto :(
  PROTO_SILV,
  PROTO_TRAC09,
  PROTO_PICZ,
  PROTO_SWIFT,
#endif
#if defined(CPUARM)
  PROTO_MULTIMODULE,
  PROTO_SBUS,
#endif
  PROTO_MAX,
  PROTO_NONE
};

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
  MODULE_SUBTYPE_R9M_LBT,
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
  MM_RF_PROTO_LAST= MM_RF_PROTO_CFLIE
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
  MODULE_TYPE_XJT,
  MODULE_TYPE_DSM2,
  MODULE_TYPE_CROSSFIRE,
  MODULE_TYPE_MULTIMODULE,
  MODULE_TYPE_R9M,
  MODULE_TYPE_SBUS,
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

enum TelemetryType
{
  PROTOCOL_TELEMETRY_FIRST,
  PROTOCOL_FRSKY_SPORT = PROTOCOL_TELEMETRY_FIRST,
  PROTOCOL_FRSKY_D,
  PROTOCOL_FRSKY_D_SECONDARY,
  PROTOCOL_PULSES_CROSSFIRE,
  PROTOCOL_SPEKTRUM,
  PROTOCOL_FLYSKY_IBUS,
  PROTOCOL_MULTIMODULE,
  PROTOCOL_TELEMETRY_LAST=PROTOCOL_MULTIMODULE
};

enum DisplayTrims
{
  DISPLAY_TRIMS_NEVER,
  DISPLAY_TRIMS_CHANGE,
  DISPLAY_TRIMS_ALWAYS
};

#define TOTAL_EEPROM_USAGE (sizeof(ModelData)*MAX_MODELS + sizeof(RadioData))

extern RadioData g_eeGeneral;
extern ModelData g_model;

PACK(union u_int8int16_t {
  struct {
    int8_t  lo;
    uint8_t hi;
  } bytes_t;
  int16_t word;
});

#endif // _MYEEPROM_H_
