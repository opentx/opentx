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

class TimerWidget: public Widget
{
  public:
    TimerWidget(const WidgetFactory * factory, const Zone & zone, Widget::PersistentData * persistentData):
      Widget(factory, zone, persistentData)
    {
    }

    virtual void refresh();

    static const ZoneOption options[];
};

const ZoneOption TimerWidget::options[] = {
  { "Timer source", ZoneOption::Timer, OPTION_VALUE_UNSIGNED(0) },
  { NULL, ZoneOption::Bool }
};

void TimerWidget::refresh()
{
  uint32_t index = persistentData->options[0].unsignedValue;
  TimerData & timerData = g_model.timers[index];
  TimerState & timerState = timersStates[index];

  if (zone.w >= 180 && zone.h >= 70) {
    if (timerState.val >= 0 || !(timerState.val % 2)) {
      lcdDrawBitmapPattern(zone.x, zone.y, LBM_TIMER_BACKGROUND, MAINVIEW_PANES_COLOR);
    }
    else {
      lcdDrawBitmapPattern(zone.x, zone.y, LBM_TIMER_BACKGROUND, HEADER_BGCOLOR);
    }
    if (timerData.start && timerState.val >= 0) {
      lcdDrawBitmapPatternPie(
        zone.x + 2,
        zone.y + 3, LBM_RSCALE, MAINVIEW_GRAPHICS_COLOR, 0,
        timerState.val <= 0 ? 360 : 360 * (timerData.start - timerState.val) / timerData.start);
    }
    else {
      lcdDrawBitmapPattern(zone.x + 3, zone.y + 4, LBM_TIMER, MAINVIEW_GRAPHICS_COLOR);
    }
    if (abs(timerState.val) >= 3600) {
      drawTimer(zone.x + 70, zone.y + 31, abs(timerState.val), TEXT_COLOR | MIDSIZE | LEFT | TIMEHOUR);
    }
    else {
      drawTimer(zone.x + 76, zone.y + 31, abs(timerState.val), TEXT_COLOR | DBLSIZE | LEFT);
    }
    if (ZLEN(timerData.name) > 0) {
      lcdDrawSizedText(zone.x + 78, zone.y + 20, timerData.name, LEN_TIMER_NAME, ZCHAR | SMLSIZE | TEXT_COLOR);
    }
    else {
      drawStringWithIndex(zone.x + 137, zone.y + 17, "TMR", index + 1, SMLSIZE | TEXT_COLOR);
    }
  }
  else {
    if (timerState.val < 0 && timerState.val % 2) {
      lcdDrawSolidFilledRect(zone.x, zone.y, zone.w, zone.h, HEADER_ICON_BGCOLOR);
    }
    drawStringWithIndex(zone.x + 2, zone.y, "TMR", index + 1, SMLSIZE | TEXT_INVERTED_COLOR);
    if (zone.w > 100 && zone.h > 40) {
      if (abs(timerState.val) >= 3600) {
        drawTimer(zone.x + 3, zone.y + 16, abs(timerState.val), TEXT_INVERTED_COLOR | LEFT | TIMEHOUR);
      }
      else {
        drawTimer(zone.x + 3, zone.y + 16, abs(timerState.val), TEXT_INVERTED_COLOR | LEFT | MIDSIZE);
      }
    }
    else {
      if (abs(timerState.val) >= 3600) {
        drawTimer(zone.x + 3, zone.y + 14, abs(timerState.val), TEXT_INVERTED_COLOR | LEFT | SMLSIZE | TIMEHOUR);
      }
      else {
        drawTimer(zone.x + 3, zone.y + 14, abs(timerState.val), TEXT_INVERTED_COLOR | LEFT);
      }
    }
  }
}

BaseWidgetFactory<TimerWidget> timerWidget("Timer", TimerWidget::options);
