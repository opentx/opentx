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

#if defined(PCBTARANIS)
#define IF_9X(x)
#else
#define IF_9X(x) x,
#endif

#if defined(PCBSKY9X)
#define IF_PCBSKY9X(x) x,
#else
#define IF_PCBSKY9X(x)
#endif

#if defined(PCBTARANIS)
#define IF_PCBTARANIS()    true
#define CASE_PCBTARANIS(x) x,
#else
#define IF_PCBTARANIS()    false
#define CASE_PCBTARANIS(x)
#endif

#if defined(CPUARM)
#define IF_CPUARM(x) x,
#else
#define IF_CPUARM(x)
#endif

#if defined(BATTGRAPH) || defined(PCBTARANIS)
#define IF_BATTGRAPH(x) x,
#else
#define IF_BATTGRAPH(x)
#endif

#if defined(CPUARM) || defined(PCBGRUVIN9X)
#define IF_PERSISTENT_TIMERS(x) x,
#else
#define IF_PERSISTENT_TIMERS(x)
#endif

#if defined(RTCLOCK)
#define IF_RTCLOCK(x) x,
#else
#define IF_RTCLOCK(x)
#endif

#if defined(BUZZER)
#define IF_BUZZER(x) x,
#else
#define IF_BUZZER(x)
#endif

#if defined(AUDIO)
#define IF_AUDIO(x) x,
#else
#define IF_AUDIO(x)
#endif

#if defined(VOICE)
#define IF_VOICE(x) x,
#else
#define IF_VOICE(x)
#endif

#if defined(PWM_BACKLIGHT)
#define CASE_PWM_BACKLIGHT(x) x,
#else
#define CASE_PWM_BACKLIGHT(x)
#endif

#if defined(FRSKY) && defined(FRSKY_HUB) && defined(GPS)
#define IF_GPS(x) x,
#else
#define IF_GPS(x)
#endif

#if defined(VARIO)
#define IF_VARIO(x) x,
#else
#define IF_VARIO(x)
#endif

#if defined(HAPTIC)
#define IF_HAPTIC(x) x,
#else
#define IF_HAPTIC(x)
#endif

#if defined(SPLASH)
#define IF_SPLASH(x) x,
#else
#define IF_SPLASH(x)
#endif

#if defined(FRSKY)
#define IF_FRSKY(x) x,
#else
#define IF_FRSKY(x)
#endif

#if defined(PXX)
#define IF_PXX(x) x,
#else
#define IF_PXX(x)
#endif

#if defined(SDCARD)
#define IF_SDCARD(x) x,
#else
#define IF_SDCARD(x)
#endif

#if defined(BLUETOOTH)
#define IF_BLUETOOTH(x) x,
#else
#define IF_BLUETOOTH(x)
#endif

#if defined(HELI)
#define IF_HELI(x) x,
#else
#define IF_HELI(x)
#endif

#if defined(TEMPLATES)
#define IF_TEMPLATES(x) x,
#else
#define IF_TEMPLATES(x)
#endif

#if defined(FLIGHT_MODES)
#define IF_FLIGHT_MODES(x) x,
#else
#define IF_FLIGHT_MODES(x)
#endif

#if defined(CURVES)
#define IF_CURVES(x) x,
#else
#define IF_CURVES(x)
#endif

#if defined(GVARS)
#define IF_GVARS(x) x,
#else
#define IF_GVARS(x)
#endif

#if ROTARY_ENCODERS > 0
  #define ROTARY_ENCODER_NAVIGATION
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

#define IS_FAI_FORBIDDEN(idx) (IS_FAI_ENABLED() && idx > MIXSRC_FIRST_TELEM-1+TELEM_A2-1)

#if defined(SIMU)
  #ifndef FORCEINLINE
    #define FORCEINLINE
  #endif
  #if !defined(NOINLINE)
    #define NOINLINE
  #endif
  #define CONVERT_PTR(x) ((uint32_t)(uint64_t)(x))
#else
  #define FORCEINLINE inline __attribute__ ((always_inline))
  #define NOINLINE __attribute__ ((noinline))
  #define SIMU_SLEEP(x)
  #define CONVERT_PTR(x) ((uint32_t)(x))
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
#else
  #include "targets/stock/board_stock.h"
#endif

#include "debug.h"

#if defined(SIMU)
#include "simpgmspace.h"
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

#define NUM_STICKS    4

enum EnumKeys {
  KEY_MENU,
  KEY_EXIT,
#if defined(PCBTARANIS)
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

  TRM_BASE,
  TRM_LH_DWN = TRM_BASE,
  TRM_LH_UP,
  TRM_LV_DWN,
  TRM_LV_UP,
  TRM_RV_DWN,
  TRM_RV_UP,
  TRM_RH_DWN,
  TRM_RH_UP,

#if ROTARY_ENCODERS > 0 || defined(ROTARY_ENCODER_NAVIGATION)
  BTN_REa,
#endif
#if ROTARY_ENCODERS > 0
  BTN_REb,
#endif

