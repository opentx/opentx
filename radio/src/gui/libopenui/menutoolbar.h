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
#if defined(HARDWARE_TOUCH)
      Button(window, rect, nullptr, BUTTON_CHECKED_ON_FOCUS),
#else
      Button(window, rect, nullptr),
#endif
      picto(picto)
    {
    }

    void paint(BitmapBuffer * dc) override
    {
      if (checked()) {
        dc->drawSolidFilledRect(MENUS_TOOLBAR_BUTTON_PADDING, MENUS_TOOLBAR_BUTTON_PADDING, MENUS_TOOLBAR_BUTTON_WIDTH - 2 * MENUS_TOOLBAR_BUTTON_PADDING, MENUS_TOOLBAR_BUTTON_WIDTH - 2 * MENUS_TOOLBAR_BUTTON_PADDING, HEADER_BGCOLOR);
        dc->drawSizedText(rect.w / 2, (rect.h - getFontHeight(flags)) / 2 + 1, &picto, 1, CENTERED | MENU_TITLE_COLOR);
      }
      else {
        dc->drawSizedText(rect.w / 2, (rect.h - getFontHeight(flags)) / 2 + 1, &picto, 1, CENTERED | TEXT_COLOR);
      }
    }

#if defined(HARDWARE_TOUCH)
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

    void check(bool checked=true)
    {
      Button::check(checked);
      onPress();
      Button::check(checked);
    }

  protected:
    char picto;
};

template <class T>
class MenuToolbar: public Window {
  friend T;

  public:
    MenuToolbar(T * choice, Menu * menu):
      Window(menu, MENUS_TOOLBAR_RECT, NO_SCROLLBAR),
      choice(choice),
      menu(menu)
    {
      FormField::clearCurrentField();
    }

    ~MenuToolbar() override
    {
      deleteChildren();
    }

    void paint(BitmapBuffer * dc) override
    {
      dc->clear(CURVE_AXIS_COLOR);
    }

#if defined(HARDWARE_KEYS)
    void onKeyEvent(event_t event) override
    {
      if (event == EVT_KEY_BREAK(KEY_PGDN)) {
        if (current == children.end()) {
          current = children.begin();
        }
        else {
          static_cast<MenuToolbarButton *>(*current)->check(false);
          ++current;
        }
        if (current != children.end()) {
          auto button = static_cast<MenuToolbarButton *>(*current);
          button->check(true);
          scrollTo(button);
        }
        else {
          setScrollPositionY(0);
        }
      }
      else if (event == EVT_KEY_LONG(KEY_PGDN)) {
        killEvents(event);
        if (current == children.end()) {
          --current;
        }
        else {
          static_cast<MenuToolbarButton *>(*current)->check(false);
          if (current == children.begin())
            current = children.end();
          else
            --current;
        }
        if (current != children.end()) {
          auto button = static_cast<MenuToolbarButton *>(*current);
          button->check(true);
          scrollTo(button);
        }
        else {
          setScrollPositionY(0);
        }
      }
    }
#endif

#if defined(HARDWARE_TOUCH)
    bool onTouchEnd(coord_t x, coord_t y) override
    {
      Window::onTouchEnd(x, y);
      return true; // = don't close the menu (inverted so that click outside the menu closes it)
    }
#endif

  protected:
    std::list<Window *>::iterator current = children.end();
    T * choice;
    Menu * menu;
    coord_t y = 0; // TODO NV14 was 5;

    void addButton(char picto, int16_t filtermin, int16_t filtermax)
    {
      int vmin = choice->vmin;
      int vmax = choice->vmax;

      if (vmin > filtermin || vmax < filtermin)
        return;

      if (choice->isValueAvailable && getFirstAvailable(filtermin, filtermax, choice->isValueAvailable) == 0)
        return;

      auto button = new MenuToolbarButton(this, {0, y, MENUS_TOOLBAR_BUTTON_WIDTH, MENUS_TOOLBAR_BUTTON_WIDTH}, picto);
      button->setPressHandler([=]() {
        if (button->checked()) {
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

      setInnerHeight(y);
    }
};

#endif // _MENUTOOLBAR_H_

