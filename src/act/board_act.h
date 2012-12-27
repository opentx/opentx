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

#ifndef board_act_h
#define board_act_h

#include <stdio.h>
/* #include "stm32f2xx_rcc.h" */
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"

// Keys
#define PIN_BUTTON_PLUS                 GPIO_Pin_0      //SW3 PE.10
#define PIN_BUTTON_ENTER                GPIO_Pin_1      //SW2 PE.11
#define PIN_BUTTON_MINUS                GPIO_Pin_2      //SW1 PE.12
#define PIN_BUTTON_MENU                 GPIO_Pin_11     //SW4 PD.07
#define PIN_BUTTON_EXIT                 GPIO_Pin_12     //SW5 PD.03
#define PIN_BUTTON_PAGE                 GPIO_Pin_10     //SW6 PD.02

#include "x9d/audio_driver.h"

// TODO elsewhere
#if !defined(SIMU)
#define SD_IS_HC()                     (1)
#endif


extern "C" {
extern void init_SDcard();
extern void sdInit();
}

#define sdPoll10ms()

void usbMassStorage();

void setVolume( register uint8_t volume );
#define JACK_PPM_OUT()
#define JACK_PPM_IN()

void configure_pins( uint32_t pins, uint16_t config );
uint16_t getCurrent();

extern uint8_t temperature ;              // Raw temp reading
extern uint8_t maxTemperature ;           // Raw temp reading
uint8_t getTemperature();

extern int32_t Card_state;
extern volatile uint32_t Card_initialized;
#define SD_ST_DATA              9
#define SD_ST_MOUNTED           10

#define SD_GET_SIZE_MB()        (0)
#define SD_GET_BLOCKNR()        (0)
#define SD_GET_SPEED()          (0)

void soft_power_off();

#define strcpy_P strcpy
#define strcat_P strcat

extern uint32_t readKeys();
#define KEYS_PRESSED() (~readKeys())
#define DBLKEYS_PRESSED_RGT_LFT(i) ((in & (0x20 + 0x40)) == (0x20 + 0x40))
#define DBLKEYS_PRESSED_UP_DWN(i)  ((in & (0x10 + 0x08)) == (0x10 + 0x08))
#define DBLKEYS_PRESSED_RGT_UP(i)  ((in & (0x20 + 0x10)) == (0x20 + 0x10))
#define DBLKEYS_PRESSED_LFT_DWN(i) ((in & (0x40 + 0x08)) == (0x40 + 0x08))

extern uint16_t sessionTimer;

#define BOOTLOADER_REQUEST() (0/*usbPlugged()*/)

#define SLAVE_MODE() (0/*check_soft_power() == e_power_trainer*/)

#if !defined(SIMU)
#define wdt_disable()
#define wdt_enable(x)
#define wdt_reset()
#endif

uint32_t check_soft_power();

#define setBacklight(xx)
#define __BACKLIGHT_ON
#define __BACKLIGHT_OFF
#define IS_BACKLIGHT_ON() (1)

#endif
