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
#include "menu_screens.h"
#include "screen_theme.h"
#include "screen_setup.h"

#if 0 // The add button
menuPageCount = updateMainviewsMenu();

  if (event == EVT_KEY_FIRST(KEY_ENTER) && getRegisteredLayouts().size()) {
    const LayoutFactory * lf = getRegisteredLayouts().front();
    customScreens[menuPageCount-2] = lf->create(&g_model.screenData[menuPageCount-2].layoutData);
    strncpy(g_model.screenData[menuPageCount-2].layoutName, lf->getName(), sizeof(g_model.screenData[menuPageCount-2].layoutName));
    s_editMode = 0;
    menuHorizontalPosition = -1;
    killEvents(KEY_ENTER);
    storageDirty(EE_MODEL);
    return false;
  }

  SIMPLE_MENU_WITH_OPTIONS(STR_ADDMAINVIEW, THEME_ICONS, menuTabScreensSetup, menuPageCount, menuPageCount-1, 0);
  return true;
#endif

ScreensMenu::ScreensMenu():
  TabsGroup()
{
  addTab(new ScreenThemePage());
  for (int index=0; index<MAX_CUSTOM_SCREENS; index++) {
    auto customScreen = customScreens[index];
    if (customScreen) {
      auto page = new ScreenSetupPage(index);
      page->setOnPageDestroyedHandler([=]() {
        removeTab(1 + index);
      });
      addTab(page);
    }
    else {
      // addTab(new ScreenAddPage(index));
      break;
    }
  }
}
