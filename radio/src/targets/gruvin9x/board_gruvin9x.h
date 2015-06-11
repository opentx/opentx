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

#ifndef board_gruvin9x_h
#define board_gruvin9x_h

#include "../common_avr/board_avr.h"

#define KEYS_GPIO_REG_MENU         pinl
#define KEYS_GPIO_REG_EXIT         pinl
#define KEYS_GPIO_REG_RIGHT        pinl
#define KEYS_GPIO_REG_LEFT         pinl
#define KEYS_GPIO_REG_UP           pinl
#define KEYS_GPIO_REG_DOWN         pinl
#define KEYS_GPIO_PIN_MENU          (1<<INP_L_KEY_MEN)
#define KEYS_GPIO_PIN_EXIT          (1<<INP_L_KEY_EXT)
#define KEYS_GPIO_PIN_UP            (1<<INP_L_KEY_UP)
#define KEYS_GPIO_PIN_DOWN          (1<<INP_L_KEY_DWN)
#define KEYS_GPIO_PIN_RIGHT         (1<<INP_L_KEY_RGT)
#define KEYS_GPIO_PIN_LEFT          (1<<INP_L_KEY_LFT)

#define TRIMS_GPIO_REG_LHL           pinj
#define TRIMS_GPIO_REG_LVD          pinj
#define TRIMS_GPIO_REG_RVU          pinj
#define TRIMS_GPIO_REG_RHL           pinj
#define TRIMS_GPIO_REG_LHR           pinj
#define TRIMS_GPIO_REG_LVU          pinj
#define TRIMS_GPIO_REG_RVD          pinj
#define TRIMS_GPIO_REG_RHR           pinj
#define TRIMS_GPIO_PIN_LHL            (1<<INP_J_TRM_LH_DWN)
#define TRIMS_GPIO_PIN_LVD           (1<<INP_J_TRM_LV_DWN)
#define TRIMS_GPIO_PIN_RVU           (1<<INP_J_TRM_RV_UP)
#define TRIMS_GPIO_PIN_RHL            (1<<INP_J_TRM_RH_DWN)
#define TRIMS_GPIO_PIN_LHR            (1<<INP_J_TRM_LH_UP)
#define TRIMS_GPIO_PIN_LVU           (1<<INP_J_TRM_LV_UP)
#define TRIMS_GPIO_PIN_RVD           (1<<INP_J_TRM_RV_DWN)
#define TRIMS_GPIO_PIN_RHR            (1<<INP_J_TRM_RH_UP)

#define TIMER_16KHZ_VECT         TIMER2_OVF_vect
#define COUNTER_16KHZ            TCNT2
#define TIMER_10MS_VECT          TIMER2_COMPA_vect
#define TIMER_10MS_COMPVAL       OCR2A
#define PAUSE_10MS_INTERRUPT()   TIMSK2 &= ~(1<<OCIE2A)
#define RESUME_10MS_INTERRUPT()  TIMSK2 |= (1<<OCIE2A)
#define PAUSE_PPMIN_INTERRUPT()  TIMSK3 &= ~(1<<ICIE3)
#define RESUME_PPMIN_INTERRUPT() TIMSK3 |= (1<<ICIE3)

#define SLAVE_MODE()             (PING & (1<<INP_G_RF_POW))
#define JACK_PPM_OUT()           PORTG |= (1<<OUT_G_SIM_CTL)
#define JACK_PPM_IN()            PORTG &= ~(1<<OUT_G_SIM_CTL)

// SD driver
#define sdDone()
#define SD_IS_HC()               (0)
#define SD_GET_SPEED()           (0)
#if !defined(SIMU)
bool sdMounted();
void sdMountPoll();
void sdPoll10ms();
#endif

#define speakerOn()                TCCR0A |=  (1 << COM0A0)
#define speakerOff()               TCCR0A &= ~(1 << COM0A0)

// Backlight driver
#define OUT_C_LIGHT                0
#define backlightEnable()          PORTC |=  (1<<OUT_C_LIGHT)
#define backlightDisable()         PORTC &= ~(1<<OUT_C_LIGHT)
#define isBacklightEnable()        (PORTC & (1<<OUT_C_LIGHT))

#define INP_L_SPARE6    7
#define INP_L_SPARE5    6
#define INP_L_KEY_EXT   5
#define INP_L_KEY_MEN   4
#define INP_L_KEY_LFT   3
#define INP_L_KEY_RGT   2
#define INP_L_KEY_UP    1
#define INP_L_KEY_DWN   0

