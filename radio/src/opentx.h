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

#ifndef opentx_h
#define opentx_h

#include <inttypes.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#if defined(SIMU)
  #define SWITCH_SIMU(a, b)  (a)
#else
  #define SWITCH_SIMU(a, b)  (b)
#endif

#if defined(PCBSKY9X)
  #define IS_PCBSKY9X        true
  #define CASE_PCBSKY9X(x)   x,
#else
  #define IS_PCBSKY9X        false
  #define CASE_PCBSKY9X(x)
#endif

#if defined(CPUARM)
  #define CASE_CPUARM(x)     x,
  #define IF_CPUARM(x)       x
#else
  #define CASE_CPUARM(x)
  #define IF_CPUARM(x)
#endif

#if defined(VARIO) && defined(CPUARM)
  #define CASE_VARIO_CPUARM(x) x,
#else
  #define CASE_VARIO_CPUARM(x)
#endif

#if defined(LUA)
  #define CASE_LUA(x) x,
#else
  #define CASE_LUA(x)
#endif

#if defined(CPUARM) || defined(CPUM2560)
  #define CASE_PERSISTENT_TIMERS(x) x,
#else
  #define CASE_PERSISTENT_TIMERS(x)
#endif

#if defined(RTCLOCK)
  #define CASE_RTCLOCK(x) x,
#else
  #define CASE_RTCLOCK(x)
#endif

#if defined(BUZZER)
  #define CASE_BUZZER(x) x,
#else
  #define CASE_BUZZER(x)
#endif

#if defined(AUDIO)
  #define CASE_AUDIO(x) x,
#else
  #define CASE_AUDIO(x)
#endif

#if defined(VOICE)
  #define CASE_VOICE(x) x,
#else
  #define CASE_VOICE(x)
#endif

#if defined(PWM_BACKLIGHT)
  #define CASE_PWM_BACKLIGHT(x) x,
#else
  #define CASE_PWM_BACKLIGHT(x)
#endif

#if defined(FRSKY) && defined(FRSKY_HUB) && defined(GPS)
  #define CASE_GPS(x) x,
#else
  #define CASE_GPS(x)
#endif

#if defined(VARIO)
  #define CASE_VARIO(x) x,
#else
  #define CASE_VARIO(x)
#endif

#if defined(HAPTIC)
  #define CASE_HAPTIC(x) x,
#else
  #define CASE_HAPTIC(x)
#endif

#if defined(SPLASH)
  #define CASE_SPLASH(x) x,
#else
  #define CASE_SPLASH(x)
#endif

#if defined(FRSKY)
  #define CASE_FRSKY(x) x,
#else
  #define CASE_FRSKY(x)
#endif

#if defined(MAVLINK)
  #define CASE_MAVLINK(x) x,
#else
  #define CASE_MAVLINK(x)
#endif

#if defined(PXX)
  #define CASE_PXX(x) x,
#else
  #define CASE_PXX(x)
#endif

#if defined(SDCARD)
  #define CASE_SDCARD(x) x,
#else
  #define CASE_SDCARD(x)
#endif

#if defined(BLUETOOTH)
  #define CASE_BLUETOOTH(x) x,
#else
  #define CASE_BLUETOOTH(x)
#endif

#if defined(HELI)
  #define CASE_HELI(x) x,
#else
  #define CASE_HELI(x)
#endif

#if defined(TEMPLATES)
  #define CASE_TEMPLATES(x) x,
#else
  #define CASE_TEMPLATES(x)
#endif

#if defined(FLIGHT_MODES)
  #define CASE_FLIGHT_MODES(x) x,
#else
  #define CASE_FLIGHT_MODES(x)
#endif

#if defined(CURVES)
  #define CASE_CURVES(x) x,
#else
  #define CASE_CURVES(x)
#endif

#if defined(GVARS)
  #define CASE_GVARS(x) x,
#else
  #define CASE_GVARS(x)
#endif

#if defined(PCBTARANIS) && defined(REVPLUS)
  #define CASE_REVPLUS(x) x,
#else
  #define CASE_REVPLUS(x)
#endif

#if defined(PCBTARANIS) && defined(REV9E)
  #define CASE_REV9E(x) x,
#else
  #define CASE_REV9E(x)
#endif

#if ROTARY_ENCODERS > 0
  #define ROTARY_ENCODER_NAVIGATION
#endif

#if defined(STM32F4)
  #define _NOCCM __attribute__((section(".ram")))
#else
  #define _NOCCM
#endif

#if defined(SIMU) || defined(CPUARM) || GCC_VERSION < 472
  typedef int32_t int24_t;
#else
  typedef __int24 int24_t;
#endif

#if defined(FAI)
  #define IS_FAI_ENABLED() true
  #define IF_FAI_CHOICE(x)
#elif defined(FAI_CHOICE)
  #define IS_FAI_ENABLED() g_eeGeneral.fai
  #define IF_FAI_CHOICE(x) x,
#else
  #define IS_FAI_ENABLED() false
  #define IF_FAI_CHOICE(x)
#endif

#define IS_FAI_FORBIDDEN(idx) (IS_FAI_ENABLED() && idx >= MIXSRC_FIRST_TELEM)

#if defined(SIMU)
  #ifndef FORCEINLINE
    #define FORCEINLINE
  #endif
  #if !defined(NOINLINE)
    #define NOINLINE
  #endif
  #define CONVERT_PTR_UINT(x) ((uint32_t)(uint64_t)(x))
  #define CONVERT_UINT_PTR(x) ((uint32_t*)(uint64_t)(x))
  char *convertSimuPath(const char *path);
#else
  #define FORCEINLINE inline __attribute__ ((always_inline))
  #define NOINLINE __attribute__ ((noinline))
  #define SIMU_SLEEP(x)
  #define CONVERT_PTR_UINT(x) ((uint32_t)(x))
  #define CONVERT_UINT_PTR(x) ((uint32_t *)(x))
  #define convertSimuPath(x) (x)
#endif

#if !defined(CPUM64) && !defined(ACCURAT_THROTTLE_TIMER)
    //  code cost is about 16 bytes for higher throttle accuracy for timer
    //  would not be noticable anyway, because all version up to this change had only 16 steps;
    //  now it has already 32  steps; this define would increase to 128 steps
  #if !defined(ACCURAT_THROTTLE_TIMER)
    #define ACCURAT_THROTTLE_TIMER
  #endif
#endif

// RESX range is used for internal calculation; The menu says -100.0 to 100.0; internally it is -1024 to 1024 to allow some optimizations
#define RESX_SHIFT 10
#define RESX       1024
#define RESXu      1024u
#define RESXul     1024ul
#define RESXl      1024l

#if defined(PCBTARANIS)
  #include "targets/taranis/board_taranis.h"
#elif defined(PCBSKY9X)
  #include "targets/sky9x/board_sky9x.h"
#elif defined(PCBGRUVIN9X)
  #include "targets/gruvin9x/board_gruvin9x.h"
#elif defined(PCBMEGA2560)
  #include "targets/mega2560/board_mega2560.h"
#else
  #include "targets/stock/board_stock.h"
#endif

#include "debug.h"

#if defined(SIMU)
#include "targets/simu/simpgmspace.h"
#elif defined(CPUARM)
typedef const unsigned char pm_uchar;
typedef const char pm_char;
typedef const uint16_t pm_uint16_t;
typedef const uint8_t pm_uint8_t;
typedef const int16_t pm_int16_t;
typedef const int8_t pm_int8_t;
#define pgm_read_byte(address_short) (*(uint8_t*)(address_short))
#define PSTR(adr) adr
#define PROGMEM
#define pgm_read_adr(x) *(x)
#define cli()
#define sei()
extern void boardInit();
#else
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "pgmtypes.h"

#include <avr/eeprom.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL  // 16 MHz
#include <util/delay.h>
#define pgm_read_adr(address_short) pgm_read_word(address_short)
#include <avr/wdt.h>
#endif

#if defined(PCBTARANIS)
  #if defined(REV9E)
    #define NUM_SWITCHES   18 // yes, it's a lot!
  #else
    #define NUM_SWITCHES   8
  #endif
  #define NUM_SW_SRCRAW    8
  #define SWSRC_THR        SWSRC_SF2
  #define SWSRC_GEA        SWSRC_SG2
  #define SWSRC_ID0        SWSRC_SA0
  #define SWSRC_ID1        SWSRC_SA1
  #define SWSRC_ID2        SWSRC_SA2
  #define SW_DSM2_BIND     SW_SH2
