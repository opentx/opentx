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
    ValueWidget(const WidgetFactory * factory, Window * parent, const rect_t & rect, Widget::PersistentData * persistentData):
      Widget(factory, parent, rect, persistentData)
    {
    }

    void paint(BitmapBuffer * dc) override
    {
      // TODO DELETE THOSE TEST INIT VALUE !!
      persistentData->options[0].value.unsignedValue = 1;
      // TODO END TEST INIT VALUE

      const int NUMBERS_PADDING = 4;

      mixsrc_t field = persistentData->options[0].value.unsignedValue;
      lcdSetColor(persistentData->options[1].value.unsignedValue);

      dc->clear(DEFAULT_BGCOLOR);
      dc->drawFilledRect(0,0, width(), height(), SOLID, MAINVIEW_PANES_COLOR | OPACITY(5));

      coord_t x = 0, y = 0, xValue = 0, yValue = 0, xLabel = 0, yLabel = 0;
      LcdFlags attrValue, attrLabel = 0;

      if (width() < 120 && height() < 50) {
        xValue = x;
        yValue = y + 14;
        xLabel = x;
        yLabel = y;
        attrValue = LEFT | NO_UNIT | FONT(XS);
        attrLabel = FONT(XS);
      }
      else if (height() < 50) {
        xValue = x + width() - NUMBERS_PADDING;
        yValue = y - 2;
        xLabel = x + NUMBERS_PADDING;
        yLabel = y + 2;
        attrValue = RIGHT | NO_UNIT | FONT(XL);
      }
      else {
        xValue = x + NUMBERS_PADDING;
        yValue = y + 18;
        xLabel = x + NUMBERS_PADDING;
        yLabel = y + 2;
        if (field >= MIXSRC_FIRST_TELEM) {
          if (isGPSSensor(1 + (field - MIXSRC_FIRST_TELEM) / 3)) {
            attrValue = LEFT | FONT(L) | EXPANDED;
          }
          else {
            attrValue = LEFT | FONT(XL);
          }
        }
#if defined(INTERNAL_GPS)
        else if (field == MIXSRC_TX_GPS) {
            attrValue = LEFT | FONT(L) | EXPANDED;
          }
#endif
        else {
          attrValue = LEFT | FONT(XL);
        }
      }

      if (field >= MIXSRC_FIRST_TIMER && field <= MIXSRC_LAST_TIMER) {
        TimerState & timerState = timersStates[field - MIXSRC_FIRST_TIMER];
        if (timerState.val < 0) {
          lcdSetColor(lcdColorTable[ALARM_COLOR_INDEX]);
        }
        drawSource(dc, x + NUMBERS_PADDING, y + 2, field, CUSTOM_COLOR);
        drawSource(dc, x + NUMBERS_PADDING + 1, y + 3, field, BLACK);
        drawTimer(dc, xValue, yValue, abs(timerState.val), attrValue | FONT(XL) | CUSTOM_COLOR);
        return;
      }

      if (field >= MIXSRC_FIRST_TELEM) {
        TelemetryItem & telemetryItem = telemetryItems[(field - MIXSRC_FIRST_TELEM) / 3]; // TODO macro to convert a source to a telemetry index
        if (!telemetryItem.isAvailable() || telemetryItem.isOld()) {
          lcdSetColor(lcdColorTable[TEXT_DISABLE_COLOR_INDEX]);
        }
      }

      if (persistentData->options[2].value.boolValue) {
        drawSource(dc,xLabel + 1, yLabel + 1, field, attrLabel | BLACK);
        drawSourceValue(dc, xValue + 1, yValue + 1, field, attrValue | BLACK);
      }
      drawSource(dc, xLabel, yLabel, field, attrLabel | CUSTOM_COLOR);
      drawSourceValue(dc, xValue, yValue, field, attrValue | CUSTOM_COLOR);

    }

    void checkEvents() override
    {
      if (lastValue != getValue(persistentData->options[0].value.unsignedValue)) {
        lastValue = getValue(persistentData->options[0].value.unsignedValue);
        invalidate();
      }
    }

    static const ZoneOption options[];
    int32_t lastValue = 0;
};

