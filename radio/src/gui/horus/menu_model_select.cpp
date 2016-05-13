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
#include "storage/modelslist.h"

#define CATEGORIES_WIDTH               140

enum ModelSelectMode {
  MODE_SELECT_CATEGORY,
  MODE_SELECT_MODEL,
  MODE_RENAME_CATEGORY,
};

uint8_t selectMode;
ModelsList modelslist;

ModelsCategory * currentCategory;
int currentCategoryIndex;
ModelCell * currentModel;

void drawCategory(coord_t y, const char * name, bool selected)
{
  if (selected) {
    lcdDrawSolidFilledRect(0, y-INVERT_VERT_MARGIN, CATEGORIES_WIDTH, INVERT_LINE_HEIGHT+2, HEADER_BGCOLOR);
    lcdDrawBitmapPattern(CATEGORIES_WIDTH-12, y, LBM_LIBRARY_CURSOR, MENU_TITLE_COLOR);
    if (selectMode == MODE_SELECT_CATEGORY) {
      drawShadow(0, y-INVERT_VERT_MARGIN, CATEGORIES_WIDTH, INVERT_LINE_HEIGHT+2);
    }
  }
  lcdDrawText(MENUS_MARGIN_LEFT, y, name, MENU_TITLE_COLOR);
}

void drawModel(coord_t x, coord_t y, ModelCell * model, bool current, bool selected)
{
  lcd->drawBitmap(x+1, y+1, &model->buffer);
  if (current) {
    lcd->drawBitmapPattern(x+66, y+43, LBM_ACTIVE_MODEL, TITLE_BGCOLOR);
  }
  if (selected) {
    lcdDrawSolidRect(x, y, MODELCELL_WIDTH+2, MODELCELL_HEIGHT+2, 1, TITLE_BGCOLOR);
    drawShadow(x, y, MODELCELL_WIDTH+2, MODELCELL_HEIGHT+2);
  }
}

#define MODEL_INDEX()       (menuVerticalPosition*2+menuHorizontalPosition)

void setCurrentCategory(unsigned int index)
{
  currentCategoryIndex = index;
  std::list<ModelsCategory *>::iterator it = modelslist.categories.begin();
  std::advance(it, index);
  currentCategory = *it;
}

void setCurrentModel(unsigned int index)
{
  std::list<ModelCell *>::iterator it = currentCategory->begin();
  std::advance(it, MODEL_INDEX());
  currentModel = *it;
}

void onCategorySelectMenu(const char * result)
{
  if (result == STR_CREATE_MODEL) {
    storageCheck(true);
    currentModel = modelslist.addModel(currentCategory, createModel());
    selectMode = MODE_SELECT_MODEL;
    menuVerticalPosition = currentCategory->size() - 1;
  }
  else if (result == STR_CREATE_CATEGORY) {
    currentCategory = modelslist.createCategory();
    menuVerticalPosition = currentCategoryIndex = modelslist.categories.size() - 1;
  }
  else if (result == STR_RENAME_CATEGORY) {
    selectMode = MODE_RENAME_CATEGORY;
    s_editMode = EDIT_MODIFY_STRING;
    editNameCursorPos = 0;
  }
  else if (result == STR_DELETE_CATEGORY) {
    modelslist.removeCategory(currentCategory);
    setCurrentCategory(currentCategoryIndex-1);
  }
}

