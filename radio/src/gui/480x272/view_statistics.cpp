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
#include "stamp.h"

#define MENU_STATS_COLUMN1    (MENUS_MARGIN_LEFT + 120)
#define MENU_STATS_COLUMN2    (LCD_W/2)
#define MENU_STATS_COLUMN3    (LCD_W/2 + 120)

bool menuStatsGraph(event_t event)
{
  switch (event) {
    case EVT_KEY_LONG(KEY_ENTER):
      g_eeGeneral.globalTimer = 0;
      storageDirty(EE_GENERAL);
      sessionTimer = 0;
      killEvents(event);
      break;
  }

  SIMPLE_MENU(STR_STATISTICS, STATS_ICONS, menuTabStats, e_StatsGraph, 1);

  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP, "Session");
  drawTimer(MENU_STATS_COLUMN1, MENU_CONTENT_TOP, sessionTimer, TIMEHOUR);
  lcdDrawText(MENU_STATS_COLUMN2, MENU_CONTENT_TOP, "Battery");
  drawTimer(MENU_STATS_COLUMN3, MENU_CONTENT_TOP, g_eeGeneral.globalTimer+sessionTimer, TIMEHOUR);

  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP+FH, "Throttle");
  drawTimer(MENU_STATS_COLUMN1, MENU_CONTENT_TOP+FH, s_timeCumThr, TIMEHOUR);
  lcdDrawText(MENU_STATS_COLUMN2, MENU_CONTENT_TOP+FH, "Throttle %", TIMEHOUR);
  drawTimer(MENU_STATS_COLUMN3, MENU_CONTENT_TOP+FH, s_timeCum16ThrP/16, TIMEHOUR);

  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP+2*FH, "Timers");
  lcdDrawText(MENU_STATS_COLUMN1, MENU_CONTENT_TOP+2*FH, "[1]", HEADER_COLOR);
  drawTimer(lcdNextPos+5, MENU_CONTENT_TOP+2*FH, timersStates[0].val, TIMEHOUR);
  lcdDrawText(MENU_STATS_COLUMN2, MENU_CONTENT_TOP+2*FH, "[2]", HEADER_COLOR);
  drawTimer(lcdNextPos+5, MENU_CONTENT_TOP+2*FH, timersStates[1].val, TIMEHOUR);
#if TIMERS > 2
  lcdDrawText(MENU_STATS_COLUMN3, MENU_CONTENT_TOP+2*FH, "[3]", HEADER_COLOR);
  drawTimer(lcdNextPos+5, MENU_CONTENT_TOP+2*FH, timersStates[2].val, TIMEHOUR);
#endif

  const coord_t x = 10;
  const coord_t y = 240;
  lcdDrawHorizontalLine(x-3, y, MAXTRACE+3+3, SOLID, TEXT_COLOR);
  lcdDrawVerticalLine(x, y-96, 96+3, SOLID, TEXT_COLOR);
  for (coord_t i=0; i<MAXTRACE; i+=6) {
    lcdDrawVerticalLine(x+i, y-1, 3, SOLID, TEXT_COLOR);
  }

  uint16_t traceRd = s_traceWr > MAXTRACE ? s_traceWr - MAXTRACE : 0;
  coord_t prev_yv = (coord_t)-1;
  for (coord_t i=1; i<=MAXTRACE && traceRd<s_traceWr; i++, traceRd++) {
    uint8_t h = s_traceBuf[traceRd % MAXTRACE];
    coord_t yv = y - 2 - 3*h;
    if (prev_yv != (coord_t)-1) {
      if (prev_yv < yv) {
        for (int y=prev_yv; y<=yv; y++) {
          lcdDrawBitmapPattern(x + i - 3, y, LBM_POINT, TEXT_COLOR);
        }
      }
      else {
        for (int y=yv; y<=prev_yv; y++) {
          lcdDrawBitmapPattern(x + i - 3, y, LBM_POINT, TEXT_COLOR);
        }
      }
    }
    else {
      lcdDrawBitmapPattern(x + i - 3, yv, LBM_POINT, TEXT_COLOR);
    }
    prev_yv = yv;
  }

  lcdDrawText(LCD_W/2, MENU_FOOTER_TOP, STR_MENUTORESET, MENU_TITLE_COLOR | CENTERED);
  return true;
}

