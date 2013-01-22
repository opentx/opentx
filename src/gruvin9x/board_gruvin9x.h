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

#ifndef board_gruvin9x_h
#define board_gruvin9x_h

#define GPIO_BUTTON_MENU       pinl
#define GPIO_BUTTON_EXIT       pinl
#define GPIO_BUTTON_RIGHT      pinl
#define GPIO_BUTTON_LEFT       pinl
#define GPIO_BUTTON_UP         pinl
#define GPIO_BUTTON_DOWN       pinl
#define PIN_BUTTON_MENU        (1<<INP_L_KEY_MEN)
#define PIN_BUTTON_EXIT        (1<<INP_L_KEY_EXT)
#define PIN_BUTTON_UP          (1<<INP_L_KEY_UP)
#define PIN_BUTTON_DOWN        (1<<INP_L_KEY_DWN)
#define PIN_BUTTON_RIGHT       (1<<INP_L_KEY_RGT)
#define PIN_BUTTON_LEFT        (1<<INP_L_KEY_LFT)

#define TIMER_16KHZ_VECT TIMER2_OVF_vect
#define COUNTER_16KHZ TCNT2
#define TIMER_10MS_VECT  TIMER2_COMPA_vect
#define PAUSE_10MS_INTERRUPT() TIMSK2 &= ~(1<<OCIE2A)
#define RESUME_10MS_INTERRUPT() TIMSK2 |= (1<<OCIE2A)
#define PAUSE_PULSES_INTERRUPT() TIMSK1 &= ~(1<<OCIE1A)
#define RESUME_PULSES_INTERRUPT() TIMSK1 |= (1<<OCIE1A)
#define PAUSE_PPMIN_INTERRUPT() TIMSK3 &= ~(1<<ICIE3)
#define RESUME_PPMIN_INTERRUPT() TIMSK3 |= (1<<ICIE3)

#define SLAVE_MODE() (PING & (1<<INP_G_RF_POW))
#define JACK_PPM_OUT() PORTG |= (1<<OUT_G_SIM_CTL)
#define JACK_PPM_IN() PORTG &= ~(1<<OUT_G_SIM_CTL)

// SD driver
#if !defined(SIMU)
void sdMountPoll();
void sdPoll10ms();
#define SD_IS_HC()       (0)
#define SD_GET_BLOCKNR() (0)
#define SD_GET_SIZE_MB() (0)
#define SD_GET_SPEED()   (0)
#endif

#define SPEAKER_ON   TCCR0A |=  (1 << COM0A0)
#define SPEAKER_OFF  TCCR0A &= ~(1 << COM0A0)
#define __BACKLIGHT_ON  PORTC |=  (1 << OUT_C_LIGHT)
#define __BACKLIGHT_OFF PORTC &= ~(1 << OUT_C_LIGHT)
#define IS_BACKLIGHT_ON() (PORTC & (1<<OUT_C_LIGHT))

#endif
