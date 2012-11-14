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

#define BOX_WIDTH     23
#define BAR_HEIGHT    (BOX_WIDTH-1l)
#define MARKER_WIDTH  5
#define BOX_LIMIT     (BOX_WIDTH-MARKER_WIDTH)
#define LBOX_CENTERX  (DISPLAY_W/4 + 10)
#define LBOX_CENTERY  (DISPLAY_H-9-BOX_WIDTH/2)
#define RBOX_CENTERX  (3*DISPLAY_W/4 - 10)
#define RBOX_CENTERY  (DISPLAY_H-9-BOX_WIDTH/2)

void doMainScreenGrphics()
{
  lcd_square(LBOX_CENTERX-BOX_WIDTH/2, LBOX_CENTERY-BOX_WIDTH/2, BOX_WIDTH);
  lcd_square(RBOX_CENTERX-BOX_WIDTH/2, RBOX_CENTERY-BOX_WIDTH/2, BOX_WIDTH);

  DO_CROSS(LBOX_CENTERX,LBOX_CENTERY,3)
  DO_CROSS(RBOX_CENTERX,RBOX_CENTERY,3)

  int16_t calibStickVert = calibratedStick[CONVERT_MODE(1+1)-1];
  if (g_eeGeneral.throttleReversed && CONVERT_MODE(1+1)-1 == THR_STICK)
    calibStickVert = -calibStickVert;
  lcd_square(LBOX_CENTERX+(calibratedStick[CONVERT_MODE(0+1)-1]*BOX_LIMIT/(2*RESX))-MARKER_WIDTH/2, LBOX_CENTERY-(calibStickVert*BOX_LIMIT/(2*RESX))-MARKER_WIDTH/2, MARKER_WIDTH, ROUND);
  calibStickVert = calibratedStick[CONVERT_MODE(2+1)-1];
  if (g_eeGeneral.throttleReversed && CONVERT_MODE(2+1)-1 == THR_STICK)
    calibStickVert = -calibStickVert;
  lcd_square(RBOX_CENTERX+(calibratedStick[CONVERT_MODE(3+1)-1]*BOX_LIMIT/(2*RESX))-MARKER_WIDTH/2, RBOX_CENTERY-(calibStickVert*BOX_LIMIT/(2*RESX))-MARKER_WIDTH/2, MARKER_WIDTH, ROUND);

  // Optimization by Mike Blandford
  {
    uint8_t x, y, len ;  // declare temporary variables
    for( x = -5, y = 4 ; y < 7 ; x += 5, y += 1 )
    {
      len = ((calibratedStick[y]+RESX)*BAR_HEIGHT/(RESX*2))+1l;  // calculate once per loop
      V_BAR(DISPLAY_W/2+x,DISPLAY_H-8, len)
    }
  }
}

#if defined(PCBX9D)
#define EVT_KEY_MODEL_MENU   EVT_KEY_BREAK(KEY_MENU)
#define EVT_KEY_GENERAL_MENU EVT_KEY_LONG(KEY_MENU)
#define EVT_KEY_TELEMETRY    EVT_KEY_LONG(KEY_PAGE)
#define EVT_KEY_STATISTICS   EVT_KEY_LONG(KEY_ENTER)
#else
#define EVT_KEY_MODEL_MENU   EVT_KEY_LONG(KEY_RIGHT)
#define EVT_KEY_GENERAL_MENU EVT_KEY_LONG(KEY_LEFT)
#define EVT_KEY_TELEMETRY    EVT_KEY_LONG(KEY_DOWN)
#define EVT_KEY_STATISTICS   EVT_KEY_LONG(KEY_UP)
#endif

