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
#include "opentx.h"
#include "diskio.h"

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

char * getFileIndex(char * filename, unsigned int & value)
{
  value = 0;
  char * pos = getFileExtension(filename);
  if (!pos || pos == filename)
    return NULL;
  int multiplier = 1;
  while (pos > filename) {
    pos--;
    char c = *pos;
    if (c >= '0' && c <= '9') {
      value += multiplier * (c - '0');
      multiplier *= 10;
    }
    else {
      return pos+1;
    }
  }
  return filename;
}

uint8_t getDigitsCount(unsigned int value)
{
  uint8_t count = 1;
  while (value >= 10) {
    value /= 10;
    ++count;
  }
  return count;
}

int findNextFileIndex(char * filename, uint8_t size, const char * directory)
{
  unsigned int index;
  char * indexPos = getFileIndex(filename, index);
  char extension[LEN_FILE_EXTENSION+1];
  strncpy(extension, getFileExtension(filename), sizeof(extension));
  while (1) {
    index++;
    if ((indexPos - filename) + getDigitsCount(index) + LEN_FILE_EXTENSION > size) {
      return 0;
    }
    char * pos = strAppendUnsigned(indexPos, index);
    strAppend(pos, extension);
    if (!isFileAvailable(filename, directory)) {
      return index;
    }
  }
}

bool isExtensionMatching(const char * extension, const char * pattern, uint8_t flags)
{
  if (flags & LIST_SD_FILE_EXT) {
    for (int i=0; pattern[i]!='\0'; i+=LEN_FILE_EXTENSION) {
      if (strncasecmp(extension, &pattern[i], LEN_FILE_EXTENSION) == 0) {
        return true;
      }
    }
    return false;
  }
  else {
    return strcasecmp(extension, pattern) == 0;
  }
}

bool sdListFiles(const char * path, const char * extension, const uint8_t maxlen, const char * selection, uint8_t flags)
{
  FILINFO fno;
  DIR dir;

#if defined(CPUARM)
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
    if (!(flags & LIST_SD_FILE_EXT)) {
      strcat(reusableBuffer.modelsel.menu_bss[0], extension);
    }
    if (f_stat(reusableBuffer.modelsel.menu_bss[0], &fno) != FR_OK || (fno.fattrib & AM_DIR)) {
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
  POPUP_MENU_SET_BSS_FLAG();

  FRESULT res = f_opendir(&dir, path);
  if (res == FR_OK) {

    if (flags & LIST_NONE_SD_FILE) {
      popupMenuNoItems++;
      if (selection) {
        lastpopupMenuOffset++;
      }
      else if (popupMenuOffset==0 || popupMenuOffset < lastpopupMenuOffset) {
        char * line = reusableBuffer.modelsel.menu_bss[0];
        memset(line, 0, MENU_LINE_LENGTH);
        strcpy(line, "---");
        popupMenuItems[0] = line;
      }
    }

    for (;;) {
      res = f_readdir(&dir, &fno);                   /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */

      uint8_t len = strlen(fno.fname);
      uint8_t maxlen_with_extension = (flags & LIST_SD_FILE_EXT) ? maxlen : maxlen+LEN_FILE_EXTENSION;
      if (len < LEN_FILE_EXTENSION+1 || len > maxlen_with_extension || !isExtensionMatching(fno.fname+len-LEN_FILE_EXTENSION, extension, flags) || (fno.fattrib & AM_DIR)) continue;

      popupMenuNoItems++;

      if (!(flags & LIST_SD_FILE_EXT)) {
        fno.fname[len-LEN_FILE_EXTENSION] = '\0';
      }

      if (popupMenuOffset == 0) {
        if (selection && strncasecmp(fno.fname, selection, maxlen) < 0) {
          lastpopupMenuOffset++;
        }
        else {
          for (uint8_t i=0; i<MENU_MAX_DISPLAY_LINES; i++) {
            char * line = reusableBuffer.modelsel.menu_bss[i];
            if (line[0] == '\0' || strcasecmp(fno.fname, line) < 0) {
              if (i < MENU_MAX_DISPLAY_LINES-1) memmove(reusableBuffer.modelsel.menu_bss[i+1], line, sizeof(reusableBuffer.modelsel.menu_bss[i]) * (MENU_MAX_DISPLAY_LINES-1-i));
              memset(line, 0, MENU_LINE_LENGTH);
              strcpy(line, fno.fname);
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
          char * line = reusableBuffer.modelsel.menu_bss[i];
          if (line[0] == '\0' || strcasecmp(fno.fname, line) > 0) {
            if (i > 0) memmove(reusableBuffer.modelsel.menu_bss[0], reusableBuffer.modelsel.menu_bss[1], sizeof(reusableBuffer.modelsel.menu_bss[i]) * i);
            memset(line, 0, MENU_LINE_LENGTH);
            strcpy(line, fno.fname);
            break;
          }
        }
        for (uint8_t i=0; i<min(popupMenuNoItems, (uint16_t)MENU_MAX_DISPLAY_LINES); i++) {
          popupMenuItems[i] = reusableBuffer.modelsel.menu_bss[i];
        }
      }
      else if (popupMenuOffset > lastpopupMenuOffset) {
        if (strcasecmp(fno.fname, reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-2]) > 0 && strcasecmp(fno.fname, reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-1]) < 0) {
          memset(reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-1], 0, MENU_LINE_LENGTH);
          strcpy(reusableBuffer.modelsel.menu_bss[MENU_MAX_DISPLAY_LINES-1], fno.fname);
        }
      }
      else {
        if (strcasecmp(fno.fname, reusableBuffer.modelsel.menu_bss[1]) < 0 && strcasecmp(fno.fname, reusableBuffer.modelsel.menu_bss[0]) > 0) {
          memset(reusableBuffer.modelsel.menu_bss[0], 0, MENU_LINE_LENGTH);
          strcpy(reusableBuffer.modelsel.menu_bss[0], fno.fname);
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

// returns true if current working dir is at the root level
bool isCwdAtRoot()
{
  char path[10];
  if (f_getcwd(path, sizeof(path)-1) == FR_OK) {
    return (strcasecmp("/", path) == 0);
  }
  return false;
}

/*
  Wrapper around the f_readdir() function which 
  also returns ".." entry for sub-dirs. (FatFS 0.12 does
  not return ".", ".." dirs anymore)
*/
FRESULT sdReadDir(DIR * dir, FILINFO * fno, bool & firstTime)
{
  FRESULT res;
  if (firstTime && !isCwdAtRoot()) {
    // fake parent directory entry
    strcpy(fno->fname, "..");
    fno->fattrib = AM_DIR;
    res = FR_OK;
  }
  else {
    res = f_readdir(dir, fno);                   /* Read a directory item */
  }
  firstTime = false;
  return res;
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
  return (sdGetNoSectors() * BLOCK_SIZE) / 1000000;
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
