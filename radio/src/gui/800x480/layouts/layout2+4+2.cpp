/*
 * Copyright (C) JumperTX
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

const uint8_t LBM_LAYOUT_2P4P2[] = {
#include "mask_layout2+4+2.lbm"
};

const ZoneOption OPTIONS_LAYOUT_2P4P2[] = {
  { "Top bar", ZoneOption::Bool },
  { "Flight mode", ZoneOption::Bool },
  { "Sliders", ZoneOption::Bool },
  { "Trims", ZoneOption::Bool },
  { "Panel1 background", ZoneOption::Bool },
  { "  Color", ZoneOption::Color },
  { "Panel2 background", ZoneOption::Bool },
  { "  Color", ZoneOption::Color },
  { NULL, ZoneOption::Bool }
};

class Layout2P4P2: public Layout
{
  public:
    Layout2P4P2(const LayoutFactory * factory, Layout::PersistentData * persistentData):
      Layout(factory, persistentData)
    {
    }

    virtual void create()
    {
      Layout::create();
      persistentData->options[0].boolValue = true;
      persistentData->options[1].boolValue = true;
      persistentData->options[2].boolValue = true;
      persistentData->options[3].boolValue = true;
      persistentData->options[4].boolValue = false;
      persistentData->options[5].unsignedValue = RGB(77, 112, 203);
      persistentData->options[6].boolValue = false;
      persistentData->options[7].unsignedValue = RGB(77, 112, 203);
    }

    virtual unsigned int getZonesCount() const
    {
      return 8;
    }

    virtual Zone getZone(unsigned int index) const
    {
      Zone zone;
      zone.x = (index >= 6) ? 410 : 60;

      if(index <= 1)
      zone.y = 60+(index % 6) * 93;
      else if((index >=2) & (index <= 5))
      zone.y = 248+((index - 2) % 6) * 46;
      else if(index == 6)
      zone.y = 60;
      else
      zone.y = 294;

      zone.w = 330;

      if(index <= 1)
      zone.h = 81;
      else if((index >=2) & (index <= 5))
      zone.h = 34;
      else if(index == 6)
      zone.h = 222;
      else
      zone.h = 126;
      return zone;
    }

    virtual void refresh();
};

void Layout2P4P2::refresh()
{
  theme->drawBackground();

  if (persistentData->options[0].boolValue) {
    drawTopBar();
  }

  if (persistentData->options[1].boolValue) {
    // Flight mode
    lcdDrawSizedText(LCD_W / 2 - getTextWidth(g_model.flightModeData[mixerCurrentFlightMode].name,
                                              sizeof(g_model.flightModeData[mixerCurrentFlightMode].name),
                                              ZCHAR | SMLSIZE) / 2,
                     237,
                     g_model.flightModeData[mixerCurrentFlightMode].name,
                     sizeof(g_model.flightModeData[mixerCurrentFlightMode].name), ZCHAR | SMLSIZE);
  }

  if (persistentData->options[2].boolValue) {
    // Pots and rear sliders positions
    drawMainPots();
  }

  if (persistentData->options[3].boolValue) {
    // Trims
    drawTrims(mixerCurrentFlightMode);
  }

  if (persistentData->options[4].boolValue) {
    lcdSetColor(persistentData->options[5].unsignedValue);
    lcdDrawSolidFilledRect(50, 50, 180, 170, CUSTOM_COLOR);
  }

  if (persistentData->options[6].boolValue) {
    lcdSetColor(persistentData->options[7].unsignedValue);
    lcdDrawSolidFilledRect(250, 50, 180, 170, CUSTOM_COLOR);
  }

  Layout::refresh();
}

BaseLayoutFactory<Layout2P4P2> layout2P4P2("Layout2P4P2", LBM_LAYOUT_2P4P2, OPTIONS_LAYOUT_2P4P2);
