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

#define MENU_HEADER_HEIGHT             45
#define MENU_TITLE_TOP                 48
#define MENU_TITLE_HEIGHT              21
#define MENU_BODY_TOP                  69  // MENU_TITLE_TOP + MENU_TITLE_HEIGHT
#define MENU_CONTENT_TOP               72
#define MENU_BODY_HEIGHT               182
#define MENU_FOOTER_TOP                251 // MENU_BODY_TOP + MENU_BODY_HEIGHT
#define MENU_FOOTER_HEIGHT             21

#define MENUS_MARGIN_LEFT              6

#define DEFAULT_SCROLLBAR_X            LCD_W-10
#define DEFAULT_SCROLLBAR_Y            MENU_CONTENT_TOP
#define DEFAULT_SCROLLBAR_H            (MENU_FOOTER_TOP - DEFAULT_SCROLLBAR_Y - 6)

#define NUM_BODY_LINES                 9
#define FH                             20

#define INVERT_HORZ_MARGIN             3
#define INVERT_VERT_MARGIN             1
#define INVERT_LINE_HEIGHT             18

#define CURVE_SIDE_WIDTH               100
#define CURVE_CENTER_X                 (LCD_W-CURVE_SIDE_WIDTH-7)
#define CURVE_CENTER_Y                 151
#define CURVE_COORD_WIDTH              36
#define CURVE_COORD_HEIGHT             17

#define MENU_TITLE_NEXT_POS            (lcdNextPos + 10)
#define MENU_INIT_VPOS                 -1

#define MENU_ICONS_SPACING             31
#define SUBMENU_LINE_WIDTH             230

#define BITMAP_BUFFER_SIZE(width, height)   (4 + (width)*(height)*3)
#define MODEL_BITMAP_WIDTH             (3*64)
#define MODEL_BITMAP_HEIGHT            (3*32)
#define MODEL_BITMAP_SIZE              BITMAP_BUFFER_SIZE(MODEL_BITMAP_WIDTH, MODEL_BITMAP_HEIGHT)
extern uint8_t modelBitmap[MODEL_BITMAP_SIZE];
bool loadModelBitmap(char * name, uint8_t * bitmap);
#define LOAD_MODEL_BITMAP()            loadModelBitmap(g_model.header.bitmap, modelBitmap)

void drawSplash();
void drawScreenIndex(uint8_t index, uint8_t count);
void drawVerticalScrollbar(coord_t x, coord_t y, coord_t h, uint16_t offset, uint16_t count, uint8_t visible);
void drawHorizontalScrollbar(coord_t x, coord_t y, coord_t w, uint16_t offset, uint16_t count, uint8_t visible);
void drawHeader(int index);
void drawColumnHeader(const char * const * headers, uint8_t index);
void drawFooter();
#define drawStatusLine(...)
void drawProgressBar(const char *label);
void updateProgressBar(int num, int den);
void drawTopmenuDatetime();
void drawSubmenuTemplate(const char * name, uint16_t scrollbar_X);
void drawStick(coord_t centrex, int16_t xval, int16_t yval);
void drawSticks();

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

// Header bitmaps
extern const uint8_t LBM_TOPMENU_POLYGON[];
extern const uint8_t LBM_DOT[];
extern const uint8_t LBM_CURRENT_BG[];
extern const uint8_t LBM_CURRENT_SHADOW[];
extern const uint8_t LBM_CURRENT_DOT[];

// Main view icons
extern const uint8_t LBM_MAINVIEW_BACKGROUND[];
extern const uint8_t LBM_TOPMENU_OPENTX[];
extern const uint8_t LBM_TOPMENU_USB[];
extern const uint8_t LBM_HTRIM_FRAME[];
extern const uint8_t LBM_VTRIM_FRAME[];
extern const uint8_t LBM_TRIM_SHADOW[];
extern const uint8_t LBM_RADIO_ICON[];
extern const uint8_t LBM_MODEL_ICON[];
extern const uint8_t LBM_TIMER_BACKGROUND[];
extern const uint8_t LBM_RSCALE[];

// Menu icons
extern const uint8_t * const LBM_RADIO_ICONS[];
extern const uint8_t * const LBM_MODEL_ICONS[];

// Model selection icons
extern const uint8_t LBM_LIBRARY_ICON[];
extern const uint8_t LBM_LIBRARY_SLOT[];
extern const uint8_t LBM_LIBRARY_CURSOR[];
extern const uint8_t LBM_SCORE0[];
extern const uint8_t LBM_SCORE1[];
extern const uint8_t LBM_STAR0[];
extern const uint8_t LBM_STAR1[];

// Other icons
extern const uint8_t LBM_SPLASH[];
extern const uint8_t LBM_ASTERISK[];
extern const uint8_t LBM_POINT[];
extern const uint8_t LBM_CURVE_POINT[];
extern const uint8_t LBM_CURVE_POINT_CENTER[];
extern const uint8_t LBM_CURVE_COORD_SHADOW[];
extern const uint8_t LBM_SHUTDOWN[];
extern const uint8_t LBM_SLEEP[];
extern const uint8_t LBM_SHUTDOWN_CIRCLE[];
extern const uint8_t LBM_SLIDER_BAR_LEFT[];
extern const uint8_t LBM_SLIDER_BAR_RIGHT[];
extern const uint8_t LBM_SLIDER_POINT_OUT[];
extern const uint8_t LBM_SLIDER_POINT_MID[];
extern const uint8_t LBM_SLIDER_POINT_IN[];

