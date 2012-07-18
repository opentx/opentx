/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
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

#ifndef open9x_h
#define open9x_h

#define MAJ_VERS 2
#define MIN_VERS 00

#include <inttypes.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

#if defined(PCBSTD)
#define WDT_RESET_STOCK() wdt_reset()
#else
#define WDT_RESET_STOCK()
#endif

#if defined(PCBARM)
#include "board_ersky9x.h"
#endif

#if defined(SIMU)
#include "simpgmspace.h"
#elif defined(PCBARM)
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
#define wdt_disable() WDT->WDT_MR = WDT_MR_WDDIS;
#define wdt_enable(x) WDT->WDT_MR = 0x3FFF2FFF
#define wdt_reset()   WDT->WDT_CR = 0xA5000001
extern void board_init();
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

#include "myeeprom.h"

#define PPM_CENTER 1500

#if defined(PCBARM)
#include "eeprom_arm.h"
#include "pulses_arm.h"
#else
#include "eeprom_avr.h"
#include "pulses_avr.h"
#endif

#if defined(DSM2)
#define IS_DSM2_PROTOCOL(protocol) (protocol==PROTO_DSM2)
#else
#define IS_DSM2_PROTOCOL(protocol) (0)
#endif

#if defined(PXX)
#define IS_PXX_PROTOCOL(protocol) (protocol==PROTO_PXX)
#else
#define IS_PXX_PROTOCOL(protocol) (0)
#endif

#include "lcd.h"
#include "menus.h"
#ifdef TEMPLATES
#include "templates.h"
#endif

#if !defined(SIMU)
#define assert(x)
#define printf printf_not_allowed
#endif

// G: The following comments relate to the original stock PCB only
//
//                  elev                        thr
//                   LV                         RV
//                 2 ^                        4 ^
//                   1                          2
//                   |     rudd                 |     aile
//              <----X--3-> LH             <----X--0-> RH
//              6    |    7                1    |    0
//                   |                          |
//                 3 v                        5 v
//
//PORTA  7      6       5       4       3       2       1       0
//       O      O       O       O       O       O       O       O
//       ------------------------ LCD_DAT -----------------------
//
//PORTB  7      6       5       4       3       2       1       0
//       O      i       i       i       i       i       i       O
//       light  KEY_LFT KEY_RGT KEY_UP  KEY_DWN KEY_EXT KEY_MEN  PPM
//
//PORTC  7      6       5       4       3       2       1       0
//       -      -       O       O       O       O       O       -
//       NC     NC    LCD_E   LCD_RNW  LCD_A0  LCD_RES LCD_CS1  NC
//
//PORTD  7      6       5       4       3       2       1       0
//       i      i       i       i       i       i       i       i
//     TRM_D_DWN _UP  TRM_C_DWN _UP   TRM_B_DWN _UP   TRM_A_DWN _UP
//
//PORTE  7      6       5       4       3       2       1       0
//       i      i       i       i       O       i       i       i
//     PPM_IN  ID2    Trainer  Gear   Buzzer   ElevDR  AileDR  THRCT
//
//PORTF  7      6       5       4       3       2       1       0
//       ai     ai      ai      ai      ai      ai      ai      ai
// ANA_ BAT   PITT_TRM HOV_THR HOV_PIT  STCK_LH STCK_RV STCK_LV STCK_RH
//                                      rud    thro   elev   aile
//PORTG  7      6       5       4       3       2       1       0
//       -      -       -       O       i               i       i
//                            SIM_CTL  ID1      Haptic      RF_POW RuddDR

#define PORTA_LCD_DAT  PORTA
#define PORTC_LCD_CTRL PORTC
#define OUT_C_LCD_E     5
#define OUT_C_LCD_RnW   4
#define OUT_C_LCD_A0    3
#define OUT_C_LCD_RES   2
#define OUT_C_LCD_CS1   1

#if defined (PCBV4)

#  define INP_P_SPARE6    7
#  define INP_P_SPARE5    6
#  define INP_P_KEY_EXT   5
#  define INP_P_KEY_MEN   4
#  define INP_P_KEY_LFT   3
#  define INP_P_KEY_RGT   2
#  define INP_P_KEY_UP    1
#  define INP_P_KEY_DWN   0

