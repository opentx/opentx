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

#include "opentx.h"

void displayKeyState(uint8_t x, uint8_t y, uint8_t key)
{
  uint8_t t = keyState(key);
  lcdDrawChar(x, y, t+'0', t ? INVERS : 0);
}

#if !defined(PCBTARANIS)
void displaySwitchState(uint8_t x, uint8_t y, uint8_t sw)
{
  swsrc_t t = switchState(sw);
  lcdDrawChar(x, y, (t ? '1' : '0'), t ? INVERS : 0);
}
#endif

void menuRadioDiagKeys(event_t event)
{
  SIMPLE_MENU(STR_MENU_RADIO_SWITCHES, menuTabGeneral, MENU_RADIO_SWITCHES_TEST, 1);

  lcdDrawText(14*FW, MENU_HEADER_HEIGHT+2*FH, STR_VTRIM);

  for (uint8_t i=0; i<9; i++) {
    coord_t y;

    if (i < 8) {
      y = MENU_HEADER_HEIGHT + FH*3 + FH*(i/2);
      if (i&1) lcd_img(14*FW, y, sticks, i/2, 0);
      displayKeyState(i&1? 20*FW : 18*FW, y, TRM_BASE+i);
    }

    if (i < 6) {
      y = MENU_HEADER_HEIGHT + FH + FH*i;
      lcdDrawTextAtIndex(0, y, STR_VKEYS, (5-i), 0);
      displayKeyState(5*FW+2, y, KEY_MENU+(5-i));
    }
  
#if defined(PCBTARANIS)
    if (i < NUM_SWITCHES) {
      if (SWITCH_EXISTS(i)) {
        getvalue_t val = getValue(MIXSRC_FIRST_SWITCH+i);
        getvalue_t sw = ((val < 0) ? 3*i+1 : ((val == 0) ? 3*i+2 : 3*i+3));
        drawSwitch(8*FW, y, sw, 0);
      }
    }
#else
    if (i != SW_ID0) {
      y = MENU_HEADER_HEIGHT +i*FH - 2*FH;
      drawSwitch(8*FW, y, i+1, 0);
      displaySwitchState(11*FW+2, y, i);
    }
#endif
  }

#if defined(ROTARY_ENCODER_NAVIGATION)
  for (uint8_t i=0; i<DIM(rotencValue); i++) {
    coord_t y = MENU_HEADER_HEIGHT /* ??? + 1 ??? */ + i*FH;
    lcdDrawTextAtIndex(14*FW, y, STR_VRENCODERS, i, 0);
  #if defined(ROTARY_ENCODERS)
    lcdDrawNumber(18*FW, y, rotencValue[i], LEFT|(keyState(BTN_REa+i) ? INVERS : 0));
  #else
    lcdDrawNumber(18*FW, y, rotencValue[i], LEFT);
  #endif
  }
#endif

}
