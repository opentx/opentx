#ifndef board_gruvin9x_h
#define board_gruvin9x_h

#include "io_mega2560.h"

#define TIMER_16KHZ_VECT TIMER2_OVF_vect
#define COUNTER_16KHZ TCNT2
#define TIMER_10MS_VECT  TIMER2_COMPA_vect
#define TIMER_10MS_COMPVAL OCR2A
#define PAUSE_10MS_INTERRUPT() TIMSK2 &= ~(1<<OCIE2A)
#define RESUME_10MS_INTERRUPT() TIMSK2 |= (1<<OCIE2A)
#define PAUSE_PPMIN_INTERRUPT() TIMSK3 &= ~(1<<ICIE3)
#define RESUME_PPMIN_INTERRUPT() TIMSK3 |= (1<<ICIE3)
#define SLAVE_MODE() (PING & (1<<INP_G_RF_POW))

#define __BACKLIGHT_ON  PORTC |=  (1 << OUT_C_LIGHT)
#define __BACKLIGHT_OFF PORTC &= ~(1 << OUT_C_LIGHT)
#define IS_BACKLIGHT_ON() (PORTC & (1<<OUT_C_LIGHT))

// SD driver
#if !defined(SIMU)
bool sdMounted();
void sdMountPoll();
void sdPoll10ms();
#endif

#define SPEAKER_ON   TCCR0A |=  (1 << COM0A0)
#define SPEAKER_OFF  TCCR0A &= ~(1 << COM0A0)

#if defined(VOICE)
#  define OUT_H_14DRESET 3
#  define OUT_H_14DCLK   4
#  define OUT_H_14DDATA  5
#  define INP_H_14DBUSY  6
#endif

// Keys driver
#define KEYS_PRESSED() (~PINL) // used only for DBLKEYS code.
#define DBLKEYS_PRESSED_RGT_LFT(i) ((in & ((1<<INP_L_KEY_RGT) + (1<<INP_L_KEY_LFT))) == ((1<<INP_L_KEY_RGT) + (1<<INP_L_KEY_LFT)))
#define DBLKEYS_PRESSED_UP_DWN(i)  ((in & ((1<<INP_L_KEY_UP)  + (1<<INP_L_KEY_DWN))) == ((1<<INP_L_KEY_UP)  + (1<<INP_L_KEY_DWN)))
#define DBLKEYS_PRESSED_RGT_UP(i)  ((in & ((1<<INP_L_KEY_RGT) + (1<<INP_L_KEY_UP)))  == ((1<<INP_L_KEY_RGT) + (1<<INP_L_KEY_UP)))
#define DBLKEYS_PRESSED_LFT_DWN(i) ((in & ((1<<INP_L_KEY_LFT) + (1<<INP_L_KEY_DWN))) == ((1<<INP_L_KEY_LFT) + (1<<INP_L_KEY_DWN)))

// Power driver
uint8_t pwrCheck();
#define UNEXPECTED_SHUTDOWN() ((mcusr & (1 << WDRF)) || g_eeGeneral.unexpectedShutdown)

// USB fake driver
#define usbPlugged()    false

#endif
