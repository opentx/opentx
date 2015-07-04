/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
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

#include <stdint.h>
#include "diskio.h"
#include "ff.h"

#define LIST_NONE_SD_FILE  1

bool listSdFiles(const char *path, const char *extension, const uint8_t maxlen, const char *selection, uint8_t flags=0)
{
  FILINFO fno;
  DIR dir;
  char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
  TCHAR lfn[_MAX_LFN + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
#endif

#if defined(PCBTARANIS)
  s_menu_offset_type = MENU_OFFSET_EXTERNAL;
#endif
  static uint16_t s_last_menu_offset = 0;

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

  if (s_menu_offset == 0) {
    s_last_menu_offset = 0;
    memset(reusableBuffer.modelsel.menu_bss, 0, sizeof(reusableBuffer.modelsel.menu_bss));
  }
  else if (s_menu_offset == s_menu_count - MENU_MAX_DISPLAY_LINES) {
    s_last_menu_offset = 0xffff;
    memset(reusableBuffer.modelsel.menu_bss, 0, sizeof(reusableBuffer.modelsel.menu_bss));
  }
  else if (s_menu_offset == s_last_menu_offset) {
    // should not happen, only there because of Murphy's law
    return true;
  }
  else if (s_menu_offset > s_last_menu_offset) {
    memmove(reusableBuffer.modelsel.menu_bss[0], reusableBuffer.modelsel.menu_bss[1], (MENU_MAX_DISPLAY_LINES-1)*MENU_LINE_LENGTH);
    memset(reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-1], 0xff, MENU_LINE_LENGTH);
  }
  else {
    memmove(reusableBuffer.modelsel.menu_bss[1], reusableBuffer.modelsel.menu_bss[0], (MENU_MAX_DISPLAY_LINES-1)*MENU_LINE_LENGTH);
    memset(reusableBuffer.modelsel.menu_bss[0], 0, MENU_LINE_LENGTH);
  }

  s_menu_count = 0;
  s_menu_flags = BSS;

  FRESULT res = f_opendir(&dir, path);        /* Open the directory */
  if (res == FR_OK) {

    if (flags & LIST_NONE_SD_FILE) {
      s_menu_count++;
      if (selection) {
        s_last_menu_offset++;
      }
      else if (s_menu_offset==0 || s_menu_offset < s_last_menu_offset) {
        char *line = reusableBuffer.modelsel.menu_bss[0];
        memset(line, 0, MENU_LINE_LENGTH);
        strcpy(line, "---");
        s_menu[0] = line;
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

      s_menu_count++;
      fn[len-4] = '\0';

      if (s_menu_offset == 0) {
        if (selection && strncasecmp(fn, selection, maxlen) < 0) {
          s_last_menu_offset++;
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
        for (uint8_t i=0; i<min(s_menu_count, (uint16_t)MENU_MAX_DISPLAY_LINES); i++) {
          s_menu[i] = reusableBuffer.modelsel.menu_bss[i];
        }

      }
      else if (s_last_menu_offset == 0xffff) {
        for (int i=MENU_MAX_DISPLAY_LINES-1; i>=0; i--) {
          char *line = reusableBuffer.modelsel.menu_bss[i];
          if (line[0] == '\0' || strcasecmp(fn, line) > 0) {
            if (i > 0) memmove(reusableBuffer.modelsel.menu_bss[0], reusableBuffer.modelsel.menu_bss[1], sizeof(reusableBuffer.modelsel.menu_bss[i]) * i);
            memset(line, 0, MENU_LINE_LENGTH);
            strcpy(line, fn);
            break;
          }
        }
        for (uint8_t i=0; i<min(s_menu_count, (uint16_t)MENU_MAX_DISPLAY_LINES); i++) {
          s_menu[i] = reusableBuffer.modelsel.menu_bss[i];
        }
      }
      else if (s_menu_offset > s_last_menu_offset) {
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

  if (s_menu_offset > 0)
    s_last_menu_offset = s_menu_offset;
  else
    s_menu_offset = s_last_menu_offset;

  return s_menu_count;
}

#if defined(CPUARM) && defined(SDCARD)
const char *fileCopy(const char *filename, const char *srcDir, const char *destDir)
{
  FIL srcFile;
  FIL dstFile;
  char buf[256];
  UINT read = sizeof(buf);
  UINT written = sizeof(buf);

  char path[2*CLIPBOARD_PATH_LEN+1];
  char *tmp = strAppend(path, srcDir, CLIPBOARD_PATH_LEN);
  *tmp++ = '/';
  strAppend(tmp, filename, CLIPBOARD_PATH_LEN);

  FRESULT result = f_open(&srcFile, path, FA_OPEN_EXISTING | FA_READ);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  tmp = strAppend(path, destDir, CLIPBOARD_PATH_LEN);
  *tmp++ = '/';
  strAppend(tmp, filename, CLIPBOARD_PATH_LEN);

  result = f_open(&dstFile, path, FA_CREATE_ALWAYS | FA_WRITE);
  if (result != FR_OK) {
    f_close(&srcFile);
    return SDCARD_ERROR(result);
  }

  while (result==FR_OK && read==sizeof(buf) && written==sizeof(buf)) {
    result = f_read(&srcFile, buf, sizeof(buf), &read);
    if (result == FR_OK) {
      result = f_write(&dstFile, buf, read, &written);
    }
  }

  f_close(&dstFile);
  f_close(&srcFile);

  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  return NULL;
}
#endif // #if defined(PCBTARANIS)


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