  NUM_KEYS,
  SW_BASE=NUM_KEYS,

#if defined(PCBTARANIS)
  SW_SA0=SW_BASE,
  SW_SA1,
  SW_SA2,
  SW_SB0,
  SW_SB1,
  SW_SB2,
  SW_SC0,
  SW_SC1,
  SW_SC2,
  SW_SD0,
  SW_SD1,
  SW_SD2,
  SW_SE0,
  SW_SE1,
  SW_SE2,
  SW_SF0,
  SW_SF2,
  SW_SG0,
  SW_SG1,
  SW_SG2,
  SW_SH0,
  SW_SH2,
#else
  SW_ID0=SW_BASE,
  SW_ID1,
  SW_ID2,
#if defined(EXTRA_3POS)
  SW_ID3,
  SW_ID4,
  SW_ID5,
#endif

  SW_THR,
  SW_RUD,
  SW_ELE,
  SW_AIL,
  SW_GEA,
  SW_TRN,
#endif

};

#if defined(PCBTARANIS)
  #define NUM_SWITCHES  8
  #define IS_3POS(sw)   ((sw) != 5 && (sw) != 7)
  #define MAX_PSWITCH   (SW_SH2-SW_SA0+1)
  #define NUM_POTS      4
  #define NUM_SW_SRCRAW 8
  #define SWSRC_THR     SWSRC_SF2
  #define SWSRC_GEA     SWSRC_SG2
  #define SWSRC_ID0     SWSRC_SA0
  #define SWSRC_ID1     SWSRC_SA1
  #define SWSRC_ID2     SWSRC_SA2
#else
  #define NUM_SWITCHES  7
  #define IS_3POS(sw)   ((sw) == 0)
  #define IS_MOMENTARY(sw) (sw == SWSRC_TRN)
  #define MAX_PSWITCH   (SW_TRN-SW_ID0+1)  // 9 physical switches
  #define NUM_POTS      3
  #define NUM_SW_SRCRAW 1
#endif

#define MAX_SWITCH    (MAX_PSWITCH+NUM_CSW)

#if defined(PCBTARANIS)
#define KEY_RIGHT  KEY_PLUS
#define KEY_LEFT   KEY_MINUS
#define KEY_UP     KEY_PLUS
#define KEY_DOWN   KEY_MINUS
#else
#define KEY_ENTER  KEY_MENU
#define KEY_PLUS   KEY_RIGHT
#define KEY_MINUS  KEY_LEFT
#endif

#include "myeeprom.h"

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
  // This doesn't need protection on this processor
  #define tmr10ms_t uint32_t
  extern volatile tmr10ms_t g_tmr10ms;
  #define get_tmr10ms() g_tmr10ms
  typedef int32_t rotenc_t;
  typedef int32_t getvalue_t;
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

#if defined(PCBSKY9X)
  #include "eeprom_raw.h"
#else
  #include "eeprom_rlc.h"
#endif

#if defined(CPUARM)
  #include "protocols/pulses_arm.h"
#else
  #include "protocols/pulses_avr.h"
#endif

#if defined(PCBTARANIS)
  #define MODEL_BITMAP_WIDTH  64
  #define MODEL_BITMAP_HEIGHT 32
  #define MODEL_BITMAP_SIZE   (2+4*(MODEL_BITMAP_WIDTH*MODEL_BITMAP_HEIGHT/8))
  extern uint8_t modelBitmap[MODEL_BITMAP_SIZE];
  void loadModelBitmap(char *name, uint8_t *bitmap);
  #define LOAD_MODEL_BITMAP() loadModelBitmap(g_model.header.bitmap, modelBitmap)
#else
  #define LOAD_MODEL_BITMAP()
#endif

#if defined(DSM2)
  extern bool s_bind_mode;
  extern bool s_rangecheck_mode;
  extern uint8_t s_bind_allowed;
#endif

#if defined(CPUARM)
  #define IS_PPM_PROTOCOL(protocol)     (protocol==PROTO_PPM)
#else
  #define IS_PPM_PROTOCOL(protocol)     (protocol<=PROTO_PPMSIM)
#endif

#if defined(PXX)
  #define IS_PXX_PROTOCOL(protocol)  (protocol==PROTO_PXX)
#else
  #define IS_PXX_PROTOCOL(protocol)  (0)
#endif

#if defined(DSM2)
  #define IS_DSM2_PROTOCOL(protocol) (protocol>=PROTO_DSM2_LP45 && protocol<=PROTO_DSM2_DSMX)
#else
  #define IS_DSM2_PROTOCOL(protocol) (0)
#endif

#if defined(DSM2_SERIAL)
  #define IS_DSM2_SERIAL_PROTOCOL(protocol)  (IS_DSM2_PROTOCOL(protocol))
#else
  #define IS_DSM2_SERIAL_PROTOCOL(protocol)  (0)
#endif

