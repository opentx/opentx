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

class GaugeWidget: public Widget
{
  public:
    GaugeWidget(const WidgetFactory * factory, const Zone & zone, Widget::PersistentData * persistentData):
      Widget(factory, zone, persistentData)
    {
    }

    virtual void refresh();

    static const ZoneOption options[];
};

const ZoneOption GaugeWidget::options[] = {
  { "Source", ZoneOption::Source, OPTION_VALUE_UNSIGNED(1) },
  { "Min", ZoneOption::Integer, OPTION_VALUE_SIGNED(-RESX), OPTION_VALUE_SIGNED(-RESX), OPTION_VALUE_SIGNED(RESX) },
  { "Max", ZoneOption::Integer, OPTION_VALUE_SIGNED(RESX), OPTION_VALUE_SIGNED(-RESX), OPTION_VALUE_SIGNED(RESX) },
  { "Color", ZoneOption::Color, OPTION_VALUE_UNSIGNED(RED) },
  { NULL, ZoneOption::Bool }
};

void GaugeWidget::refresh()
{
  mixsrc_t index = persistentData->options[0].unsignedValue;
  int32_t min = persistentData->options[1].signedValue;
  int32_t max = persistentData->options[2].signedValue;
  uint16_t color = persistentData->options[3].unsignedValue;

  int32_t value = getValue(index);
  int32_t value_in_range = value;
  if (value < min)
    value_in_range = min;
  else if (value > max)
    value_in_range = max;

  int w = divRoundClosest(zone.w * (value_in_range - min), (max - min));
  int percent = divRoundClosest(100 * (value_in_range - min), (max - min));

  // Gauge label
  drawSource(zone.x, zone.y, index, SMLSIZE | TEXT_INVERTED_COLOR);

  // Gauge
  lcdSetColor(color);
  lcdDrawSolidFilledRect(zone.x, zone.y + 16, zone.w, 16, TEXT_INVERTED_COLOR);
  lcdDrawNumber(zone.x+zone.w/2, zone.y + 17, percent, SMLSIZE | CUSTOM_COLOR | CENTERED, 0, NULL, "%");
  lcd->invertRect(zone.x + w, zone.y + 16, zone.w - w, 16, CUSTOM_COLOR);
}

BaseWidgetFactory<GaugeWidget> gaugeWidget("Gauge", GaugeWidget::options);