#  define INP_J_TRM_RH_UP   7
#  define INP_J_TRM_RH_DWN  6
#  define INP_J_TRM_RV_UP   5
#  define INP_J_TRM_RV_DWN  4
#  define INP_J_TRM_LV_UP   3
#  define INP_J_TRM_LV_DWN  2
#  define INP_J_TRM_LH_UP   1
#  define INP_J_TRM_LH_DWN  0

#  define INP_E_PPM_IN      7
#  define INP_E_ROT_ENC_1_B 6
#  define INP_E_ROT_ENC_1_A 5
#  define INP_E_USB_D_PLS   4
#  define OUT_E_BUZZER      3
#  define INP_E_USB_D_NEG   2
#  define INP_E_TELEM_RX    1
#  define OUT_E_TELEM_TX    0

#  define INP_D_HAPTIC         7
#  define INP_D_SPARE4         6
#  define INP_D_ROT_ENC_2_PUSH 5
#  define INP_D_ROT_ENC_1_PUSH 4
#  define OUT_D_ROT_ENC_2_B    3
#  define INP_D_ROT_ENC_2_A    2
#  define INP_D_I2C_SCL        1
#  define INP_D_I2C_SDA        0

#  define INP_G_Gear     5
#  define INP_G_ThrCt    2
#  define OUT_G_SIM_CTL  4 //1 : phone-jack=ppm_in
#  define INP_G_ID1      3
#  define INP_G_RF_POW   1
#  define INP_G_RuddDR   0

#  define INP_C_AileDR   7
#  define INP_C_ElevDR   6
#  define OUT_C_LIGHT    0

#  define OUT_B_Speaker  7
#  define OUT_B_PPM      6 // will be switched by TCNT1==OCR1B in hardware
#  define INP_B_Trainer  5
#  define INP_B_ID2      4

#ifdef SOMO
#  define OUT_H_14DCLK   4
#  define OUT_H_14DDATA  5
#  define OUT_H_14DBUSY  6
#endif

#else // boards prior to v4 ...

#define OUT_B_LIGHT   7
#define INP_B_KEY_LFT 6
#define INP_B_KEY_RGT 5
#define INP_B_KEY_UP  4
#define INP_B_KEY_DWN 3
#define INP_B_KEY_EXT 2
#define INP_B_KEY_MEN 1
#define OUT_B_PPM     0

#define INP_D_TRM_LH_UP   7
#define INP_D_TRM_LH_DWN  6
#define INP_D_TRM_RV_DWN  5
#define INP_D_TRM_RV_UP   4
#define INP_D_TRM_LV_DWN  3
#define INP_D_TRM_LV_UP   2
#define INP_D_TRM_RH_DWN  1
#define INP_D_TRM_RH_UP   0

#define INP_E_PPM_IN  7
#define INP_E_ID2     6
#define INP_E_Trainer 5
#define INP_E_Gear    4
#define OUT_E_BUZZER  3
#define INP_E_ElevDR  2
#define INP_E_AileDR  1
#define INP_E_ThrCt   0

#if defined(JETI) || defined(FRSKY) || defined(ARDUPILOT) || defined(NMEA) || defined(MAVLINK)
#undef INP_E_ThrCt
#undef INP_E_AileDR
#define INP_C_ThrCt   6
#define INP_C_AileDR  7
#endif

#define OUT_G_SIM_CTL  4 //1 : phone-jack=ppm_in
#define INP_G_ID1      3
#define INP_G_HAPTIC   2
#define INP_G_RF_POW   1
#define INP_G_RuddDR   0

#endif // defined (PCBV4)

#if defined(PCBARM)
#define SLAVE_MODE (check_soft_power() == e_power_trainer)
#else
#define SLAVE_MODE (PING & (1<<INP_G_RF_POW))
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

