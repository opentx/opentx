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

#ifndef _KEYBOARD_BASE_H_
#define _KEYBOARD_BASE_H_

#include "mainwindow.h"
#include "libopenui_types.h"

class Page;
class TabsGroup;

template <class T>
class Keyboard: public Window {
  public:
    Keyboard(coord_t height):
      Window(nullptr, {0, LCD_H - height, LCD_W, height}, OPAQUE)
    {
    }

    void setField(T * field)
    {
      this->field = field;
      attach(&mainWindow);
      Window * w = getPageBody();
      w->setHeight(LCD_H - height() - w->top());
      w->scrollTo(field);
      invalidate();
    }

    void disable(bool resizePage)
    {
      detach();
      if (field) {
        Window * w = getPageBody();
        w->setHeight(LCD_H - 0 - w->top());
        field = nullptr;
      }
    }

    T * getField() const
    {
      return field;
    }

  protected:
    T * field = nullptr;
    Window * getPageBody()
    {
      Window * parent = field;
      while (1) {
        Window * tmp = parent->getParent();
        if ((tmp->getWindowFlags() & OPAQUE) && tmp->width() == LCD_W && tmp->height() == LCD_H) {
          return parent;
        }
        parent = tmp;
      }
    }
};

#endif // _KEYBOARD_BASE_H_