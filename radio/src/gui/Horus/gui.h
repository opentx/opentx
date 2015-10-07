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

#define MENU_HEADER_HEIGHT    45
#define MENU_TITLE_TOP        48
#define MENU_TITLE_HEIGHT     21
#define MENU_BODY_TOP         70
#define MENU_CONTENT_TOP      72
#define MENU_BODY_HEIGHT      182
#define MENU_FOOTER_TOP       251
#define MENU_FOOTER_HEIGHT    21

#define MENU_TITLE_LEFT       6

#define DEFAULT_SCROLLBAR_X   LCD_W-10
#define DEFAULT_SCROLLBAR_Y   MENU_CONTENT_TOP
#define DEFAULT_SCROLLBAR_H   (MENU_FOOTER_TOP - DEFAULT_SCROLLBAR_Y - 6)

#define NUM_BODY_LINES        9
#define FH                    20

#define INVERT_HORZ_MARGIN    3
#define INVERT_VERT_MARGIN    1
#define INVERT_LINE_HEIGHT    18

void displaySplash();
void displayScreenIndex(uint8_t index, uint8_t count);
void displayScrollbar(coord_t x, coord_t y, coord_t h, uint16_t offset, uint16_t count, uint8_t visible);
void drawHeader(int index);
void drawFooter();
#define drawStatusLine(...)
void displayProgressBar(const char *label);
void updateProgressBar(int num, int den);

