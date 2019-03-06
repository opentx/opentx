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

#include <keys.h>
#include "checkbox.h"
#include "draw_functions.h"

void CheckBox::paint(BitmapBuffer * dc)
{
  bool hasFocus = this->hasFocus();
  uint8_t value = getValue();
  if (hasFocus) {
    dc->drawSolidFilledRect(0, 2, 16, 16, TEXT_INVERTED_BGCOLOR);
    if (value) {
      dc->drawSolidFilledRect(2, 4, 12, 12, TEXT_BGCOLOR);
      dc->drawSolidFilledRect(3, 5, 10, 10, SCROLLBOX_COLOR);
    }
    else {
      dc->drawSolidFilledRect(2, 4, 12, 12, TEXT_BGCOLOR);
    }
  }
  else {
    if (value) {
      dc->drawSolidFilledRect(2, 4, 12, 12, SCROLLBOX_COLOR);
      drawSolidRect(dc, 0, 2, 16, 16, 1, CURVE_AXIS_COLOR);
    }
    else {
      drawSolidRect(dc, 0, 2, 16, 16, 1, CURVE_AXIS_COLOR);
    }
  }
}

void CheckBox::onKeyEvent(event_t event)
{
  TRACE_WINDOWS("%s received event 0x%X", getWindowDebugString().c_str(), event);

  if (event == EVT_KEY_BREAK(KEY_ENTER)) {
    setValue(!getValue());
    invalidate({0, 0, 20, 20});
  }
  else {
    FormField::onKeyEvent(event);
  }
}

#if defined(HARDWARE_TOUCH)
bool CheckBox::onTouchEnd(coord_t x, coord_t y)
{
  setValue(!getValue());
  setFocus();
  invalidate({0, 0, 20, 20});
  return true;
}
#endif
