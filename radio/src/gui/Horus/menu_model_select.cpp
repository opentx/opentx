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

#include "../../opentx.h"

#define POS_MODEL_NAME    35
#define POS_MODEL_SIZE    170

void onModelSelectMenu(const char *result)
{
  int sub = m_posVert;

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
  else if (result == STR_DELETE_MODEL) {
    POPUP_CONFIRMATION(STR_DELETEMODEL);
    SET_WARNING_INFO(modelHeaders[sub].name, sizeof(g_model.header.name), ZCHAR);
  }
  else {
    // The user choosed a file on SD to restore
    POPUP_WARNING(eeRestoreModel(sub, (char *)result));
    if (!s_warning && g_eeGeneral.currModel == sub)
      eeLoadModel(sub);
  }
}

char *strAppendModelSize(char *dest, int size)
{
  char s[8] = "---";
  if (size) sprintf(s, "%d", size);
  dest = strSetCursor(dest, POS_MODEL_SIZE-getTextWidth(s));
  return strAppend(dest, s);
}

void menuModelSelect(evt_t event)
{
  if (s_warning_result) {
    s_warning_result = 0;
    eeDeleteModel(m_posVert); // delete file
    s_copyMode = 0;
    event = EVT_ENTRY_UP;
  }

  evt_t _event_ = event;
  event = (IS_ROTARY_BREAK(event) || IS_ROTARY_LONG(event) ? 0 : event);
  if ((s_copyMode && EVT_KEY_MASK(event) == KEY_EXIT) || event == EVT_KEY_BREAK(KEY_EXIT)) {
    event = 0;
  }

  int8_t oldSub = m_posVert;

  // TODO SIMPLE_MENU(STR_MENUMODELSEL, menuTabModel, e_ModelSelect, MAX_MODELS, DEFAULT_SCROLLBAR_X);

  if (s_editMode > 0) s_editMode = 0;

  int8_t sub = m_posVert;

  switch (_event_)
  {
      case EVT_ENTRY:
        m_posVert = sub = g_eeGeneral.currModel;
        if (sub >= NUM_BODY_LINES) s_pgOfs = sub-(NUM_BODY_LINES-1);
        s_copyMode = 0;
        s_editMode = EDIT_MODE_INIT;
        eeCheck(true);
        break;
      case EVT_KEY_LONG(KEY_EXIT):
        if (s_copyMode && s_copyTgtOfs == 0 && g_eeGeneral.currModel != sub && eeModelExists(sub)) {
          POPUP_CONFIRMATION(STR_DELETEMODEL);
          SET_WARNING_INFO(modelHeaders[sub].name, sizeof(g_model.header.name), ZCHAR);
          killEvents(_event_);
          break;
        }
        // no break
      case EVT_KEY_BREAK(KEY_EXIT):
        if (s_copyMode) {
          sub = m_posVert = (s_copyMode == MOVE_MODE || s_copySrcRow<0) ? (MAX_MODELS+sub+s_copyTgtOfs) % MAX_MODELS : s_copySrcRow;
          s_copyMode = 0;
        }
        else {
          if (m_posVert != g_eeGeneral.currModel) {
            m_posVert = g_eeGeneral.currModel;
            s_pgOfs = 0;
          }
          else if (_event_ != EVT_KEY_LONG(KEY_EXIT)) {
            popMenu();
          }
        }
        break;
      case EVT_KEY_LONG(KEY_ENTER):
      case EVT_KEY_BREAK(KEY_ENTER):
        s_editMode = 0;
        if (READ_ONLY()) {
          if (g_eeGeneral.currModel != sub && eeModelExists(sub)) {
            selectModel(sub);
          }
        }
        else if (s_copyMode && (s_copyTgtOfs || s_copySrcRow>=0)) {
          eeCheck(true); // force writing of current model data before this is changed
          uint8_t cur = (MAX_MODELS + sub + s_copyTgtOfs) % MAX_MODELS;
          if (s_copyMode == COPY_MODE) {
            if (!eeCopyModel(cur, s_copySrcRow)) {
              cur = sub;
            }
          }

          s_copySrcRow = g_eeGeneral.currModel; // to update the currModel value
          while (sub != cur) {
            uint8_t src = cur;
            cur = (s_copyTgtOfs > 0 ? cur+MAX_MODELS-1 : cur+1) % MAX_MODELS;
            eeSwapModels(src, cur);
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
          _event_ = EVT_ENTRY_UP;
        }
        else if (_event_ == EVT_KEY_LONG(KEY_ENTER)) {
          s_copyMode = 0;
          killEvents(_event_);
          if (g_eeGeneral.currModel != sub) {
            if (eeModelExists(sub)) {
              MENU_ADD_ITEM(STR_SELECT_MODEL);
              MENU_ADD_SD_ITEM(STR_BACKUP_MODEL);
              MENU_ADD_ITEM(STR_COPY_MODEL);
              MENU_ADD_ITEM(STR_MOVE_MODEL);
              MENU_ADD_ITEM(STR_DELETE_MODEL);
            }
            else {
              MENU_ADD_ITEM(STR_CREATE_MODEL);
              MENU_ADD_ITEM(STR_RESTORE_MODEL);
            }
          }
          else {
            MENU_ADD_SD_ITEM(STR_BACKUP_MODEL);
            MENU_ADD_ITEM(STR_COPY_MODEL);
            MENU_ADD_ITEM(STR_MOVE_MODEL);
          }
          menuHandler = onModelSelectMenu;
        }
        else if (eeModelExists(sub)) {
          s_copyMode = (s_copyMode == COPY_MODE ? MOVE_MODE : COPY_MODE);
          s_copyTgtOfs = 0;
          s_copySrcRow = -1;
        }
        break;

#if 0
      case EVT_KEY_BREAK(KEY_PAGE):
      case EVT_KEY_LONG(KEY_PAGE):
        chainMenu(_event_ == EVT_KEY_BREAK(KEY_PAGE) ? menuModelSetup : menuTabModel[DIM(menuTabModel)-1]);
        killEvents(_event_);
        break;
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
            sub = eeFindEmptyModel(s_copySrcRow, IS_ROTARY_DOWN(_event_) || _event_==EVT_KEY_FIRST(KEY_MOVE_DOWN));
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

  for (int i=0; i<NUM_BODY_LINES; i++) {
    coord_t y = MENU_CONTENT_TOP + i*FH;
    int k = i+s_pgOfs;

    LcdFlags flags = 0;
    if (sub==k && s_copyMode == 0) {
      flags |= TEXT_INVERTED_COLOR;
      lcdDrawFilledRect(MENU_TITLE_LEFT-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN, 150, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
    }

    char str[20];
    strAppendDigits(str, k+1);
    lcd_putsAtt(MENU_TITLE_LEFT, y, str, flags);

    if (s_copyMode == MOVE_MODE || (s_copyMode == COPY_MODE && s_copySrcRow >= 0)) {
      if (k == sub) {
        if (s_copyMode == COPY_MODE) {
          k = s_copySrcRow;
          // lcd_rect(MENU_TITLE_LEFT-5, y-3, 170, 16, SOLID, YELLOW);
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
      strcat_modelname(str, k);
      lcd_putsAtt(POS_MODEL_NAME, y, str, flags);
      if (k==g_eeGeneral.currModel && (s_copyMode!=COPY_MODE || s_copySrcRow<0 || i+s_pgOfs!=(vertpos_t)sub)) {
        lcd_puts(1, y, "*");
      }
    }
    else {
      lcd_putsAtt(POS_MODEL_NAME, y, "---", flags);
    }

    if (s_copyMode && (vertpos_t)sub==i+s_pgOfs) {
      lcd_rect(MENU_TITLE_LEFT-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN, 150, INVERT_LINE_HEIGHT, s_copyMode == COPY_MODE ? SOLID : DOTTED, WARNING_COLOR);
    }
  }

  // TODO lcdDrawBitmap(200, 67, logos, 60*g_model.header.bitmap, 60);
}