enum EnumKeys {
  KEY_MENU ,
  KEY_EXIT ,
  KEY_DOWN ,
  KEY_UP  ,
  KEY_RIGHT ,
  KEY_LEFT ,
  TRM_LH_DWN  ,
  TRM_LH_UP   ,
  TRM_LV_DWN  ,
  TRM_LV_UP   ,
  TRM_RV_DWN  ,
  TRM_RV_UP   ,
  TRM_RH_DWN  ,
  TRM_RH_UP   ,
#if defined(ROTARY_ENCODERS)
  BTN_REa,
  BTN_REb,
#endif
  //SW_NC     ,
  //SW_ON     ,
  SW_ThrCt  ,
  SW_RuddDR ,
  SW_ElevDR ,
  SW_ID0    ,
  SW_ID1    ,
  SW_ID2    ,
  SW_AileDR ,
  SW_Gear   ,
  SW_Trainer,
  NUM_KEYS = SW_ThrCt
};

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
//#define KSTATE_SHORT   96
#define KSTATE_START    97
#define KSTATE_PAUSE    98
#define KSTATE_KILLED   99
  uint8_t m_vals:FILTERBITS;   // key debounce?  4 = 40ms
  uint8_t m_dblcnt:2;
  uint8_t m_cnt;
  uint8_t m_state;
public:
  void input(bool val, EnumKeys enuk);
  bool state()       { return m_vals==FFVAL;                }
  void pauseEvents() { m_state = KSTATE_PAUSE;  m_cnt   = 0;}
  void killEvents()  { m_state = KSTATE_KILLED; m_dblcnt=0; }
  uint8_t getDbl()   { return m_dblcnt;                     }
};

extern Key keys[NUM_KEYS];

#define CURVE_BASE 7

#define SWASH_TYPE_120   1
#define SWASH_TYPE_120X  2
#define SWASH_TYPE_140   3
#define SWASH_TYPE_90    4
#define SWASH_TYPE_NUM   4

#define CS_OFF       0
#define CS_VPOS      1  //v>offset
#define CS_VNEG      2  //v<offset
#define CS_APOS      3  //|v|>offset
#define CS_ANEG      4  //|v|<offset
#define CS_AND       5
#define CS_OR        6
#define CS_XOR       7
#define CS_EQUAL     8
#define CS_NEQUAL    9
#define CS_GREATER   10
#define CS_LESS      11
#define CS_EGREATER  12
#define CS_ELESS     13
#define CS_MAXF      13  //max function

#define CS_VOFS       0
#define CS_VBOOL      1
#define CS_VCOMP      2
#define CS_STATE(x)   ((x)<CS_AND ? CS_VOFS : ((x)<CS_EQUAL ? CS_VBOOL : CS_VCOMP))

#define SW_BASE      SW_ThrCt
#define SW_BASE_DIAG SW_ThrCt

#define MAX_PSWITCH   (SW_Trainer-SW_ThrCt+1)  // 9 physical switches
#define MAX_SWITCH    (MAX_PSWITCH+NUM_CSW)
#define SWITCH_ON     (1+MAX_SWITCH)
#define SWITCH_OFF    (-SWITCH_ON)
#if defined(PCBARM)
#define MAX_DRSWITCH  (MAX_PSWITCH+NUM_CSW)  // 15(9+6) !switches + 0 + 15 switches: 5 bits needed
#else
#define MAX_DRSWITCH  (MAX_PSWITCH+NUM_CSW/2)  // 15(9+6) !switches + 0 + 15 switches: 5 bits needed
#endif

#define NUM_STICKS      4
#define NUM_POTS        3
#define NUM_CYC         3
#define CSW_PPM_BASE    (MIXSRC_3POS+NUM_CYC) // because srcRaw is shifted +1!
#define NUM_CAL_PPM     4
#define NUM_PPM         8
#define CSW_CHOUT_BASE  (CSW_PPM_BASE+NUM_PPM)

#if defined(FRSKY_HUB)
#define NUM_TELEMETRY      TELEM_CSW_MAX
#elif defined(WS_HOW_HIGH)
#define NUM_TELEMETRY      TELEM_ALT
#elif defined(FRSKY)
#define NUM_TELEMETRY      TELEM_RSSI_RX
#elif defined(MAVLINK)
// Number sw position
#define NUM_TELEMETRY      4
#define ROTARY_SW_CHANNEL "UP  DOWN"
// Channel number for rotary switch
//#define MIX_SW_ROLL_CHAN (CSW_CHOUT_BASE+NUM_CHNOUT+NUM_VIRTUAL) // GVA:Rotary switch
#define MIX_INC_ROTARY_SW (CSW_CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS+1)
#define MIX_DEC_ROTARY_SW (CSW_CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS+1)
#else
#define NUM_TELEMETRY      TELEM_TM2
#endif

