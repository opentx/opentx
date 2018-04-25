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

#ifndef _BOARD_GRUVIN9X_H_
#define _BOARD_GRUVIN9X_H_

#include "../common/avr/board_avr.h"

// Board driver
void boardInit(void);
#define boardOff()  pwrOff()

// Keys
#define KEYS_GPIO_REG_MENU         pinl
#define KEYS_GPIO_PIN_MENU         (1<<4)
#define KEYS_GPIO_REG_EXIT         pinl
#define KEYS_GPIO_PIN_EXIT         (1<<5)
#define KEYS_GPIO_REG_RIGHT        pinl
#define KEYS_GPIO_PIN_RIGHT        (1<<2)
#define KEYS_GPIO_REG_LEFT         pinl
#define KEYS_GPIO_PIN_LEFT         (1<<3)
#define KEYS_GPIO_REG_UP           pinl
#define KEYS_GPIO_PIN_UP           (1<<1)
#define KEYS_GPIO_REG_DOWN         pinl
#define KEYS_GPIO_PIN_DOWN         (1<<0)
#define IS_SHIFT_KEY(index)        (false)
#define IS_SHIFT_PRESSED()         (false)

// Trims
#define NUM_TRIMS                  4
#define NUM_TRIMS_KEYS             (NUM_TRIMS * 2)
#define TRIMS_GPIO_REG_LHL         pinj
#define TRIMS_GPIO_REG_LVD         pinj
#define TRIMS_GPIO_REG_RVU         pinj
#define TRIMS_GPIO_REG_RHL         pinj
#define TRIMS_GPIO_REG_LHR         pinj
#define TRIMS_GPIO_REG_LVU         pinj
#define TRIMS_GPIO_REG_RVD         pinj
#define TRIMS_GPIO_REG_RHR         pinj
#define TRIMS_GPIO_PIN_LHL         (1<<0)
#define TRIMS_GPIO_PIN_LVD         (1<<2)
#define TRIMS_GPIO_PIN_RVU         (1<<5)
#define TRIMS_GPIO_PIN_RHL         (1<<6)
#define TRIMS_GPIO_PIN_LHR         (1<<1)
#define TRIMS_GPIO_PIN_LVU         (1<<3)
#define TRIMS_GPIO_PIN_RVD         (1<<4)
#define TRIMS_GPIO_PIN_RHR         (1<<7)

#define TIMER_16KHZ_VECT           TIMER2_OVF_vect
#define COUNTER_16KHZ              TCNT2
#define TIMER_10MS_VECT            TIMER2_COMPA_vect
#define TIMER_10MS_COMPVAL         OCR2A
#define PAUSE_10MS_INTERRUPT()     TIMSK2 &= ~(1<<OCIE2A)
#define RESUME_10MS_INTERRUPT()    TIMSK2 |= (1<<OCIE2A)
#define PAUSE_PPMIN_INTERRUPT()    TIMSK3 &= ~(1<<ICIE3)
#define RESUME_PPMIN_INTERRUPT()   TIMSK3 |= (1<<ICIE3)

#define SLAVE_MODE()               (PING & (1<<INP_G_RF_POW))
#define JACK_PPM_OUT()             PORTG |= (1<<OUT_G_SIM_CTL)
#define JACK_PPM_IN()              PORTG &= ~(1<<OUT_G_SIM_CTL)

// SD driver
#define BLOCK_SIZE                     512 /* Block Size in Bytes */
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
#define OUT_C_LIGHT                    0
#define backlightEnable()              PORTC |=  (1<<OUT_C_LIGHT)
#define backlightDisable()             PORTC &= ~(1<<OUT_C_LIGHT)
#define isBacklightEnabled()           (PORTC & (1<<OUT_C_LIGHT))
#define BACKLIGHT_ENABLE()             backlightEnable()
#define BACKLIGHT_DISABLE()            backlightDisable()

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
#define TRIMS_PRESSED()            (~PINJ)
#define KEYS_PRESSED()             (~PINL) // used only for DBLKEYS code.

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
#define hapticOn()                 PORTD &= ~(1 << OUT_D_HAPTIC)
#define hapticOff()                PORTD |=  (1 << OUT_D_HAPTIC)

// Rotary encoder driver
#define REA_DOWN()                 (~PIND & 0x20)
#define REB_DOWN()                 (~PIND & 0x10)
#define ROTENC_DOWN()              (REA_DOWN() || REB_DOWN())

// Buzzer driver
#define buzzerOn()                 PORTE |=  (1 << OUT_E_BUZZER)
#define buzzerOff()                PORTE &= ~(1 << OUT_E_BUZZER)

// Battery driver
#define BATTERY_MIN                90  // 9V
#define BATTERY_MAX                120 // 12V
#define BATTERY_WARN               90  // 9V

// Analogs driver
#define NUM_MOUSE_ANALOGS          0
#define NUM_DUMMY_ANAS             0

#endif // _BOARD_GRUVIN9X_H_
