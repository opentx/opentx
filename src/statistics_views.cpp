/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
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

#include "open9x.h"

void menuProcStatistic(uint8_t event)
{
  TITLE(STR_MENUSTAT);
  switch(event)
  {
    case EVT_KEY_FIRST(KEY_UP):
      chainMenu(menuProcDebug);
      break;
    case EVT_KEY_FIRST(KEY_DOWN):
    case EVT_KEY_FIRST(KEY_EXIT):
      chainMenu(menuMainView);
      break;
  }

  lcd_puts(  1*FW, FH*1, STR_TM1TM2);
  putsTime(    5*FW, FH*1, s_timerVal[0], 0, 0);
  putsTime(   12*FW, FH*1, s_timerVal[1], 0, 0);

  lcd_puts(  1*FW, FH*2, STR_THRTHP);
  putsTime(    5*FW, FH*2, s_timeCumThr, 0, 0);
  putsTime(   12*FW, FH*2, s_timeCum16ThrP/16, 0, 0);

  lcd_puts( 17*FW, FH*0, STR_TOT);
  putsTime(   12*FW, FH*0, s_timeCumTot, 0, 0);

  uint8_t traceRd = (s_traceCnt < 0 ? s_traceWr : 0);
  const uint8_t x=5;
  const uint8_t y=60;
  lcd_hline(x-3,y,120+3+3);
  lcd_vline(x,y-32,32+3);

  for(uint8_t i=0; i<120; i+=6)
  {
    lcd_vline(x+i+6,y-1,3);
  }
  for(uint8_t i=1; i<=120; i++)
  {
    lcd_vline(x+i,y-s_traceBuf[traceRd],s_traceBuf[traceRd]);
    traceRd++;
    if(traceRd>=MAXTRACE) traceRd=0;
    if(traceRd==s_traceWr) break;
  }
}

#define MENU_DEBUG_COL_OFS (14*FW)
void menuProcDebug(uint8_t event)
{
  TITLE(STR_MENUDEBUG);
  switch(event)
  {
    case EVT_KEY_FIRST(KEY_MENU):
#if !defined(PCBARM)
      g_tmr1Latency_min = 0xff;
      g_tmr1Latency_max = 0;
#endif
      g_timeMainMax    = 0;
      AUDIO_KEYPAD_UP();
      break;
    case EVT_KEY_FIRST(KEY_DOWN):
      chainMenu(menuProcStatistic);
      break;
    case EVT_KEY_FIRST(KEY_UP):
    case EVT_KEY_FIRST(KEY_EXIT):
      chainMenu(menuMainView);
      break;
  }

#if !defined(PCBARM)
  lcd_putsLeft(1*FH, STR_TMR1LATMAXUS);
  lcd_outdez8(MENU_DEBUG_COL_OFS , 1*FH, g_tmr1Latency_max/2 );
  lcd_putsLeft(2*FH, STR_TMR1LATMINUS);
  lcd_outdez8(MENU_DEBUG_COL_OFS , 2*FH, g_tmr1Latency_min/2 );
  lcd_putsLeft(3*FH, STR_TMR1JITTERUS);
  lcd_outdez8(MENU_DEBUG_COL_OFS , 3*FH, (g_tmr1Latency_max - g_tmr1Latency_min) /2 );
#endif

  lcd_putsLeft(4*FH, STR_TMAINMAXMS);
#if defined(PCBARM)
  lcd_outdezAtt(MENU_DEBUG_COL_OFS, 4*FH, (g_timeMainMax)/20, PREC2);
#else
  lcd_outdezAtt(MENU_DEBUG_COL_OFS, 4*FH, (g_timeMainMax*100)/16, PREC2);
#endif

#if defined(PCBARM)
#if defined(REVB)
  lcd_putsLeft(2*FH, STR_CURRENT);
  putsTelemetryValue(MENU_DEBUG_COL_OFS, 2*FH, getCurrent(), UNIT_MILLIAMPS, 0) ;
#endif

  lcd_putsLeft(3*FH, PSTR("CPU temp.\010>"));
  putsTelemetryValue(MENU_DEBUG_COL_OFS, 3*FH, (((((int32_t)Temperature - 838 ) * 621 ) >> 11 ) - 20), UNIT_DEGREES, 0 ) ;
  putsTelemetryValue(20*FW+2, 3*FH, (((((int32_t)maxTemperature - 838 ) * 621 ) >> 11 ) - 20), UNIT_DEGREES, 0 ) ;
  // TODO mAh, Battery from ersky9x?
#endif

#if !defined(PCBARM)
  lcd_puts( 0*FW,  5*FH, STR_FREESTACKMINB);
  lcd_outdezAtt(14*FW,  5*FH, stack_free(), UNSIGN) ;
#endif

  lcd_puts( 3*FW,  7*FH, STR_MENUTORESET);
}
