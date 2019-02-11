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

#include "curveedit.h"
#include "keyboard_curve.h"
#include "opentx.h" // TODO for applyCustomCurve

CurveEdit::CurveEdit(Window * parent, const rect_t &rect, uint8_t index) :
  Curve(parent, rect, [=](int x) -> int {
    return applyCustomCurve(x, index);
  }),
  index(index),
  current(0)
{
  update();
}

void CurveEdit::update()
{
  clearPoints();
  CurveInfo & curve = g_model.curves[index];
  for (uint8_t i = 0; i < 5 + curve.points; i++) {
    if (hasFocus() && current == i) {
      position = [=] () -> int {
        return getPoint(index, i).x;
      };
    }
    else {
      addPoint(getPoint(index, i), TEXT_COLOR);
    }
  }
  invalidate();
}

bool CurveEdit::onTouchEnd(coord_t x, coord_t y)
{
  if (!hasFocus()) {
    setFocus();
    update();
  }

  CurveKeyboard * keyboard = CurveKeyboard::instance();
  if (keyboard->getField() != this) {
    keyboard->setField(this);
  }

  CurveInfo & curve = g_model.curves[index];
  for (int i=0; i<5 + curve.points; i++) {
    if (i != current) {
      point_t point = getPoint(index, i);
      if (abs(getPointX(point.x) - x) <= 10 && abs(getPointY(point.y) - y) <= 10) {
        current = i;
        update();
        break;
      }
    }
  }

  return true;
}

void CurveEdit::onFocusLost()
{
  CurveKeyboard::instance()->disable(true);
}

void CurveEdit::next()
{
  if (++current == points.size()) {
    current = 0;
  }
  update();
}

void CurveEdit::previous()
{
  if (current-- == 0) {
    current = points.size() - 1;
  }
  update();
}

void CurveEdit::up()
{
  int8_t & point = curveAddress(index)[current];
  point = min<int8_t>(100, ++point);
  storageDirty(EE_MODEL);
  invalidate();
}

void CurveEdit::down()
{
  int8_t & point = curveAddress(index)[current];
  point = max<int8_t>(-100, --point);
  storageDirty(EE_MODEL);
  invalidate();
}

void CurveEdit::right()
{
  CurveInfo & curve = g_model.curves[index];
  if (curve.type == CURVE_TYPE_CUSTOM && current != 0 && current != curve.points - 1) {
    int8_t * points = curveAddress(index);
    int8_t * point = &points[5 + curve.points + current - 1];
    int8_t xmax = (current == (curve.points - 2) ? +100 : *(point + 1));
    *point = min<int8_t>(*point + 1, xmax-1);
    storageDirty(EE_MODEL);
    invalidate();
  }
}

void CurveEdit::left()
{
  CurveInfo & curve = g_model.curves[index];
  if (curve.type == CURVE_TYPE_CUSTOM && current != 0 && current != curve.points - 1) {
    int8_t * points = curveAddress(index);
    int8_t * point = &points[5 + curve.points + current - 1];
    int8_t xmin = (current == 1 ? -100 : *(point - 1));
    *point = max<int8_t>(xmin+1, *point - 1);
    storageDirty(EE_MODEL);
    invalidate();
  }
}

bool CurveEdit::isCustomCurve()
{
  return g_model.curves[index].type == CURVE_TYPE_CUSTOM;
}
