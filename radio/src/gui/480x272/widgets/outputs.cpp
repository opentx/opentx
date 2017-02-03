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

#define RECT_BORDER                    1
#define RECT_WIDTH                     (w - RECT_BORDER * 2)
#define ROW_HEIGHT                     17

class OutputsWidget: public Widget
{
  public:
    OutputsWidget(const WidgetFactory * factory, const Zone & zone, Widget::PersistentData * persistentData):
      Widget(factory, zone, persistentData)
    {
    }

    virtual void refresh();

    uint8_t drawChannels(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t firstChan, bool bg_shown, uint16_t bg_color)
    {
      char chanString[] = "CH32";
      uint8_t numChan = h / ROW_HEIGHT;
      uint8_t row_height = (h - numChan * ROW_HEIGHT >= numChan ? ROW_HEIGHT + 1 : ROW_HEIGHT);
      uint8_t lastChan = firstChan + numChan;

      for (uint8_t curChan = firstChan; curChan < lastChan && curChan < 33; curChan++) {
        int16_t chanVal = calcRESXto100(channelOutputs[curChan-1]);
        if (bg_shown) {
          lcdSetColor(bg_color);
          lcdDrawSolidFilledRect(x + RECT_BORDER, y + RECT_BORDER + (curChan - firstChan) * row_height, RECT_WIDTH , row_height - RECT_BORDER, CUSTOM_COLOR);
        }
        if (chanVal > 0) {
          lcdDrawSolidFilledRect(x + RECT_BORDER + RECT_WIDTH / 2,  y + RECT_BORDER + (curChan -firstChan) * row_height, divRoundClosest(RECT_WIDTH * chanVal, 200), row_height - RECT_BORDER, MAINVIEW_GRAPHICS_COLOR);
        }
        else if (chanVal < 0) {
          uint16_t startpoint = x + RECT_BORDER;
          uint16_t endpoint = startpoint + RECT_WIDTH / 2;
          uint16_t size = divRoundClosest(- RECT_WIDTH * chanVal, 200);
          lcdDrawSolidFilledRect(endpoint - size,  y + RECT_BORDER + (curChan - firstChan) * row_height, size, row_height - RECT_BORDER, MAINVIEW_GRAPHICS_COLOR);
        }
        lcd->drawSolidVerticalLine(x + RECT_BORDER + RECT_WIDTH / 2, y + RECT_BORDER + (curChan - firstChan) * row_height, row_height - RECT_BORDER, MAINVIEW_GRAPHICS_COLOR);
        lcdDrawRect(x, y + (curChan - firstChan) * row_height, w, row_height+1);
        lcdDrawNumber(x + RECT_WIDTH - 10, y + (curChan - firstChan) * row_height + 1, chanVal, SMLSIZE | TEXT_COLOR | RIGHT, 0, NULL, "%");
        if (g_model.limitData[curChan - 1].name[0] != 0)  {
          strAppendSigned(chanString, curChan, 2);
          lcdDrawText(x + 2, y + (curChan - firstChan) * row_height + 1, chanString, SMLSIZE | TEXT_COLOR | LEFT);
          lcdDrawSizedText(x + 25, y + (curChan - firstChan) * row_height + 1, g_model.limitData[curChan - 1].name, sizeof(g_model.limitData[curChan - 1].name), SMLSIZE | TEXT_COLOR | LEFT | ZCHAR);
      } 
        else {
          strAppend(chanString, "CH");
          strAppendSigned(&chanString[2], curChan, 2);
          lcdDrawText(x + 2, y + (curChan - firstChan) * row_height + 1, chanString, SMLSIZE | TEXT_COLOR | LEFT);
        }
      }
      return lastChan - 1;
    }

    void twoColumns()
    {
      uint8_t endColumn = drawChannels(zone.x, zone.y, zone.w / 2, zone.h, persistentData->options[0].unsignedValue, persistentData->options[1].boolValue, persistentData->options[2].unsignedValue);
      drawChannels(zone.x + zone.w / 2 + 2, zone.y, zone.w / 2, zone.h, endColumn + 1, persistentData->options[1].boolValue, persistentData->options[2].unsignedValue);
    }

    void oneColumn()
    {
      drawChannels(zone.x, zone.y, zone.w, zone.h, persistentData->options[0].unsignedValue, persistentData->options[1].boolValue, persistentData->options[2].unsignedValue);
    }

    static const ZoneOption options[];
};

const ZoneOption OutputsWidget::options[] = {
  { "First channel", ZoneOption::Integer, OPTION_VALUE_UNSIGNED(1), OPTION_VALUE_UNSIGNED(1), OPTION_VALUE_UNSIGNED(32) },
  { "Fill Background?", ZoneOption::Bool, OPTION_VALUE_BOOL(false) },
  { "BG Color", ZoneOption::Color, OPTION_VALUE_UNSIGNED(LIGHTGREY) },
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
