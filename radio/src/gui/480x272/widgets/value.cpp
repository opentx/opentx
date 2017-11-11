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

class ValueWidget: public Widget
{
  public:
    ValueWidget(const WidgetFactory * factory, const Zone & zone, Widget::PersistentData * persistentData):
      Widget(factory, zone, persistentData)
    {
    }

    virtual void refresh();

    static const ZoneOption options[];
};

const ZoneOption ValueWidget::options[] = {
  { "Source", ZoneOption::Source, OPTION_VALUE_UNSIGNED(MIXSRC_Rud) },
  { "Color", ZoneOption::Color, OPTION_VALUE_UNSIGNED(WHITE) },
  { "Shadow", ZoneOption::Bool, OPTION_VALUE_BOOL(false)  },
  { NULL, ZoneOption::Bool }
};

void ValueWidget::refresh()
{
  const int NUMBERS_PADDING = 4;

  mixsrc_t field = persistentData->options[0].unsignedValue;
  lcdSetColor(persistentData->options[1].unsignedValue);

  int x = zone.x;
  int y = zone.y;

  // TRACE("w=%d, h=%d", zone.w, zone.h);

  // lcdDrawFilledRect(zone.x, zone.y, zone.w, zone.h, SOLID, MAINVIEW_PANES_COLOR | OPACITY(5));

  int xValue, yValue, xLabel, yLabel;
  LcdFlags attrValue, attrLabel=0;
  if (zone.w < 120 && zone.h < 50) {
    xValue = x;
    yValue = y+14;
    xLabel = x;
    yLabel = y;
    attrValue = LEFT | NO_UNIT | MIDSIZE;
    attrLabel = SMLSIZE;
  }
  else if (zone.h < 50) {
    xValue = x+zone.w-NUMBERS_PADDING;
    yValue = y-2;
    xLabel = x+NUMBERS_PADDING;
    yLabel = y+2;
    attrValue = RIGHT | NO_UNIT | DBLSIZE;
  }
  else {
    xValue = x+NUMBERS_PADDING;
    yValue = y+18;
    xLabel = x+NUMBERS_PADDING;
    yLabel = y+2;
    if (field >= MIXSRC_FIRST_TELEM) {
      if (isGPSSensor(1+(field-MIXSRC_FIRST_TELEM)/3)) {
        attrValue = LEFT | MIDSIZE | EXPANDED;
      }
      else {
        attrValue = LEFT | DBLSIZE;
      }
    }
#if defined(INTERNAL_GPS)
    else if (field == MIXSRC_TX_GPS) {
      attrValue = LEFT | MIDSIZE | EXPANDED;
    }
#endif
    else {
      attrValue = LEFT | DBLSIZE;
    }
  }

  if (field >= MIXSRC_FIRST_TIMER && field <= MIXSRC_LAST_TIMER) {
    TimerState & timerState = timersStates[field-MIXSRC_FIRST_TIMER];
    if (timerState.val < 0) {
      lcdSetColor(ALARM_COLOR_INDEX);
    }
    drawSource(x+NUMBERS_PADDING, y+2, field, CUSTOM_COLOR);
    drawSource(x+NUMBERS_PADDING + 1, y + 3, field, BLACK);
    drawTimer(xValue, yValue, abs(timerState.val), attrValue|DBLSIZE|CUSTOM_COLOR);
    return;
  }

  if (field >= MIXSRC_FIRST_TELEM) {
    TelemetryItem & telemetryItem = telemetryItems[(field-MIXSRC_FIRST_TELEM)/3]; // TODO macro to convert a source to a telemetry index
    if (!telemetryItem.isAvailable() || telemetryItem.isOld()) {
      lcdSetColor(ALARM_COLOR_INDEX);
    }
  }

  if(persistentData->options[2].boolValue) {
    drawSource(xLabel + 1, yLabel + 1, field, attrLabel|BLACK);
    drawSourceValue(xValue + 1, yValue + 1, field, attrValue|BLACK);
  }
  drawSource(xLabel, yLabel, field, attrLabel|CUSTOM_COLOR);
  drawSourceValue(xValue, yValue, field, attrValue|CUSTOM_COLOR);

}

BaseWidgetFactory<ValueWidget> ValueWidget("Value", ValueWidget::options);
