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

#include <stdint.h>
#include "diskio.h"
#include "opentx.h"

const char * sdCheckAndCreateDirectory(const char * path)
{
  DIR archiveFolder;

  FRESULT result = f_opendir(&archiveFolder, path);
  if (result != FR_OK) {
    if (result == FR_NO_PATH)
      result = f_mkdir(path);
    if (result != FR_OK)
      return SDCARD_ERROR(result);
  }
  else {
    f_closedir(&archiveFolder);
  }

  return NULL;
}

bool isFileAvailable(const char * path)
{
  return f_stat(path, 0) == FR_OK;
}

bool isFileAvailable(const char * filename, const char * directory)
{
  char path[256];
  char * pos = strAppend(path, directory);
  *pos = '/';
  strAppend(pos+1, filename);
  return isFileAvailable(path);
}

char * getFileExtension(char * filename, int size)
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

char * getFileIndex(char * filename, unsigned int & value)
{
  value = 0;
  char * pos = getFileExtension(filename);
  if (!pos || pos == filename)
    return NULL;
  while (pos > filename) {
    pos--;
    char c = *pos;
    if (c >= '0' && c <= '9') {
      value = (value * 10) + (c - '\0');
    }
    else {
      return pos+1;
    }
  }
  return filename;
}

int findNextFileIndex(char * filename, const char * directory)
{
  unsigned int index;
  char * indexPos = getFileIndex(filename, index);
  char extension[LEN_FILE_EXTENSION+1];
  strncpy(extension, getFileExtension(filename), sizeof(extension));
  do {
    char * pos = strAppendNumber(indexPos, ++index);
    strAppend(pos, extension);
    if (!isFileAvailable(filename, directory)) {
      return index;
    }
  } while (index < 99);
  return 0;
}

bool sdListFiles(const char * path, const char * extension, const uint8_t maxlen, const char * selection, uint8_t flags)
{
  FILINFO fno;
  DIR dir;
  char * fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
  TCHAR lfn[_MAX_LFN + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif

#if defined(PCBTARANIS)
  popupMenuOffsetType = MENU_OFFSET_EXTERNAL;
#endif
  static uint16_t lastpopupMenuOffset = 0;

#if defined(CPUARM)
  static uint8_t s_last_flags;

  if (selection) {
    s_last_flags = flags;
    memset(reusableBuffer.modelsel.menu_bss, 0, sizeof(reusableBuffer.modelsel.menu_bss));
    strcpy(reusableBuffer.modelsel.menu_bss[0], path);
    strcat(reusableBuffer.modelsel.menu_bss[0], "/");
    strncat(reusableBuffer.modelsel.menu_bss[0], selection, maxlen);
    strcat(reusableBuffer.modelsel.menu_bss[0], extension);
    if (f_stat(reusableBuffer.modelsel.menu_bss[0], &fno) != FR_OK) {
      selection = NULL;
    }
  }
  else {
    flags = s_last_flags;
  }
#endif

  if (popupMenuOffset == 0) {
    lastpopupMenuOffset = 0;
    memset(reusableBuffer.modelsel.menu_bss, 0, sizeof(reusableBuffer.modelsel.menu_bss));
  }
  else if (popupMenuOffset == popupMenuNoItems - MENU_MAX_DISPLAY_LINES) {
    lastpopupMenuOffset = 0xffff;
    memset(reusableBuffer.modelsel.menu_bss, 0, sizeof(reusableBuffer.modelsel.menu_bss));
  }
  else if (popupMenuOffset == lastpopupMenuOffset) {
    // should not happen, only there because of Murphy's law
    return true;
  }
  else if (popupMenuOffset > lastpopupMenuOffset) {
    memmove(reusableBuffer.modelsel.menu_bss[0], reusableBuffer.modelsel.menu_bss[1], (MENU_MAX_DISPLAY_LINES-1)*MENU_LINE_LENGTH);
    memset(reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-1], 0xff, MENU_LINE_LENGTH);
  }
  else {
    memmove(reusableBuffer.modelsel.menu_bss[1], reusableBuffer.modelsel.menu_bss[0], (MENU_MAX_DISPLAY_LINES-1)*MENU_LINE_LENGTH);
    memset(reusableBuffer.modelsel.menu_bss[0], 0, MENU_LINE_LENGTH);
  }

  popupMenuNoItems = 0;
  POPUP_MENU_ITEMS_FROM_BSS();

  FRESULT res = f_opendir(&dir, path);        /* Open the directory */
  if (res == FR_OK) {

    if (flags & LIST_NONE_SD_FILE) {
      popupMenuNoItems++;
      if (selection) {
        lastpopupMenuOffset++;
      }
      else if (popupMenuOffset==0 || popupMenuOffset < lastpopupMenuOffset) {
        char *line = reusableBuffer.modelsel.menu_bss[0];
        memset(line, 0, MENU_LINE_LENGTH);
        strcpy(line, "---");
        popupMenuItems[0] = line;
      }
    }

    for (;;) {
      res = f_readdir(&dir, &fno);                   /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */

#if _USE_LFN
      fn = *fno.lfname ? fno.lfname : fno.fname;
#else
      fn = fno.fname;
#endif

      uint8_t len = strlen(fn);
      if (len < 5 || len > maxlen+4 || strcasecmp(fn+len-4, extension) || (fno.fattrib & AM_DIR)) continue;

      popupMenuNoItems++;
      fn[len-4] = '\0';

      if (popupMenuOffset == 0) {
        if (selection && strncasecmp(fn, selection, maxlen) < 0) {
          lastpopupMenuOffset++;
        }
        else {
          for (uint8_t i=0; i<MENU_MAX_DISPLAY_LINES; i++) {
            char *line = reusableBuffer.modelsel.menu_bss[i];
            if (line[0] == '\0' || strcasecmp(fn, line) < 0) {
              if (i < MENU_MAX_DISPLAY_LINES-1) memmove(reusableBuffer.modelsel.menu_bss[i+1], line, sizeof(reusableBuffer.modelsel.menu_bss[i]) * (MENU_MAX_DISPLAY_LINES-1-i));
              memset(line, 0, MENU_LINE_LENGTH);
              strcpy(line, fn);
              break;
            }
          }
        }
        for (uint8_t i=0; i<min(popupMenuNoItems, (uint16_t)MENU_MAX_DISPLAY_LINES); i++) {
          popupMenuItems[i] = reusableBuffer.modelsel.menu_bss[i];
        }

      }
      else if (lastpopupMenuOffset == 0xffff) {
        for (int i=MENU_MAX_DISPLAY_LINES-1; i>=0; i--) {
          char *line = reusableBuffer.modelsel.menu_bss[i];
          if (line[0] == '\0' || strcasecmp(fn, line) > 0) {
            if (i > 0) memmove(reusableBuffer.modelsel.menu_bss[0], reusableBuffer.modelsel.menu_bss[1], sizeof(reusableBuffer.modelsel.menu_bss[i]) * i);
            memset(line, 0, MENU_LINE_LENGTH);
            strcpy(line, fn);
            break;
          }
        }
        for (uint8_t i=0; i<min(popupMenuNoItems, (uint16_t)MENU_MAX_DISPLAY_LINES); i++) {
          popupMenuItems[i] = reusableBuffer.modelsel.menu_bss[i];
        }
      }
      else if (popupMenuOffset > lastpopupMenuOffset) {
        if (strcasecmp(fn, reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-2]) > 0 && strcasecmp(fn, reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-1]) < 0) {
          memset(reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-1], 0, MENU_LINE_LENGTH);
          strcpy(reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-1], fn);
        }
      }
      else {
        if (strcasecmp(fn, reusableBuffer.modelsel.menu_bss[1]) < 0 && strcasecmp(fn, reusableBuffer.modelsel.menu_bss[0]) > 0) {
          memset(reusableBuffer.modelsel.menu_bss[0], 0, MENU_LINE_LENGTH);
          strcpy(reusableBuffer.modelsel.menu_bss[0], fn);
        }
      }
    }
  }

  if (popupMenuOffset > 0)
    lastpopupMenuOffset = popupMenuOffset;
  else
    popupMenuOffset = lastpopupMenuOffset;

  return popupMenuNoItems;
}

