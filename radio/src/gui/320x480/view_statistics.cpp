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

#include "view_statistics.h"
#include "opentx.h"
#include "stamp.h"
#include "libopenui.h"

#define MENU_STATS_COLUMN1    (MENUS_MARGIN_LEFT + 120)

class StatisticsBody : public Window {
  public:
    StatisticsBody(Window * parent, const rect_t &rect) :
      Window(parent, rect)
    {
      auto reset = new TextButton(this, {10, 320, LCD_W - 20, lineHeight}, "Push to reset");
      reset->setPressHandler([=]() {
        g_eeGeneral.globalTimer = 0;
        storageDirty(EE_GENERAL);
        sessionTimer = 0;
        return 0;
      });
    }

#if defined(TRACE_WINDOWS_ENABLED)
    std::string getName() override
    {
      return "StatisticsBody";
    }
#endif

    void checkEvents() override
    {
      if (get_tmr10ms() - lastRefresh > 100) {
        invalidate();
        lastRefresh = get_tmr10ms();
      }
    }

    void paint(BitmapBuffer * dc) override
    {
      lcdDrawText(MENUS_MARGIN_LEFT, 0, "Session");
      drawTimer(MENU_STATS_COLUMN1, 0, sessionTimer, TIMEHOUR);
      lcdDrawText(MENUS_MARGIN_LEFT, FH, "Battery");
      drawTimer(MENU_STATS_COLUMN1, FH, g_eeGeneral.globalTimer + sessionTimer, TIMEHOUR);

      lcdDrawText(MENUS_MARGIN_LEFT, 2 * FH, "Throttle");
      drawTimer(MENU_STATS_COLUMN1, 2 * FH, s_timeCumThr, TIMEHOUR);
      lcdDrawText(MENUS_MARGIN_LEFT, 3 * FH, "Throttle %", TIMEHOUR);
      drawTimer(MENU_STATS_COLUMN1, 3 * FH, s_timeCum16ThrP / 16, TIMEHOUR);

      for (uint32_t idx = 0; idx < TIMERS; idx++) {
        drawStringWithIndex(MENUS_MARGIN_LEFT, (4 + idx) * FH, "Timer", idx);
        drawTimer(MENU_STATS_COLUMN1, (4 + idx) * FH, timersStates[idx].val, TIMEHOUR);
      }

      const coord_t x = 10;
      const coord_t y = 270;
      lcdDrawHorizontalLine(x - 3, y, MAXTRACE + 3 + 3, SOLID, TEXT_COLOR);
      lcdDrawVerticalLine(x, y - 96, 96 + 3, SOLID, TEXT_COLOR);
      for (coord_t i = 0; i < MAXTRACE; i += 6) {
        lcdDrawVerticalLine(x + i, y - 1, 3, SOLID, TEXT_COLOR);
      }

      uint16_t traceRd = s_traceWr > MAXTRACE ? s_traceWr - MAXTRACE : 0;
      coord_t prev_yv = (coord_t) -1;
      for (coord_t i = 1; i <= MAXTRACE && traceRd < s_traceWr; i++, traceRd++) {
        uint8_t h = s_traceBuf[traceRd % MAXTRACE];
        coord_t yv = y - 2 - 3 * h;
        if (prev_yv != (coord_t) -1) {
          if (prev_yv < yv) {
            for (int y = prev_yv; y <= yv; y++) {
              lcdDrawBitmapPattern(x + i - 3, y, LBM_POINT, TEXT_COLOR);
            }
          }
          else {
            for (int y = yv; y <= prev_yv; y++) {
              lcdDrawBitmapPattern(x + i - 3, y, LBM_POINT, TEXT_COLOR);
            }
          }
        }
        else {
          lcdDrawBitmapPattern(x + i - 3, yv, LBM_POINT, TEXT_COLOR);
        }
        prev_yv = yv;
      }
    }

  protected:
    tmr10ms_t lastRefresh = 0;
};

class StatisticsFooter : public Window {
  public:
    StatisticsFooter(Window * parent, const rect_t &rect) :
      Window(parent, rect)
    {
    }

    void paint(BitmapBuffer * dc) override
    {

    }
};

class StatisticsPage : public PageTab {
  public:
    StatisticsPage() :
      PageTab(STR_STATISTICS, ICON_STATS_THROTTLE_GRAPH)
    {
    }

    void build(Window * window) override
    {
      new StatisticsBody(window, {0, 0, LCD_W, window->height() - footerHeight});
      new StatisticsFooter(window, {0, window->height() - footerHeight, LCD_W, footerHeight});
    }

