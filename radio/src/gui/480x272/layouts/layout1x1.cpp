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

const uint8_t LBM_LAYOUT_1x1[] = {
#include "mask_layout1x1.lbm"
};

const ZoneOption OPTIONS_LAYOUT_1x1[] = {
  { "Top bar", ZoneOption::Bool },
  { "Sliders+Trims", ZoneOption::Bool },
  { NULL, ZoneOption::Bool }
};

class Layout1x1: public Layout
{
  public:
    Layout1x1(const LayoutFactory * factory, Layout::PersistentData * persistentData):
      Layout(factory, persistentData)
    {
    }

    virtual void create()
    {
      Layout::create();
      persistentData->options[0].boolValue = true;
      persistentData->options[1].boolValue = true;
    }

    virtual unsigned int getZonesCount() const
    {
      return 1;
    }

    virtual Zone getZone(unsigned int index) const
    {
      Zone zone = { 10, 10, LCD_W - 2*10, LCD_H - 2*10 };
      if (persistentData->options[0].boolValue) {
        zone.y += MENU_HEADER_HEIGHT;
        zone.h -= MENU_HEADER_HEIGHT;
      }
      if (persistentData->options[1].boolValue) {
        zone.x += 35;
        zone.w -= 2*35;
        zone.h -= 35;
      }
      return zone;
    }

    virtual void refresh();
};

void Layout1x1::refresh()
{
  theme->drawBackground();

  if (persistentData->options[0].boolValue) {
    drawTopBar();
  }

  if (persistentData->options[1].boolValue) {
    // Sliders + Trims + Flight mode
    lcdDrawSizedText(LCD_W / 2 - getTextWidth(g_model.flightModeData[mixerCurrentFlightMode].name,  sizeof(g_model.flightModeData[mixerCurrentFlightMode].name), ZCHAR | SMLSIZE) / 2,
                     232,
                     g_model.flightModeData[mixerCurrentFlightMode].name,
                     sizeof(g_model.flightModeData[mixerCurrentFlightMode].name), ZCHAR | SMLSIZE);
    drawMainPots();
    drawTrims(mixerCurrentFlightMode);
  }

  Layout::refresh();
}

BaseLayoutFactory<Layout1x1> layout1x1("Layout1x1", LBM_LAYOUT_1x1, OPTIONS_LAYOUT_1x1);