#else
  #define NUM_SWITCHES     7
  #define IS_3POS(sw)      ((sw) == 0)
  #define IS_MOMENTARY(sw) (sw == SWSRC_TRN)
  #define NUM_SW_SRCRAW    1
  #define SW_DSM2_BIND     SW_TRN
#endif

#define NUM_PSWITCH        (SWSRC_LAST_SWITCH-SWSRC_FIRST_SWITCH+1)
#define NUM_POTSSW         (NUM_XPOTS*6)

#if defined(PCBTARANIS)
  #define KEY_RIGHT        KEY_PLUS
  #define KEY_LEFT         KEY_MINUS
  #define KEY_UP           KEY_PLUS
  #define KEY_DOWN         KEY_MINUS
#else
  #define KEY_ENTER        KEY_MENU
  #define KEY_PLUS         KEY_RIGHT
  #define KEY_MINUS        KEY_LEFT
#endif

#include "myeeprom.h"

#if defined(CPUM64)
  void memclear(void *ptr, uint8_t size);
#else
  #define memclear(p, s) memset(p, 0, s)
#endif

#if defined(PCBTARANIS) && defined(REV9E)
  #define IS_SLIDER_AVAILABLE(x)    ((x)==SLIDER1 || (x)==SLIDER2 || (g_eeGeneral.slidersConfig & (0x01 << ((x)-SLIDER3))))
  #define IS_POT_AVAILABLE(x)       ((x)<POT1 || ((x)<=POT_LAST && ((g_eeGeneral.potsConfig & (0x03 << (2*((x)-POT1))))!=0)) || ((x)>=SLIDER1 && IS_SLIDER_AVAILABLE(x)))
  #define IS_POT_MULTIPOS(x)        ((x)>=POT1 && (x)<=POT_LAST && ((g_eeGeneral.potsConfig>>(2*((x)-POT1)))&0x03)==POT_MULTIPOS_SWITCH)
  #define IS_POT_WITHOUT_DETENT(x)  ((x)>=POT1 && (x)<=POT_LAST && ((g_eeGeneral.potsConfig>>(2*((x)-POT1)))&0x03)==POT_WITHOUT_DETENT)
#elif defined(PCBTARANIS) && defined(REVPLUS)
  #define IS_POT_AVAILABLE(x)       ((x)!=POT3 || (g_eeGeneral.potsConfig & (0x03 << (2*((x)-POT1))))!=POT_NONE)
  #define IS_POT_MULTIPOS(x)        ((x)>=POT1 && (x)<=POT_LAST && ((g_eeGeneral.potsConfig>>(2*((x)-POT1)))&0x03)==POT_MULTIPOS_SWITCH)
  #define IS_POT_WITHOUT_DETENT(x)  ((x)>=POT1 && (x)<=POT_LAST && ((g_eeGeneral.potsConfig>>(2*((x)-POT1)))&0x03)==POT_WITHOUT_DETENT)
#elif defined(PCBTARANIS)
  #define IS_POT_AVAILABLE(x)       ((x)!=POT3)
  #define IS_POT_MULTIPOS(x)        ((x)>=POT1 && (x)<=POT_LAST && ((g_eeGeneral.potsConfig>>(2*((x)-POT1)))&0x03)==POT_MULTIPOS_SWITCH)
  #define IS_POT_WITHOUT_DETENT(x)  ((x)>=POT1 && (x)<=POT_LAST && ((g_eeGeneral.potsConfig>>(2*((x)-POT1)))&0x03)==POT_WITHOUT_DETENT)
#else
  #define IS_POT_AVAILABLE(x)       (true)
  #define IS_POT_MULTIPOS(x)        (false)
  #define IS_POT_WITHOUT_DETENT(x)  (true)
#endif

#define IS_POT(x)                   ((x)>=POT1 && (x)<=POT_LAST)

#define GET_LOWRES_POT_POSITION(i)  (getValue(MIXSRC_FIRST_POT+(i)) >> 4)
#define SAVE_POT_POSITION(i)        g_model.potsWarnPosition[i] = GET_LOWRES_POT_POSITION(i)

#if ROTARY_ENCODERS > 0
  #define IF_ROTARY_ENCODERS(x) x,
#else
  #define IF_ROTARY_ENCODERS(x)
#endif

#define PPM_CENTER 1500

#if defined(PPM_CENTER_ADJUSTABLE)
  #define PPM_CH_CENTER(ch) (PPM_CENTER+limitAddress(ch)->ppmCenter)
#else
  #define PPM_CH_CENTER(ch) (PPM_CENTER)
#endif

#if defined(CPUARM)
  #include "fifo.h"
  // This doesn't need protection on this processor
  typedef uint32_t tmr10ms_t;
  extern volatile tmr10ms_t g_tmr10ms;
  #define get_tmr10ms() g_tmr10ms
  typedef int32_t rotenc_t;
  typedef int32_t getvalue_t;
  typedef uint32_t mixsrc_t;
#else
  #define tmr10ms_t uint16_t
  extern volatile tmr10ms_t g_tmr10ms;
  extern inline uint16_t get_tmr10ms()
  {
    uint16_t time  ;
    cli();
    time = g_tmr10ms ;
    sei();
    return time ;
  }
  typedef int8_t rotenc_t;
  typedef int16_t getvalue_t;
  typedef uint8_t mixsrc_t;
  void watchdogSetTimeout(uint32_t timeout);
#endif

#if defined(NAVIGATION_STICKS)
  extern uint8_t StickScrollAllowed;
  extern uint8_t StickScrollTimer;
  #define STICK_SCROLL_TIMEOUT          9
  #define STICK_SCROLL_DISABLE()        StickScrollAllowed = 0
#else
  #define STICK_SCROLL_DISABLE()
#endif

#include "eeprom_common.h"

#if defined(EEPROM_RLC)
  #include "eeprom_rlc.h"
#else
  #include "eeprom_raw.h"
#endif

#include "pulses/pulses.h"

#if defined(PCBTARANIS)
  #define BITMAP_BUFFER_SIZE(width, height)   (2 + width * ((height+7)/8)*4)
  #define MODEL_BITMAP_WIDTH  64
  #define MODEL_BITMAP_HEIGHT 32
  #define MODEL_BITMAP_SIZE   BITMAP_BUFFER_SIZE(MODEL_BITMAP_WIDTH, MODEL_BITMAP_HEIGHT)
  extern uint8_t modelBitmap[MODEL_BITMAP_SIZE];
  void loadModelBitmap(char *name, uint8_t *bitmap);
  #define LOAD_MODEL_BITMAP() loadModelBitmap(g_model.header.bitmap, modelBitmap)
#else
  #define LOAD_MODEL_BITMAP()
#endif

#if defined(XCURVES)
  void loadCurves();
  #define LOAD_MODEL_CURVES() loadCurves()
#else
  #define LOAD_MODEL_CURVES()
#endif

#if defined(CPUARM)
  static const int8_t maxChannelsModules[] = { 0, 8, 8, -2 }; // relative to 8!
  static const int8_t maxChannelsXJT[] = { 0, 8, 0, 4 }; // relative to 8!
  #define NUM_CHANNELS(idx)                 (8+g_model.moduleData[idx].channelsCount)
  #define MAX_TRAINER_CHANNELS()            (8)
#endif