  protected:
    static constexpr coord_t footerHeight = 30;
};


class AnalogsBody : public Window {
  public:
    AnalogsBody(Window * parent, const rect_t &rect) :
      Window(parent, rect)
    {
      setInnerHeight(100); // TODO
    }

    void checkEvents() override
    {
      // Perma refresh this page
      invalidate();
    }

    void paint(BitmapBuffer * dc) override
    {
      for (uint8_t i = 0; i < NUM_ANALOGS; i++) {
        coord_t y = MENU_CONTENT_TOP + (i / 2) * FH;
        coord_t x = MENUS_MARGIN_LEFT + (i & 1 ? LCD_W / 2 : 0);
        lcdDrawNumber(x, y, i + 1, LEADING0 | LEFT, 2, NULL, ":");
        lcdDrawHexNumber(x + 40, y, anaIn(i));
        if (i < NUM_STICKS + NUM_POTS + NUM_SLIDERS) {
          lcdDrawNumber(x + 100, y, (int16_t) calibratedAnalogs[CONVERT_MODE(i)] * 25 / 256);
        }
      }
    }

  protected:
};

class AnalogsFooter : public Window {
  public:
    AnalogsFooter(Window * parent, const rect_t &rect) :
      Window(parent, rect)
    {
    }

    void paint(BitmapBuffer * dc) override
    {

    }
};

class AnalogsPage : public PageTab {
  public:
    AnalogsPage() :
      PageTab("Analogs", ICON_STATS_ANALOGS)
    {
    }

    void build(Window * window) override
    {
      new AnalogsBody(window, {0, 0, LCD_W, window->height() - footerHeight});
      new AnalogsFooter(window, {0, window->height() - footerHeight, LCD_W, footerHeight});
    }

  protected:
    static constexpr coord_t footerHeight = 30;
};

class DebugBody : public Window {
  public:
    DebugBody(Window * parent, const rect_t &rect) :
      Window(parent, rect)
    {
      setInnerHeight(100); // TODO
    }

    void checkEvents() override
    {
      // Perma refresh this page
      invalidate();
    }

    void paint(BitmapBuffer * dc) override
    {
      lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP, "Free Mem");
      lcdDrawNumber(MENU_STATS_COLUMN1, MENU_CONTENT_TOP, availableMemory(), LEFT, 0, NULL, "b");

      lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + FH, STR_TMIXMAXMS);
      lcdDrawNumber(MENU_STATS_COLUMN1, MENU_CONTENT_TOP + FH, DURATION_MS_PREC2(maxMixerDuration), PREC2 | LEFT, 0, NULL, "ms");

      lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 2 * FH, STR_FREESTACKMINB);
      lcdDrawText(MENU_STATS_COLUMN1, MENU_CONTENT_TOP + 2 * FH + 1, "[Menus]", HEADER_COLOR | SMLSIZE);
      lcdDrawNumber(lcdNextPos + 5, MENU_CONTENT_TOP + 2 * FH, menusStack.available(), LEFT);
      lcdDrawText(MENU_STATS_COLUMN1, MENU_CONTENT_TOP + 3 * FH + 1, "[Mix]", HEADER_COLOR | SMLSIZE);
      lcdDrawNumber(lcdNextPos + 5, MENU_CONTENT_TOP + 3 * FH, mixerStack.available(), LEFT);
      lcdDrawText(MENU_STATS_COLUMN1, MENU_CONTENT_TOP + 4 * FH + 1, "[Audio]", HEADER_COLOR | SMLSIZE);
      lcdDrawNumber(lcdNextPos + 5, MENU_CONTENT_TOP + 4 * FH, audioStack.available(), LEFT);

      int line = 5;

#if defined(DISK_CACHE)
      lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + line * FH, "SD cache hits");
      lcdDrawNumber(MENU_STATS_COLUMN1, MENU_CONTENT_TOP + line * FH, diskCache.getHitRate(), PREC1 | LEFT, 0, NULL, "%");
      ++line;
#endif

