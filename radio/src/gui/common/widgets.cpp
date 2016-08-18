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

#if defined(FLIGHT_MODES)
void drawFlightMode(coord_t x, coord_t y, int8_t idx, LcdFlags att)
{
  if (idx==0) {
    lcdDrawTextAtIndex(x, y, STR_MMMINV, 0, att); // TODO macro
    return;
  }
  // TODO this code was not included in Taranis! and used with abs(...) on Horus
  if (idx < 0) {
    lcdDrawChar(x-2, y, '!', att);
    idx = -idx;
  }
#if defined(CONDENSED)
  if (att & CONDENSED) {
    lcdDrawNumber(x+FW*1, y, idx-1, (att & ~CONDENSED), 1);
    return;
  }
#endif
  drawStringWithIndex(x, y, STR_FP, idx-1, att);
}
#endif

#if defined(CPUARM)
void drawCurveRef(coord_t x, coord_t y, CurveRef & curve, LcdFlags att)
{
  if (curve.value != 0) {
    switch (curve.type) {
      case CURVE_REF_DIFF:
        lcdDrawText(x, y, "D", att);
        GVAR_MENU_ITEM(lcdNextPos, y, curve.value, -100, 100, LEFT|att, 0, 0);
        break;
      
      case CURVE_REF_EXPO:
        lcdDrawText(x, y, "E", att);
        GVAR_MENU_ITEM(lcdNextPos, y, curve.value, -100, 100, LEFT|att, 0, 0);
        break;
      
      case CURVE_REF_FUNC:
        lcdDrawTextAtIndex(x, y, STR_VCURVEFUNC, curve.value, att);
        break;
      
      case CURVE_REF_CUSTOM:
        drawCurveName(x, y, curve.value, att);
        break;
    }
  }
}
#endif
