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
#include "tp_gt911.h"

bool menuRadioDiagTouch(event_t event)
{
  static bool touchAvailable = false;

  SIMPLE_SUBMENU(STR_MENU_RADIO_TOUCH, ICON_MODEL_SETUP, 1);

  if (event == EVT_ENTRY || event == EVT_ENTRY_UP) {
    touchAvailable = touchPanelInit();
    touchEventFlag = 0;
  }
  else if (menuEvent) {
    //touchDeInit();
    return false;
  }

  if (touchAvailable) {
    lcdDrawText(LCD_W / 2, LCD_H / 2, STR_TOUCH_EXIT, CENTERED);
  }
  else {
    lcdDrawText(LCD_W / 2, LCD_H / 2, STR_TOUCH_NOTFOUND, CENTERED);
  }

  if (touchEventFlag) {
    touchPanelDeInit();
    popMenu();
  }
  return true;
}
