/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

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

#if LCD_W >= 212
  #define MIXES_2ND_COLUMN             (18*FW)
#else
  #define MIXES_2ND_COLUMN             (12*FW)
#endif

void displaySplash();
void displayScreenIndex(uint8_t index, uint8_t count, uint8_t attr);
void drawStick(coord_t centrex, int16_t xval, int16_t yval);

#if !defined(CPUM64)
  void drawVerticalScrollbar(coord_t x, coord_t y, coord_t h, uint16_t offset, uint16_t count, uint8_t visible);
#endif

#define SET_SCROLLBAR_X(x)
#define LOAD_MODEL_BITMAP()