bool menuStatsDebug(event_t event)
{
  switch(event) {
    case EVT_ENTRY:
    case EVT_ENTRY_UP:
      break;

    case EVT_KEY_FIRST(KEY_ENTER):
      maxMixerDuration  = 0;
#if defined(LUA)
      maxLuaInterval = 0;
      maxLuaDuration = 0;
#endif
      break;
  }

  if (!check_simple(event, e_StatsDebug, menuTabStats, DIM(menuTabStats), 1)) {
    disableVBatBridge();
    return false;
  }

  drawMenuTemplate("Debug", 0, STATS_ICONS, OPTION_MENU_TITLE_BAR);

  coord_t y = MENU_CONTENT_TOP;

  lcdDrawText(MENUS_MARGIN_LEFT, y, "Free Mem");
  lcdDrawNumber(MENU_STATS_COLUMN1, y, availableMemory(), LEFT, 0, NULL, "b");
  y += FH;

#if defined(LUA)
  lcdDrawText(MENUS_MARGIN_LEFT, y, "Lua scripts");
  lcdDrawText(MENU_STATS_COLUMN1, y+1, "[Duration]", HEADER_COLOR|SMLSIZE);
  lcdDrawNumber(lcdNextPos+5, y, 10*maxLuaDuration, LEFT, 0, NULL, "ms");
  lcdDrawText(lcdNextPos+20, y+1, "[Interval]", HEADER_COLOR|SMLSIZE);
  lcdDrawNumber(lcdNextPos+5, y, 10*maxLuaInterval, LEFT, 0, NULL, "ms");
  y += FH;

  // lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP+line*FH, "Lua memory");
  lcdDrawText(MENU_STATS_COLUMN1, y+1, "[S]", HEADER_COLOR|SMLSIZE);
  lcdDrawNumber(lcdNextPos+5, y, luaGetMemUsed(lsScripts), LEFT);
  lcdDrawText(lcdNextPos+20, y+1, "[W]", HEADER_COLOR|SMLSIZE);
  lcdDrawNumber(lcdNextPos+5, y, luaGetMemUsed(lsWidgets), LEFT);
  lcdDrawText(lcdNextPos+20, y+1, "[B]", HEADER_COLOR|SMLSIZE);
  lcdDrawNumber(lcdNextPos+5, y, luaExtraMemoryUsage, LEFT);
  y += FH;
#endif

  lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TMIXMAXMS);
  lcdDrawNumber(MENU_STATS_COLUMN1, y, DURATION_MS_PREC2(maxMixerDuration), PREC2|LEFT, 0, NULL, "ms");
  y += FH;

  lcdDrawText(MENUS_MARGIN_LEFT, y, STR_FREE_STACK);
  lcdDrawText(MENU_STATS_COLUMN1, y+1, "[Menus]", HEADER_COLOR|SMLSIZE);
  lcdDrawNumber(lcdNextPos+5, y, menusStack.available(), LEFT);
  lcdDrawText(lcdNextPos+20, y+1, "[Mix]", HEADER_COLOR|SMLSIZE);
  lcdDrawNumber(lcdNextPos+5, y, mixerStack.available(), LEFT);
  lcdDrawText(lcdNextPos+20, y+1, "[Audio]", HEADER_COLOR|SMLSIZE);
  lcdDrawNumber(lcdNextPos+5, y, audioStack.available(), LEFT);
  y += FH;

#if defined(DISK_CACHE) && defined(DEBUG)
  lcdDrawText(MENUS_MARGIN_LEFT, y, "SD cache hits");
  lcdDrawNumber(MENU_STATS_COLUMN1, y, diskCache.getHitRate(), PREC1|LEFT, 0, NULL, "%");
  y += FH;
#endif

