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

#ifndef board_ersky9x_h
#define board_ersky9x_h

#include <stdio.h>

#if !defined(SIMU)
#ifdef __cplusplus
extern "C" {
#endif
#include <CoOS.h>
#ifdef __cplusplus
}
#endif
#else
#define OS_MutexID int
#define OS_FlagID int
#define OS_TID int
#define OS_TCID int
#define OS_STK char
#define CoSetFlag(...)
#define CoClearFlag(...)
#define CoSetTmrCnt(...)
#define CoEnterISR(...)
#define CoExitISR(...)
#define CoStartTmr(...)
#define CoWaitForSingleFlag(...) 0
#define CoEnterMutexSection(...)
#define CoLeaveMutexSection(...)
#define CoTickDelay(...)
#define CoCreateFlag(...) 0
#endif

#include "ersky9x/libraries/board/board.h"
#include "ersky9x/sound_driver.h"
#include "ersky9x/haptic_driver.h"
#include "ersky9x/debug.h"

#if defined(BLUETOOTH)
#include "ersky9x/bluetooth.h"
#endif

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

#define strcpy_P strcpy
#define strcat_P strcat

extern uint32_t read_keys();
#define KEYS_PRESSED() (~read_keys())
#define DBLKEYS_PRESSED_RGT_LFT(i) ((in & (0x20 + 0x40)) == (0x20 + 0x40))
#define DBLKEYS_PRESSED_UP_DWN(i)  ((in & (0x10 + 0x08)) == (0x10 + 0x08))
#define DBLKEYS_PRESSED_RGT_UP(i)  ((in & (0x20 + 0x10)) == (0x20 + 0x10))
#define DBLKEYS_PRESSED_LFT_DWN(i) ((in & (0x40 + 0x08)) == (0x40 + 0x08))

#define usbPlugged() (PIOC->PIO_PDSR & PIO_PC25)

// CooCox global variables

#define MIXER_PRIO          5
#define MENUS_PRIO          10
#define AUDIO_PRIO          7
#define BT_PRIO             15
#define DEBUG_PRIO          10


#define MIXER_STACK_SIZE    500
#define MENUS_STACK_SIZE    2000
#define AUDIO_STACK_SIZE    500
#define BT_STACK_SIZE       500
#define DEBUG_STACK_SIZE    500

extern OS_MutexID mixerMutex;
extern OS_TID mixerTaskId;
extern OS_TID menusTaskId;
extern OS_TID audioTaskId;

#if defined(BLUETOOTH)
extern OS_TID btTaskId;
#endif

#if defined(DEBUG)
extern OS_TID debugTaskId;
#endif

extern OS_TCID audioTimer;
extern OS_FlagID audioFlag;

extern OS_MutexID audioMutex;
extern OS_MutexID mixerMutex;

extern uint32_t Master_frequency ;

struct t_serial_tx
{
  uint8_t *buffer ;
  uint16_t size ;
  volatile uint16_t ready ;
};

#define SD_STATE_ERROR       -1
#define SD_STATE_STARTUP      0
#define SD_STATE_CONNECTED    1
#define SD_STATE_MOUNTED      3

extern volatile int8_t sdState;

#define CardIsConnected() ( (PIOB->PIO_PDSR & PIO_PB7) == 0 )
#define CardIsMounted()   (CardIsConnected() && sdState==SD_STATE_MOUNTED)

#include "debug.h"

#ifdef SIMU
#define CONVERT_PTR(x) ((uint32_t)(uint64_t)(x))
#else
#include "core_cm3.h"
#include "memories/sdmmc.h"
#define CONVERT_PTR(x) ((uint32_t)(x))
#endif

#if defined(REVB)
extern uint16_t Current_analogue;
extern uint16_t Current_max;
extern uint32_t Current_accumulator;
extern uint32_t Current_used;
extern uint16_t sessionTimer;
#endif

#endif