#define NUM_XCHNRAW  (NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS+NUM_STICKS+1/*MAX*/+1/*ID3*/+NUM_CYC+NUM_PPM+NUM_CHNOUT)
#define NUM_XCHNCSW  (NUM_XCHNRAW+NUM_TELEMETRY)
#define NUM_XCHNMIX  (NUM_XCHNRAW+MAX_SWITCH)
#define NUM_XCHNPLAY (NUM_XCHNRAW+TELEM_DISPLAY_MAX)

#define DSW_THR  1
#define DSW_RUD  2
#define DSW_ELE  3
#define DSW_ID0  4
#define DSW_ID1  5
#define DSW_ID2  6
#define DSW_AIL  7
#define DSW_GEA  8
#define DSW_TRN  9
#define DSW_SW1  10
#define DSW_SW2  11
#define DSW_SW3  12
#define DSW_SW4  13
#define DSW_SW5  14
#define DSW_SW6  15

#define THRCHK_DEADBAND 16
#define SPLASH_TIMEOUT  (4*100)  // 4 seconds

#define TRM_BASE TRM_LH_DWN

#define _MSK_KEY_BREAK     0x20
#define _MSK_KEY_REPT      0x40
#define _MSK_KEY_LONG      0x80
#define IS_KEY_BREAK(key)  (((key)&0xf0)        ==  0x20)
#define IS_KEY_FIRST(key)  (((key)&0xf0)        ==  0x60)
#define EVT_KEY_BREAK(key) ((key)|_MSK_KEY_BREAK)
#define EVT_KEY_FIRST(key) ((key)|_MSK_KEY_REPT|0x20)
#define EVT_KEY_REPT(key)  ((key)|_MSK_KEY_REPT)
#define EVT_KEY_LONG(key)  ((key)|_MSK_KEY_LONG)
#define EVT_ENTRY          (0xff - _MSK_KEY_REPT)
#define EVT_ENTRY_UP       (0xfe - _MSK_KEY_REPT)
#define EVT_KEY_MASK       (0x0f)

#define HEART_TIMER_PULSES  1
#define HEART_TIMER10ms     2
extern uint8_t heartbeat;

#define MAX_ALERT_TIME   60

extern uint16_t inacCounter;

#if defined(PXX)
extern uint8_t pxxFlag;
#endif

#define PXX_SEND_RXNUM     0x01
#define PXX_SEND_FAILSAFE  0x02 // TODO where is it used?

#define ZCHAR_MAX (40 + LEN_SPECIAL_CHARS)

extern char idx2char(int8_t idx);

void clearKeyEvents();
void pauseEvents(uint8_t enuk);
void killEvents(uint8_t enuk);
#if defined(PCBSTD)
uint8_t getEvent();
#else
uint8_t getEvent(bool trim);
#endif
void putEvent(uint8_t evt);

uint8_t keyDown();

enum PowerState {
  e_power_on,
  e_power_trainer,
  e_power_usb,
  e_power_off
};

#if defined(PCBARM)
uint32_t keyState(EnumKeys enuk);
uint32_t check_soft_power();
#else
#if defined(PCBV4)
uint8_t check_soft_power();
#else
#define check_soft_power() (e_power_on)
#endif

bool keyState(EnumKeys enuk);
#endif
void readKeysAndTrims();

uint16_t evalChkSum();

extern void alert(const pm_char * t, const pm_char * s);
extern void message(const pm_char *title, const pm_char *s, const char *last);

void    perMain();
void    per10ms();

int16_t getValue(uint8_t i);
bool    getSwitch(int8_t swtch, bool nc);

extern uint8_t switches_states;
int8_t  getMovedSwitch();

#ifdef FLIGHT_PHASES
extern uint8_t getFlightPhase();
#else
#define getFlightPhase() 0
#endif

extern uint8_t getTrimFlightPhase(uint8_t phase, uint8_t idx);
extern int16_t getRawTrimValue(uint8_t phase, uint8_t idx);
extern int16_t getTrimValue(uint8_t phase, uint8_t idx);
extern void setTrimValue(uint8_t phase, uint8_t idx, int16_t trim);

extern uint8_t s_perout_flight_phase;

#if defined(ROTARY_ENCODERS)
int16_t getRotaryEncoder(uint8_t idx);
void incRotaryEncoder(uint8_t idx, int8_t inc);
inline bool navigationRotaryEncoder(uint8_t event)
{
  return g_eeGeneral.reNavigation == ((event & EVT_KEY_MASK) - BTN_REa + 1);
}
#endif

