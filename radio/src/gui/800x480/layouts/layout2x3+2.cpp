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

const uint8_t LBM_LAYOUT_2X3P2[] = {
#include "mask_layout2x3+2.lbm"
};

uint8_t Margin = 60;
uint8_t Space_X = 19;
uint8_t Space_Y = 12;
uint8_t Body_X;
uint8_t Body_Y;

const ZoneOption OPTIONS_LAYOUT_2X3P2[] = {
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

class Layout2X3P2: public Layout
{
  public:
    Layout2X3P2(const LayoutFactory * factory, Layout::PersistentData * persistentData):
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

      Body_X = (LCD_W - Margin * 2 - Space_X * 2) / 3;				//Body_X
			
      if (index <= 5)																					//Body_Y
      Body_Y = (LCD_H - Margin * 2 - Space_Y * 2) / 3;
      else Body_Y = (LCD_H - Margin * 2 - Space_Y) / 2;
			
      if(index <= 2)
      zone.x = Margin;
      else if((index >=3) & (index <=5))
      zone.x = Margin + Body_X + Space_X;
	    else
	    zone.x = Margin + Body_X * 2 + Space_X * 2;

      if(index <= 2)
      zone.y = Margin + (index % 3) * (Body_Y + Space_Y);
      else if((index >=3) & (index <=5))
      zone.y = Margin + ((index-3) % 3) * (Body_Y + Space_Y);
	    else
	    zone.y = Margin + ((index-6) % 3) * (Body_Y + Space_Y);
  
      zone.w = Body_X;

      zone.h = Body_Y;
			
      return zone;
    }

    virtual void refresh();
};

void Layout2X3P2::refresh()
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
    lcdDrawSolidFilledRect(55, 55, (LCD_W / 2) - Margin - 5, LCD_H - (Margin-5) * 2, CUSTOM_COLOR);
  }
		
  if (persistentData->options[6].boolValue) {
    lcdSetColor(persistentData->options[7].unsignedValue);
    lcdDrawSolidFilledRect((LCD_W / 2 + 5), (LCD_W / 2 + 5), (LCD_W / 2) - Margin - 10, LCD_H - (Margin-10) * 2, CUSTOM_COLOR);
  }

  Layout::refresh();
}

BaseLayoutFactory<Layout2X3P2> layout2X3P2("Layout2X3P2", LBM_LAYOUT_2X3P2, OPTIONS_LAYOUT_2X3P2);
