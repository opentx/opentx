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

#ifndef _PAGE_H_
#define _PAGE_H_

#include "window.h"
#include "button.h"

class Page;

class PageHeader: public Window {
  public:
    PageHeader(Page * parent, const rect_t & rect);

    virtual void paint(BitmapBuffer * dc) override;

  protected:
    IconButton back;
};

class Page: public Window {
  public:
    Page();

    ~Page() override;

#if defined(DEBUG_WINDOWS)
    std::string getName() override
    {
      return "Page";
    }
#endif

    bool onTouchStart(coord_t x, coord_t y) override
    {
      Window::onTouchStart(x, y);
      return true;
    }

    bool onTouchEnd(coord_t x, coord_t y) override;

    bool onTouchSlide(coord_t x, coord_t y, coord_t startX, coord_t startY, coord_t slideX, coord_t slideY) override
    {
      Window::onTouchSlide(x, y, startX, startY, slideX, slideY);
      return true;
    }

    void paint(BitmapBuffer * dc) override;

  protected:
    PageHeader header;
    Window body;
    static constexpr coord_t headerHeight = 60;
};

#endif // _PAGE_H_
