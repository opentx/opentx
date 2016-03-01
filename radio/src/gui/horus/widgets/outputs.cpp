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

    static const ZoneOption options[];
};

const ZoneOption OutputsWidget::options[] = {
  { "Min", ZoneOption::Integer, OPTION_DEFAULT_VALUE({ .unsignedValue = 0 }) },
  { "Max", ZoneOption::Integer, OPTION_DEFAULT_VALUE({ .unsignedValue = 7 }) },
  { "Color", ZoneOption::Color, OPTION_DEFAULT_VALUE({ .unsignedValue = RED }) },
  { NULL, ZoneOption::Bool }
};

void OutputsWidget::refresh()
{
  int firstChannel = MIXSRC_CH1 + persistentData->options[0].unsignedValue;
  int lastChannel = MIXSRC_CH1 + persistentData->options[1].unsignedValue;
  uint16_t color = persistentData->options[2].unsignedValue;

  lcdSetColor(color);
  lcd->drawSolidHorizontalLine(zone.x, zone.y+zone.h/2, zone.w, CUSTOM_COLOR);

  int count = lastChannel - firstChannel + 1;

  for (int ch=firstChannel, i=0; ch<=lastChannel; ch++, i++) {
    int32_t value = limit<int32_t>(-RESX, getValue(ch), RESX);
    int h = divRoundClosest((zone.h-1)/2 * value, RESX);
    lcdDrawSolidFilledRect(zone.x+(zone.w*i)/count, zone.y + zone.h/2, zone.w/count-1, -h, CUSTOM_COLOR);
  }
}

BaseWidgetFactory<OutputsWidget> outputsWidget("Outputs", OutputsWidget::options);
