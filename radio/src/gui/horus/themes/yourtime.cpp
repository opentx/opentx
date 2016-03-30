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

const uint8_t LBM_TOPMENU_MASK_OPENTX[] = {
#include "mask_topmenu_opentx.lbm"
};

const ZoneOption OPTIONS_THEME_DEFAULT[] = {
  { "Background color", ZoneOption::Color, OPTION_DEFAULT_VALUE_UNSIGNED(RGB(0, 40, 40)) },
  { "Index color", ZoneOption::Color, OPTION_DEFAULT_VALUE_UNSIGNED(RGB(230, 32, 24)) },
  { "White text", ZoneOption::Bool, OPTION_DEFAULT_VALUE_BOOL(1) },
  { "Theme logo", ZoneOption::Bool, OPTION_DEFAULT_VALUE_BOOL(1) },
  { NULL, ZoneOption::Bool }
};

class YourtimeTheme: public Theme
{
  public:
    YourtimeTheme():
      Theme("Yourtime", OPTIONS_THEME_DEFAULT)
    {
      loadColors();
    }

    void loadColors() const
    {
      lcdColorTable[TEXT_COLOR_INDEX] = WHITE;
      lcdColorTable[TEXT_BGCOLOR_INDEX] = BLACK;
      lcdColorTable[TEXT_INVERTED_COLOR_INDEX] = WHITE;
      lcdColorTable[TEXT_INVERTED_BGCOLOR_INDEX] = RED;
      lcdColorTable[LINE_COLOR_INDEX] = GREY;
      lcdColorTable[SCROLLBOX_COLOR_INDEX] = RED;
      lcdColorTable[MENU_TITLE_BGCOLOR_INDEX] = DARKGREY;
      lcdColorTable[MENU_TITLE_COLOR_INDEX] = WHITE;
      lcdColorTable[MENU_TITLE_DISABLE_COLOR_INDEX] = RGB(0x42, 0x42, 0x42);
      lcdColorTable[HEADER_COLOR_INDEX] = DARKGREY;
      lcdColorTable[ALARM_COLOR_INDEX] = RGB(0xff, 0x57, 0x22);
      lcdColorTable[WARNING_COLOR_INDEX] = RGB(0xff, 0x57, 0x22);;
      lcdColorTable[TEXT_DISABLE_COLOR_INDEX] = GREY;
      lcdColorTable[CURVE_AXIS_COLOR_INDEX] = LIGHTGREY;
      lcdColorTable[CURVE_COLOR_INDEX] = RED;
      lcdColorTable[CURVE_CURSOR_COLOR_INDEX] = RED;
      lcdColorTable[TITLE_BGCOLOR_INDEX] = RED;
      lcdColorTable[TRIM_BGCOLOR_INDEX] = RED;
      lcdColorTable[TRIM_SHADOW_COLOR_INDEX] = BLACK;
      lcdColorTable[MAINVIEW_PANES_COLOR_INDEX] = BLACK;
      lcdColorTable[MAINVIEW_GRAPHICS_COLOR_INDEX] = RED;
      lcdColorTable[HEADER_BGCOLOR_INDEX] = DARKRED;
      lcdColorTable[HEADER_ICON_BGCOLOR_INDEX] = RED;
      lcdColorTable[HEADER_CURRENT_BGCOLOR_INDEX] = RED;
      lcdColorTable[OVERLAY_COLOR_INDEX] = BLACK;
    }

    virtual void load() const
    {
      loadColors();
      Theme::load();
      if (!backgroundBitmap) backgroundBitmap = BitmapBuffer::load(getThemePath("mainbg.png"));
      if (!bglogoBitmap) bglogoBitmap = BitmapBuffer::load(getThemePath("bglogo.png"));
      if (!aboutBackgroundBitmap) aboutBackgroundBitmap = BitmapBuffer::load(getThemePath("aboutbg.bmp"));
      if (!barbgBitmap) barbgBitmap = BitmapBuffer::load(getThemePath("barbg.png"));
      update();
    }

