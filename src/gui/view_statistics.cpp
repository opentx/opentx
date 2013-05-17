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

#include "../opentx.h"

void menuStatisticsView(uint8_t event)
{
  TITLE(STR_MENUSTAT);

  switch(event)
  {
#if !defined(PCBTARANIS) || defined(DEBUG)
    case EVT_KEY_FIRST(KEY_UP):
      chainMenu(menuStatisticsDebug);
      return;
    case EVT_KEY_FIRST(KEY_DOWN):
#endif
    case EVT_KEY_FIRST(KEY_EXIT):
      chainMenu(menuMainView);
      return;
  }

  lcd_puts(  1*FW, FH*1, STR_TM1TM2);
  putsTime(    5*FW+5*FWNUM+1, FH*1, s_timerVal[0], 0, 0);
  putsTime(   12*FW+5*FWNUM+1, FH*1, s_timerVal[1], 0, 0);

  lcd_puts(  1*FW, FH*2, STR_THRTHP);
  putsTime(    5*FW+5*FWNUM+1, FH*2, s_timeCumThr, 0, 0);
  putsTime(   12*FW+5*FWNUM+1, FH*2, s_timeCum16ThrP/16, 0, 0);

  lcd_puts( 17*FW, FH*0, STR_TOT);
  putsTime(   12*FW+5*FWNUM+1, FH*0, s_timeCumTot, 0, 0);

#if defined(THRTRACE)
  uint8_t traceRd = (s_traceCnt < 0 ? s_traceWr : 0);
  const uint8_t x=5;
  const uint8_t y=60;
  lcd_hline(x-3,y,MAXTRACE+3+3);
  lcd_vline(x,y-32,32+3);

  for (uint8_t i=0; i<MAXTRACE; i+=6) {
    lcd_vline(x+i+6,y-1,3);
  }
  for (uint8_t i=1; i<=MAXTRACE; i++) {
    lcd_vline(x+i, y-s_traceBuf[traceRd], s_traceBuf[traceRd]);
    traceRd++;
    if (traceRd>=MAXTRACE) traceRd = 0;
    if (traceRd==s_traceWr) break;
  }
#endif
}

#if defined(CPUARM)
#define MENU_DEBUG_COL_OFS (13*FW)
#else
#define MENU_DEBUG_COL_OFS (14*FW)
#endif

