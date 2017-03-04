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

#if defined(GVARS_IN_CURVES_SCREEN)
  #warning "define still not added to CMakeLists.txt"
  #define CURVE_SELECTED() (sub >= 0 && sub < MAX_CURVES)
  #define GVAR_SELECTED()  (sub >= MAX_CURVES)
#else
  #define CURVE_SELECTED() (sub >= 0)
#endif

void drawCurve(coord_t offset)
{
  drawFunction(applyCurrentCurve, offset);
  
  uint8_t i = 0;
  do {
    point_t point = getPoint(i);
    i++;
    if (point.x == 0) break;
    lcdDrawFilledRect(point.x-offset, point.y-1, 3, 3, SOLID, FORCE); // do markup square
  } while (1);
}

void menuModelCurvesAll(event_t event)
{
#if defined(GVARS_IN_CURVES_SCREEN)
  SIMPLE_MENU(STR_MENUCURVES, menuTabModel, MENU_MODEL_CURVES, HEADER_LINE+MAX_CURVES+MAX_GVARS);
#else
  SIMPLE_MENU(STR_MENUCURVES, menuTabModel, MENU_MODEL_CURVES, HEADER_LINE+MAX_CURVES);
#endif

  int8_t sub = menuVerticalPosition - HEADER_LINE;

  switch (event) {
#if defined(ROTARY_ENCODER_NAVIGATION)
    case EVT_ROTARY_BREAK:
#endif
#if !defined(PCBTARANIS)
    case EVT_KEY_FIRST(KEY_RIGHT):
#endif
    case EVT_KEY_FIRST(KEY_ENTER):
      if (CURVE_SELECTED() && !READ_ONLY()) {
        s_curveChan = sub;
        pushMenu(menuModelCurveOne);
      }
      break;
  }

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i + menuVerticalOffset;
    LcdFlags attr = (sub == k ? INVERS : 0);
#if defined(GVARS_IN_CURVES_SCREEN)
    if (k >= MAX_CURVES) {
      drawStringWithIndex(0, y, STR_GV, k-MAX_CURVES+1);
      if (GVAR_SELECTED()) {
        if (attr && s_editMode>0) attr |= BLINK;
        lcdDrawNumber(10*FW, y, GVAR_VALUE(k-MAX_CURVES, -1), attr);
        if (attr) g_model.gvars[k-MAX_CURVES] = checkIncDec(event, g_model.gvars[k-MAX_CURVES], -1000, 1000, EE_MODEL);
      }
    }
    else
#endif
    {
      drawStringWithIndex(0, y, STR_CV, k+1, attr);
#if defined(CPUARM)
      CurveData & crv = g_model.curves[k];
      editName(4*FW, y, crv.name, sizeof(crv.name), 0, 0);
#if LCD_W >= 212
      lcdDrawNumber(11*FW, y, 5+crv.points, LEFT);
      lcdDrawText(lcdLastRightPos, y, STR_PTS, 0);
#endif
#endif
    }
  }

  if (CURVE_SELECTED()) {
    s_curveChan = sub;
#if !defined(CPUARM) || LCD_W >= 212
    drawCurve(23);
#else
    drawCurve(10);
#endif
  }
}

#if defined(CPUARM)
void editCurveRef(coord_t x, coord_t y, CurveRef & curve, event_t event, LcdFlags flags)
{
  coord_t x1 = x;
  LcdFlags flags1 = flags;
  if (flags & RIGHT) {
    x1 -= 9*FW;
    flags1 -= RIGHT;
  }
  else {
    x += 5*FW;
  }

  uint8_t active = (flags & INVERS);

  if (menuHorizontalPosition == 0) {
    flags = flags & RIGHT;
  }
  else {
    flags1 = 0;
  }

  lcdDrawTextAtIndex(x1, y, "\004DiffExpoFuncCstm", curve.type, flags1);

  if (active && menuHorizontalPosition==0) {
    CHECK_INCDEC_MODELVAR_ZERO(event, curve.type, CURVE_REF_CUSTOM);
    if (checkIncDec_Ret) curve.value = 0;
  }
  switch (curve.type) {
    case CURVE_REF_DIFF:
    case CURVE_REF_EXPO:
      curve.value = GVAR_MENU_ITEM(x, y, curve.value, -100, 100, LEFT | flags, 0, event);
      break;
    case CURVE_REF_FUNC:
      lcdDrawTextAtIndex(x, y, STR_VCURVEFUNC, curve.value, flags);
      if (active && menuHorizontalPosition==1) CHECK_INCDEC_MODELVAR_ZERO(event, curve.value, CURVE_BASE-1);
      break;
    case CURVE_REF_CUSTOM:
      drawCurveName(x, y, curve.value, flags);
      if (active && menuHorizontalPosition==1) {
        if (event==EVT_KEY_LONG(KEY_ENTER) && curve.value!=0) {
          s_curveChan = (curve.value<0 ? -curve.value-1 : curve.value-1);
          pushMenu(menuModelCurveOne);
        }
        else {
          CHECK_INCDEC_MODELVAR(event, curve.value, -MAX_CURVES, MAX_CURVES);
        }
      }
      break;
  }
}
#endif
