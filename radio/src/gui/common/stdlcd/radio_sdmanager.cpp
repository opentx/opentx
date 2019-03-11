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

#include "opentx.h"

#define REFRESH_FILES()        do { reusableBuffer.sdmanager.offset = 65535; menuVerticalPosition = 0; } while(0)
#define NODE_TYPE(fname)       fname[SD_SCREEN_FILE_LENGTH+1]
#define IS_DIRECTORY(fname)    ((bool)(!NODE_TYPE(fname)))
#define IS_FILE(fname)         ((bool)(NODE_TYPE(fname)))

void menuRadioSdManagerInfo(event_t event)
{
  SIMPLE_SUBMENU(STR_SD_INFO_TITLE, 1);

  lcdDrawTextAlignedLeft(2*FH, STR_SD_TYPE);
  lcdDrawText(10*FW, 2*FH, SD_IS_HC() ? STR_SDHC_CARD : STR_SD_CARD);

  lcdDrawTextAlignedLeft(3*FH, STR_SD_SIZE);
  lcdDrawNumber(10*FW, 3*FH, sdGetSize(), LEFT);
  lcdDrawChar(lcdLastRightPos, 3*FH, 'M');

  lcdDrawTextAlignedLeft(4*FH, STR_SD_SECTORS);
#if defined(SD_GET_FREE_BLOCKNR)
  lcdDrawNumber(10*FW, 4*FH,  SD_GET_FREE_BLOCKNR()/1000, LEFT);
  lcdDrawChar(lcdLastRightPos, 4*FH, '/');
  lcdDrawNumber(lcdLastRightPos+FW, 4*FH, sdGetNoSectors()/1000, LEFT);
#else
  lcdDrawNumber(10*FW, 4*FH, sdGetNoSectors()/1000, LEFT);
#endif
  lcdDrawChar(lcdLastRightPos, 4*FH, 'k');

  lcdDrawTextAlignedLeft(5*FH, STR_SD_SPEED);
  lcdDrawNumber(10*FW, 5*FH, SD_GET_SPEED()/1000, LEFT);
  lcdDrawText(lcdLastRightPos, 5*FH, "kb/s");
}

inline bool isFilenameGreater(bool isfile, const char * fn, const char * line)
{
  return (isfile && IS_DIRECTORY(line)) || (isfile==IS_FILE(line) && strcasecmp(fn, line) > 0);
}

inline bool isFilenameLower(bool isfile, const char * fn, const char * line)
{
  return (!isfile && IS_FILE(line)) || (isfile==IS_FILE(line) && strcasecmp(fn, line) < 0);
}

void getSelectionFullPath(char * lfn)
{
  f_getcwd(lfn, _MAX_LFN);
  strcat(lfn, PSTR("/"));
  strcat(lfn, reusableBuffer.sdmanager.lines[menuVerticalPosition - HEADER_LINE - menuVerticalOffset]);
}

