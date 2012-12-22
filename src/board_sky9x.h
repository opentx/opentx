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

#ifndef board_sky9x_h
#define board_sky9x_h

#include <stdio.h>
#include "sky9x/board.h"
#include "sky9x/audio_driver.h"
#include "sky9x/haptic_driver.h"
#include "sky9x/debug.h"

#if defined(BLUETOOTH)
#include "sky9x/bluetooth.h"
#endif

extern "C" {
extern void init_SDcard();
extern void sdInit();
}

#define sdPoll10ms()

void usbMassStorage();

#define PIN_ENABLE           0x001
#define PIN_PERIPHERAL       0x000
#define PIN_INPUT            0x002
#define PIN_OUTPUT           0x000
#define PIN_PULLUP           0x004
#define PIN_NO_PULLUP        0x000
#define PIN_PULLDOWN         0x008
#define PIN_NO_PULLDOWN      0x000
#define PIN_PERI_MASK_L      0x010
#define PIN_PERI_MASK_H      0x020
#define PIN_PER_A            0x000
#define PIN_PER_B            0x010
#define PIN_PER_C            0x020
#define PIN_PER_D            0x030
#define PIN_PORT_MASK        0x0C0
#define PIN_PORTA            0x000
#define PIN_PORTB            0x040
#define PIN_PORTC            0x080
#define PIN_LOW              0x000
#define PIN_HIGH             0x100

void configure_pins( uint32_t pins, uint16_t config );
uint16_t getCurrent();

extern uint8_t temperature ;              // Raw temp reading
extern uint8_t maxTemperature ;           // Raw temp reading
uint8_t getTemperature();

#define strcpy_P strcpy
#define strcat_P strcat

extern uint32_t readKeys();
#define KEYS_PRESSED() (~readKeys())
#define DBLKEYS_PRESSED_RGT_LFT(i) ((in & (0x20 + 0x40)) == (0x20 + 0x40))
#define DBLKEYS_PRESSED_UP_DWN(i)  ((in & (0x10 + 0x08)) == (0x10 + 0x08))
#define DBLKEYS_PRESSED_RGT_UP(i)  ((in & (0x20 + 0x10)) == (0x20 + 0x10))
#define DBLKEYS_PRESSED_LFT_DWN(i) ((in & (0x40 + 0x08)) == (0x40 + 0x08))

#if !defined(REVA)
extern uint16_t Current_analogue;
extern uint16_t Current_max;
extern uint32_t Current_accumulator;
extern uint32_t Current_used;
extern uint16_t sessionTimer;
#endif

void soft_power_off();

#if defined(REVC)
#define BOOTLOADER_REQUEST() (usbPlugged())
#else
#define BOOTLOADER_REQUEST() (check_soft_power() == e_power_usb)
#endif

#define SLAVE_MODE() (check_soft_power() == e_power_trainer)
#define JACK_PPM_OUT() PIOC->PIO_PDR = PIO_PC22
#define JACK_PPM_IN() PIOC->PIO_PER = PIO_PC22

void setSticksGain(uint8_t gains);

uint32_t spi_operation( register uint8_t *tx, register uint8_t *rx, register uint32_t count );

#if !defined(SIMU)
#define wdt_disable()
#define wdt_enable(x) WDT->WDT_MR = 0x3FFF217F;
#define wdt_reset()   WDT->WDT_CR = 0xA5000001
#endif

#define setBacklight(xx) PWM->PWM_CH_NUM[0].PWM_CDTYUPD = xx;

#endif