#if defined(PCBTARANIS)
  static const int8_t maxChannelsModules[] = { 0, 8, 8, 0, 0 };
  static const int8_t maxChannelsXJT[] = { 0, 8, 0, 4 };
  #define IS_MODULE_XJT(idx)        ((idx==0 || g_model.externalModule == MODULE_TYPE_XJT) && (g_model.moduleData[idx].rfProtocol != RF_PROTO_OFF))
  #define IS_MODULE_PPM(idx)        (idx==2 || (idx==1 && g_model.externalModule == MODULE_TYPE_PPM))
  #define NUM_CHANNELS(idx)         (8+g_model.moduleData[idx].channelsCount)
  #define MAX_PORT1_CHANNELS()      (maxChannelsXJT[1+g_model.moduleData[0].rfProtocol])
  #define MAX_PORT2_CHANNELS()      ((g_model.externalModule == MODULE_TYPE_XJT) ? maxChannelsXJT[1+g_model.moduleData[1].rfProtocol] : maxChannelsModules[g_model.externalModule])
  #define MAX_TRAINER_CHANNELS()    (8)
  #define MAX_CHANNELS(idx)         (idx==0 ? MAX_PORT1_CHANNELS() : (idx==1 ? MAX_PORT2_CHANNELS() : MAX_TRAINER_CHANNELS()))
#elif defined(PCBSKY9X)
  #define NUM_PORT1_CHANNELS()      (IS_PXX_PROTOCOL(g_model.protocol) ? 8 : (IS_DSM2_PROTOCOL(g_model.protocol) ? 6 : (8+g_model.moduleData[0].channelsCount)))
  #define NUM_PORT2_CHANNELS()      (8+g_model.moduleData[1].channelsCount)
  #define NUM_CHANNELS(idx)         (idx==0 ? NUM_PORT1_CHANNELS() : (8+g_model.moduleData[idx].channelsCount))
#endif

#include "lcd.h"
#include "gui/menus.h"

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
//NOTICE!  =>  1..4 -> 1..4
#define CONVERT_MODE(x)  (((x)<=4) ? pgm_read_byte(modn12x3 + 4*stickMode + (x)-1) : (x) )

#define RUD_STICK 0
#define ELE_STICK 1
#define THR_STICK 2
#define AIL_STICK 3

extern uint8_t channel_order(uint8_t x);

class Key
{
#define FILTERBITS      4
#ifdef SIMU
#define FFVAL 1
#else
#define FFVAL          ((1<<FILTERBITS)-1)
#endif
#define KSTATE_OFF      0
#define KSTATE_RPTDELAY 95 // gruvin: delay state before key repeating starts
#define KSTATE_START    97
#define KSTATE_PAUSE    98
#define KSTATE_KILLED   99
  uint8_t m_vals;   // key debounce?  4 = 40ms
  uint8_t m_cnt;
  uint8_t m_state;
public:
  void input(bool val, EnumKeys enuk);
  bool state()       { return m_vals > 0; }
  void pauseEvents() { m_state = KSTATE_PAUSE; m_cnt = 0; }
  void killEvents()  { m_state = KSTATE_KILLED; }
};

extern Key keys[NUM_KEYS];

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

#define SWASH_TYPE_120   1
#define SWASH_TYPE_120X  2
#define SWASH_TYPE_140   3
#define SWASH_TYPE_90    4
#define SWASH_TYPE_NUM   4

enum CswFunctions {
  CS_OFF,
  CS_VEQUAL, // v==offset
  CS_VPOS,   // v>offset
  CS_VNEG,   // v<offset
  CS_APOS,   // |v|>offset
  CS_ANEG,   // |v|<offset
  CS_AND,
  CS_OR,
  CS_XOR,
  CS_EQUAL,
  CS_GREATER,
  CS_LESS,
  CS_DIFFEGREATER,
  CS_ADIFFEGREATER,
  CS_TIMER,
  CS_MAXF = CS_TIMER
};

#define CS_VOFS         0
#define CS_VBOOL        1
#define CS_VCOMP        2
#define CS_VDIFF        3
#define CS_VTIMER       4
uint8_t cswFamily(uint8_t func);
int16_t cswTimerValue(int8_t val);

#define NUM_CYC         3
#define NUM_CAL_PPM     4
#define NUM_PPM         8

#define THRCHK_DEADBAND 16

#if defined(FSPLASH) || defined(XSPLASH)
#define SPLASH_TIMEOUT  (g_eeGeneral.splashMode == 0 ? 60000/*infinite=10mn*/ : ((4*100) * (g_eeGeneral.splashMode & 0x03)))
#else
#define SPLASH_TIMEOUT  (4*100)  // 4 seconds
#endif

#define EVT_KEY_MASK(e)      ((e) & 0x1f)

#define _MSK_KEY_BREAK       0x20
#define _MSK_KEY_REPT        0x40
#define _MSK_KEY_FIRST       0x60
#define _MSK_KEY_LONG        0x80

