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

#ifndef _COLORS_H_
#define _COLORS_H_

// remove windows default definitions
#undef OPAQUE
#undef RGB

#define RGB(r, g, b)                   (uint16_t)((((r) & 0xF8) << 8) + (((g) & 0xFC) << 3) + (((b) & 0xF8) >> 3))
#define ARGB(a, r, g, b)               (uint16_t)((((a) & 0xF0) << 8) + (((r) & 0xF0) << 4) + (((g) & 0xF0) << 0) + (((b) & 0xF0) >> 4))

#define WHITE                          RGB(0xFF, 0xFF, 0xFF)
#define BLACK                          RGB(0, 0, 0)
#define YELLOW                         RGB(0xF0, 0xD0, 0x10)
#define BLUE                           RGB(0x30, 0xA0, 0xE0)
#define GREY                           RGB(96, 96, 96)
#define DARKGREY                       RGB(64, 64, 64)
#define LIGHTGREY                      RGB(180, 180, 180)
#define RED                            RGB(229, 32, 30)
#define DARKRED                        RGB(160, 0, 6)
#define GREEN                          RGB(25, 150, 50)

#define OPACITY_MAX                    0x0F
#define OPACITY(x)                     ((x)<<24)

enum LcdColorIndex
{
  TEXT_COLOR_INDEX,
  TEXT_BGCOLOR_INDEX,
  TEXT_INVERTED_COLOR_INDEX,
  TEXT_INVERTED_BGCOLOR_INDEX,
  TEXT_STATUSBAR_COLOR_INDEX,
  LINE_COLOR_INDEX,
  SCROLLBOX_COLOR_INDEX,
  MENU_TITLE_BGCOLOR_INDEX,
  MENU_TITLE_COLOR_INDEX,
  MENU_TITLE_DISABLE_COLOR_INDEX,
  HEADER_COLOR_INDEX,
  ALARM_COLOR_INDEX,
  WARNING_COLOR_INDEX,
  TEXT_DISABLE_COLOR_INDEX,
  CURVE_AXIS_COLOR_INDEX,
  CURVE_COLOR_INDEX,
  CURVE_CURSOR_COLOR_INDEX,
  HEADER_BGCOLOR_INDEX,
  HEADER_ICON_BGCOLOR_INDEX,
  HEADER_CURRENT_BGCOLOR_INDEX,
  TITLE_BGCOLOR_INDEX,
  TRIM_BGCOLOR_INDEX,
  TRIM_SHADOW_COLOR_INDEX,
  MAINVIEW_PANES_COLOR_INDEX,
  MAINVIEW_GRAPHICS_COLOR_INDEX,
  OVERLAY_COLOR_INDEX,
  CUSTOM_COLOR_INDEX,
  BARGRAPH1_COLOR_INDEX,
  BARGRAPH2_COLOR_INDEX,
  BARGRAPH_BGCOLOR_INDEX,
  LCD_COLOR_COUNT
};

extern uint16_t lcdColorTable[LCD_COLOR_COUNT];

#define COLOR(index)                   ((index) << 16)
#define COLOR_IDX(att)                 uint8_t((att) >> 16)

#define TEXT_COLOR                     COLOR(TEXT_COLOR_INDEX)
#define TEXT_BGCOLOR                   COLOR(TEXT_BGCOLOR_INDEX)
#define TEXT_INVERTED_COLOR            COLOR(TEXT_INVERTED_COLOR_INDEX)
#define TEXT_INVERTED_BGCOLOR          COLOR(TEXT_INVERTED_BGCOLOR_INDEX)
#define TEXT_STATUSBAR_COLOR           COLOR(TEXT_STATUSBAR_COLOR_INDEX)
#define LINE_COLOR                     COLOR(LINE_COLOR_INDEX)
#define SCROLLBOX_COLOR                COLOR(SCROLLBOX_COLOR_INDEX)
#define HEADER_SEPARATOR_COLOR         COLOR(HEADER_SEPARATOR_COLOR_INDEX)
#define MENU_TITLE_BGCOLOR             COLOR(MENU_TITLE_BGCOLOR_INDEX)
#define MENU_TITLE_COLOR               COLOR(MENU_TITLE_COLOR_INDEX)
#define MENU_TITLE_DISABLE_COLOR       COLOR(MENU_TITLE_DISABLE_COLOR_INDEX)
#define HEADER_COLOR                   COLOR(HEADER_COLOR_INDEX)
#define ALARM_COLOR                    COLOR(ALARM_COLOR_INDEX)
#define WARNING_COLOR                  COLOR(WARNING_COLOR_INDEX)
#define TEXT_DISABLE_COLOR             COLOR(TEXT_DISABLE_COLOR_INDEX)
#define CURVE_AXIS_COLOR               COLOR(CURVE_AXIS_COLOR_INDEX)
#define CURVE_COLOR                    COLOR(CURVE_COLOR_INDEX)
#define CURVE_CURSOR_COLOR             COLOR(CURVE_CURSOR_COLOR_INDEX)
#define TITLE_BGCOLOR                  COLOR(TITLE_BGCOLOR_INDEX)
#define TRIM_BGCOLOR                   COLOR(TRIM_BGCOLOR_INDEX)
#define TRIM_SHADOW_COLOR              COLOR(TRIM_SHADOW_COLOR_INDEX)
#define HEADER_BGCOLOR                 COLOR(HEADER_BGCOLOR_INDEX)
#define HEADER_ICON_BGCOLOR            COLOR(HEADER_ICON_BGCOLOR_INDEX)
#define HEADER_CURRENT_BGCOLOR         COLOR(HEADER_CURRENT_BGCOLOR_INDEX)
#define MAINVIEW_PANES_COLOR           COLOR(MAINVIEW_PANES_COLOR_INDEX)
#define MAINVIEW_GRAPHICS_COLOR        COLOR(MAINVIEW_GRAPHICS_COLOR_INDEX)
#define OVERLAY_COLOR                  COLOR(OVERLAY_COLOR_INDEX)
#define BARGRAPH1_COLOR                COLOR(BARGRAPH1_COLOR_INDEX)
#define BARGRAPH2_COLOR                COLOR(BARGRAPH2_COLOR_INDEX)
#define BARGRAPH_BGCOLOR               COLOR(BARGRAPH_BGCOLOR_INDEX)
#define CUSTOM_COLOR                   COLOR(CUSTOM_COLOR_INDEX)

#define ARGB_SPLIT(color, a, r, g, b) \
  uint16_t a = ((color) & 0xF000) >> 12; \
  uint16_t r = ((color) & 0x0F00) >> 8; \
  uint16_t g = ((color) & 0x00F0) >> 4; \
  uint16_t b = ((color) & 0x000F)

#define RGB_SPLIT(color, r, g, b) \
  uint16_t r = ((color) & 0xF800) >> 11; \
  uint16_t g = ((color) & 0x07E0) >> 5; \
  uint16_t b = ((color) & 0x001F)

#define RGB_JOIN(r, g, b) \
  (((r) << 11) + ((g) << 5) + (b))

#define GET_RED(color) \
  (((color) & 0xF800) >> 8)

#define GET_GREEN(color) \
  (((color) & 0x07E0) >> 3)

#define GET_BLUE(color) \
  (((color) & 0x001F) << 3)

#endif // _COLORS_H_
