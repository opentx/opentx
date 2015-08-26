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
#include "../../timers.h"

void menuStatisticsView(uint8_t event)
{
  TITLE(STR_MENUSTAT);

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

  lcd_puts(  1*FW, FH*0, STR_TOTTM1TM2THRTHP);
  putsTimer(    5*FW+5*FWNUM+1, FH*1, timersStates[0].val, 0, 0);
  putsTimer(   12*FW+5*FWNUM+1, FH*1, timersStates[1].val, 0, 0);

  putsTimer(    5*FW+5*FWNUM+1, FH*2, s_timeCumThr, 0, 0);
  putsTimer(   12*FW+5*FWNUM+1, FH*2, s_timeCum16ThrP/16, 0, 0);

  putsTimer(   12*FW+5*FWNUM+1, FH*0, sessionTimer, 0, 0);
  
  lcd_puts(    24*FW, FH*0, "TOT");
  putsTimer(   28*FW+5*FWNUM+1, FH*0, g_eeGeneral.globalTimer + sessionTimer, TIMEHOUR, 0);
  
  if (TIMERS == 3) {
    lcd_puts(    24*FW, FH*1, "TM3");
    putsTimer(   28*FW+5*FWNUM+1, FH*1, timersStates[2].val, TIMEHOUR, 0);
  }

#if defined(THRTRACE)
  coord_t traceRd = (s_traceCnt < 0 ? s_traceWr : 0);
  const coord_t x = 5;
  const coord_t y = 60;
  lcd_hline(x-3, y, MAXTRACE+3+3);
  lcd_vline(x, y-32, 32+3);

  for (coord_t i=0; i<MAXTRACE; i+=6) {
    lcd_vline(x+i+6, y-1, 3);
  }
  for (coord_t i=1; i<=MAXTRACE; i++) {
    lcd_vline(x+i, y-s_traceBuf[traceRd], s_traceBuf[traceRd]);
    traceRd++;
    if (traceRd>=MAXTRACE) traceRd = 0;
    if (traceRd==s_traceWr) break;
  }
#endif
}

#define MENU_DEBUG_COL1_OFS   (11*FW-2)
#define MENU_DEBUG_Y_MIXMAX   (2*FH-3)
#define MENU_DEBUG_Y_LUA      (3*FH-2)
#define MENU_DEBUG_Y_FREE_RAM (4*FH-1)
#define MENU_DEBUG_Y_STACK    (5*FH)
#define MENU_DEBUG_Y_RTOS     (6*FH)

void menuStatisticsDebug(uint8_t event)
{
  TITLE(STR_MENUDEBUG);

#if defined(WATCHDOG_TEST)
  if (s_warning_result) {
    s_warning_result = 0;
    // do a user requested watchdog test
    TRACE("Performing watchdog test");
    pausePulses();
  }
#endif

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
#if defined(WATCHDOG_TEST)
    case EVT_KEY_LONG(KEY_MENU):
      {
        POPUP_CONFIRMATION("Test the watchdog?");
        const char * w = "The radio will reset!";
        SET_WARNING_INFO(w, strlen(w), 0);
      }
      break;
#endif
  }

  lcd_putsLeft(MENU_DEBUG_Y_FREE_RAM, "Free Mem");
  lcd_outdezAtt(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_FREE_RAM, getAvailableMemory(), LEFT);
  lcd_puts(lcdLastPos, MENU_DEBUG_Y_FREE_RAM, "b");

#if defined(LUA)
  lcd_putsLeft(MENU_DEBUG_Y_LUA, "Lua scripts");
  lcd_putsAtt(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_LUA+1, "[Duration]", SMLSIZE);
  lcd_outdezAtt(lcdLastPos, MENU_DEBUG_Y_LUA, 10*maxLuaDuration, LEFT);
  lcd_putsAtt(lcdLastPos+2, MENU_DEBUG_Y_LUA+1, "[Interval]", SMLSIZE);
  lcd_outdezAtt(lcdLastPos, MENU_DEBUG_Y_LUA, 10*maxLuaInterval, LEFT);
#endif

  lcd_putsLeft(MENU_DEBUG_Y_MIXMAX, STR_TMIXMAXMS);
  lcd_outdezAtt(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_MIXMAX, DURATION_MS_PREC2(maxMixerDuration), PREC2|LEFT);
  lcd_puts(lcdLastPos, MENU_DEBUG_Y_MIXMAX, "ms");

  lcd_putsLeft(MENU_DEBUG_Y_RTOS, STR_FREESTACKMINB);
  lcd_putsAtt(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_RTOS+1, "[M]", SMLSIZE);
  lcd_outdezAtt(lcdLastPos, MENU_DEBUG_Y_RTOS, stack_free(0), UNSIGN|LEFT);
  lcd_putsAtt(lcdLastPos+2, MENU_DEBUG_Y_RTOS+1, "[X]", SMLSIZE);
  lcd_outdezAtt(lcdLastPos, MENU_DEBUG_Y_RTOS, stack_free(1), UNSIGN|LEFT);
  lcd_putsAtt(lcdLastPos+2, MENU_DEBUG_Y_RTOS+1, "[A]", SMLSIZE);
  lcd_outdezAtt(lcdLastPos, MENU_DEBUG_Y_RTOS, stack_free(2), UNSIGN|LEFT);
  lcd_putsAtt(lcdLastPos+2, MENU_DEBUG_Y_RTOS+1, "[I]", SMLSIZE);
  lcd_outdezAtt(lcdLastPos, MENU_DEBUG_Y_RTOS, stack_free(255), UNSIGN|LEFT);

  lcd_puts(3*FW, 7*FH+1, STR_MENUTORESET);
  lcd_status_line();
}


#if defined(DEBUG_TRACE_BUFFER)
#include "stamp-opentx.h"

void menuTraceBuffer(uint8_t event)
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
  lcd_puts(4*FW, FH, "Time");
  lcd_puts(14*FW, FH, "Event");
  lcd_puts(20*FW, FH, "Data");

  for (uint8_t i=0; i<LCD_LINES-2; i++) {
    y = 1 + (i+2)*FH;
    k = i+s_pgOfs;

    //item
    lcd_outdezAtt(0, y, k, LEFT | (sub==k ? INVERS : 0));

    const struct TraceElement * te = getTraceElement(k);
    if (te) {
      //time
      putstime_t tme = te->time % SECS_PER_DAY;
      putsTimer(4*FW, y, tme, TIMEHOUR|LEFT, TIMEHOUR|LEFT);
      //event
      lcd_outdezNAtt(14*FW, y, te->event, LEADING0|LEFT, 3);
      //data
      lcd_putsn  (20*FW, y, "0x", 2);
      lcd_outhex4(22*FW-2, y, (uint16_t)(te->data >> 16));
      lcd_outhex4(25*FW, y, (uint16_t)(te->data & 0xFFFF));
    }

  }


}
#endif //#if defined(DEBUG_TRACE_BUFFER)
