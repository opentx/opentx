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

#ifndef _BOARD_MEGA2560_H_
#define _BOARD_MEGA2560_H_

#include "../common/avr/board_avr.h"

// Board driver
void boardInit(void);
#define boardOff()  pwrOff()

// Keys
#define KEYS_GPIO_REG_MENU        pinl
#define KEYS_GPIO_PIN_MENU        (1<<4)
#define KEYS_GPIO_REG_EXIT        pinl
#define KEYS_GPIO_PIN_EXIT        (1<<5)
#define KEYS_GPIO_REG_RIGHT       pinl
#define KEYS_GPIO_PIN_RIGHT       (1<<2)
#define KEYS_GPIO_REG_LEFT        pinl
#define KEYS_GPIO_PIN_LEFT        (1<<3)
#define KEYS_GPIO_REG_UP          pinl
#define KEYS_GPIO_PIN_UP          (1<<1)
#define KEYS_GPIO_REG_DOWN        pinl
#define KEYS_GPIO_PIN_DOWN        (1<<0)

#define IS_SHIFT_KEY(index)       (false)
#define IS_SHIFT_PRESSED()        (false)

// Trims
#define NUM_TRIMS                 4
#define NUM_TRIMS_KEYS            (NUM_TRIMS * 2)
#define TRIMS_GPIO_REG_LHL        pinf
#define TRIMS_GPIO_PIN_LHL        (1<<7)
#define TRIMS_GPIO_REG_LVD        pinf
#define TRIMS_GPIO_PIN_LVD        (1<<5)
#define TRIMS_GPIO_REG_RVU        pinf
#define TRIMS_GPIO_PIN_RVU        (1<<2)
#define TRIMS_GPIO_REG_RHL        pinf
#define TRIMS_GPIO_PIN_RHL        (1<<1)
#define TRIMS_GPIO_REG_LHR        pinf
#define TRIMS_GPIO_PIN_LHR        (1<<6)
#define TRIMS_GPIO_REG_LVU        pinf
#define TRIMS_GPIO_PIN_LVU        (1<<4)
#define TRIMS_GPIO_REG_RVD        pinf
#define TRIMS_GPIO_PIN_RVD        (1<<3)
#define TRIMS_GPIO_REG_RHR        pinf
#define TRIMS_GPIO_PIN_RHR        (1<<0)

#define TIMER_16KHZ_VECT          TIMER2_OVF_vect
#define COUNTER_16KHZ             TCNT2
#define TIMER_10MS_VECT           TIMER2_COMPA_vect
#define TIMER_10MS_COMPVAL        OCR2A
#define PAUSE_10MS_INTERRUPT()    TIMSK2 &= ~(1<<OCIE2A)
#define RESUME_10MS_INTERRUPT()   TIMSK2 |= (1<<OCIE2A)
#define PAUSE_PPMIN_INTERRUPT()   TIMSK3 &= ~(1<<ICIE3)
#define RESUME_PPMIN_INTERRUPT()  TIMSK3 |= (1<<ICIE3)

#define SLAVE_MODE()              ~PINH & (1<<INP_H_RF_Activated)
#define JACK_PPM_OUT()            PORTB &= ~(1<<OUT_B_SIM_CTL)
#define JACK_PPM_IN()             PORTB |= (1<<OUT_B_SIM_CTL)

// Backlight driver
#define backlightEnable()         PORTC |= (1<<OUT_C_LIGHT)
#define backlightDisable()        PORTC &= ~(1<<OUT_C_LIGHT)
#define isBacklightEnabled()      (PORTC & (1<<OUT_C_LIGHT))
#define BACKLIGHT_ENABLE()        backlightEnable()
#define BACKLIGHT_DISABLE()       backlightDisable()

// SD driver
#define BLOCK_SIZE                512 /* Block Size in Bytes */
#define sdDone()
#define SD_IS_HC()                (0)
#define SD_GET_SPEED()            (0)
#if !defined(SIMU)
bool sdMounted(void);
void sdMountPoll(void);
void sdPoll10ms(void);
#endif

// Switchs driver
#define INP_C_ID2                 1
#define INP_C_ID1                 0
#define INP_D_AileDR              7
#define INP_G_ThrCt               2
#define INP_G_Gear                1
#define INP_G_RuddDR              0
#define INP_L_ElevDR              6
#define INP_L_Trainer             7

