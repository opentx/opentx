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

#ifndef _CHECKBOX_H_
#define _CHECKBOX_H_

#include "window.h"

class CheckBox : public Window {
  public:
    CheckBox(Window * parent, const rect_t & rect, std::function<uint8_t()> getValue, std::function<void(uint8_t)> setValue,
             LcdFlags flags = 0) :
      Window(parent, rect),
      getValue(getValue),
      setValue(setValue)
    {
    }

#if defined(DEBUG_WINDOWS)
    std::string getName() override
    {
        return "CheckBox";
    }
#endif

    void paint(BitmapBuffer * dc) override;

    bool onTouchEnd(coord_t x, coord_t y) override;

  protected:
    const char * label;
    std::function<uint8_t()> getValue;
    std::function<void(uint8_t)> setValue;
};

#endif // _CHECKBOX_H_