#define EVT_KEY_BREAK(key)   ((key)|_MSK_KEY_BREAK)
#define EVT_KEY_FIRST(key)   ((key)|_MSK_KEY_FIRST)
#define EVT_KEY_REPT(key)    ((key)|_MSK_KEY_REPT)
#define EVT_KEY_LONG(key)    ((key)|_MSK_KEY_LONG)

#define IS_KEY_BREAK(evt)    (((evt)&0xe0) == _MSK_KEY_BREAK)
#define IS_KEY_FIRST(evt)    (((evt)&0xe0) == _MSK_KEY_FIRST)
#define IS_KEY_LONG(evt)     (((evt)&0xe0) == _MSK_KEY_LONG)

#define EVT_ENTRY            0xbf
#define EVT_ENTRY_UP         0xbe
#define EVT_MENU_UP          0xbd

#if defined(PCBTARANIS)
  #define EVT_ROTARY_BREAK   EVT_KEY_BREAK(KEY_ENTER)
  #define EVT_ROTARY_LONG    EVT_KEY_LONG(KEY_ENTER)
#else
  #define EVT_ROTARY_BREAK   0xcf
  #define EVT_ROTARY_LONG    0xce
  #define EVT_ROTARY_LEFT    0xdf
  #define EVT_ROTARY_RIGHT   0xde
#endif

#if defined(PCBTARANIS)
  #define IS_ROTARY_LEFT(evt)   (evt==EVT_KEY_FIRST(KEY_MINUS) || evt==EVT_KEY_REPT(KEY_MINUS))
  #define IS_ROTARY_RIGHT(evt)  (evt==EVT_KEY_FIRST(KEY_PLUS) || evt==EVT_KEY_REPT(KEY_PLUS))
  #define IS_ROTARY_UP(evt)     (evt==EVT_KEY_FIRST(KEY_PLUS) || evt==EVT_KEY_REPT(KEY_PLUS))
  #define IS_ROTARY_DOWN(evt)   (evt==EVT_KEY_FIRST(KEY_MINUS) || evt==EVT_KEY_REPT(KEY_MINUS))
  #define IS_ROTARY_BREAK(evt)  (evt==EVT_KEY_BREAK(KEY_ENTER))
  #define IS_ROTARY_LONG(evt)   (evt==EVT_KEY_LONG(KEY_ENTER))
  #define IS_ROTARY_EVENT(evt)  (0)
  #define CASE_EVT_ROTARY_BREAK /*case EVT_KEY_BREAK(KEY_ENTER):*/
  #define CASE_EVT_ROTARY_LONG  /*case EVT_KEY_LONG(KEY_ENTER):*/
  #define CASE_EVT_ROTARY_LEFT  case EVT_KEY_FIRST(KEY_MOVE_DOWN): case EVT_KEY_REPT(KEY_MOVE_DOWN):
  #define CASE_EVT_ROTARY_RIGHT case EVT_KEY_FIRST(KEY_MOVE_UP): case EVT_KEY_REPT(KEY_MOVE_UP):
#elif defined(ROTARY_ENCODER_NAVIGATION)
  #define IS_ROTARY_LEFT(evt)   (evt == EVT_ROTARY_LEFT)
  #define IS_ROTARY_RIGHT(evt)  (evt == EVT_ROTARY_RIGHT)
  #define IS_ROTARY_UP(evt)     IS_ROTARY_LEFT(evt)
  #define IS_ROTARY_DOWN(evt)   IS_ROTARY_RIGHT(evt)
  #define IS_ROTARY_BREAK(evt)  (evt == EVT_ROTARY_BREAK)
  #define IS_ROTARY_LONG(evt)   (evt == EVT_ROTARY_LONG)
  #define IS_ROTARY_EVENT(evt)  (EVT_KEY_MASK(evt) >= 0x0e)
  #define CASE_EVT_ROTARY_BREAK case EVT_ROTARY_BREAK:
  #define CASE_EVT_ROTARY_LONG  case EVT_ROTARY_LONG:
  #define CASE_EVT_ROTARY_LEFT  case EVT_ROTARY_LEFT:
  #define CASE_EVT_ROTARY_RIGHT case EVT_ROTARY_RIGHT:
#else
  #define IS_ROTARY_LEFT(evt)   (0)
  #define IS_ROTARY_RIGHT(evt)  (0)
  #define IS_ROTARY_UP(evt)     (0)
  #define IS_ROTARY_DOWN(evt)   (0)
  #define IS_ROTARY_BREAK(evt)  (0)
  #define IS_ROTARY_LONG(evt)   (0)
  #define IS_ROTARY_EVENT(evt)  (0)
  #define CASE_EVT_ROTARY_BREAK
  #define CASE_EVT_ROTARY_LONG
  #define CASE_EVT_ROTARY_LEFT
  #define CASE_EVT_ROTARY_RIGHT
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
  uint16_t sum;
};
extern struct t_inactivity inactivity;

