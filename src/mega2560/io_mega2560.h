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

#ifndef _HAL_
#define _HAL_

/*
 * GPIOS
 */

// Keys
#define GPIO_BUTTON_MENU                GPIOL->IDR  // only one port : to simplify ? //
#define GPIO_BUTTON_EXIT                GPIOL->IDR
#define GPIO_BUTTON_RIGHT               GPIOL->IDR
#define GPIO_BUTTON_LEFT                GPIOL->IDR
#define GPIO_BUTTON_UP                  GPIOL->IDR
#define GPIO_BUTTON_DOWN                GPIOL->IDR
#define	PIN_BUTTON_MENU                 GPIO_Pin_4
#define PIN_BUTTON_EXIT                 GPIO_Pin_5
#define	PIN_BUTTON_RIGHT                GPIO_Pin_2
#define PIN_BUTTON_LEFT                 GPIO_Pin_3
#define PIN_BUTTON_UP                   GPIO_Pin_1
#define PIN_BUTTON_DOWN                 GPIO_Pin_0

// Trims
#define GPIO_TRIM_LH_L                  GPIOK->IDR  // only one port : to simplify ? //
#define GPIO_TRIM_LH_R                  GPIOK->IDR
#define GPIO_TRIM_LV_DN                 GPIOK->IDR
#define GPIO_TRIM_LV_UP                 GPIOK->IDR
#define GPIO_TRIM_RH_L                  GPIOK->IDR
#define GPIO_TRIM_RH_R                  GPIOK->IDR
#define GPIO_TRIM_RV_DN                 GPIOK->IDR
#define GPIO_TRIM_RV_UP                 GPIOK->IDR
#define PIN_TRIM_LH_L                   GPIO_Pin_0
#define PIN_TRIM_LH_R                   GPIO_Pin_1
#define PIN_TRIM_LV_DN                  GPIO_Pin_2
#define PIN_TRIM_LV_UP                  GPIO_Pin_3
#define	PIN_TRIM_RH_L                   GPIO_Pin_4
#define PIN_TRIM_RH_R                   GPIO_Pin_5
#define	PIN_TRIM_RV_DN                  GPIO_Pin_5
#define	PIN_TRIM_RV_UP                  GPIO_Pin_6

// Switchs
#define GPIO_PIN_SW_A                   GPIOC->IDR  // A, B, C... to rename to ID1/2... ? //
#define GPIO_PIN_SW_B                   GPIOC->IDR
#define GPIO_PIN_SW_C                   GPIOD->IDR
#define GPIO_PIN_SW_D                   GPIOG->IDR
#define GPIO_PIN_SW_E                   GPIOG->IDR
#define GPIO_PIN_SW_F                   GPIOG->IDR
#define GPIO_PIN_SW_G                   GPIOL->IDR
#define GPIO_PIN_SW_H                   GPIOL->IDR
#define PIN_SW_A                        GPIO_Pin_1
#define PIN_SW_B                        GPIO_Pin_0
#define PIN_SW_C                        GPIO_Pin_7
#define PIN_SW_D                        GPIO_Pin_2
#define PIN_SW_E                        GPIO_Pin_1
#define PIN_SW_F                        GPIO_Pin_0
#define PIN_SW_G                        GPIO_Pin_6
#define PIN_SW_H                        GPIO_Pin_7

// Rotary encoders
#define GPIO_PIN_ROT_ENC_1_A            GPIOH->IDR
#define GPIO_PIN_ROT_ENC_1_B            GPIOJ->IDR
#define GPIO_PIN_ROT_ENC_1_PUSH         GPIOJ->IDR
#define GPIO_PIN_ROT_ENC_2_A            GPIOD->IDR
#define GPIO_PIN_ROT_ENC_2_B            GPIOD->IDR
#define GPIO_PIN_ROT_ENC_2_PUSH         GPIOH->IDR
#define PIN_ROT_ENC_1_A                 GPIO_Pin_1
#define PIN_ROT_ENC_1_B                 GPIO_Pin_0
#define PIN_ROT_ENC_1_PUSH              GPIO_Pin_1
#define PIN_ROT_ENC_2_A                 GPIO_Pin_0
#define PIN_ROT_ENC_2_B                 GPIO_Pin_3
#define PIN_ROT_ENC_2_PUSH              GPIO_Pin_2

// ADC
#define GPIO_PIN_ADC                    GPIOK->IDR
#define PIN_STK_J1                      GPIO_Pin_0
#define PIN_STK_J2                      GPIO_Pin_1
#define PIN_STK_J3                      GPIO_Pin_2
#define PIN_STK_J4                      GPIO_Pin_3
#define PIN_POT_1                       GPIO_Pin_4
#define PIN_POT_2                       GPIO_Pin_5
#define PIN_POT_3                       GPIO_Pin_6
#define PIN_MVOLT                       GPIO_Pin_7

// Accessories
#define GPIO_PIN_PPM_IN                 GPIOB->IDR
#define GPIO_PIN_PPM_OUT                GPIOB->IDR
#define GPIO_PIN_SIM_CTL                GPIOB->IDR
#define GPIO_PIN_TELEM_RX               GPIOE->IDR
#define GPIO_PIN_TELEM_TX               GPIOE->IDR
#define GPIO_PIN_SPEAKER                GPIOE->IDR
#define GPIO_PIN_BUZZER                 GPIOB->IDR
#define GPIO_PIN_JACK_PRES              GPIOH->IDR
#define GPIO_PIN_HOLD_PWR               GPIOH->IDR
#define PIN_PPM_IN                      GPIO_Pin_7
#define PIN_PPM_OUT                     GPIO_Pin_6
#define PIN_SIM_CTL                     GPIO_Pin_5
#define PIN_TELEM_RX                    GPIO_Pin_0
#define PIN_TELEM_TX                    GPIO_Pin_1
#define PIN_SPEAKER                     GPIO_Pin_3
#define PIN_BUZZER                      GPIO_Pin_4
#define PIN_JACK_PRES                   GPIO_Pin_5
#define PIN_HOLD_PWR                    GPIO_Pin_4

// LCD
#define GPIO_LCD_DAT                    GPIOA->IDR
#define GPIO_LCD_CTRL                   GPIOC->IDR
#define PIN_LCD_E                       GPIO_Pin_7
#define PIN_LCD_RnW                     GPIO_Pin_6
#define PIN_LCD_A0                      GPIO_Pin_5
#define PIN_LCD_RES                     GPIO_Pin_4
#define PIN_LCD_CS1                     GPIO_Pin_3
#define PIN_LCD_LIGHT                   GPIO_Pin_2

// SD
#define GPIO_SD                         GPIOB->IDR
#define PIN_SD_MISO                     GPIO_Pin_3
#define PIN_SD_MOSI                     GPIO_Pin_2
#define PIN_SD_SCK                      GPIO_Pin_1
#define PIN_SD_CS                       GPIO_Pin_0
  #define SD_IS_HC()       (0)   // ?
  #define SD_GET_BLOCKNR() (0)   // ?
  #define SD_GET_SIZE_MB() (0)   // ?
  #define SD_GET_SPEED()   (0)   // ?

#endif