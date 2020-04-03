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

#ifndef __COLORLCD_DRAW_FUNCTIONS__
#define __COLORLCD_DRAW_FUNCTIONS__

#include "lcd.h"

void drawStringWithIndex(coord_t x, coord_t y, const char * str, int idx, LcdFlags flags, const char * prefix, const char * suffix);
void drawValueWithUnit(coord_t x, coord_t y, int32_t val, uint8_t unit, LcdFlags att);
int editChoice(coord_t x, coord_t y, const char * values, int value, int min, int max, LcdFlags attr, event_t event, IsValueAvailable isValueAvailable=nullptr);
uint8_t editCheckBox(uint8_t value, coord_t x, coord_t y, LcdFlags attr, event_t event);
swsrc_t editSwitch(coord_t x, coord_t y, swsrc_t value, LcdFlags attr, event_t event);
void drawFatalErrorScreen(const char * message);
void runFatalErrorScreen(const char * message);
void drawPower(coord_t x, coord_t y, int8_t dBm, LcdFlags att);
void lcdDrawMMM(coord_t x, coord_t y, LcdFlags flags);

#if defined(FLIGHT_MODES)
void drawFlightMode(coord_t x, coord_t y, int8_t idx, LcdFlags att);
#endif

#endif // __COLORLCD_DRAW_FUNCTIONS__
