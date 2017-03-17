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
  TITLE(STR_MENUCURVE);
  lcdDrawNumber(PSIZE(TR_MENUCURVE)*FW+1, 0, s_curveChan+1, INVERS|LEFT);
  DISPLAY_PROGRESS_BAR(20*FW+1);

  CurveInfo crv = curveInfo(s_curveChan);

  switch (event) {
    case EVT_ENTRY:
      s_editMode = 1;
      break;
      
    CASE_EVT_ROTARY_BREAK
    case EVT_KEY_BREAK(KEY_ENTER):
      if (s_editMode <= 0)
        menuHorizontalPosition = 0;
      if (s_editMode == 1 && crv.custom)
        s_editMode = 2;
      else
        s_editMode = 1;
      break;
      
    case EVT_KEY_LONG(KEY_ENTER):
      if (s_editMode <= 0) {
        if (int8_t(++menuHorizontalPosition) > 4)
          menuHorizontalPosition = -4;
        for (uint8_t i=0; i<crv.points; i++)
          crv.crv[i] = (i-(crv.points/2)) * int8_t(menuHorizontalPosition) * 50 / (crv.points-1);
        storageDirty(EE_MODEL);
        killEvents(event);
      }
      break;
      
    case EVT_KEY_BREAK(KEY_EXIT):
      if (s_editMode > 0) {
        if (--s_editMode == 0)
          menuHorizontalPosition = 0;
      }
      else {
        popMenu();
      }
      break;

    /* CASE_EVT_ROTARY_LEFT */
    case EVT_KEY_REPT(KEY_LEFT):
    case EVT_KEY_FIRST(KEY_LEFT):
      if (s_editMode==1 && menuHorizontalPosition>0) menuHorizontalPosition--;
      if (s_editMode <= 0) {
        if (crv.custom) {
          moveCurve(s_curveChan, -crv.points+2);
        }
        else if (crv.points > MIN_POINTS_PER_CURVE) {
          moveCurve(s_curveChan, -1, (crv.points+1)/2);
        }
        else {
          AUDIO_WARNING2();
        }
        return;
      }
      break;

    /* CASE_EVT_ROTARY_RIGHT */
    case EVT_KEY_REPT(KEY_RIGHT):
    case EVT_KEY_FIRST(KEY_RIGHT):
      if (s_editMode==1 && menuHorizontalPosition<(crv.points-1)) menuHorizontalPosition++;
      if (s_editMode <= 0) {
        if (!crv.custom) {
          moveCurve(s_curveChan, crv.points-2, crv.points);
        }
        else if (crv.points < MAX_POINTS_PER_CURVE) {
          if (moveCurve(s_curveChan, 1)) {
            for (int8_t i=crv.points+crv.points-2; i>=0; i--) {
              if (i%2)
                crv.crv[i] = (crv.crv[i/2] + crv.crv[1+i/2]) / 2;
              else
                crv.crv[i] = crv.crv[i/2];
            }
          }
        }
        else {
          AUDIO_WARNING2();
        }
      }
      break;
  }

  lcdDrawTextAlignedLeft(7*FH, STR_TYPE);
  uint8_t attr = (s_editMode <= 0 ? INVERS : 0);
  lcdDrawNumber(5*FW-2, 7*FH, crv.points, LEFT|attr);
  lcdDrawText(lcdLastRightPos, 7*FH, crv.custom ? PSTR("pt'") : PSTR("pt"), attr);

  drawCurve();

  if (s_editMode>0) {
    uint8_t i = menuHorizontalPosition;
    point_t point = getPoint(i);

    if (s_editMode==1 || !BLINK_ON_PHASE) {
      // do selection square
      lcdDrawFilledRect(point.x-1, point.y-2, 5, 5, SOLID, FORCE);
      lcdDrawFilledRect(point.x, point.y-1, 3, 3, SOLID);
    }

    int8_t x = -100 + 200*i/(crv.points-1);
    if (crv.custom && i>0 && i<crv.points-1) x = crv.crv[crv.points+i-1];
    lcdDrawText(7, 2*FH, PSTR("x=")); lcdDrawNumber(7+2*FW, 2*FH, x, LEFT);
    lcdDrawText(7, 3*FH, PSTR("y=")); lcdDrawNumber(7+2*FW, 3*FH, crv.crv[i], LEFT);
    lcdDrawRect(3, 1*FH+4, 7*FW-2, 3*FH-2);

    if (p1valdiff || event==EVT_KEY_FIRST(KEY_DOWN) || event==EVT_KEY_FIRST(KEY_UP) || event==EVT_KEY_REPT(KEY_DOWN) || event==EVT_KEY_REPT(KEY_UP))
      CHECK_INCDEC_MODELVAR(event, crv.crv[i], -100, 100);  // edit Y on up/down

    if (i>0 && i<crv.points-1 && s_editMode==2 && (event==EVT_KEY_FIRST(KEY_LEFT) || event==EVT_KEY_FIRST(KEY_RIGHT) || event==EVT_KEY_REPT(KEY_LEFT) || event==EVT_KEY_REPT(KEY_RIGHT)))
      CHECK_INCDEC_MODELVAR(event, crv.crv[crv.points+i-1], i==1 ? -99 : crv.crv[crv.points+i-2]+1, i==crv.points-2 ? 99 : crv.crv[crv.points+i]-1);  // edit X on left/right
  }
}
