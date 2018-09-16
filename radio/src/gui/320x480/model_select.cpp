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

#include <algorithm>
#include "model_select.h"
#include "opentx.h"
#include "libwindows.h"
#include "storage/modelslist.h"

#define CATEGORIES_WIDTH               120
#define MODELS_LEFT                    123
#define MODELS_COLUMN_WIDTH            174

enum ModelSelectMode {
  MODE_SELECT_MODEL,
  MODE_RENAME_CATEGORY,
  MODE_MOVE_MODEL,
};

enum ModelDeleteMode {
  MODE_DELETE_MODEL,
  MODE_DELETE_CATEGORY,
};

uint8_t selectMode, deleteMode;
ModelsList modelslist;

ModelsCategory * currentCategory;
int currentCategoryIndex;
ModelCell * currentModel;

void drawCategory(coord_t y, const char * name, bool selected)
{
  if (selected) {
    lcdDrawSolidFilledRect(1, y-INVERT_VERT_MARGIN, CATEGORIES_WIDTH-10, INVERT_LINE_HEIGHT+2, TEXT_INVERTED_BGCOLOR);
    lcdDrawText(6, y, name, TEXT_COLOR | INVERS);
  }
  else {
    lcdDrawText(6, y, name, TEXT_COLOR);
  }
}

void drawModel(coord_t x, coord_t y, ModelCell * model, bool current, bool selected)
{
  lcd->drawBitmap(x+1, y+1, model->getBuffer());
  if (current) {
    lcd->drawBitmapPattern(x+66, y+43, LBM_ACTIVE_MODEL, TITLE_BGCOLOR);
  }
  if (selected) {
    lcdDrawSolidRect(x, y, MODELCELL_WIDTH+2, MODELCELL_HEIGHT+2, 1, TITLE_BGCOLOR);
    drawShadow(x, y, MODELCELL_WIDTH+2, MODELCELL_HEIGHT+2);
    if (selectMode == MODE_MOVE_MODEL) {
      lcd->drawMask(x+MODELCELL_WIDTH+2-modelselModelMoveBackground->getWidth(), y, modelselModelMoveBackground, TITLE_BGCOLOR);
      lcd->drawMask(x+MODELCELL_WIDTH+2-modelselModelMoveBackground->getWidth()+12, y+5, modelselModelMoveIcon, TEXT_BGCOLOR);
    }
  }
}

uint16_t categoriesVerticalOffset = 0;
uint16_t categoriesVerticalPosition = 0;
#define MODEL_INDEX()       (menuVerticalPosition*2+menuHorizontalPosition)

#if 0
void setCurrentModel(unsigned int index)
{
  auto it = currentCategory->begin();
  std::advance(it, index);
  currentModel = *it;
}
#endif


void setCurrentCategory(unsigned int index)
{
  currentCategoryIndex = index;
  std::list<ModelsCategory *>::iterator it = modelslist.categories.begin();
  std::advance(it, index);
  currentCategory = *it;
  categoriesVerticalPosition = index;
  categoriesVerticalOffset = limit<int>(categoriesVerticalPosition-4, categoriesVerticalOffset, min<int>(categoriesVerticalPosition, max<int>(0, modelslist.categories.size()-5)));
  /*if (currentCategory->size() > 0)
    setCurrentModel(0);
  else
    currentModel = NULL;*/
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

  menuVerticalOffset = 0;
  found = false;
  index = 0;
  for (ModelsCategory::iterator it = currentCategory->begin(); it != currentCategory->end(); ++it, ++index) {
    if (*it == modelslist.currentModel) {
      // setCurrentModel(index);
      found = true;
      break;
    }
  }
  if (!found) {
    // setCurrentModel(0);
  }
}

class ModelselectButton: public Button {
  public:
    ModelselectButton(Window * parent, const rect_t & rect, ModelCell * modelCell, Window * footer);

    virtual void paint(BitmapBuffer * dc) override {
      drawSolidRect(dc, 0, 0, rect.w, rect.h, 2, hasFocus() ? SCROLLBOX_COLOR : CURVE_AXIS_COLOR);
      dc->drawBitmap(10, 2, modelCell->getBuffer());
      if (modelCell == modelslist.currentModel) {
        dc->drawBitmapPattern(112, 71, LBM_ACTIVE_MODEL, TITLE_BGCOLOR);
      }
    }

    const char * modelFilename() {
      return modelCell->modelFilename;
    }

  protected:
    ModelCell * modelCell;
};

class ModelselectFooter: public Window {
  public:
    ModelselectFooter(Window * parent, const rect_t & rect):
      Window(parent, rect)
    {
    }