extern uint16_t s_timeCumTot;
extern uint16_t s_timeCumThr;  //gewichtete laufzeit in 1/16 sec
extern uint16_t s_timeCum16ThrP; //gewichtete laufzeit in 1/16 sec
extern uint8_t  s_timerState[2];
extern int16_t  s_timerVal[2];
extern uint8_t  s_timerVal_10ms[2];
extern int8_t safetyCh[NUM_CHNOUT];

extern uint8_t trimsCheckTimer;

#define TMR_OFF     0
#define TMR_RUNNING 1
#define TMR_BEEPING 2
#define TMR_STOPPED 3
void resetTimer(uint8_t idx);
void resetAll();

extern uint8_t g_tmr1Latency_max;
extern uint8_t g_tmr1Latency_min;
extern uint16_t g_timeMainMax;
#if defined(PCBV4)
extern uint8_t  g_timeMainLast;
#endif

#if defined(THRTRACE)
#define MAXTRACE 120
extern uint8_t s_traceBuf[MAXTRACE];
extern uint8_t s_traceWr;
extern int8_t s_traceCnt;
#endif

extern int8_t *s_trimPtr[NUM_STICKS];

#if defined(PCBARM)
static inline uint16_t getTmr2MHz() { return TC1->TC_CHANNEL[0].TC_CV; }
#else
uint16_t getTmr16KHz();
#endif

uint16_t stack_free();

#ifdef SPLASH
void doSplash();
#endif

void checkLowEEPROM();
void checkTHR();
void checkSwitches();
void checkAlarm();

#define ADC_VREF_TYPE 0x40 // AVCC with external capacitor at AREF pin

#define GETADC_SING = 0
#define GETADC_OSMP = 1
#define GETADC_FILT = 2

#if !defined(SIMU)
void getADC_single();
void getADC_osmp();
void getADC_filt();
#endif

extern uint8_t  s_eeDirtyMsk;

#define STORE_MODELVARS eeDirty(EE_MODEL)
#define STORE_GENERALVARS eeDirty(EE_GENERAL)

extern void backlightOn();

#if defined (PCBARM)
#define BACKLIGHT_ON    (PWM->PWM_CH_NUM[0].PWM_CDTY = g_eeGeneral.backlightBright)
#define BACKLIGHT_OFF   (PWM->PWM_CH_NUM[0].PWM_CDTY = 100)
#ifdef REVB
#define NUMBER_ANALOG   9
#else
#define NUMBER_ANALOG   8
extern uint16_t Analog_values[NUMBER_ANALOG] ;
void read_9_adc(void ) ;
#endif
#elif defined (PCBV4)
#define SPEAKER_ON   TCCR0A |=  (1 << COM0A0)
#define SPEAKER_OFF  TCCR0A &= ~(1 << COM0A0)
#define BACKLIGHT_ON  PORTC |=  (1 << OUT_C_LIGHT)
#define BACKLIGHT_OFF PORTC &= ~(1 << OUT_C_LIGHT)
#else
#define BACKLIGHT_ON  PORTB |=  (1 << OUT_B_LIGHT)
#define BACKLIGHT_OFF PORTB &= ~(1 << OUT_B_LIGHT)
#endif

#define BUZZER_ON     PORTE |=  (1 << OUT_E_BUZZER)
#define BUZZER_OFF    PORTE &= ~(1 << OUT_E_BUZZER)

#if defined(HAPTIC)
#if defined(PCBARM)
#define HAPTIC_OFF    hapticOff()
#elif defined(PCBV4)
#define HAPTIC_ON     PORTD &= ~(1 << INP_D_HAPTIC)
#define HAPTIC_OFF    PORTD |=  (1 << INP_D_HAPTIC)
#else
#define HAPTIC_ON     PORTG |=  (1 << INP_G_HAPTIC)
#define HAPTIC_OFF    PORTG &= ~(1 << INP_G_HAPTIC)
#endif
#else
#define HAPTIC_ON
#define HAPTIC_OFF
#endif

#define BITMASK(bit) (1<<(bit))

/// liefert Dimension eines Arrays
#define DIM(arr) (sizeof((arr))/sizeof((arr)[0]))

