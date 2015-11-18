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
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
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

coord_t getCurveYCoord(FnFuncP fn, coord_t x)
{
  return limit(0, CURVE_CENTER_Y - divRoundClosest(fn(divRoundClosest(x * RESX, CURVE_SIDE_WIDTH)) * CURVE_SIDE_WIDTH, RESX), LCD_H-1);
}

void drawFunction(FnFuncP fn, int offset)
{
  int left = CURVE_CENTER_X-offset-CURVE_SIDE_WIDTH;
  int right = CURVE_CENTER_X-offset+CURVE_SIDE_WIDTH;
  int center = CURVE_CENTER_X-offset;

  // Axis
  lcdDrawSolidHorizontalLine(left, CURVE_CENTER_Y, CURVE_SIDE_WIDTH*2+1, CURVE_AXIS_COLOR);
  lcdDrawSolidVerticalLine(center, CURVE_CENTER_Y-CURVE_SIDE_WIDTH, CURVE_SIDE_WIDTH*2, CURVE_AXIS_COLOR);

  // Extra lines
  lcdDrawVerticalLine(left+CURVE_SIDE_WIDTH/2, CURVE_CENTER_Y-CURVE_SIDE_WIDTH, CURVE_SIDE_WIDTH*2, STASHED, CURVE_AXIS_COLOR);
  lcdDrawVerticalLine(right-CURVE_SIDE_WIDTH/2, CURVE_CENTER_Y-CURVE_SIDE_WIDTH, CURVE_SIDE_WIDTH*2, STASHED, CURVE_AXIS_COLOR);
  lcdDrawHorizontalLine(left, CURVE_CENTER_Y-CURVE_SIDE_WIDTH/2, CURVE_SIDE_WIDTH*2+1, STASHED, CURVE_AXIS_COLOR);
  lcdDrawHorizontalLine(left, CURVE_CENTER_Y+CURVE_SIDE_WIDTH/2, CURVE_SIDE_WIDTH*2+1, STASHED, CURVE_AXIS_COLOR);

  // Outside border
  lcdDrawSolidVerticalLine(left, CURVE_CENTER_Y-CURVE_SIDE_WIDTH, CURVE_SIDE_WIDTH*2, TEXT_COLOR);
  lcdDrawSolidVerticalLine(right, CURVE_CENTER_Y-CURVE_SIDE_WIDTH, CURVE_SIDE_WIDTH*2, TEXT_COLOR);
  lcdDrawSolidHorizontalLine(left, CURVE_CENTER_Y-CURVE_SIDE_WIDTH, CURVE_SIDE_WIDTH*2+1, TEXT_COLOR);
  lcdDrawSolidHorizontalLine(left, CURVE_CENTER_Y+CURVE_SIDE_WIDTH, CURVE_SIDE_WIDTH*2+1, TEXT_COLOR);

  coord_t prev_yv = (coord_t)-1;

  for (int xv=-CURVE_SIDE_WIDTH; xv<=CURVE_SIDE_WIDTH; xv+=1) {
    coord_t yv = getCurveYCoord(fn, xv);
    if (prev_yv != (coord_t)-1) {
      if (prev_yv < yv) {
        for (int y=prev_yv; y<=yv; y+=1) {
          lcdDrawBitmapPattern(CURVE_CENTER_X+xv-offset-2, y-2, LBM_POINT, TEXT_COLOR);
        }
      }
      else {
        for (int y=yv; y<=prev_yv; y+=1) {
          lcdDrawBitmapPattern(CURVE_CENTER_X+xv-offset-2, y-2, LBM_POINT, TEXT_COLOR);
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
  lcdDrawSolidFilledRect(x, y, CURVE_COORD_WIDTH, CURVE_COORD_HEIGHT, CURVE_CURSOR_COLOR);
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