void onModelSelectMenu(const char * result)
{
  if (result == STR_SELECT_MODEL) {
    memcpy(g_eeGeneral.currModelFilename, currentModel->name, LEN_MODEL_FILENAME);
    storageDirty(EE_GENERAL);
    storageCheck(true);
    loadModel(g_eeGeneral.currModelFilename);
    chainMenu(menuMainView);
  }
  else if (result == STR_DELETE_MODEL) {
    POPUP_CONFIRMATION(STR_DELETEMODEL);
    SET_WARNING_INFO(currentModel->name, LEN_MODEL_FILENAME, 0);
  }
  else if (result == STR_CHANGE_CATEGORY) {

  }
  else if (result == STR_DUPLICATE_MODEL) {
    char duplicatedFilename[LEN_MODEL_FILENAME+1];
    memcpy(duplicatedFilename, currentModel->name, sizeof(duplicatedFilename));
    if (findNextFileIndex(duplicatedFilename, MODELS_PATH)) {
      sdCopyFile(currentModel->name, MODELS_PATH, duplicatedFilename, MODELS_PATH);
      modelslist.addModel(currentCategory, duplicatedFilename);
      menuVerticalPosition = currentCategory->size() - 1;
      setCurrentModel(menuVerticalPosition);
    }
    else {
      POPUP_WARNING("Invalid File");
    }
  }
}

