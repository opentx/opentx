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

#include "lcd.h"
#include "menus.h"
#include "widgets.h"
#include "bitmaps.h"
#include "theme.h"

#define MENU_HEADER_HEIGHT             45
#define MENU_TITLE_TOP                 48
#define MENU_TITLE_HEIGHT              21
#define MENU_BODY_TOP                  (MENU_TITLE_TOP+MENU_TITLE_HEIGHT)
#define MENU_CONTENT_TOP               (MENU_BODY_TOP+3)
#define MENU_FOOTER_HEIGHT             21
#define MENU_FOOTER_TOP                (LCD_H-MENU_FOOTER_HEIGHT)
#define MENU_BODY_HEIGHT               (MENU_FOOTER_TOP-MENU_BODY_TOP)
#define MENUS_MARGIN_LEFT              6

#define DEFAULT_SCROLLBAR_X            LCD_W-10
#define DEFAULT_SCROLLBAR_Y            MENU_CONTENT_TOP
#define DEFAULT_SCROLLBAR_H            (MENU_FOOTER_TOP - DEFAULT_SCROLLBAR_Y - 6)

#define FH                             20
#define NUM_BODY_LINES                 (MENU_BODY_HEIGHT / FH)

#define INVERT_HORZ_MARGIN             3
#define INVERT_VERT_MARGIN             1
#define INVERT_LINE_HEIGHT             18

#define CURVE_SIDE_WIDTH               100
#define CURVE_CENTER_X                 (LCD_W-CURVE_SIDE_WIDTH-7)
#define CURVE_CENTER_Y                 151
#define CURVE_COORD_WIDTH              36
#define CURVE_COORD_HEIGHT             17

#define DATETIME_SEPARATOR_X           (LCD_W-55)
#define DATETIME_LINE1                 9
#define DATETIME_LINE2                 23
#define DATETIME_MIDDLE                (LCD_W+DATETIME_SEPARATOR_X+8)/2

#define MENU_TITLE_NEXT_POS            (lcdNextPos + 10)
#define MENU_INIT_VPOS                 -1

#define MENU_ICONS_SPACING             31
#define SUBMENU_LINE_WIDTH             230

#define LOAD_MODEL_BITMAP()

#if defined(FLIGHT_MODES)
void displayFlightModes(coord_t x, coord_t y, FlightModesType value, uint8_t attr);
FlightModesType editFlightModes(coord_t x, coord_t y, evt_t event, FlightModesType value, uint8_t attr);
#else
  #define displayFlightModes(...)
#endif

// Curve functions
coord_t getCurveYCoord(FnFuncP fn, int x, int width);
void drawFunction(FnFuncP fn, int offset);
void drawCurveVerticalScale(int x);
void drawCurveHorizontalScale();
void drawCurveCoord(int x, int y, const char * text, bool active=false);
void drawCurvePoint(int x, int y, LcdFlags color);

extern Layout * customScreens[MAX_CUSTOM_SCREENS];
extern Topbar * topbar;
