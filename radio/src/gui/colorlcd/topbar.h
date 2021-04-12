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

#ifndef _TOPBAR_H_
#define _TOPBAR_H_

#include "widgets_container.h"

#define MAX_TOPBAR_ZONES               4
#define MAX_TOPBAR_OPTIONS             1 // just because of VC++ which doesn't like 0-size arrays :(
#define TOPBAR_ZONE_WIDTH              70
constexpr coord_t TOPBAR_ZONE_MARGIN = 3;
constexpr coord_t TOPBAR_HEIGHT = MENU_HEADER_HEIGHT - 2 * TOPBAR_ZONE_MARGIN;

class TopBar: public WidgetsContainer<MAX_TOPBAR_ZONES, MAX_TOPBAR_OPTIONS>
{
  public:
    explicit TopBar(Window * parent);

#if defined(DEBUG_WINDOWS)
    std::string getName() const override
    {
      return "TopBar";
    }
#endif

    unsigned int getZonesCount() const override;

    rect_t getZone(unsigned int index) const override;

    void setVisible(bool visible);
  
    void paint(BitmapBuffer * dc) override;

    void checkEvents() override
    {
      invalidate();
    }
};

#endif // _TOPBAR_H_
