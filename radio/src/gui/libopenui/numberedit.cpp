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
#include <opentx.h>
#include "numberedit.h"
#include "draw_functions.h"

#if defined(HARDWARE_TOUCH)
#include "keyboard_number.h"
#endif

NumberEdit::NumberEdit(Window * parent, const rect_t & rect, int32_t vmin, int32_t vmax, std::function<int32_t()> getValue, std::function<void(int32_t)> setValue, LcdFlags flags):
  BaseNumberEdit(parent, rect, vmin, vmax, std::move(getValue), std::move(setValue), flags)
{
}

void NumberEdit::paint(BitmapBuffer * dc)
{
  FormField::paint(dc);

  LcdFlags textColor;
  if (editMode)
    textColor = TEXT_INVERTED_COLOR;
  else if (hasFocus())
    textColor = TEXT_INVERTED_BGCOLOR;
  else if (enabled)
    textColor = TEXT_COLOR;
  else
    textColor = CURVE_AXIS_COLOR;

  int32_t value = getValue();
  if (displayFunction)
    displayFunction(dc, textColor, value);
  else if (value == 0 && !zeroText.empty())
    dc->drawText(3, 0, zeroText.c_str(), textColor | flags);
  else
    drawNumber(dc, 3, 0, value, textColor | flags, 0, prefix.c_str(), suffix.c_str());
}

#if defined(HARDWARE_KEYS)
void NumberEdit::onKeyEvent(event_t event)
{
  TRACE_WINDOWS("%s received event 0x%X", getWindowDebugString().c_str(), event);

  if (editMode) {
    if (event == EVT_ROTARY_RIGHT) {
      setValue(getValue() + (rotencSpeed * step));
      return;
    }
    else if (event == EVT_ROTARY_LEFT) {
      setValue(getValue() - (rotencSpeed * step));
      return;
    }
  }

  FormField::onKeyEvent(event);
}
#endif

#if defined(HARDWARE_TOUCH)
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
#endif

void NumberEdit::onFocusLost()
{
#if defined(HARDWARE_TOUCH)
  NumberKeyboard::instance()->disable(true);
#endif

  FormField::onFocusLost();
}
