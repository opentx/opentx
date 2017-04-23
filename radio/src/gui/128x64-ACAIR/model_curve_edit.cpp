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

void menuModelCurveOne(event_t event)
{
  CurveData & crv = g_model.curves[s_curveChan];
  int8_t * points = curveAddress(s_curveChan);

  drawStringWithIndex(PSIZE(TR_MENUCURVES)*FW+FW, 0, STR_CV, s_curveChan+1);
  
  SIMPLE_SUBMENU(STR_MENUCURVES, 5+crv.points + (crv.type==CURVE_TYPE_CUSTOM ? 5+crv.points-2 : 0));

  drawCurve(0);

  LcdFlags attr = (s_editMode > 0 ? INVERS|BLINK : INVERS);
  for (uint8_t i=0; i<5+crv.points; i++) {
    point_t point = getPoint(i);
    uint8_t selectionMode = 0;
    if (crv.type==CURVE_TYPE_CUSTOM) {
      if (menuVerticalPosition==2*i || (i==5+crv.points-1 && menuVerticalPosition==5+crv.points+5+crv.points-2-1))
        selectionMode = 2;
      else if (i>0 && menuVerticalPosition==-1+2*i)
        selectionMode = 1;
    }
    else if (menuVerticalPosition == i) {
      selectionMode = 2;
    }
    
    if (selectionMode > 0) {
      int8_t x = getCurveX(5+crv.points, i);
      if (crv.type==CURVE_TYPE_CUSTOM && i>0 && i<5+crv.points-1) {
        x = points[5+crv.points+i-1];
      }
  
      // Selection X / Y
      lcdDrawFilledRect(3, 2*FH+4, 7*FW-2, 4*FH-2, SOLID, ERASE);
      lcdDrawRect(3, 2*FH+4, 7*FW-2, 4*FH-2);
      drawStringWithIndex(7, 3*FH, STR_PT, i+1, LEFT);
      lcdDrawText(7, 4*FH, PSTR("x="));
      lcdDrawNumber(7+2*FW+1, 4*FH, x, LEFT|(selectionMode==1?attr:0));
      lcdDrawText(7, 5*FH, PSTR("y="));
      lcdDrawNumber(7+2*FW+1, 5*FH, points[i], LEFT|(selectionMode==2?attr:0));
      
      // Selection square
      lcdDrawFilledRect(point.x-1, point.y-2, 5, 5, SOLID, FORCE);
      lcdDrawFilledRect(point.x, point.y-1, 3, 3, SOLID);
      
      if (s_editMode > 0) {
        if (selectionMode == 1)
          CHECK_INCDEC_MODELVAR(event, points[5+crv.points+i-1], i==1 ? -100 : points[5+crv.points+i-2], i==5+crv.points-2 ? 100 : points[5+crv.points+i]);  // edit X
        else if (selectionMode == 2)
          CHECK_INCDEC_MODELVAR(event, points[i], -100, 100);
      }
    }
  }
}
