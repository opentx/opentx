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

#ifndef _FORM_H_
#define _FORM_H_

#include "window.h"

class FormField: public Window {
  friend class FormWindow;

  public:
    FormField(Window * parent, const rect_t & rect) :
      Window(parent, rect)
    {
      if (current) {
        setPreviousField(current);
        current->setNextField(this);
      }

      current = this;
    }

    inline void setNextField(FormField *field)
    {
      next = field;
    }

    inline void setPreviousField(FormField * field)
    {
      previous = field;
    }

    inline static void clearCurrentField()
    {
      current = nullptr;
    };

    inline static FormField * getCurrentField()
    {
      return current;
    };

    inline FormField * getPreviousField()
    {
      return previous;
    };

    inline FormField * getNextField()
    {
      return next;
    };

    inline static void link(FormField * previous, FormField * next)
    {
      previous->setNextField(next);
      next->setPreviousField(previous);
    }

    void paint(BitmapBuffer * dc) override;

    void onKeyEvent(event_t event) override;

  protected:
    static FormField * current;
    FormField * next = nullptr;
    FormField * previous = nullptr;
    bool editMode = false;
};

class FormWindow: public Window {
  public:
    FormWindow(Window * parent, const rect_t & rect) :
            Window(parent, rect)
    {
    }

    void clear()
    {
      Window::clear();
      FormField::current = nullptr;
    }

    inline void setFirst(FormField * field)
    {
      first = field;
    }

  protected:
    FormField * first = nullptr;
    void onKeyEvent(event_t event) override;
};

#endif