#if defined(PXX)
extern uint8_t pxxFlag[NUM_MODULES];
#endif

#define PXX_SEND_RXNUM       0x01
#define PXX_SEND_FAILSAFE    (1 << 4)
#define PXX_SEND_RANGECHECK  (1 << 5)

#define ZCHAR_MAX (40 + LEN_SPECIAL_CHARS)

extern char idx2char(int8_t idx);

extern uint8_t s_evt;
#define putEvent(evt) s_evt = evt
void clearKeyEvents();
void pauseEvents(uint8_t enuk);
void killEvents(uint8_t enuk);
#if defined(CPUARM)
  uint8_t getEvent(bool trim);
#else
  uint8_t getEvent();
#endif

uint8_t keyDown();

enum PowerState {
  e_power_on,
  e_power_trainer,
  e_power_usb,
  e_power_off
};

bool switchState(EnumKeys enuk);
void readKeysAndTrims();

uint16_t evalChkSum();

#if defined(VOICE)
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
  e_perout_mode_inactive_phase = 1,
  e_perout_mode_notrainer = 2,
  e_perout_mode_notrims = 4,
  e_perout_mode_nosticks = 8,
  e_perout_mode_noinput = e_perout_mode_notrainer+e_perout_mode_notrims+e_perout_mode_nosticks
};

// Fiddle to force compiler to use a pointer
#if defined(CPUARM) || defined(SIMU)
  #define FORCE_INDIRECT(ptr)
#else
  #define FORCE_INDIRECT(ptr) __asm__ __volatile__ ("" : "=e" (ptr) : "0" (ptr))
#endif

extern uint8_t s_perout_flight_phase;

#if defined(CPUARM)
  #define bitfield_channels_t uint32_t
#else
  #define bitfield_channels_t uint16_t
#endif

void perOut(uint8_t mode, uint8_t tick10ms);
void perMain();
NOINLINE void per10ms();

getvalue_t getValue(uint8_t i);
bool       getSwitch(int8_t swtch);

extern swstate_t switches_states;
int8_t  getMovedSwitch();
#if defined(AUTOSOURCE)
int8_t getMovedSource();
#endif

#if defined(FLIGHT_MODES)
  extern uint8_t getFlightPhase();
#else
  #define getFlightPhase() 0
#endif

extern uint8_t getTrimFlightPhase(uint8_t phase, uint8_t idx);
extern int16_t getRawTrimValue(uint8_t phase, uint8_t idx);
extern int16_t getTrimValue(uint8_t phase, uint8_t idx);
extern void setTrimValue(uint8_t phase, uint8_t idx, int16_t trim);

#if defined(ROTARY_ENCODERS)
  int16_t getRotaryEncoder(uint8_t idx);
  void incRotaryEncoder(uint8_t idx, int8_t inc);
#endif

#if defined(PCBGRUVIN9X)
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
  #endif

  #define GVAR_DISPLAY_TIME     100 /*1 second*/;
  extern uint8_t s_gvar_timer;
  extern uint8_t s_gvar_last;
#else
  #define GET_GVAR(x, ...) (x)
#endif

#if defined(CPUARM)
  #define GV1_SMALL  128
  // define here range for ARM based controllers; could be nearly unlimited, but could cause problems in mixer calculation (overflows)
  #define GV1_LARGE  512
#else
  #define GV1_SMALL  128
  #define GV1_LARGE  256
#endif

#define GV_IS_GV_VALUE(x,min,max)    ( (x>max) || (x<min) )
#define GV_GET_GV1_VALUE(max)        ( (max<=GV_RANGESMALL) ? GV1_SMALL : GV1_LARGE )
#define GV_INDEX_CALCULATION(x,max)  ( (max<=GV1_SMALL) ? (uint8_t) x-GV1_SMALL  : \
                                       (  (x&(GV1_LARGE*2-1))-GV1_LARGE ) )
#define GV_INDEX_CALC_DELTA(x,delta) ((x&(delta*2-1)) - delta)

#define GV_CALC_VALUE_IDX_POS(idx,delta) (-delta+idx)
#define GV_CALC_VALUE_IDX_NEG(idx,delta) (delta+idx)

#define GV_RANGESMALL      (GV1_SMALL - (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGESMALL_NEG  (-GV1_SMALL + (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGELARGE      (GV1_LARGE - (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGELARGE_NEG  (-GV1_LARGE + (RESERVE_RANGE_FOR_GVARS+1))

extern uint16_t s_timeCumTot;
extern uint16_t s_timeCumThr;  //gewichtete laufzeit in 1/16 sec
extern uint16_t s_timeCum16ThrP; //gewichtete laufzeit in 1/16 sec

struct TimerState {
  uint8_t  lastPos;
  uint16_t cnt;
  uint16_t sum;
  uint8_t  toggled;
  uint8_t  state;
  int16_t  val;
  uint8_t  val_10ms;
};

extern TimerState timersStates[MAX_TIMERS];

