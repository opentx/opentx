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

#include "menus.h"

#define BOX_WIDTH     23
#define BAR_HEIGHT    (BOX_WIDTH-1l)
#define MARKER_WIDTH  5
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define BOX_LIMIT     (BOX_WIDTH-MARKER_WIDTH)
#define LBOX_CENTERX  (  SCREEN_WIDTH/4 + 10)
#define LBOX_CENTERY  (SCREEN_HEIGHT-9-BOX_WIDTH/2)
#define RBOX_CENTERX  (3*SCREEN_WIDTH/4 - 10)
#define RBOX_CENTERY  (SCREEN_HEIGHT-9-BOX_WIDTH/2)

void doMainScreenGrphics()
{
  lcd_square(LBOX_CENTERX-BOX_WIDTH/2, LBOX_CENTERY-BOX_WIDTH/2, BOX_WIDTH);
  lcd_square(RBOX_CENTERX-BOX_WIDTH/2, RBOX_CENTERY-BOX_WIDTH/2, BOX_WIDTH);

  DO_CROSS(LBOX_CENTERX,LBOX_CENTERY,3)
  DO_CROSS(RBOX_CENTERX,RBOX_CENTERY,3)

  lcd_square(LBOX_CENTERX+(calibratedStick[CONVERT_MODE(0+1)-1]*BOX_LIMIT/(2*RESX))-MARKER_WIDTH/2, LBOX_CENTERY-(calibratedStick[CONVERT_MODE(1+1)-1]*BOX_LIMIT/(2*RESX))-MARKER_WIDTH/2, MARKER_WIDTH, ROUND);
  lcd_square(RBOX_CENTERX+(calibratedStick[CONVERT_MODE(3+1)-1]*BOX_LIMIT/(2*RESX))-MARKER_WIDTH/2, RBOX_CENTERY-(calibratedStick[CONVERT_MODE(2+1)-1]*BOX_LIMIT/(2*RESX))-MARKER_WIDTH/2, MARKER_WIDTH, ROUND);

  // Optimization by Mike Blandford
  {
    uint8_t x, y, len ;  // declare temporary variables
    for( x = -5, y = 4 ; y < 7 ; x += 5, y += 1 )
    {
      len = ((calibratedStick[y]+RESX)*BAR_HEIGHT/(RESX*2))+1l;  // calculate once per loop
      V_BAR(SCREEN_WIDTH/2+x,SCREEN_HEIGHT-8, len)
    }
  }
}

