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

#include "../opentx.h"

uint8_t eeFindEmptyModel(uint8_t id, bool down)
{
  uint8_t i = id;
  for (;;) {
    i = (MAX_MODELS + (down ? i+1 : i-1)) % MAX_MODELS;
    if (!eeModelExists(i)) break;
    if (i == id) return 0xff; // no free space in directory left
  }
  return i;
}

void selectModel(uint8_t sub)
{
  displayPopup(STR_LOADINGMODEL);
  saveTimers();
  eeCheck(true); // force writing of current model data before this is changed
  g_eeGeneral.currModel = sub;
  eeDirty(EE_GENERAL);
  eeLoadModel(sub);
}

#if defined(SDCARD)
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
  else if (s_menu_offset == s_menu_count - MENU_MAX_LINES) {
    s_last_menu_offset = 0xffff;
    memset(reusableBuffer.modelsel.menu_bss, 0, sizeof(reusableBuffer.modelsel.menu_bss));
  }
  else if (s_menu_offset == s_last_menu_offset) {
    // should not happen, only there because of Murphy's law
    return true;
  }
  else if (s_menu_offset > s_last_menu_offset) {
    memmove(reusableBuffer.modelsel.menu_bss[0], reusableBuffer.modelsel.menu_bss[1], (MENU_MAX_LINES-1)*MENU_LINE_LENGTH);
    memset(reusableBuffer.modelsel.menu_bss[MENU_MAX_LINES-1], 0xff, MENU_LINE_LENGTH);
  }
  else {
    memmove(reusableBuffer.modelsel.menu_bss[1], reusableBuffer.modelsel.menu_bss[0], (MENU_MAX_LINES-1)*MENU_LINE_LENGTH);
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
          for (uint8_t i=0; i<MENU_MAX_LINES; i++) {
            char *line = reusableBuffer.modelsel.menu_bss[i];
            if (line[0] == '\0' || strcasecmp(fn, line) < 0) {
              if (i < MENU_MAX_LINES-1) memmove(reusableBuffer.modelsel.menu_bss[i+1], line, sizeof(reusableBuffer.modelsel.menu_bss[i]) * (MENU_MAX_LINES-1-i));
              memset(line, 0, MENU_LINE_LENGTH);
              strcpy(line, fn);
              break;
            }
          }
        }
        for (uint8_t i=0; i<min(s_menu_count, (uint16_t)MENU_MAX_LINES); i++)
          s_menu[i] = reusableBuffer.modelsel.menu_bss[i];
      }
      else if (s_last_menu_offset == 0xffff) {
        for (int i=MENU_MAX_LINES-1; i>=0; i--) {
          char *line = reusableBuffer.modelsel.menu_bss[i];
          if (line[0] == '\0' || strcasecmp(fn, line) > 0) {
            if (i > 0) memmove(reusableBuffer.modelsel.menu_bss[0], reusableBuffer.modelsel.menu_bss[1], sizeof(reusableBuffer.modelsel.menu_bss[i]) * i);
            memset(line, 0, MENU_LINE_LENGTH);
            strcpy(line, fn);
            break;
          }
        }
        for (uint8_t i=0; i<min(s_menu_count, (uint16_t)MENU_MAX_LINES); i++)
          s_menu[i] = reusableBuffer.modelsel.menu_bss[i];
      }
      else if (s_menu_offset > s_last_menu_offset) {
        if (strcasecmp(fn, reusableBuffer.modelsel.menu_bss[MENU_MAX_LINES-2]) > 0 && strcasecmp(fn, reusableBuffer.modelsel.menu_bss[MENU_MAX_LINES-1]) < 0) {
          memset(reusableBuffer.modelsel.menu_bss[MENU_MAX_LINES-1], 0, MENU_LINE_LENGTH);
          strcpy(reusableBuffer.modelsel.menu_bss[MENU_MAX_LINES-1], fn);
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
#endif

#if defined(PCBTARANIS)
  static int8_t modelselBitmapIdx;
  static uint8_t modelselBitmap[MODEL_BITMAP_SIZE];
  #define MODELSEL_W 133
  #define BMP_DIRTY()  modelselBitmapIdx = -1
#else
  #define MODELSEL_W LCD_W
  #define BMP_DIRTY()
#endif

#if defined(NAVIGATION_MENUS)
void onModelSelectMenu(const char *result)
{
  int8_t sub = m_posVert;

  if (result == STR_SELECT_MODEL || result == STR_CREATE_MODEL) {
    selectModel(sub);
  }
  else if (result == STR_COPY_MODEL) {
    s_copyMode = COPY_MODE;
    s_copyTgtOfs = 0;
    s_copySrcRow = -1;
  }
  else if (result == STR_MOVE_MODEL) {
    s_copyMode = MOVE_MODE;
    s_copyTgtOfs = 0;
    s_copySrcRow = -1;
  }
#if defined(SDCARD)
  else if (result == STR_BACKUP_MODEL) {
    eeCheck(true); // force writing of current model data before this is changed
    POPUP_WARNING(eeBackupModel(sub));
  }
  else if (result == STR_RESTORE_MODEL || result == STR_UPDATE_LIST) {
    if (!listSdFiles(MODELS_PATH, MODELS_EXT, MENU_LINE_LENGTH-1, NULL)) {
      POPUP_WARNING(STR_NO_MODELS_ON_SD);
      s_menu_flags = 0;
    }
  }
#endif
  else if (result == STR_DELETE_MODEL) {
    POPUP_CONFIRMATION(STR_DELETEMODEL);
#if defined(CPUARM)
    SET_WARNING_INFO(modelHeaders[sub].name, sizeof(g_model.header.name), ZCHAR);
#else
    char * name = reusableBuffer.modelsel.mainname;
    eeLoadModelName(sub, name);
    SET_WARNING_INFO(name, sizeof(g_model.header.name), ZCHAR);
#endif
  }
#if defined(SDCARD)
  else {
    // The user choosed a file on SD to restore
    POPUP_WARNING(eeRestoreModel(sub, (char *)result));
    BMP_DIRTY();
    if (!s_warning && g_eeGeneral.currModel == sub)
      eeLoadModel(sub);
  }
#endif
}
#endif

void menuModelSelect(uint8_t event)
{
  if (s_warning_result) {
    s_warning_result = 0;
    eeDeleteModel(m_posVert); // delete file
    s_copyMode = 0;
    event = EVT_ENTRY_UP;
    BMP_DIRTY();
  }

  uint8_t _event_ = (IS_ROTARY_BREAK(event) || IS_ROTARY_LONG(event) ? 0 : event);

#if defined(PCBTARANIS)
  if ((s_copyMode && EVT_KEY_MASK(event) == KEY_EXIT) || event == EVT_KEY_BREAK(KEY_EXIT))
    _event_ -= KEY_EXIT;
#else
  if (s_copyMode && EVT_KEY_MASK(event) == KEY_EXIT)
    _event_ -= KEY_EXIT;
#endif

  int8_t oldSub = m_posVert;

  check_submenu_simple(_event_, MAX_MODELS-1);

#if defined(NAVIGATION_POT2)
  if (event==0 && p2valdiff<0) {
    event = EVT_KEY_FIRST(KEY_RIGHT);
  }
#endif

  if (s_editMode > 0) s_editMode = 0;

#if !defined(CPUARM)
  if (event) {
    eeFlush(); // flush eeprom write
  }
#endif

  int8_t sub = m_posVert;

  switch (event)
  {
      case EVT_ENTRY:
        m_posVert = sub = g_eeGeneral.currModel;
        if (sub >= LCD_LINES-1) s_pgOfs = sub-LCD_LINES+2;
        s_copyMode = 0;
        s_editMode = EDIT_MODE_INIT;
        BMP_DIRTY();
        eeCheck(true);
        break;
      case EVT_KEY_LONG(KEY_EXIT):
        if (s_copyMode && s_copyTgtOfs == 0 && g_eeGeneral.currModel != sub && eeModelExists(sub)) {
          POPUP_CONFIRMATION(STR_DELETEMODEL);
#if defined(CPUARM)
          SET_WARNING_INFO(modelHeaders[sub].name, sizeof(g_model.header.name), ZCHAR);
#else
          char * name = reusableBuffer.modelsel.mainname;
          eeLoadModelName(sub, name);
          SET_WARNING_INFO(name, sizeof(g_model.header.name), ZCHAR);
#endif
          killEvents(event);
          break;
        }
        // no break
#if defined(ROTARY_ENCODER_NAVIGATION)
      case EVT_ROTARY_LONG:
        killEvents(event);
        if (s_editMode < 0) {
          popMenu();
          break;
        }
        else if (!s_copyMode) {
          m_posVert = sub = g_eeGeneral.currModel;
          s_copyMode = 0;
          s_editMode = EDIT_MODE_INIT;
        }
        // no break
#endif
      case EVT_KEY_BREAK(KEY_EXIT):
        if (s_copyMode) {
          sub = m_posVert = (s_copyMode == MOVE_MODE || s_copySrcRow<0) ? (MAX_MODELS+sub+s_copyTgtOfs) % MAX_MODELS : s_copySrcRow;
          s_copyMode = 0;
        }
#if defined(PCBTARANIS)
        else {
          if (m_posVert != g_eeGeneral.currModel) {
            m_posVert = g_eeGeneral.currModel;
            s_pgOfs = 0;
          }
          else if (event != EVT_KEY_LONG(KEY_EXIT)) {
            popMenu();
          }
        }
#endif
        break;
#if defined(ROTARY_ENCODER_NAVIGATION)
      case EVT_ROTARY_BREAK:
        if (s_editMode == -1) {
          s_editMode = 0;
          break;
        }
        // no break;
#endif
      case EVT_KEY_LONG(KEY_ENTER):
      case EVT_KEY_BREAK(KEY_ENTER):
        s_editMode = 0;
        if (READ_ONLY()) {
          if (g_eeGeneral.currModel != sub && eeModelExists(sub)) {
            selectModel(sub);
          }
        }
        else if (s_copyMode && (s_copyTgtOfs || s_copySrcRow>=0)) {
          displayPopup(s_copyMode==COPY_MODE ? STR_COPYINGMODEL : STR_MOVINGMODEL);
          eeCheck(true); // force writing of current model data before this is changed

          uint8_t cur = (MAX_MODELS + sub + s_copyTgtOfs) % MAX_MODELS;

          if (s_copyMode == COPY_MODE) {
            if (eeCopyModel(cur, s_copySrcRow))
              BMP_DIRTY();
            else
              cur = sub;
          }

          s_copySrcRow = g_eeGeneral.currModel; // to update the currModel value
          while (sub != cur) {
            uint8_t src = cur;
            cur = (s_copyTgtOfs > 0 ? cur+MAX_MODELS-1 : cur+1) % MAX_MODELS;
            eeSwapModels(src, cur);
            BMP_DIRTY();
            if (src == s_copySrcRow)
              s_copySrcRow = cur;
            else if (cur == s_copySrcRow)
              s_copySrcRow = src;
          }

          if (s_copySrcRow != g_eeGeneral.currModel) {
            g_eeGeneral.currModel = s_copySrcRow;
            eeDirty(EE_GENERAL);
          }

          s_copyMode = 0;
          event = EVT_ENTRY_UP;
        }
        else if (event == EVT_KEY_LONG(KEY_ENTER)
#if !defined(PCBTARANIS)
            || IS_ROTARY_BREAK(event)
#endif
            ) {
          s_copyMode = 0;
          killEvents(event);
#if defined(NAVIGATION_MENUS)
          if (g_eeGeneral.currModel != sub) {
            if (eeModelExists(sub)) {
              MENU_ADD_ITEM(STR_SELECT_MODEL);
              MENU_ADD_SD_ITEM(STR_BACKUP_MODEL);
              MENU_ADD_ITEM(STR_COPY_MODEL);
              MENU_ADD_ITEM(STR_MOVE_MODEL);
              MENU_ADD_ITEM(STR_DELETE_MODEL);
            }
            else {
#if defined(SDCARD)
              MENU_ADD_ITEM(STR_CREATE_MODEL);
              MENU_ADD_ITEM(STR_RESTORE_MODEL);
#else
              selectModel(sub);
#endif
            }
          }
          else {
            MENU_ADD_SD_ITEM(STR_BACKUP_MODEL);
            MENU_ADD_ITEM(STR_COPY_MODEL);
            MENU_ADD_ITEM(STR_MOVE_MODEL);
          }
          menuHandler = onModelSelectMenu;
#else
          if (g_eeGeneral.currModel != sub) {
            selectModel(sub);
          }
#endif
        }
        else if (eeModelExists(sub)) {
          s_copyMode = (s_copyMode == COPY_MODE ? MOVE_MODE : COPY_MODE);
          s_copyTgtOfs = 0;
          s_copySrcRow = -1;
        }
        break;

#if defined(PCBTARANIS)
      case EVT_KEY_BREAK(KEY_PAGE):
      case EVT_KEY_LONG(KEY_PAGE):
        chainMenu(event == EVT_KEY_BREAK(KEY_PAGE) ? menuModelSetup : menuTabModel[DIM(menuTabModel)-1]);
        killEvents(event);
        break;
#else
#if defined(ROTARY_ENCODER_NAVIGATION)
      case EVT_ROTARY_LEFT:
      case EVT_ROTARY_RIGHT:
#endif
      case EVT_KEY_FIRST(KEY_LEFT):
      case EVT_KEY_FIRST(KEY_RIGHT):
#if defined(ROTARY_ENCODER_NAVIGATION)
        if ((!IS_ROTARY_RIGHT(event) && !IS_ROTARY_LEFT(event)) || s_editMode < 0) {
#endif
        if (sub == g_eeGeneral.currModel) {
          chainMenu((IS_ROTARY_RIGHT(event) || event == EVT_KEY_FIRST(KEY_RIGHT)) ? menuModelSetup : menuTabModel[DIM(menuTabModel)-1]);
        }
        else {
          AUDIO_WARNING2();
        }
        break;
#if defined(ROTARY_ENCODER_NAVIGATION)
        }
        // no break
#endif
#endif

      case EVT_KEY_FIRST(KEY_MOVE_UP):
      case EVT_KEY_REPT(KEY_MOVE_UP):
      case EVT_KEY_FIRST(KEY_MOVE_DOWN):
      case EVT_KEY_REPT(KEY_MOVE_DOWN):
        if (s_copyMode) {
          int8_t next_ofs = s_copyTgtOfs + oldSub - m_posVert;
          if (next_ofs == MAX_MODELS || next_ofs == -MAX_MODELS)
            next_ofs = 0;

          if (s_copySrcRow < 0 && s_copyMode==COPY_MODE) {
            s_copySrcRow = oldSub;
            // find a hole (in the first empty slot above / below)
            sub = eeFindEmptyModel(s_copySrcRow, IS_ROTARY_DOWN(event) || event==EVT_KEY_FIRST(KEY_MOVE_DOWN));
            if (sub < 0) {
              // no free room for duplicating the model
              AUDIO_ERROR();
              sub = oldSub;
              s_copyMode = 0;
            }
            next_ofs = 0;
            m_posVert = sub;
          }
          s_copyTgtOfs = next_ofs;
        }
        break;
  }

#if defined(PCBHORUS)
#elif defined(PCBTARANIS)
  lcd_puts(27*FW-(LEN_FREE-4)*FW, 0, STR_FREE);
  if (event) reusableBuffer.modelsel.eepromfree = EeFsGetFree();
  lcd_outdezAtt(20*FW, 0, reusableBuffer.modelsel.eepromfree, 0);
  lcd_puts(21*FW, 0, STR_BYTES);
#elif !defined(PCBSKY9X)
  lcd_puts(9*FW-(LEN_FREE-4)*FW, 0, STR_FREE);
  if (event) reusableBuffer.modelsel.eepromfree = EeFsGetFree();
  lcd_outdezAtt(17*FW, 0, reusableBuffer.modelsel.eepromfree, 0);
#endif

#if defined(PCBHORUS)
  // nothing
#elif defined(PCBTARANIS)
  displayScreenIndex(e_ModelSelect, DIM(menuTabModel), 0);
  lcd_filled_rect(0, 0, LCD_W, FH, SOLID, FILL_WHITE|GREY_DEFAULT);
#elif defined(ROTARY_ENCODER_NAVIGATION)
  displayScreenIndex(e_ModelSelect, DIM(menuTabModel), (sub == g_eeGeneral.currModel) ? ((IS_RE_NAVIGATION_ENABLE() && s_editMode < 0) ? INVERS|BLINK : INVERS) : 0);
#else
  displayScreenIndex(e_ModelSelect, DIM(menuTabModel), (sub == g_eeGeneral.currModel) ? INVERS : 0);
#endif

  TITLE(STR_MENUMODELSEL);

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_TITLE_HEIGHT + 1 + i*FH;
    uint8_t k = i+s_pgOfs;
    lcd_outdezNAtt(3*FW+2, y, k+1, LEADING0+((!s_copyMode && sub==k) ? INVERS : 0), 2);

    if (s_copyMode == MOVE_MODE || (s_copyMode == COPY_MODE && s_copySrcRow >= 0)) {
      if (k == sub) {
        if (s_copyMode == COPY_MODE) {
          k = s_copySrcRow;
          lcd_putc(MODELSEL_W-FW, y, '+');
        }
        else {
          k = sub + s_copyTgtOfs;
        }
      }
      else if (s_copyTgtOfs < 0 && ((k < sub && k >= sub+s_copyTgtOfs) || (k-MAX_MODELS < sub && k-MAX_MODELS >= sub+s_copyTgtOfs)))
        k += 1;
      else if (s_copyTgtOfs > 0 && ((k > sub && k <= sub+s_copyTgtOfs) || (k+MAX_MODELS > sub && k+MAX_MODELS <= sub+s_copyTgtOfs)))
        k += MAX_MODELS-1;
    }

    k %= MAX_MODELS;

    if (eeModelExists(k)) {
#if defined(PCBSKY9X)
      putsModelName(4*FW, y, modelHeaders[k].name, k, 0);
#elif defined(CPUARM)
      putsModelName(4*FW, y, modelHeaders[k].name, k, 0);
      lcd_outdezAtt(20*FW, y, eeModelSize(k), 0);
#else
      char * name = reusableBuffer.modelsel.listnames[i];
      if (event) eeLoadModelName(k, name);
      putsModelName(4*FW, y, name, k, 0);
      lcd_outdezAtt(20*FW, y, eeModelSize(k), 0);
#endif
      if (k==g_eeGeneral.currModel && (s_copyMode!=COPY_MODE || s_copySrcRow<0 || i+s_pgOfs!=(vertpos_t)sub)) lcd_putc(1, y, '*');
    }

    if (s_copyMode && (vertpos_t)sub==i+s_pgOfs) {
      lcd_filled_rect(9, y, MODELSEL_W-1-9, 7);
      lcd_rect(8, y-1, MODELSEL_W-1-7, 9, s_copyMode == COPY_MODE ? SOLID : DOTTED);
    }
  }

#if defined(PCBTARANIS)
  if (modelselBitmapIdx != m_posVert) {
    modelselBitmapIdx = m_posVert;
    if (modelselBitmapIdx == g_eeGeneral.currModel)
      memcpy(modelselBitmap, modelBitmap, MODEL_BITMAP_SIZE);
    else
      loadModelBitmap(modelHeaders[sub].bitmap, modelselBitmap);
  }
#if !defined(PCBHORUS)
  lcd_bmp(22*FW+2, 2*FH+FH/2, modelselBitmap);
#endif
#endif
}