extern int8_t safetyCh[NUM_CHNOUT];

extern uint8_t trimsCheckTimer;

extern int16_t csLastValue[NUM_CSW];
#define CS_LAST_VALUE_INIT -32768

#define TMR_OFF     0
#define TMR_RUNNING 1
#define TMR_BEEPING 2
#define TMR_STOPPED 3
void resetTimer(uint8_t idx);
void resetAll();

extern uint8_t unexpectedShutdown;
extern uint8_t g_tmr1Latency_max;
extern uint8_t g_tmr1Latency_min;
extern uint16_t maxMixerDuration;
extern uint16_t lastMixerDuration;

#if defined(THRTRACE)
  #define MAXTRACE (LCD_W - 8)
  extern uint8_t s_traceBuf[MAXTRACE];
  extern uint8_t s_traceWr;
  extern int     s_traceCnt;
#endif

#if defined(PCBTARANIS)
  static inline uint16_t getTmr2MHz() { return 0; }
#elif defined(PCBSKY9X)
  static inline uint16_t getTmr2MHz() { return TC1->TC_CHANNEL[0].TC_CV; }
#else
  uint16_t getTmr16KHz();
#endif

#if defined(CPUARM)
  uint16_t stack_free(uint8_t tid);
#else
  uint16_t stack_free();
#endif

#if defined(CPUM64)
  void memclear(void *ptr, uint8_t size);
#else
  #define memclear(p, s) memset(p, 0, s)
#endif

#if defined(SPLASH)
  void doSplash();
#endif

void checkLowEEPROM();
void checkTHR();
void checkSwitches();
void checkAlarm();
void checkAll();

#define ADC_VREF_TYPE 0x40 // AVCC with external capacitor at AREF pin

#if !defined(SIMU)
  void getADC();
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
  SLIDER1,
  SLIDER2,
#else
  POT1,
  POT2,
  POT3,
#endif
  TX_VOLTAGE,
#if defined(PCBSKY9X) && !defined(REVA)
  TX_CURRENT,
#endif
  NUMBER_ANALOG
};

#if defined(PCBSTD) && defined(VOICE) && !defined(SIMU)
  #define BACKLIGHT_ON()    (Voice.Backlight = 1)
  #define BACKLIGHT_OFF()   (Voice.Backlight = 0)
#else
  #define BACKLIGHT_ON()    __BACKLIGHT_ON
  #define BACKLIGHT_OFF()   __BACKLIGHT_OFF
#endif

#define BUZZER_ON     PORTE |=  (1 << OUT_E_BUZZER)
#define BUZZER_OFF    PORTE &= ~(1 << OUT_E_BUZZER)

#if defined(HAPTIC)
  #if defined(PCBSKY9X)
    #define HAPTIC_OFF    hapticOff()
  #elif defined(PCBGRUVIN9X)
    #define HAPTIC_ON     PORTD &= ~(1 << OUT_D_HAPTIC)
    #define HAPTIC_OFF    PORTD |=  (1 << OUT_D_HAPTIC)
  #else
    #define HAPTIC_ON     PORTG |=  (1 << OUT_G_HAPTIC)
    #define HAPTIC_OFF    PORTG &= ~(1 << OUT_G_HAPTIC)
  #endif
#else
  #define HAPTIC_ON
  #define HAPTIC_OFF
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

#if defined(HELI) || defined(FRSKY_HUB)
uint16_t isqrt32(uint32_t n);
#endif

#if defined(CPUARM)
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

#if defined(CPUARM) || defined(PCBGRUVIN9X)
void saveTimers();
#else
#define saveTimers()
#endif

void generalDefault();
void modelDefault(uint8_t id);

#if defined(PXX) && defined(CPUARM)
void checkModelIdUnique(uint8_t id);
#endif

#if defined(CPUARM)
inline int16_t calc100to256_16Bits(register int16_t x)  // @@@2 open.20.fsguruh: return x*2.56
{
  return ((int16_t) x * 256) / 100;
}

inline int16_t calc100to256(register int8_t x)  // @@@2 open.20.fsguruh: return x*2.56
{
  return ((int16_t) x * 256) / 100;
}

inline int16_t calc100toRESX_16Bits(register int16_t x) // @@@ open.20.fsguruh
{
  return x * 1024 / 100;
}

inline int32_t calc100toRESX(register int8_t x)
{
  return x * 1024 / 100;
}

inline int16_t calc1000toRESX(register int32_t x)  // improve calc time by Pat MacKenzie
{
  return x * 1024 / 1000;
}

inline int16_t calcRESXto1000(register int32_t x)
{
  return x * 1000 / 1024;
}

