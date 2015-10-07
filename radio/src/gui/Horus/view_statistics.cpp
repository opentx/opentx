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

void menuStatisticsView(evt_t event)
{
  drawMenuTemplate("Statistics", event);

  switch(event)
  {
    case EVT_KEY_FIRST(KEY_UP):
      chainMenu(menuStatisticsDebug);
      break;

    case EVT_KEY_LONG(KEY_MENU):
      g_eeGeneral.globalTimer = 0;
      eeDirty(EE_GENERAL);
      sessionTimer = 0;
      break;

    case EVT_KEY_FIRST(KEY_EXIT):
      chainMenu(menuMainView);
      break;
  }

  lcd_putsAtt(  10, MENU_CONTENT_TOP + FH*0, "\037\145TOT:\037\317BATT:", HEADER_COLOR);
  lcd_putsAtt(  10, MENU_CONTENT_TOP + FH*1, "TM1:\037\145TM2:", HEADER_COLOR);
  lcd_putsAtt(  10, MENU_CONTENT_TOP + FH*2, "THR:\037\145TH%:", HEADER_COLOR);

  putsTimer(    45, MENU_CONTENT_TOP + FH*1, timersStates[0].val);
  putsTimer(   140, MENU_CONTENT_TOP + FH*1, timersStates[1].val);
  putsTimer(    45, MENU_CONTENT_TOP + FH*2, s_timeCumThr);
  putsTimer(   140, MENU_CONTENT_TOP + FH*2, s_timeCum16ThrP/16);
  putsTimer(   140, MENU_CONTENT_TOP + FH*0, sessionTimer);
  putsTimer(   250, MENU_CONTENT_TOP + 0*FH, g_eeGeneral.globalTimer+sessionTimer, TIMEHOUR);

#if defined(THRTRACE)
  coord_t traceRd = (s_traceCnt < 0 ? s_traceWr : 0);
  const coord_t x = 4;
  const coord_t y = 200;
  lcd_hlineStip(x-3, y, MAXTRACE+3+3, SOLID, TEXT_COLOR);
  lcd_vlineStip(x, y-96, 96+3, SOLID, TEXT_COLOR);

  for (coord_t i=0; i<MAXTRACE; i+=6) {
    lcd_vlineStip(x+i+6, y-1, 3, SOLID, TEXT_COLOR);
  }
  for (coord_t i=1; i<=MAXTRACE; i++) {
    lcd_vlineStip(x+i, y-3*s_traceBuf[traceRd], 3*s_traceBuf[traceRd], SOLID, TEXT_COLOR);
    traceRd++;
    if (traceRd>=MAXTRACE) traceRd = 0;
    if (traceRd==s_traceWr) break;
  }
#endif
}

#define MENU_DEBUG_COL1_OFS   (11*10-2)
#define MENU_DEBUG_Y_MIXMAX   (MENU_CONTENT_TOP + 2*FH)
#define MENU_DEBUG_Y_LUA      (MENU_CONTENT_TOP + 3*FH)
#define MENU_DEBUG_Y_FREE_RAM (MENU_CONTENT_TOP + 4*FH)
#define MENU_DEBUG_Y_STACK    (MENU_CONTENT_TOP + 5*FH)
#define MENU_DEBUG_Y_RTOS     (MENU_CONTENT_TOP + 6*FH)