#if defined(PCBTARANIS)
  #if defined(TARANIS_INTERNAL_PPM)
    #define IS_MODULE_PPM(idx)              (idx==TRAINER_MODULE || (idx==INTERNAL_MODULE && g_model.moduleData[INTERNAL_MODULE].type==MODULE_TYPE_PPM)|| (idx==EXTERNAL_MODULE && g_model.moduleData[EXTERNAL_MODULE].type==MODULE_TYPE_PPM))
    #define IS_MODULE_XJT(idx)              (((idx==INTERNAL_MODULE && g_model.moduleData[INTERNAL_MODULE].type==MODULE_TYPE_XJT)|| (idx==EXTERNAL_MODULE && g_model.moduleData[EXTERNAL_MODULE].type==MODULE_TYPE_XJT)) && (g_model.moduleData[idx].rfProtocol != RF_PROTO_OFF))
  #else
    #define IS_MODULE_PPM(idx)              (idx==TRAINER_MODULE || (idx==EXTERNAL_MODULE && g_model.moduleData[EXTERNAL_MODULE].type==MODULE_TYPE_PPM))
    #define IS_MODULE_XJT(idx)              ((idx==INTERNAL_MODULE || g_model.moduleData[EXTERNAL_MODULE].type==MODULE_TYPE_XJT) && (g_model.moduleData[idx].rfProtocol != RF_PROTO_OFF))
  #endif
  #if defined(DSM2)
    #define IS_MODULE_DSM2(idx)             (idx==EXTERNAL_MODULE && g_model.moduleData[EXTERNAL_MODULE].type==MODULE_TYPE_DSM2)
  #else
    #define IS_MODULE_DSM2(idx)             (false)
  #endif
  #if defined(TARANIS_INTERNAL_PPM)
    #define MAX_INTERNAL_MODULE_CHANNELS()  ((g_model.moduleData[INTERNAL_MODULE].type == MODULE_TYPE_XJT) ? maxChannelsXJT[1+g_model.moduleData[0].rfProtocol] : maxChannelsModules[g_model.moduleData[INTERNAL_MODULE].type])
  #else
    #define MAX_INTERNAL_MODULE_CHANNELS()  (maxChannelsXJT[1+g_model.moduleData[INTERNAL_MODULE].rfProtocol])
  #endif
  #define MAX_EXTERNAL_MODULE_CHANNELS()    ((g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_XJT) ? maxChannelsXJT[1+g_model.moduleData[1].rfProtocol] : maxChannelsModules[g_model.moduleData[EXTERNAL_MODULE].type])
  #define MAX_CHANNELS(idx)                 (idx==INTERNAL_MODULE ? MAX_INTERNAL_MODULE_CHANNELS() : (idx==EXTERNAL_MODULE ? MAX_EXTERNAL_MODULE_CHANNELS() : MAX_TRAINER_CHANNELS()))
#elif defined(PCBSKY9X) && !defined(REVA) && !defined(REVX)
  #define IS_MODULE_PPM(idx)                (idx==TRAINER_MODULE || idx==EXTRA_MODULE || (idx==EXTERNAL_MODULE && g_model.moduleData[EXTERNAL_MODULE].type==MODULE_TYPE_PPM))
  #define IS_MODULE_XJT(idx)                (idx==EXTERNAL_MODULE && g_model.moduleData[EXTERNAL_MODULE].type==MODULE_TYPE_XJT)
  #define IS_MODULE_DSM2(idx)               (idx==EXTERNAL_MODULE && g_model.moduleData[EXTERNAL_MODULE].type==MODULE_TYPE_DSM2)
  #define MAX_EXTERNAL_MODULE_CHANNELS()    ((g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_XJT) ? maxChannelsXJT[1+g_model.moduleData[0].rfProtocol] : maxChannelsModules[g_model.moduleData[EXTERNAL_MODULE].type])
  #define MAX_EXTRA_MODULE_CHANNELS()       (0) // Only PPM
  #define MAX_CHANNELS(idx)                 (idx==EXTERNAL_MODULE ? MAX_EXTERNAL_MODULE_CHANNELS() : (idx==EXTRA_MODULE ? MAX_EXTRA_MODULE_CHANNELS() : MAX_TRAINER_CHANNELS()))
#else
  #define IS_MODULE_PPM(idx)                (idx==TRAINER_MODULE || (idx==EXTERNAL_MODULE && g_model.moduleData[EXTERNAL_MODULE].type==MODULE_TYPE_PPM))
  #define IS_MODULE_XJT(idx)                (idx==EXTERNAL_MODULE && g_model.moduleData[EXTERNAL_MODULE].type==MODULE_TYPE_XJT)
  #define IS_MODULE_DSM2(idx)               (idx==EXTERNAL_MODULE && g_model.moduleData[EXTERNAL_MODULE].type==MODULE_TYPE_DSM2)
  #define MAX_EXTERNAL_MODULE_CHANNELS()    ((g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_XJT) ? maxChannelsXJT[1+g_model.moduleData[EXTERNAL_MODULE].rfProtocol] : maxChannelsModules[g_model.moduleData[EXTERNAL_MODULE].type])
  #define MAX_CHANNELS(idx)                 (idx==EXTERNAL_MODULE ? MAX_EXTERNAL_MODULE_CHANNELS() : MAX_TRAINER_CHANNELS())
#endif

#if defined(CPUARM)
  #define MASK_CFN_TYPE  uint64_t  // current max = 64 function switches
  #define MASK_FUNC_TYPE uint32_t  // current max = 32 functions
#elif defined(CPUM64)
  #define MASK_CFN_TYPE  uint16_t  // current max = 16 function switches
  #define MASK_FUNC_TYPE uint8_t   // current max = 8  functions
#else
  #define MASK_CFN_TYPE  uint32_t  // current max = 32 function switches
  #define MASK_FUNC_TYPE uint8_t   // current max = 8 functions
#endif

typedef struct {
  MASK_FUNC_TYPE activeFunctions;
  MASK_CFN_TYPE  activeSwitches;
  tmr10ms_t lastFunctionTime[NUM_CFN];

  inline bool isFuunctionActive(uint8_t func)
  {
    return activeFunctions & ((MASK_FUNC_TYPE)1 << func);
  }

  void reset()
  {
    memclear(this, sizeof(*this));
  }
} CustomFunctionsContext;

#include "gui/gui.h"

#if defined(TEMPLATES)
  #include "templates.h"
#endif

#if !defined(SIMU)
  #define assert(x)
  #if !defined(CPUARM) || !defined(DEBUG)
    #define printf printf_not_allowed
  #endif
#endif

extern const pm_uint8_t bchout_ar[];
extern const pm_uint8_t modn12x3[];

extern uint8_t stickMode;

//convert from mode 1 to mode stickMode
//NOTICE!  =>  0..3 -> 0..3
#define RUD_STICK 0
#define ELE_STICK 1
#define THR_STICK 2
#define AIL_STICK 3
#define CONVERT_MODE(x)  (((x)<=AIL_STICK) ? pgm_read_byte(modn12x3 + 4*stickMode + (x)) : (x) )

extern uint8_t channel_order(uint8_t x);

enum BaseCurves {
  CURVE_NONE,
  CURVE_X_GT0,
  CURVE_X_LT0,
  CURVE_ABS_X,
  CURVE_F_GT0,
  CURVE_F_LT0,
  CURVE_ABS_F,
  CURVE_BASE
};

#define THRCHK_DEADBAND 16

#if defined(PCBTARANIS)
  #define SPLASH_NEEDED() (g_eeGeneral.splashMode != 3)
#elif defined(CPUARM)
  #define SPLASH_NEEDED() (g_model.moduleData[EXTERNAL_MODULE].type != MODULE_TYPE_DSM2 && !g_eeGeneral.splashMode)
#else
  #define SPLASH_NEEDED() (!IS_DSM2_PROTOCOL(g_model.protocol) && !g_eeGeneral.splashMode)
#endif

#if defined(FSPLASH)
  #define SPLASH_TIMEOUT  (g_eeGeneral.splashMode == 0 ? 60000/*infinite=10mn*/ : ((4*100) * (g_eeGeneral.splashMode & 0x03)))
#elif defined(PCBTARANIS)
  #define SPLASH_TIMEOUT  (g_eeGeneral.splashMode==-4 ? 1500 : (g_eeGeneral.splashMode<=0 ? (400-g_eeGeneral.splashMode*200) : (400-g_eeGeneral.splashMode*100)))
#else
  #define SPLASH_TIMEOUT  (4*100)  // 4 seconds
#endif

#if defined(PCBTARANIS)
  #define IS_RE_NAVIGATION_ENABLE()   true
  #define NAVIGATION_RE_IDX()         0
#elif defined(ROTARY_ENCODERS)
  #define NAVIGATION_RE_IDX()         (g_eeGeneral.reNavigation - 1)
  #define IS_RE_NAVIGATION_ENABLE()   g_eeGeneral.reNavigation