#if defined(LUA)
      lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP+line*FH, "Lua duration");
      lcdDrawNumber(MENU_STATS_COLUMN1, MENU_CONTENT_TOP+line*FH, 10*maxLuaDuration, LEFT, 0, NULL, "ms");
      ++line;

      lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP+line*FH, "Lua interval");
      lcdDrawNumber(MENU_STATS_COLUMN1, MENU_CONTENT_TOP+line*FH, 10*maxLuaInterval, LEFT, 0, NULL, "ms");
      ++line;

      lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP+line*FH, "Lua memory");
      lcdDrawText(MENU_STATS_COLUMN1, MENU_CONTENT_TOP+line*FH+1, "[S]", HEADER_COLOR|SMLSIZE);
      lcdDrawNumber(lcdNextPos+5, MENU_CONTENT_TOP+line*FH, luaGetMemUsed(lsScripts), LEFT);
      lcdDrawText(lcdNextPos+20, MENU_CONTENT_TOP+line*FH+1, "[W]", HEADER_COLOR|SMLSIZE);
      lcdDrawNumber(lcdNextPos+5, MENU_CONTENT_TOP+line*FH, luaGetMemUsed(lsWidgets), LEFT);
      lcdDrawText(lcdNextPos+20, MENU_CONTENT_TOP+line*FH+1, "[B]", HEADER_COLOR|SMLSIZE);
      lcdDrawNumber(lcdNextPos+5, MENU_CONTENT_TOP+line*FH, luaExtraMemoryUsage, LEFT);
      ++line;
#endif

      lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + line * FH, "Tlm RX Errs");
      lcdDrawNumber(MENU_STATS_COLUMN1, MENU_CONTENT_TOP + line * FH, telemetryErrors, LEFT);

      lcdDrawText(LCD_W / 2, MENU_FOOTER_TOP, STR_MENUTORESET, MENU_TITLE_COLOR | CENTERED);
    }

  protected:
};

class DebugFooter : public Window {
  public:
    DebugFooter(Window * parent, const rect_t &rect) :
      Window(parent, rect)
    {
    }

    void paint(BitmapBuffer * dc) override
    {

    }
};

class DebugPage : public PageTab {
  public:
    DebugPage() :
      PageTab("Debug", ICON_STATS_DEBUG)
    {
    }

    void build(Window * window) override
    {
      new DebugBody(window, {0, 0, LCD_W, window->height() - footerHeight});
      new DebugFooter(window, {0, window->height() - footerHeight, LCD_W, footerHeight});
    }

  protected:
    static constexpr coord_t footerHeight = 30;
};

StatisticsMenu::StatisticsMenu() :
  TabsGroup()
{
  addTab(new StatisticsPage());
  addTab(new DebugPage());
  addTab(new AnalogsPage());
}

#if 0
#define MENU_STATS_COLUMN1    (MENUS_MARGIN_LEFT + 120)
#define MENU_STATS_COLUMN2    (LCD_W/2)
#define MENU_STATS_COLUMN3    (LCD_W/2 + 120)

bool menuStatsGraph(event_t event)
{
  switch(event) {
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
  switch(event)
  {
    case EVT_KEY_FIRST(KEY_ENTER):
      maxMixerDuration  = 0;
#if defined(LUA)
      maxLuaInterval = 0;
      maxLuaDuration = 0;
#endif
      break;
  }

  SIMPLE_MENU("Debug", STATS_ICONS, menuTabStats, e_StatsDebug, 1);

  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP, "Free Mem");
  lcdDrawNumber(MENU_STATS_COLUMN1, MENU_CONTENT_TOP, availableMemory(), LEFT, 0, NULL, "b");

  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP+FH, STR_TMIXMAXMS);
  lcdDrawNumber(MENU_STATS_COLUMN1, MENU_CONTENT_TOP+FH, DURATION_MS_PREC2(maxMixerDuration), PREC2|LEFT, 0, NULL, "ms");

  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP+2*FH, STR_FREESTACKMINB);
  lcdDrawText(MENU_STATS_COLUMN1, MENU_CONTENT_TOP+2*FH+1, "[Menus]", HEADER_COLOR|SMLSIZE);
  lcdDrawNumber(lcdNextPos+5, MENU_CONTENT_TOP+2*FH, menusStack.available(), LEFT);
  lcdDrawText(lcdNextPos+20, MENU_CONTENT_TOP+2*FH+1, "[Mix]", HEADER_COLOR|SMLSIZE);
  lcdDrawNumber(lcdNextPos+5, MENU_CONTENT_TOP+2*FH, mixerStack.available(), LEFT);
  lcdDrawText(lcdNextPos+20, MENU_CONTENT_TOP+2*FH+1, "[Audio]", HEADER_COLOR|SMLSIZE);
  lcdDrawNumber(lcdNextPos+5, MENU_CONTENT_TOP+2*FH, audioStack.available(), LEFT);

  int line = 3;

