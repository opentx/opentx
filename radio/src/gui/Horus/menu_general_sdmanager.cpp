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

#include "../../opentx.h"

#define REFRESH_FILES()        do { reusableBuffer.sdmanager.offset = 65535; lastBitmap = -1; } while (0)
#define NODE_TYPE(fname)       fname[SD_SCREEN_FILE_LENGTH+1]
#define IS_DIRECTORY(fname)    ((bool)(!NODE_TYPE(fname)))
#define IS_FILE(fname)         ((bool)(NODE_TYPE(fname)))

int lastBitmap;

bool menuGeneralSdManagerInfo(evt_t event)
{
  SIMPLE_SUBMENU(STR_SD_INFO_TITLE, 1, DEFAULT_SCROLLBAR_X);

  lcdDrawText(MENUS_MARGIN_LEFT, 2*FH, STR_SD_TYPE);
  lcdDrawText(100, 2*FH, SD_IS_HC() ? STR_SDHC_CARD : STR_SD_CARD);

  lcdDrawText(MENUS_MARGIN_LEFT, 3*FH, STR_SD_SIZE);
  lcdDrawNumber(100, 3*FH, sdGetSize(), LEFT, 0, NULL, "M");

  lcdDrawText(MENUS_MARGIN_LEFT, 4*FH, STR_SD_SECTORS);
#if defined(SD_GET_FREE_BLOCKNR)
  lcdDrawNumber(100, 4*FH, SD_GET_FREE_BLOCKNR()/1000, LEFT, 0, NULL, "/");
  lcdDrawNumber(150, 4*FH, sdGetNoSectors()/1000, LEFT);
#else
  lcdDrawNumber(100, 4*FH, sdGetNoSectors()/1000, LEFT, 0, NULL, "k");
#endif

  lcdDrawText(MENUS_MARGIN_LEFT, 5*FH, STR_SD_SPEED);
  lcdDrawNumber(100, 5*FH, SD_GET_SPEED()/1000, LEFT, 0, NULL, "kb/s");

  return true;
}

inline bool isFilenameGreater(bool isfile, const char * fn, const char * line)
{
  return (isfile && IS_DIRECTORY(line)) || (isfile==IS_FILE(line) && strcasecmp(fn, line) > 0);
}

inline bool isFilenameLower(bool isfile, const char * fn, const char * line)
{
  return (!isfile && IS_FILE(line)) || (isfile==IS_FILE(line) && strcasecmp(fn, line) < 0);
}

void getSelectionFullPath(char *lfn)
{
  f_getcwd(lfn, _MAX_LFN);
  strcat(lfn, PSTR("/"));
  strcat(lfn, reusableBuffer.sdmanager.lines[m_posVert - s_pgOfs]);
}

void onSdManagerMenu(const char *result)
{
  TCHAR lfn[_MAX_LFN+1];

  // TODO possible buffer overflows here!

  uint8_t index = m_posVert-s_pgOfs;
  char *line = reusableBuffer.sdmanager.lines[index];

  if (result == STR_SD_INFO) {
    pushMenu(menuGeneralSdManagerInfo);
  }
  else if (result == STR_SD_FORMAT) {
    POPUP_CONFIRMATION(STR_CONFIRM_FORMAT);
  }
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
    char * ext = getFileExtension(line, SD_SCREEN_FILE_LENGTH+1);
    if (ext) {
      // write spaces to allow a longer filename
      memset(ext, ' ', SD_SCREEN_FILE_LENGTH-LEN_FILE_EXTENSION-(ext-line));
      line[SD_SCREEN_FILE_LENGTH-LEN_FILE_EXTENSION] = '\0';
    }
    else {
      int len = strlen(line);
      memset(line + strlen(line), ' ', SD_SCREEN_FILE_LENGTH - len);
      line[SD_SCREEN_FILE_LENGTH] = '\0';
    }
    s_editMode = EDIT_MODIFY_STRING;
    editNameCursorPos = 0;
  }
  else if (result == STR_DELETE_FILE) {
    getSelectionFullPath(lfn);
    f_unlink(lfn);
    REFRESH_FILES();
  }
  else if (result == STR_PLAY_FILE) {
    getSelectionFullPath(lfn);
    audioQueue.stopAll();
    audioQueue.playFile(lfn, 0, ID_PLAY_FROM_SD_MANAGER);
  }
#if 0
  else if (result == STR_ASSIGN_BITMAP) {
    strAppendFilename(g_model.header.bitmap, line, sizeof(g_model.header.bitmap));
    memcpy(modelHeaders[g_eeGeneral.currModel].bitmap, g_model.header.bitmap, sizeof(g_model.header.bitmap));
    storageDirty(EE_MODEL);
  }
#endif
  else if (result == STR_VIEW_TEXT) {
    getSelectionFullPath(lfn);
    pushMenuTextView(lfn);
  }
#if 0
  else if (result == STR_FLASH_BOOTLOADER) {
    getSelectionFullPath(lfn);
    flashBootloader(lfn);
  }
