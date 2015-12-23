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

#include <stdio.h>
#include "../../opentx.h"

uint8_t s_curveChan;

int curveFn(int x)
{
  return applyCustomCurve(x, s_curveChan);
}

struct point_t {
  coord_t x;
  coord_t y;
};

point_t getPoint(uint8_t i)
{
  point_t result = {0, 0};
  CurveInfo &crv = g_model.curves[s_curveChan];
  int8_t *points = curveAddress(s_curveChan);
  bool custom = (crv.type == CURVE_TYPE_CUSTOM);
  uint8_t count = 5+crv.points;
  if (i < count) {
    result.x = CURVE_CENTER_X-1-CURVE_SIDE_WIDTH+i*CURVE_SIDE_WIDTH*2/(count-1);
    result.y = CURVE_CENTER_Y - (points[i]) * (CURVE_SIDE_WIDTH-1) / 100;
    if (custom && i>0 && i<count-1)
      result.x = CURVE_CENTER_X-1-CURVE_SIDE_WIDTH + (100 + (100 + points[count+i-1]) * (2*CURVE_SIDE_WIDTH)) / 200;
  }
  return result;
}

void DrawCurve(int offset=0)
{
  drawFunction(curveFn, offset);

  /*int i = 0;
  do {
    point_t point = getPoint(i++);
    if (point.x == 0) break;
    lcdDrawSolidFilledRect(point.x-offset, point.y-1, 3, 3, TEXT_COLOR); // do markup square
  } while(1);*/
}

extern int8_t * curveEnd[MAX_CURVES];
bool moveCurve(uint8_t index, int8_t shift)
{
  if (curveEnd[MAX_CURVES-1] + shift > g_model.points + sizeof(g_model.points)) {
    AUDIO_WARNING2();
    return false;
  }

  int8_t *nextCrv = curveAddress(index+1);
  memmove(nextCrv+shift, nextCrv, 5*(MAX_CURVES-index-1)+curveEnd[MAX_CURVES-1]-curveEnd[index]);
  if (shift < 0) memclear(&g_model.points[NUM_POINTS-1] + shift, -shift);
  while (index<MAX_CURVES) {
    curveEnd[index++] += shift;
  }

  storageDirty(EE_MODEL);
  return true;
}

void displayPresetChoice(evt_t event)
{
  displayWarning(event);
  lcdDrawNumber(WARNING_LINE_X, WARNING_INFOLINE_Y-10, 45*warningInputValue/4, LEFT|INVERS, 0, NULL, "@");

  if (warningResult) {
    warningResult = 0;
    CurveInfo & crv = g_model.curves[s_curveChan];
    int8_t * points = curveAddress(s_curveChan);
    for (int i=0; i<5+crv.points; i++)
      points[i] = (i-((5+crv.points)/2)) * warningInputValue * 50 / (4+crv.points);
    if (crv.type == CURVE_TYPE_CUSTOM) {
      for (int i=0; i<3+crv.points; i++)
        points[crv.points+i] = -100 + ((i+1)*200) / (4+crv.points);
    }
  }
}

void onCurveOneMenu(const char *result)
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
      for (int i=0; i<3+crv.points; i++)
        points[crv.points+i] = -100 + ((i+1)*200) / (4+crv.points);
    }
  }
}

#define MODEL_CURVE_ONE_2ND_COLUMN        130

enum MenuModelCurveOneItems {
  ITEM_CURVE_NAME,
  ITEM_CURVE_TYPE,
  ITEM_CURVE_POINTS,
  ITEM_CURVE_SMOOTH,
  ITEM_CURVE_COORDS1,
  ITEM_CURVE_COORDS2,
};

