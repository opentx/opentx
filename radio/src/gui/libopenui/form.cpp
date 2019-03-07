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

#include "form.h"
#include "opentx.h"
#include "draw_functions.h"

FormField * FormField::current = nullptr;

#if defined(HARDWARE_KEYS)
void FormWindow::onKeyEvent(event_t event)
{
  TRACE_WINDOWS("%s received event 0x%X", getWindowDebugString().c_str(), event);

  if (event == EVT_KEY_BREAK(KEY_EXIT) && first && getFocus() != first) {
    first->setFocus();
  }
  else {
    Window::onKeyEvent(event);
  }
}

void FormField::onKeyEvent(event_t event)
{
  TRACE_WINDOWS("%s received event 0x%X", FormField::getWindowDebugString().c_str(), event);

  if (event == EVT_ROTARY_RIGHT/*EVT_KEY_BREAK(KEY_DOWN)*/) {
    if (next) {
      next->setFocus();
    }
  }
  else if (event == EVT_ROTARY_LEFT/*EVT_KEY_BREAK(KEY_UP)*/) {
    if (previous) {
      previous->setFocus();
    }
  }
  else if (event == EVT_KEY_BREAK(KEY_ENTER)) {
    editMode = !editMode;
    invalidate();
  }
  else if (event == EVT_KEY_BREAK(KEY_EXIT) && editMode) {
    editMode = false;
    invalidate();
  }
  else {
    Window::onKeyEvent(event);
  }
}

void FormGroup::onKeyEvent(event_t event)
{
  TRACE_WINDOWS("%s received event 0x%X", getWindowDebugString().c_str(), event);

  if (event == EVT_KEY_BREAK(KEY_ENTER)) {
    editMode = true;
    first->setFocus();
  }
  else if (event == EVT_KEY_BREAK(KEY_EXIT) && editMode) {
    editMode = false;
    setFocus();
  }
  else {
    FormField::onKeyEvent(event);
  }
}
#endif

void FormField::paint(BitmapBuffer * dc)
{
  if (editMode) {
    dc->drawSolidFilledRect(0, 0, rect.w, rect.h, TEXT_INVERTED_BGCOLOR);
  }
  else if (hasFocus()) {
    drawSolidRect(dc, 0, 0, rect.w, rect.h, 2, TEXT_INVERTED_BGCOLOR);
  }
  else if (!(windowFlags & BORDER_FOCUS_ONLY)) {
    drawSolidRect(dc, 0, 0, rect.w, rect.h, 1, CURVE_AXIS_COLOR);
  }
}

void FormGroup::paint(BitmapBuffer * dc)
{
  if (!editMode && hasFocus()) {
    drawSolidRect(dc, 0, 0, rect.w, rect.h, 2, TEXT_INVERTED_BGCOLOR);
  }
  else if (!(windowFlags & BORDER_FOCUS_ONLY)) {
    drawSolidRect(dc, 0, 0, rect.w, rect.h, 1, CURVE_AXIS_COLOR);
  }
}
