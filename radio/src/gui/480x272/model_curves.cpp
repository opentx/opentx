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

void drawCurve(int x, int y, int width)
{
  drawFunction(applyCurrentCurve, x, y, width);
}

void displayPresetChoice(event_t event)
{
  runPopupWarning(event);
  lcdDrawNumber(WARNING_LINE_X, WARNING_INFOLINE_Y-10, 45*warningInputValue/4, LEFT|INVERS, 0, NULL, "@");

  if (warningResult) {
    warningResult = 0;
    CurveInfo & crv = g_model.curves[s_curveChan];
    int8_t * points = curveAddress(s_curveChan);
    int k = 25 * warningInputValue;
    int dx = 2000 / (5+crv.points-1);
    for (uint8_t i=0; i<5+crv.points; i++) {
      int x = -1000 + i * dx;
      points[i] = div_and_round(div_and_round(k * x, 100), 10);
    }
    if (crv.type == CURVE_TYPE_CUSTOM) {
      resetCustomCurveX(points, 5+crv.points);
    }
  }
}

void onCurveOneMenu(const char * result)
{
  if (result == STR_CURVE_PRESET) {
    POPUP_INPUT(STR_PRESET, displayPresetChoice, 0, -4, 4);
  }
  else if (result == STR_MIRROR) {
    CurveInfo & crv = g_model.curves[s_curveChan];
    int8_t * points = curveAddress(s_curveChan);
    for (int i=0; i<5+crv.points; i++)
      points[i] = -points[i];
  }
  else if (result == STR_CLEAR) {
    CurveInfo & crv = g_model.curves[s_curveChan];
    int8_t * points = curveAddress(s_curveChan);
    for (int i=0; i<5+crv.points; i++)
      points[i] = 0;
    if (crv.type == CURVE_TYPE_CUSTOM) {
      resetCustomCurveX(points, 5+crv.points);
    }
  }
}

#define MODEL_CURVE_ONE_2ND_COLUMN     130

enum MenuModelCurveOneItems {
  ITEM_CURVE_NAME,
  ITEM_CURVE_TYPE,
  ITEM_CURVE_POINTS,
  ITEM_CURVE_SMOOTH,
  ITEM_CURVE_COORDS1,
  ITEM_CURVE_COORDS2,
};

#define IS_COORDS1_LINE_NEEDED()       (crv.type==CURVE_TYPE_CUSTOM && crv.points > -3)

