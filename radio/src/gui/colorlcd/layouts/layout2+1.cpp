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

const uint8_t LBM_LAYOUT_2P1[] = {
#include "mask_layout2+1.lbm"
};

const ZoneOption OPTIONS_LAYOUT_2P1[] = {
  { "Top bar", ZoneOption::Bool },
  { "Flight mode", ZoneOption::Bool },
  { "Sliders", ZoneOption::Bool },
  { "Trims", ZoneOption::Bool },
  { nullptr, ZoneOption::Bool }
};

const rect_t ZONES_LAYOUT_2P1[3] = {
  { 240, 60, 192, 152 },
  { 48, 60, 180, 70 },
  { 48, 142, 180, 70 }
};

class Layout2P1: public Layout
{
  public:
    Layout2P1(const LayoutFactory * factory, Layout::PersistentData * persistentData):
      Layout(factory, persistentData)
    {
    }

    void create() override
    {
      Layout::create();
      persistentData->options[0] = ZoneOptionValueTyped { ZOV_Bool, OPTION_VALUE_BOOL(true) };
      persistentData->options[1] = ZoneOptionValueTyped { ZOV_Bool, OPTION_VALUE_BOOL(true) };
      persistentData->options[2] = ZoneOptionValueTyped { ZOV_Bool, OPTION_VALUE_BOOL(true) };
      persistentData->options[3] = ZoneOptionValueTyped { ZOV_Bool, OPTION_VALUE_BOOL(true) };
    }

    unsigned int getZonesCount() const override
    {
      return DIM(ZONES_LAYOUT_2P1);
    }

    rect_t getZone(unsigned int index) const override
    {
      return ZONES_LAYOUT_2P1[index];
    }

//    virtual void refresh();
};

//void Layout2P1::refresh()
//{
//  theme->drawBackground();
//
//  if (persistentData->options[0].value.boolValue) {
//    drawTopBar();
//  }
//
//  if (persistentData->options[1].value.boolValue) {
//    // Flight mode
//    lcdDrawSizedText(LCD_W / 2 - getTextWidth(g_model.flightModeData[mixerCurrentFlightMode].name,
//                                              sizeof(g_model.flightModeData[mixerCurrentFlightMode].name),
//                                              ZCHAR | FONT(XS)) / 2,
//                     232,
//                     g_model.flightModeData[mixerCurrentFlightMode].name,
//                     sizeof(g_model.flightModeData[mixerCurrentFlightMode].name), ZCHAR | FONT(XS));
//  }
//
//  if (persistentData->options[2].value.boolValue) {
//    // Pots and rear sliders positions
//    drawMainPots();
//  }
//
//  if (persistentData->options[3].value.boolValue) {
//    // Trims
//    drawTrims(mixerCurrentFlightMode);
//  }
//
//  Layout::refresh();
//}

BaseLayoutFactory<Layout2P1> layout2P1("Layout2P1", "2 + 1", LBM_LAYOUT_2P1, OPTIONS_LAYOUT_2P1);
const LayoutFactory * defaultLayout = &layout2P1;