#if defined(DEBUG_LATENCY)
  lcdDrawText(MENUS_MARGIN_LEFT, y, "Heartbeat");
  if (heartbeatCapture.valid)
    lcdDrawNumber(MENU_STATS_COLUMN1, y, heartbeatCapture.count, LEFT);
  else
    lcdDrawText(MENU_STATS_COLUMN1, y, "---");
  y += FH;
#endif

#if defined(DEBUG)
  lcdDrawText(MENUS_MARGIN_LEFT, y, "Telem RX Errs");
  lcdDrawNumber(MENU_STATS_COLUMN1, y, telemetryErrors, LEFT);
  y += FH;
#endif

#if defined(INTERNAL_GPS)
  lcdDrawText(MENUS_MARGIN_LEFT, y, "Internal GPS");
  lcdDrawText(MENU_STATS_COLUMN1, y+1, "[Fix]", HEADER_COLOR|SMLSIZE);
  lcdDrawText(lcdNextPos+2, y, (gpsData.fix ? "Yes" : "No"), LEFT);
  lcdDrawText(lcdNextPos+20, y+1, "[Sats]", HEADER_COLOR|SMLSIZE);
  lcdDrawNumber(lcdNextPos+5, y, gpsData.numSat, LEFT);
  lcdDrawText(lcdNextPos+20, y+1, "[Hdop]", HEADER_COLOR|SMLSIZE);
  lcdDrawNumber(lcdNextPos+5, y, gpsData.hdop, PREC2|LEFT);
#endif

  lcdDrawText(LCD_W/2, MENU_FOOTER_TOP, STR_MENUTORESET, MENU_TITLE_COLOR | CENTERED);
  return true;
}

#if defined(DEBUG_TRACE_BUFFER)
#define STATS_TRACES_INDEX_POS         MENUS_MARGIN_LEFT
#define STATS_TRACES_TIME_POS          MENUS_MARGIN_LEFT + 4*10
#define STATS_TRACES_EVENT_POS         MENUS_MARGIN_LEFT + 14*10
#define STATS_TRACES_DATA_POS          MENUS_MARGIN_LEFT + 20*10

bool menuStatsTraces(event_t event)
{
  switch(event)
  {
    case EVT_KEY_LONG(KEY_ENTER):
      dumpTraceBuffer();
      killEvents(event);
      break;
  }

  SIMPLE_MENU("", STATS_ICONS, menuTabStats, e_StatsTraces, TRACE_BUFFER_LEN);

  uint8_t k = 0;
  int8_t sub = menuVerticalPosition;

  lcdDrawChar(STATS_TRACES_INDEX_POS, MENU_TITLE_TOP+2, '#', MENU_TITLE_COLOR);
  lcdDrawText(STATS_TRACES_TIME_POS, MENU_TITLE_TOP+2, "Time", MENU_TITLE_COLOR);
  lcdDrawText(STATS_TRACES_EVENT_POS, MENU_TITLE_TOP+2, "Event", MENU_TITLE_COLOR);
  lcdDrawText(STATS_TRACES_DATA_POS, MENU_TITLE_TOP+2, "Data", MENU_TITLE_COLOR);

  for (uint8_t i=0; i<NUM_BODY_LINES; i++) {
    coord_t y = MENU_CONTENT_TOP + i * FH;
    k = i+menuVerticalOffset;

    // item
    lcdDrawNumber(STATS_TRACES_INDEX_POS, y, k, LEFT | (sub==k ? INVERS : 0));

    const struct TraceElement * te = getTraceElement(k);
    if (te) {
      // time
      int32_t tme = te->time % SECS_PER_DAY;
      drawTimer(STATS_TRACES_TIME_POS, y, tme, TIMEHOUR|LEFT);
      // event
      lcdDrawNumber(STATS_TRACES_EVENT_POS, y, te->event, LEADING0|LEFT, 3);
      // data
      lcdDrawSizedText(STATS_TRACES_DATA_POS, y, "0x", 2);
      lcdDrawHexNumber(lcdNextPos, y, (uint16_t)(te->data >> 16));
      lcdDrawHexNumber(lcdNextPos, y, (uint16_t)(te->data & 0xFFFF));
    }

  }

  return true;
}
#endif // defined(DEBUG_TRACE_BUFFER)
