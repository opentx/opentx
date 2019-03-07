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

#ifndef _GRIDLAYOUT_H_
#define _GRIDLAYOUT_H_

#include "window.h"

static constexpr uint8_t lineSpacing = 2;
static constexpr uint8_t lineHeight = 20;
static constexpr uint8_t indentWidth = 10;

class GridLayout {
  public:
    GridLayout(coord_t width):
      width(width)
    {
    }

    GridLayout(Window * window):
      width(window->width())
    {
    }

    rect_t getSlot(uint8_t count = 1, uint8_t index = 0) const
    {
      coord_t width = (this->width - (count - 1) * lineSpacing) / count;
      coord_t left = (width + lineSpacing) * index;
      return {left, currentY, width, lineHeight};
    }

    void spacer(coord_t height=lineSpacing)
    {
      currentY += height;
    }

    void nextLine(coord_t height=lineHeight)
    {
      spacer(height + lineSpacing);
    }

    void addWindow(Window * window)
    {
      window->adjustHeight();
      currentY += window->rect.h + lineSpacing;
    }

    coord_t getWindowHeight() const
    {
      return currentY;
    }

  protected:
    coord_t width;
    coord_t currentY = 0;
};

class FormGridLayout: public GridLayout {
  public:
    FormGridLayout():
      GridLayout(LCD_W)
    {
    }

    void setLabelWidth(coord_t value)
    {
      labelWidth = value;
    }

    void setMarginLeft(coord_t value)
    {
      lineMarginLeft = value;
    }

    void setMarginRight(coord_t value)
    {
      lineMarginRight = value;
    }

    rect_t getLineSlot()
    {
      return { lineMarginLeft, currentY, LCD_W - lineMarginRight - lineMarginLeft, lineHeight };
    }

    rect_t getCenteredSlot(coord_t width = LCD_W ) const
    {
      return { lineMarginLeft + (LCD_W - width)/2, currentY, min(width, LCD_W - lineMarginRight - lineMarginLeft), lineHeight };
    }

    rect_t getLabelSlot(bool indent = false) const
    {
      coord_t left = indent ? lineMarginLeft + indentWidth : lineMarginLeft;
      return { left, currentY, labelWidth - left, lineHeight };
    }

    rect_t getFieldSlot(uint8_t count = 1, uint8_t index = 0) const
    {
      coord_t width = (LCD_W - labelWidth - lineMarginRight - (count - 1) * lineSpacing) / count;
      coord_t left = labelWidth + (width + lineSpacing) * index;
      return {left, currentY, width, lineHeight};
    }

  protected:
    coord_t labelWidth = 240;
    coord_t lineMarginLeft = 6;
    coord_t lineMarginRight = 10;
};

#endif
