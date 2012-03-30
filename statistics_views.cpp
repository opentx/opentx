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

uint16_t Current ;
uint32_t Current_sum ;
uint8_t  Current_count ;
void menuProcDebug(uint8_t event)
{
  TITLE(STR_MENUDEBUG);
  switch(event)
  {
    case EVT_KEY_FIRST(KEY_MENU):
      g_tmr1Latency_min = 0xff;
      g_tmr1Latency_max = 0;
      g_timeMain    = 0;
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
  lcd_putsLeft(1*FH, STR_TMR1LATMAXUS);
  lcd_outdez8(15*FW , 1*FH, g_tmr1Latency_max/2 );
  lcd_putsLeft(2*FH, STR_TMR1LATMINUS);
  lcd_outdez8(15*FW , 2*FH, g_tmr1Latency_min/2 );
  lcd_putsLeft(3*FH, STR_TMR1JITTERUS);
  lcd_outdez8(15*FW , 3*FH, (g_tmr1Latency_max - g_tmr1Latency_min) /2 );
  lcd_putsLeft(4*FH, STR_TMAINMAXMS);
#if defined(PCBARM)
  lcd_outdezAtt(15*FW, 4*FH, (g_timeMain)/20, PREC2);
#else
  lcd_outdezAtt(15*FW, 4*FH, (g_timeMain*100)/16, PREC2);
#endif

#if defined(PCBARM) && defined(REVB)
  // TODO then there are 2 means, is it needed?
  Current_sum += anaIn(NUMBER_ANALOG-1) ;
  if ( ++Current_count > 49 )
  {
    Current = Current_sum / 5 ;
    Current_sum = 0 ;
    Current_count = 0 ;
  }
  lcd_putsLeft(6*FH, STR_CURRENT);
  lcd_outhex4(10*FW+3, 6*FH, Current ) ;
  lcd_outdezAtt(18*FW, 6*FH, Current/22, 0 ) ;
#endif


#ifdef DEBUG
  lcd_puts( 0*FW,  5*FH, STR_T10MSUS);
  lcd_outdez8(15*FW , 5*FH, g_time_per10/2 );
#endif
#if !defined(SIMU) && !defined(PCBARM)
  lcd_puts( 0*FW,  6*FH, STR_FREESTACKMINB);
  lcd_outdezAtt(18*FW-1,  6*FH, stack_free(), UNSIGN) ;
#endif
  lcd_puts( 3*FW,  7*FH, STR_MENUTORESET);
}
