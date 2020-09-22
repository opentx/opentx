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

coord_t getCurveYCoord(FnFuncP fn, int x, int width)
{
  return limit(-width, -divRoundClosest(fn(divRoundClosest(x * RESX, width)) * width, RESX), +width);
}

void drawFunction(FnFuncP fn, int x, int y, int width)
{
  int left = x - width;
  int right = x + width;

  // Axis
  lcdDrawSolidHorizontalLine(left, y, width*2+1, CURVE_AXIS_COLOR);
  lcdDrawSolidVerticalLine(x, y-width, width*2, CURVE_AXIS_COLOR);

  // Extra lines
  lcdDrawVerticalLine(left+width/2, y-width, width*2, STASHED, CURVE_AXIS_COLOR);
  lcdDrawVerticalLine(right-width/2, y-width, width*2, STASHED, CURVE_AXIS_COLOR);
  lcdDrawHorizontalLine(left, y-width/2, width*2+1, STASHED, CURVE_AXIS_COLOR);
  lcdDrawHorizontalLine(left, y+width/2, width*2+1, STASHED, CURVE_AXIS_COLOR);

  // Outside border
  lcdDrawSolidVerticalLine(left, y-width, width*2, TEXT_COLOR);
  lcdDrawSolidVerticalLine(right, y-width, width*2, TEXT_COLOR);
  lcdDrawSolidHorizontalLine(left, y-width, width*2+1, TEXT_COLOR);
  lcdDrawSolidHorizontalLine(left, y+width, width*2+1, TEXT_COLOR);

  coord_t prev_yv = (coord_t)-1;

  for (int xv=-width; xv<=width; xv+=1) {
    coord_t yv = y + getCurveYCoord(fn, xv, width);
    if (prev_yv != (coord_t)-1) {
      if (prev_yv < yv) {
        for (int y=prev_yv; y<=yv; y+=1) {
          lcdDrawBitmapPattern(x+xv-2, y-2, LBM_POINT, TEXT_COLOR);
        }
      }
      else {
        for (int y=yv; y<=prev_yv; y+=1) {
          lcdDrawBitmapPattern(x+xv-2, y-2, LBM_POINT, TEXT_COLOR);
        }
      }
    }
    prev_yv = yv;
  }
}

void drawCurveVerticalScale(int x)
{
  for (int i=0; i<=20; i++) {
    lcdDrawSolidHorizontalLine(x, CURVE_CENTER_Y-CURVE_SIDE_WIDTH+i*CURVE_SIDE_WIDTH/10, 10, TEXT_COLOR);
  }
}

void drawCurveHorizontalScale()
{
  for (int i=0; i<=20; i++) {
    lcdDrawSolidVerticalLine(CURVE_CENTER_X-CURVE_SIDE_WIDTH+i*CURVE_SIDE_WIDTH/10, CURVE_CENTER_Y+CURVE_SIDE_WIDTH+5, 10, TEXT_COLOR);
  }
}

void drawCurveCoord(int x, int y, const char * text, bool active)
{
  lcd->drawSolidFilledRect(x, y, CURVE_COORD_WIDTH, CURVE_COORD_HEIGHT, CURVE_CURSOR_COLOR);
  lcdDrawText(x+3+(CURVE_COORD_WIDTH-1-getTextWidth(text, SMLSIZE))/2, y+1, text, LEFT|SMLSIZE|TEXT_BGCOLOR);
  if (active) {
    lcdDrawBitmapPattern(x, y, LBM_CURVE_COORD_SHADOW, TEXT_COLOR);
  }
}

void drawCurvePoint(int x, int y, LcdFlags color)
{
  lcdDrawBitmapPattern(x, y, LBM_CURVE_POINT, color);
  lcdDrawBitmapPattern(x, y, LBM_CURVE_POINT_CENTER, TEXT_BGCOLOR);
}

void drawCursor(FnFuncP fn)
{
  char textx[5];
  char texty[5];
  int x = getValue(s_currSrcRaw);
  if (s_currSrcRaw >= MIXSRC_FIRST_TELEM) {
    strAppendUnsigned(textx, calcRESXto100(x));
    // TODO drawSensorCustomValue(LCD_W-8, 6*FH, ed->srcRaw - MIXSRC_FIRST_TELEM, x);
    if (s_currScale > 0)
      x = (x * 1024) / convertTelemValue(s_currSrcRaw - MIXSRC_FIRST_TELEM + 1, s_currScale);
  }
  else {
    strAppendSigned(textx, calcRESXto100(x));
  }
  
  x = limit(-1024, x, 1024);
  int y = limit<int>(-1024, applyCurrentCurve(x), 1024);
  strAppendSigned(texty, calcRESXto100(y));

  x = divRoundClosest(x * CURVE_SIDE_WIDTH, RESX);
  y = CURVE_CENTER_Y + getCurveYCoord(applyCurrentCurve, x, CURVE_SIDE_WIDTH);

  lcdDrawSolidFilledRect(CURVE_CENTER_X + x, CURVE_CENTER_Y - CURVE_SIDE_WIDTH, 2, 2 * CURVE_SIDE_WIDTH + 2, CURVE_CURSOR_COLOR);
  lcdDrawSolidFilledRect(CURVE_CENTER_X - CURVE_SIDE_WIDTH-2, y-1, 2*CURVE_SIDE_WIDTH + 2, 2, CURVE_CURSOR_COLOR);
  lcdDrawBitmapPattern(CURVE_CENTER_X + x - 4, y - 4, LBM_CURVE_POINT, CURVE_CURSOR_COLOR);
  lcdDrawBitmapPattern(CURVE_CENTER_X + x - 4, y - 4, LBM_CURVE_POINT_CENTER, TEXT_BGCOLOR);

  int left = limit(CURVE_CENTER_X - CURVE_SIDE_WIDTH, CURVE_CENTER_X - CURVE_COORD_WIDTH / 2 + x, CURVE_CENTER_X + CURVE_SIDE_WIDTH - CURVE_COORD_WIDTH + 2);
  drawCurveCoord(left, CURVE_CENTER_Y + CURVE_SIDE_WIDTH + 2, textx);
  int top = limit(CURVE_CENTER_Y - CURVE_SIDE_WIDTH - 1, -CURVE_COORD_HEIGHT / 2 + y, CURVE_CENTER_Y + CURVE_SIDE_WIDTH - CURVE_COORD_HEIGHT + 1);
  drawCurveCoord(CURVE_CENTER_X-CURVE_SIDE_WIDTH - 37, top, texty);
}