#ifndef FORCEINLINE
#define FORCEINLINE inline __attribute__ ((always_inline))
#endif

#ifndef NOINLINE
#define NOINLINE __attribute__ ((noinline))
#endif

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

/// Markiert einen EEPROM-Bereich als dirty. der Bereich wird dann in
/// eeCheck ins EEPROM zurueckgeschrieben.
void eeWriteBlockCmp(const void *i_pointer_ram, uint16_t i_pointer_eeprom, size_t size);
void eeDirty(uint8_t msk);
void eeCheck(bool immediately=false);
void eeReadAll();
bool eeModelExists(uint8_t id);
uint16_t eeLoadModelName(uint8_t id, char *name);
void eeLoadModel(uint8_t id);
void generalDefault();
void modelDefault(uint8_t id);
void resetProto();

#if defined(PCBARM)
inline int32_t calc100toRESX(register int8_t x)
{
  return ((uint32_t)x*655)>>6 ;
}

inline int16_t calc1000toRESX( register int32_t x)  // improve calc time by Pat MacKenzie
{
    register int32_t y = x>>5;
    x+=y;
    y=y>>2;
    x-=y;
    return x+(y>>2);
    //  return x + x/32 - x/128 + x/512;
}
#else
inline int16_t calc100toRESX(int8_t x)
{
  // return (int16_t)x*10 + x/4 - x/64;
  return ((x*41)>>2) - x/64;
}

extern int16_t calc1000toRESX(int16_t x);
#endif

extern volatile uint16_t g_tmr10ms;

#if defined(PCBARM)
// This doesn't need protection on this processor
#define get_tmr10ms() g_tmr10ms
#else
extern inline uint16_t get_tmr10ms()
{
  uint16_t time  ;
  cli();
  time = g_tmr10ms ;
  sei();
  return time ;
}
#endif

#define TMR_VAROFS  5

extern const char stamp1[];
extern const char stamp2[];
extern const char stamp3[];
extern const char stamp4[];

extern uint8_t            g_vbat100mV;
extern volatile uint16_t  g_tmr10ms;
extern volatile uint8_t   g_blinkTmr10ms;
extern uint8_t            g_beepCnt;
extern uint8_t            g_beepVal[5];

#include "translations.h"

extern uint8_t            ppmInState; //0=unsync 1..8= wait for value i-1
extern int16_t            g_ppmIns[8];
extern int16_t            ex_chans[NUM_CHNOUT]; // Outputs (before LIMITS) of the last perMain
extern int16_t            g_chans512[NUM_CHNOUT];
extern uint16_t           BandGap;

extern uint16_t expou(uint16_t x, uint16_t k);
extern int16_t expo(int16_t x, int16_t k);
extern int16_t intpol(int16_t, uint8_t);
extern int16_t applyCurve(int16_t, int8_t);
extern void applyExpos(int16_t *anas);

extern uint16_t anaIn(uint8_t chan);
extern int16_t calibratedStick[NUM_STICKS+NUM_POTS];

#define FLASH_DURATION 50

extern uint8_t  beepAgain;
extern uint16_t g_LightOffCounter;
extern uint8_t mixWarning;

extern PhaseData *phaseaddress(uint8_t idx);
extern ExpoData *expoaddress(uint8_t idx);
extern MixData *mixaddress(uint8_t idx);
extern LimitData *limitaddress(uint8_t idx);

extern void deleteExpoMix(uint8_t expo, uint8_t idx);

extern void incSubtrim(uint8_t idx, int16_t inc);
extern void instantTrim();
extern void moveTrimsToOffsets();

#if defined(PCBARM)
#define ACTIVE_EXPOS_TYPE uint32_t
#define ACTIVE_MIXES_TYPE uint64_t
#else
#define ACTIVE_EXPOS_TYPE uint16_t
#define ACTIVE_MIXES_TYPE uint32_t
#endif

#ifdef BOLD_FONT
extern ACTIVE_EXPOS_TYPE activeExpos;
extern ACTIVE_MIXES_TYPE activeMixes;
inline bool isExpoActive(uint8_t expo)
{
  return activeExpos & ((ACTIVE_EXPOS_TYPE)1 << expo);
}

inline bool isMixActive(uint8_t mix)
{
  return activeMixes & ((ACTIVE_MIXES_TYPE)1 << mix);
}
#else
#define isExpoActive(x) false
#define isMixActive(x) false
#endif

