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

#include "ff.h"
#if defined(PCBV4)
// TODO because this file is used for ersky9x
#include "rtc.h"
#endif
#include "gtime.h"

#define LOGS_PATH    "/9XLOGS"   // no trailing slash = important
#define MODELS_PATH  "/9XMODELS"
#define MODELS_EXT   ".bin"

extern char g_logFilename[21]; // "/9XLOGS/M00_000.CSV\0" max required length = 21
extern int8_t g_logState; // 0=closed, >0 = opened, <0 is error
extern FATFS g_FATFS_Obj;
extern FIL g_oLogFile;

extern void initLogs();
extern void writeLogs();

const pm_char *SDCARD_ERROR(FRESULT result)
{
  if (result == FR_NOT_READY)
    return STR_NO_SDCARD;
  else
    return STR_SDCARD_ERROR;
}

// MM/SD card Disk IO Support
extern void disk_timerproc(void);
extern uint8_t g_ms100; // defined in drivers.cpp

#endif

