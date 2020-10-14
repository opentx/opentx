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

const uint8_t LBM_LAYOUT_2x4[] = {
#include "mask_layout2x4.lbm"
};

const ZoneOption OPTIONS_LAYOUT_2x4[] = {
  { "Top bar", ZoneOption::Bool },
  { "Flight mode", ZoneOption::Bool },
  { "Sliders", ZoneOption::Bool },
  { "Trims", ZoneOption::Bool },
  { "Panel1 background", ZoneOption::Bool },
  { "  Color", ZoneOption::Color },
  { "Panel2 background", ZoneOption::Bool },
  { "  Color", ZoneOption::Color },
  { nullptr, ZoneOption::Bool }
};

class Layout2x4: public Layout
{
  public:
    Layout2x4(const LayoutFactory * factory, Layout::PersistentData * persistentData):
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
      persistentData->options[4] = ZoneOptionValueTyped { ZOV_Bool, OPTION_VALUE_BOOL(true) };
      persistentData->options[5] = ZoneOptionValueTyped { ZOV_Unsigned, OPTION_VALUE_UNSIGNED( RGB(77,112,203)) };
      persistentData->options[6] = ZoneOptionValueTyped { ZOV_Bool, OPTION_VALUE_BOOL(true) };
      persistentData->options[7] = ZoneOptionValueTyped { ZOV_Unsigned, OPTION_VALUE_UNSIGNED( RGB(77,112,203)) };
    }

    unsigned int getZonesCount() const override
    {
      return 8;
    }

    rect_t getZone(unsigned int index) const override
    {
      rect_t zone;
      zone.x = (index >= 4) ? 260 : 60;
      zone.y = 56 + (index % 4) * 42;
      zone.w = 160;
      zone.h = 32;
      return zone;
    }

//    virtual void refresh();
};

//void Layout2x4::refresh()
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
//  if (persistentData->options[4].value.boolValue) {
//    lcdSetColor(persistentData->options[5].value.unsignedValue);
//    lcdDrawSolidFilledRect(50, 50, 180, 170, CUSTOM_COLOR);
//  }
//
//  if (persistentData->options[6].value.boolValue) {
//    lcdSetColor(persistentData->options[7].value.unsignedValue);
//    lcdDrawSolidFilledRect(250, 50, 180, 170, CUSTOM_COLOR);
//  }
//
//  Layout::refresh();
//}

BaseLayoutFactory<Layout2x4> layout2x4("Layout2x4", "2 x 4", LBM_LAYOUT_2x4, OPTIONS_LAYOUT_2x4);