inline int16_t calcRESXto100(register int32_t x)
{
  return x * 100 / 1024;
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

#define TMR_VAROFS  5

extern const char stamp1[];
extern const char stamp2[];
extern const char stamp3[];
extern const char eeprom_stamp[];

extern uint8_t            g_vbat100mV;
#define g_blinkTmr10ms (*(uint8_t*)&g_tmr10ms)
extern uint8_t            g_beepCnt;
extern uint8_t            g_beepVal[5];

extern uint8_t            ppmInState; //0=unsync 1..8= wait for value i-1
extern int16_t            g_ppmIns[8];
extern int32_t            chans[NUM_CHNOUT];
extern int16_t            ex_chans[NUM_CHNOUT]; // Outputs (before LIMITS) of the last perMain
extern int16_t            channelOutputs[NUM_CHNOUT];
extern uint16_t           BandGap;

#if defined(CPUARM)
  #define NUM_INPUTS      (NUM_STICKS)
#else
  #define NUM_INPUTS      (NUM_STICKS)
#endif

extern int16_t expo(int16_t x, int16_t k);
extern int16_t intpol(int16_t, uint8_t);
extern int16_t applyCurve(int16_t, int8_t);
extern void applyExpos(int16_t *anas, uint8_t mode);
extern int16_t applyLimits(uint8_t channel, int32_t value);

extern uint16_t anaIn(uint8_t chan);
extern int16_t thrAnaIn(uint8_t chan);
extern int16_t calibratedStick[NUM_STICKS+NUM_POTS];

#define FLASH_DURATION 20 /*200ms*/

extern uint8_t beepAgain;
extern uint16_t lightOffCounter;
extern uint8_t flashCounter;
extern uint8_t mixWarning;

extern PhaseData *phaseAddress(uint8_t idx);
extern ExpoData *expoAddress(uint8_t idx);
extern MixData *mixAddress(uint8_t idx);
extern LimitData *limitAddress(uint8_t idx);
extern int8_t *curveAddress(uint8_t idx);
extern CustomSwData *cswAddress(uint8_t idx);

struct CurveInfo {
  int8_t *crv;
  uint8_t points;
  bool custom;
};
extern CurveInfo curveInfo(uint8_t idx);

extern void deleteExpoMix(uint8_t expo, uint8_t idx);

extern void incSubtrim(uint8_t idx, int16_t inc);
extern void instantTrim();
extern void moveTrimsToOffsets();

#if defined(CPUARM)
  #define ACTIVE_PHASES_TYPE uint16_t
#else
  #define ACTIVE_PHASES_TYPE uint8_t
#endif

PACK(typedef struct t_SwOn {
  uint16_t delay:10;
  int16_t  now:2;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  int16_t  prev:2;
  int16_t  activeMix:1;
  int16_t  activeExpo:1;
}) SwOn;
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

#if defined(CPUARM)
  #define MASK_CFN_TYPE uint32_t  // current max = 32 function switches
  #define MASK_FUNC_TYPE uint32_t // current max = 32 functions
#elif defined(CPUM64)
  #define MASK_CFN_TYPE uint16_t  // current max = 16 function switches
  #define MASK_FUNC_TYPE uint16_t // current max = 16 functions
#else
  #define MASK_CFN_TYPE uint32_t  // current max = 32 function switches
  #define MASK_FUNC_TYPE uint16_t // current max = 16 functions
#endif

extern MASK_CFN_TYPE  activeSwitches;
extern MASK_CFN_TYPE  activeFnSwitches;
extern MASK_FUNC_TYPE activeFunctions;
extern tmr10ms_t lastFunctionTime[NUM_CFN];

inline bool isFunctionActive(uint8_t func)
{
  return activeFunctions & ((MASK_FUNC_TYPE)1 << (func-FUNC_TRAINER));
}

#if defined(ROTARY_ENCODERS)
  // Global rotary encoder registers
  extern volatile rotenc_t g_rotenc[ROTARY_ENCODERS];
#elif defined(ROTARY_ENCODER_NAVIGATION)
  extern volatile rotenc_t g_rotenc[1];
#endif

#if defined(FRSKY_SPORT)
  // FrSky SPORT Telemetry
  #include "telemetry/frsky_sport.h"
#elif defined (FRSKY)
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
    AU_INACTIVITY,
    AU_TX_BATTERY_LOW,
#if defined(PCBSKY9X)
    AU_TX_MAH_HIGH,
    AU_TX_TEMP_HIGH,
#endif
#if defined(VOICE)
    AU_THROTTLE_ALERT,
    AU_SWITCH_ALERT,
    AU_BAD_EEPROM,
    AU_EEPROM_FORMATTING,
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
    AU_TADA,
    AU_POT_STICK_MIDDLE,
    AU_MIX_WARNING_1,
    AU_MIX_WARNING_2,
    AU_MIX_WARNING_3,
    AU_TIMER_LT10,
    AU_TIMER_20,
    AU_TIMER_30,
#if defined(PCBTARANIS)
    AU_A1_ORANGE,
    AU_A1_RED,
    AU_A2_ORANGE,
    AU_A2_RED,
    AU_RSSI_ORANGE,
    AU_RSSI_RED,
    AU_SWR_RED,
#endif
    AU_FRSKY_FIRST,
    AU_FRSKY_BEEP1 = AU_FRSKY_FIRST,
    AU_FRSKY_BEEP2,
    AU_FRSKY_BEEP3,
    AU_FRSKY_WARN1,
    AU_FRSKY_WARN2,
    AU_FRSKY_CHEEP,
    AU_FRSKY_RING,
    AU_FRSKY_SCIFI,
    AU_FRSKY_ROBOT,
    AU_FRSKY_CHIRP,
    AU_FRSKY_TADA,
    AU_FRSKY_CRICKET,
    AU_FRSKY_SIREN,
    AU_FRSKY_ALARMC,
    AU_FRSKY_RATATA,
    AU_FRSKY_TICK,
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

#if defined(BUZZER)
#include "buzzer.h"
#endif

#if defined(PCBSTD) && defined(VOICE)
#include "targets/stock/voice.h"
#endif

#if defined(PCBGRUVIN9X) && defined(VOICE)
#include "targets/gruvin9x/somo14d.h"
#endif

#include "translations.h"

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

// Re-useable byte array to save having multiple buffers
#define SD_SCREEN_FILE_LENGTH (32)
union ReusableBuffer
{
    struct
    {
        char listnames[LCD_LINES-1][LEN_MODEL_NAME];
        uint16_t eepromfree;
#if defined(SDCARD)
        char menu_bss[MENU_MAX_LINES][MENU_LINE_LENGTH];
        char mainname[42]; // because reused for SD backup / restore
#else
        char mainname[LEN_MODEL_NAME];
#endif

    } modelsel;

    struct
    {
        int16_t midVals[NUM_STICKS+NUM_POTS];
        int16_t loVals[NUM_STICKS+NUM_POTS];
        int16_t hiVals[NUM_STICKS+NUM_POTS];
        uint8_t state;
    } calib;

#if defined(SDCARD)
    struct
    {
        char lines[LCD_LINES-1][SD_SCREEN_FILE_LENGTH+1+1]; // the last char is used to store the flags (directory) of the line
        uint32_t available;
        uint16_t offset;
        uint16_t count;
    } sdmanager;
#endif
};

extern union ReusableBuffer reusableBuffer;

void checkFlashOnBeep();

#if defined(FRSKY) || defined(CPUARM)
FORCEINLINE void convertUnit(getvalue_t & val, uint8_t & unit);
void putsTelemetryValue(xcoord_t x, uint8_t y, lcdint_t val, uint8_t unit, uint8_t att);
#else
#define convertUnit(...)
#endif

#if defined(CPUARM)
uint8_t zlen(const char *str, uint8_t size);
char * strcat_zchar(char * dest, char * name, uint8_t size, const char *defaultName, uint8_t defaultNameSize, uint8_t defaultIdx);
#define strcat_modelname(dest, idx) strcat_zchar(dest, modelHeaders[idx].name, LEN_MODEL_NAME, STR_MODEL, PSIZE(TR_MODEL), idx+1)
#define strcat_phasename(dest, idx) strcat_zchar(dest, g_model.phaseData[idx].name, LEN_FP_NAME, NULL, 0, 0)
#define strcat_mixername(dest, idx) strcat_zchar(dest, g_model.mixData[idx].name, LEN_EXPOMIX_NAME, NULL, 0, 0)
#define ZLEN(s) zlen(s, sizeof(s))
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
  THLD_CURRENT,
  THLD_CONSUMPTION,
  THLD_MAX,
};
extern uint8_t barsThresholds[THLD_MAX];
#endif