#if defined(DISK_CACHE)
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP+line*FH, "SD cache hits");
  lcdDrawNumber(MENU_STATS_COLUMN1, MENU_CONTENT_TOP+line*FH, diskCache.getHitRate(), PREC1|LEFT, 0, NULL, "%");
  ++line;
#endif

#if defined(LUA)
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP+line*FH, "Lua duration");
  lcdDrawNumber(MENU_STATS_COLUMN1, MENU_CONTENT_TOP+line*FH, 10*maxLuaDuration, LEFT, 0, NULL, "ms");
  ++line;

  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP+line*FH, "Lua interval");
  lcdDrawNumber(MENU_STATS_COLUMN1, MENU_CONTENT_TOP+line*FH, 10*maxLuaInterval, LEFT, 0, NULL, "ms");
  ++line;

  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP+line*FH, "Lua memory");
  lcdDrawText(MENU_STATS_COLUMN1, MENU_CONTENT_TOP+line*FH+1, "[S]", HEADER_COLOR|SMLSIZE);
  lcdDrawNumber(lcdNextPos+5, MENU_CONTENT_TOP+line*FH, luaGetMemUsed(lsScripts), LEFT);
  lcdDrawText(lcdNextPos+20, MENU_CONTENT_TOP+line*FH+1, "[W]", HEADER_COLOR|SMLSIZE);
  lcdDrawNumber(lcdNextPos+5, MENU_CONTENT_TOP+line*FH, luaGetMemUsed(lsWidgets), LEFT);
  lcdDrawText(lcdNextPos+20, MENU_CONTENT_TOP+line*FH+1, "[B]", HEADER_COLOR|SMLSIZE);
  lcdDrawNumber(lcdNextPos+5, MENU_CONTENT_TOP+line*FH, luaExtraMemoryUsage, LEFT);
  ++line;
#endif

  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP+line*FH, "Tlm RX Errs");
  lcdDrawNumber(MENU_STATS_COLUMN1, MENU_CONTENT_TOP+line*FH, telemetryErrors, LEFT);

  lcdDrawText(LCD_W/2, MENU_FOOTER_TOP, STR_MENUTORESET, MENU_TITLE_COLOR | CENTERED);
  return true;
}

bool menuStatsAnalogs(event_t event)
{
  SIMPLE_MENU("Analogs", STATS_ICONS, menuTabStats, e_StatsAnalogs, 1);

  for (uint8_t i=0; i<NUM_ANALOGS; i++) {
    coord_t y = MENU_CONTENT_TOP + (i/2)*FH;
    coord_t x = MENUS_MARGIN_LEFT + (i & 1 ? LCD_W/2 : 0);
    lcdDrawNumber(x, y, i+1, LEADING0|LEFT, 2, NULL, ":");
    lcdDrawHexNumber(x+40, y, anaIn(i));
#if defined(JITTER_MEASURE)
    lcdDrawNumber(x+100, y, rawJitter[i].get());
    lcdDrawNumber(x+140, y, avgJitter[i].get());
    lcdDrawNumber(x+180, y, (int16_t)calibratedAnalogs[CONVERT_MODE(i)]*250/256, PREC1);
#else
    if (i < NUM_STICKS+NUM_POTS+NUM_SLIDERS)
      lcdDrawNumber(x+100, y, (int16_t)calibratedAnalogs[CONVERT_MODE(i)]*25/256);
#if defined(PCBHORUS)
    else if (i >= MOUSE1)
      lcdDrawNumber(x+100, y, (int16_t)calibratedAnalogs[CALIBRATED_MOUSE1+i-MOUSE1]*25/256);
#endif
#endif
  }

  // RAS
  if ((isModuleXJT(INTERNAL_MODULE) && IS_INTERNAL_MODULE_ON()) || (isModulePXX(EXTERNAL_MODULE) && !IS_INTERNAL_MODULE_ON())) {
    lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP+7*FH, "RAS");
    lcdDrawNumber(MENUS_MARGIN_LEFT+100, MENU_CONTENT_TOP+7*FH, telemetryData.swr.value);
    lcdDrawText(MENUS_MARGIN_LEFT + LCD_W/2, MENU_CONTENT_TOP+7*FH, "XJTVER");
    lcdDrawNumber(LCD_W/2 + MENUS_MARGIN_LEFT+100, MENU_CONTENT_TOP+7*FH, telemetryData.xjtVersion);
  }

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
      putstime_t tme = te->time % SECS_PER_DAY;
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
#endif
