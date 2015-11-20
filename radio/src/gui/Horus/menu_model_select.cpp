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

#define CATEGORIES_WIDTH               140

#define MODELCELL_WIDTH                153
#define MODELCELL_HEIGHT               61

enum ModelSelectMode {
  MODE_SELECT_CATEGORY,
  MODE_SELECT_MODEL,
  MODE_RENAME_CATEGORY,
};

uint8_t selectMode;
uint8_t currentCategory;
char selectedFilename[LEN_MODEL_FILENAME+1];
char selectedCategory[LEN_MODEL_FILENAME+1];

void drawCategory(coord_t y, const char * name, bool selected)
{
  if (selected) {
    lcdDrawSolidFilledRect(0, y-INVERT_VERT_MARGIN, CATEGORIES_WIDTH, INVERT_LINE_HEIGHT, HEADER_BGCOLOR);
    lcdDrawBitmapPattern(CATEGORIES_WIDTH-12, y, LBM_LIBRARY_CURSOR, MENU_TITLE_COLOR);
    if (selectMode == MODE_SELECT_CATEGORY) {
      drawShadow(0, y-INVERT_VERT_MARGIN, CATEGORIES_WIDTH, INVERT_LINE_HEIGHT);
    }
  }
  lcdDrawText(MENUS_MARGIN_LEFT, y, name, MENU_TITLE_COLOR);
}

void drawModel(coord_t x, coord_t y, const char * name, bool selected)
{
  ModelHeader header;
  const char * error = readModel(name, (uint8_t *)&header, sizeof(header));
  if (error) {
    lcdDrawText(x+5, y+2, "(Invalid Model)", TEXT_COLOR);
    lcdDrawBitmapPattern(x+5, y+23, LBM_LIBRARY_SLOT, TEXT_COLOR);
  }
  else {
    lcdDrawTextWithLen(x+5, y+2, header.name, LEN_MODEL_NAME, ZCHAR|TEXT_COLOR);
    putsTimer(x+104, y+41, 0, TEXT_COLOR|LEFT);
    for (int i=0; i<4; i++) {
      lcdDrawBitmapPattern(x+104+i*11, y+25, LBM_SCORE0, TITLE_BGCOLOR);
    }
    loadModelBitmap(header.bitmap, modelBitmap);
    lcdDrawBitmap(x+5, y+24, modelBitmap, 0, 0, getBitmapScale(modelBitmap, 64, 32));
  }
  lcdDrawSolidHorizontalLine(x+5, y+19, 143, LINE_COLOR);
  if (selectMode == MODE_SELECT_MODEL && selected) {
    lcdDrawSolidRect(x, y, MODELCELL_WIDTH, MODELCELL_HEIGHT, TITLE_BGCOLOR);
    drawShadow(x, y, MODELCELL_WIDTH, MODELCELL_HEIGHT);
  }
}

void onCategorySelectMenu(const char * result)
{
  if (result == STR_CREATE_CATEGORY) {
    storageAppendCategory("Category");
  }
  else if (result == STR_RENAME_CATEGORY) {
    selectMode = MODE_RENAME_CATEGORY;
  }
  else if (result == STR_DELETE_CATEGORY) {
    storageRemoveCategory(currentCategory--);
  }
}

void onModelSelectMenu(const char * result)
{
  if (result == STR_SELECT_MODEL) {
    memcpy(g_eeGeneral.currModelFilename, selectedFilename, LEN_MODEL_FILENAME);
    storageDirty(EE_GENERAL);
    loadModel(g_eeGeneral.currModelFilename);
    chainMenu(menuMainView);
  }
  else if (result == STR_DELETE_MODEL) {
    POPUP_CONFIRMATION(STR_DELETEMODEL);
    SET_WARNING_INFO(selectedFilename, LEN_MODEL_FILENAME, 0);
  }
  else if (result == STR_DUPLICATE_MODEL) {
    char duplicatedFilename[LEN_MODEL_FILENAME+1];
    memcpy(duplicatedFilename, selectedFilename, sizeof(duplicatedFilename));
    if (findNextFileIndex(duplicatedFilename, MODELS_PATH)) {
      sdCopyFile(selectedFilename, MODELS_PATH, duplicatedFilename, MODELS_PATH);
      storageInsertModel(duplicatedFilename, currentCategory, -1);
    }
    else {
      POPUP_WARNING("Invalid Filename");
    }
  }
}

#define MODEL_INDEX()       (m_posVert*2+m_posHorz)

