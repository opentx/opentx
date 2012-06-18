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

#include "../open9x.h"
#include "FatFs/ff.h"

char g_logFilename[21]; // "/O9XLOGS/M00_000.CSV\0" max required length = 21
FATFS g_FATFS_Obj; // TODO not here
FIL g_oLogFile;
const pm_char * g_logError = NULL;

const pm_char * openLogs()
{
  // Determine and set log file filename
  FRESULT result;
  DIR folder;
  char *buf = g_logFilename;

  // close any file left open. E.G. Changing models with log switch still on.
  if (g_oLogFile.fs) f_close(&g_oLogFile);

  result = f_mount(0, &g_FATFS_Obj);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  strcpy_P(buf, STR_LOGS_PATH);

  result = f_opendir(&folder, buf);
  if (result != FR_OK) {
    if (result == FR_NO_PATH)
      result = f_mkdir(buf);
    if (result != FR_OK)
      return SDCARD_ERROR(result);
  }

  buf[sizeof(LOGS_PATH)-1] = '/';
  memcpy(&buf[sizeof(LOGS_PATH)], g_model.name, sizeof(g_model.name));
  buf[sizeof(LOGS_PATH)+FILENAME_MAXLEN] = '\0';

  uint8_t i = sizeof(LOGS_PATH)+FILENAME_MAXLEN-1;
  uint8_t len = 0;
  while (i>sizeof(LOGS_PATH)-1) {
    if (!len && buf[i])
      len = i+1;
    if (len) {
      if (buf[i])
        buf[i] = idx2char(buf[i]);
      else
        buf[i] = '_';
    }
    i--;
  }

  if (len == 0) {
    uint8_t num = g_eeGeneral.currModel + 1;
    strcpy_P(&buf[sizeof(LOGS_PATH)], STR_MODEL);
    buf[sizeof(LOGS_PATH) + PSIZE(TR_MODEL)] = (char)((num / 10) + '0');
    buf[sizeof(LOGS_PATH) + PSIZE(TR_MODEL) + 1] = (char)((num % 10) + '0');
    len = sizeof(LOGS_PATH) + PSIZE(TR_MODEL) + 2;
  }

  strcpy_P(&buf[len], STR_LOGS_EXT);

  result = f_open(&g_oLogFile, buf, FA_OPEN_ALWAYS | FA_WRITE);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  if (g_oLogFile.fsize == 0) {
    f_puts("Date,Time,Buffer,RX,TX,A1,A2,", &g_oLogFile);
    if (g_model.frsky.usrProto == USR_PROTO_FRSKY_HUB)
      f_puts("GPS Date,GPS Time,Long,Lat,Course,GPS Speed,GPS Alt,Baro Alt,Temp1,Temp2,RPM,Fuel,Volts,AccelX,AccelY,AccelZ,", &g_oLogFile);
    f_puts("Rud,Ele,Thr,Ail,P1,P2,P3,THR,RUD,ELE,ID0,ID1,ID2,AIL,GEA,TRN\n", &g_oLogFile);
  }

  result = f_lseek(&g_oLogFile, g_oLogFile.fsize); // append
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  return NULL;
}

void closeLogs()
{
  f_close(&g_oLogFile);
}

void writeLogs()
{
  if (g_oLogFile.fs && isFunctionActive(FUNC_LOGS))
  {
      static struct gtm t;
      struct gtm *at = &t;
      filltm(&g_unixTime, &t);

      f_printf(&g_oLogFile, "%4d-%02d-%02d,", at->tm_year+1900, at->tm_mon+1, at->tm_mday);
      f_printf(&g_oLogFile, "%02d:%02d:%02d,", at->tm_hour, at->tm_min, at->tm_sec);

#ifdef FRSKY
      f_printf(&g_oLogFile, "%d,", frskyStreaming);
      f_printf(&g_oLogFile, "%d,", frskyRSSI[0].value);
      f_printf(&g_oLogFile, "%d,", frskyRSSI[1].value);
      f_printf(&g_oLogFile, "%d,", frskyTelemetry[0].value);
      f_printf(&g_oLogFile, "%d,", frskyTelemetry[1].value);
#endif
#ifdef FRSKY_HUB
      if (g_model.frsky.usrProto == USR_PROTO_FRSKY_HUB) {
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
      }
#endif
      for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++)
        f_printf(&g_oLogFile, "%d,", calibratedStick[i]);
      f_printf(&g_oLogFile, "%d,", keyState(SW_ThrCt));
      f_printf(&g_oLogFile, "%d,", keyState(SW_RuddDR));
      f_printf(&g_oLogFile, "%d,", keyState(SW_ElevDR));
      f_printf(&g_oLogFile, "%d,", keyState(SW_ID0));
      f_printf(&g_oLogFile, "%d,", keyState(SW_ID1));
      f_printf(&g_oLogFile, "%d,", keyState(SW_ID2));
      f_printf(&g_oLogFile, "%d,", keyState(SW_AileDR));
      f_printf(&g_oLogFile, "%d,", keyState(SW_Gear));
      f_printf(&g_oLogFile, "%d\n", keyState(SW_Trainer));

      // TODO Don't close the log file here. We have 'soft off' available on the v4.1 board. Once
      // that is implemented, it can take care of closing the file, should the radio be
      // powered off before the FUNC SWITCH is turned off.
  }
}



