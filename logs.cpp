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

#include "open9x.h"
#include "ff.h"

// "/G9XLOGS/M00_000.TXT\0" max required length = 21
char g_logFilename[22];

// These global so we can close any open file from anywhere
FATFS FATFS_Obj;
FIL g_oLogFile;
int8_t g_logState = 0; // 0=closed, >0=opened, <0=error

void startLogs()
{
  // Determine and set log file filename
  FRESULT result;

  // close any file left open. E.G. Changing models with log switch still on.
  if (g_oLogFile.fs) f_close(&g_oLogFile);
  g_logState = 0;

  strcpy_P(g_logFilename, PSTR("/G9XLOGS/M00_000.TXT"));

  uint8_t num = g_eeGeneral.currModel + 1;
  char *n = &g_logFilename[11];
  *n = (char)((num % 10) + '0');
  *(--n) = (char)((num / 10) + '0');

  result = f_mount(0, &FATFS_Obj);
  if (result!=FR_OK)
  {
    strcpy_P(g_logFilename, PSTR("FILE SYSTEM ERROR"));
    g_logState = -result;
    return;
  }

  // Skip over any existing log files ... _000, _001, etc. (or find first gap in numbering)
  while (1)
  {
    result = f_open(&g_oLogFile, g_logFilename, FA_OPEN_EXISTING | FA_READ);
    if (result == FR_OK)
    {
      f_close(&g_oLogFile);

      // bump log file counter (file extension)
      n = &g_logFilename[15];
      if (++*n > '9')
      {
        *n='0';
        n--;
        if (++*n > '9')
        {
          *n='0';
          n--;
          if (++*n > '9')
          {
            *n='0';
            break; // Wow. We looped back around past 999 to 000! abort loop
          }
        }
      }
    }
    else if (result == FR_NO_FILE /*TODO check this code*/)
    {
      break;
    }
    else if (result == FR_NO_PATH)
    {
      if (f_mkdir("/G9XLOGS") != FR_OK)
      {
        strcpy_P(g_logFilename, PSTR("Check /G9XLOGS folder"));
        g_logState = -result;
        return;
      }
    }
    else
    {
      g_logState = -result;
      if (result == FR_NOT_READY)
        strcpy_P(g_logFilename, PSTR("DATA CARD NOT PRESENT"));
      else
        strcpy_P(g_logFilename, PSTR("DATA CARD ERROR"));
      return;
    }
  }

  // g_logFilename should now be set appropriately.
}

// TODO FORCEINLINE this function
void doLogs()
{
  FRESULT result;

  if (isFunctionActive(FUNC_LOGS))
  {
    if (g_logState==0)
    {
      result = f_mount(0, &FATFS_Obj);
      if (result != FR_OK)
      {
        g_logState = -result;
        // TODO beepAgain = result - 1;
        AUDIO_KEYPAD_UP();
      }
      else
      {
        // create new log file using filename set up in startLogs()
        result = f_open(&g_oLogFile, g_logFilename, FA_OPEN_ALWAYS | FA_WRITE);
        if (result != FR_OK)
        {
          g_logState = -result;
          // TODO beepAgain = result - 1;
          AUDIO_KEYPAD_UP();
        }
        else
        {
          f_lseek(&g_oLogFile, g_oLogFile.fsize); // append
          g_logState = 1;
          AUDIO_WARNING2();
        }
      }
    }

    if (g_logState>0)
    {
      // TODO here we write logs
    }
  }
  else if (g_logState > 0)
  {
    f_close(&g_oLogFile);
    AUDIO_WARNING2();
    g_logState = 0;
  }
}