void menuMainView(uint8_t event)
{
  static bool instantTrimSwLock;
  static bool trim2OfsSwLock;

  uint8_t view = g_eeGeneral.view;
  uint8_t view_base = view & 0x0f;

  switch(event)
  {
    /* TODO if timer2 is OFF, it's possible to use this timer2 as in er9x...
    case EVT_KEY_BREAK(KEY_MENU):
      if (view_base == e_timer2) {
        Timer2_running = !Timer2_running;
        AUDIO_KEYPAD_UP();
      }
    break;
    */
    case EVT_KEY_LONG(KEY_MENU):// go to last menu
      pushMenu(lastPopMenu());
      killEvents(event);
      break;
    case EVT_KEY_BREAK(KEY_RIGHT):
    case EVT_KEY_BREAK(KEY_LEFT):
      if (view_base == e_inputs) {
        g_eeGeneral.view ^= ALTERNATE_VIEW;
        eeDirty(EE_GENERAL);
        AUDIO_KEYPAD_UP();
      }
      break;
    case EVT_KEY_LONG(KEY_RIGHT):
      pushMenu(menuProcModelSelect);
      killEvents(event);
      break;
    case EVT_KEY_LONG(KEY_LEFT):
      pushMenu(menuProcSetup);
      killEvents(event);
      break;
    case EVT_KEY_BREAK(KEY_UP):
    case EVT_KEY_BREAK(KEY_DOWN):
      g_eeGeneral.view = (event == EVT_KEY_BREAK(KEY_UP) ? (view_base == MAIN_VIEW_MAX ? 0 : view_base + 1) : (view_base == 0 ? MAIN_VIEW_MAX : view_base - 1));
      eeDirty(EE_GENERAL);
      AUDIO_KEYPAD_UP();
      break;
    case EVT_KEY_LONG(KEY_UP):
      chainMenu(menuProcStatistic);
      killEvents(event);
      break;
    case EVT_KEY_LONG(KEY_DOWN):
#if defined(FRSKY)
      chainMenu(menuProcFrsky);
#elif defined(JETI)
      JETI_EnableRXD(); // enable JETI-Telemetry reception
      chainMenu(menuProcJeti);
#elif defined(ARDUPILOT)
      ARDUPILOT_EnableRXD(); // enable ArduPilot-Telemetry reception
      chainMenu(menuProcArduPilot);
#elif defined(NMEA)
      NMEA_EnableRXD(); // enable NMEA-Telemetry reception
      chainMenu(menuProcNMEA);
#else
      chainMenu(menuProcDebug);
#endif
      killEvents(event);
      break;
    case EVT_KEY_FIRST(KEY_EXIT):
      if(s_timerState[0]==TMR_BEEPING) {
        s_timerState[0] = TMR_STOPPED;
      }
#ifdef NAVIGATION_RE1
      else if (s_warning) {
        s_warning = NULL;
      }
#endif
      else if (view == e_timer2) {
        resetTimer(1);
      }
      else {
        resetTimer(0);
      }
      AUDIO_KEYPAD_UP();
      break;
    case EVT_KEY_LONG(KEY_EXIT):
      resetAll();
      AUDIO_KEYPAD_UP();
      break;
    case EVT_ENTRY:
      killEvents(KEY_EXIT);
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      instantTrimSwLock = true;
      trim2OfsSwLock = true;
      break;
#ifdef NAVIGATION_RE1
    case EVT_KEY_LONG(BTN_RE1):
      killEvents(event);
      if (s_inflight_value && !s_warning) {
        s_warning = s_inflight_label;
        s_editMode = 1;
        break;
      }
      // no break
    case EVT_KEY_BREAK(BTN_RE1):
      s_warning = NULL;
      break;
#endif
  }

  bool trimSw = isFunctionActive(FUNC_INSTANT_TRIM);
  if (!instantTrimSwLock && trimSw) instantTrim();
  instantTrimSwLock = trimSw;
  
  trimSw = isFunctionActive(FUNC_TRIMS_2_OFS);
  if (!trim2OfsSwLock && trimSw) moveTrimsToOffsets();
  trim2OfsSwLock = trimSw;

  {
    uint8_t phase = getFlightPhase();
    lcd_putsnAtt(6*FW+2, 2*FH, g_model.phaseData[phase].name, sizeof(g_model.phaseData[phase].name), ZCHAR);

    uint8_t att = (g_vbat100mV < g_eeGeneral.vBatWarn ? BLINK : 0) | DBLSIZE;
    putsModelName(2*FW-2, 0*FH, g_model.name, g_eeGeneral.currModel, DBLSIZE);
    putsVBat(6*FW-1, 2*FH, att|NO_UNIT);
    lcd_putc(6*FW, 3*FH, 'V');

    if (g_model.timer1.mode) {
      uint8_t att = DBLSIZE | (s_timerState[0]==TMR_BEEPING ? BLINK : 0);
      putsTime(12*FW+3, FH*2, s_timerVal[0], att, att);
      putsTmrMode(s_timerVal[0] >= 0 ? 9*FW-FW/2+5 : 9*FW-FW/2-2, FH*3, g_model.timer1.mode, SHRT_TM_MODE);
    }

    // trim sliders
    for(uint8_t i=0; i<4; i++)
    {
#define TL 27
      //                        LH LV RV RH
      static uint8_t x[4]    = {128*1/4+2, 4, 128-4, 128*3/4-2};
      static uint8_t vert[4] = {0,1,1,0};
      uint8_t xm, ym;
      xm = x[CONVERT_MODE(i+1)-1];

      uint8_t att = ROUND;
      int16_t val = getTrimValue(getTrimFlightPhase(i, phase), i);

      if (val < -125 || val > 125)
        att = BLINK|ROUND;

      if (val < -(TL+1)*4)
        val = -(TL+1);
      else if (val > (TL+1)*4)
        val = TL+1;
      else
        val /= 4;

      if (vert[i]) {
        ym = 31;
        lcd_vline(xm, ym-TL, TL*2);
        if (i!=2 || !g_model.thrTrim) {
          lcd_vline(xm-1, ym-1,  3);
          lcd_vline(xm+1, ym-1,  3);
        }
        ym -= val;
      }
      else {
        ym = 60;
        lcd_hline(xm-TL, ym, TL*2);
        lcd_hline(xm-1, ym-1,  3);
        lcd_hline(xm-1, ym+1,  3);
        xm += val;
      }
      lcd_square(xm-3, ym-3, 7, att);
    }
  }

  if(view_base<e_inputs) {
    for(uint8_t i=0; i<8; i++)
    {
      uint8_t x0,y0;
      int16_t val = g_chans512[i];
      //val += g_model.limitData[i].revert ? g_model.limitData[i].offset : -g_model.limitData[i].offset;
      switch(view_base)
      {
        case e_outputValues:
          x0 = (i%4*9+3)*FW/2;
          y0 = i/4*FH+40;
          // *1000/1024 = x - x/32 + x/128
#define GPERC(x)  (x - x/32 + x/128)
#if defined (DECIMALS_DISPLAYED)
          lcd_outdezAtt( x0+4*FW , y0, GPERC(val), PREC1);
#else
          lcd_outdezAtt( x0+4*FW , y0, GPERC(val)/10, 0); // G: Don't like the decimal part*
#endif
          break;
        case e_outputBars:
#define WBAR2 (50/2)
          x0       = i<4 ? 128/4+2 : 128*3/4-2;
          y0       = 38+(i%4)*5;

          int8_t len = (abs(val) * WBAR2 + 512) / 1024;
          if(len>WBAR2)  len =  WBAR2;  // prevent bars from going over the end - comment for debugging
          lcd_hlineStip(x0-WBAR2, y0, WBAR2*2+1, DOTTED);
          lcd_vline(x0,y0-2,5);
          if (val>0){
            x0+=1;
          }else{
            x0-=len;
          }
          lcd_hline(x0,y0+1,len);
          lcd_hline(x0,y0-1,len);
          break;
      }
    }
  }
  else if (view_base == e_inputs) {
    if (view == e_inputs) {
      // hardware inputs
      doMainScreenGrphics();
      for (uint8_t i=0; i<6; i++) {
        int8_t sw1 = (i<3 ? 1+i : 4+i);
        int8_t sw2 = (sw1 == 9 ? (getSwitch(4, 0) ? 4 : (getSwitch(5, 0) ? 5 : 6)) : sw1);
        putsSwitches(i<3 ? 2*FW-2: 17*FW-1, (i%3)*FH+4*FH, sw2, getSwitch(sw1, 0) ? INVERS : 0);
      }
    }
    else {
      // virtual inputs
      for (uint8_t i=0; i<8; i++) {
        int16_t val = g_chans512[8+i];
        int8_t len = limit((int16_t)0, (int16_t)(((val+1024) * BAR_HEIGHT) / 2048), (int16_t)BAR_HEIGHT);
        V_BAR(SCREEN_WIDTH/2-5*4+2+i*5, SCREEN_HEIGHT-8, len)
      }
      for (uint8_t i=0; i<12; i++) {
        if ((i%6) < 3) lcd_puts(i<6 ? 2*FW-2 : 16*FW-2, (i%3)*FH+4*FH, STR_SW);
        lcd_putcAtt((i<6 ? 2*FW-2 : 16*FW-2) + 2 * FW + ((i%6) < 3 ? 0 : FW), (i%3)*FH+4*FH, i<9 ? '1'+i : 'A'+i-9, getSwitch(10+i, 0) ? INVERS : 0);
      }
    }
  }
  else { // timer2
    putsTime(33+FW+2, FH*5, s_timerVal[1], DBLSIZE, DBLSIZE);
    putsTmrMode(s_timerVal[1] >= 0 ? 20-FW/2+5 : 20-FW/2-2, FH*6, g_model.timer2.mode, SHRT_TM_MODE);
    // lcd_outdezNAtt(33+11*FW, FH*6, s_timerVal_10ms[1], LEADING0, 2); // 1/100s
  }

#ifdef NAVIGATION_RE1
  check_rotary_encoder();
  if (s_warning) {
    int8_t value = (((uint8_t)(*s_inflight_value)) >> s_inflight_bitshift) - s_inflight_shift;
    if (p1valdiff) {
      value = checkIncDecModel(event, value, s_inflight_min, s_inflight_max);
      *s_inflight_value = (((uint8_t)(*s_inflight_value)) & ((1 << s_inflight_bitshift) - 1)) + ((s_inflight_shift + value) << s_inflight_bitshift);
    }
    displayBox();
    lcd_outdezAtt(16, 4*FH, value, LEFT);
  }
#endif

  theFile.DisplayProgressBar(20*FW+1);
}
