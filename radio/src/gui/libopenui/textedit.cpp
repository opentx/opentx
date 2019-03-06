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

#include "textedit.h"
// #include "keyboard_text.h"
#include "draw_functions.h"
#include "opentx.h"

void TextEdit::paint(BitmapBuffer * dc)
{
  LcdFlags flags = ZCHAR;

  FormField::paint(dc);

  if (editMode) {
    dc->drawSizedText(3, 0, value, length, TEXT_INVERTED_COLOR | flags);
    coord_t left = (cursorPos == 0 ? 0 : getTextWidth(value, cursorPos, flags));
    char s[] = { (flags & ZCHAR) ? zchar2char(value[cursorPos]) : value[cursorPos], '\0' };
    dc->drawSolidFilledRect(3+left-1, 1, getTextWidth(s, 1) + 1, height() - 2, TEXT_INVERTED_COLOR);
    dc->drawText(3 + left, 0, s, TEXT_COLOR);
  }
  else {
    const char * displayedValue = value;
    LcdFlags textColor;
    if (zlen(value, length) == 0) {
      displayedValue = "---";
      textColor = hasFocus() ? TEXT_INVERTED_BGCOLOR : CURVE_AXIS_COLOR;
    }
    else if (hasFocus()) {
      textColor = TEXT_INVERTED_BGCOLOR | ZCHAR;
    }
    else {
      textColor = TEXT_COLOR | ZCHAR;
    }
    dc->drawSizedText(3, 0, displayedValue, length, textColor);
  }

#if defined(HARDWARE_TOUCH)
  auto keyboard = TextKeyboard::instance();
  if (hasFocus() && keyboard->getField() == this) {
    coord_t cursorPos = keyboard->getCursorPos();
    dc->drawSolidFilledRect(cursorPos + 2, 2, 2, 21, 0); // TEXT_INVERTED_BGCOLOR);
  }
#endif
}

#if defined(HARDWARE_KEYS)
void TextEdit::onKeyEvent(event_t event)
{
  TRACE_WINDOWS("%s received event 0x%X", getWindowDebugString().c_str(), event);

  LcdFlags flags = ZCHAR;

  if (editMode) {
    int8_t c = value[cursorPos];
    if (!(flags & ZCHAR)) {
      c = char2zchar(c);
    }
    int8_t v = c;

    switch (event) {
      case EVT_ROTARY_RIGHT:
      case EVT_ROTARY_LEFT:
        v = limit(0, abs(v) + (event == EVT_ROTARY_RIGHT ? +rotencSpeed : -rotencSpeed), ZCHAR_MAX);
        if (c <= 0) v = -v;
        break;

      case EVT_KEY_BREAK(KEY_LEFT):
        if (cursorPos > 0) {
          cursorPos--;
          invalidate();
        }
        break;

      case EVT_KEY_BREAK(KEY_RIGHT):
        if (cursorPos < length - 1) {
          cursorPos++;
          invalidate();
        }
        break;

      case EVT_KEY_BREAK(KEY_ENTER):
        if (cursorPos < length - 1) {
          cursorPos++;
          invalidate();
        }
        else {
          FormField::onKeyEvent(event);
        }
        break;

      case EVT_KEY_BREAK(KEY_EXIT):
        cursorPos = 0;
        FormField::onKeyEvent(event);
        break;

      case EVT_KEY_LONG(KEY_ENTER):
        if (v == 0) {
          killEvents(event);
          FormField::onKeyEvent(EVT_KEY_BREAK(KEY_ENTER));
          break;
        }
        // no break

      case EVT_KEY_LONG(KEY_LEFT):
      case EVT_KEY_LONG(KEY_RIGHT):
        if (v >= -26 && v <= 26) {
          v = -v; // toggle case
          if (event == EVT_KEY_LONG(KEY_LEFT)) {
            killEvents(KEY_LEFT);
          }
        }
        break;
    }

    if (c != v) {
      if (!(flags & ZCHAR)) {
        if (v != '\0' || value[cursorPos+1] != '\0')
          v = zchar2char(v);
      }
      value[cursorPos] = v;
      invalidate();
      // TODO storageDirty(...);
    }
  }
  else {
    FormField::onKeyEvent(event);
  }
}
#endif

#if defined(HARDWARE_TOUCH)
bool TextEdit::onTouchEnd(coord_t x, coord_t y)
{
  if (!hasFocus()) {
    setFocus();
  }

  auto keyboard = TextKeyboard::instance();
  if (keyboard->getField() != this) {
    keyboard->setField(this);
  }

  keyboard->setCursorPos(x);

  return true;
}

void TextEdit::onFocusLost()
{
  TextKeyboard::instance()->disable(true);
  // TODO storageDirty(...);
}
#endif
