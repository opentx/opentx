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

const uint8_t LBM_LAYOUT_4P2[] = {
#include "mask_layout4+2.lbm"
};

const ZoneOption OPTIONS_LAYOUT_4P2[] = {
  { STR_TOP_BAR, ZoneOption::Bool },
  { STR_FLIGHT_MODE, ZoneOption::Bool },
  { STR_SLIDERS, ZoneOption::Bool },
  { STR_TRIMS, ZoneOption::Bool },
  { NULL, ZoneOption::Bool }
};

const Zone ZONES_LAYOUT_4P2[6] = {
  { 240, 60, 192, 76 },
  { 240, 142, 192, 76 },
  { 48, 60, 180, 35 },
  { 48, 102, 180, 35 },
  { 48, 144, 180, 35 },
  { 48, 186, 180, 35 },
};

class Layout4P2: public Layout
{
  public:
    Layout4P2(const LayoutFactory * factory, Layout::PersistentData * persistentData):
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
      persistentData->options[4].boolValue = true;
      persistentData->options[5].boolValue = true;
    }

    virtual unsigned int getZonesCount() const
    {
      return 6;
    }

    virtual Zone getZone(unsigned int index) const
    {
      constexpr coord_t xoffset = 3; // lines are drawn left of pos
      constexpr coord_t vseparator = 8;
      constexpr coord_t trims = 20;
      constexpr coord_t sliders = 20;
      constexpr coord_t left = 1;

      coord_t blockw =  239 - (persistentData->options[2].boolValue == true ? sliders : 0) - (persistentData->options[3].boolValue == true ? trims : 0) - vseparator - xoffset;

      Zone zone;
      zone.x = (index >= 4) ? 2 * xoffset + 240 : xoffset + left + (persistentData->options[2].boolValue == true ? sliders : 0) + (persistentData->options[3].boolValue == true ? trims : 0);
      zone.y = (index >= 4) ? 56 + (index % 2) * 84 : 56 + (index % 4) * 42 ;
      zone.w = blockw;
      zone.h = (index >= 4) ? 74 : 32;
      return zone;
    }

    virtual void refresh();
};

void Layout4P2::refresh()
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
                     232,
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

  Layout::refresh();
}

BaseLayoutFactory<Layout4P2> layout4P2("Layout4P2", LBM_LAYOUT_4P2, OPTIONS_LAYOUT_4P2);