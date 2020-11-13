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

#pragma once

#include "libopenui.h"
#include "trims.h"

#if defined(PCBNV14)
constexpr uint8_t SLIDER_TICKS_COUNT = 30;
#else
constexpr uint8_t SLIDER_TICKS_COUNT = 40;
#endif
constexpr coord_t HMARGIN = 5;
constexpr coord_t HORIZONTAL_SLIDERS_WIDTH = SLIDER_TICKS_COUNT * 4 + TRIM_SQUARE_SIZE;
constexpr coord_t MULTIPOS_H = 20;
constexpr coord_t MULTIPOS_W = 50;
inline coord_t VERTICAL_SLIDERS_HEIGHT(bool topbar)
{
  return topbar ? SLIDER_TICKS_COUNT * 4 + TRIM_SQUARE_SIZE : SLIDER_TICKS_COUNT * 5 + TRIM_SQUARE_SIZE;
}

class MainViewSlider : public Window
{
  public:
    MainViewSlider(Window * parent, const rect_t & rect, std::function<int16_t()> getValue):
      Window(parent, rect),
      getValue(std::move(getValue))
    {
    }

    void checkEvents() override
    {
      Window::checkEvents();
      int8_t newValue = getValue();
      if (value != newValue) {
        value = newValue;
        invalidate();
      }
    }

  protected:
    std::function<int16_t()> getValue;
    int8_t value = 0;
};

class MainViewHorizontalSlider : public MainViewSlider
{
  public:
    using MainViewSlider::MainViewSlider;

    void paint(BitmapBuffer * dc) override
    {
      // The ticks
      int delta = (width() - TRIM_SQUARE_SIZE) / SLIDER_TICKS_COUNT;
      coord_t x = TRIM_SQUARE_SIZE / 2;
      for (uint8_t i = 0; i <= SLIDER_TICKS_COUNT; i++) {
        if (i == 0 || i == SLIDER_TICKS_COUNT / 2 || i == SLIDER_TICKS_COUNT)
          dc->drawSolidVerticalLine(x, 2, 13, DEFAULT_COLOR);
        else
          dc->drawSolidVerticalLine(x, 4, 9, DEFAULT_COLOR);
        x += delta;
      }

      // The square
      auto value = - getValue();
      x = width() - TRIM_SQUARE_SIZE - divRoundClosest((width() - TRIM_SQUARE_SIZE) * (value + RESX), 2 * RESX);
      drawTrimSquare(dc, x, 0, TRIM_BGCOLOR);
    }
};

class MainView6POS : public MainViewSlider
{
  public:
    using MainViewSlider::MainViewSlider;

    void paint(BitmapBuffer * dc) override
    {
      // The ticks
      int delta = (width() - TRIM_SQUARE_SIZE) / (XPOTS_MULTIPOS_COUNT - 1);
      coord_t x = TRIM_SQUARE_SIZE / 2;
      for (uint8_t i = 0; i <= XPOTS_MULTIPOS_COUNT; i++) {
        dc->drawSolidVerticalLine(x, 4, 9, DEFAULT_COLOR);
        x += delta;
      }

      // The square
      auto value = getValue();
      x = TRIM_SQUARE_SIZE / 2 + divRoundClosest((width() - TRIM_SQUARE_SIZE) * (value -1) , 6);
      drawTrimSquare(dc, x, 0, TRIM_BGCOLOR);
      dc->drawNumber(x + 1, 0, value, FOCUS_COLOR);
    }
};

class MainViewVerticalSlider : public MainViewSlider
{
  public:
    using MainViewSlider::MainViewSlider;

    void paint(BitmapBuffer * dc) override
    {
      uint8_t slidersTick = height() / 5;
      // The ticks
      int delta = (height() - TRIM_SQUARE_SIZE) / slidersTick;
      coord_t y = TRIM_SQUARE_SIZE / 2;
      for (uint8_t i = 0; i <= slidersTick; i++) {
        if (i == 0 || i == slidersTick / 2 || i == slidersTick)
          dc->drawSolidHorizontalLine(2, y, 13, DEFAULT_COLOR);
        else
          dc->drawSolidHorizontalLine(4, y, 9, DEFAULT_COLOR);
        y += delta;
      }

      // The square
      auto value = getValue();
      y = height() - TRIM_SQUARE_SIZE - divRoundClosest((height() - TRIM_SQUARE_SIZE) * (value + RESX), 2 * RESX);
      drawTrimSquare(dc, 0, y, TRIM_BGCOLOR);
    }
};