bool menuModelSelect(evt_t event)
{
  if (warningResult) {
    warningResult = 0;
    int modelIndex = MODEL_INDEX();
    modelslist.removeModel(currentCategory, currentModel);
    s_copyMode = 0;
    event = EVT_REFRESH;
    if (modelIndex > 0) {
      modelIndex--;
      menuVerticalPosition = modelIndex / 2;
      menuHorizontalPosition = modelIndex & 1;
      setCurrentModel(modelIndex);
    }
  }

  switch(event) {
    case 0:
      // no need to refresh the screen
      return false;

    case EVT_ENTRY:
      selectMode = MODE_SELECT_CATEGORY;
      modelslist.load();
      menuVerticalPosition = 0;
      setCurrentCategory(0);
      setCurrentModel(0);
      break;

    case EVT_KEY_FIRST(KEY_EXIT):
      switch (selectMode) {
        case MODE_SELECT_CATEGORY:
          chainMenu(menuMainView);
          return false;
        case MODE_SELECT_MODEL:
          selectMode = MODE_SELECT_CATEGORY;
          menuVerticalPosition = currentCategoryIndex;
          break;
      }
      break;

    case EVT_KEY_BREAK(KEY_ENTER):
    case EVT_KEY_BREAK(KEY_RIGHT):
      switch (selectMode) {
        case MODE_SELECT_CATEGORY:
          if (currentCategory->size() > 0) {
            selectMode = MODE_SELECT_MODEL;
            menuVerticalPosition = 0;
            setCurrentModel(menuVerticalPosition);
          }
          break;
      }
      break;

    case EVT_KEY_LONG(KEY_ENTER):
      if (selectMode == MODE_SELECT_CATEGORY) {
        killEvents(event);
        POPUP_MENU_ADD_ITEM(STR_CREATE_MODEL);
        POPUP_MENU_ADD_ITEM(STR_CREATE_CATEGORY);
        POPUP_MENU_ADD_ITEM(STR_RENAME_CATEGORY);
        if (currentCategoryIndex > 0) {
          POPUP_MENU_ADD_ITEM(STR_DELETE_CATEGORY);
        }
        POPUP_MENU_START(onCategorySelectMenu);
      }
      else if (selectMode == MODE_SELECT_MODEL) {
        killEvents(event);
        ModelHeader header;
        const char * error = readModel(currentModel->name, (uint8_t *)&header, sizeof(header));
        if (!error) {
          if (strncmp(currentModel->name, g_eeGeneral.currModelFilename, LEN_MODEL_FILENAME) != 0) {
            POPUP_MENU_ADD_ITEM(STR_SELECT_MODEL);
          }
          POPUP_MENU_ADD_ITEM(STR_DUPLICATE_MODEL);
        }
        // POPUP_MENU_ADD_SD_ITEM(STR_BACKUP_MODEL);
        // POPUP_MENU_ADD_ITEM(STR_MOVE_MODEL);
        POPUP_MENU_ADD_ITEM(STR_CHANGE_CATEGORY);
        POPUP_MENU_ADD_ITEM(STR_DELETE_MODEL);
        // POPUP_MENU_ADD_ITEM(STR_CREATE_MODEL);
        // POPUP_MENU_ADD_ITEM(STR_RESTORE_MODEL);
        POPUP_MENU_START(onModelSelectMenu);
      }
      break;
  }

  // Header
  theme->drawTopbarBackground(ICON_LIBRARY);

  // Body
  lcdDrawSolidFilledRect(0, MENU_HEADER_HEIGHT, CATEGORIES_WIDTH, LCD_H-MENU_HEADER_HEIGHT-MENU_FOOTER_HEIGHT, TITLE_BGCOLOR);
  lcdDrawSolidFilledRect(CATEGORIES_WIDTH, MENU_HEADER_HEIGHT, LCD_W-CATEGORIES_WIDTH, LCD_H-MENU_HEADER_HEIGHT-MENU_FOOTER_HEIGHT, TEXT_BGCOLOR);

  // Footer
  lcdDrawSolidFilledRect(0, MENU_FOOTER_TOP, LCD_W, MENU_FOOTER_HEIGHT, HEADER_BGCOLOR);

  // Categories
  coord_t y = MENU_HEADER_HEIGHT+4;
  for (std::list<ModelsCategory *>::iterator it = modelslist.categories.begin(); it != modelslist.categories.end(); ++it, y+=FH) {
    if (y < MENU_FOOTER_TOP - 5) {
      if (selectMode == MODE_RENAME_CATEGORY && currentCategory == *it) {
        lcdDrawSolidFilledRect(0, y-INVERT_VERT_MARGIN, CATEGORIES_WIDTH, INVERT_LINE_HEIGHT, TEXT_BGCOLOR);
        editName(MENUS_MARGIN_LEFT, y, currentCategory->name, LEN_MODEL_FILENAME, event, 1, 0);
        if (s_editMode == 0 || event == EVT_KEY_BREAK(KEY_EXIT)) {
          // storageRenameCategory(currentCategory, selectedCategory);
          selectMode = MODE_SELECT_CATEGORY;
          putEvent(EVT_REFRESH);
        }
      }
      else {
        drawCategory(y, (*it)->name, currentCategory==*it);
      }
    }
  }

  if (selectMode == MODE_SELECT_CATEGORY) {
    if (navigate(event, modelslist.categories.size(), 9)) {
      putEvent(EVT_REFRESH);
      setCurrentCategory(menuVerticalPosition);
    }
  }

  // Models
  int index = 0;
  y = MENU_HEADER_HEIGHT+7;
  for (ModelsCategory::iterator it = currentCategory->begin(); it != currentCategory->end(); ++it, ++index) {
    if (index >= menuVerticalOffset*2 && index < (menuVerticalOffset+3)*2) {
      bool selected = (selectMode==MODE_SELECT_MODEL && index==menuVerticalPosition*2+menuHorizontalPosition);
      bool current = !strncmp((*it)->name, g_eeGeneral.currModelFilename, LEN_MODEL_FILENAME);
      if (index & 1) {
        drawModel(CATEGORIES_WIDTH+MENUS_MARGIN_LEFT+162, y, *it, current, selected);
        y += 66;
      }
      else {
        drawModel(CATEGORIES_WIDTH+MENUS_MARGIN_LEFT+1, y, *it, current, selected);
      }
    }
  }
  if (selectMode == MODE_SELECT_MODEL) {
    if (index == 0) {
      selectMode = MODE_SELECT_CATEGORY;
      menuVerticalPosition = currentCategoryIndex;
      putEvent(EVT_REFRESH);
    }
    else if (navigate(event, index, 3, 2)) {
      putEvent(EVT_REFRESH);
      setCurrentModel(MODEL_INDEX());
    }
    if (currentModel) {
      lcd->drawText(CATEGORIES_WIDTH+MENUS_MARGIN_LEFT, MENU_FOOTER_TOP+2, currentModel->name, TEXT_INVERTED_COLOR);
    }
  }

  drawVerticalScrollbar(DEFAULT_SCROLLBAR_X, MENU_HEADER_HEIGHT+7, MENU_FOOTER_TOP-MENU_HEADER_HEIGHT-15, menuVerticalOffset, (index+1)/2, 3);

  return true;
}
