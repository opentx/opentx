/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
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

#ifndef board_stock_h
#define board_stock_h

#define GPIO_BUTTON_MENU       pinb
#define GPIO_BUTTON_EXIT       pinb
#define GPIO_BUTTON_RIGHT      pinb
#define GPIO_BUTTON_LEFT       pinb
#define GPIO_BUTTON_UP         pinb
#define GPIO_BUTTON_DOWN       pinb
#define PIN_BUTTON_MENU        (1<<INP_B_KEY_MEN)
#define PIN_BUTTON_EXIT        (1<<INP_B_KEY_EXT)
#define PIN_BUTTON_UP          (1<<INP_B_KEY_UP)
#define PIN_BUTTON_DOWN        (1<<INP_B_KEY_DWN)
#define PIN_BUTTON_RIGHT       (1<<INP_B_KEY_RGT)
#define PIN_BUTTON_LEFT        (1<<INP_B_KEY_LFT)

#define GPIO_TRIM_LH_L         pind
#define GPIO_TRIM_LV_DN        pind
#define GPIO_TRIM_RV_UP        pind
#define GPIO_TRIM_RH_L         pind
#define GPIO_TRIM_LH_R         pind
#define GPIO_TRIM_LV_UP        pind
#define GPIO_TRIM_RV_DN        pind
#define GPIO_TRIM_RH_R         pind
#define PIN_TRIM_LH_L          (1<<INP_D_TRM_LH_DWN)
#define PIN_TRIM_LV_DN         (1<<INP_D_TRM_LV_DWN)
#define PIN_TRIM_RV_UP         (1<<INP_D_TRM_RV_UP)
#define PIN_TRIM_RH_L          (1<<INP_D_TRM_RH_DWN)
#define PIN_TRIM_LH_R          (1<<INP_D_TRM_LH_UP)
#define PIN_TRIM_LV_UP         (1<<INP_D_TRM_LV_UP)
#define PIN_TRIM_RV_DN         (1<<INP_D_TRM_RV_DWN)
#define PIN_TRIM_RH_R          (1<<INP_D_TRM_RH_UP)

#define TIMER_16KHZ_VECT TIMER0_OVF_vect
#define COUNTER_16KHZ TCNT0

#define TIMER_10MS_VECT  TIMER0_COMP_vect
#define PAUSE_10MS_INTERRUPT() TIMSK &= ~(1<<OCIE0)
#define RESUME_10MS_INTERRUPT() TIMSK |= (1<<OCIE0)

#define PAUSE_PULSES_INTERRUPT() TIMSK &= ~(1<<OCIE1A);
#define RESUME_PULSES_INTERRUPT() TIMSK |= (1<<OCIE1A)

#define PAUSE_PPMIN_INTERRUPT() ETIMSK &= ~(1<<TICIE3)
#define RESUME_PPMIN_INTERRUPT() ETIMSK |= (1<<TICIE3)

bool checkSlaveMode();
#define SLAVE_MODE() checkSlaveMode()
#define JACK_PPM_OUT() PORTG &= ~(1<<OUT_G_SIM_CTL)
#define JACK_PPM_IN() PORTG |=  (1<<OUT_G_SIM_CTL)

#if defined(PWM_BACKLIGHT)
void backlightFadeOn();
#define __BACKLIGHT_ON  backlightFadeOn()
void backlightFadeOff();
#define __BACKLIGHT_OFF backlightFadeOff()
bool getBackLightState();
#define IS_BACKLIGHT_ON() getBackLightState()
void fadeBacklight();
#else

#if defined(SP22)
#define __BACKLIGHT_ON  PORTB &= ~(1 << OUT_B_LIGHT)
#define __BACKLIGHT_OFF PORTB |=  (1 << OUT_B_LIGHT)
#define IS_BACKLIGHT_ON() (~PORTB & (1<<OUT_B_LIGHT))
#else
#define __BACKLIGHT_ON  PORTB |=  (1 << OUT_B_LIGHT)
#define __BACKLIGHT_OFF PORTB &= ~(1 << OUT_B_LIGHT)
#define IS_BACKLIGHT_ON() (PORTB & (1<<OUT_B_LIGHT))
#endif
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

#if defined(JETI) || defined(FRSKY) || defined(ARDUPILOT) || defined(NMEA) || defined(MAVLINK)
#define INP_C_ThrCt   6
#define INP_C_AileDR  7
#else
#define INP_E_ThrCt   0
#define INP_E_AileDR  1
#endif

#define OUT_G_SIM_CTL  4 //1 : phone-jack=ppm_in
#define INP_G_ID1      3
#define OUT_G_HAPTIC   2
#define INP_G_RF_POW   1
#define INP_G_RuddDR   0

#define KEYS_PRESSED() (~PINB)
#define DBLKEYS_PRESSED_RGT_LFT(i) ((in & ((1<<INP_B_KEY_RGT) + (1<<INP_B_KEY_LFT))) == ((1<<INP_B_KEY_RGT) + (1<<INP_B_KEY_LFT)))
#define DBLKEYS_PRESSED_UP_DWN(i)  ((in & ((1<<INP_B_KEY_UP)  + (1<<INP_B_KEY_DWN))) == ((1<<INP_B_KEY_UP)  + (1<<INP_B_KEY_DWN)))
#define DBLKEYS_PRESSED_RGT_UP(i)  ((in & ((1<<INP_B_KEY_RGT) + (1<<INP_B_KEY_UP)))  == ((1<<INP_B_KEY_RGT) + (1<<INP_B_KEY_UP)))
#define DBLKEYS_PRESSED_LFT_DWN(i) ((in & ((1<<INP_B_KEY_LFT) + (1<<INP_B_KEY_DWN))) == ((1<<INP_B_KEY_LFT) + (1<<INP_B_KEY_DWN)))

#define PORTA_LCD_DAT  PORTA
#define PORTC_LCD_CTRL PORTC
#define OUT_C_LCD_E     5
#define OUT_C_LCD_RnW   4
#define OUT_C_LCD_A0    3
#define OUT_C_LCD_RES   2
#define OUT_C_LCD_CS1   1

#if defined(ROTARY_ENCODER_NAVIGATION)
extern uint8_t RotEncoder ;
void rotencPoll();
#endif

#endif
