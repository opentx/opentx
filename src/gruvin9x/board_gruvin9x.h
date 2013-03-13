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

#define GPIO_TRIM_LH_L         pinj
#define GPIO_TRIM_LV_DN        pinj
#define GPIO_TRIM_RV_UP        pinj
#define GPIO_TRIM_RH_L         pinj
#define GPIO_TRIM_LH_R         pinj
#define GPIO_TRIM_LV_UP        pinj
#define GPIO_TRIM_RV_DN        pinj
#define GPIO_TRIM_RH_R         pinj
#define PIN_TRIM_LH_L          (1<<INP_J_TRM_LH_DWN)
#define PIN_TRIM_LV_DN         (1<<INP_J_TRM_LV_DWN)
#define PIN_TRIM_RV_UP         (1<<INP_J_TRM_RV_UP)
#define PIN_TRIM_RH_L          (1<<INP_J_TRM_RH_DWN)
#define PIN_TRIM_LH_R          (1<<INP_J_TRM_LH_UP)
#define PIN_TRIM_LV_UP         (1<<INP_J_TRM_LV_UP)
#define PIN_TRIM_RV_DN         (1<<INP_J_TRM_RV_DWN)
#define PIN_TRIM_RH_R          (1<<INP_J_TRM_RH_UP)

#define TIMER_16KHZ_VECT TIMER2_OVF_vect
#define COUNTER_16KHZ TCNT2
#define TIMER_10MS_VECT  TIMER2_COMPA_vect
#define TIMER_10MS_COMPVAL OCR2A
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
bool sdMounted();
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


#  define INP_L_SPARE6    7
#  define INP_L_SPARE5    6
#  define INP_L_KEY_EXT   5
#  define INP_L_KEY_MEN   4
#  define INP_L_KEY_LFT   3
#  define INP_L_KEY_RGT   2
#  define INP_L_KEY_UP    1
#  define INP_L_KEY_DWN   0

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

#  define OUT_D_HAPTIC         7
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

// LCD driver
#define PORTA_LCD_DAT  PORTA
#define PORTC_LCD_CTRL PORTC
#define OUT_C_LCD_E     5
#define OUT_C_LCD_RnW   4
#define OUT_C_LCD_A0    3
#define OUT_C_LCD_RES   2
#define OUT_C_LCD_CS1   1

// Power driver
uint8_t pwrCheck();
#define UNEXPECTED_SHUTDOWN() ((mcusr & (1 << WDRF)) || g_eeGeneral.unexpectedShutdown)

#endif
