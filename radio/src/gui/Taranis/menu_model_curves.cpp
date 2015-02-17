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

uint8_t s_curveChan;

int16_t curveFn(int16_t x)
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
    result.x = X0-1-WCHART+i*WCHART*2/(count-1);
    result.y = (LCD_H-1) - (100 + points[i]) * (LCD_H-1) / 200;
    if (custom && i>0 && i<count-1)
      result.x = X0-1-WCHART + (100 + (100 + points[count+i-1]) * (2*WCHART)) / 200;
  }
  return result;
}

void DrawCurve(uint8_t offset=0)
{
  DrawFunction(curveFn, offset);

  uint8_t i = 0;
  do {
    point_t point = getPoint(i);
    i++;
    if (point.x == 0) break;
    drawFilledRect(point.x-offset, point.y-1, 3, 3, SOLID, FORCE); // do markup square
  } while(1);
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

  eeDirty(EE_MODEL);
  return true;
}

void displayPresetChoice(uint8_t event)
{
  displayWarning(event);
  lcd_outdezAtt(WARNING_LINE_X+FW*7, WARNING_LINE_Y, 45*s_warning_input_value/4, LEFT|INVERS);
  lcd_putcAtt(lcdLastPos, WARNING_LINE_Y, '@', INVERS);

  if (s_warning_result) {
    s_warning_result = 0;
    CurveInfo & crv = g_model.curves[s_curveChan];
    int8_t * points = curveAddress(s_curveChan);
    for (uint8_t i=0; i<5+crv.points; i++)
      points[i] = (i-((5+crv.points)/2)) * s_warning_input_value * 50 / (4+crv.points);
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

void menuModelCurveOne(uint8_t event)
{
  static uint8_t pointsOfs = 0;
  CurveInfo & crv = g_model.curves[s_curveChan];
  int8_t * points = curveAddress(s_curveChan);

  lcd_puts(9*FW, 0, TR_PT "\003X\006Y");
  drawFilledRect(0, 0, LCD_W, FH, SOLID, FILL_WHITE|GREY_DEFAULT);

  SIMPLE_SUBMENU(STR_MENUCURVE, 4 + 5+crv.points + (crv.type==CURVE_TYPE_CUSTOM ? 5+crv.points-2 : 0));
  lcd_outdezAtt(PSIZE(TR_MENUCURVE)*FW+1, 0, s_curveChan+1, INVERS|LEFT);

  lcd_putsLeft(FH+1, STR_NAME);
  editName(INDENT_WIDTH, 2*FH+1, g_model.curveNames[s_curveChan], sizeof(g_model.curveNames[s_curveChan]), event, m_posVert==0);

  uint8_t attr = (m_posVert==1 ? (s_editMode>0 ? INVERS|BLINK : INVERS) : 0);
  lcd_putsLeft(3*FH+1, STR_TYPE);
  lcd_putsiAtt(INDENT_WIDTH, 4*FH+1, STR_CURVE_TYPES, crv.type, attr);
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

  attr = (m_posVert==2 ? (s_editMode>0 ? INVERS|BLINK : INVERS) : 0);
  lcd_putsLeft(5*FH+1, STR_COUNT);
  lcd_outdezAtt(INDENT_WIDTH, 6*FH+1, 5+crv.points, LEFT|attr);
  lcd_putsAtt(lcdLastPos, 6*FH+1, STR_PTS, attr);
  if (attr) {
    int8_t count = checkIncDecModel(event, crv.points, -3, 12); // 2pts - 17pts
    if (checkIncDec_Ret) {
      int8_t newPoints[MAX_POINTS];
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

  lcd_putsLeft(7*FH+1, STR_SMOOTH);
  menu_lcd_onoff(7*FW, 7*FH+1, crv.smooth, m_posVert==3 ? INVERS : 0);
  if (m_posVert==3) crv.smooth = checkIncDecModel(event, crv.smooth, 0, 1);

  switch(event) {
    case EVT_ENTRY:
      pointsOfs = 0;
      SET_SCROLLBAR_X(0);
      break;
    case EVT_KEY_LONG(KEY_ENTER):
      if (m_posVert > 1) {
        killEvents(event);
        MENU_ADD_ITEM(STR_CURVE_PRESET);
        MENU_ADD_ITEM(STR_MIRROR);
        MENU_ADD_ITEM(STR_CLEAR);
        menuHandler = onCurveOneMenu;
      }
      break;
    case EVT_KEY_LONG(KEY_MENU):
      pushMenu(menuChannelsView);
      killEvents(event);
  }

  DrawCurve(FW);

  uint8_t posY = FH+1;
  attr = (s_editMode > 0 ? INVERS|BLINK : INVERS);
  for (uint8_t i=0; i<5+crv.points; i++) {
    point_t point = getPoint(i);
    uint8_t selectionMode = 0;
    if (crv.type==CURVE_TYPE_CUSTOM) {
      if (m_posVert==4+2*i || (i==5+crv.points-1 && m_posVert==4+5+crv.points+5+crv.points-2-1))
        selectionMode = 2;
      else if (i>0 && m_posVert==3+2*i)
        selectionMode = 1;
    }
    else if (m_posVert == 4+i) {
      selectionMode = 2;
    }

    if (i>=pointsOfs && i<pointsOfs+7) {
      int8_t x = -100 + 200*i/(5+crv.points-1);
      if (crv.type==CURVE_TYPE_CUSTOM && i>0 && i<5+crv.points-1) x = points[5+crv.points+i-1];
      lcd_outdezAtt(6+8*FW,  posY, i+1, LEFT);
      lcd_outdezAtt(3+12*FW, posY, x, LEFT|(selectionMode==1?attr:0));
      lcd_outdezAtt(3+16*FW, posY, points[i], LEFT|(selectionMode==2?attr:0));
      posY += FH;
    }

    if (selectionMode > 0) {
      // do selection square
      drawFilledRect(point.x-FW-1, point.y-2, 5, 5, SOLID, FORCE);
      drawFilledRect(point.x-FW, point.y-1, 3, 3, SOLID);
      if (s_editMode > 0) {
        if (selectionMode == 1)
          CHECK_INCDEC_MODELVAR(event, points[5+crv.points+i-1], i==1 ? -100 : points[5+crv.points+i-2], i==5+crv.points-2 ? 100 : points[5+crv.points+i]);  // edit X
        else if (selectionMode == 2)
          CHECK_INCDEC_MODELVAR(event, points[i], -100, 100);
      }
      if (i < pointsOfs)
        pointsOfs = i;
      else if (i > pointsOfs+6)
        pointsOfs = i-6;
    }
  }
}

void editCurveRef(coord_t x, coord_t y, CurveRef & curve, uint8_t event, uint8_t attr)
{
  lcd_putsiAtt(x, y, "\004DiffExpoFuncCstm", curve.type, m_posHorz==0 ? attr : 0);
  if (attr && m_posHorz==0) {
    CHECK_INCDEC_MODELVAR_ZERO(event, curve.type, CURVE_REF_CUSTOM);
    if (checkIncDec_Ret) curve.value = 0;
  }
  switch (curve.type) {
    case CURVE_REF_DIFF:
    case CURVE_REF_EXPO:
      curve.value = GVAR_MENU_ITEM(x+5*FW, y, curve.value, -100, 100, m_posHorz==1 ? LEFT|attr : LEFT, 0, event);
      break;
    case CURVE_REF_FUNC:
      lcd_putsiAtt(x+5*FW, y, STR_VCURVEFUNC, curve.value, m_posHorz==1 ? attr : 0);
      if (attr && m_posHorz==1) CHECK_INCDEC_MODELVAR_ZERO(event, curve.value, CURVE_BASE-1);
      break;
    case CURVE_REF_CUSTOM:
      putsCurve(x+5*FW+2, y, curve.value, m_posHorz==1 ? attr : 0);
      if (attr && m_posHorz==1) {
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

void menuModelCurvesAll(uint8_t event)
{
  SIMPLE_MENU(STR_MENUCURVES, menuTabModel, e_CurvesAll, MAX_CURVES);

  int  sub = m_posVert;

  switch (event) {
    case EVT_KEY_BREAK(KEY_ENTER):
      if (!READ_ONLY()) {
        s_curveChan = sub;
        pushMenu(menuModelCurveOne);
      }
      break;
  }

  for (int i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    int k = i + s_pgOfs;
    LcdFlags attr = (sub == k ? INVERS : 0);
    {
      putsStrIdx(0, y, STR_CV, k+1, attr);
      editName(4*FW, y, g_model.curveNames[k], sizeof(g_model.curveNames[k]), 0, 0);
      CurveInfo & crv = g_model.curves[k];
      lcd_outdezAtt(11*FW, y, 5+crv.points, LEFT);
      lcd_putsAtt(lcdLastPos, y, STR_PTS, 0);
    }
  }

  s_curveChan = sub;
  DrawCurve(23);
}
