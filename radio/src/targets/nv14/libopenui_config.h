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

#include "lcd.h"

constexpr uint32_t MENU_HEADER_BUTTON_WIDTH =      60;
constexpr uint32_t MENU_HEADER_BUTTONS_LEFT =      MENU_HEADER_BUTTON_WIDTH;

#define MENU_TOOLTIPS
constexpr uint32_t MENU_HEADER_HEIGHT =            60;
constexpr uint32_t MENU_TITLE_TOP =                61;
constexpr uint32_t MENU_TITLE_HEIGHT =             22;
constexpr uint32_t MENU_BODY_TOP =                 MENU_TITLE_TOP + MENU_TITLE_HEIGHT;
constexpr uint32_t MENU_CONTENT_TOP =              MENU_BODY_TOP + 1;
constexpr uint32_t MENU_FOOTER_HEIGHT =            0;
constexpr uint32_t MENU_FOOTER_TOP =               LCD_H - MENU_FOOTER_HEIGHT;
constexpr uint32_t MENU_BODY_HEIGHT =              MENU_FOOTER_TOP - MENU_BODY_TOP;
constexpr uint32_t MENUS_MARGIN_LEFT =             6;

constexpr uint32_t DEFAULT_SCROLLBAR_X =           LCD_W - 10;
constexpr uint32_t DEFAULT_SCROLLBAR_Y =           MENU_CONTENT_TOP;
constexpr uint32_t DEFAULT_SCROLLBAR_H =           MENU_FOOTER_TOP - DEFAULT_SCROLLBAR_Y - 6;

constexpr uint32_t PAGE_PADDING =                  8;
constexpr uint32_t PAGE_LINE_HEIGHT =              26;
constexpr uint32_t PAGE_LINE_SPACING =             6;
constexpr uint32_t PAGE_INDENT_WIDTH =             10;
constexpr uint32_t PAGE_LABEL_WIDTH =              140;
constexpr uint32_t FH =                            PAGE_LINE_HEIGHT;
constexpr uint32_t NUM_BODY_LINES =                MENU_BODY_HEIGHT / PAGE_LINE_HEIGHT;

constexpr uint32_t FIELD_PADDING_LEFT =            3;
constexpr uint32_t FIELD_PADDING_TOP =             2;

constexpr uint32_t CURVE_SIDE_WIDTH =              100;
constexpr uint32_t CURVE_CENTER_X =                LCD_W - CURVE_SIDE_WIDTH - 7;
constexpr uint32_t CURVE_CENTER_Y =                151;
constexpr uint32_t CURVE_COORD_WIDTH =             36;
constexpr uint32_t CURVE_COORD_HEIGHT =            17;

constexpr uint32_t DATETIME_SEPARATOR_X =          LCD_W - 53;
constexpr uint32_t DATETIME_LINE1 =                9;
constexpr uint32_t DATETIME_LINE2 =                23;
constexpr uint32_t DATETIME_MIDDLE =               (LCD_W + DATETIME_SEPARATOR_X + 4) / 2;

constexpr uint32_t MENUS_TOOLBAR_BUTTON_WIDTH =    50;
constexpr uint32_t MENUS_TOOLBAR_BUTTON_PADDING =  11;
constexpr rect_t MENUS_TOOLBAR_RECT =              { 35, 95, 50, 370 };

constexpr uint32_t SUBMENU_LINE_WIDTH =            230;

#endif // _LIBOPENUI_CONFIG_H_
