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

#include "screen_setup.h"
#include "widgets_setup.h"
#include "opentx.h"
#include "libopenui.h"

#define SET_DIRTY() storageDirty(EE_MODEL)

int getOptionsCount(const ZoneOption * options); // TODO move this
Window * createOptionEdit(Window * parent, const rect_t &rect, const ZoneOption * option, ZoneOptionValue * value);

ScreenSetupPage::ScreenSetupPage(uint8_t index) :
  PageTab("Main view " + std::to_string(index + 1), ICON_THEME_VIEW1 + index),
  index(index)
{
}

void ScreenSetupPage::rebuild(Window * window)
{
  window->clear();
  build(window);
}

void ScreenSetupPage::build(Window * window)
{
  Layout * layout = customScreens[index];
  GridLayout grid;
  grid.setLabelWidth(100);
  grid.spacer(8);

  // Layout choice
  auto layouts = getRegisteredLayouts();
  new StaticText(window, grid.getLabelSlot(), STR_LAYOUT);
  layoutChoice = new NumberEdit(window, grid.getFieldSlot(), 0, layouts.size() - 1,
                                [=]() {
                                  int currentIndex = 0;
                                  for (auto factory: getRegisteredLayouts()) {
                                    if (factory == layout->getFactory())
                                      return currentIndex;
                                    ++currentIndex;
                                  }
                                  return 0;
                                });
  layoutChoice->setWidth(55);
  layoutChoice->setHeight(35);
  layoutChoice->setSetValueHandler([=](uint8_t newValue) {
    delete customScreens[index];
    auto it = getRegisteredLayouts().begin();
    std::advance(it, newValue);
    auto factory = *it;
    customScreens[index] = factory->create(&g_model.screenData[index].layoutData);
    strncpy(g_model.screenData[index].layoutName, factory->getName(), LAYOUT_NAME_LEN);
    SET_DIRTY();
    rebuild(window);
    layoutChoice->setFocus();
    NumberKeyboard::instance()->setField(layoutChoice);
  });
  layoutChoice->setDisplayHandler([=](BitmapBuffer * dc, LcdFlags flags, int32_t index) {
    auto it = getRegisteredLayouts().begin();
    std::advance(it, index);
    (*it)->drawThumb(dc, 2, 2, LINE_COLOR);
  });
  grid.nextLine(35);

  // Setup widgets button
  new TextButton(window, grid.getFieldSlot(), STR_SETUP_WIDGETS,
                 [=]() -> uint8_t {
                   // TODO new WidgetsSetupPage(index); (no way to get out of it right now!)
                   return 0;
                 });
  grid.nextLine();

  // Layout options
  const ZoneOption * options = layout->getFactory()->getOptions();
  int optionsCount = getOptionsCount(options);

  for (int i=0; i<optionsCount; i++) {
    const ZoneOption * option = &options[i];
    ZoneOptionValue * value = layout->getOptionValue(i);
    new StaticText(window, grid.getLabelSlot(), option->name);
    createOptionEdit(window, grid.getFieldSlot(), option, value);
    // TODO handler => theme->update();
    grid.nextLine();
  }

  // Delete screen button
  if (index > 0) {
    new TextButton(window, grid.getLineSlot(), STR_REMOVE_SCREEN,
                   [=]() -> uint8_t {
                     delete layout;
                     if (index != MAX_CUSTOM_SCREENS - 1) {
                       memmove(&g_model.screenData[index], &g_model.screenData[index + 1], sizeof(CustomScreenData) * (MAX_CUSTOM_SCREENS - index - 1));
                       memmove(&customScreens[index], &customScreens[index + 1], sizeof(Layout *) * (MAX_CUSTOM_SCREENS - index - 1));
                     }
                     memset(&g_model.screenData[MAX_CUSTOM_SCREENS - 1], 0, sizeof(CustomScreenData));
                     customScreens[MAX_CUSTOM_SCREENS - 1] = NULL;
                     loadCustomScreens();
                     destroy();
                     return 0;
                   });
    grid.nextLine();
  }

  window->setInnerHeight(grid.getWindowHeight());
}

