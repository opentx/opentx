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

#ifndef _LIBOPENUI_CONFIG_H_
#define _LIBOPENUI_CONFIG_H_

#include "debug.h"
#include "libopenui_types.h"
#include "colors.h"
#include "board.h"
#include "keys.h"

typedef uint16_t pixel_t;

constexpr uint32_t MENU_HEADER_BUTTON_WIDTH =      33;
constexpr uint32_t MENU_HEADER_BUTTONS_LEFT =      47;

#define MENU_TOOLTIPS
constexpr uint32_t MENU_HEADER_HEIGHT =            45;
constexpr uint32_t MENU_TITLE_TOP =                48;
constexpr uint32_t MENU_TITLE_HEIGHT =             21;
constexpr uint32_t MENU_BODY_TOP =                 MENU_TITLE_TOP + MENU_TITLE_HEIGHT;
constexpr uint32_t MENU_CONTENT_TOP =              MENU_BODY_TOP + 1;
constexpr uint32_t MENU_FOOTER_HEIGHT =            0;
constexpr uint32_t MENU_FOOTER_TOP =               LCD_H - MENU_FOOTER_HEIGHT;
constexpr uint32_t MENU_BODY_HEIGHT =              MENU_FOOTER_TOP - MENU_BODY_TOP;
constexpr uint32_t MENUS_MARGIN_LEFT =             6;

constexpr uint32_t DEFAULT_SCROLLBAR_X =           LCD_W - 10;
constexpr uint32_t DEFAULT_SCROLLBAR_Y =           MENU_CONTENT_TOP;
constexpr uint32_t DEFAULT_SCROLLBAR_H =           MENU_FOOTER_TOP - DEFAULT_SCROLLBAR_Y - 6;

constexpr uint32_t PAGE_PADDING =                  6;
constexpr uint32_t PAGE_LINE_HEIGHT =              20;
constexpr uint32_t PAGE_LINE_SPACING =             2;
constexpr uint32_t PAGE_INDENT_WIDTH =             10;
constexpr uint32_t PAGE_LABEL_WIDTH =              240;
constexpr uint32_t FH =                            PAGE_LINE_HEIGHT;
constexpr uint32_t NUM_BODY_LINES =                MENU_BODY_HEIGHT / PAGE_LINE_HEIGHT;

constexpr uint32_t FIELD_PADDING_LEFT =            3;
constexpr uint32_t FIELD_PADDING_TOP =             0;

constexpr uint32_t INVERT_HORZ_MARGIN =            2;
constexpr uint32_t INVERT_VERT_MARGIN =            1;
constexpr uint32_t INVERT_LINE_HEIGHT =            20;

constexpr uint32_t CURVE_SIDE_WIDTH =              100;
constexpr uint32_t CURVE_CENTER_X =                LCD_W - CURVE_SIDE_WIDTH - 7;
constexpr uint32_t CURVE_CENTER_Y =                151;
constexpr uint32_t CURVE_COORD_WIDTH =             36;
constexpr uint32_t CURVE_COORD_HEIGHT =            17;

constexpr uint32_t DATETIME_SEPARATOR_X =          LCD_W - 53;
constexpr uint32_t DATETIME_LINE1 =                7;
constexpr uint32_t DATETIME_LINE2 =                22;
constexpr uint32_t DATETIME_MIDDLE =               (LCD_W + DATETIME_SEPARATOR_X + 1) / 2;

constexpr uint32_t MENUS_TOOLBAR_BUTTON_WIDTH =    30;
constexpr uint32_t MENUS_TOOLBAR_BUTTON_PADDING =  3;
constexpr rect_t MENUS_TOOLBAR_RECT =              { 100, 51, 30, 209 };

constexpr uint32_t ALERT_FRAME_TOP =               70;
constexpr uint32_t ALERT_FRAME_HEIGHT =            (LCD_H - 2 * ALERT_FRAME_TOP);
constexpr uint32_t ALERT_BITMAP_TOP =              ALERT_FRAME_TOP + 15;
constexpr uint32_t ALERT_BITMAP_LEFT =             40;
constexpr uint32_t ALERT_TITLE_TOP =               ALERT_FRAME_TOP + 10;
constexpr uint32_t ALERT_TITLE_LEFT =              186;
constexpr uint32_t ALERT_TITLE_LINE_HEIGHT =       30;
constexpr uint32_t ALERT_MESSAGE_TOP =             ALERT_TITLE_TOP + 90;
constexpr uint32_t ALERT_MESSAGE_LEFT =            ALERT_TITLE_LEFT;
constexpr uint32_t ALERT_ACTION_TOP =              240;

// TODO common code, not in target
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

#if !defined(BOOT)
#define FONT_TABLE_SIZE 16
#else
#define FONT_TABLE_SIZE 1
#endif

constexpr uint32_t INPUT_EDIT_LABELS_WIDTH = 120;

#endif // _LIBOPENUI_CONFIG_H_
