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

#include "coloredit.h"
#include "numberedit.h"
#include "bitfield.h"
#include "lcd.h"

constexpr uint8_t PART_BITS[3][2] = { {0, 5}, {5, 6}, {11, 5} };

class ColorBox: public Window {
  public:
    ColorBox(Window * parent, const rect_t & rect, LcdFlags color):
      Window(parent, rect),
      color(color)
    {
    }

#if defined(TRACE_WINDOWS_ENABLED)
    std::string getName() override
    {
      return "ColorBox";
    }
#endif

    void paint(BitmapBuffer * dc) override
    {
      lcdSetColor(color);
      dc->drawSolidFilledRect(0, 0, width(), height(), TEXT_COLOR);
      dc->drawSolidFilledRect(1, 1, width() - 2, height() - 2, CUSTOM_COLOR);
    }

    void setColor(LcdFlags value)
    {
      color = value;
      invalidate();
    }

  protected:
    LcdFlags color;
};

ColorEdit::ColorEdit(Window * parent, const rect_t & rect, std::function<int32_t()> getValue, std::function<void(int32_t)> setValue):
  Window(parent, rect)
{
  auto width = rect.w / 4 - 5;

  // The color box
  auto box = new ColorBox(this, {0, 0, rect.w - 3 * width, rect.h}, getValue());

  // The 3 parts of RGB
  for (uint8_t part = 0; part < 3; part++) {
    new NumberEdit(this, {rect.w - (3 - part) * width + 5, 0, width - 5, rect.h}, 0, (1 << PART_BITS[part][1]) - 1,
                   [=]() {
                     return BF_GET(getValue(), PART_BITS[part][0], PART_BITS[part][1]);
                   },
                   [=](uint8_t newValue) {
                     uint16_t value = getValue();
                     BF_SET(value, newValue, PART_BITS[part][0], PART_BITS[part][1]);
                     setValue(value);
                     box->setColor(value);
                   });
  }

}

ColorEdit::~ColorEdit()
{
  deleteChildren();
}
