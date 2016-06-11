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
    lcdDrawSolidFilledRect(0, y-INVERT_VERT_MARGIN, CATEGORIES_WIDTH-1, INVERT_LINE_HEIGHT+2, HEADER_BGCOLOR);
    lcdDrawBitmapPattern(CATEGORIES_WIDTH-14, y+2, LBM_LIBRARY_CURSOR, MENU_TITLE_COLOR);
    if (selectMode == MODE_SELECT_CATEGORY) {
      drawShadow(0, y-INVERT_VERT_MARGIN, CATEGORIES_WIDTH-2, INVERT_LINE_HEIGHT+2);
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

uint16_t categoriesVerticalOffset = 0;
uint16_t categoriesVerticalPosition = 0;
uint16_t modelsVerticalOffset = 0;
uint16_t modelsVerticalPosition = 0;
uint16_t modelsHorizontalPosition = 0;
#define MODEL_INDEX()       (modelsVerticalPosition*2+modelsHorizontalPosition)

void setCurrentCategory(unsigned int index)
{
  currentCategoryIndex = index;
  std::list<ModelsCategory *>::iterator it = modelslist.categories.begin();
  std::advance(it, index);
  currentCategory = *it;
  categoriesVerticalPosition = index;
  categoriesVerticalOffset = limit<int>(categoriesVerticalPosition-9, categoriesVerticalOffset, min<int>(categoriesVerticalPosition, max<int>(0, modelslist.categories.size()-10)));
}

void setCurrentModel(unsigned int index)
{
  std::list<ModelCell *>::iterator it = currentCategory->begin();
  std::advance(it, index);
  currentModel = *it;
  modelsVerticalPosition = index / 2;
  modelsHorizontalPosition = index & 1;
  modelsVerticalOffset = limit<int>(modelsVerticalPosition-2, modelsVerticalOffset, min<int>(modelsVerticalPosition, max<int>(0, (currentCategory->size()-5)/2)));
}

void onCategorySelectMenu(const char * result)
{
  if (result == STR_CREATE_MODEL) {
    storageCheck(true);
    currentModel = modelslist.addModel(currentCategory, createModel());
    selectMode = MODE_SELECT_MODEL;
    setCurrentModel(currentCategory->size() - 1);
  }
  else if (result == STR_CREATE_CATEGORY) {
    currentCategory = modelslist.createCategory();
    setCurrentCategory(modelslist.categories.size() - 1);
  }
  else if (result == STR_RENAME_CATEGORY) {
    selectMode = MODE_RENAME_CATEGORY;
    s_editMode = EDIT_MODIFY_STRING;
    editNameCursorPos = 0;
  }
  else if (result == STR_DELETE_CATEGORY) {
    modelslist.removeCategory(currentCategory);
    modelslist.save();
    setCurrentCategory(currentCategoryIndex-1);
  }
}

void onModelSelectMenu(const char * result)
{
  if (result == STR_SELECT_MODEL) {
    storageDirty(EE_GENERAL);
    storageCheck(true);
    loadModel(g_eeGeneral.currModelFilename);
    chainMenu(menuMainView);
  }
  else if (result == STR_DELETE_MODEL) {
    POPUP_CONFIRMATION(STR_DELETEMODEL);
    SET_WARNING_INFO(currentModel->name, LEN_MODEL_FILENAME, 0);
  }
  else if (result == STR_CREATE_MODEL) {
    storageCheck(true);
    currentModel = modelslist.currentModel = modelslist.addModel(currentCategory, createModel());
    selectMode = MODE_SELECT_MODEL;
    setCurrentModel(currentCategory->size() - 1);
  }
  else if (result == STR_DUPLICATE_MODEL) {
    char duplicatedFilename[LEN_MODEL_FILENAME+1];
    memcpy(duplicatedFilename, currentModel->name, sizeof(duplicatedFilename));
    if (findNextFileIndex(duplicatedFilename, MODELS_PATH)) {
      sdCopyFile(currentModel->name, MODELS_PATH, duplicatedFilename, MODELS_PATH);
      modelslist.addModel(currentCategory, duplicatedFilename);
      unsigned int index = currentCategory->size() - 1;
      setCurrentModel(index);
    }
    else {
      POPUP_WARNING("Invalid File");
    }
  }
}

void initModelsList()
{
  modelslist.load();

  categoriesVerticalOffset = 0;
  bool found = false;
  int index = 0;
  for (std::list<ModelsCategory *>::iterator it = modelslist.categories.begin(); it != modelslist.categories.end(); ++it, ++index) {
    if (*it == modelslist.currentCategory) {
      setCurrentCategory(index);
      found = true;
      break;
    }
  }
  if (!found) {
    setCurrentCategory(0);
  }

  modelsVerticalOffset = 0;
  found = false;
  index = 0;
  for (ModelsCategory::iterator it = currentCategory->begin(); it != currentCategory->end(); ++it, ++index) {
    if (*it == modelslist.currentModel) {
      setCurrentModel(index);
      found = true;
      break;
    }
  }
  if (!found) {
    setCurrentModel(0);
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
    }
    setCurrentModel(modelIndex);
  }

  switch(event) {
    case 0:
      // no need to refresh the screen
      return false;

    case EVT_ENTRY:
      selectMode = MODE_SELECT_MODEL;
      initModelsList();
      break;

    case EVT_KEY_FIRST(KEY_EXIT):
      switch (selectMode) {
        case MODE_SELECT_CATEGORY:
          chainMenu(menuMainView);
          return false;
        case MODE_SELECT_MODEL:
          selectMode = MODE_SELECT_CATEGORY;
          break;
      }
      break;

    case EVT_KEY_BREAK(KEY_ENTER):
    case EVT_KEY_FIRST(KEY_RIGHT):
      if (selectMode == MODE_SELECT_CATEGORY && currentCategory->size() > 0) {
        selectMode = MODE_SELECT_MODEL;
        setCurrentModel(0);
      }
      break;

    case EVT_KEY_FIRST(KEY_LEFT):
      if (selectMode == MODE_SELECT_MODEL) {
        selectMode = MODE_SELECT_CATEGORY;
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
          if (currentModel != modelslist.currentModel) {
            POPUP_MENU_ADD_ITEM(STR_SELECT_MODEL);
          }
          POPUP_MENU_ADD_ITEM(STR_CREATE_MODEL);
          POPUP_MENU_ADD_ITEM(STR_DUPLICATE_MODEL);
        }
        // POPUP_MENU_ADD_SD_ITEM(STR_BACKUP_MODEL);
        // POPUP_MENU_ADD_ITEM(STR_MOVE_MODEL);
        // POPUP_MENU_ADD_ITEM(STR_CHANGE_CATEGORY);
        if (currentModel != modelslist.currentModel) {
          POPUP_MENU_ADD_ITEM(STR_DELETE_MODEL);
        }
        // POPUP_MENU_ADD_ITEM(STR_RESTORE_MODEL);
        POPUP_MENU_START(onModelSelectMenu);
      }
      break;
  }

  if (currentCategory && currentCategory->size() == 0) {
    selectMode = MODE_SELECT_CATEGORY;
    currentModel = NULL;
  }

  // Header
  theme->drawTopbarBackground(ICON_LIBRARY);

  // Body
  lcdDrawSolidFilledRect(0, MENU_HEADER_HEIGHT, CATEGORIES_WIDTH, LCD_H-MENU_HEADER_HEIGHT-MENU_FOOTER_HEIGHT, TEXT_DISABLE_COLOR);
  lcdDrawSolidFilledRect(CATEGORIES_WIDTH, MENU_HEADER_HEIGHT, LCD_W-CATEGORIES_WIDTH, LCD_H-MENU_HEADER_HEIGHT-MENU_FOOTER_HEIGHT, TEXT_BGCOLOR);

  // Footer
  lcdDrawSolidFilledRect(0, MENU_FOOTER_TOP, LCD_W, MENU_FOOTER_HEIGHT, HEADER_BGCOLOR);

  // Categories
  int index = 0;
  coord_t y = MENU_HEADER_HEIGHT+3;
  drawVerticalScrollbar(CATEGORIES_WIDTH-1, MENU_HEADER_HEIGHT, MENU_FOOTER_TOP-MENU_HEADER_HEIGHT, categoriesVerticalOffset, modelslist.categories.size(), 10);
  for (std::list<ModelsCategory *>::iterator it = modelslist.categories.begin(); it != modelslist.categories.end(); ++it, ++index) {
    if (index >= categoriesVerticalOffset && index < categoriesVerticalOffset+10) {
      if (selectMode == MODE_RENAME_CATEGORY && currentCategory == *it) {
        lcdDrawSolidFilledRect(0, y-INVERT_VERT_MARGIN+1, CATEGORIES_WIDTH-2, INVERT_LINE_HEIGHT, TEXT_BGCOLOR);
        editName(MENUS_MARGIN_LEFT, y, currentCategory->name, LEN_MODEL_FILENAME, event, 1, 0);
        if (s_editMode == 0 || event == EVT_KEY_BREAK(KEY_EXIT)) {
          modelslist.save();
          selectMode = MODE_SELECT_CATEGORY;
          putEvent(EVT_REFRESH);
        }
      }
      else {
        drawCategory(y, (*it)->name, currentCategory==*it);
      }
      y += FH;
    }
  }

  if (selectMode == MODE_SELECT_CATEGORY) {
    menuVerticalPosition = categoriesVerticalPosition;
    menuVerticalOffset = categoriesVerticalOffset;
    if (navigate(event, modelslist.categories.size(), 10)) {
      categoriesVerticalPosition = menuVerticalPosition;
      categoriesVerticalOffset = menuVerticalOffset;
      putEvent(EVT_REFRESH);
      setCurrentCategory(categoriesVerticalPosition);
    }
  }

  // Models
  index = 0;
  y = MENU_HEADER_HEIGHT+7;
  for (ModelsCategory::iterator it = currentCategory->begin(); it != currentCategory->end(); ++it, ++index) {
    if (index >= modelsVerticalOffset*2 && index < (modelsVerticalOffset+3)*2) {
      bool selected = (selectMode==MODE_SELECT_MODEL && index==modelsVerticalPosition*2+modelsHorizontalPosition);
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
    menuVerticalPosition = modelsVerticalPosition;
    menuHorizontalPosition = modelsHorizontalPosition;
    menuVerticalOffset = modelsVerticalOffset;
    if (navigate(event, index, 3, 2)) {
      modelsVerticalPosition = menuVerticalPosition;
      modelsHorizontalPosition = menuHorizontalPosition;
      modelsVerticalOffset = menuVerticalOffset;
      putEvent(EVT_REFRESH);
      setCurrentModel(MODEL_INDEX());
    }
    if (currentModel) {
      lcd->drawText(CATEGORIES_WIDTH+MENUS_MARGIN_LEFT, MENU_FOOTER_TOP+2, currentModel->name, TEXT_INVERTED_COLOR);
    }
  }

  drawVerticalScrollbar(DEFAULT_SCROLLBAR_X, MENU_HEADER_HEIGHT+7, MENU_FOOTER_TOP-MENU_HEADER_HEIGHT-15, modelsVerticalOffset, (index+1)/2, 3);

  return true;
}
