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

#include "open9x.h"
#include "ff.h"

char g_logFilename[21]; // "/O9XLOGS/M00_000.CSV\0" max required length = 21
FATFS g_FATFS_Obj;
FIL g_oLogFile;
int8_t g_logState = 0; // 0=closed, >0=opened, <0=error
const pm_char * g_logError = NULL;

void initLogs()
{
  // Determine and set log file filename
  FRESULT result;

  // close any file left open. E.G. Changing models with log switch still on.
  if (g_oLogFile.fs) f_close(&g_oLogFile);
  g_logState = 0;

  strcpy_P(g_logFilename, PSTR(LOG_PATH "M00_000.CSV"));

  // Set log file model number
  uint8_t num = g_eeGeneral.currModel + 1;
  char *n = &g_logFilename[11]; // TODO constant
  *n = (char)((num % 10) + '0');
  *(--n) = (char)((num / 10) + '0');

  result = f_mount(0, &g_FATFS_Obj);
  if (result!=FR_OK)
  {
    g_logState = -result;
    g_logError = PSTR("SDCARD F/S ERROR");
    return;
  }

  // Loop, skipping over any existing log files ... _000, _001, etc. until we have a unique file name
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
      if (f_mkdir(LOG_PATH) != FR_OK)
      {
        g_logState = -result;
        g_logError = PSTR("Check " LOG_PATH " folder");
        return;
      }
    }
    else
    {
      g_logState = -result;
      if (result == FR_NOT_READY)
        g_logError = PSTR("NO SDCARD");
      else
        g_logError = PSTR("SDCARD ERROR");
      return;
    }
  }

  // g_logFilename should now be set appropriately.
}

void writeLogs()
{
  FRESULT result;

  if (isFunctionActive(FUNC_LOGS))
  {
    if (g_logState==0)
    {
      result = f_mount(0, &g_FATFS_Obj);
      if (result != FR_OK)
      {
        g_logState = -result;
        // TODO beepAgain = result - 1;
        AUDIO_WARNING2();
      }
      else
      {
        // create new log file using filename set up in initLogs()
        result = f_open(&g_oLogFile, g_logFilename, FA_OPEN_ALWAYS | FA_WRITE);
        if (result != FR_OK)
        {
          g_logState = -result;
          // TODO beepAgain = result - 1; // TODO FOR DEBUG -- count out error_number beeps
          AUDIO_KEYPAD_UP();
        }
        else
        {
          if (g_oLogFile.fsize == 0) {
            // if data type == Hub TODO
            f_puts("Buffer,RX,TX,A1,A2,Date,Time,Long,Lat,Course,Speed,Alt,BarAlt,"
                "Temp1,Temp2,RPM,Fuel,Volts,AccelX,AccelY,AccelZ,THR,RUD,ELE,IDL0,IDL1,IDL2,AIL,GEA,TRN\n",
                &g_oLogFile);
          }
          
          f_lseek(&g_oLogFile, g_oLogFile.fsize); // append
          g_logState = 1;
          AUDIO_WARNING2();
        }
      }
    }

    if (g_logState>0)
    {
// For now, append 'anything' as a test
      // if data type == Hub
      f_printf(&g_oLogFile, "%d,", frskyStreaming);
      f_printf(&g_oLogFile, "%d,", frskyRSSI[0].value);
      f_printf(&g_oLogFile, "%d,", frskyRSSI[1].value);
      f_printf(&g_oLogFile, "%d,", frskyTelemetry[0].value);
      f_printf(&g_oLogFile, "%d,", frskyTelemetry[1].value);
      f_printf(&g_oLogFile, "%4d-%02d-%02d,", frskyHubData.year+2000, frskyHubData.month, frskyHubData.day);
      f_printf(&g_oLogFile, "%02d:%02d:%02d,", frskyHubData.hour, frskyHubData.min, frskyHubData.sec);
      f_printf(&g_oLogFile, "%03d.%04d%c,", frskyHubData.gpsLongitude_bp, frskyHubData.gpsLongitude_ap,
          frskyHubData.gpsLongitudeEW ? frskyHubData.gpsLongitudeEW : '-');
      f_printf(&g_oLogFile, "%03d.%04d%c,", frskyHubData.gpsLatitude_bp, frskyHubData.gpsLatitude_ap, 
          frskyHubData.gpsLatitudeNS ? frskyHubData.gpsLatitudeNS : '-');
      f_printf(&g_oLogFile, "%03d.%d,", frskyHubData.gpsCourse_bp, frskyHubData.gpsCourse_ap);
      f_printf(&g_oLogFile, "%d.%d,", frskyHubData.gpsSpeed_bp, frskyHubData.gpsSpeed_ap);
      f_printf(&g_oLogFile, "%03d.%d,", frskyHubData.gpsAltitude_bp, frskyHubData.gpsAltitude_ap);
      f_printf(&g_oLogFile, "%d.%d,", frskyHubData.baroAltitude_bp, frskyHubData.baroAltitude_ap);
      f_printf(&g_oLogFile, "%d,", frskyHubData.temperature1);
      f_printf(&g_oLogFile, "%d,", frskyHubData.temperature2);
      f_printf(&g_oLogFile, "%d,", frskyHubData.rpm);
      f_printf(&g_oLogFile, "%d,", frskyHubData.fuelLevel);
      f_printf(&g_oLogFile, "%d,", frskyHubData.volts);
      f_printf(&g_oLogFile, "%d,", frskyHubData.accelX);
      f_printf(&g_oLogFile, "%d,", frskyHubData.accelY);
      f_printf(&g_oLogFile, "%d,", frskyHubData.accelZ);
      f_printf(&g_oLogFile, "%d,", keyState(SW_ThrCt));
      f_printf(&g_oLogFile, "%d,", keyState(SW_RuddDR));
      f_printf(&g_oLogFile, "%d,", keyState(SW_ElevDR));
      f_printf(&g_oLogFile, "%d,", keyState(SW_ID0));
      f_printf(&g_oLogFile, "%d,", keyState(SW_ID1));
      f_printf(&g_oLogFile, "%d,", keyState(SW_ID2));
      f_printf(&g_oLogFile, "%d,", keyState(SW_AileDR));
      f_printf(&g_oLogFile, "%d,", keyState(SW_Gear));
      f_printf(&g_oLogFile, "%d\n", keyState(SW_Trainer));

      // Don't close the log file here. We have 'soft off' available on the v4.1 board. Once
      // that is implemented, it can take care of closing the file, should the radio be
      // powered off before the FUNC SWITCH is turned off.
    }
  }
  else if (g_logState > 0)
  {
    f_close(&g_oLogFile);
    AUDIO_WARNING2();
    g_logState = 0;
  }
}



