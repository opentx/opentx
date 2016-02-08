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

    void refresh();

    static const ZoneOption options[];
};

const ZoneOption TimerWidget::options[] = {
  { "Timer source", ZoneOption::Timer },
  { NULL, ZoneOption::Bool }
};

void TimerWidget::refresh()
{
  uint32_t index = persistentData->options[0].unsignedValue;

  TimerData & timerData = g_model.timers[index];
  TimerState & timerState = timersStates[index];
  lcdDrawBitmapPattern(zone.x, zone.y, LBM_TIMER_BACKGROUND, TEXT_BGCOLOR);
  if (timerData.start) {
    lcdDrawBitmapPatternPie(zone.x+2, zone.y+3, LBM_RSCALE, TITLE_BGCOLOR, 0, timerState.val <= 0 ? 360 : 360*(timerData.start-timerState.val)/timerData.start);
  }
  else {
    lcdDrawBitmapPattern(zone.x+3, zone.y+4, LBM_TIMER, TITLE_BGCOLOR);
  }
  putsTimer(zone.x+76, zone.y+31, abs(timerState.val), TEXT_COLOR|DBLSIZE|LEFT);
  if (ZLEN(timerData.name) > 0) {
    lcdDrawSizedText(zone.x+78, zone.y+20, timerData.name, LEN_TIMER_NAME, ZCHAR|SMLSIZE|TEXT_COLOR);
  }
  drawStringWithIndex(zone.x+137, zone.y+17, "TMR", index+1, SMLSIZE|TEXT_COLOR);
}

BaseWidgetFactory<TimerWidget> timerWidget("Timer", "", TimerWidget::options);
