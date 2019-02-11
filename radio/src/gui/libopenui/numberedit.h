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

#ifndef _NUMBEREDIT_H_
#define _NUMBEREDIT_H_

#include "basenumberedit.h"
#include <string>

class NumberEdit : public BaseNumberEdit {
  public:
    NumberEdit(Window * parent, const rect_t & rect, int32_t vmin, int32_t vmax,
               std::function<int32_t()> getValue, std::function<void(int32_t)> setValue = nullptr, LcdFlags flags = 0);

#if defined(DEBUG_WINDOWS)
    std::string getName() override
    {
      return "NumberEdit(" + std::to_string(getValue()) + ")";
    }
#endif

    void paint(BitmapBuffer * dc) override;

    void enable(bool enabled)
    {
      this->enabled = enabled;
    }

    void setPrefix(std::string value)
    {
      prefix = std::move(value);
    }

    void setSuffix(std::string value)
    {
      suffix = std::move(value);
    }

    void setZeroText(std::string value)
    {
      zeroText = std::move(value);
    }

    void setDisplayHandler(std::function<void(BitmapBuffer *, LcdFlags, int32_t)> function)
    {
      displayFunction = std::move(function);
    }

#if defined(TOUCH_HARDWARE)
    bool onTouchEnd(coord_t x, coord_t y) override;
#endif

    void onFocusLost() override;

  protected:
    std::function<void(BitmapBuffer *, LcdFlags, int32_t)> displayFunction;
    std::string prefix;
    std::string suffix;
    std::string zeroText;
    bool enabled = true;
};

#endif // _NUMBEREDIT_H_
