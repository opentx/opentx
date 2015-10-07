/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "../../opentx.h"

#define STATUS_BAR_Y     (7*FH+1)
#define TELEM_2ND_COLUMN (11*10)

#if defined(FRSKY_HUB) && defined(GAUGES)
bar_threshold_t barsThresholds[THLD_MAX];
#endif

NOINLINE uint8_t getRssiAlarmValue(uint8_t alarm)
{
  return (45 - 3*alarm + g_model.frsky.rssiAlarms[alarm].value);
}

int barCoord(int value, int min, int max, int width)
{
  if (value <= min)
    return 0;
  else if (value >= max)
    return width;
  else
    return (width * (value - min) + (max - min)/2) / (max - min);
}

void displayGaugesTelemetryScreen(FrSkyScreenData & screen)
{
  const int GAUGE_MARGIN = 16;
  const int GAUGE_HEIGHT = 25;
  const int GAUGE_WIDTH = LCD_W - 2*GAUGE_MARGIN;
  const int GAUGE_PADDING_HORZ = 4;
  const int GAUGE_PADDING_VERT = 2;
  const int VAL_WIDTH = 50;
  const int BAR_WIDTH = GAUGE_WIDTH-2*GAUGE_PADDING_HORZ-VAL_WIDTH;

  for (int i=0; i<4; ++i) {
    FrSkyBarData & bar = screen.bars[i];
    source_t source = bar.source;
    getvalue_t barMin = bar.barMin;
    getvalue_t barMax = bar.barMax;
    if (source <= MIXSRC_LAST_CH) {
      barMin = calc100toRESX(barMin);
      barMax = calc100toRESX(barMax);
    }
    if (source && barMax > barMin) {
      int y = GAUGE_MARGIN + i*(GAUGE_HEIGHT+GAUGE_MARGIN);

      // The black background
      lcdDrawFilledRect(GAUGE_MARGIN, y, GAUGE_WIDTH, GAUGE_HEIGHT, TEXT_BGCOLOR);

      getvalue_t value = getValue(source);
      LcdFlags color = TEXT_COLOR;
      if (source >= MIXSRC_FIRST_TELEM) {
        TelemetryItem & telemetryItem = telemetryItems[(source-MIXSRC_FIRST_TELEM)/3]; // TODO macro to convert a source to a telemetry index
        if (!telemetryItem.isAvailable()) {
          continue;
        }
        else if (telemetryItem.isOld()) {
          color = ALARM_COLOR;
        }
      }
      else if (source >= MIXSRC_FIRST_TIMER && source <= MIXSRC_LAST_TIMER) {
        if (value < 0) {
          putsMixerSource(GAUGE_MARGIN+GAUGE_PADDING_HORZ, y+GAUGE_PADDING_VERT, source, ALARM_COLOR);
          putsTimer(GAUGE_MARGIN+GAUGE_WIDTH-VAL_WIDTH, y+GAUGE_PADDING_VERT+3, -value, LEFT|DBLSIZE|ALARM_COLOR);
          // TODO lcdDrawFilledRect(GAUGE_MARGIN+GAUGE_PADDING_HORZ, y+15, BAR_WIDTH, 6, ALARM_BGCOLOR);
          continue;
        }
      }
      
      // The label
      putsMixerSource(GAUGE_MARGIN+GAUGE_PADDING_HORZ, y+GAUGE_PADDING_VERT, source, color);

      // The value
      putsChannel(GAUGE_MARGIN+GAUGE_WIDTH-VAL_WIDTH, y+GAUGE_PADDING_VERT+3, source, LEFT|DBLSIZE|NO_UNIT|color);

      // The bar
      // TODO lcdDrawFilledRect(GAUGE_MARGIN+GAUGE_PADDING_HORZ, y+15, BAR_WIDTH, 6, SOLID, color);
      //uint8_t thresholdX = 0;
      int width = barCoord(value, barMin, barMax, BAR_WIDTH-2);
      //uint8_t barShade = SOLID;
      lcdDrawFilledRect(GAUGE_MARGIN+GAUGE_PADDING_HORZ+1, y+16, width, 4, TEXT_BGCOLOR);
/*      for (uint8_t j=24; j<99; j+=25) {
        if (j>thresholdX || j>width) {
          lcd_vline(j*BAR_WIDTH/100+BAR_LEFT+1, y+1, barHeight);
        }
      }
      if (thresholdX) {
        lcd_vlineStip(BAR_LEFT+1+thresholdX, y-2, barHeight+3, DOTTED);
        lcd_hline(BAR_LEFT+thresholdX, y-2, 3);
      }
      */
    }
  }
}

void displayNumbersTelemetryScreen(FrSkyScreenData & screen)
{
  const int NUMBERS_W = 56;
  const int NUMBERS_H = 30;
  const int NUMBERS_MARGIN = 16;
  const int NUMBERS_PADDING = 4;

  for (int i=0; i<4; i++) {
    for (int j=0; j<NUM_LINE_ITEMS; j++) {
      source_t field = screen.lines[i].sources[j];
      if (field) {
        LcdFlags color = WHITE;
        coord_t pos[] = { NUMBERS_MARGIN, (LCD_W-NUMBERS_W)/2, LCD_W-NUMBERS_MARGIN-NUMBERS_W };
        int x = pos[j];
        int y = NUMBERS_MARGIN + i*(NUMBERS_H+NUMBERS_MARGIN);
        lcdDrawFilledRect(x, y, NUMBERS_W, NUMBERS_H, TEXT_BGCOLOR);
        if (field >= MIXSRC_FIRST_TELEM) {
          TelemetryItem & telemetryItem = telemetryItems[(field-MIXSRC_FIRST_TELEM)/3]; // TODO macro to convert a source to a telemetry index
          if (!telemetryItem.isAvailable()) {
            continue;
          }
          else if (telemetryItem.isOld()) {
            color = RED;
          }
        }
        else if (field >= MIXSRC_FIRST_TIMER && field <= MIXSRC_LAST_TIMER) {
          TimerState & timerState = timersStates[field-MIXSRC_FIRST_TIMER];
          if (timerState.val < 0) {
            color = RED;
          }
          putsMixerSource(x+NUMBERS_PADDING, y+2, field, color);
          putsTimer(x+NUMBERS_PADDING, y+12, abs(timerState.val), LEFT|DBLSIZE|color);
          continue;
        }
        putsMixerSource(x+NUMBERS_PADDING, y+2, field, color);
        putsChannel(x+NUMBERS_PADDING, y+12, field, LEFT|DBLSIZE|NO_UNIT|color);
      }
    }
  }
}

void displayCustomTelemetryScreen(uint8_t index)
{
  FrSkyScreenData & screen = g_model.frsky.screens[index];

#if defined(GAUGES)
  if (IS_BARS_SCREEN(index)) {
    return displayGaugesTelemetryScreen(screen);
  }
#endif

  displayNumbersTelemetryScreen(screen);
}

void displayTelemetryScreen(int index, unsigned int evt)
{
#if defined(LUA)
  if (TELEMETRY_SCREEN_TYPE(index) == TELEMETRY_SCREEN_TYPE_SCRIPT) {
    luaTask(evt, RUN_TELEM_FG_SCRIPT, true);
    return;
  }
#endif

  if (TELEMETRY_SCREEN_TYPE(index) == TELEMETRY_SCREEN_TYPE_NONE) {
    return;
  }

  if (index < MAX_TELEMETRY_SCREENS) {
    return displayCustomTelemetryScreen(index);
  }
}