#elif defined(ROTARY_ENCODER_NAVIGATION)
  #define IS_RE_NAVIGATION_ENABLE()   true
  #define NAVIGATION_RE_IDX()         0
#endif

#define HEART_TIMER_10MS     1
#define HEART_TIMER_PULSES   2 // when multiple modules this is the first one
#if defined(PCBTARANIS)
  #define HEART_WDT_CHECK      (HEART_TIMER_10MS + (HEART_TIMER_PULSES << 0) + (HEART_TIMER_PULSES << 1))
#else
  #define HEART_WDT_CHECK      (HEART_TIMER_10MS + HEART_TIMER_PULSES)
#endif
extern uint8_t heartbeat;

#if defined(CPUARM)
void watchdogSetTimeout(uint32_t timeout);
#endif

#define MAX_ALERT_TIME   60

struct t_inactivity
{
  uint16_t counter;
  uint8_t  sum;
};

extern struct t_inactivity inactivity;

#define LEN_STD_CHARS 40

#if defined(TRANSLATIONS_CZ)
#define ZCHAR_MAX (LEN_STD_CHARS)
#else
#define ZCHAR_MAX (LEN_STD_CHARS + LEN_SPECIAL_CHARS)
#endif

char hex2zchar(uint8_t hex);
char idx2char(int8_t idx);
#if defined(CPUARM) || defined(SIMU)
int8_t char2idx(char c);
void str2zchar(char *dest, const char *src, int size);
int zchar2str(char *dest, const char *src, int size);
#endif

#include "keys.h"
#include "pwr.h"

#if defined(PCBTARANIS)
div_t switchInfo(int switchPosition);
#endif

bool switchState(EnumKeys enuk);
uint8_t trimDown(uint8_t idx);
void readKeysAndTrims();

uint16_t evalChkSum();

#if !defined(GUI)
  #define MESSAGE_SOUND_ARG
  #define MESSAGE(...)
  #define ALERT(...)
#elif defined(VOICE)
  #define MESSAGE_SOUND_ARG , uint8_t sound
  #define MESSAGE(title, msg, info, sound) message(title, msg, info, sound)
  #define ALERT(title, msg, sound) alert(title, msg, sound)
#else
  #define MESSAGE_SOUND_ARG
  #define MESSAGE(title, msg, info, sound) message(title, msg, info)
  #define ALERT(title, msg, sound) alert(title, msg)
#endif

extern void message(const pm_char *title, const pm_char *s, const char *last MESSAGE_SOUND_ARG);
extern void alert(const pm_char * t, const pm_char * s MESSAGE_SOUND_ARG);

enum PerOutMode {
  e_perout_mode_normal = 0,
  e_perout_mode_inactive_flight_mode = 1,
  e_perout_mode_notrainer = 2,
  e_perout_mode_notrims = 4,
  e_perout_mode_nosticks = 8,
  e_perout_mode_noinput = e_perout_mode_notrainer+e_perout_mode_notrims+e_perout_mode_nosticks
};


#if defined(MODULE_ALWAYS_SEND_PULSES)
extern uint8_t startupWarningState;

enum StartupWarningStates {
  STARTUP_WARNING_THROTTLE,
  STARTUP_WARNING_SWITCHES,
  STARTUP_WARNING_DONE,
};
#endif


// Fiddle to force compiler to use a pointer
#if defined(CPUARM) || defined(SIMU)
  #define FORCE_INDIRECT(ptr)
#else
  #define FORCE_INDIRECT(ptr) __asm__ __volatile__ ("" : "=e" (ptr) : "0" (ptr))
#endif

extern uint8_t mixerCurrentFlightMode;
extern uint8_t lastFlightMode;
extern uint8_t flightModeTransitionLast;

#if defined(CPUARM)
  #define bitfield_channels_t uint32_t
#else
  #define bitfield_channels_t uint16_t
#endif

#if defined(SIMU)
  inline int getAvailableMemory() { return 1000; }
#elif defined(CPUARM) && !defined(SIMU)
  extern unsigned char *heap;
  extern int _end;
  extern int _estack;
  extern int _main_stack_start;
  extern int _heap_end;
  #define getAvailableMemory() ((unsigned int)((unsigned char *)&_heap_end - heap))
#endif

void evalFlightModeMixes(uint8_t mode, uint8_t tick10ms);
void evalMixes(uint8_t tick10ms);
void doMixerCalculations();

#if defined(CPUARM)
  void checkTrims();
#endif
void perMain();
NOINLINE void per10ms();

getvalue_t getValue(mixsrc_t i);

#if defined(CPUARM)
#define GETSWITCH_MIDPOS_DELAY   1
bool getSwitch(int8_t swtch, uint8_t flags=0);
#else
bool getSwitch(int8_t swtch);
#endif

void logicalSwitchesTimerTick();
void logicalSwitchesReset();

#if defined(CPUARM)
  void evalLogicalSwitches(bool isCurrentPhase=true);
  void logicalSwitchesCopyState(uint8_t src, uint8_t dst);
  #define LS_RECURSIVE_EVALUATION_RESET()
#else
  #define evalLogicalSwitches(xxx)
  #define GETSWITCH_RECURSIVE_TYPE uint16_t
  extern volatile GETSWITCH_RECURSIVE_TYPE s_last_switch_used;
  extern volatile GETSWITCH_RECURSIVE_TYPE s_last_switch_value;
  #define LS_RECURSIVE_EVALUATION_RESET() s_last_switch_used = 0
#endif

#if defined(PCBTARANIS)
  void getSwitchesPosition(bool startup);
#else
  #define getSwitchesPosition(...)
#endif

extern swarnstate_t switches_states;
int8_t  getMovedSwitch();

#if defined(PCBTARANIS)
  #define GET_MOVED_SOURCE_PARAMS uint8_t min
  int8_t getMovedSource(GET_MOVED_SOURCE_PARAMS);
  #define GET_MOVED_SOURCE(min, max) getMovedSource(min)
#else
  #define GET_MOVED_SOURCE_PARAMS
  int8_t getMovedSource();
  #define GET_MOVED_SOURCE(min, max) getMovedSource()
#endif

#if defined(FLIGHT_MODES)
  extern uint8_t getFlightMode();
#else
  #define getFlightMode() 0
#endif

#if !defined(PCBTARANIS)
  uint8_t getTrimFlightPhase(uint8_t phase, uint8_t idx);
#else
  #define getTrimFlightPhase(phase, idx) (phase)
#endif

#if defined(GVARS)
  extern int8_t trimGvar[NUM_STICKS];
  #define TRIM_REUSED(idx) trimGvar[idx] >= 0
#else
  #define TRIM_REUSED(idx) 0
#endif

trim_t getRawTrimValue(uint8_t phase, uint8_t idx);
int getTrimValue(uint8_t phase, uint8_t idx);

#if defined(PCBTARANIS)
  bool setTrimValue(uint8_t phase, uint8_t idx, int trim);
#else
  void setTrimValue(uint8_t phase, uint8_t idx, int trim);
#endif

#if defined(ROTARY_ENCODERS)
  int16_t getRotaryEncoder(uint8_t idx);
  void incRotaryEncoder(uint8_t idx, int8_t inc);
#endif

#if defined(PCBGRUVIN9X) || defined(PCBMEGA2560)
  #define ROTARY_ENCODER_GRANULARITY (1)
#elif defined(CPUARM)
  #define ROTARY_ENCODER_GRANULARITY (2 << g_eeGeneral.rotarySteps)
#else
  #define ROTARY_ENCODER_GRANULARITY (2)
#endif

#if defined(GVARS)
  #if defined(PCBSTD)
    int16_t getGVarValue(int16_t x, int16_t min, int16_t max);
    void setGVarValue(uint8_t x, int8_t value);
    #define GET_GVAR(x, min, max, p) getGVarValue(x, min, max)
    #define SET_GVAR(idx, val, p) setGVarValue(idx, val)  
  #else
    uint8_t getGVarFlightPhase(uint8_t phase, uint8_t idx);
    int16_t getGVarValue(int16_t x, int16_t min, int16_t max, int8_t phase);
    void setGVarValue(uint8_t x, int16_t value, int8_t phase);  
    #define GET_GVAR(x, min, max, p) getGVarValue(x, min, max, p)
    #define SET_GVAR(idx, val, p) setGVarValue(idx, val, p)      
    #define GVAR_DISPLAY_TIME     100 /*1 second*/;
    extern uint8_t s_gvar_timer;
    extern uint8_t s_gvar_last;
  #endif