#if defined(CPUARM) && defined(SDCARD)
const char * sdCopyFile(const char * srcPath, const char * destPath)
{
  FIL srcFile;
  FIL destFile;
  char buf[256];
  UINT read = sizeof(buf);
  UINT written = sizeof(buf);

  FRESULT result = f_open(&srcFile, srcPath, FA_OPEN_EXISTING | FA_READ);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  result = f_open(&destFile, destPath, FA_CREATE_ALWAYS | FA_WRITE);
  if (result != FR_OK) {
    f_close(&srcFile);
    return SDCARD_ERROR(result);
  }

  while (result==FR_OK && read==sizeof(buf) && written==sizeof(buf)) {
    result = f_read(&srcFile, buf, sizeof(buf), &read);
    if (result == FR_OK) {
      result = f_write(&destFile, buf, read, &written);
    }
  }

  f_close(&destFile);
  f_close(&srcFile);

  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  return NULL;
}

const char * sdCopyFile(const char * srcFilename, const char * srcDir, const char * destFilename, const char * destDir)
{
  char srcPath[2*CLIPBOARD_PATH_LEN+1];
  char * tmp = strAppend(srcPath, srcDir, CLIPBOARD_PATH_LEN);
  *tmp++ = '/';
  strAppend(tmp, srcFilename, CLIPBOARD_PATH_LEN);

  char destPath[2*CLIPBOARD_PATH_LEN+1];
  tmp = strAppend(destPath, destDir, CLIPBOARD_PATH_LEN);
  *tmp++ = '/';
  strAppend(tmp, destFilename, CLIPBOARD_PATH_LEN);

  return sdCopyFile(srcPath, destPath);
}
#endif // defined(CPUARM) && defined(SDCARD)


#if !defined(SIMU) || defined(SIMU_DISKIO)
uint32_t sdGetNoSectors()
{
  static DWORD noSectors = 0;
  if (noSectors == 0 ) {
    disk_ioctl(0, GET_SECTOR_COUNT, &noSectors);
  }
  return noSectors;
}

uint32_t sdGetSize()
{
  return (sdGetNoSectors() * 512) / 1000000;
}

uint32_t sdGetFreeSectors()
{
  DWORD nofree;
  FATFS * fat;
  if (f_getfree("", &nofree, &fat) != FR_OK) {
    return 0;
  }
  return nofree * fat->csize;
}

#else  // #if !defined(SIMU) || defined(SIMU_DISKIO)

uint32_t sdGetNoSectors()
{
  return 0;
}

uint32_t sdGetSize()
{
  return 0;
}

uint32_t sdGetFreeSectors()
{
  return 10;
}

#endif  // #if !defined(SIMU) || defined(SIMU_DISKIO)
