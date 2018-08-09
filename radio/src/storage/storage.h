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
  #define WRITE_DELAY_10MS 100
#elif defined(RAMBACKUP)
  #define WRITE_DELAY_10MS 1500 /* 15s */
#elif defined(PCBTARANIS)
  #define WRITE_DELAY_10MS 500
#elif defined(PCBSKY9X) && !defined(REV0)
  #define WRITE_DELAY_10MS 500
#else
  #define WRITE_DELAY_10MS 200
#endif

extern uint8_t   storageDirtyMsk;
extern tmr10ms_t storageDirtyTime10ms;
#define TIME_TO_WRITE()                (storageDirtyMsk && (tmr10ms_t)(get_tmr10ms() - storageDirtyTime10ms) >= (tmr10ms_t)WRITE_DELAY_10MS)

#if defined(RAMBACKUP)
extern uint8_t   rambackupDirtyMsk;
extern tmr10ms_t rambackupDirtyTime10ms;
#define TIME_TO_RAMBACKUP()            (rambackupDirtyMsk && (tmr10ms_t)(get_tmr10ms() - rambackupDirtyTime10ms) >= (tmr10ms_t)100)
#endif

//
// Generic storage interface
//
void storageEraseAll(bool warn);
//void storageFormat();
void storageReadAll();
void storageCheck(bool immediately);

//
// Generic storage functions (implemented in storage_common.cpp)
//
void storageDirty(uint8_t msk);
void storageFlushCurrentModel();

void preModelLoad();
void postModelLoad(bool alarms);

#if defined(EEPROM)
#include "eeprom_common.h"
#endif

#if defined(SDCARD_RAW) || defined(SDCARD_YAML)
#include "sdcard_common.h"
#endif

#if defined(RAMBACKUP)
#include "rambackup.h"
#endif

#endif // _STORAGE_H_