#else
  #define GET_GVAR(x, ...) (x)
#endif

#if defined(CPUARM)
  #define GV_GET_GV1_VALUE(max)        ( (max<=GV_RANGESMALL && min>=GV_RANGESMALL_NEG) ? GV1_SMALL : GV1_LARGE )
  #define GV_INDEX_CALCULATION(x,max)  ( (max<=GV_RANGESMALL && min>=GV_RANGESMALL_NEG) ? (uint8_t) x-GV1_SMALL : ((x&(GV1_LARGE*2-1))-GV1_LARGE) )
  #define GV_IS_GV_VALUE(x,min,max)    ( (max>GV1_SMALL || min<-GV1_SMALL) ? (x>GV_RANGELARGE || x<GV_RANGELARGE_NEG) : (x>max) || (x<min) )
#else
  #define GV_GET_GV1_VALUE(max)        ( (max<=GV_RANGESMALL) ? GV1_SMALL : GV1_LARGE )
  #define GV_INDEX_CALCULATION(x,max)  ( (max<=GV1_SMALL) ? (uint8_t) x-GV1_SMALL : ((x&(GV1_LARGE*2-1))-GV1_LARGE) )
  #define GV_IS_GV_VALUE(x,min,max)    ( (x>max) || (x<min) )
#endif

#define GV_INDEX_CALC_DELTA(x,delta) ((x&(delta*2-1)) - delta)

#define GV_CALC_VALUE_IDX_POS(idx,delta) (-delta+idx)
#define GV_CALC_VALUE_IDX_NEG(idx,delta) (delta+idx)

#define GV_RANGESMALL      (GV1_SMALL - (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGESMALL_NEG  (-GV1_SMALL + (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGELARGE      (GV1_LARGE - (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGELARGE_NEG  (-GV1_LARGE + (RESERVE_RANGE_FOR_GVARS+1))
#if defined(CPUARM)
  // the define GV1_LARGE marks the highest bit value used for this variables
  // because this would give too big numbers for ARM, we limit it further for
  // offset and weight
  #define GV_RANGELARGE_WEIGHT      (GV_RANGE_WEIGHT)
  #define GV_RANGELARGE_WEIGHT_NEG (-GV_RANGE_WEIGHT)
  #define GV_RANGELARGE_OFFSET      (GV_RANGE_OFFSET)
  #define GV_RANGELARGE_OFFSET_NEG (-GV_RANGE_OFFSET)
#else
  // for stock we just use as much as possible
  #define GV_RANGELARGE_WEIGHT      GV_RANGELARGE
  #define GV_RANGELARGE_WEIGHT_NEG  GV_RANGELARGE_NEG
  #define GV_RANGELARGE_OFFSET      GV_RANGELARGE
  #define GV_RANGELARGE_OFFSET_NEG  GV_RANGELARGE_NEG
#endif

extern uint16_t sessionTimer;
extern uint16_t s_timeCumThr;
extern uint16_t s_timeCum16ThrP;

#if defined(OVERRIDE_CHANNEL_FUNCTION)
#if defined(CPUARM)
  typedef int16_t safetych_t;
  #define OVERRIDE_CHANNEL_UNDEFINED -4096
#else
  typedef int8_t safetych_t;
  #define OVERRIDE_CHANNEL_UNDEFINED -128
#endif
extern safetych_t safetyCh[NUM_CHNOUT];
#endif

extern uint8_t trimsCheckTimer;

#if defined(CPUARM)
extern uint8_t trimsDisplayTimer;
extern uint8_t trimsDisplayMask;
#endif

void flightReset();

extern uint8_t unexpectedShutdown;

extern uint16_t maxMixerDuration;

#if !defined(CPUARM)
extern uint8_t g_tmr1Latency_max;
extern uint8_t g_tmr1Latency_min;
extern uint16_t lastMixerDuration;
#endif

#if defined(CPUARM)
  #define DURATION_MS_PREC2(x) ((x)/20)
#else
  #define DURATION_MS_PREC2(x) ((x)*100)/16
#endif

#if defined(THRTRACE)
  #define MAXTRACE (LCD_W - 8)
  extern uint8_t  s_traceBuf[MAXTRACE];
  #if LCD_W >= 255
    extern int16_t  s_traceWr;
    extern int16_t  s_traceCnt;
  #else
    extern uint8_t  s_traceWr;
    extern int16_t  s_traceCnt;
  #endif
  extern uint8_t  s_cnt_10s;
  extern uint16_t s_cnt_samples_thr_10s;
  extern uint16_t s_sum_samples_thr_10s;
  #define RESET_THR_TRACE() s_traceCnt = s_traceWr = s_cnt_10s = s_cnt_samples_thr_10s = s_sum_samples_thr_10s = s_timeCum16ThrP = s_timeCumThr = 0
#else
  #define RESET_THR_TRACE() s_timeCum16ThrP = s_timeCumThr = 0
#endif

#if defined(CPUSTM32)
  static inline uint16_t getTmr2MHz() { return TIM7->CNT; }
#elif defined(CPUARM)
  static inline uint16_t getTmr2MHz() { return TC1->TC_CHANNEL[0].TC_CV; }
#else
  uint16_t getTmr16KHz();
#endif

#if defined(CPUARM)
  uint32_t stack_free(uint32_t tid);
  void stack_paint();
#else
  uint16_t stack_free();
#endif

#if defined(SPLASH)
  void doSplash();
#endif

#if MENUS_LOCK == 1
  extern bool readonly;
  extern bool readonlyUnlocked();
  #define READ_ONLY() readonly
  #define READ_ONLY_UNLOCKED() readonlyUnlocked()
#else
  #define READ_ONLY() false
  #define READ_ONLY_UNLOCKED() true
#endif

void checkLowEEPROM();
void checkTHR();
void checkSwitches();
void checkAlarm();
void checkAll();

#if !defined(SIMU)
  void getADC();
#endif

#if defined(PCBTARANIS)
void processSbusInput();
#endif

extern void backlightOn();

enum Analogs {
  STICK1,
  STICK2,
  STICK3,
  STICK4,
#if defined(PCBTARANIS)
  POT1,
  POT2,
  POT3,
  #if defined(REV9E)
    POT4,
    POT_LAST = POT4,
  #else
    POT_LAST = POT3,
  #endif
  SLIDER1,
  SLIDER2,
  #if defined(REV9E)
    SLIDER3,
    SLIDER4,
  #endif
#else
  POT1,
  POT2,
  POT3,
  POT_LAST = POT3,
#endif
#if defined(TELEMETRY_MOD_14051) || defined(TELEMETRY_MOD_14051_SWAPPED)
  // When the mod is applied, ADC7 is connected to 14051's X pin and TX_VOLTAGE
  // is connected to 14051's X0 pin (one of the multiplexed inputs). TX_VOLTAGE
  // value is filled in by processMultiplexAna().

  // This shifts TX_VOLTAGE from 7 to 8 and makes X14051 take the 7th position
  // corresponding to ADC7.
  X14051,
#endif
  TX_VOLTAGE,
#if defined(PCBSKY9X) && !defined(REVA)
  TX_CURRENT,
#endif
  NUMBER_ANALOG
};

void checkBacklight();

#if defined(PCBSTD) && defined(VOICE) && !defined(SIMU)
  #define BACKLIGHT_ON()    (Voice.Backlight = 1)
  #define BACKLIGHT_OFF()   (Voice.Backlight = 0)
#else
  #define BACKLIGHT_ON()    backlightEnable()
  #define BACKLIGHT_OFF()   backlightDisable()
#endif

#define BITMASK(bit) (1<<(bit))

/// liefert Dimension eines Arrays
#define DIM(arr) (sizeof((arr))/sizeof((arr)[0]))

