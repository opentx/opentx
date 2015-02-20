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

void menuGeneralSdManagerInfo(uint8_t event)
{
  SIMPLE_SUBMENU(STR_SD_INFO_TITLE, 1);

  lcd_putsLeft(2*FH, STR_SD_TYPE);
  lcd_puts(10*FW, 2*FH, SD_IS_HC() ? STR_SDHC_CARD : STR_SD_CARD);

  lcd_putsLeft(3*FH, STR_SD_SIZE);
  lcd_outdezAtt(10*FW, 3*FH, sdGetSize(), LEFT);
  lcd_putc(lcdLastPos, 3*FH, 'M');

  lcd_putsLeft(4*FH, STR_SD_SECTORS);
#if defined(SD_GET_FREE_BLOCKNR)
  lcd_outdezAtt(10*FW, 4*FH,  SD_GET_FREE_BLOCKNR()/1000, LEFT);
  lcd_putc(lcdLastPos, 4*FH, '/');
  lcd_outdezAtt(lcdLastPos+FW, 4*FH, sdGetNoSectors()/1000, LEFT);
#else
  lcd_outdezAtt(10*FW, 4*FH, sdGetNoSectors()/1000, LEFT);
#endif
  lcd_putc(lcdLastPos, 4*FH, 'k');

  lcd_putsLeft(5*FH, STR_SD_SPEED);
  lcd_outdezAtt(10*FW, 5*FH, SD_GET_SPEED()/1000, LEFT);
  lcd_puts(lcdLastPos, 5*FH, "kb/s");
}

inline bool isFilenameGreater(bool isfile, const char * fn, const char * line)
{
  return (isfile && !line[SD_SCREEN_FILE_LENGTH+1]) || (isfile==(bool)line[SD_SCREEN_FILE_LENGTH+1] && strcasecmp(fn, line) > 0);
}

inline bool isFilenameLower(bool isfile, const char * fn, const char * line)
{
  return (!isfile && line[SD_SCREEN_FILE_LENGTH+1]) || (isfile==(bool)line[SD_SCREEN_FILE_LENGTH+1] && strcasecmp(fn, line) < 0);
}

void onSdManagerMenu(const char *result)
{
  TCHAR lfn[_MAX_LFN+1];

  uint8_t index = m_posVert-1-s_pgOfs;
  if (result == STR_SD_INFO) {
    pushMenu(menuGeneralSdManagerInfo);
  }
  else if (result == STR_SD_FORMAT) {
    POPUP_CONFIRMATION(STR_CONFIRM_FORMAT);
  }
  else if (result == STR_DELETE_FILE) {
    f_getcwd(lfn, _MAX_LFN);
    strcat_P(lfn, PSTR("/"));
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    f_unlink(lfn);
    strncpy(statusLineMsg, reusableBuffer.sdmanager.lines[index], 13);
    strcpy_P(statusLineMsg+min((uint8_t)strlen(statusLineMsg), (uint8_t)13), STR_REMOVED);
    showStatusLine();
    if ((uint16_t)m_posVert == reusableBuffer.sdmanager.count) m_posVert--;
    reusableBuffer.sdmanager.offset = s_pgOfs-1;
  }
#if defined(CPUARM)
  /* TODO else if (result == STR_LOAD_FILE) {
    f_getcwd(lfn, _MAX_LFN);
    strcat(lfn, "/");
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    POPUP_WARNING(eeLoadModelSD(lfn));
  } */
  else if (result == STR_PLAY_FILE) {
    f_getcwd(lfn, _MAX_LFN);
    strcat(lfn, "/");
    strcat(lfn, reusableBuffer.sdmanager.lines[index]);
    audioQueue.stopAll();
    audioQueue.playFile(lfn, 0, ID_PLAY_FROM_SD_MANAGER);
  }
#endif
}

