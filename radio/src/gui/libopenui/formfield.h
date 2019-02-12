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

#ifndef _FORMFIELD_H_
#define _FORMFIELD_H_

#include "window.h"

class FormField: public Window {
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

  protected:
    static FormField * current;
    FormField * next = nullptr;
    FormField * previous = nullptr;

    void onKeyEvent(event_t event) override;
};

#endif
