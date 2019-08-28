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

#include "slider.h"
#include "bitmaps.h"
#include "opentx.h" // TODO remove this

void Slider::paint(BitmapBuffer * dc)
{
  int val = limit(vmin, getValue(), vmax);
  int w = divRoundClosest((rect.w - 16) * (val - vmin), vmax - vmin);

  if (editMode) {
    dc->drawBitmapPattern(0, 11, LBM_SLIDER_BAR_LEFT, TEXT_INVERTED_BGCOLOR);
    dc->drawSolidFilledRect(4, 11, rect.w - 8, 4, TEXT_INVERTED_BGCOLOR);
    dc->drawBitmapPattern(rect.w - 4, 11, LBM_SLIDER_BAR_RIGHT, TEXT_INVERTED_BGCOLOR);
  }
  else {
    dc->drawBitmapPattern(0, 11, LBM_SLIDER_BAR_LEFT, LINE_COLOR);
    dc->drawSolidFilledRect(4, 11, rect.w - 8, 4, LINE_COLOR);
    dc->drawBitmapPattern(rect.w - 4, 11, LBM_SLIDER_BAR_RIGHT, LINE_COLOR);
  }

  dc->drawBitmapPattern(w, 5, LBM_SLIDER_POINT_OUT, TEXT_COLOR);
  dc->drawBitmapPattern(w, 5, LBM_SLIDER_POINT_MID, TEXT_BGCOLOR);
  // if ((options & INVERS) && (!(options & BLINK) || !BLINK_ON_PHASE))
  if (hasFocus()) {
    dc->drawBitmapPattern(w, 5, LBM_SLIDER_POINT_IN, TEXT_INVERTED_BGCOLOR);
  }
}

int Slider::value(coord_t x) const
{
  return vmin + ((vmax - vmin) * x + (rect.w / 2)) / rect.w;
}

#if defined(HARDWARE_KEYS)
void Slider::onKeyEvent(event_t event)
{
  TRACE_WINDOWS("%s received event 0x%X", getWindowDebugString().c_str(), event);

  if (editMode) {
    if (event == EVT_ROTARY_RIGHT) {
      setValue(getValue() + rotencSpeed);
      return;
    }
    else if (event == EVT_ROTARY_LEFT) {
      setValue(getValue() - rotencSpeed);
      return;
    }
  }

  FormField::onKeyEvent(event);
}
#endif

#if defined(HARDWARE_TOUCH)
bool Slider::onTouchStart(coord_t x, coord_t y)
{
  if (!hasFocus()) {
    setFocus();
  }
  sliding = (value(x) == getValue());
  return true;
}

bool Slider::onTouchEnd(coord_t x, coord_t y)
{
  setValue(value(x));
  invalidate();
  return true;
}

bool Slider::onTouchSlide(coord_t x, coord_t y, coord_t startX, coord_t startY, coord_t slideX, coord_t slideY)
{
  if (hasFocus() && sliding) {
    int newValue = value(x);
    if (getValue() != newValue) {
      setValue(newValue);
      invalidate();
    }
  }
  return true;
}
#endif
