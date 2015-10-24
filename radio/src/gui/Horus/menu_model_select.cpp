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
    storageCheck(true); // force writing of current model data before this is changed
    POPUP_WARNING(eeBackupModel(sub));
  }
  else if (result == STR_RESTORE_MODEL || result == STR_UPDATE_LIST) {
    if (!sdListFiles(MODELS_PATH, MODELS_EXT, MENU_LINE_LENGTH-1, NULL)) {
      POPUP_WARNING(STR_NO_MODELS_ON_SD);
      s_menu_flags = 0;
    }
  }
  else if (result == STR_DELETE_MODEL) {
    POPUP_CONFIRMATION(STR_DELETEMODEL);
    // SET_WARNING_INFO(modelHeaders[sub].name, sizeof(g_model.header.name), ZCHAR);
  }
  else {
    // The user choosed a file on SD to restore
    POPUP_WARNING(eeRestoreModel(sub, (char *)result));
    // if (!s_warning && g_eeGeneral.currModel == sub)
    //   eeLoadModel(sub);
  }
}

char *strAppendModelSize(char *dest, int size)
{
  char s[8] = "---";
  if (size) sprintf(s, "%d", size);
  dest = strSetCursor(dest, POS_MODEL_SIZE-getTextWidth(s));
  return strAppend(dest, s);
}

#define CATEGORIES_WIDTH 140

void menuModelSelect(evt_t event)
{
  // Header
  lcdDrawSolidFilledRect(0, 0, LCD_W, MENU_HEADER_HEIGHT, HEADER_BGCOLOR);
  lcdDrawBitmapPattern(0, 0, LBM_TOPMENU_POLYGON, TITLE_BGCOLOR);
  // lcdDrawBitmapPattern(4, 10, LBM_TOPMENU_OPENTX, MENU_TITLE_COLOR);
  drawTopmenuDatetime();

  // Categories
  lcdDrawSolidFilledRect(0, MENU_HEADER_HEIGHT, CATEGORIES_WIDTH, LCD_H-MENU_HEADER_HEIGHT-MENU_FOOTER_HEIGHT, TITLE_BGCOLOR);

  // Models
  lcdDrawSolidFilledRect(CATEGORIES_WIDTH, MENU_HEADER_HEIGHT, LCD_W-CATEGORIES_WIDTH, LCD_H-MENU_HEADER_HEIGHT-MENU_FOOTER_HEIGHT, TEXT_BGCOLOR);

  // Footer
  lcdDrawSolidFilledRect(0, MENU_FOOTER_TOP, LCD_W, MENU_FOOTER_HEIGHT, HEADER_BGCOLOR);
}
