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

const uint8_t LBM_TOPMENU_BMP_OPENTX[] __DMA = {
#include "bmp_topmenu_opentx.lbm"
};

const uint8_t LBM_THEME_DARKBLUE[] __DMA = {
#include "bmp_darkblue.lbm"
};

class DarkblueTheme: public Theme
{
  public:
    DarkblueTheme():
      Theme("Darkblue", LBM_THEME_DARKBLUE)
    {
    }

    virtual void load() const
    {
      lcdColorTable[TEXT_COLOR_INDEX] = WHITE;
      lcdColorTable[TEXT_BGCOLOR_INDEX] = RGB(10, 78, 121);
      lcdColorTable[TEXT_INVERTED_COLOR_INDEX] = WHITE;
      lcdColorTable[TEXT_INVERTED_BGCOLOR_INDEX] = RGB(32, 34, 42);
      lcdColorTable[LINE_COLOR_INDEX] = GREY;
      lcdColorTable[SCROLLBOX_COLOR_INDEX] = WHITE;
      lcdColorTable[MENU_TITLE_BGCOLOR_INDEX] = DARKGREY;
      lcdColorTable[MENU_TITLE_COLOR_INDEX] = WHITE;
      lcdColorTable[MENU_TITLE_DISABLE_COLOR_INDEX] = BLACK;
      lcdColorTable[HEADER_COLOR_INDEX] = RGB(32, 34, 42);
      lcdColorTable[ALARM_COLOR_INDEX] = RGB(32, 34, 42);
      lcdColorTable[WARNING_COLOR_INDEX] = YELLOW;
      lcdColorTable[TEXT_DISABLE_COLOR_INDEX] = GREY;
      lcdColorTable[CURVE_AXIS_COLOR_INDEX] = LIGHTGREY;
      lcdColorTable[CURVE_COLOR_INDEX] = RGB(32, 34, 42);
      lcdColorTable[CURVE_CURSOR_COLOR_INDEX] = RGB(32, 34, 42);
      lcdColorTable[TITLE_BGCOLOR_INDEX] = RGB(32, 34, 42);
      lcdColorTable[TRIM_BGCOLOR_INDEX] = RGB(32, 34, 42);
      lcdColorTable[TRIM_SHADOW_COLOR_INDEX] = RGB(100, 100, 100);
      lcdColorTable[MAINVIEW_PANES_COLOR_INDEX] = GREY;
      lcdColorTable[MAINVIEW_GRAPHICS_COLOR_INDEX] = WHITE;
      lcdColorTable[HEADER_BGCOLOR_INDEX] = RGB(32, 34, 42);
      lcdColorTable[HEADER_ICON_BGCOLOR_INDEX] = BLACK;
      lcdColorTable[HEADER_CURRENT_BGCOLOR_INDEX] = RGB(10, 78, 121);
      lcdColorTable[OVERLAY_COLOR_INDEX] = BLACK;
    }

    void drawTopbarBackground(const uint8_t * icon) const
    {
      lcdDrawSolidFilledRect(0, 0, LCD_W, MENU_HEADER_HEIGHT, HEADER_BGCOLOR);
      lcdDrawSolidFilledRect(0, 0, 41, MENU_HEADER_HEIGHT, HEADER_ICON_BGCOLOR);

      if (icon) {
        lcdDrawBitmapPattern(5, 7, icon, MENU_TITLE_COLOR);
      }
      else {
        lcdDrawBitmap(5, 7, LBM_TOPMENU_BMP_OPENTX);
      }

      drawTopbarDatetime();
    }
};

DarkblueTheme darkblueTheme;