/// liefert Betrag des Arguments
template<class t> FORCEINLINE t abs(t a) { return a>0?a:-a; }
/// liefert das Minimum der Argumente
template<class t> FORCEINLINE t min(t a, t b) { return a<b?a:b; }
/// liefert das Maximum der Argumente
template<class t> FORCEINLINE t max(t a, t b) { return a>b?a:b; }
template<class t> FORCEINLINE t sgn(t a) { return a>0 ? 1 : (a < 0 ? -1 : 0); }
template<class t> FORCEINLINE t limit(t mi, t x, t ma) { return min(max(mi,x),ma); }
template<class t> void SWAP(t & a, t & b) { t tmp = b; b = a; a = tmp; }

uint16_t isqrt32(uint32_t n);

#if defined(CPUARM) && !defined(BOOT)
#if !defined(SIMU)
extern "C" {
#include <CoOS.h>
}
#endif

extern OS_MutexID mixerMutex;
inline void pauseMixerCalculations()
{
  CoEnterMutexSection(mixerMutex);
}

inline void resumeMixerCalculations()
{
  CoLeaveMutexSection(mixerMutex);
}
#else
#define pauseMixerCalculations()
#define resumeMixerCalculations()
#endif

void generalDefault();
void modelDefault(uint8_t id);

#if defined(CPUARM)
bool isFileAvailable(const char * filename);
void checkModelIdUnique(uint8_t index, uint8_t module);
#endif

#if defined(CPUARM)

inline int divRoundClosest(const int n, const int d)
{
  return ((n < 0) ^ (d < 0)) ? ((n - d/2)/d) : ((n + d/2)/d);
}

#define calc100to256_16Bits(x) calc100to256(x)
#define calc100toRESX_16Bits(x) calc100toRESX(x)

inline int calc100to256(register int x)
{
  return divRoundClosest(x*256, 100);
}

inline int calc100toRESX(register int x)
{
  return divRoundClosest(x*RESX, 100);
}

inline int calc1000toRESX(register int x)
{
  return divRoundClosest(x*RESX, 1000);
}

inline int calcRESXto1000(register int x)
{
  return divRoundClosest(x*1000, RESX);
}

inline int calcRESXto100(register int x)
{
  return divRoundClosest(x*100, RESX);
}

#else
extern int16_t calc100to256_16Bits(int16_t x); // @@@2 open.20.fsguruh: return x*2.56
extern int16_t calc100to256(int8_t x); // @@@2 open.20.fsguruh: return x*2.56
extern int16_t calc100toRESX_16Bits(int16_t x); // @@@ open.20.fsguruh
extern int16_t calc100toRESX(int8_t x);
extern int16_t calc1000toRESX(int16_t x);
extern int16_t calcRESXto1000(int16_t x);
extern int8_t  calcRESXto100(int16_t x);
#endif

#if defined(COLORLCD)
extern const char vers_stamp[];
extern const char date_stamp[];
extern const char time_stamp[];
extern const char eeprom_stamp[];
#else
extern const char vers_stamp[];
#endif

extern uint8_t            g_vbat100mV;
#define g_blinkTmr10ms (*(uint8_t*)&g_tmr10ms)
extern uint8_t            g_beepCnt;
extern uint8_t            g_beepVal[5];

extern uint8_t            ppmInState; //0=unsync 1..8= wait for value i-1
extern uint8_t            ppmInValid;
#define PPM_IN_VALID_TIMEOUT 100
extern int16_t            g_ppmIns[NUM_TRAINER];
extern int32_t            chans[NUM_CHNOUT];
extern int16_t            ex_chans[NUM_CHNOUT]; // Outputs (before LIMITS) of the last perMain
extern int16_t            channelOutputs[NUM_CHNOUT];
extern uint16_t           BandGap;

#if defined(VIRTUALINPUTS)
  #define NUM_INPUTS      (MAX_INPUTS)
#else
  #define NUM_INPUTS      (NUM_STICKS)
#endif

int intpol(int x, uint8_t idx);
int expo(int x, int k);

#if defined(CURVES) && defined(XCURVES)
  int applyCurve(int x, CurveRef & curve);
#elif defined(CURVES)
  int applyCurve(int x, int8_t idx);
#else
  #define applyCurve(x, idx) (x)
#endif

#if defined(CPUARM)
  inline int getMaximumValue(int source)
  {
    if (source < MIXSRC_FIRST_CH)
      return 100;
    else if (source <= MIXSRC_LAST_CH)
      return g_model.extendedLimits ? 150 : 100;
    else if (source >= MIXSRC_FIRST_TIMER && source <= MIXSRC_LAST_TIMER)
      return (23*60)+59;
    else
      return 30000;
  }
#endif

#if defined(XCURVES)
  int applyCustomCurve(int x, uint8_t idx);
#else
  #define applyCustomCurve(x, idx) intpol(x, idx)
#endif

#if defined(XCURVES)
  #define APPLY_EXPOS_EXTRA_PARAMS_INC , uint8_t ovwrIdx=0, int16_t ovwrValue=0
  #define APPLY_EXPOS_EXTRA_PARAMS     , uint8_t ovwrIdx, int16_t ovwrValue
#else
  #define APPLY_EXPOS_EXTRA_PARAMS_INC
  #define APPLY_EXPOS_EXTRA_PARAMS
#endif

#if defined(VIRTUALINPUTS)
void clearInputs();
void defaultInputs();
#endif

void applyExpos(int16_t *anas, uint8_t mode APPLY_EXPOS_EXTRA_PARAMS_INC);
int16_t applyLimits(uint8_t channel, int32_t value);

void evalInputs(uint8_t mode);
uint16_t anaIn(uint8_t chan);
extern int16_t calibratedStick[NUM_STICKS+NUM_POTS];

#define FLASH_DURATION 20 /*200ms*/

extern uint8_t beepAgain;
extern uint16_t lightOffCounter;
extern uint8_t flashCounter;
extern uint8_t mixWarning;

FlightModeData *flightModeAddress(uint8_t idx);
ExpoData *expoAddress(uint8_t idx);
MixData *mixAddress(uint8_t idx);
LimitData *limitAddress(uint8_t idx);
int8_t *curveAddress(uint8_t idx);
LogicalSwitchData *lswAddress(uint8_t idx);

// static variables used in evalFlightModeMixes - moved here so they don't interfere with the stack
// It's also easier to initialize them here.
#if defined(VIRTUALINPUTS)
  extern int8_t  virtualInputsTrims[NUM_INPUTS];
#else
  extern int16_t rawAnas[NUM_INPUTS];
#endif

extern int16_t  anas [NUM_INPUTS];
extern int16_t  trims[NUM_STICKS];
extern BeepANACenter bpanaCenter;

extern uint8_t s_mixer_first_run_done;

extern int8_t s_currCh;
uint8_t getExpoMixCount(uint8_t expo);
void deleteExpoMix(uint8_t expo, uint8_t idx);
void insertExpoMix(uint8_t expo, uint8_t idx);
void applyDefaultTemplate();

void incSubtrim(uint8_t idx, int16_t inc);
void instantTrim();
FORCEINLINE void evalTrims();
void copyTrimsToOffset(uint8_t ch);
void copySticksToOffset(uint8_t ch);
void moveTrimsToOffsets();

#if defined(CPUARM)
#define ACTIVE_PHASES_TYPE uint16_t
#define DELAY_POS_SHIFT    0
#define DELAY_POS_MARGIN   3
#define delayval_t         int16_t
PACK(typedef struct {
  uint16_t delay;
  int16_t  now;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  int16_t  prev;
  uint8_t  activeMix;
  uint8_t  activeExpo;
}) SwOn;
#else
#define ACTIVE_PHASES_TYPE uint8_t
#define DELAY_POS_SHIFT    10
#define DELAY_POS_MARGIN   0
#define delayval_t         int8_t
PACK(typedef struct {
  uint16_t delay:10;
  int16_t  now:2;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  int16_t  prev:2;
  int16_t  activeMix:1;
  int16_t  activeExpo:1;
}) SwOn;
#endif

extern SwOn   swOn  [MAX_MIXERS];
extern int24_t act   [MAX_MIXERS];

#ifdef BOLD_FONT
  inline bool isExpoActive(uint8_t expo)
  {
    return swOn[expo].activeExpo;
  }

  inline bool isMixActive(uint8_t mix)
  {
    return swOn[mix].activeMix;
  }
#else
  #define isExpoActive(x) false
  #define isMixActive(x) false
#endif

