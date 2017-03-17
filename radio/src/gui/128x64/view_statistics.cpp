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

void menuStatisticsView(event_t event)
{
  TITLE(STR_MENUSTAT);

  switch (event) {
    case EVT_KEY_FIRST(KEY_UP):
      chainMenu(menuStatisticsDebug);
      break;

#if defined(CPUARM)
    case EVT_KEY_LONG(KEY_MENU):
      g_eeGeneral.globalTimer = 0;
      storageDirty(EE_GENERAL);
      sessionTimer = 0;
      break;
#endif
    case EVT_KEY_FIRST(KEY_EXIT):
      chainMenu(menuMainView);
      break;
  }

  lcdDrawText(  1*FW, FH*0, STR_TOTTM1TM2THRTHP);
  
  drawTimer(    5*FW+5*FWNUM+1, FH*1, timersStates[0].val, RIGHT, 0);
  drawTimer(   12*FW+5*FWNUM+1, FH*1, timersStates[1].val, RIGHT, 0);

  drawTimer(    5*FW+5*FWNUM+1, FH*2, s_timeCumThr, RIGHT, 0);
  drawTimer(   12*FW+5*FWNUM+1, FH*2, s_timeCum16ThrP/16, RIGHT, 0);

  drawTimer(   12*FW+5*FWNUM+1, FH*0, sessionTimer, RIGHT, 0);

#if defined(THRTRACE)
  const coord_t x = 5;
  const coord_t y = 60;
  lcdDrawSolidHorizontalLine(x-3, y, MAXTRACE+3+3);
  lcdDrawSolidVerticalLine(x, y-32, 32+3);
  for (coord_t i=0; i<MAXTRACE; i+=6) {
    lcdDrawSolidVerticalLine(x+i+6, y-1, 3);
  }

  uint16_t traceRd = s_traceWr > MAXTRACE ? s_traceWr - MAXTRACE : 0;
  for (coord_t i=1; i<=MAXTRACE && traceRd<s_traceWr; i++, traceRd++) {
    uint8_t h = s_traceBuf[traceRd % MAXTRACE];
    lcdDrawSolidVerticalLine(x+i, y-h, h);
  }
#endif
}

#if defined(CPUARM)
  #define MENU_DEBUG_COL1_OFS          (11*FW-3)
  #define MENU_DEBUG_COL2_OFS          (17*FW)
  #define MENU_DEBUG_Y_CURRENT         (1*FH)
  #define MENU_DEBUG_Y_MAH             (2*FH)
  #define MENU_DEBUG_Y_CPU_TEMP        (3*FH)
  #define MENU_DEBUG_Y_COPROC          (4*FH)
  #define MENU_DEBUG_Y_MIXMAX          (5*FH)
  #define MENU_DEBUG_Y_RTOS            (6*FH)
#else
  #define MENU_DEBUG_COL1_OFS          (14*FW)
#endif

