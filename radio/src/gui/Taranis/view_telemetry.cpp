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
#define TELEM_2ND_COLUMN (11*FW)

#if defined(FRSKY_HUB) && defined(GAUGES)
bar_threshold_t barsThresholds[THLD_MAX];
#endif

uint8_t s_frsky_view = 0;

#define BAR_LEFT    30
#define BAR_WIDTH   152

void displayRssiLine()
{
  if (TELEMETRY_STREAMING()) {
    lcd_hline(0, 55, 212, 0); // separator
    uint8_t rssi = min((uint8_t)99, TELEMETRY_RSSI());
    lcd_putsn(0, STATUS_BAR_Y, STR_RX, 2); lcd_outdezNAtt(4*FW, STATUS_BAR_Y, rssi, LEADING0, 2);
    lcd_rect(BAR_LEFT, 57, 78, 7);
    drawFilledRect(BAR_LEFT+1, 58, 19*rssi/25, 5, (rssi < getRssiAlarmValue(0)) ? DOTTED : SOLID);
  }
  else {
    lcd_putsAtt(7*FW, STATUS_BAR_Y, STR_NODATA, BLINK);
    lcd_status_line();
  }
}

NOINLINE uint8_t getRssiAlarmValue(uint8_t alarm)
{
  return (45 - 3*alarm + g_model.frsky.rssiAlarms[alarm].value);
}

int barCoord(int value, int min, int max)
{
  if (value <= min)
    return 0;
  else if (value >= max)
    return BAR_WIDTH-1;
  else
    return ((BAR_WIDTH-1) * (value - min)) / (max - min);
}

void displayGaugesTelemetryScreen(FrSkyScreenData & screen)
{
  // Custom Screen with gauges
  int barHeight = 5;
  for (int i=3; i>=0; i--) {
    FrSkyBarData & bar = screen.bars[i];
    source_t source = bar.source;
    getvalue_t barMin = bar.barMin;
    getvalue_t barMax = bar.barMax;
    if (source <= MIXSRC_LAST_CH) {
      barMin = calc100toRESX(barMin);
      barMax = calc100toRESX(barMax);
    }
    if (source && barMax > barMin) {
      int y = barHeight+6+i*(barHeight+6);
      putsMixerSource(0, y+barHeight-5, source, 0);
      lcd_rect(BAR_LEFT, y, BAR_WIDTH+1, barHeight+2);
      getvalue_t value = getValue(source);
      putsChannel(BAR_LEFT+2+BAR_WIDTH, y+barHeight-5, source, LEFT);
      uint8_t thresholdX = 0;
      int width = barCoord(value, barMin, barMax);
      uint8_t barShade = SOLID;
      drawFilledRect(BAR_LEFT+1, y+1, width, barHeight, barShade);
      for (uint8_t j=24; j<99; j+=25) {
        if (j>thresholdX || j>width) {
          lcd_vline(j*BAR_WIDTH/100+BAR_LEFT+1, y+1, barHeight);
        }
      }
      if (thresholdX) {
        lcd_vlineStip(BAR_LEFT+1+thresholdX, y-2, barHeight+3, DOTTED);
        lcd_hline(BAR_LEFT+thresholdX, y-2, 3);
      }
    }
    else {
      barHeight += 2;
    }
  }
  displayRssiLine();
}

