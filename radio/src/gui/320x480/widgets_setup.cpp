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

#include "widgets_setup.h"
#include "opentx.h"

WidgetsSetupPage::WidgetsSetupPage(uint8_t index):
  ViewMain(false),
  index(index)
{
}

bool WidgetsSetupPage::onTouchEnd(coord_t x, coord_t y)
{
  return true;
}

bool WidgetsSetupPage::onTouchSlide(coord_t x, coord_t y, coord_t startX, coord_t startY, coord_t slideX, coord_t slideY)
{
  return true;
}

void WidgetsSetupPage::paint(BitmapBuffer * dc)
{
  ViewMain::paint(dc);
  dc->drawFilledRect(0, 0, LCD_W, LCD_H, SOLID, TEXT_COLOR | OPACITY(8));

  WidgetsContainerInterface * layout = customScreens[index];

  for (int i=layout->getZonesCount()-1; i>=0; i--) {
    Zone zone = layout->getZone(i);
    LcdFlags color;
    int padding, thickness;
    if (layout == topbar) {
      color = MENU_TITLE_COLOR;
      padding = 2;
      thickness = 1;
    }
    else {
      color = TEXT_INVERTED_BGCOLOR;
      padding = 4;
      thickness = 2;
    }
    if (0 == i) {
      lcdDrawSolidRect(zone.x-padding, zone.y-padding, zone.w+2*padding, zone.h+2*padding, thickness, color);
      /*if (event == EVT_KEY_FIRST(KEY_ENTER)) {
        killEvents(KEY_ENTER);
        currentZone = menuVerticalPosition;
        currentWidget = currentContainer->getWidget(menuVerticalPosition);
        if (currentWidget) {
          POPUP_MENU_ADD_ITEM(STR_SELECT_WIDGET);
          if (currentWidget->getFactory()->getOptions())
            POPUP_MENU_ADD_ITEM(STR_WIDGET_SETTINGS);
          POPUP_MENU_ADD_ITEM(STR_REMOVE_WIDGET);
          POPUP_MENU_START(onZoneMenu);
        }
        else {
          onZoneMenu(STR_SELECT_WIDGET);
        }
      }*/
    }
    else {
      /*if (widgetNeedsSettings) {
        currentWidget = currentContainer->getWidget(menuVerticalPosition);
        if (currentWidget) {
          widgetNeedsSettings = false;
          onZoneMenu(STR_WIDGET_SETTINGS);
        }
      }*/
      lcdDrawRect(zone.x-padding, zone.y-padding, zone.w+2*padding, zone.h+2*padding, thickness, 0x3F, color);
    }
  }
}

