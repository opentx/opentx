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
#include "numberedit.h"
#include "draw_functions.h"

#if defined(TOUCH_INTERFACE)
#include "keyboard_number.h"
#endif

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
    if (editMode) {
      dc->drawSolidFilledRect(0, 0, rect.w, rect.h, TEXT_INVERTED_BGCOLOR);
      textColor = TEXT_INVERTED_COLOR;
    }
    else {
      textColor = TEXT_INVERTED_BGCOLOR;
    }
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
    dc->drawText(3, 0, zeroText.c_str(), textColor | flags);
  }
  else {
    drawNumber(dc, 3, 0, value, textColor | flags, 0, prefix.c_str(), suffix.c_str());
  }
  if (!editMode) {
    drawSolidRect(dc, 0, 0, rect.w, rect.h, 1, lineColor);
  }
}

void NumberEdit::onKeyEvent(event_t event)
{
#if defined(DEBUG_WINDOWS)
  TRACE("%s received event 0x%X", getWindowDebugString().c_str(), event);
#endif

  if (event == EVT_KEY_BREAK(KEY_ENTER)) {
    editMode = !editMode;
    invalidate();
  }
  else if (editMode) {
    if (event == EVT_KEY_BREAK(KEY_EXIT)) {
      editMode = false;
    }
  }
  else {
    FormField::onKeyEvent(event);
  }
}

#if defined(TOUCH_INTERFACE)
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
#if defined(TOUCH_INTERFACE)
  NumberKeyboard::instance()->disable(true);
#endif

  FormField::onFocusLost();
}