bool menuModelCurveOne(evt_t event)
{
  static uint8_t pointsOfs = 0;
  CurveInfo & crv = g_model.curves[s_curveChan];
  int8_t * points = curveAddress(s_curveChan);

  SUBMENU(STR_MENUCURVE, crv.type==CURVE_TYPE_CUSTOM ? 6 : 5, 0, { 0, 0, 0, 0, uint8_t(5+crv.points-1), uint8_t(5+crv.points-1) });

  lcdDrawNumber(MENU_TITLE_NEXT_POS, MENU_TITLE_TOP+1, s_curveChan+1, LEFT|TEXT_COLOR);

  // Curve name
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP, STR_NAME);
  editName(MODEL_CURVE_ONE_2ND_COLUMN, MENU_CONTENT_TOP, g_model.curveNames[s_curveChan], sizeof(g_model.curveNames[s_curveChan]), event, menuVerticalPosition==ITEM_CURVE_NAME);

  // Curve type
  LcdFlags attr = (menuVerticalPosition==ITEM_CURVE_TYPE ? (s_editMode>0 ? INVERS|BLINK : INVERS) : 0);
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + FH, "Type");
  lcdDrawTextAtIndex(MODEL_CURVE_ONE_2ND_COLUMN, MENU_CONTENT_TOP + FH, STR_CURVE_TYPES, crv.type, attr);
  if (attr) {
    uint8_t newType = checkIncDecModelZero(event, crv.type, CURVE_TYPE_LAST);
    if (newType != crv.type) {
      for (int i=1; i<4+crv.points; i++)
        points[i] = calcRESXto100(applyCustomCurve(calc100toRESX(-100 + i*200/(4+crv.points)), s_curveChan));
      moveCurve(s_curveChan, checkIncDec_Ret > 0 ? 3+crv.points : -3-crv.points);
      if (newType == CURVE_TYPE_CUSTOM) {
        for (int i=0; i<3+crv.points; i++)
          points[5+crv.points+i] = -100 + ((i+1)*200) / (4+crv.points);
      }
      crv.type = newType;
    }
  }

  // Curve points count
  attr = (menuVerticalPosition==ITEM_CURVE_POINTS ? (s_editMode>0 ? INVERS|BLINK : INVERS) : 0);
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 2*FH, STR_COUNT);
  lcdDrawNumber(MODEL_CURVE_ONE_2ND_COLUMN, MENU_CONTENT_TOP + 2*FH, 5+crv.points, LEFT|attr, 0, NULL, STR_PTS);
  if (attr) {
    int count = checkIncDecModel(event, crv.points, -3, 12); // 2pts - 17pts
    if (checkIncDec_Ret) {
      int newPoints[MAX_POINTS];
      newPoints[0] = points[0];
      newPoints[4+count] = points[4+crv.points];
      for (int i=1; i<4+count; i++)
        newPoints[i] = calcRESXto100(applyCustomCurve(calc100toRESX(-100 + (i*200) / (4+count)), s_curveChan));
      moveCurve(s_curveChan, checkIncDec_Ret*(crv.type==CURVE_TYPE_CUSTOM?2:1));
      for (int i=0; i<5+count; i++) {
        points[i] = newPoints[i];
        if (crv.type == CURVE_TYPE_CUSTOM && i!=0 && i!=4+count)
          points[5+count+i-1] = -100 + (i*200) / (4+count);
      }
      crv.points = count;
    }
  }

  // Curve smooth
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 3*FH, STR_SMOOTH);
  drawCheckBox(MODEL_CURVE_ONE_2ND_COLUMN, MENU_CONTENT_TOP + 3*FH, crv.smooth, menuVerticalPosition==ITEM_CURVE_SMOOTH ? INVERS : 0);
  if (menuVerticalPosition==ITEM_CURVE_SMOOTH) crv.smooth = checkIncDecModel(event, crv.smooth, 0, 1);

  switch(event) {
    case EVT_ENTRY:
      pointsOfs = 0;
      break;
    case EVT_KEY_LONG(KEY_ENTER):
      if (menuVerticalPosition > ITEM_CURVE_POINTS) {
        killEvents(event);
        POPUP_MENU_ADD_ITEM(STR_CURVE_PRESET);
        POPUP_MENU_ADD_ITEM(STR_MIRROR);
        POPUP_MENU_ADD_ITEM(STR_CLEAR);
        popupMenuHandler = onCurveOneMenu;
      }
      break;
    case EVT_KEY_LONG(KEY_MENU):
      pushMenu(menuChannelsView);
      killEvents(event);
  }

  DrawCurve();
  drawCurveHorizontalScale();
  if (menuVerticalPosition < ITEM_CURVE_COORDS1) drawCurveVerticalScale(CURVE_CENTER_X-CURVE_SIDE_WIDTH-15);

  coord_t posX = 47;
  attr = (s_editMode > 0 ? INVERS|BLINK : INVERS);
  for (int i=0; i<5+crv.points; i++) {
    point_t point = getPoint(i);
    uint8_t selectionMode = 0;
    if (menuHorizontalPosition == i) {
      if (menuVerticalPosition == ITEM_CURVE_COORDS1)
        selectionMode = (crv.type==CURVE_TYPE_CUSTOM ? 1 : 2);
      else if (menuVerticalPosition == ITEM_CURVE_COORDS2)
        selectionMode = 2;
    }

    if (selectionMode == 1) {
      if (menuHorizontalPosition == 0) {
        REPEAT_LAST_CURSOR_MOVE(1);
      }
      else if (menuHorizontalPosition == 4+crv.points) {
        REPEAT_LAST_CURSOR_MOVE(3+crv.points);
      }
    }

    int8_t x = -100 + 200*i/(5+crv.points-1);
    if (crv.type==CURVE_TYPE_CUSTOM && i>0 && i<5+crv.points-1) x = points[5+crv.points+i-1];

    if (i>=pointsOfs && i<pointsOfs+5) {
      lcdDrawNumber(posX, MENU_CONTENT_TOP + 5*FH, i+1, TEXT_DISABLE_COLOR);
      lcdDrawNumber(posX, MENU_CONTENT_TOP + 6*FH+2, x, (selectionMode==1 ? attr : 0));
      lcdDrawNumber(posX, MENU_CONTENT_TOP + 7*FH+6, points[i], (selectionMode==2 ? attr : 0));
      posX += 45;
    }

    if (selectionMode > 0) {
      lcdDrawSolidFilledRect(point.x, CURVE_CENTER_Y-CURVE_SIDE_WIDTH, 2, 2*CURVE_SIDE_WIDTH+2, CURVE_CURSOR_COLOR);

      char text[5];
      sprintf(text, "%d", points[i]);

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

      sprintf(text, "%d", x);
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

  lcdDrawHorizontalLine(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 6*FH - 1, SUBMENU_LINE_WIDTH, DOTTED, CURVE_AXIS_COLOR);
  lcdDrawHorizontalLine(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 7*FH + 2, SUBMENU_LINE_WIDTH, DOTTED, CURVE_AXIS_COLOR);
  drawHorizontalScrollbar(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 9*FH - 1, SUBMENU_LINE_WIDTH, pointsOfs, 5+crv.points, 5);

  return true;
}

void editCurveRef(coord_t x, coord_t y, CurveRef & curve, evt_t event, uint8_t attr)
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
      putsCurve(lcdNextPos+10, y, curve.value, (menuHorizontalPosition==1 ? attr : 0));
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

bool menuModelCurvesAll(evt_t event)
{
  SIMPLE_MENU(STR_MENUCURVES, menuTabModel, e_CurvesAll, MAX_CURVES, DEFAULT_SCROLLBAR_X);

  int8_t  sub = menuVerticalPosition;

  switch (event) {
    case EVT_KEY_BREAK(KEY_ENTER):
      if (!READ_ONLY()) {
        s_curveChan = sub;
        pushMenu(menuModelCurveOne);
      }
      break;
  }

  for (int i=0; i<NUM_BODY_LINES; ++i) {
    coord_t y = MENU_CONTENT_TOP + i*FH;
    uint8_t k = i + menuVerticalOffset;
    LcdFlags attr = (sub == k ? INVERS : 0);
    {
      putsStrIdx(MENUS_MARGIN_LEFT, y, STR_CV, k+1, attr);
      editName(50, y, g_model.curveNames[k], sizeof(g_model.curveNames[k]), 0, 0);
      CurveInfo & crv = g_model.curves[k];
      lcdDrawNumber(100, y, 5+crv.points, LEFT, 0, NULL, STR_PTS);
    }
  }

  if (sub >= 0) {
    s_curveChan = sub;
    DrawCurve(23);
  }

  return true;
}