enum CswFunctionFamilies {
  LS_FAMILY_OFS,
  LS_FAMILY_BOOL,
  LS_FAMILY_COMP,
  LS_FAMILY_DIFF,
  LS_FAMILY_TIMER,
  LS_FAMILY_STICKY,
  LS_FAMILY_RANGE,
  LS_FAMILY_EDGE
};

uint8_t lswFamily(uint8_t func);
int16_t lswTimerValue(delayval_t val);

enum FunctionsActive {
  FUNCTION_TRAINER,
  FUNCTION_INSTANT_TRIM = FUNCTION_TRAINER+4,
  FUNCTION_VARIO,
  FUNCTION_BACKLIGHT,
#if defined(SDCARD)
  FUNCTION_LOGS,
#endif
#if defined(CPUARM)
  FUNCTION_BACKGND_MUSIC,
  FUNCTION_BACKGND_MUSIC_PAUSE,
#endif
};

#define VARIO_FREQUENCY_ZERO   700/*Hz*/
#define VARIO_FREQUENCY_RANGE  1000/*Hz*/
#define VARIO_REPEAT_ZERO      500/*ms*/
#define VARIO_REPEAT_MAX       80/*ms*/

#if defined(CPUARM)
extern CustomFunctionsContext modelFunctionsContext;
extern CustomFunctionsContext globalFunctionsContext;
inline bool isFunctionActive(uint8_t func)
{
  return globalFunctionsContext.isFuunctionActive(func) || modelFunctionsContext.isFuunctionActive(func);
}
void evalFunctions(const CustomFunctionData * functions, CustomFunctionsContext & functionsContext);
inline void customFunctionsReset()
{
  globalFunctionsContext.reset();
  modelFunctionsContext.reset();
}
#else
extern CustomFunctionsContext modelFunctionsContext;
#define isFunctionActive(func) modelFunctionsContext.isFuunctionActive(func)
void evalFunctions();
#define customFunctionsReset() modelFunctionsContext.reset()
#endif

#if defined(ROTARY_ENCODERS)
  // Global rotary encoder registers
  extern volatile rotenc_t g_rotenc[ROTARY_ENCODERS];
#elif defined(ROTARY_ENCODER_NAVIGATION)
  extern volatile rotenc_t g_rotenc[1];
#endif

#if defined(CPUARM)
  #include "telemetry/telemetry.h"
#endif

#if defined (FRSKY)
  // FrSky Telemetry
  #include "telemetry/frsky.h"
#elif defined(JETI)
  // Jeti-DUPLEX Telemetry
  #include "telemetry/jeti.h"
#elif defined(ARDUPILOT)
  // ArduPilot Telemetry
  #include "telemetry/ardupilot.h"
#elif defined(NMEA)
  // NMEA Telemetry
  #include "telemetry/nmea.h"
#elif defined(MAVLINK)
  // Mavlink Telemetry
  #include "telemetry/mavlink.h"
#endif

#define PLAY_REPEAT(x)            (x)                 /* Range 0 to 15 */
#define PLAY_NOW                  0x10
#define PLAY_BACKGROUND           0x20
#define PLAY_INCREMENT(x)         ((uint8_t)(((uint8_t)x) << 6))   /* -1, 0, 1, 2 */

/* make sure the defines below always go in numeric order */
enum AUDIO_SOUNDS {
    AU_TADA,
#if defined(CPUARM)
    AU_BYE,
#endif
#if defined(VOICE)
    AU_THROTTLE_ALERT,
    AU_SWITCH_ALERT,
    AU_BAD_EEPROM,
    AU_EEPROM_FORMATTING,
#endif
    AU_TX_BATTERY_LOW,
    AU_INACTIVITY,
#if defined(CPUARM)
    AU_A1_ORANGE,
    AU_A1_RED,
    AU_A2_ORANGE,
    AU_A2_RED,
    AU_A3_ORANGE,
    AU_A3_RED,
    AU_A4_ORANGE,
    AU_A4_RED,
    AU_RSSI_ORANGE,
    AU_RSSI_RED,
    AU_SWR_RED,
    AU_TELEMETRY_LOST,
    AU_TELEMETRY_BACK,
#endif
#if defined(PCBSKY9X)
    AU_TX_MAH_HIGH,
    AU_TX_TEMP_HIGH,
#endif
    AU_ERROR,
    AU_KEYPAD_UP,
    AU_KEYPAD_DOWN,
    AU_MENUS,
    AU_TRIM_MOVE,
    AU_WARNING1,
    AU_WARNING2,
    AU_WARNING3,
    AU_TRIM_MIDDLE,
#if defined(CPUARM)
    AU_TRIM_END,
#endif
#if defined(PCBTARANIS)
    AU_STICK1_MIDDLE,
    AU_STICK2_MIDDLE,
    AU_STICK3_MIDDLE,
    AU_STICK4_MIDDLE,
    AU_POT1_MIDDLE,
    AU_POT2_MIDDLE,
    AU_SLIDER1_MIDDLE,
    AU_SLIDER2_MIDDLE,
#elif defined(CPUARM)
    AU_STICK1_MIDDLE,
    AU_STICK2_MIDDLE,
    AU_STICK3_MIDDLE,
    AU_STICK4_MIDDLE,
    AU_POT1_MIDDLE,
    AU_POT2_MIDDLE,
    AU_POT3_MIDDLE,
#else
    AU_POT_MIDDLE,
#endif
    AU_MIX_WARNING_1,
    AU_MIX_WARNING_2,
    AU_MIX_WARNING_3,
    AU_TIMER_00,
    AU_TIMER_LT10,
    AU_TIMER_20,
    AU_TIMER_30,

    AU_FRSKY_FIRST,
    AU_FRSKY_BEEP1 = AU_FRSKY_FIRST,
    AU_FRSKY_BEEP2,
    AU_FRSKY_BEEP3,
    AU_FRSKY_WARN1,
    AU_FRSKY_WARN2,
    AU_FRSKY_CHEEP,
    AU_FRSKY_RATATA,
    AU_FRSKY_TICK,
    AU_FRSKY_SIREN,
    AU_FRSKY_RING,
    AU_FRSKY_SCIFI,
    AU_FRSKY_ROBOT,
    AU_FRSKY_CHIRP,
    AU_FRSKY_TADA,
    AU_FRSKY_CRICKET,
    AU_FRSKY_ALARMC,
    AU_FRSKY_LAST,

    AU_NONE=0xff
};

#if defined(AUDIO)
#if defined(CPUARM)
#include "audio_arm.h"
#else
#include "audio_avr.h"
#endif
#endif

#include "buzzer.h"

#if defined(PCBSTD) && defined(VOICE)
#include "targets/stock/voice.h"
#endif

#if defined(PCBGRUVIN9X) && defined(VOICE)
#include "targets/gruvin9x/somo14d.h"
#endif

#include "translations.h"
#include "fonts.h"

#if defined(HAPTIC)
#include "haptic.h"
#endif

#if defined(SDCARD)
#include "sdcard.h"
#endif

#if defined(RTCLOCK)
#include "rtc.h"
#endif

#if defined(CPUARM)
extern uint8_t requiredSpeakerVolume;
#endif

#if defined(PCBTARANIS)
extern uint8_t requestScreenshot;
#endif

extern void checkBattery();
extern void opentxClose();
extern void opentxInit();

// Re-useable byte array to save having multiple buffers
#define SD_SCREEN_FILE_LENGTH (32)
union ReusableBuffer
{
    // 275 bytes
    struct
    {
        char listnames[LCD_LINES-1][LEN_MODEL_NAME];
#if !defined(CPUARM)
        uint16_t eepromfree;
#endif
#if defined(SDCARD)
        char menu_bss[MENU_MAX_LINES][MENU_LINE_LENGTH];
        char mainname[45]; // because reused for SD backup / restore, max backup filename 44 chars: "/MODELS/MODEL0134353-2014-06-19-04-51-27.bin"
#else
        char mainname[LEN_MODEL_NAME];
#endif

    } modelsel;