void menuStatisticsDebug(event_t event)
{
  TITLE(STR_MENUDEBUG);

  switch (event) {
#if defined(CPUARM)
    case EVT_KEY_LONG(KEY_ENTER):
#if defined(PCBSKY9X)
      g_eeGeneral.mAhUsed = 0;
      Current_used = 0;
#endif
      g_eeGeneral.globalTimer = 0;
      sessionTimer = 0;
      storageDirty(EE_GENERAL);
      killEvents(event);
      break;
#endif
      
    case EVT_KEY_FIRST(KEY_ENTER):
#if !defined(CPUARM)
      g_tmr1Latency_min = 0xff;
      g_tmr1Latency_max = 0;
#endif
      maxMixerDuration  = 0;
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

#if defined(PCBSKY9X)
  if ((ResetReason&RSTC_SR_RSTTYP) == (2<<8)) {
    lcdDrawText(LCD_W-8*FW, 0*FH, "WATCHDOG");
  }
  else if (unexpectedShutdown) {
    lcdDrawText(LCD_W-13*FW, 0*FH, "UNEXP.SHTDOWN");
  }
#endif

#if defined(TX_CAPACITY_MEASUREMENT)
  // current
  lcdDrawTextAlignedLeft(MENU_DEBUG_Y_CURRENT, STR_CPU_CURRENT);
  drawValueWithUnit(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_CURRENT, getCurrent(), UNIT_MILLIAMPS, LEFT);
  uint32_t current_scale = 488 + g_eeGeneral.txCurrentCalibration;
  lcdDrawChar(MENU_DEBUG_COL2_OFS, MENU_DEBUG_Y_CURRENT, '>');
  drawValueWithUnit(MENU_DEBUG_COL2_OFS+FW+1, MENU_DEBUG_Y_CURRENT, Current_max*10*current_scale/8192, UNIT_RAW, LEFT);
  // consumption
  lcdDrawTextAlignedLeft(MENU_DEBUG_Y_MAH, STR_CPU_MAH);
  drawValueWithUnit(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_MAH, g_eeGeneral.mAhUsed + Current_used*current_scale/8192/36, UNIT_MAH, LEFT|PREC1);
#endif

#if defined(PCBSKY9X)
  lcdDrawTextAlignedLeft(MENU_DEBUG_Y_CPU_TEMP, STR_CPU_TEMP);
  drawValueWithUnit(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_CPU_TEMP, getTemperature(), UNIT_TEMPERATURE, LEFT);
  lcdDrawChar(MENU_DEBUG_COL2_OFS, MENU_DEBUG_Y_CPU_TEMP, '>');
  drawValueWithUnit(MENU_DEBUG_COL2_OFS+FW+1, MENU_DEBUG_Y_CPU_TEMP, maxTemperature+g_eeGeneral.temperatureCalib, UNIT_TEMPERATURE, LEFT);
#endif

#if defined(COPROCESSOR)
  lcdDrawTextAlignedLeft(MENU_DEBUG_Y_COPROC, STR_COPROC_TEMP);

  if (Coproc_read==0) {
    lcdDrawText(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_COPROC, PSTR("Co Proc NACK"),INVERS);
  }
  else if (Coproc_read==0x81) {
    lcdDrawText(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_COPROC, PSTR("Inst.TinyApp"),INVERS);
  }
  else if (Coproc_read<3) {
    lcdDrawText(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_COPROC, PSTR("Upgr.TinyApp"),INVERS);
  }
  else {
    drawValueWithUnit(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_COPROC, Coproc_temp, UNIT_TEMPERATURE, LEFT);
    drawValueWithUnit(MENU_DEBUG_COL2_OFS, MENU_DEBUG_Y_COPROC, Coproc_maxtemp, UNIT_TEMPERATURE, LEFT);
  }
#endif

#if defined(CPUARM)
  lcdDrawTextAlignedLeft(MENU_DEBUG_Y_MIXMAX, STR_TMIXMAXMS);
  lcdDrawNumber(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_MIXMAX, DURATION_MS_PREC2(maxMixerDuration), PREC2|LEFT);
  lcdDrawText(lcdLastRightPos, MENU_DEBUG_Y_MIXMAX, "ms");
#endif

#if defined(CPUARM)
  lcdDrawTextAlignedLeft(MENU_DEBUG_Y_RTOS, STR_FREESTACKMINB);
  lcdDrawNumber(MENU_DEBUG_COL1_OFS, MENU_DEBUG_Y_RTOS+2, menusStack.available(), UNSIGN|LEFT|TINSIZE);
  lcdDrawText(lcdLastRightPos, MENU_DEBUG_Y_RTOS, "/");
  lcdDrawNumber(lcdLastRightPos, MENU_DEBUG_Y_RTOS+2, mixerStack.available(), UNSIGN|LEFT|TINSIZE);
  lcdDrawText(lcdLastRightPos, MENU_DEBUG_Y_RTOS, "/");
  lcdDrawNumber(lcdLastRightPos, MENU_DEBUG_Y_RTOS+2, audioStack.available(), UNSIGN|LEFT|TINSIZE);
#endif

#if !defined(CPUARM)
  lcdDrawTextAlignedLeft(1*FH, STR_TMR1LATMAXUS);
  lcdDraw8bitsNumber(MENU_DEBUG_COL1_OFS , 1*FH, g_tmr1Latency_max/2 );
  lcdDrawTextAlignedLeft(2*FH, STR_TMR1LATMINUS);
  lcdDraw8bitsNumber(MENU_DEBUG_COL1_OFS , 2*FH, g_tmr1Latency_min/2 );
  lcdDrawTextAlignedLeft(3*FH, STR_TMR1JITTERUS);
  lcdDraw8bitsNumber(MENU_DEBUG_COL1_OFS , 3*FH, (g_tmr1Latency_max - g_tmr1Latency_min) /2 );
  lcdDrawTextAlignedLeft(4*FH, STR_TMIXMAXMS);
  lcdDrawNumber(MENU_DEBUG_COL1_OFS, 4*FH, DURATION_MS_PREC2(maxMixerDuration), PREC2);
  lcdDrawTextAlignedLeft(5*FH, STR_FREESTACKMINB);
  lcdDrawNumber(14*FW, 5*FH, stackAvailable(), UNSIGN) ;
#endif

  lcdDrawText(4*FW, 7*FH+1, STR_MENUTORESET);
  lcdInvertLastLine();
}
