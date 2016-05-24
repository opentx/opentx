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

class OutputsWidget: public Widget
{
  public:
    OutputsWidget(const WidgetFactory * factory, const Zone & zone, Widget::PersistentData * persistentData):
      Widget(factory, zone, persistentData)
    {
    }

    virtual void refresh();
    
    void drawChannels(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t firstChan, uint8_t lastChan)
    {
      char chanString[]="CH32";
      uint8_t offset =80;
      
      for (uint8_t curChan = firstChan; curChan < lastChan + 1; curChan++)
      {
        int16_t chanVal = calcRESXto1000(channelOutputs[curChan-1]) / 10;
        
        sprintf(chanString,"CH%02i", curChan);
        lcdDrawText(x, y + (curChan - firstChan) * 20, chanString,SMLSIZE + TEXT_COLOR + LEFT);
        sprintf(chanString,"%3i", chanVal);
        lcdDrawText(x + offset - 2, y + (curChan - firstChan) * 20, chanString,SMLSIZE + TEXT_COLOR + RIGHT);
        lcdDrawRect(x + offset, y + (curChan -firstChan) * 20, w - 90, 18);
        lcd->drawSolidVerticalLine(x + offset + floor((w - 90) / 2), y + (curChan - firstChan) * 20, 18, MAINVIEW_GRAPHICS_COLOR);
        if (chanVal > 0)
        {
          lcdDrawSolidFilledRect(x + offset + floor((w - 90) / 2),  y + (curChan -firstChan) * 20, (w - 90) * chanVal / 200 , 18, MAINVIEW_GRAPHICS_COLOR);
        }
        else if (chanVal < 0)
        {
          uint16_t endpoint = x + offset + (w - 90) / 2;
          uint16_t startpoint = x + offset;
          uint16_t size = floor(abs((endpoint - startpoint) * chanVal / 100));
          lcdDrawSolidFilledRect(endpoint - size,  y + (curChan - firstChan) * 20, size, 18, MAINVIEW_GRAPHICS_COLOR);
        }
      }
    };

    void zoneXLarge()
    {
      lcd->drawSolidVerticalLine(zone.x + zone.w / 2 - 5, zone.y, zone.h, CUSTOM_COLOR);
      drawChannels(zone.x, zone.y, floor(zone.w / 2), zone.h, 1, 8);
      drawChannels(zone.x + floor(zone.w / 2), zone.y, floor(zone.w / 2), zone.h, 9, 16);
    };
    
    void zoneLarge()
    {
      drawChannels(zone.x, zone.y, zone.w, zone.h, 1, 8);
    };

    void zoneSmall()
    {
      drawChannels(zone.x, zone.y, zone.w, zone.h, 1, 4);
    };


    static const ZoneOption options[];
};

const ZoneOption OutputsWidget::options[] = {
  { NULL, ZoneOption::Bool }
};


void OutputsWidget::refresh()
{
  if (zone.w > 360 and zone.h > 165) OutputsWidget::zoneXLarge();
  else if (zone.w > 180 and zone.h > 145) OutputsWidget::zoneLarge();
  else if (zone.w > 170 and zone.h > 65) OutputsWidget::zoneSmall();
}

BaseWidgetFactory<OutputsWidget> outputsWidget("Outputs", OutputsWidget::options);
