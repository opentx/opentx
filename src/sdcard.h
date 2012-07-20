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
#include "FatFs/fattime.h"

#ifdef SIMU
#define MODELS_PATH  "."
#define LOGS_PATH    "."
#define SOUNDS_PATH  "."
#define SYSTEM_SOUNDS_PATH "."
#else
#define MODELS_PATH         "/9XMODELS"
#define LOGS_PATH           "/9XLOGS"   // no trailing slash = important
#define SOUNDS_PATH         "/9XSOUNDS" // no trailing slash = important
#define SYSTEM_SOUNDS_PATH  "/9XSOUNDS/SYSTEM" // no trailing slash = important
#endif

#define MODELS_EXT   ".bin"
#define LOGS_EXT     ".csv"
#define SOUNDS_EXT   ".wav"

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

extern void sdPoll10mS();

#endif