bool menuModelSelect(evt_t event)
{
  if (s_warning_result) {
    s_warning_result = 0;
    int modelIndex = MODEL_INDEX();
    storageRemoveModel(currentCategory, modelIndex);
    s_copyMode = 0;
    event = EVT_REFRESH;
    if (modelIndex > 0) {
      modelIndex--;
      m_posVert = modelIndex / 2;
      m_posHorz = modelIndex & 1;
    }
  }

  switch(event) {
    case 0:
      // no need to refresh the screen
      return false;

    case EVT_ENTRY:
      selectMode = MODE_SELECT_CATEGORY;
      currentCategory = m_posVert = 0;
      break;

    case EVT_KEY_FIRST(KEY_EXIT):
      switch (selectMode) {
        case MODE_SELECT_CATEGORY:
          chainMenu(menuMainView);
          return false;
        case MODE_SELECT_MODEL:
          selectMode = MODE_SELECT_CATEGORY;
          m_posVert = currentCategory;
          break;
      }
      break;

    case EVT_KEY_BREAK(KEY_ENTER):
    case EVT_KEY_BREAK(KEY_RIGHT):
      switch (selectMode) {
        case MODE_SELECT_CATEGORY:
          selectMode = MODE_SELECT_MODEL;
          m_posVert = 0;
          break;
      }
      break;

    case EVT_KEY_LONG(KEY_ENTER):
      if (selectMode == MODE_SELECT_CATEGORY) {
        killEvents(event);
        MENU_ADD_ITEM(STR_CREATE_CATEGORY);
        MENU_ADD_ITEM(STR_RENAME_CATEGORY);
        if (currentCategory > 0)
          MENU_ADD_ITEM(STR_DELETE_CATEGORY);
        menuHandler = onCategorySelectMenu;
      }
      else if (selectMode == MODE_SELECT_MODEL) {
        killEvents(event);
        ModelHeader header;
        const char * error = readModel(selectedFilename, (uint8_t *)&header, sizeof(header));
        if (!error) {
          MENU_ADD_ITEM(STR_SELECT_MODEL);
          MENU_ADD_ITEM(STR_DUPLICATE_MODEL);
        }
        // MENU_ADD_SD_ITEM(STR_BACKUP_MODEL);
        // MENU_ADD_ITEM(STR_MOVE_MODEL);
        MENU_ADD_ITEM(STR_DELETE_MODEL);
        // MENU_ADD_ITEM(STR_CREATE_MODEL);
        // MENU_ADD_ITEM(STR_RESTORE_MODEL);
        menuHandler = onModelSelectMenu;
      }
      break;
  }

  // Header
  lcdDrawSolidFilledRect(0, 0, LCD_W, MENU_HEADER_HEIGHT, HEADER_BGCOLOR);
  lcdDrawBitmapPattern(0, 0, LBM_TOPMENU_POLYGON, TITLE_BGCOLOR);
  lcdDrawBitmapPattern(5, 7, LBM_LIBRARY_ICON, MENU_TITLE_COLOR);
  drawTopmenuDatetime();

  // Body
  lcdDrawSolidFilledRect(0, MENU_HEADER_HEIGHT, CATEGORIES_WIDTH, LCD_H-MENU_HEADER_HEIGHT-MENU_FOOTER_HEIGHT, TITLE_BGCOLOR);
  lcdDrawSolidFilledRect(CATEGORIES_WIDTH, MENU_HEADER_HEIGHT, LCD_W-CATEGORIES_WIDTH, LCD_H-MENU_HEADER_HEIGHT-MENU_FOOTER_HEIGHT, TEXT_BGCOLOR);

  // Footer
  lcdDrawSolidFilledRect(0, MENU_FOOTER_TOP, LCD_W, MENU_FOOTER_HEIGHT, HEADER_BGCOLOR);

  // Categories
  StorageModelsList storage;
  const char * error = storageOpenModelsList(&storage);
  if (!error) {
    bool result = true;
    coord_t y = MENU_HEADER_HEIGHT+10;
    int index = 0;
    while (1) {
      char line[LEN_MODEL_FILENAME+1];
      result = storageReadNextCategory(&storage, line, LEN_MODEL_FILENAME);
      if (!result)
        break;
      if (y < LCD_H) {
        if (selectMode == MODE_RENAME_CATEGORY && currentCategory == index) {
          s_editMode = EDIT_MODIFY_STRING;
          lcdDrawSolidFilledRect(0, y-INVERT_VERT_MARGIN, CATEGORIES_WIDTH, INVERT_LINE_HEIGHT, TEXT_BGCOLOR);
          editName(MENUS_MARGIN_LEFT, y, selectedCategory, LEN_MODEL_FILENAME, event, 1, 0);
          if (s_editMode == 0 || event == EVT_KEY_BREAK(KEY_EXIT)) {
            storageRenameCategory(currentCategory, selectedCategory);
            selectMode = MODE_SELECT_CATEGORY;
            putEvent(EVT_REFRESH);
          }
        }
        else {
          if (currentCategory == index) {
            memcpy(selectedCategory, line, sizeof(selectedCategory));
          }
          drawCategory(y, line, currentCategory==index);
        }
      }


      y += FH;
      index++;
    }
    if (selectMode == MODE_SELECT_CATEGORY) {
      if (navigate(event, index, 9)) {
        putEvent(EVT_REFRESH);
        currentCategory = m_posVert;
      }
    }
  }

  // Models
  if (!error) {
    bool result = storageSeekCategory(&storage, currentCategory);
    coord_t y = MENU_HEADER_HEIGHT+7;
    int index = 0;
    while (result) {
      char line[LEN_MODEL_FILENAME+1];
      result = storageReadNextModel(&storage, line, LEN_MODEL_FILENAME);
      if (!result)
        break;
      if (index >= s_pgOfs*2 && index < (s_pgOfs+3)*2) {
        bool selected = (m_posVert*2+m_posHorz==index);
        if (index & 1) {
          drawModel(CATEGORIES_WIDTH+MENUS_MARGIN_LEFT+162, y, line, selected);
          y += 66;
        }
        else {
          drawModel(CATEGORIES_WIDTH+MENUS_MARGIN_LEFT+1, y, line, selected);
        }
        if (selected) {
          memcpy(selectedFilename, line, sizeof(selectedFilename));
        }
      }
      index++;
    }
    if (selectMode == MODE_SELECT_MODEL) {
      if (navigate(event, index, 3, 2)) {
        putEvent(EVT_REFRESH);
      }
    }
    drawVerticalScrollbar(DEFAULT_SCROLLBAR_X, MENU_HEADER_HEIGHT+7, MENU_FOOTER_TOP-MENU_HEADER_HEIGHT-15, s_pgOfs, (index+1)/2, 3);
  }

  return true;
}