bool menuModelCurveOne(event_t event)
{
  static uint8_t pointsOfs = 0;
  CurveData & crv = g_model.curves[s_curveChan];
  int8_t * points = curveAddress(s_curveChan);

  SUBMENU_WITH_OPTIONS(STR_MENUCURVE, ICON_MODEL_CURVES, IS_COORDS1_LINE_NEEDED() ? 6 : 5, OPTION_MENU_NO_FOOTER, { 0, 0, 0, 0, uint8_t(5+crv.points-1), uint8_t(5+crv.points-1) });
  drawStringWithIndex(50, 3+FH, STR_CV, s_curveChan+1, MENU_TITLE_COLOR);
  lcdDrawSolidFilledRect(0, MENU_FOOTER_TOP, 250, MENU_FOOTER_HEIGHT, HEADER_BGCOLOR);

  if (IS_COORDS1_LINE_NEEDED() && menuVerticalPosition == ITEM_CURVE_COORDS1) {
    if (menuHorizontalPosition == 0) {
      if (CURSOR_MOVED_RIGHT(event))
        menuHorizontalPosition = 1;
      else
        menuVerticalPosition -= 1;
    }
    else if (menuHorizontalPosition == 4+crv.points) {
      if (CURSOR_MOVED_RIGHT(event))
        (menuHorizontalPosition = 0, menuVerticalPosition += 1);
      else
        menuHorizontalPosition -= 1;
    }
  }

  // Curve name
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP-FH, STR_NAME);
  editName(MODEL_CURVE_ONE_2ND_COLUMN, MENU_CONTENT_TOP-FH, crv.name, sizeof(crv.name), event, menuVerticalPosition==ITEM_CURVE_NAME);

  // Curve type
  LcdFlags attr = (menuVerticalPosition==ITEM_CURVE_TYPE ? (s_editMode>0 ? INVERS|BLINK : INVERS) : 0);
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP, "Type");
  lcdDrawTextAtIndex(MODEL_CURVE_ONE_2ND_COLUMN, MENU_CONTENT_TOP, STR_CURVE_TYPES, crv.type, attr);
  if (attr) {
    uint8_t newType = checkIncDecModelZero(event, crv.type, CURVE_TYPE_LAST);
    if (newType != crv.type) {
      for (int i = 1; i < 4 + crv.points; i++) {
        points[i] = calcRESXto100(applyCustomCurve(calc100toRESX(-100 + i * 200 / (4 + crv.points)), s_curveChan));
      }
      if (moveCurve(s_curveChan, checkIncDec_Ret > 0 ? 3 + crv.points : -3 - crv.points)) {
        if (newType == CURVE_TYPE_CUSTOM) {
          resetCustomCurveX(points, 5 + crv.points);
        }
        crv.type = newType;
      }
    }
  }

  // Curve points count
  attr = (menuVerticalPosition==ITEM_CURVE_POINTS ? (s_editMode>0 ? INVERS|BLINK : INVERS) : 0);
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + FH, STR_COUNT);
  lcdDrawNumber(MODEL_CURVE_ONE_2ND_COLUMN, MENU_CONTENT_TOP + FH, 5+crv.points, LEFT|attr, 0, NULL, STR_PTS);
  if (attr) {
    int count = checkIncDecModel(event, crv.points, -3, 12); // 2pts - 17pts
    if (checkIncDec_Ret) {
      int newPoints[MAX_POINTS_PER_CURVE];
      newPoints[0] = points[0];
      newPoints[4+count] = points[4+crv.points];
      for (int i=1; i<4+count; i++)
        newPoints[i] = calcRESXto100(applyCustomCurve(-RESX + (i * 2 * RESX) / (4 + count), s_curveChan));
      if (moveCurve(s_curveChan, checkIncDec_Ret*(crv.type==CURVE_TYPE_CUSTOM ? 2 :1))) {
        for (int i = 0; i < 5 + count; i++) {
          points[i] = newPoints[i];
          if (crv.type == CURVE_TYPE_CUSTOM && i != 0 && i != 4 + count)
            points[5 + count + i - 1] = -100 + (i * 200) / (4 + count);
        }
        crv.points = count;
      }
    }
  }

  // Curve smooth
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 2*FH, STR_SMOOTH);
  drawCheckBox(MODEL_CURVE_ONE_2ND_COLUMN, MENU_CONTENT_TOP + 2*FH, crv.smooth, menuVerticalPosition==ITEM_CURVE_SMOOTH ? INVERS : 0);
  if (menuVerticalPosition==ITEM_CURVE_SMOOTH) crv.smooth = checkIncDecModel(event, crv.smooth, 0, 1);

  switch(event) {
    case EVT_ENTRY:
      pointsOfs = 0;
      break;
    case EVT_KEY_LONG(KEY_ENTER):
      if (menuVerticalPosition > ITEM_CURVE_NAME) {
        killEvents(event);
        POPUP_MENU_ADD_ITEM(STR_CURVE_PRESET);
        POPUP_MENU_ADD_ITEM(STR_MIRROR);
        POPUP_MENU_ADD_ITEM(STR_CLEAR);
        POPUP_MENU_START(onCurveOneMenu);
      }
      break;
    // TODO?
    // case EVT_KEY_LONG(KEY_MENU):
    //  pushMenu(menuChannelsView);
    //  killEvents(event);
  }

  drawCurve(CURVE_CENTER_X, CURVE_CENTER_Y, CURVE_SIDE_WIDTH);
  drawCurveHorizontalScale();
  if (menuVerticalPosition < ITEM_CURVE_COORDS1) drawCurveVerticalScale(CURVE_CENTER_X-CURVE_SIDE_WIDTH-15);

  coord_t posX = 47;
  attr = (s_editMode > 0 ? INVERS|BLINK : INVERS);
  for (int i=0; i<5+crv.points; i++) {
    point_t point = getPoint(i);
    uint8_t selectionMode = 0;
    if (menuHorizontalPosition == i) {
      if (menuVerticalPosition == ITEM_CURVE_COORDS1)
        selectionMode = (IS_COORDS1_LINE_NEEDED() ? 1 : 2);
      else if (menuVerticalPosition == ITEM_CURVE_COORDS2)
        selectionMode = 2;
    }

    int8_t x = -100 + 200*i/(5+crv.points-1);
    if (crv.type==CURVE_TYPE_CUSTOM && i>0 && i<5+crv.points-1) {
      x = points[5+crv.points+i-1];
    }

    if (i>=pointsOfs && i<pointsOfs+5) {
      lcdDrawNumber(posX, MENU_CONTENT_TOP + 5*FH-12, i+1, TEXT_DISABLE_COLOR|RIGHT);
      lcdDrawNumber(posX, MENU_CONTENT_TOP + 6*FH-10,   x, RIGHT|(selectionMode==1 ? attr : 0));
      lcdDrawNumber(posX, MENU_CONTENT_TOP + 7*FH-6, points[i], RIGHT|(selectionMode==2 ? attr : 0));
      posX += 45;
    }

    if (selectionMode > 0) {
      lcdDrawSolidFilledRect(point.x, CURVE_CENTER_Y-CURVE_SIDE_WIDTH, 2, 2*CURVE_SIDE_WIDTH+2, CURVE_CURSOR_COLOR);

      char text[5];
      strAppendSigned(text, points[i]);

      if (point.x >= CURVE_CENTER_X) {
        drawCurveVerticalScale(point.x-15);
        if (points[i-1] > points[i]) {
          drawCurveCoord(point.x+1-CURVE_COORD_WIDTH, point.y, text, selectionMode==2);
        }
        else {
          drawCurveCoord(point.x+1-CURVE_COORD_WIDTH, point.y-CURVE_COORD_HEIGHT-1, text, selectionMode==2);
        }
      }
      else {
        drawCurveVerticalScale(point.x+7);
        if (points[i+1] > points[i]) {
          drawCurveCoord(point.x+1, point.y, text, selectionMode==2);
        }
        else {
          drawCurveCoord(point.x+1, point.y-CURVE_COORD_HEIGHT-1, text, selectionMode==2);
        }
      }

      drawCurvePoint(point.x-3, point.y-4, CURVE_CURSOR_COLOR);

      strAppendSigned(text, x);
      drawCurveCoord(limit(CURVE_CENTER_X-CURVE_SIDE_WIDTH-1, point.x-CURVE_COORD_WIDTH/2, CURVE_CENTER_X+CURVE_SIDE_WIDTH-CURVE_COORD_WIDTH+1), CURVE_CENTER_Y+CURVE_SIDE_WIDTH+2, text, selectionMode==1);

      if (s_editMode > 0) {
        if (selectionMode == 1)
          CHECK_INCDEC_MODELVAR(event, points[5+crv.points+i-1], i==1 ? -100 : points[5+crv.points+i-2], i==5+crv.points-2 ? 100 : points[5+crv.points+i]);  // edit X
        else if (selectionMode == 2)
          CHECK_INCDEC_MODELVAR(event, points[i], -100, 100);
      }
      if (i < pointsOfs)
        pointsOfs = i;
      else if (i > pointsOfs+5-1)
        pointsOfs = i-5+1;
    }
    else {
      drawCurvePoint(point.x-3, point.y-4, TEXT_COLOR);
    }
  }

  lcdDrawHorizontalLine(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 6*FH - 13, SUBMENU_LINE_WIDTH, DOTTED, CURVE_AXIS_COLOR);
  lcdDrawHorizontalLine(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 7*FH - 10, SUBMENU_LINE_WIDTH, DOTTED, CURVE_AXIS_COLOR);
  drawHorizontalScrollbar(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 9*FH - 15, SUBMENU_LINE_WIDTH, pointsOfs, 5+crv.points, 5);

  return true;
}