#endif
#if defined(LUA)
  else if (result == STR_EXECUTE_FILE) {
    getSelectionFullPath(lfn);
    luaExec(lfn);
  }
#endif
}

bool menuGeneralSdManager(evt_t _event)
{
  if (s_warning_result) {
    s_warning_result = 0;
    displayPopup(STR_FORMATTING);
    closeLogs();
    audioQueue.stopSD();
    if (f_mkfs(0, 1, 0) == FR_OK) {
      f_chdir("/");
      REFRESH_FILES();
    }
    else {
      POPUP_WARNING(STR_SDCARD_ERROR);
    }
  }

  evt_t event = (EVT_KEY_MASK(_event) == KEY_ENTER ? 0 : _event);
  SIMPLE_MENU(SD_IS_HC() ? STR_SDHC_CARD : STR_SD_CARD, menuTabGeneral, e_Sd, reusableBuffer.sdmanager.count, DEFAULT_SCROLLBAR_X);

  int index = m_posVert-s_pgOfs;

  switch(_event) {
    case EVT_ENTRY:
      f_chdir(ROOT_PATH);
      REFRESH_FILES();
      break;

#if 0
    case EVT_KEY_LONG(KEY_MENU):
      if (!READ_ONLY() && s_editMode == 0) {
        killEvents(_event);
        MENU_ADD_ITEM(STR_SD_INFO);
        MENU_ADD_ITEM(STR_SD_FORMAT);
        menuHandler = onSdManagerMenu;
      }
      break;
#endif

    case EVT_KEY_BREAK(KEY_EXIT):
      REFRESH_FILES();
      break;

    case EVT_KEY_BREAK(KEY_ENTER):
      if (m_posVert < 0) {
        if (reusableBuffer.sdmanager.count > 0) {
          m_posVert = 0;
        }
        break;
      }
      else if (s_editMode > 0) {
        break;
      }
      else {
        if (IS_DIRECTORY(reusableBuffer.sdmanager.lines[index])) {
          f_chdir(reusableBuffer.sdmanager.lines[index]);
          s_pgOfs = 0;
          m_posVert = 1;
          index = 1;
          REFRESH_FILES();
          killEvents(_event);
          return true;
        }
      }
      // no break

    case EVT_KEY_LONG(KEY_ENTER):
      if (s_editMode == 0) {
        killEvents(_event);
        char * line = reusableBuffer.sdmanager.lines[index];
        char * ext = getFileExtension(line, SD_SCREEN_FILE_LENGTH+1);
        if (!strcmp(line, "..")) {
          break;      // no menu for parent dir
        }
        if (ext) {
          if (!strcasecmp(ext, SOUNDS_EXT)) {
            MENU_ADD_ITEM(STR_PLAY_FILE);
          }
          else if (!strcasecmp(ext, TEXT_EXT)) {
            MENU_ADD_ITEM(STR_VIEW_TEXT);
          }
#if defined(LUA)
          else if (!strcasecmp(ext, SCRIPTS_EXT)) {
            MENU_ADD_ITEM(STR_EXECUTE_FILE);
          }
#endif
        }
        if (!READ_ONLY()) {
          if (IS_FILE(line))
            MENU_ADD_ITEM(STR_COPY_FILE);
          if (clipboard.type == CLIPBOARD_TYPE_SD_FILE)
            MENU_ADD_ITEM(STR_PASTE);
          MENU_ADD_ITEM(STR_RENAME_FILE);
          if (IS_FILE(line))
            MENU_ADD_ITEM(STR_DELETE_FILE);
        }
        menuHandler = onSdManagerMenu;
      }
      break;
  }

  if (reusableBuffer.sdmanager.offset != s_pgOfs) {
    FILINFO fno;
    DIR dir;
    char *fn;   /* This function is assuming non-Unicode cfg. */
    TCHAR lfn[_MAX_LFN + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
    
    if (s_pgOfs == 0) {
      reusableBuffer.sdmanager.offset = 0;
      memset(reusableBuffer.sdmanager.lines, 0, sizeof(reusableBuffer.sdmanager.lines));
    }
    else if (s_pgOfs == reusableBuffer.sdmanager.count-NUM_BODY_LINES) {
      reusableBuffer.sdmanager.offset = s_pgOfs;
      memset(reusableBuffer.sdmanager.lines, 0, sizeof(reusableBuffer.sdmanager.lines));
    }
    else if (s_pgOfs > reusableBuffer.sdmanager.offset) {
      memmove(reusableBuffer.sdmanager.lines[0], reusableBuffer.sdmanager.lines[1], (NUM_BODY_LINES-1)*sizeof(reusableBuffer.sdmanager.lines[0]));
      memset(reusableBuffer.sdmanager.lines[NUM_BODY_LINES-1], 0xff, SD_SCREEN_FILE_LENGTH);
      NODE_TYPE(reusableBuffer.sdmanager.lines[NUM_BODY_LINES-1]) = 1;
    }
    else {
      memmove(reusableBuffer.sdmanager.lines[1], reusableBuffer.sdmanager.lines[0], (NUM_BODY_LINES-1)*sizeof(reusableBuffer.sdmanager.lines[0]));
      memset(reusableBuffer.sdmanager.lines[0], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
    }

    reusableBuffer.sdmanager.count = 0;

    FRESULT res = f_opendir(&dir, ".");        /* Open the directory */
    if (res == FR_OK) {
      for (;;) {
        res = f_readdir(&dir, &fno);                   /* Read a directory item */
        if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
        if (fno.fname[0] == '.' && fno.fname[1] == '\0') continue;             /* Ignore dot entry */
#if _USE_LFN
        fn = *fno.lfname ? fno.lfname : fno.fname;
#else
        fn = fno.fname;
#endif
        if (strlen(fn) > SD_SCREEN_FILE_LENGTH) continue;

        reusableBuffer.sdmanager.count++;

        bool isfile = !(fno.fattrib & AM_DIR);

        if (s_pgOfs == 0) {
          for (int i=0; i<NUM_BODY_LINES; i++) {
            char *line = reusableBuffer.sdmanager.lines[i];
            if (line[0] == '\0' || isFilenameLower(isfile, fn, line)) {
              if (i < NUM_BODY_LINES-1) memmove(reusableBuffer.sdmanager.lines[i+1], line, sizeof(reusableBuffer.sdmanager.lines[i]) * (NUM_BODY_LINES-1-i));
              memset(line, 0, sizeof(reusableBuffer.sdmanager.lines[0]));
              strcpy(line, fn);
              NODE_TYPE(line) = isfile;
              break;
            }
          }
        }
        else if (reusableBuffer.sdmanager.offset == s_pgOfs) {
          for (int8_t i=NUM_BODY_LINES-1; i>=0; i--) {
            char *line = reusableBuffer.sdmanager.lines[i];
            if (line[0] == '\0' || isFilenameGreater(isfile, fn, line)) {
              if (i > 0) memmove(reusableBuffer.sdmanager.lines[0], reusableBuffer.sdmanager.lines[1], sizeof(reusableBuffer.sdmanager.lines[0]) * i);
              memset(line, 0, sizeof(reusableBuffer.sdmanager.lines[0]));
              strcpy(line, fn);
              NODE_TYPE(line) = isfile;
              break;
            }
          }
        }
        else if (s_pgOfs > reusableBuffer.sdmanager.offset) {
          if (isFilenameGreater(isfile, fn, reusableBuffer.sdmanager.lines[NUM_BODY_LINES-2]) && isFilenameLower(isfile, fn, reusableBuffer.sdmanager.lines[NUM_BODY_LINES-1])) {
            memset(reusableBuffer.sdmanager.lines[NUM_BODY_LINES-1], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
            strcpy(reusableBuffer.sdmanager.lines[NUM_BODY_LINES-1], fn);
            NODE_TYPE(reusableBuffer.sdmanager.lines[NUM_BODY_LINES-1]) = isfile;
          }
        }
        else {
          if (isFilenameLower(isfile, fn, reusableBuffer.sdmanager.lines[1]) && isFilenameGreater(isfile, fn, reusableBuffer.sdmanager.lines[0])) {
            memset(reusableBuffer.sdmanager.lines[0], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
            strcpy(reusableBuffer.sdmanager.lines[0], fn);
            NODE_TYPE(reusableBuffer.sdmanager.lines[0]) = isfile;
          }
        }
      }
    }
  }

  reusableBuffer.sdmanager.offset = s_pgOfs;

  for (int i=0; i<NUM_BODY_LINES; i++) {
    coord_t y = MENU_CONTENT_TOP + i*FH;
    LcdFlags attr = (index == i ? INVERS : 0);
    if (reusableBuffer.sdmanager.lines[i][0]) {
      if (IS_DIRECTORY(reusableBuffer.sdmanager.lines[i])) {
        char s[sizeof(reusableBuffer.sdmanager.lines[0])+2];
        sprintf(s, "[%s]", reusableBuffer.sdmanager.lines[i]);
        lcdDrawText(MENUS_MARGIN_LEFT, y, s, attr);
      }
      else {
        lcdDrawText(MENUS_MARGIN_LEFT, y, reusableBuffer.sdmanager.lines[i], attr);
      }
    }
  }

  char * ext = getFileExtension(reusableBuffer.sdmanager.lines[index], SD_SCREEN_FILE_LENGTH+1);
  if (ext && !strcasecmp(ext, BITMAPS_EXT)) {
    if (lastBitmap != m_posVert) {
      if (bmpLoad(modelBitmap, reusableBuffer.sdmanager.lines[index], MODEL_BITMAP_WIDTH, MODEL_BITMAP_HEIGHT)) {
        ((uint32_t *)modelBitmap)[0] = 0;
        lastBitmap = m_posVert;
      }
    }
    lcdDrawBitmap(LCD_W/2, (LCD_H-MODEL_BITMAP_HEIGHT)/2, modelBitmap);
  }
  
  return true;
}