extern uint16_t activeFunctions;
inline bool isFunctionActive(uint8_t func)
{
  return activeFunctions & (1 << (func-FUNC_TRAINER));
}

#ifdef DISPLAY_USER_DATA
#define TELEM_SCREEN_BUFFER_SIZE 21
extern char userDataDisplayBuf[TELEM_SCREEN_BUFFER_SIZE]; // text buffer for frsky telem. user data experiments
#endif

#if defined(ROTARY_ENCODERS)
// Global rotary encoder registers -- 8-bit, 0-255
#if defined(PCBARM)
extern volatile uint32_t g_rotenc[ROTARY_ENCODERS];
#else
extern volatile uint8_t g_rotenc[ROTARY_ENCODERS];
#endif
#endif

#ifdef JETI
// Jeti-DUPLEX Telemetry
extern uint16_t jeti_keys;
#include "jeti.h"
#endif

#if defined (FRSKY)
// FrSky Telemetry
#include "frsky.h"
#endif

#ifdef ARDUPILOT
// ArduPilot Telemetry
#include "ardupilot.h"
#endif

#ifdef NMEA
// NMEA Telemetry
#include "nmea.h"
#endif

#ifdef MAVLINK
// Mavlink Telemetry
#include "rotarysw.h"
#include "mavlink.h"
#endif

// REPEAT uses 0x01 to 0x0f
#define PLAY_NOW                  0x10
#define PLAY_SOUND_VARIO          0x20

/* make sure the defines below always go in numeric order */
enum AUDIO_SOUNDS {
    AU_INACTIVITY,
    AU_TX_BATTERY_LOW,
    AU_ERROR,
    AU_KEYPAD_UP,
    AU_KEYPAD_DOWN,
    AU_MENUS,
    AU_TRIM_MOVE,
    AU_WARNING1,
    AU_WARNING2,
    AU_WARNING3,
    AU_TRIM_MIDDLE,
#if defined(PCBARM)
    AU_TRIM_END,
#endif
    AU_TADA,
    AU_POT_STICK_MIDDLE,
    AU_MIX_WARNING_1,
    AU_MIX_WARNING_2,
    AU_MIX_WARNING_3,
    AU_TIMER_LT3,
    AU_TIMER_10,
    AU_TIMER_20,
    AU_TIMER_30,
    AU_FRSKY_FIRST,
    AU_FRSKY_WARN1 = AU_FRSKY_FIRST,
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
};
#if defined(AUDIO)
#if defined(PCBARM)
#include "ersky9x/audio.h"
#else
#include "stock/audio.h"
#endif
#else
#include "beeper.h"
#endif

#if defined(HAPTIC)
#include "haptic.h"
#endif

#if defined(SDCARD)
#include "sdcard.h"
#endif

#if defined(SOMO)
#include "gruvin9x/somo14d.h"
#endif

#if defined(PCBV4)
#include "gruvin9x/rtc.h"
extern uint8_t g_ms100; // global to allow time set function to reset to zero
#endif

// Re-useable byte array to save having multiple buffers
#define SD_SCREEN_FILE_LENGTH (32)
union ReusableBuffer
{
    /* 128 bytes on stock */

#if !defined(PCBARM)
    uint8_t eefs_buffer[BLOCKS];           // used by EeFsck
#endif

    struct
    {
        char mainname[42]; // used by menuProcModelSelect and for SD card archive / restore
        char listnames[7][10];
        uint16_t listsizes[7];
        uint16_t eepromfree;
    } models;

    struct
    {
        int16_t midVals[7];
        int16_t loVals[7];
        int16_t hiVals[7];
    } calib;                               // used by menuProcDiagCalib

#if defined(SDCARD)
    struct
    {
        char lines[7][SD_SCREEN_FILE_LENGTH+1];
        uint8_t flags[7];
        uint32_t available;
        uint16_t offset;
        uint16_t count;
    } sd;
#endif
};

extern union ReusableBuffer reusableBuffer;

void checkFlashOnBeep();

#if defined(FRSKY) || defined(PCBARM)
void putsTelemetryValue(uint8_t x, uint8_t y, int16_t val, uint8_t unit, uint8_t att);
#endif

#endif // gruvin9x_h
/*eof*/