void menuGeneralSdManager(uint8_t _event)
{
  FILINFO fno;
  DIR dir;
  char *fn;   /* This function is assuming non-Unicode cfg. */
  TCHAR lfn[_MAX_LFN + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);

#if defined(SDCARD)
  if (s_warning_result) {
    s_warning_result = 0;
    displayPopup(STR_FORMATTING);
    closeLogs();
#if defined(PCBSKY9X)
    Card_state = SD_ST_DATA;
#endif
#if defined(CPUARM)
    audioQueue.stopSD();
#endif
    if (f_mkfs(0, 1, 0) == FR_OK) {
      f_chdir("/");
      reusableBuffer.sdmanager.offset = -1;
    }
    else {
      POPUP_WARNING(STR_SDCARD_ERROR);
    }
  }
#endif

  uint8_t event = ((READ_ONLY() && EVT_KEY_MASK(_event) == KEY_ENTER) ? 0 : _event);
  SIMPLE_MENU(SD_IS_HC() ? STR_SDHC_CARD : STR_SD_CARD, menuTabGeneral, e_Sd, 1+reusableBuffer.sdmanager.count);

  if (s_editMode > 0)
    s_editMode = 0;

  switch(_event) {
    case EVT_ENTRY:
      f_chdir(ROOT_PATH);
      reusableBuffer.sdmanager.offset = 65535;
      break;

    CASE_EVT_ROTARY_BREAK
    case EVT_KEY_FIRST(KEY_RIGHT):
    case EVT_KEY_FIRST(KEY_ENTER):
    {
      if (m_posVert > 0) {
        vertpos_t index = m_posVert-1-s_pgOfs;
        if (!reusableBuffer.sdmanager.lines[index][SD_SCREEN_FILE_LENGTH+1]) {
          f_chdir(reusableBuffer.sdmanager.lines[index]);
          s_pgOfs = 0;
          m_posVert = 1;
          reusableBuffer.sdmanager.offset = 65535;
          killEvents(_event);
          break;
        }
      }
      if (!IS_ROTARY_BREAK(_event) || m_posVert==0)
        break;
      // no break;
    }

    case EVT_KEY_LONG(KEY_ENTER):
      killEvents(_event);
      if (m_posVert == 0) {
        MENU_ADD_ITEM(STR_SD_INFO);
        MENU_ADD_ITEM(STR_SD_FORMAT);
      }
      else
      {
#if defined(CPUARM)
        uint8_t index = m_posVert-1-s_pgOfs;
        // TODO duplicated code for finding extension
        char * ext = reusableBuffer.sdmanager.lines[index];
        int len = strlen(ext) - 4;
        ext += len;
        /* TODO if (!strcasecmp(ext, MODELS_EXT)) {
          s_menu[s_menu_count++] = STR_LOAD_FILE;
        }
        else */ if (!strcasecmp(ext, SOUNDS_EXT)) {
          MENU_ADD_ITEM(STR_PLAY_FILE);
        }
#endif
        if (!READ_ONLY()) {
          MENU_ADD_ITEM(STR_DELETE_FILE);
          // MENU_ADD_ITEM(STR_RENAME_FILE);  TODO: Implement
          // MENU_ADD_ITEM(STR_COPY_FILE);    TODO: Implement
        }
      }
      menuHandler = onSdManagerMenu;
      break;
  }

  if (reusableBuffer.sdmanager.offset != s_pgOfs) {
    if (s_pgOfs == 0) {
      reusableBuffer.sdmanager.offset = 0;
      memset(reusableBuffer.sdmanager.lines, 0, sizeof(reusableBuffer.sdmanager.lines));
    }
    else if (s_pgOfs == reusableBuffer.sdmanager.count-7) {
      reusableBuffer.sdmanager.offset = s_pgOfs;
      memset(reusableBuffer.sdmanager.lines, 0, sizeof(reusableBuffer.sdmanager.lines));
    }
    else if (s_pgOfs > reusableBuffer.sdmanager.offset) {
      memmove(reusableBuffer.sdmanager.lines[0], reusableBuffer.sdmanager.lines[1], 6*sizeof(reusableBuffer.sdmanager.lines[0]));
      memset(reusableBuffer.sdmanager.lines[6], 0xff, SD_SCREEN_FILE_LENGTH);
      reusableBuffer.sdmanager.lines[6][SD_SCREEN_FILE_LENGTH+1] = 1;
    }
    else {
      memmove(reusableBuffer.sdmanager.lines[1], reusableBuffer.sdmanager.lines[0], 6*sizeof(reusableBuffer.sdmanager.lines[0]));
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
          for (uint8_t i=0; i<LCD_LINES-1; i++) {
            char *line = reusableBuffer.sdmanager.lines[i];
            if (line[0] == '\0' || isFilenameLower(isfile, fn, line)) {
              if (i < 6) memmove(reusableBuffer.sdmanager.lines[i+1], line, sizeof(reusableBuffer.sdmanager.lines[i]) * (6-i));
              memset(line, 0, sizeof(reusableBuffer.sdmanager.lines[i]));
              strcpy(line, fn);
              line[SD_SCREEN_FILE_LENGTH+1] = isfile;
              break;
            }
          }
        }
        else if (reusableBuffer.sdmanager.offset == s_pgOfs) {
          for (int8_t i=6; i>=0; i--) {
            char *line = reusableBuffer.sdmanager.lines[i];
            if (line[0] == '\0' || isFilenameGreater(isfile, fn, line)) {
              if (i > 0) memmove(reusableBuffer.sdmanager.lines[0], reusableBuffer.sdmanager.lines[1], sizeof(reusableBuffer.sdmanager.lines[0]) * i);
              memset(line, 0, sizeof(reusableBuffer.sdmanager.lines[i]));
              strcpy(line, fn);
              line[SD_SCREEN_FILE_LENGTH+1] = isfile;
              break;
            }
          }
        }
        else if (s_pgOfs > reusableBuffer.sdmanager.offset) {
          if (isFilenameGreater(isfile, fn, reusableBuffer.sdmanager.lines[5]) && isFilenameLower(isfile, fn, reusableBuffer.sdmanager.lines[6])) {
            memset(reusableBuffer.sdmanager.lines[6], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
            strcpy(reusableBuffer.sdmanager.lines[6], fn);
            reusableBuffer.sdmanager.lines[6][SD_SCREEN_FILE_LENGTH+1] = isfile;
          }
        }
        else {
          if (isFilenameLower(isfile, fn, reusableBuffer.sdmanager.lines[1]) && isFilenameGreater(isfile, fn, reusableBuffer.sdmanager.lines[0])) {
            memset(reusableBuffer.sdmanager.lines[0], 0, sizeof(reusableBuffer.sdmanager.lines[0]));
            strcpy(reusableBuffer.sdmanager.lines[0], fn);
            reusableBuffer.sdmanager.lines[0][SD_SCREEN_FILE_LENGTH+1] = isfile;
          }
        }
      }
    }
  }

  reusableBuffer.sdmanager.offset = s_pgOfs;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    lcdNextPos = 0;
    uint8_t attr = (m_posVert-1-s_pgOfs == i ? BSS|INVERS : BSS);
    if (reusableBuffer.sdmanager.lines[i][0]) {
      if (!reusableBuffer.sdmanager.lines[i][SD_SCREEN_FILE_LENGTH+1]) { lcd_putcAtt(0, y, '[', attr); }
      lcd_putsAtt(lcdNextPos, y, reusableBuffer.sdmanager.lines[i], attr);
      if (!reusableBuffer.sdmanager.lines[i][SD_SCREEN_FILE_LENGTH+1]) { lcd_putcAtt(lcdNextPos, y, ']', attr); }
    }
  }
}
