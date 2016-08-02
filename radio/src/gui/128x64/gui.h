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

#ifndef _GUI_H_
#define _GUI_H_

#include "gui_common.h"
#include "lcd.h"
#include "menus.h"

extern const pm_uchar sticks[] PROGMEM;

#define NUM_BODY_LINES                 (LCD_LINES-1)
#define MENU_HEADER_HEIGHT             FH
#define MENU_INIT_VPOS                 0
#define DEFAULT_SCROLLBAR_X            (LCD_W-1)

#define WCHART                         (LCD_H/2)
#define X0                             (LCD_W-WCHART-2)
#define Y0                             (LCD_H/2)

#define MIXES_2ND_COLUMN               (12*FW)

void drawSplash();
void drawScreenIndex(uint8_t index, uint8_t count, uint8_t attr);
void drawStick(coord_t centrex, int16_t xval, int16_t yval);
inline void drawProgressBar(const char * label)
{
  // TODO
}
inline void updateProgressBar(int num, int den)
{
  // TODO
}
inline void drawSleepBitmap()
{
  // TODO
}

#if !defined(CPUM64)
  void drawVerticalScrollbar(coord_t x, coord_t y, coord_t h, uint16_t offset, uint16_t count, uint8_t visible);
#endif


#define SET_SCROLLBAR_X(x)
#define LOAD_MODEL_BITMAP()

#define IS_MAIN_VIEW_DISPLAYED()       menuHandlers[0] == menuMainView
#if defined(TELEMETRY_FRSKY)
#define IS_TELEMETRY_VIEW_DISPLAYED()  menuHandlers[0] == menuTelemetryFrsky
#else
#define IS_TELEMETRY_VIEW_DISPLAYED()  false
#endif
#define IS_OTHER_VIEW_DISPLAYED()      false

#endif // _GUI_H_