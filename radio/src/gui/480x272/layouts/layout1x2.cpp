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

#include "opentx.h"

const uint8_t LBM_LAYOUT_1x2[] = {
#include "mask_layout1x2.lbm"
};

const ZoneOption OPTIONS_LAYOUT_1x2[] = {
  { STR_TOP_BAR, ZoneOption::Bool },
  { NULL, ZoneOption::Bool }
};

class Layout1x2: public Layout
{
  public:
    Layout1x2(const LayoutFactory * factory, Layout::PersistentData * persistentData):
      Layout(factory, persistentData)
    {
    }

    virtual void create()
    {
      Layout::create();
      persistentData->options[0].boolValue = true;
    }

    virtual unsigned int getZonesCount() const
    {
      return 2;
    }

    virtual Zone getZone(unsigned int index) const
    {
      Zone zone;
      zone.w = (LCD_W-2*10);
      
      zone.x = 10;
       
      if (persistentData->options[0].boolValue) {
        zone.h = (LCD_H-MENU_HEADER_HEIGHT-3*10) / 2;
        zone.y = MENU_HEADER_HEIGHT + 10;
      }
      else {
        zone.h = (LCD_H-3*10) /2;
        zone.y = 10;
      }
      if (index >= 1) {
        zone.y += 10 + zone.h;
      }      
      return zone;
    }

    virtual void refresh();
};

void Layout1x2::refresh()
{
  theme->drawBackground();

  if (persistentData->options[0].boolValue) {
    drawTopBar();
  }

  Layout::refresh();
}

BaseLayoutFactory<Layout1x2> Layout1x2("Layout1x2", LBM_LAYOUT_1x2, OPTIONS_LAYOUT_1x2);
