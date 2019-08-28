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

#ifndef _STATIC_H_
#define _STATIC_H_

#include <string>
#include "window.h"
#include "button.h" // TODO just for BUTTON_BACKGROUND

class StaticText : public Window {
  public:
    StaticText(Window * parent, const rect_t & rect, std::string text = "", LcdFlags flags = 0) :
      Window(parent, rect),
      text(std::move(text)),
      flags(flags)
    {
    }

#if defined(DEBUG_WINDOWS)
    std::string getName() override
    {
      return "StaticText \"" + text + "\"";
    }
#endif

    void setFlags(LcdFlags flags)
    {
      this->flags = flags;
      invalidate();
    }

    void paint(BitmapBuffer * dc)
    {
      if (flags & BUTTON_BACKGROUND)
        dc->drawSolidFilledRect(0, 0, rect.w, rect.h, CURVE_AXIS_COLOR);
      if (flags & CENTERED)
        dc->drawText(rect.w / 2, (rect.h - getFontHeight(flags)) / 2, text.c_str(), CENTERED | flags);
      else
        dc->drawText(0, 0, text.c_str(), flags);
    }

    void setText(std::string value)
    {
      text = std::move(value);
      invalidate();
    }

  protected:
    std::string text;
    LcdFlags flags;
};

class Subtitle: public StaticText {
  public:
    Subtitle(Window * parent, const rect_t & rect, const char * text):
      StaticText(parent, rect, text, BOLD)
    {
    }

#if defined(DEBUG_WINDOWS)
    std::string getName() override
    {
      return "Subtitle \"" + text + "\"";
    }
#endif
};

class StaticBitmap: public Window {
  public:
    StaticBitmap(Window * parent, const rect_t & rect, const char * filename):
      Window(parent, rect),
      bitmap(BitmapBuffer::load(filename))
    {
    }

    StaticBitmap(Window * parent, const rect_t & rect, const BitmapBuffer * bitmap):
      Window(parent, rect),
      bitmap(bitmap)
    {
    }

#if defined(DEBUG_WINDOWS)
    std::string getName() override
    {
      return "StaticBitmap";
    }
#endif

    void paint(BitmapBuffer * dc) override
    {
      dc->drawBitmap(0, 0, bitmap);
    }

  protected:
    const BitmapBuffer * bitmap;
};

#endif
