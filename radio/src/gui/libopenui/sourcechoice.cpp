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

#include "sourcechoice.h"
#include "menutoolbar.h"
#include "menu.h"
#include "dataconstants.h"
#include "lcd.h"
#include "strhelpers.h"
#include "draw_functions.h"

class SourceChoiceMenuToolbar : public MenuToolbar<SourceChoice> {
  public:
    SourceChoiceMenuToolbar(SourceChoice * choice, Menu * menu):
      MenuToolbar<SourceChoice>(choice, menu)
    {
      addButton(char('\314'), MIXSRC_FIRST_INPUT, MIXSRC_LAST_INPUT);
#if defined(LUA_MODEL_SCRIPTS)
      addButton(char('\322'), MIXSRC_LAST_LUA, MIXSRC_FIRST_LUA);
#endif
      addButton(char('\307'), MIXSRC_FIRST_STICK, MIXSRC_LAST_STICK);
      addButton(char('\310'), MIXSRC_LAST_POT, MIXSRC_FIRST_POT);
      addButton(char('\315'), MIXSRC_MAX, MIXSRC_MAX);
#if defined(HELI)
      addButton(char('\316'), MIXSRC_LAST_HELI, MIXSRC_FIRST_HELI);
#endif
      addButton(char('\313'), MIXSRC_LAST_TRIM, MIXSRC_FIRST_TRIM);
      addButton(char('\312'), MIXSRC_LAST_SWITCH, MIXSRC_FIRST_SWITCH);
      addButton(char('\317'), MIXSRC_LAST_TRAINER, MIXSRC_FIRST_TRAINER);
      addButton(char('\320'), MIXSRC_LAST_CH, MIXSRC_FIRST_CH);
#if defined(GVARS)
      addButton(char('\311'), MIXSRC_LAST_GVAR, MIXSRC_FIRST_GVAR);
#endif
      addButton(char('\321'), MIXSRC_LAST_TELEM, MIXSRC_FIRST_TELEM);
    }
};

void SourceChoice::paint(BitmapBuffer * dc)
{
  bool hasFocus = this->hasFocus();
  unsigned value = getValue();
  LcdFlags textColor = (value == 0 ? CURVE_AXIS_COLOR : 0);
  LcdFlags lineColor = CURVE_AXIS_COLOR;
  if (hasFocus) {
    textColor = TEXT_INVERTED_BGCOLOR;
    lineColor = TEXT_INVERTED_BGCOLOR;
  }
  drawSource(dc, 3, 2, value, textColor);
  drawSolidRect(dc, 0, 0, rect.w, rect.h, 1, lineColor);
}

void SourceChoice::fillMenu(Menu * menu, std::function<bool(int16_t)> filter)
{
  auto value = getValue();
  int count = 0;
  int current = -1;

  menu->removeLines();

  for (int i = vmin; i <= vmax; ++i) {
    if (filter && !filter(i))
      continue;
    if (isValueAvailable && !isValueAvailable(i))
      continue;
    menu->addLine(getSourceString(i), [=]() {
      setValue(i);
    });
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
bool SourceChoice::onTouchEnd(coord_t, coord_t)
{
  auto menu = new Menu();
  fillMenu(menu);

  menu->setToolbar(new SourceChoiceMenuToolbar(this, menu));

  setFocus();
  return true;
}
#endif
