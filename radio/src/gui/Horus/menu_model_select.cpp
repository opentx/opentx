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

#include "../../opentx.h"

#define CATEGORIES_WIDTH 140

void drawModel(coord_t x, coord_t y, const char * name)
{
  lcdDrawSolidRect(x, y, 153, 61, LINE_COLOR);
  lcdDrawText(x+5, y+2, name, TEXT_COLOR);
  lcdDrawSolidHorizontalLine(x+5, y+19, 143, LINE_COLOR);
  lcdDrawBitmapPattern(x+5, y+23, LBM_LIBRARY_SLOT, TEXT_COLOR);
}

void menuModelSelect(evt_t event)
{
  switch(event) {
    case 0:
      // no need to refresh the screen
      return;

    case EVT_KEY_FIRST(KEY_EXIT):
      chainMenu(menuMainView);
      return;
  }

  // Header
  lcdDrawSolidFilledRect(0, 0, LCD_W, MENU_HEADER_HEIGHT, HEADER_BGCOLOR);
  lcdDrawBitmapPattern(0, 0, LBM_TOPMENU_POLYGON, TITLE_BGCOLOR);
  lcdDrawBitmapPattern(5, 7, LBM_LIBRARY_ICON, MENU_TITLE_COLOR);
  drawTopmenuDatetime();

  // Categories
  lcdDrawSolidFilledRect(0, MENU_HEADER_HEIGHT, CATEGORIES_WIDTH, LCD_H-MENU_HEADER_HEIGHT-MENU_FOOTER_HEIGHT, TITLE_BGCOLOR);

  StorageModelsList storage;
  const char * error = storageOpenModelsList(&storage);
  if (!error) {
    bool result = true;
    coord_t y = MENU_HEADER_HEIGHT+10;
    while (y < LCD_H) {
      char line[256];
      result = storageReadNextCategory(&storage, line, sizeof(line)-1);
      if (!result)
        break;
      lcdDrawText(MENUS_MARGIN_LEFT, y, line, MENU_TITLE_COLOR);
      y += FH;
    }
  }

  // Models
  lcdDrawSolidFilledRect(CATEGORIES_WIDTH, MENU_HEADER_HEIGHT, LCD_W-CATEGORIES_WIDTH, LCD_H-MENU_HEADER_HEIGHT-MENU_FOOTER_HEIGHT, TEXT_BGCOLOR);
  if (!error) {
    bool result = storageSeekCategory(&storage, 0);
    coord_t y = MENU_HEADER_HEIGHT+7;
    unsigned int i = 0;
    while (result) {
      char line[256];
      result = storageReadNextModel(&storage, line, sizeof(line)-1);
      if (!result)
        break;
      if (y < LCD_H) {
        coord_t x;
        if (i & 1) {
          drawModel(CATEGORIES_WIDTH+MENUS_MARGIN_LEFT+162, y, line);
          y += 66;
        }
        else {
          drawModel(CATEGORIES_WIDTH+MENUS_MARGIN_LEFT+1, y, line);
        }
      }
      i++;
    }
  }

  drawScrollbar(DEFAULT_SCROLLBAR_X, MENU_HEADER_HEIGHT+7, MENU_FOOTER_TOP-MENU_HEADER_HEIGHT-15, 0, 4, 2);

  // Footer
  lcdDrawSolidFilledRect(0, MENU_FOOTER_TOP, LCD_W, MENU_FOOTER_HEIGHT, HEADER_BGCOLOR);
}
