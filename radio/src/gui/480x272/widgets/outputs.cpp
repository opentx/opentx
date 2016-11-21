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

#define RECT_OFFSET                    80
#define RECT_WIDTH                     (w - RECT_OFFSET)
#define ROW_HEIGHT                     17

class OutputsWidget: public Widget
{
  public:
    OutputsWidget(const WidgetFactory * factory, const Zone & zone, Widget::PersistentData * persistentData):
      Widget(factory, zone, persistentData)
    {
    }

    virtual void refresh();

    uint8_t drawChannels(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t firstChan)
    {
      char chanString[] = "CH32";
      uint8_t numChan = h / ROW_HEIGHT;
      uint8_t row_height = (h - numChan * ROW_HEIGHT >= numChan ? ROW_HEIGHT + 1 : ROW_HEIGHT);
      uint8_t lastChan = firstChan + numChan;

      for (uint8_t curChan = firstChan; curChan < lastChan && curChan < 33; curChan++) {
        int16_t chanVal = calcRESXto100(channelOutputs[curChan-1]);
        strAppend(chanString, "CH");
        strAppendSigned(&chanString[2], curChan, 2);
        lcdDrawText(x, y + (curChan - firstChan) * row_height + 1, chanString, SMLSIZE | TEXT_COLOR | LEFT);
        strAppendSigned(chanString, chanVal);
        lcdDrawText(x + RECT_OFFSET - 2, y + (curChan - firstChan) * row_height + 1, chanString, SMLSIZE | TEXT_COLOR | RIGHT);
        if (chanVal > 0) {
          lcdDrawSolidFilledRect(x + RECT_OFFSET + RECT_WIDTH / 2,  y + (curChan -firstChan) * row_height, divRoundClosest(RECT_WIDTH * chanVal, 200), row_height, MAINVIEW_GRAPHICS_COLOR);
        }
        else if (chanVal < 0) {
          uint16_t startpoint = x + RECT_OFFSET;
          uint16_t endpoint = startpoint + RECT_WIDTH / 2;
          uint16_t size = divRoundClosest(- RECT_WIDTH * chanVal, 200);
          lcdDrawSolidFilledRect(endpoint - size,  y + (curChan - firstChan) * row_height, size, row_height, MAINVIEW_GRAPHICS_COLOR);
        }
        lcdDrawRect(x + RECT_OFFSET, y + (curChan - firstChan) * row_height, RECT_WIDTH, row_height+1);
        lcd->drawSolidVerticalLine(x + RECT_OFFSET + RECT_WIDTH / 2, y + (curChan - firstChan) * row_height, row_height, MAINVIEW_GRAPHICS_COLOR);
      }
      return lastChan - 1;
    }

    void twoColumns()
    {
      uint8_t endColumn = drawChannels(zone.x, zone.y, zone.w / 2, zone.h, persistentData->options[0].unsignedValue);
      drawChannels(zone.x + zone.w / 2 + 2, zone.y, zone.w / 2, zone.h, endColumn + 1);
    }

    void oneColumn()
    {
      drawChannels(zone.x, zone.y, zone.w, zone.h, persistentData->options[0].unsignedValue);
    }

    static const ZoneOption options[];
};

const ZoneOption OutputsWidget::options[] = {
  { "First channel", ZoneOption::Integer, OPTION_VALUE_UNSIGNED(1), OPTION_VALUE_UNSIGNED(1), OPTION_VALUE_UNSIGNED(32) },
  { NULL, ZoneOption::Bool }
};


void OutputsWidget::refresh()
{
  if (zone.w > 300 && zone.h > 20)
    twoColumns();
  else if (zone.w > 150 && zone.h > 20)
    oneColumn();
};

BaseWidgetFactory<OutputsWidget> outputsWidget("Outputs", OutputsWidget::options);
