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

#include "numberedit.h"
#include "draw_functions.h"
#include "keyboard_number.h"

NumberEdit::NumberEdit(Window * parent, const rect_t & rect, int32_t vmin, int32_t vmax, std::function<int32_t()> getValue, std::function<void(int32_t)> setValue, LcdFlags flags):
  BaseNumberEdit(parent, rect, vmin, vmax, std::move(getValue), std::move(setValue), flags)
{
}

void NumberEdit::paint(BitmapBuffer * dc)
{
  bool hasFocus = this->hasFocus();
  LcdFlags textColor = CURVE_AXIS_COLOR;
  LcdFlags lineColor = CURVE_AXIS_COLOR;
  if (hasFocus) {
    textColor = TEXT_INVERTED_BGCOLOR;
    lineColor = TEXT_INVERTED_BGCOLOR;
  }
  else if (enabled) {
    textColor = TEXT_COLOR;
  }
  int32_t value = _getValue();
  if (displayFunction) {
    displayFunction(dc, textColor, value);
  }
  else if (value == 0 && !zeroText.empty()) {
    dc->drawText(3, 2, zeroText.c_str(), textColor | flags);
  }
  else {
    drawNumber(dc, 3, 2, value, textColor | flags, 0, prefix.c_str(), suffix.c_str());
  }
  drawSolidRect(dc, 0, 0, rect.w, rect.h, 1, lineColor);
}

bool NumberEdit::onTouchEnd(coord_t, coord_t)
{
  if (!enabled) {
    return true;
  }

  if (!hasFocus()) {
    setFocus();
  }

  NumberKeyboard * keyboard = NumberKeyboard::instance();
  if (keyboard->getField() != this) {
    keyboard->setField(this);
  }

  return true;
}

void NumberEdit::onFocusLost()
{
  NumberKeyboard::instance()->disable(true);
}