    virtual void update() const
    {
      uint32_t colorB = g_eeGeneral.themeData.options[0].unsignedValue;
      uint32_t color = g_eeGeneral.themeData.options[1].unsignedValue;
      lcdColorTable[TEXT_INVERTED_BGCOLOR_INDEX] = color;
      lcdColorTable[SCROLLBOX_COLOR_INDEX] = color;
      lcdColorTable[CURVE_COLOR_INDEX] = color;
      lcdColorTable[CURVE_CURSOR_COLOR_INDEX] = color;
      lcdColorTable[TITLE_BGCOLOR_INDEX] = color;
      lcdColorTable[MENU_TITLE_DISABLE_COLOR_INDEX] = RGB(0x72+GET_RED(colorB)/4, 0x72+GET_GREEN(colorB)/4, 0x72+GET_BLUE(colorB)/4);
      lcdColorTable[TRIM_BGCOLOR_INDEX] = color;
      lcdColorTable[MAINVIEW_GRAPHICS_COLOR_INDEX] = color;
      lcdColorTable[HEADER_BGCOLOR_INDEX] = colorB;
      lcdColorTable[HEADER_ICON_BGCOLOR_INDEX] = color;
      lcdColorTable[HEADER_CURRENT_BGCOLOR_INDEX] = color;
      lcdColorTable[ALARM_COLOR_INDEX] = color;
      lcdColorTable[WARNING_COLOR_INDEX] = color;
      if (g_eeGeneral.themeData.options[2].boolValue) {
        lcdColorTable[TEXT_COLOR_INDEX] = WHITE;
        lcdColorTable[TEXT_BGCOLOR_INDEX] = BLACK;
        lcdColorTable[MAINVIEW_PANES_COLOR_INDEX] = RGB(0x40+GET_RED(colorB)/4, 0x40+GET_GREEN(colorB)/4, 0x40+GET_BLUE(colorB)/4);;
      }
      else {
        lcdColorTable[TEXT_COLOR_INDEX] = BLACK;
        lcdColorTable[TEXT_BGCOLOR_INDEX] = WHITE;
        lcdColorTable[MAINVIEW_PANES_COLOR_INDEX] = WHITE;
      }
    }

    virtual void drawBackground() const
    {
      lcdSetColor(g_eeGeneral.themeData.options[0].unsignedValue);
      lcdDrawSolidFilledRect(0, 0, LCD_W, LCD_H, CUSTOM_COLOR);
      if (backgroundBitmap) {
        lcd->drawBitmap(0, 0, backgroundBitmap);
      }
      if (g_eeGeneral.themeData.options[3].boolValue) {
        if (bglogoBitmap) {
          lcd->drawBitmap(108, 50, bglogoBitmap);
        }
      }
    }

    virtual void drawAboutBackground() const
    {
      lcd->drawBitmap(0, 0, aboutBackgroundBitmap);
    }

    virtual void drawTopbarBackground(const uint8_t * icon) const
    {
      lcdDrawSolidFilledRect(0, 0, LCD_W, MENU_HEADER_HEIGHT, HEADER_BGCOLOR);
      if (barbgBitmap) {
        lcd->drawBitmap(0, 0, barbgBitmap);
      }
      if (icon) {
        lcdDrawBitmapPattern(5, 7, icon, MENU_TITLE_COLOR);
      }
      else {
        lcdDrawBitmapPattern(4, 10, LBM_TOPMENU_MASK_OPENTX, MENU_TITLE_COLOR);
      }

      drawTopbarDatetime();
    }

  protected:
    static const BitmapBuffer * backgroundBitmap;
    static const BitmapBuffer * bglogoBitmap;
    static const BitmapBuffer * aboutBackgroundBitmap;
    static const BitmapBuffer * barbgBitmap;
};

const BitmapBuffer * YourtimeTheme::backgroundBitmap = NULL;
const BitmapBuffer * YourtimeTheme::bglogoBitmap = NULL;
const BitmapBuffer * YourtimeTheme::aboutBackgroundBitmap = NULL;
const BitmapBuffer * YourtimeTheme::barbgBitmap = NULL;

const YourtimeTheme yourtimeTheme;