void editCurveRef(coord_t x, coord_t y, CurveRef & curve, event_t event, LcdFlags attr)
{
  lcdDrawTextAtIndex(x, y, "\004DiffExpoFuncCstm", curve.type, (menuHorizontalPosition==0 ? attr : 0));
  if (attr && menuHorizontalPosition==0) {
    CHECK_INCDEC_MODELVAR_ZERO(event, curve.type, CURVE_REF_CUSTOM);
    if (checkIncDec_Ret) curve.value = 0;
  }
  switch (curve.type) {
    case CURVE_REF_DIFF:
    case CURVE_REF_EXPO:
      curve.value = GVAR_MENU_ITEM(lcdNextPos+10, y, curve.value, -100, 100, menuHorizontalPosition==1 ? LEFT|attr : LEFT, 0, event);
      break;
    case CURVE_REF_FUNC:
      lcdDrawTextAtIndex(lcdNextPos+10, y, STR_VCURVEFUNC, curve.value, (menuHorizontalPosition==1 ? attr : 0));
      if (attr && menuHorizontalPosition==1) CHECK_INCDEC_MODELVAR_ZERO(event, curve.value, CURVE_BASE-1);
      break;
    case CURVE_REF_CUSTOM:
      drawCurveName(lcdNextPos+10, y, curve.value, (menuHorizontalPosition==1 ? attr : 0));
      if (attr && menuHorizontalPosition==1) {
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

#define CURVES_NAME_POS                60
#define CURVES_POINTS_POS              120

bool menuModelCurvesAll(event_t event)
{
  SIMPLE_MENU(STR_MENUCURVES, MODEL_ICONS, menuTabModel, MENU_MODEL_CURVES, MAX_CURVES);

  s_curveChan = menuVerticalPosition;

  switch (event) {
    case EVT_KEY_BREAK(KEY_ENTER):
      if (!READ_ONLY()) {
        pushMenu(menuModelCurveOne);
      }
      break;
  }

  for (int i=0; i<NUM_BODY_LINES; ++i) {
    coord_t y = MENU_CONTENT_TOP + i*FH;
    uint8_t k = i + menuVerticalOffset;
    LcdFlags attr = (menuVerticalPosition == k ? INVERS : 0);
    {
      drawStringWithIndex(MENUS_MARGIN_LEFT, y, STR_CV, k+1, attr);
      CurveData & crv = g_model.curves[k];
      editName(CURVES_NAME_POS, y, crv.name, sizeof(crv.name), 0, 0);
      lcdDrawNumber(CURVES_POINTS_POS, y, 5+crv.points, LEFT, 0, NULL, STR_PTS);
    }
  }

  drawCurve(CURVE_CENTER_X, CURVE_CENTER_Y+10, 80);

  return true;
}
