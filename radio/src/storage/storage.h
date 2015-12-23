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
