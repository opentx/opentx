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

#ifndef _CURVEEDIT_H_
#define _CURVEEDIT_H_

#include "curve.h"

class CurveEdit: public Curve {
  friend class CurveKeyboard;

  public:
    CurveEdit(Window * parent, const rect_t & rect, uint8_t index);

    void checkEvents() override
    {
      // no permanent refresh
    }

    void update();

    bool onTouchEnd(coord_t x, coord_t y) override;

    void onFocusLost() override;

  protected:
    uint8_t index;
    uint8_t current;
    void next();
    void previous();
    void up();
    void down();
    void right();
    void left();
    bool isCustomCurve();
};

#endif // _CURVEEDIT_H_