void menuStatisticsDebug(uint8_t event)
{
  TITLE(STR_MENUDEBUG);

  switch(event)
  {
#if defined(CPUARM)
    case EVT_KEY_LONG(KEY_MENU):
      g_eeGeneral.mAhUsed = 0;
      g_eeGeneral.globalTimer = 0;
      eeDirty(EE_GENERAL);
#if defined(PCBSKY9X)
      Current_used = 0;
#endif
      sessionTimer = 0;
      killEvents(event);
      AUDIO_KEYPAD_UP();
      break;
#endif
    case EVT_KEY_FIRST(KEY_MENU):
#if !defined(CPUARM)
      g_tmr1Latency_min = 0xff;
      g_tmr1Latency_max = 0;
#endif
      maxMixerDuration  = 0;
      AUDIO_KEYPAD_UP();
      break;
    case EVT_KEY_FIRST(KEY_DOWN):
      chainMenu(menuStatisticsView);
      return;
    case EVT_KEY_FIRST(KEY_UP):
    case EVT_KEY_FIRST(KEY_EXIT):
      chainMenu(menuMainView);
      return;
  }

#if defined(PCBSKY9X)
  if ((ResetReason&RSTC_SR_RSTTYP) == (2<<8)) {
    lcd_puts(LCD_W-8*FW, 0*FH, "WATCHDOG");
  }
  else if (unexpectedShutdown) {
    lcd_puts(LCD_W-13*FW, 0*FH, "UNEXP.SHTDOWN");
  }
#endif

#if defined(PCBSKY9X) && !defined(REVA)
  lcd_putsLeft(1*FH, STR_CPU_CURRENT);
  putsTelemetryValue(MENU_DEBUG_COL_OFS, 1*FH, getCurrent(), UNIT_MILLIAMPS, 0);
  uint32_t current_scale = 488 + g_eeGeneral.currentCalib;
  putsTelemetryValue(20*FW+2, 1*FH, Current_max*10*current_scale/8192, UNIT_RAW, 0);

  lcd_putsLeft(2*FH, STR_CPU_MAH);
  putsTelemetryValue(MENU_DEBUG_COL_OFS, 2*FH, g_eeGeneral.mAhUsed + Current_used*current_scale/8192/36, UNIT_MAH, PREC1);
  putsTime(17*FW+5*FWNUM+1, 2*FH, g_eeGeneral.globalTimer + sessionTimer, 0, 0);
#endif

#if defined(PCBSKY9X)
  lcd_putsLeft(3*FH, STR_CPU_TEMP);
  putsTelemetryValue(MENU_DEBUG_COL_OFS, 3*FH, getTemperature(), UNIT_DEGREES, 0);
  putsTelemetryValue(20*FW+2, 3*FH, maxTemperature+g_eeGeneral.temperatureCalib, UNIT_DEGREES, 0);
#endif

#if defined(PCBSKY9X)
  lcd_putsLeft(4*FH, STR_COPROC_TEMP);

  if (Coproc_read==0) {
    lcd_putsAtt(9*FW+2, 4*FH, PSTR("Co Proc NACK"),INVERS);
  }
  else if (Coproc_read==0x81) {
    lcd_putsAtt(9*FW+2, 4*FH, PSTR("Inst.TinyApp"),INVERS);
  }
  else if (Coproc_read<3) {
    lcd_putsAtt(9*FW+2, 4*FH, PSTR("Upgr.TinyApp"),INVERS);
  }
  else {
    putsTelemetryValue(MENU_DEBUG_COL_OFS, 4*FH, Coproc_temp, UNIT_DEGREES, 0);
    putsTelemetryValue(20*FW+2, 4*FH, Coproc_maxtemp, UNIT_DEGREES, 0);
  }
#endif

#if defined(CPUARM)
  lcd_putsLeft(5*FH, STR_TMIXMAXMS);
  lcd_outdezAtt(MENU_DEBUG_COL_OFS, 5*FH, (maxMixerDuration)/20, PREC2);

  lcd_putsLeft(6*FH, STR_FREESTACKMINB);
  lcd_outdezAtt(13*FW, 6*FH, stack_free(0), UNSIGN);
  lcd_putc(13*FW, 6*FH, '/');
  lcd_outdezAtt(13*FW+FWNUM, 6*FH, stack_free(1), UNSIGN|LEFT);
  lcd_putc(lcdLastPos, 6*FH, '/');
  lcd_outdezAtt(lcdLastPos+FWNUM, 6*FH, stack_free(2), UNSIGN|LEFT);
#else
  lcd_putsLeft(1*FH, STR_TMR1LATMAXUS);
  lcd_outdez8(MENU_DEBUG_COL_OFS , 1*FH, g_tmr1Latency_max/2 );
  lcd_putsLeft(2*FH, STR_TMR1LATMINUS);
  lcd_outdez8(MENU_DEBUG_COL_OFS , 2*FH, g_tmr1Latency_min/2 );
  lcd_putsLeft(3*FH, STR_TMR1JITTERUS);
  lcd_outdez8(MENU_DEBUG_COL_OFS , 3*FH, (g_tmr1Latency_max - g_tmr1Latency_min) /2 );
  lcd_putsLeft(4*FH, STR_TMIXMAXMS);
  lcd_outdezAtt(MENU_DEBUG_COL_OFS, 4*FH, (maxMixerDuration*100)/16, PREC2);
  lcd_putsLeft(5*FH, STR_FREESTACKMINB);
  lcd_outdezAtt(14*FW, 5*FH, stack_free(), UNSIGN) ;
#endif

  lcd_puts(3*FW, 7*FH+1, STR_MENUTORESET);
  lcd_status_line();
}
