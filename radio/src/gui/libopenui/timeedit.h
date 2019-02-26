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

#ifndef _TIMEEDIT_H_
#define _TIMEEDIT_H_

#include "basenumberedit.h"

class TimeEdit : public BaseNumberEdit {
  public:
    TimeEdit(Window * parent, const rect_t & rect, int32_t vmin, int32_t vmax, std::function<int32_t()> getValue, std::function<void(int32_t)> setValue = nullptr, LcdFlags flags = 0);

#if defined(TRACE_WINDOWS_ENABLED)
    std::string getName() override
    {
      return "TimeEdit";
    }
#endif

    void paint(BitmapBuffer * dc) override;

#if defined(TOUCH_HARDWARE)
    bool onTouchEnd(coord_t x, coord_t y) override;
#endif

    void onFocusLost() override;
};

#endif // _TIMEEDIT_H_