    virtual void paint(BitmapBuffer * dc) override {
      dc->drawSolidFilledRect(0, 5, rect.w, 2, CURVE_AXIS_COLOR);
      dc->drawBitmap(7, 12, modelselSdFreeBitmap);
      uint32_t size = sdGetSize() / 100;
      drawNumber(dc, 24, 11, size, PREC1|SMLSIZE, 0, NULL, "GB");
      dc->drawBitmap(77, 12, modelselModelQtyBitmap);
      drawNumber(dc, 99, 11, modelslist.modelsCount, SMLSIZE);
      ModelselectButton * selectedModel = dynamic_cast<ModelselectButton *>(focusWindow);
      if (selectedModel) {
        dc->drawBitmap(7, 37, modelselModelNameBitmap);
        dc->drawText(24, 32, selectedModel->modelFilename(), SMLSIZE | TEXT_COLOR);
      }
    }
};

class ModelselectPage: public PageTab {
  public:
    ModelselectPage() :
      PageTab(STR_MODEL_SELECT, ICON_MODEL_SELECT)
    {
    }

    static void updateModels(Window * window, Window * footer, int selected=-1) {
      window->clear();
      int index = 0;
      for (auto it = currentCategory->begin(); it != currentCategory->end(); ++it, ++index) {
        Button * button = new ModelselectButton(window, {10, 10 + index * 104, LCD_W - 20, 94}, *it, footer);
        if (selected == index) {
          button->setFocus();
        }
      }
      window->adjustInnerHeight();
    }

    virtual void build(Window * window) override
    {
      initModelsList();
      Window * body = new Window(window, {0, 0, LCD_W, window->height() - 55});
      Window * footer = new ModelselectFooter(window, {0, window->height() - 55, LCD_W, 55});
      updateModels(body, footer);
    }
};


ModelselectMenu::ModelselectMenu():
  TabsGroup()
{
  addTab(new ModelselectPage());
}

ModelselectButton::ModelselectButton(Window * parent, const rect_t & rect, ModelCell * modelCell, Window * footer):
  Button(parent, rect,
         [=]() -> uint8_t {
           if (hasFocus()) {
             Menu * menu = new Menu();
             if (modelCell && modelCell != modelslist.currentModel) {
               menu->addLine(STR_SELECT_MODEL, [=]() {
                 // we store the latest changes if any
                 storageFlushCurrentModel();
                 storageCheck(true);
                 memcpy(g_eeGeneral.currModelFilename, modelCell->modelFilename, LEN_MODEL_FILENAME);
                 loadModel(g_eeGeneral.currModelFilename, false);
                 storageDirty(EE_GENERAL);
                 storageCheck(true);
                 // chainMenu(menuMainView);
                 postModelLoad(true);
                 modelslist.setCurrentModel(modelCell);
                 ModelselectPage::updateModels(parent, footer, modelslist.getModelIndex(modelCell));
               });
             }
             menu->addLine(STR_CREATE_MODEL, [=]() {
               storageCheck(true);
               modelslist.setCurrentModel(modelslist.addModel(currentCategory, createModel()));
#if defined(LUA)
               // chainMenu(menuModelWizard);
#endif
               ModelselectPage::updateModels(parent, footer, currentCategory->size() - 1);
             });
             if (modelCell) {
               menu->addLine(STR_DUPLICATE_MODEL, [=]() {
                 char duplicatedFilename[LEN_MODEL_FILENAME + 1];
                 memcpy(duplicatedFilename, modelCell->modelFilename, sizeof(duplicatedFilename));
                 if (findNextFileIndex(duplicatedFilename, LEN_MODEL_FILENAME, MODELS_PATH)) {
                   sdCopyFile(modelCell->modelFilename, MODELS_PATH, duplicatedFilename, MODELS_PATH);
                   modelslist.addModel(currentCategory, duplicatedFilename);
                   ModelselectPage::updateModels(parent, footer, currentCategory->size() - 1);
                 }
                 else {
                   POPUP_WARNING("Invalid File");
                 }
               });
             }
             // menu->addLine(STR_MOVE_MODEL);
             if (modelCell && modelCell != modelslist.currentModel) {
               menu->addLine(STR_DELETE_MODEL, [=]() {
                 // POPUP_CONFIRMATION(STR_DELETEMODEL);
                 // SET_WARNING_INFO(modelCell->modelName, LEN_MODEL_NAME, 0);
                 unsigned int index = modelslist.getModelIndex(modelCell);
                 if (index > 0)
                   --index;
                 modelslist.removeModel(currentCategory, modelCell);
                 ModelselectPage::updateModels(parent, footer, index);
               });
             }
           }
           else {
             setFocus();
             footer->invalidate();
           }
           return 1;
         }, BUTTON_CHECKED_ON_FOCUS),
  modelCell(modelCell)
{
}