    // 103 bytes
    struct
    {
        int16_t midVals[NUM_STICKS+NUM_POTS];
        int16_t loVals[NUM_STICKS+NUM_POTS];
        int16_t hiVals[NUM_STICKS+NUM_POTS];
        uint8_t state;
#if defined(PCBTARANIS)
        struct {
          uint8_t stepsCount;
          int16_t steps[XPOTS_MULTIPOS_COUNT];
          uint8_t lastCount;
          int16_t lastPosition;
        } xpotsCalib[NUM_XPOTS];
#endif
    } calib;

#if defined(SDCARD)
    // 274 bytes
    struct
    {
        char lines[LCD_LINES-1][SD_SCREEN_FILE_LENGTH+1+1]; // the last char is used to store the flags (directory) of the line
        uint32_t available;
        uint16_t offset;
        uint16_t count;
        char originalName[SD_SCREEN_FILE_LENGTH+1];
    } sdmanager;
#endif
};

extern union ReusableBuffer reusableBuffer;

void checkFlashOnBeep();

#if defined(CPUARM)
void putsValueWithUnit(coord_t x, coord_t y, lcdint_t val, uint8_t unit, LcdFlags att);
#elif defined(FRSKY)
void convertUnit(getvalue_t & val, uint8_t & unit); // TODO check FORCEINLINE on stock
#else
#define convertUnit(...)
#endif

#if defined(CPUARM)
uint8_t zlen(const char *str, uint8_t size);
bool zexist(const char *str, uint8_t size);
char * strcat_zchar(char *dest, const char *name, uint8_t size, const char *defaultName=NULL, uint8_t defaultNameSize=0, uint8_t defaultIdx=0);
#define strcat_modelname(dest, idx) strcat_zchar(dest, modelHeaders[idx].name, LEN_MODEL_NAME, STR_MODEL, PSIZE(TR_MODEL), idx+1)
#define strcat_phasename(dest, idx) strcat_zchar(dest, g_model.flightModeData[idx].name, LEN_FLIGHT_MODE_NAME, STR_FP, PSIZE(TR_FP), idx+1)
#define ZLEN(s) zlen(s, sizeof(s))
#define ZEXIST(s) zexist(s, sizeof(s))
#endif

// Stick tolerance varies between transmitters, Higher is better
#if defined (PCB9XR) || defined (PCB9XR128)
  #define STICK_TOLERANCE 16
#else
  #define STICK_TOLERANCE 64
#endif

#if defined(FRSKY_HUB) && defined(GAUGES)
enum BarThresholdIdx {
  THLD_ALT,
  THLD_RPM,
  THLD_FUEL,
  THLD_T1,
  THLD_T2,
  THLD_SPEED,
  THLD_DIST,
  THLD_GPSALT,
  THLD_CELL,
  THLD_CELLS_SUM,
  THLD_VFAS,
  THLD_CURRENT,
  THLD_CONSUMPTION,
  THLD_MAX,
};

#if defined(CPUARM)
  typedef uint16_t bar_threshold_t;
  #define FILL_THRESHOLD(idx, val) barsThresholds[idx] = (val)
#else
  typedef uint8_t bar_threshold_t;
  #define FILL_THRESHOLD(idx, val) barsThresholds[idx] = 128 + (val)
#endif

extern bar_threshold_t barsThresholds[THLD_MAX];
#else
#define FILL_THRESHOLD(idx, val)
#endif

#if defined(FRSKY)
  ls_telemetry_value_t minTelemValue(source_t channel);
  ls_telemetry_value_t maxTelemValue(source_t channel);
#else
  #define minTelemValue(channel) 255
  #define maxTelemValue(channel) 255
#endif

#if defined(CPUARM)
getvalue_t convert16bitsTelemValue(source_t channel, ls_telemetry_value_t value);
ls_telemetry_value_t max8bitsTelemValue(source_t channel);
#endif

getvalue_t convert8bitsTelemValue(source_t channel, ls_telemetry_value_t value);
getvalue_t convertLswTelemValue(LogicalSwitchData * cs);

#if defined(CPUARM)
  #define convertTelemValue(channel, value) convert16bitsTelemValue(channel, value)
  #define convertBarTelemValue(channel, value) convert8bitsTelemValue(channel, value)
  #define maxBarTelemValue(channel) max8bitsTelemValue(channel)
#else
  #define convertTelemValue(channel, value) convert8bitsTelemValue(channel, value)
  #define convertBarTelemValue(channel, value) convert8bitsTelemValue(channel, value)
  #define maxBarTelemValue(channel) maxTelemValue(channel)
#endif

#if defined(FRSKY) || defined(CPUARM)
lcdint_t applyChannelRatio(source_t channel, lcdint_t val);
#define ANA_CHANNEL_UNIT(channel) g_model.frsky.channels[channel].type
#endif

getvalue_t div10_and_round(getvalue_t value);
getvalue_t div100_and_round(getvalue_t value);

#if defined(FRSKY)
NOINLINE uint8_t getRssiAlarmValue(uint8_t alarm);

extern const pm_uint8_t bchunit_ar[];

#if defined(CPUARM)
  #define FRSKY_MULTIPLIER_MAX 5
#else
  #define FRSKY_MULTIPLIER_MAX 3
#endif

enum TelemetryViews {
  TELEMETRY_CUSTOM_SCREEN_1,
  TELEMETRY_CUSTOM_SCREEN_2,
#if defined(CPUARM)
  TELEMETRY_CUSTOM_SCREEN_3,
  TELEMETRY_CUSTOM_SCREEN_4,
  TELEMETRY_VIEW_MAX = TELEMETRY_CUSTOM_SCREEN_4
#else
  TELEMETRY_VOLTAGES_SCREEN,
  TELEMETRY_AFTER_FLIGHT_SCREEN,
  TELEMETRY_VIEW_MAX = TELEMETRY_AFTER_FLIGHT_SCREEN
#endif
};

extern uint8_t s_frsky_view;

#endif

#define EARTH_RADIUSKM ((uint32_t)6371)
#define EARTH_RADIUS ((uint32_t)111194)

#if defined(PCBTARANIS)
double gpsToDouble(bool neg, int16_t bp, int16_t ap);
#endif
void getGpsPilotPosition();
void getGpsDistance();
void varioWakeup();

#if defined(AUDIO) && defined(BUZZER)
  #define IS_SOUND_OFF() (g_eeGeneral.buzzerMode==e_mode_quiet && g_eeGeneral.beepMode==e_mode_quiet)
#else
  #define IS_SOUND_OFF() (g_eeGeneral.beepMode == e_mode_quiet)
#endif

#if defined(CPUARM)
  #define IS_IMPERIAL_ENABLE() (g_eeGeneral.imperial)
#elif defined(IMPERIAL_UNITS)
  #define IS_IMPERIAL_ENABLE() (1)
#else
  #define IS_IMPERIAL_ENABLE() (0)
#endif

#if !defined(CPUARM)
  #define IS_USR_PROTO_FRSKY_HUB()   (g_model.frsky.usrProto == USR_PROTO_FRSKY)
  #define IS_USR_PROTO_WS_HOW_HIGH() (g_model.frsky.usrProto == USR_PROTO_WS_HOW_HIGH)
#endif

#if defined(FRSKY) && defined(FRSKY_HUB) && defined(GPS)
  #define IS_GPS_AVAILABLE()         IS_USR_PROTO_FRSKY_HUB()
#else
  #define IS_GPS_AVAILABLE()         (0)
#endif

#if defined(PCBTARANIS)
  extern const pm_uchar logo_taranis[];
#endif

#if defined(USB_MASS_STORAGE)
  extern void usbPluggedIn();
#endif

#include "lua_api.h"

#if defined(SDCARD)
enum ClipboardType {
  CLIPBOARD_TYPE_NONE,
  CLIPBOARD_TYPE_CUSTOM_SWITCH,
  CLIPBOARD_TYPE_CUSTOM_FUNCTION,
  CLIPBOARD_TYPE_SD_FILE,
};

#if defined(SIMU)
  #define CLIPBOARD_PATH_LEN 1024
#else
  #define CLIPBOARD_PATH_LEN 32
#endif

struct Clipboard {
  ClipboardType type;
  union {
    LogicalSwitchData csw;
    CustomFunctionData cfn;
    struct {
      char directory[CLIPBOARD_PATH_LEN];
      char filename[CLIPBOARD_PATH_LEN];
    } sd;
  } data;
};

extern Clipboard clipboard;
#endif

#if !defined(SIMU)
extern uint16_t s_anaFilt[NUMBER_ANALOG];
#endif

#endif
