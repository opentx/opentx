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

#include "choice.h"
#include "menu.h"
#include "draw_functions.h"
#include "strhelpers.h"
#include "opentx.h"

const uint8_t LBM_DROPDOWN[] = {
#include "mask_dropdown.lbm"
};

Choice::Choice(Window * parent, const rect_t &rect, const char * values, int16_t vmin, int16_t vmax,
               std::function<int16_t()> getValue, std::function<void(int16_t)> setValue, LcdFlags flags) :
  FormField(parent, rect),
  values(values),
  vmin(vmin),
  vmax(vmax),
  getValue(std::move(getValue)),
  setValue(std::move(setValue)),
  flags(flags)
{
}

void Choice::paint(BitmapBuffer * dc)
{
  bool hasFocus = this->hasFocus();
  LcdFlags textColor = 0;
  LcdFlags lineColor = CURVE_AXIS_COLOR;
  if (hasFocus) {
    textColor = TEXT_INVERTED_BGCOLOR;
    lineColor = TEXT_INVERTED_BGCOLOR;
  }
  if (textHandler)
    dc->drawText(3, 0, textHandler(getValue()).c_str());
  else
    drawTextAtIndex(dc, 3, 0, values, getValue() - vmin, flags | textColor);
  drawSolidRect(dc, 0, 0, rect.w, rect.h, 1, lineColor);
  dc->drawBitmapPattern(rect.w - 14, (rect.h - 5) / 2, LBM_DROPDOWN, lineColor);
}

void Choice::onKeyEvent(event_t event)
{
  TRACE_WINDOWS("%s received event 0x%X", getWindowDebugString().c_str(), event);

  if (event == EVT_KEY_BREAK(KEY_ENTER)) {
    openMenu();
  }
  else {
    FormField::onKeyEvent(event);
  }
}

void Choice::openMenu()
{
  auto menu = new Menu();
  auto value = getValue();
  int count = 0;
  int current = -1;

  for (int i = vmin; i <= vmax; ++i) {
    if (isValueAvailable && !isValueAvailable(i))
      continue;
    if (textHandler) {
      menu->addLine(textHandler(i), [=]() {
          setValue(i);
      });
    }
    else {
      menu->addLine(TEXT_AT_INDEX(values, i - vmin), [=]() {
          setValue(i);
          setFocus();
      });
    }
    if (value == i) {
      current = count;
    }
    ++count;
  }

  if (current >= 0) {
    menu->select(current);
  }
}

#if defined(TOUCH_INTERFACE)
bool Choice::onTouchEnd(coord_t, coord_t)
{
  openMenu();
  setFocus();
  return true;
}
#endif
