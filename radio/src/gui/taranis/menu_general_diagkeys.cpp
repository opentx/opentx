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

#include "../../opentx.h"

void displayKeyState(uint8_t x, uint8_t y, EnumKeys key)
{
  uint8_t t = switchState(key);
  lcdDrawChar(x, y, t+'0', t ? INVERS : 0);
}

void menuGeneralDiagKeys(uint8_t event)
{
  SIMPLE_MENU(STR_MENUDIAG, menuTabGeneral, e_Keys, 1);

  lcdDrawText(14*FW, MENU_HEADER_HEIGHT+2*FH, STR_VTRIM);

  for(uint8_t i=0; i<9; i++) {
    coord_t y;

    if (i<8) {
      y = MENU_HEADER_HEIGHT + FH*3 + FH*(i/2);
      if (i&1) lcd_img(14*FW, y, sticks, i/2, 0);
      displayKeyState(i&1? 20*FW : 18*FW, y, (EnumKeys)(TRM_BASE+i));
    }

    if (i<6) {
      y = i*FH+MENU_HEADER_HEIGHT+FH;
      lcdDrawTextAtIndex(0, y, STR_VKEYS, (5-i), 0);
      displayKeyState(5*FW+2, y, (EnumKeys)(KEY_MENU+(5-i)));
    }
  }
}
