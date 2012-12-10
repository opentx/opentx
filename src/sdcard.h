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

#ifndef sdcard_h
#define sdcard_h

#include "FatFs/ff.h"

#ifdef SIMU
#define ROOT_PATH    "."
#define MODELS_PATH  "."
#define LOGS_PATH    "."
#define SOUNDS_PATH  "."
#define SYSTEM_SOUNDS_PATH "."
#define BITMAPS_PATH  "."
#else
#define ROOT_PATH           "/"
#define MODELS_PATH         "/9XMODELS"
#define LOGS_PATH           "/9XLOGS"   // no trailing slash = important
#define SOUNDS_PATH         "/9XSOUNDS" // no trailing slash = important
#define SYSTEM_SOUNDS_PATH  "/9XSOUNDS/SYSTEM" // no trailing slash = important
#define BITMAPS_PATH        MODELS_PATH
#endif

#define MODELS_EXT   ".bin"
#define LOGS_EXT     ".csv"
#define SOUNDS_EXT   ".wav"
#define BITMAPS_EXT  ".bmp"

extern FATFS g_FATFS_Obj;

extern uint8_t logDelay;
extern const pm_char * openLogs();
extern void closeLogs();
extern void writeLogs();

inline const pm_char *SDCARD_ERROR(FRESULT result)
{
  if (result == FR_NOT_READY)
    return STR_NO_SDCARD;
  else
    return STR_SDCARD_ERROR;
}

#if defined(SIMU)
#define sdMountPoll()
#else
extern void sdMountPoll();
#endif

#if defined(CPUARM) && !(defined(SIMU))
extern "C" {
extern uint32_t sd_card_ready();
extern uint32_t sd_card_mounted();
}
#else
#define sd_card_ready() (true)
#define sd_card_mounted() (true)
#endif

#if defined(PCBX9D)
#define O9X_FOURCC 0x3378396F // o9x for X9D
#elif defined(PCBSKY9X)
#define O9X_FOURCC 0x3278396F // o9x for sky9x
#elif defined(PCBGRUVIN9X)
#define O9X_FOURCC 0x3178396F // o9x for gruvin9x
#endif

#if defined(PCBGRUVIN9X) || defined(SIMU)
#define SD_IS_HC() (0)
#define SD_GET_BLOCKNR() (0)
#define SD_GET_SIZE_MB() (0)
#define SD_GET_SPEED() (0)
#endif

#endif