#if defined(FRSKY)
  uint8_t maxTelemValue(uint8_t channel);
#else
  #define maxTelemValue(channel) 255
#endif

getvalue_t convertTelemValue(uint8_t channel, uint8_t value);
getvalue_t convertCswTelemValue(CustomSwData * cs);

#if defined(FRSKY) || defined(CPUARM)
lcdint_t applyChannelRatio(uint8_t channel, lcdint_t val);
#endif

#if defined(FRSKY)
NOINLINE uint8_t getRssiAlarmValue(uint8_t alarm);

extern const pm_uint8_t bchunit_ar[];

#if defined(CPUARM)
  #define FRSKY_MULTIPLIER_MAX 5
#else
  #define FRSKY_MULTIPLIER_MAX 3
#endif

#define IS_BARS_SCREEN(screenIndex) (g_model.frsky.screensType & (1<<(screenIndex)))
#endif

#define EARTH_RADIUSKM ((uint32_t)6371)
#define EARTH_RADIUS ((uint32_t)111194)

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
#elif defined(IMPERIAL)
  #define IS_IMPERIAL_ENABLE() (1)
#else
  #define IS_IMPERIAL_ENABLE() (0)
#endif

#if defined(PCBTARANIS)
  #define IS_USR_PROTO_FRSKY_HUB()   (1)
  #define IS_USR_PROTO_WS_HOW_HIGH() (0)
#else
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

#endif