void onSdManagerMenu(const char * result)
{
  TCHAR lfn[_MAX_LFN+1];

  // TODO possible buffer overflows here!

  uint8_t index = menuVerticalPosition - HEADER_LINE - menuVerticalOffset;
  char * line = reusableBuffer.sdmanager.lines[index];

  if (result == STR_SD_INFO) {
    pushMenu(menuRadioSdManagerInfo);
  }
  else if (result == STR_SD_FORMAT) {
    POPUP_CONFIRMATION(STR_CONFIRM_FORMAT);
  }
#if defined(CPUARM)
  else if (result == STR_COPY_FILE) {
    clipboard.type = CLIPBOARD_TYPE_SD_FILE;
    f_getcwd(clipboard.data.sd.directory, CLIPBOARD_PATH_LEN);
    strncpy(clipboard.data.sd.filename, line, CLIPBOARD_PATH_LEN-1);
  }
  else if (result == STR_PASTE) {
    f_getcwd(lfn, _MAX_LFN);
    // if destination is dir, copy into that dir
    if (IS_DIRECTORY(line)) {
      strcat(lfn, PSTR("/"));
      strcat(lfn, line);
    }
    if (strcmp(clipboard.data.sd.directory, lfn)) {  // prevent copying to the same directory
      POPUP_WARNING(sdCopyFile(clipboard.data.sd.filename, clipboard.data.sd.directory, clipboard.data.sd.filename, lfn));
      REFRESH_FILES();
    }
  }
  else if (result == STR_RENAME_FILE) {
    memcpy(reusableBuffer.sdmanager.originalName, line, sizeof(reusableBuffer.sdmanager.originalName));
    uint8_t fnlen = 0, extlen = 0;
    getFileExtension(line, 0, LEN_FILE_EXTENSION_MAX, &fnlen, &extlen);
    // write spaces to allow extending the length of a filename
    memset(line + fnlen - extlen, ' ', SD_SCREEN_FILE_LENGTH - fnlen + extlen);
    line[SD_SCREEN_FILE_LENGTH-extlen] = '\0';
    s_editMode = EDIT_MODIFY_STRING;
    editNameCursorPos = 0;
  }
#endif
  else if (result == STR_DELETE_FILE) {
    getSelectionFullPath(lfn);
    f_unlink(lfn);
    strncpy(statusLineMsg, line, 13);
    strcpy_P(statusLineMsg+min((uint8_t)strlen(statusLineMsg), (uint8_t)13), STR_REMOVED);
    showStatusLine();
    REFRESH_FILES();
  }
#if defined(CPUARM)
  else if (result == STR_PLAY_FILE) {
    getSelectionFullPath(lfn);
    audioQueue.stopAll();
    audioQueue.playFile(lfn, 0, ID_PLAY_FROM_SD_MANAGER);
  }
#endif
#if LCD_DEPTH > 1
  else if (result == STR_ASSIGN_BITMAP) {
    strAppendFilename(g_model.header.bitmap, line, sizeof(g_model.header.bitmap));
    memcpy(modelHeaders[g_eeGeneral.currModel].bitmap, g_model.header.bitmap, sizeof(g_model.header.bitmap));
    storageDirty(EE_MODEL);
  }
#endif
#if defined(CPUARM)
  else if (result == STR_VIEW_TEXT) {
    getSelectionFullPath(lfn);
    pushMenuTextView(lfn);
  }
#endif
#if defined(PCBTARANIS)
  else if (result == STR_FLASH_BOOTLOADER) {
    getSelectionFullPath(lfn);
    bootloaderFlash(lfn);
  }
  else if (result == STR_FLASH_INTERNAL_MODULE) {
    getSelectionFullPath(lfn);
    sportFlashDevice(INTERNAL_MODULE, lfn);
  }
  else if (result == STR_FLASH_EXTERNAL_MODULE) {
    // needed on X-Lite (as the R9M needs 2S while the external device flashing port only provides 5V)
    getSelectionFullPath(lfn);
    sportFlashDevice(EXTERNAL_MODULE, lfn);
  }
  else if (result == STR_FLASH_EXTERNAL_DEVICE) {
    getSelectionFullPath(lfn);
    sportFlashDevice(FLASHING_MODULE, lfn);
  }
#endif
#if defined(LUA)
  else if (result == STR_EXECUTE_FILE) {
    getSelectionFullPath(lfn);
    luaExec(lfn);
  }
#endif
}

