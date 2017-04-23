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

// TODO avoid this global s_currCh on ARM boards ...
int8_t s_currCh;

void drawFunction(FnFuncP fn, uint8_t offset)
{
  lcdDrawVerticalLine(CURVE_CENTER_X-offset, 0/*TODO CURVE_CENTER_Y-CURVE_SIDE_WIDTH*/, CURVE_SIDE_WIDTH*2, 0xee);
  lcdDrawHorizontalLine(CURVE_CENTER_X-CURVE_SIDE_WIDTH-offset, CURVE_CENTER_Y, CURVE_SIDE_WIDTH*2, 0xee);

  coord_t prev_yv = (coord_t)-1;

  for (int xv=-CURVE_SIDE_WIDTH; xv<=CURVE_SIDE_WIDTH; xv++) {
    coord_t yv = (LCD_H-1) - (((uint16_t)RESX + fn(xv * (RESX/CURVE_SIDE_WIDTH))) / 2 * (LCD_H-1) / RESX);
    if (prev_yv != (coord_t)-1) {
      if (abs((int8_t)yv-prev_yv) <= 1) {
        lcdDrawPoint(CURVE_CENTER_X+xv-offset-1, prev_yv, FORCE);
      }
      else {
        uint8_t tmp = (prev_yv < yv ? 0 : 1);
        lcdDrawSolidVerticalLine(CURVE_CENTER_X+xv-offset-1, yv+tmp, prev_yv-yv);
      }
    }
    prev_yv = yv;
  }
}

void menuModelExposAll(event_t event)
{
  SIMPLE_MENU(STR_MENUINPUTS, menuTabModel, MENU_MODEL_INPUTS, HEADER_LINE + 4);

  lcdDrawText(5, FH+4, "Speed switch :");
  int speed_pos = getValue(MIXSRC_SA);
  speed_pos = (speed_pos > 0 ? 2 : (speed_pos < 0 ? 0 : 1));
  lcdDrawTextAtIndex(lcdLastRightPos + FW, FH+4, "\004UP\0 MID\0DOWN", speed_pos, BOLD);

  lcdDrawText(5, 5*FH, "Rate");
  lcdDrawText(5, 6*FH+4, "Expo");

  lcdDrawText(8*FW, 3*FH+1, "THR");
  lcdDrawText(15*FW, 3*FH+1, "RUD");
  lcdDrawSolidHorizontalLine(5, 4*FH+3, LCD_W-10);

  LcdFlags flags = (s_editMode == 1 ? INVERS | BLINK : INVERS);
  for (int i=0; i<3; i++) {
    // RUD
    if (isExpoActive(i)) {
      ExpoData * expo = expoAddress(i);
      lcdDrawNumber(15*FW, 5*FH, expo->weight, menuVerticalPosition==2 ? flags : 0);
      lcdDrawText(lcdLastRightPos + 1, 5*FH, "%");
      if (menuVerticalPosition==2 && s_editMode == 1) {
        expo->weight = checkIncDecModel(event, expo->weight, 0, 100);
      }
      lcdDrawNumber(15*FW, 6*FH+4, expo->curve.value, menuVerticalPosition==3 ? flags : 0);
      lcdDrawText(lcdLastRightPos + 1, 6*FH+4, "%");
      if (menuVerticalPosition==3 && s_editMode == 1) {
        expo->curve.value = checkIncDecModel(event, expo->curve.value, 0, 100);
      }
    }
    // THR
    if (isExpoActive(3 + i)) {
      ExpoData * expo = expoAddress(3 + i);
      lcdDrawNumber(8*FW, 5*FH, expo->weight, menuVerticalPosition==0 ? flags : 0);
      lcdDrawText(lcdLastRightPos + 1, 5*FH, "%");
      if (menuVerticalPosition==0 && s_editMode == 1) {
        expo->weight = checkIncDecModel(event, expo->weight, 0, 100);
      }
      lcdDrawNumber(8*FW, 6*FH+4, expo->curve.value, menuVerticalPosition==1 ? flags : 0);
      lcdDrawText(lcdLastRightPos + 1, 6*FH+4, "%");
      if (menuVerticalPosition==1 && s_editMode == 1) {
        expo->curve.value = checkIncDecModel(event, expo->curve.value, 0, 100);
      }
    }
  }
}
