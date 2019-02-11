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

#ifndef _MENUTOOLBAR_H_
#define _MENUTOOLBAR_H_

#include "button.h"
#include "menu.h"

int getFirstAvailable(int min, int max, std::function<bool(int)>isValueAvailable);
uint8_t getFontHeight(LcdFlags flags);

class MenuToolbarButton: public Button {
  public:
    MenuToolbarButton(Window * window, const rect_t & rect, char picto):
      Button(window, rect, nullptr, BUTTON_CHECKED_ON_FOCUS),
      picto(picto)
    {
    }

    void paint(BitmapBuffer * dc) override
    {
      if (checked()) {
        dc->drawSolidFilledRect(11, 0, 28, 28, HEADER_BGCOLOR);
        dc->drawSizedText(rect.w / 2, (rect.h - getFontHeight(flags)) / 2, &picto, 1, CENTERED | MENU_TITLE_COLOR);
      }
      else {
        dc->drawSizedText(rect.w / 2, (rect.h - getFontHeight(flags)) / 2, &picto, 1, CENTERED | TEXT_COLOR);
      }
    }

#if defined(TOUCH_INTERFACE)
    bool onTouchEnd(coord_t x, coord_t y) override
    {
      if (hasFocus()) {
        check(false);
        clearFocus();
      }
      else {
        setFocus();
      }
      onPress();
      return true;
    }
#endif

  protected:
    char picto;
};

template <class T>
class MenuToolbar: public Window {
  friend T;
  public:
    MenuToolbar(T * choice, Menu * menu):
      Window(menu, { 35, 95, 50, 370 }),
      choice(choice),
      menu(menu)
    {
    }

    ~MenuToolbar() override
    {
      deleteChildren();
    }

    void paint(BitmapBuffer * dc) override
    {
      dc->clear(CURVE_AXIS_COLOR);
    }

#if defined(TOUCH_INTERFACE)
    bool onTouchEnd(coord_t x, coord_t y) override
    {
      Window::onTouchEnd(x, y);
      return true; // = don't close the menu (inverted so that click outside the menu closes it)
    }
#endif

  protected:
    T * choice;
    Menu * menu;
    coord_t y = 5;

    void addButton(char picto, int16_t filtermin, int16_t filtermax)
    {
      int vmin = choice->vmin;
      int vmax = choice->vmax;

      if (vmin > filtermin || vmax < filtermin)
        return;

      if (choice->isValueAvailable && getFirstAvailable(filtermin, filtermax, choice->isValueAvailable) == 0)
        return;

      auto button = new MenuToolbarButton(this, {0, y, 50, 30}, picto);
      button->setPressHandler([=]() {
        if (button->hasFocus()) {
          choice->fillMenu(menu, [=](int16_t index) {
            return index >= filtermin && index <= filtermax;
          });
        }
        else {
          choice->fillMenu(menu);
        }
        return 1;
      });

      y += 30;
    }
};

#endif // _MENUTOOLBAR_H_