void menuStatisticsDebug(evt_t event)
{
  drawMenuTemplate(STR_MENUDEBUG, event);

  switch(event)
  {
    case EVT_KEY_LONG(KEY_ENTER):
      g_eeGeneral.mAhUsed = 0;
      g_eeGeneral.globalTimer = 0;
      eeDirty(EE_GENERAL);
      sessionTimer = 0;
      killEvents(event);
      AUDIO_KEYPAD_UP();
      break;
    case EVT_KEY_FIRST(KEY_ENTER):
#if defined(LUA)
      maxLuaInterval = 0;
      maxLuaDuration = 0;
#endif
      maxMixerDuration  = 0;
      AUDIO_KEYPAD_UP();
      break;

#if defined(DEBUG_TRACE_BUFFER)
    case EVT_KEY_FIRST(KEY_UP):
      pushMenu(menuTraceBuffer);
      return;
#endif

    case EVT_KEY_FIRST(KEY_DOWN):
      chainMenu(menuStatisticsView);
      break;
    case EVT_KEY_FIRST(KEY_EXIT):
      chainMenu(menuMainView);
      break;
  }

  lcd_putsLeft(MENU_DEBUG_Y_FREE_RAM, "Free Mem");
  lcd_outdezAtt(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_FREE_RAM, availableMemory(), LEFT, "b");

#if defined(LUA)
  lcd_putsLeft(MENU_DEBUG_Y_LUA, "Lua scripts");
  lcd_putsAtt(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_LUA+1, "[Duration]", HEADER_COLOR|SMLSIZE);
  lcd_outdezAtt(MENU_DEBUG_COL1_OFS+30, MENU_DEBUG_Y_LUA, 10*maxLuaDuration, LEFT);
  lcd_putsAtt(MENU_DEBUG_COL1_OFS+60, MENU_DEBUG_Y_LUA+1, "[Interval]", HEADER_COLOR|SMLSIZE);
  lcd_outdezAtt(MENU_DEBUG_COL1_OFS+90, MENU_DEBUG_Y_LUA, 10*maxLuaInterval, LEFT);
#endif

  lcd_putsLeft(MENU_DEBUG_Y_MIXMAX, STR_TMIXMAXMS);
  lcd_outdezAtt(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_MIXMAX, DURATION_MS_PREC2(maxMixerDuration), PREC2|LEFT, "ms");

  lcd_putsLeft(MENU_DEBUG_Y_RTOS, STR_FREESTACKMINB);
  lcd_putsAtt(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_RTOS+1, "[Menus]", HEADER_COLOR|SMLSIZE);
  lcd_outdezAtt(MENU_DEBUG_COL1_OFS+30, MENU_DEBUG_Y_RTOS, menusStack.available(), LEFT);
  lcd_putsAtt(MENU_DEBUG_COL1_OFS+60, MENU_DEBUG_Y_RTOS+1, "[Mix]", HEADER_COLOR|SMLSIZE);
  lcd_outdezAtt(MENU_DEBUG_COL1_OFS+90, MENU_DEBUG_Y_RTOS, mixerStack.available(), LEFT);
  lcd_putsAtt(MENU_DEBUG_COL1_OFS+120, MENU_DEBUG_Y_RTOS+1, "[Audio]", HEADER_COLOR|SMLSIZE);
  lcd_outdezAtt(MENU_DEBUG_COL1_OFS+150, MENU_DEBUG_Y_RTOS, audioStack.available(), LEFT);

  lcd_putsCenter(7*FH+1, STR_MENUTORESET);
  // lcd_status_line();
}


#if defined(DEBUG_TRACE_BUFFER)
#include "stamp-opentx.h"

void menuTraceBuffer(evt_t event)
{
  switch(event)
  {
    case EVT_KEY_LONG(KEY_ENTER):
      dumpTraceBuffer();
      killEvents(event);
      break;
  }

  SIMPLE_SUBMENU("Trace Buffer " VERS_STR, TRACE_BUFFER_LEN);
  /* RTC time */
  struct gtm t;
  gettime(&t);
  putsTime(LCD_W+1, 0, t, TIMEBLINK);

  uint8_t y = 0;
  uint8_t k = 0;
  int8_t sub = m_posVert;

  lcd_putc(0, FH, '#');
  lcd_puts(4*10, FH, "Time");
  lcd_puts(14*10, FH, "Event");
  lcd_puts(20*10, FH, "Data");

  for (uint8_t i=0; i<LCD_LINES-2; i++) {
    y = 1 + (i+2)*FH;
    k = i+s_pgOfs;

    //item
    lcd_outdezAtt(0, y, k, LEFT | (sub==k ? INVERS : 0));

    const struct TraceElement * te = getTraceElement(k);
    if (te) {
      //time
      putstime_t tme = te->time % SECS_PER_DAY;
      putsTimer(4*10, y, tme, TIMEHOUR|LEFT);
      //event
      lcd_outdezNAtt(14*10, y, te->event, LEADING0|LEFT, 3);
      //data
      lcd_putsn  (20*10, y, "0x", 2);
      lcd_outhex4(22*10-2, y, (uint16_t)(te->data >> 16));
      lcd_outhex4(25*10, y, (uint16_t)(te->data & 0xFFFF));
    }

  }


}
#endif //#if defined(DEBUG_TRACE_BUFFER)
