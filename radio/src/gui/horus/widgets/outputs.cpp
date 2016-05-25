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

#define RECT_OFFSET 80
#define RECT_WIDTH 100
#define RAW_HEIGHT 20

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
      
      for (uint8_t curChan = firstChan; curChan < lastChan + 1; curChan++)
      {
        int16_t chanVal = calcRESXto1000(channelOutputs[curChan-1]) / 10;
        
        sprintf(chanString,"CH%02i", curChan);
        lcdDrawText(x, y + (curChan - firstChan) * RAW_HEIGHT, chanString,SMLSIZE + TEXT_COLOR + LEFT);
        sprintf(chanString,"%3i", chanVal);
        lcdDrawText(x + RECT_OFFSET - 2, y + (curChan - firstChan) * RAW_HEIGHT, chanString,SMLSIZE + TEXT_COLOR + RIGHT);
        lcdDrawRect(x + RECT_OFFSET, y + (curChan -firstChan) * RAW_HEIGHT, RECT_WIDTH, RAW_HEIGHT);
        lcd->drawSolidVerticalLine(x + RECT_OFFSET + floor(RECT_WIDTH / 2), y + (curChan - firstChan) * RAW_HEIGHT, RAW_HEIGHT, MAINVIEW_GRAPHICS_COLOR);
        if (chanVal > 0)
        {
          lcdDrawSolidFilledRect(x + RECT_OFFSET + floor(RECT_WIDTH / 2),  y + (curChan -firstChan) * RAW_HEIGHT, RECT_WIDTH * chanVal / 200 , RAW_HEIGHT, MAINVIEW_GRAPHICS_COLOR);
        }
        else if (chanVal < 0)
        {
          uint16_t endpoint = x + RECT_OFFSET + floor(RECT_WIDTH / 2);
          uint16_t startpoint = x + RECT_OFFSET;
          uint16_t size = floor(abs((endpoint - startpoint) * chanVal / 100));
          lcdDrawSolidFilledRect(endpoint - size,  y + (curChan - firstChan) * RAW_HEIGHT, size, RAW_HEIGHT, MAINVIEW_GRAPHICS_COLOR);
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
