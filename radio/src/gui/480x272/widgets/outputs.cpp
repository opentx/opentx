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
#define ROW_HEIGHT                     17

#define VIEW_CHANNELS_LIMIT_PCT        (g_model.extendedLimits ? LIMIT_EXT_PERCENT : 100)

class OutputsWidget: public Widget
{
  public:
    OutputsWidget(const WidgetFactory * factory, const Zone & zone, Widget::PersistentData * persistentData):
      Widget(factory, zone, persistentData)
    {
    }

    virtual void refresh();

    uint8_t drawChannels(const uint16_t & x, const uint16_t & y, const uint16_t & w, const uint16_t & h, const uint8_t & firstChan, const bool & bg_shown, const uint16_t & bg_color)
    {
      const uint8_t numChan = h / ROW_HEIGHT;
      const uint8_t lastChan = firstChan + numChan;
      const uint8_t rowH = (h - numChan * ROW_HEIGHT >= numChan ? ROW_HEIGHT + 1 : ROW_HEIGHT);
      const uint16_t barW = w - RECT_BORDER * 2;
      const uint8_t barH = rowH - RECT_BORDER;
      const uint16_t barLft = x + RECT_BORDER;
      const uint16_t barMid = barLft + barW / 2;

      for (uint8_t curChan = firstChan; curChan < lastChan && curChan <= MAX_OUTPUT_CHANNELS; curChan++) {
        const int16_t chanVal = calcRESXto100(channelOutputs[curChan-1]);
        const uint16_t rowTop = y + (curChan - firstChan) * rowH;
        const uint16_t barTop = rowTop + RECT_BORDER;
        const uint16_t fillW = divRoundClosest(barW * limit<int16_t>(0, abs(chanVal), VIEW_CHANNELS_LIMIT_PCT), VIEW_CHANNELS_LIMIT_PCT * 2);

        if (bg_shown) {
          lcdSetColor(bg_color);
          lcdDrawSolidFilledRect(barLft, barTop, barW , barH, CUSTOM_COLOR);
        }
        if (fillW)
          lcdDrawSolidFilledRect((chanVal > 0 ? barMid : barMid - fillW), barTop, fillW, barH, MAINVIEW_GRAPHICS_COLOR);
        lcd->drawSolidVerticalLine(barMid, barTop, barH, MAINVIEW_GRAPHICS_COLOR);
        lcdDrawRect(x, rowTop, w, rowH + 1);
        lcdDrawNumber(x + barW - 10, barTop, chanVal, SMLSIZE | TEXT_COLOR | RIGHT, 0, NULL, "%");
        if (g_model.limitData[curChan - 1].name[0] != 0) {
          lcdDrawNumber(barLft + 1, barTop, curChan, SMLSIZE | TEXT_COLOR | LEFT | LEADING0, 2);
          lcdDrawSizedText(barLft + 23, barTop, g_model.limitData[curChan - 1].name, sizeof(g_model.limitData[curChan - 1].name), SMLSIZE | TEXT_COLOR | LEFT | ZCHAR);
        }
        else {
          putsChn(barLft + 1, barTop, curChan, SMLSIZE | TEXT_COLOR | LEFT);
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
