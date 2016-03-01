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

#ifndef _SDCARD_H_
#define _SDCARD_H_

#include "ff.h"

#define ROOT_PATH           "/"
#define MODELS_PATH         ROOT_PATH "MODELS"      // no trailing slash = important
#define RADIO_PATH          ROOT_PATH "RADIO"       // no trailing slash = important
#define LOGS_PATH           ROOT_PATH "LOGS"
#define SCREENSHOTS_PATH    ROOT_PATH "SCREENSHOTS"
#define SOUNDS_PATH         ROOT_PATH "SOUNDS/en"
#define SOUNDS_PATH_LNG_OFS (sizeof(SOUNDS_PATH)-3)
#define SYSTEM_SUBDIR       "SYSTEM"
#define BITMAPS_PATH        ROOT_PATH "BMP"
#define FIRMWARES_PATH      ROOT_PATH "FIRMWARES"
#define EEPROMS_PATH        ROOT_PATH "EEPROMS"
#define SCRIPTS_PATH        ROOT_PATH "SCRIPTS"
#define WIZARD_PATH         SCRIPTS_PATH "/WIZARD"
#define THEMES_PATH         ROOT_PATH "THEMES"
#define LAYOUTS_PATH        ROOT_PATH "LAYOUTS"
#define WIDGETS_PATH        ROOT_PATH "WIDGETS"
#define WIZARD_NAME         "wizard.lua"
#define TEMPLATES_PATH      SCRIPTS_PATH "/TEMPLATES"
#define SCRIPTS_MIXES_PATH  SCRIPTS_PATH "/MIXES"
#define SCRIPTS_FUNCS_PATH  SCRIPTS_PATH "/FUNCTIONS"
#define SCRIPTS_TELEM_PATH  SCRIPTS_PATH "/TELEMETRY"

#define MODELS_EXT          ".bin"
#define LOGS_EXT            ".csv"
#define SOUNDS_EXT          ".wav"
#define BITMAPS_EXT         ".bmp"
#define PNG_EXT             ".png"
#define JPG_EXT             ".jpg"
#define SCRIPTS_EXT         ".lua"
#define TEXT_EXT            ".txt"
#define FIRMWARE_EXT        ".bin"
#define EEPROM_EXT          ".bin"
#define SPORT_FIRMWARE_EXT  ".frk"

#define GET_FILENAME(filename, path, var, ext) \
  char filename[sizeof(path) + sizeof(var) + sizeof(ext)]; \
  memcpy(filename, path, sizeof(path) - 1); \
  filename[sizeof(path) - 1] = '/'; \
  memcpy(&filename[sizeof(path)], var, sizeof(var)); \
  strcat(&filename[sizeof(path)], ext)

extern FATFS g_FATFS_Obj;
extern FIL g_oLogFile;

extern uint8_t logDelay;
const pm_char *openLogs();
void writeHeader();
void closeLogs();
void writeLogs();

uint32_t sdGetNoSectors();
uint32_t sdGetSize();
uint32_t sdGetFreeSectors();
const char * sdCheckAndCreateDirectory(const char * path);

#if !defined(BOOT)
inline const pm_char * SDCARD_ERROR(FRESULT result)
{
  if (result == FR_NOT_READY)
    return STR_NO_SDCARD;
  else
    return STR_SDCARD_ERROR;
}
#endif

#define LEN_FILE_EXTENSION             4
template<class T>
T * getFileExtension(T * filename, int size=0)
{
  int len = strlen(filename);
  if (size != 0 && size < len) {
    len = size;
  }
  for (int i=len; i>=len-LEN_FILE_EXTENSION; --i) {
    if (filename[i] == '.') {
      return &filename[i];
    }
  }
  return NULL;
}

#if defined(PCBTARANIS)
  #define O9X_FOURCC 0x3378396F // o9x for Taranis
#elif defined(PCBSKY9X)
  #define O9X_FOURCC 0x3278396F // o9x for sky9x
#else
  #define O9X_FOURCC 0x3178396F // o9x for gruvin9x/MEGA2560
#endif

bool isFileAvailable(const char * filename);
int findNextFileIndex(char * filename, const char * directory);

const char * sdCopyFile(const char * src, const char * dest);
const char * sdCopyFile(const char * srcFilename, const char * srcDir, const char * destFilename, const char * destDir);

#define LIST_NONE_SD_FILE   1
bool sdListFiles(const char * path, const char * extension, const uint8_t maxlen, const char * selection, uint8_t flags=0);

#endif // _SDCARD_H_