void menuRadioSdManager(event_t _event)
{
  if (warningResult) {
    warningResult = 0;
    showMessageBox(STR_FORMATTING);
    logsClose();
#if defined(PCBSKY9X)
    Card_state = SD_ST_DATA;
#endif
#if defined(CPUARM)
    audioQueue.stopSD();
#endif
    if(sdCardFormat()) {
      f_chdir("/");
      REFRESH_FILES();
    }
  }

#if LCD_DEPTH > 1
  int lastPos = menuVerticalPosition;
#endif

  event_t event = (EVT_KEY_MASK(_event) == KEY_ENTER ? 0 : _event);
  SIMPLE_MENU(SD_IS_HC() ? STR_SDHC_CARD : STR_SD_CARD, menuTabGeneral, MENU_RADIO_SD_MANAGER, HEADER_LINE + reusableBuffer.sdmanager.count);

  switch (_event) {
    case EVT_ENTRY:
      f_chdir(ROOT_PATH);
      REFRESH_FILES();
#if LCD_DEPTH > 1
      lastPos = -1;
#endif
      break;

    case EVT_ENTRY_UP:
      menuVerticalOffset = reusableBuffer.sdmanager.offset;
      break;

#if defined(PCBX9) || defined(PCBX7) // TODO NO_MENU_KEY
    case EVT_KEY_LONG(KEY_MENU):
      if (!READ_ONLY() && s_editMode == 0) {
        killEvents(_event);
        POPUP_MENU_ADD_ITEM(STR_SD_INFO);
        POPUP_MENU_ADD_ITEM(STR_SD_FORMAT);
        POPUP_MENU_START(onSdManagerMenu);
      }
      break;
#endif

    case EVT_KEY_BREAK(KEY_EXIT):
      REFRESH_FILES();
      break;

#if !defined(PCBTARANIS)
    CASE_EVT_ROTARY_BREAK
    case EVT_KEY_FIRST(KEY_RIGHT):
#endif
    case EVT_KEY_BREAK(KEY_ENTER):
      if (s_editMode > 0) {
        break;
      }
      else {
        int index = menuVerticalPosition - HEADER_LINE - menuVerticalOffset;
        if (IS_DIRECTORY(reusableBuffer.sdmanager.lines[index])) {
          f_chdir(reusableBuffer.sdmanager.lines[index]);
          menuVerticalOffset = 0;
          menuVerticalPosition = HEADER_LINE;
          REFRESH_FILES();
          killEvents(_event);
          return;
        }
      }
      break;

    case EVT_KEY_LONG(KEY_ENTER):
#if !defined(PCBX9) && !defined(PCBX7) // TODO NO_HEADER_LINE
      if (menuVerticalPosition < HEADER_LINE) {
        killEvents(_event);
        POPUP_MENU_ADD_ITEM(STR_SD_INFO);
        POPUP_MENU_ADD_ITEM(STR_SD_FORMAT);
        POPUP_MENU_START(onSdManagerMenu);
        break;
      }
#endif
      if (s_editMode <= 0) {
        killEvents(_event);
        int index = menuVerticalPosition - HEADER_LINE - menuVerticalOffset;
        char * line = reusableBuffer.sdmanager.lines[index];
        if (!strcmp(line, "..")) {
          break; // no menu for parent dir
        }
#if defined(CPUARM)
        const char * ext = getFileExtension(line);
        if (ext) {
          if (!strcasecmp(ext, SOUNDS_EXT)) {
            POPUP_MENU_ADD_ITEM(STR_PLAY_FILE);
          }
#if LCD_DEPTH > 1
          else if (isExtensionMatching(ext, BITMAPS_EXT)) {
            if (!READ_ONLY() && (ext-line) <= (int)sizeof(g_model.header.bitmap)) {
              POPUP_MENU_ADD_ITEM(STR_ASSIGN_BITMAP);
            }
          }
#endif
          else if (!strcasecmp(ext, TEXT_EXT)) {
            POPUP_MENU_ADD_ITEM(STR_VIEW_TEXT);
          }
#if defined(LUA)
          else if (isExtensionMatching(ext, SCRIPTS_EXT)) {
            POPUP_MENU_ADD_ITEM(STR_EXECUTE_FILE);
          }
#endif
#if defined(PCBTARANIS)
          else if (!READ_ONLY() && !strcasecmp(ext, FIRMWARE_EXT)) {
            TCHAR lfn[_MAX_LFN + 1];
            getSelectionFullPath(lfn);
            if (isBootloader(lfn)) {
              POPUP_MENU_ADD_ITEM(STR_FLASH_BOOTLOADER);
            }
          }
          else if (!READ_ONLY() && !strcasecmp(ext, SPORT_FIRMWARE_EXT)) {
            if (HAS_SPORT_UPDATE_CONNECTOR())
              POPUP_MENU_ADD_ITEM(STR_FLASH_EXTERNAL_DEVICE);
            POPUP_MENU_ADD_ITEM(STR_FLASH_EXTERNAL_MODULE);
            POPUP_MENU_ADD_ITEM(STR_FLASH_INTERNAL_MODULE);
          }
#endif
        }
#endif
        if (!READ_ONLY()) {
#if defined(CPUARM)
          if (IS_FILE(line))
            POPUP_MENU_ADD_ITEM(STR_COPY_FILE);
          if (clipboard.type == CLIPBOARD_TYPE_SD_FILE)
            POPUP_MENU_ADD_ITEM(STR_PASTE);
          POPUP_MENU_ADD_ITEM(STR_RENAME_FILE);
#endif
          if (IS_FILE(line))
            POPUP_MENU_ADD_ITEM(STR_DELETE_FILE);
        }
        POPUP_MENU_START(onSdManagerMenu);
      }
      break;
  }

  if (reusableBuffer.sdmanager.offset != menuVerticalOffset) {
    FILINFO fno;
    DIR dir;

    if (menuVerticalOffset == 0) {
      reusableBuffer.sdmanager.offset = 0;
      memset(reusableBuffer.sdmanager.lines, 0, sizeof(reusableBuffer.sdmanager.lines));
    }
    else if (menuVerticalOffset == reusableBuffer.sdmanager.count-NUM_BODY_LINES) {
      reusableBuffer.sdmanager.offset = menuVerticalOffset;
      memset(reusableBuffer.sdmanager.lines, 0, sizeof(reusableBuffer.sdmanager.lines));
    }
    else if (menuVerticalOffset > reusableBuffer.sdmanager.offset) {
      memmove(reusableBuffer.sdmanager.lines[0], reusableBuffer.sdmanager.lines[1], (NUM_BODY_LINES-1)*sizeof(reusableBuffer.sdmanager.lines[0]));
      memset(reusableBuffer.sdmanager.lines[NUM_BODY_LINES-1], 0xff, SD_SCREEN_FILE_LENGTH);
      NODE_TYPE(reusableBuffer.sdmanager.lines[NUM_BODY_LINES-1]) = 1;
    }
    else {
      memmove(reusableBuffer.sdmanager.lines[1], reusableBuffer.sdmanager.lines[0], (NUM_BODY_LINES-1)*sizeof(reusableBuffer.sdmanager.lines[0]));
      memset(reusableBuffer.sdmanager.lines[0], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
    }

    reusableBuffer.sdmanager.count = 0;

    FRESULT res = f_opendir(&dir, "."); // Open the directory
    if (res == FR_OK) {
      bool firstTime = true;
      for (;;) {
        res = sdReadDir(&dir, &fno, firstTime);
        if (res != FR_OK || fno.fname[0] == 0) break;              /* Break on error or end of dir */
        if (strlen(fno.fname) > SD_SCREEN_FILE_LENGTH) continue;
        if (fno.fattrib & AM_HID) continue;                        /* Ignore Windows hidden files */
        if (fno.fname[0] == '.' && fno.fname[1] != '.') continue;  /* Ignore UNIX hidden files, but not .. */

        reusableBuffer.sdmanager.count++;

        bool isfile = !(fno.fattrib & AM_DIR);

        if (menuVerticalOffset == 0) {
          for (uint8_t i=0; i<NUM_BODY_LINES; i++) {
            char * line = reusableBuffer.sdmanager.lines[i];
            if (line[0] == '\0' || isFilenameLower(isfile, fno.fname, line)) {
              if (i < NUM_BODY_LINES-1) memmove(reusableBuffer.sdmanager.lines[i+1], line, sizeof(reusableBuffer.sdmanager.lines[i]) * (NUM_BODY_LINES-1-i));
              memset(line, 0, sizeof(reusableBuffer.sdmanager.lines[0]));
              strcpy(line, fno.fname);
              NODE_TYPE(line) = isfile;
              break;
            }
          }
        }
        else if (reusableBuffer.sdmanager.offset == menuVerticalOffset) {
          for (int8_t i=NUM_BODY_LINES-1; i>=0; i--) {
            char * line = reusableBuffer.sdmanager.lines[i];
            if (line[0] == '\0' || isFilenameGreater(isfile, fno.fname, line)) {
              if (i > 0) memmove(reusableBuffer.sdmanager.lines[0], reusableBuffer.sdmanager.lines[1], sizeof(reusableBuffer.sdmanager.lines[0]) * i);
              memset(line, 0, sizeof(reusableBuffer.sdmanager.lines[0]));
              strcpy(line, fno.fname);
              NODE_TYPE(line) = isfile;
              break;
            }
          }
        }
        else if (menuVerticalOffset > reusableBuffer.sdmanager.offset) {
          if (isFilenameGreater(isfile, fno.fname, reusableBuffer.sdmanager.lines[NUM_BODY_LINES-2]) && isFilenameLower(isfile, fno.fname, reusableBuffer.sdmanager.lines[NUM_BODY_LINES-1])) {
            memset(reusableBuffer.sdmanager.lines[NUM_BODY_LINES-1], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
            strcpy(reusableBuffer.sdmanager.lines[NUM_BODY_LINES-1], fno.fname);
            NODE_TYPE(reusableBuffer.sdmanager.lines[NUM_BODY_LINES-1]) = isfile;
          }
        }
        else {
          if (isFilenameLower(isfile, fno.fname, reusableBuffer.sdmanager.lines[1]) && isFilenameGreater(isfile, fno.fname, reusableBuffer.sdmanager.lines[0])) {
            memset(reusableBuffer.sdmanager.lines[0], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
            strcpy(reusableBuffer.sdmanager.lines[0], fno.fname);
            NODE_TYPE(reusableBuffer.sdmanager.lines[0]) = isfile;
          }
        }
      }
      f_closedir(&dir);
    }
  }

  reusableBuffer.sdmanager.offset = menuVerticalOffset;
  int index = menuVerticalPosition - HEADER_LINE - menuVerticalOffset;

  for (uint8_t i=0; i<NUM_BODY_LINES; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    lcdNextPos = 0;
    LcdFlags attr = (index == i ? INVERS : 0);
    if (reusableBuffer.sdmanager.lines[i][0]) {
      if (IS_DIRECTORY(reusableBuffer.sdmanager.lines[i])) {
        lcdDrawChar(0, y, '[', s_editMode == EDIT_MODIFY_STRING ? 0 : attr);
      }
#if defined(CPUARM)
      if (s_editMode == EDIT_MODIFY_STRING && attr) {
        uint8_t extlen, efflen;
        const char * ext = getFileExtension(reusableBuffer.sdmanager.originalName, 0, 0, NULL, &extlen);
        editName(lcdNextPos, y, reusableBuffer.sdmanager.lines[i], SD_SCREEN_FILE_LENGTH - extlen, _event, attr, 0);
        efflen = effectiveLen(reusableBuffer.sdmanager.lines[i], SD_SCREEN_FILE_LENGTH - extlen);
        if (s_editMode == 0) {
          if (ext) {
            strAppend(&reusableBuffer.sdmanager.lines[i][efflen], ext);
          }
          else {
            reusableBuffer.sdmanager.lines[i][efflen] = 0;
          }
          f_rename(reusableBuffer.sdmanager.originalName, reusableBuffer.sdmanager.lines[i]);
          REFRESH_FILES();
        }
      }
      else {
        lcdDrawText(lcdNextPos, y, reusableBuffer.sdmanager.lines[i], attr);
      }
#else
      lcdDrawText(lcdNextPos, y, reusableBuffer.sdmanager.lines[i], attr);
#endif
      if (IS_DIRECTORY(reusableBuffer.sdmanager.lines[i])) {
        lcdDrawChar(lcdNextPos, y, ']', s_editMode == EDIT_MODIFY_STRING ? 0 : attr);
      }
    }
  }

#if LCD_DEPTH > 1
  const char * ext = getFileExtension(reusableBuffer.sdmanager.lines[index]);
  if (ext && isExtensionMatching(ext, BITMAPS_EXT)) {
    if (lastPos != menuVerticalPosition) {
      if (!lcdLoadBitmap(modelBitmap, reusableBuffer.sdmanager.lines[index], MODEL_BITMAP_WIDTH, MODEL_BITMAP_HEIGHT)) {
        memcpy(modelBitmap, logo_taranis, MODEL_BITMAP_SIZE);
      }
    }
    lcdDrawBitmap(22*FW+2, 2*FH+FH/2, modelBitmap);
  }
#endif
}