#define INP_J_TRM_RH_UP   7
#define INP_J_TRM_RH_DWN  6
#define INP_J_TRM_RV_UP   5
#define INP_J_TRM_RV_DWN  4
#define INP_J_TRM_LV_UP   3
#define INP_J_TRM_LV_DWN  2
#define INP_J_TRM_LH_UP   1
#define INP_J_TRM_LH_DWN  0

#define INP_E_PPM_IN      7
#define INP_E_ROT_ENC_1_B 6
#define INP_E_ROT_ENC_1_A 5
#define INP_E_USB_D_PLS   4
#define OUT_E_BUZZER      3
#define INP_E_USB_D_NEG   2
#define INP_E_TELEM_RX    1
#define OUT_E_TELEM_TX    0

#define OUT_D_HAPTIC         7
#define INP_D_SPARE4         6
#define INP_D_ROT_ENC_2_PUSH 5
#define INP_D_ROT_ENC_1_PUSH 4
#define OUT_D_ROT_ENC_2_B    3
#define INP_D_ROT_ENC_2_A    2
#define INP_D_I2C_SCL        1
#define INP_D_I2C_SDA        0

#define INP_G_Gear     5
#define INP_G_ThrCt    2
#define OUT_G_SIM_CTL  4 //1 : phone-jack=ppm_in
#define INP_G_ID1      3
#define INP_G_RF_POW   1
#define INP_G_RuddDR   0

#define INP_C_AileDR   7
#define INP_C_ElevDR   6

#define OUT_B_Speaker  7
#define OUT_B_PPM      6 // will be switched by TCNT1==OCR1B in hardware
#define INP_B_Trainer  5
#define INP_B_ID2      4

#if defined(VOICE)
#define OUT_H_14DRESET 3
#define OUT_H_14DCLK   4
#define OUT_H_14DDATA  5
#define INP_H_14DBUSY  6
#endif

// Keys driver
#define TRIMS_PRESSED() (~PINJ)
#define KEYS_PRESSED()  (~PINL) // used only for DBLKEYS code.
#define DBLKEYS_PRESSED_RGT_LFT(i) ((in & ((1<<INP_L_KEY_RGT) + (1<<INP_L_KEY_LFT))) == ((1<<INP_L_KEY_RGT) + (1<<INP_L_KEY_LFT)))
#define DBLKEYS_PRESSED_UP_DWN(i)  ((in & ((1<<INP_L_KEY_UP)  + (1<<INP_L_KEY_DWN))) == ((1<<INP_L_KEY_UP)  + (1<<INP_L_KEY_DWN)))
#define DBLKEYS_PRESSED_RGT_UP(i)  ((in & ((1<<INP_L_KEY_RGT) + (1<<INP_L_KEY_UP)))  == ((1<<INP_L_KEY_RGT) + (1<<INP_L_KEY_UP)))
#define DBLKEYS_PRESSED_LFT_DWN(i) ((in & ((1<<INP_L_KEY_LFT) + (1<<INP_L_KEY_DWN))) == ((1<<INP_L_KEY_LFT) + (1<<INP_L_KEY_DWN)))

// LCD driver
#define PORTA_LCD_DAT              PORTA
#define PORTC_LCD_CTRL             PORTC
#define OUT_C_LCD_E                5
#define OUT_C_LCD_RnW              4
#define OUT_C_LCD_A0               3
#define OUT_C_LCD_RES              2
#define OUT_C_LCD_CS1              1

// Power driver
uint8_t pwrCheck();
void pwrOff();
#define UNEXPECTED_SHUTDOWN()      ((mcusr & (1 << WDRF)) || g_eeGeneral.unexpectedShutdown)

// USB fake driver
#define usbPlugged()               false

// Haptic driver
#define hapticOff()                // TODO hapticOn() cleaner ...
#define HAPTIC_ON()                PORTD &= ~(1 << OUT_D_HAPTIC)
#define HAPTIC_OFF()               PORTD |=  (1 << OUT_D_HAPTIC)

// Rotary encoder driver
#define REA_DOWN()                 (~PIND & 0x20)
#define REB_DOWN()                 (~PIND & 0x10)
#define ROTENC_DOWN()              (REA_DOWN() || REB_DOWN())

// Buzzer driver
#define buzzerOn()                 PORTE |=  (1 << OUT_E_BUZZER)
#define buzzerOff()                PORTE &= ~(1 << OUT_E_BUZZER)

// EEPROM driver
#if !defined(SIMU)
#define eepromReadBlock(a, b, c)   eeprom_read_block(a, (const void *)b, c)
#endif

#endif