// Servitudes driver
#define INP_E_PPM_IN              7    //directly used
#define INP_B_VoiceBuzy           7    //directly used
#define OUT_B_PPM                 6
#define OUT_B_SIM_CTL             5
#define I_O_B_UNUSED              4    //unused, was Buzzer
#define INP_D_I2C_SCL             1
#define INP_D_I2C_SDA             0
#define OUT_E_VoiceData           3
#define INP_E_TELEM_RX            1
#define OUT_E_TELEM_TX            0
#define OUT_G_VoiceClock          5
#define INP_H_RF_Activated        6
#define INP_H_DSC_Activated       5    //pwrCheck(), directly used
#define INP_H_Hold_Power          4    //pwrCheck(), directly used
#define OUT_H_SpeakerBuzzer       3
#define OUT_H_VoiceReset          1
#define OUT_H_HAPTIC              0

// Rotary encoders driver
#define INP_E_ROT_ENC_1_A         4
#define INP_E_ROT_ENC_1_B         5
#define INP_D_ROT_ENC_2_A         2
#define INP_D_ROT_ENC_2_B         3
#define INP_J_ROT_ENC_1_PUSH      0
#define INP_J_ROT_ENC_2_PUSH      1
#define REA_DOWN()                (~PINJ & (1<<INP_J_ROT_ENC_1_PUSH))
#define REB_DOWN()                (~PINJ & (1<<INP_J_ROT_ENC_2_PUSH))
#define ROTENC_DOWN()             (REA_DOWN() || REB_DOWN())
#define ROTENC_DIV2                 // rotary encoders resolution/2

// LCD driver
#define PORTA_LCD_DAT            PORTA
#define PORTC_LCD_CTRL           PORTC
#if defined(LCD_KS108)              // (For KS108 LCD only) MEGA R/W pin always at 0 state in Opentx then
#define OUT_C_LCD_CS2            6  // use this pin to control second KS108 (CS2)
#else                               // and connect KS108 R/W pin to ground via a 1k resistor
#define OUT_C_LCD_RnW            6
#endif
#define OUT_C_LCD_E              7
#define OUT_C_LCD_A0             5
#define OUT_C_LCD_RES            4
#define OUT_C_LCD_CS1            3
#define OUT_C_LIGHT              2

// DBLKeys driver
#define KEYS_PRESSED()            (~PINL)

// Power driver
uint8_t pwrCheck();
void pwrOff();
#if defined(PWRMANAGE)
  #define UNEXPECTED_SHUTDOWN()   ((mcusr & (1 << WDRF)) || g_eeGeneral.unexpectedShutdown)
#else
  #define UNEXPECTED_SHUTDOWN()   (mcusr & (1 << WDRF))
#endif

// USB fake driver
#define usbPlugged()              false

// Haptic driver
#define hapticOn()                PORTH |=  (1 << OUT_H_HAPTIC)
#define hapticOff()               PORTH &= ~(1 << OUT_H_HAPTIC)

// Buzzer driver
#define buzzerOn()                PORTH |=  (1 << OUT_H_SpeakerBuzzer)
#define buzzerOff()               PORTH &= ~(1 << OUT_H_SpeakerBuzzer)

// Speaker driver
#if defined(AUDIO)
#define speakerOn()               TCCR4A |=  (1 << COM4A0)
#define speakerOff()              TCCR4A &= ~(1 << COM4A0)
#endif

// Voice driver (WTV20)
#if defined(VOICE_WTV20)
#define WTV20_Data_on             PORTE |=  (1<<OUT_E_VoiceData)
#define WTV20_Data_off            PORTE &= ~(1<<OUT_E_VoiceData)
#define WTV20_BUSY                (PINB & (1<<INP_B_VoiceBuzy))
#define WTV20_Clock_on            PORTG |=  (1<<OUT_G_VoiceClock)
#define WTV20_Clock_off           PORTG &= ~(1<<OUT_G_VoiceClock)
#define WTV20_CLK                 (PING & (1<<OUT_G_VoiceClock))
#define WTV20_Reset_on            PORTH |=  (1<<OUT_H_VoiceReset)
#define WTV20_Reset_off           PORTH &= ~(1<<OUT_H_VoiceReset)
#endif

// Voice driver (JQ6500)
#if defined(VOICE_JQ6500)
#define JQ6500_Serial_on          PORTE |=  (1<<OUT_E_VoiceData)
#define JQ6500_Serial_off         PORTE &= ~(1<<OUT_E_VoiceData)
#define JQ6500_BUSY               (PINB & (1<<INP_B_VoiceBuzy))
#endif

// Battery driver
#define BATTERY_MIN                90  // 9V
#define BATTERY_MAX                120 // 12V
#define BATTERY_WARN               90  // 9V

// Analogs driver
#define NUM_MOUSE_ANALOGS          0
#define NUM_DUMMY_ANAS             0

#endif // _BOARD_MEGA2560_H_