void menuMainView(uint8_t event)
{
  uint8_t view = g_eeGeneral.view;
  uint8_t view_base = view & 0x0f;

  uint8_t _event = event;
  if (s_global_warning) {
    event = 0;
  }

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

#if !defined(PCBX9D) && !defined(READONLY)
    case EVT_KEY_LONG(KEY_MENU):// go to last menu
      pushMenu(lastPopMenu());
      killEvents(event);
      break;
#endif

    case EVT_KEY_BREAK(KEY_RIGHT):
    case EVT_KEY_BREAK(KEY_LEFT):
      if (view_base <= e_inputs) {
#if defined(PCBSKY9X)
        if (view_base == e_inputs)
          g_eeGeneral.view ^= ALTERNATE_VIEW;
        else
          g_eeGeneral.view = (g_eeGeneral.view + (4*ALTERNATE_VIEW) + ((event==EVT_KEY_BREAK(KEY_LEFT)) ? -ALTERNATE_VIEW : ALTERNATE_VIEW)) % (4*ALTERNATE_VIEW);
#else
        g_eeGeneral.view ^= ALTERNATE_VIEW;
#endif
        eeDirty(EE_GENERAL);
        AUDIO_KEYPAD_UP();
      }
      break;

#if !defined(READONLY)
    case EVT_KEY_MODEL_MENU:
      pushMenu(menuModelSelect);
      killEvents(event);
      break;
    case EVT_KEY_GENERAL_MENU:
      pushMenu(menuGeneralSetup);
      killEvents(event);
      break;
#endif

#if defined(PCBX9D)
    case EVT_KEY_BREAK(KEY_PAGE):
#else
    case EVT_KEY_BREAK(KEY_UP):
    case EVT_KEY_BREAK(KEY_DOWN):
#endif
      g_eeGeneral.view = (event == EVT_KEY_BREAK(KEY_UP) ? (view_base == MAIN_VIEW_MAX ? 0 : view_base + 1) : (view_base == 0 ? MAIN_VIEW_MAX : view_base - 1));
      eeDirty(EE_GENERAL);
      AUDIO_KEYPAD_UP();
      break;
    case EVT_KEY_STATISTICS:
      chainMenu(menuStatisticsView);
      killEvents(event);
      break;
    case EVT_KEY_TELEMETRY:
#if defined(FRSKY)
      chainMenu(menuTelemetryFrsky);
#elif defined(JETI)
      JETI_EnableRXD(); // enable JETI-Telemetry reception
      chainMenu(menuTelemetryJeti);
#elif defined(ARDUPILOT)
      ARDUPILOT_EnableRXD(); // enable ArduPilot-Telemetry reception
      chainMenu(menuTelemetryArduPilot);
#elif defined(NMEA)
      NMEA_EnableRXD(); // enable NMEA-Telemetry reception
      chainMenu(menuTelemetryNMEA);
#elif defined(MAVLINK)
      chainMenu(menuTelemetryMavlink);
#else
      chainMenu(menuStatisticsDebug);
#endif
      killEvents(event);
      break;
    case EVT_KEY_FIRST(KEY_EXIT):
      if (s_timerState[0]==TMR_BEEPING) {
        s_timerState[0] = TMR_STOPPED;
      }
      else if (s_global_warning) {
        s_global_warning = NULL;
      }
#if defined(GVARS)
      else if (s_gvar_timer > 0) {
        s_gvar_timer = 0;
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
      break;
  }

  {
    // Flight Phase Name
    uint8_t phase = s_perout_flight_phase;
#if defined(PCBX9D)
    uint8_t len = ZLEN(g_model.phaseData[phase].name);
    if (len > 0) {
      uint8_t tmp = (DISPLAY_W - 6*FW - 2 - len*FW/2);
      lcd_putsnAtt(tmp, 5, g_model.phaseData[phase].name, sizeof(g_model.phaseData[phase].name), ZCHAR);
      lcd_rect(tmp-2, 5-2, 3+FW*len, FH+3);
    }
#else
    lcd_putsnAtt(6*FW, 2*FH, g_model.phaseData[phase].name, sizeof(g_model.phaseData[phase].name), ZCHAR);
#endif

    // Model Name
    putsModelName(2*FW-2, 0*FH, g_model.name, g_eeGeneral.currModel, DBLSIZE);

    // Voltage (or alarm if any)
#if defined(PCBSKY9X)
    if (g_vbat100mV > g_eeGeneral.vBatWarn && g_eeGeneral.temperatureWarn && getTemperature() >= g_eeGeneral.temperatureWarn) {
      putsTelemetryValue(6*FW-1, 3*FH, getTemperature(), UNIT_DEGREES, BLINK|INVERS|DBLSIZE);
    }
    else if (g_vbat100mV > g_eeGeneral.vBatWarn && g_eeGeneral.mAhWarn && (g_eeGeneral.mAhUsed + Current_used * (488 + g_eeGeneral.currentCalib)/8192/36) / 500 >= g_eeGeneral.mAhWarn) {
      putsTelemetryValue(7*FW-1, 3*FH, (g_eeGeneral.mAhUsed + Current_used*(488 + g_eeGeneral.currentCalib)/8192/36)/10, UNIT_MAH, BLINK|INVERS|DBLSIZE);
    }
    else
#endif
    {
      uint8_t att = (g_vbat100mV <= g_eeGeneral.vBatWarn ? BLINK|INVERS : 0) | DBLSIZE;
      putsVBat(6*FW-1, 2*FH, att|NO_UNIT);
      lcd_putc(6*FW, 3*FH, 'V');
    }

    // And ! in case of unexpected shutdown
    if (unexpectedShutdown) {
      lcd_putcAtt(20*FW-3, 0*FH, '!', INVERS);
    }

#if defined(PCBX9D)
    // Main timer
    if (g_model.timers[0].mode) {
      uint8_t att = DBLSIZE | (s_timerState[0]==TMR_BEEPING ? BLINK|INVERS : 0);
      putsTime(12*FW-2, FH*2, s_timerVal[0], att, att);
      putsTmrMode(s_timerVal[0] >= 0 ? 9*FW-FW/2 : 9*FW-FW/2-7, FH*3, g_model.timers[0].mode, SWCONDENSED);
    }

    // Second timer
    if (g_model.timers[1].mode) {
      uint8_t att = DBLSIZE | (s_timerState[1]==TMR_BEEPING ? BLINK|INVERS : 0);
      putsTime(23*FW+1, FH*2, s_timerVal[1], att, att);
      putsTmrMode(s_timerVal[1] >= 0 ? 20*FW-FW/2+4 : 20*FW-FW/2-3, FH*3, g_model.timers[1].mode, SWCONDENSED);
    }
#else
    // Main timer
    if (g_model.timers[0].mode) {
      uint8_t att = DBLSIZE | (s_timerState[0]==TMR_BEEPING ? BLINK|INVERS : 0);
      putsTime(12*FW+3, FH*2, s_timerVal[0], att, att);
      putsTmrMode(s_timerVal[0] >= 0 ? 9*FW-FW/2+5 : 9*FW-FW/2-2, FH*3, g_model.timers[0].mode, SWCONDENSED);
    }
#endif

    // Trim sliders
    for(uint8_t i=0; i<4; i++)
    {
#define TL 27
      //                        LH LV RV RH
      static uint8_t x[4]    = {DISPLAY_W*1/4+2, 3, DISPLAY_W-4, DISPLAY_W*3/4-2};
      static uint8_t vert[4] = {0,1,1,0};
      uint8_t xm, ym;
      xm = x[CONVERT_MODE(i+1)-1];

      uint8_t att = ROUND;
      int16_t val = getTrimValue(phase, i);

      if (val < -125 || val > 125)
        att = BLINK|INVERS|ROUND;

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

  if (view_base < e_inputs) {
    // scroll bar
    lcd_hlineStip(38, 34, 54, DOTTED);
#if defined(PCBSKY9X)
    lcd_hline(38 + (g_eeGeneral.view / ALTERNATE_VIEW) * 13, 34, 13, SOLID);
#else
    lcd_hline((g_eeGeneral.view & ALTERNATE_VIEW) ? 64 : 38, 34, 26, SOLID);
#endif

    for (uint8_t i=0; i<8; i++) {
      uint8_t x0,y0;
#if defined(PCBSKY9X)
      uint8_t chan = 8*(g_eeGeneral.view / ALTERNATE_VIEW) + i;
#else
      uint8_t chan = (g_eeGeneral.view & ALTERNATE_VIEW) ? 8+i : i;
#endif

      int16_t val = g_chans512[chan];

      switch(view_base)
      {
        case e_outputValues:
          x0 = (i%4*9+3)*FW/2;
          y0 = i/4*FH+40;
#if defined (DECIMALS_DISPLAYED)
          lcd_outdezAtt( x0+4*FW , y0, calcRESXto1000(val), PREC1);
#else
          lcd_outdezAtt( x0+4*FW , y0, calcRESXto1000(val)/10, 0); // G: Don't like the decimal part*
#endif
          break;

        case e_outputBars:
#define WBAR2 (50/2)
          x0       = i<4 ? DISPLAY_W/4+2 : DISPLAY_W*3/4-2;
          y0       = 38+(i%4)*5;

          uint16_t lim = g_model.extendedLimits ? 640*2 : 512*2;
          int8_t len = (abs(val) * WBAR2 + lim/2) / lim;

          if(len>WBAR2)  len = WBAR2;  // prevent bars from going over the end - comment for debugging
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
        putsSwitches(i<3 ? 2*FW-2: 17*FW-1, (i%3)*FH+4*FH+1, sw2, getSwitch(sw1, 0) ? INVERS : 0);
      }
    }
    else {
#if defined(ROTARY_ENCODERS)
      for (uint8_t i=0; i<NUM_ROTARY_ENCODERS; i++) {
        int16_t val = getRotaryEncoder(i);
        int8_t len = limit((int16_t)0, (int16_t)(((val+1024) * BAR_HEIGHT) / 2048), (int16_t)BAR_HEIGHT);
#if defined(EXTRA_ROTARY_ENCODERS)
#define V_BAR_W 5
        V_BAR(DISPLAY_W/2-8+V_BAR_W*i, DISPLAY_H-8, len)
#else //EXTRA_ROTARY_ENCODERS
#define V_BAR_W 5
        V_BAR(DISPLAY_W/2-3+V_BAR_W*i, DISPLAY_H-8, len)
#endif //EXTRA_ROTARY_ENCODERS
      }
#endif // ROTARY_ENCODERS
#if defined(PCBSKY9X)
      for (uint8_t i=0; i<NUM_CSW; i++) {
        int8_t len = getSwitch(10+i, 0) ? BAR_HEIGHT : 1;
        lcd_vline(16+3*i-1,DISPLAY_H-8-len,len);
        lcd_vline(16+3*i  ,DISPLAY_H-8-len,len);
      }
#elif defined(PCBGRUVIN9X) && defined(EXTRA_ROTARY_ENCODERS)
      for (uint8_t i=0; i<NUM_CSW; i++)
        putsSwitches(2*FW-2 + (i/3)*(4*FW-2) + (i/3>1 ? 3*FW+6 : 0), 4*FH+1 + (i%3)*FH, 10+i, getSwitch(10+i, 0) ? INVERS : 0);
#elif defined(PCBGRUVIN9X)
      for (uint8_t i=0; i<NUM_CSW; i++)
        putsSwitches(2*FW-2 + (i/3)*(4*FW) + (i/3>1 ? 3*FW : 0), 4*FH+1 + (i%3)*FH, 10+i, getSwitch(10+i, 0) ? INVERS : 0);
#else
      for (uint8_t i=0; i<NUM_CSW; i++)
        putsSwitches(2*FW-2 + (i/3)*(5*FW), 4*FH+1 + (i%3)*FH, 10+i, getSwitch(10+i, 0) ? INVERS : 0);
#endif
    }
  }
  else { // timer2
    putsTime(33+FW+2, FH*5, s_timerVal[1], DBLSIZE, DBLSIZE);
    putsTmrMode(s_timerVal[1] >= 0 ? 20-FW/2+5 : 20-FW/2-2, FH*6, g_model.timers[1].mode, SWCONDENSED);
    // lcd_outdezNAtt(33+11*FW, FH*6, s_timerVal_10ms[1], LEADING0, 2); // 1/100s
  }

  if (s_global_warning) {
    s_warning = s_global_warning;
    displayWarning(_event);
    if (!s_warning) s_global_warning = NULL;
    s_warning = NULL;
  }

#if defined(GVARS)
  else if (s_gvar_timer > 0) {
    s_gvar_timer--;
    s_warning = PSTR("Global Variable");
    displayBox();
#if defined(M64)
    lcd_putsAtt(16+4*FW, 5*FH, PSTR("[\010]"), BOLD);
    lcd_outdezAtt(16+4*FW+4*FW+FW/2, 5*FH, GVAR_VALUE(s_gvar_last, s_perout_flight_phase), BOLD);
#else
    lcd_putsnAtt(16, 5*FH, g_model.gvarsNames[s_gvar_last], sizeof(gvar_name_t), ZCHAR);
    lcd_putsAtt(16+7*FW, 5*FH, PSTR("[\010]"), BOLD);
    lcd_outdezAtt(16+7*FW+4*FW+FW/2, 5*FH, GVAR_VALUE(s_gvar_last, s_perout_flight_phase), BOLD);
#endif
    s_warning = NULL;
  }
#endif

#ifdef DSM2
  if (s_bind_mode) // Issue 98
    lcd_putsAtt(15*FW, 0, PSTR("BIND"), 0);
#endif
}
