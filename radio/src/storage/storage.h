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

#ifndef _STORAGE_H_
#define _STORAGE_H_

#if defined(SIMU)
  #define WRITE_DELAY_10MS 200
#elif defined(PCBTARANIS)
  #define WRITE_DELAY_10MS 500
#elif defined(PCBSKY9X) && !defined(REV0)
  #define WRITE_DELAY_10MS 500
#elif defined(PCBGRUVIN9X) && !defined(REV0)
  #define WRITE_DELAY_10MS 500
#else
  #define WRITE_DELAY_10MS 200
#endif

extern uint8_t   storageDirtyMsk;
extern tmr10ms_t storageDirtyTime10ms;

#define TIME_TO_WRITE() (storageDirtyMsk && (tmr10ms_t)(get_tmr10ms() - storageDirtyTime10ms) >= (tmr10ms_t)WRITE_DELAY_10MS)

void storageEraseAll(bool warn);
void storageFormat();
void storageReadAll();
void storageDirty(uint8_t msk);
void storageCheck(bool immediately);

void preModelLoad();
void postModelLoad(bool newModel);

#if defined(EEPROM_RLC)
#include "eeprom_common.h"
#include "eeprom_rlc.h"
#elif defined(EEPROM)
#include "eeprom_common.h"
#include "eeprom_raw.h"
#elif defined(SDCARD)
#include "sdcard_raw.h"
#endif

#endif // _STORAGE_H_
