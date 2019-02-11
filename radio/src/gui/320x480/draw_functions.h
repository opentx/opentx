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

#ifndef _DRAW_FUNCTIONS_H_
#define _DRAW_FUNCTIONS_H_

#include <stdint.h>
#include "lcd_types.h"

void drawTextAtIndex(BitmapBuffer * dc, coord_t x, coord_t y, const char * s, uint8_t idx, LcdFlags flags=0);
void drawNumber(BitmapBuffer * dc, coord_t x, coord_t y, int32_t val, LcdFlags flags=0, uint8_t len=0, const char * prefix=NULL, const char * suffix=NULL);
void drawSolidRect(BitmapBuffer * dc, coord_t x, coord_t y, coord_t w, coord_t h, uint8_t thickness=1, LcdFlags flags=0);
void drawSource(BitmapBuffer * dc, coord_t x, coord_t y, mixsrc_t idx, LcdFlags flags=0);
void drawSwitch(BitmapBuffer * dc, coord_t x, coord_t y, int32_t idx, LcdFlags flags=0);

#endif // _DRAW_FUNCTIONS_H_