const ZoneOption ValueWidget::options[] = {
  { STR_SOURCE, ZoneOption::Source, OPTION_VALUE_UNSIGNED(MIXSRC_Rud) },
  { STR_COLOR, ZoneOption::Color, OPTION_VALUE_UNSIGNED(WHITE) },
  { STR_SHADOW, ZoneOption::Bool, OPTION_VALUE_BOOL(false)  },
  { nullptr, ZoneOption::Bool }
};

//void ValueWidget::refresh()
//{
//  const int NUMBERS_PADDING = 4;
//
//  mixsrc_t field = persistentData->options[0].value.unsignedValue;
//  lcdSetColor(persistentData->options[1].value.unsignedValue);
//
//  int x = zone.x;
//  int y = zone.y;
//
//  // TRACE("w=%d, h=%d", width(), zone.h);
//
//  // lcdDrawFilledRect(zone.x, zone.y, width(), zone.h, SOLID, MAINVIEW_PANES_COLOR | OPACITY(5));
//
//  int xValue, yValue, xLabel, yLabel;
//  LcdFlags attrValue, attrLabel=0;
//  if (width() < 120 && zone.h < 50) {
//    xValue = x;
//    yValue = y+14;
//    xLabel = x;
//    yLabel = y;
//    attrValue = LEFT | NO_UNIT | FONT(L);
//    attrLabel = FONT(XS);
//  }
//  else if (zone.h < 50) {
//    xValue = x+width()-NUMBERS_PADDING;
//    yValue = y-2;
//    xLabel = x+NUMBERS_PADDING;
//    yLabel = y+2;
//    attrValue = RIGHT | NO_UNIT | FONT(XL);
//  }
//  else {
//    xValue = x+NUMBERS_PADDING;
//    yValue = y+18;
//    xLabel = x+NUMBERS_PADDING;
//    yLabel = y+2;
//    if (field >= MIXSRC_FIRST_TELEM) {
//      if (isGPSSensor(1+(field-MIXSRC_FIRST_TELEM)/3)) {
//        attrValue = LEFT | FONT(L) | EXPANDED;
//      }
//      else {
//        attrValue = LEFT | FONT(XL);
//      }
//    }
//#if defined(INTERNAL_GPS)
//    else if (field == MIXSRC_TX_GPS) {
//      attrValue = LEFT | FONT(L) | EXPANDED;
//    }
//#endif
//    else {
//      attrValue = LEFT | FONT(XL);
//    }
//  }
//
//  if (field >= MIXSRC_FIRST_TIMER && field <= MIXSRC_LAST_TIMER) {
//    TimerState & timerState = timersStates[field-MIXSRC_FIRST_TIMER];
//    if (timerState.val < 0) {
//      lcdSetColor(lcdColorTable[ALARM_COLOR_INDEX]);
//    }
//    drawSource(x+NUMBERS_PADDING, y+2, field, CUSTOM_COLOR);
//    drawSource(x+NUMBERS_PADDING + 1, y + 3, field, BLACK);
//    drawTimer(xValue, yValue, abs(timerState.val), attrValue|FONT(XL)|CUSTOM_COLOR);
//    return;
//  }
//
//  if (field >= MIXSRC_FIRST_TELEM) {
//    TelemetryItem & telemetryItem = telemetryItems[(field-MIXSRC_FIRST_TELEM)/3]; // TODO macro to convert a source to a telemetry index
//    if (!telemetryItem.isAvailable() || telemetryItem.isOld()) {
//      lcdSetColor(lcdColorTable[TEXT_DISABLE_COLOR_INDEX]);
//    }
//  }
//
//  if(persistentData->options[2].value.boolValue) {
//    drawSource(xLabel + 1, yLabel + 1, field, attrLabel|BLACK);
//    drawSourceValue(xValue + 1, yValue + 1, field, attrValue|BLACK);
//  }
//  drawSource(xLabel, yLabel, field, attrLabel|CUSTOM_COLOR);
//  drawSourceValue(xValue, yValue, field, attrValue|CUSTOM_COLOR);
//
//}

BaseWidgetFactory<ValueWidget> ValueWidget("Value", ValueWidget::options);