bool displayNumbersTelemetryScreen(FrSkyScreenData & screen)
{
  // Custom Screen with numbers
  uint8_t fields_count = 0;
  for (uint8_t i=0; i<4; i++) {
    for (uint8_t j=0; j<NUM_LINE_ITEMS; j++) {
      source_t field = screen.lines[i].sources[j];
      if (field > 0) {
        fields_count++;
      }
      if (i==3) {
        lcd_vline(69, 8, 48);
        lcd_vline(141, 8, 48);
        if (!TELEMETRY_STREAMING()) {
          displayRssiLine();
          return fields_count;
        }
      }
      if (field) {
        LcdFlags att = (i==3 ? NO_UNIT : DBLSIZE|NO_UNIT);
        coord_t pos[] = {0, 71, 143, 214};
        if (field >= MIXSRC_FIRST_TIMER && field <= MIXSRC_LAST_TIMER && i!=3) {
          // there is not enough space on LCD for displaying "Tmr1" or "Tmr2" and still see the - sign, we write "T1" or "T2" instead
          putsStrIdx(pos[j], 1+FH+2*FH*i, "T", field-MIXSRC_FIRST_TIMER+1, 0);
        }
        else if (field >= MIXSRC_FIRST_TELEM && isGPSSensor(1+(field-MIXSRC_FIRST_TELEM)/3) && telemetryItems[(field-MIXSRC_FIRST_TELEM)/3].isAvailable()) {
          // we don't display GPS name, no space for it
        }
        else {
          putsMixerSource(pos[j], 1+FH+2*FH*i, field, 0);
        }
        
        if (field >= MIXSRC_FIRST_TELEM) {
          TelemetryItem & telemetryItem = telemetryItems[(field-MIXSRC_FIRST_TELEM)/3]; // TODO macro to convert a source to a telemetry index
          if (!telemetryItem.isAvailable()) {
            continue;
          }
          else if (telemetryItem.isOld()) {
            att |= INVERS|BLINK;
          }
        }

        putsChannel(pos[j+1]-2, (i==3 ? 1+FH+2*FH*i:FH+2*FH*i), field, att);

      }
    }
  }
  lcd_status_line();
  return fields_count;
}

void displayCustomTelemetryScreen(uint8_t index)
{
  FrSkyScreenData & screen = g_model.frsky.screens[index];

#if defined(GAUGES)
  if (IS_BARS_SCREEN(s_frsky_view)) {
    return displayGaugesTelemetryScreen(screen);
  }
#endif

  displayNumbersTelemetryScreen(screen);
}

bool displayTelemetryScreen()
{
#if defined(LUA)
  if (TELEMETRY_SCREEN_TYPE(s_frsky_view) == TELEMETRY_SCREEN_TYPE_SCRIPT) {
    return true;
  }
#endif

  if (TELEMETRY_SCREEN_TYPE(s_frsky_view) == TELEMETRY_SCREEN_TYPE_NONE) {
    return false;
  }

  lcdDrawTelemetryTopBar();

  if (s_frsky_view < MAX_TELEMETRY_SCREENS) {
    displayCustomTelemetryScreen(s_frsky_view);
  }

  return true;
}

enum NavigationDirection {
  none,
  up,
  down
};

#define decrTelemetryScreen() direction = up
#define incrTelemetryScreen() direction = down

void menuTelemetryFrsky(uint8_t event)
{
  enum NavigationDirection direction = none;

  switch (event) {
    case EVT_KEY_FIRST(KEY_EXIT):
      killEvents(event);
      chainMenu(menuMainView);
      break;

    case EVT_KEY_LONG(KEY_PAGE):
      killEvents(event);
      // no break;

    case EVT_KEY_FIRST(KEY_UP):
      decrTelemetryScreen();
      break;

    case EVT_KEY_BREAK(KEY_PAGE):
    case EVT_KEY_FIRST(KEY_DOWN):
      incrTelemetryScreen();
      break;

    case EVT_KEY_LONG(KEY_ENTER):
      killEvents(event);
      MENU_ADD_ITEM(STR_RESET_TELEMETRY);
      MENU_ADD_ITEM(STR_RESET_FLIGHT);
      menuHandler = onMainViewMenu;
      break;
  }

  for (int i=0; i<=TELEMETRY_SCREEN_TYPE_MAX; i++) {
    if (direction == up) {
      if (s_frsky_view-- == 0)
        s_frsky_view = TELEMETRY_VIEW_MAX;
    }
    else if (direction == down) {
      if (s_frsky_view++ == TELEMETRY_VIEW_MAX)
        s_frsky_view = 0;
    }
    else {
      direction = down;
    }
    if (displayTelemetryScreen()) {
      return;
    }
  }

  lcdDrawTelemetryTopBar();
  lcd_puts(8*FW, 3*FH, "No Telemetry Screens");
  displayRssiLine();
}
