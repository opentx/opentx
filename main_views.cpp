/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 *
 * gruvin9x is based on code named er9x by
 * Author - Erez Raviv <erezraviv@gmail.com>, which is in turn
 * was based on the original (and ongoing) project by Thomas Husterer,
 * th9x -- http://code.google.com/p/th9x/
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

#define ALTERNATE_VIEW 0x10

enum MainViews {
  e_outputValues,
  e_outputBars,
  e_inputs,
  e_timer2,
#ifdef FRSKY
  e_telemetry,
#endif
  MAX_VIEWS
};

uint8_t tabViews[] = {
  1, /*e_outputValues*/
  1, /*e_outputBars*/
  2, /*e_inputs*/
  1, /*e_timer2*/
#if defined(FRSKY_HUB)
  5, /*e_telemetry*/
#elif defined(WS_HOW_HIGH)
  4, /*e_telemetry*/
#elif defined(FRSKY)
  3, /*e_telemetry*/
#endif
};

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

#if defined(FRSKY)
void displayA1A2(uint8_t x, uint8_t y, uint8_t idx)
{
  lcd_puts(x, y, PSTR("A :"));
  lcd_putc(x+FW, y, '1'+idx);
}

void displayRssiLine()
{
  lcd_hline(0, 54, 128, 0); // separator
  lcd_putsLeft(7*FH+1, STR_TX); lcd_outdezNAtt(4*FW, 7*FH+1, frskyRSSI[1].value, LEADING0, 2);
  lcd_rect(25, 57, 38, 7);
  lcd_filled_rect(26, 58, 9*frskyRSSI[1].value/25, 5);
  lcd_puts(105, 7*FH+1, STR_RX); lcd_outdezNAtt(105+4*FW-1, 7*FH+1, frskyRSSI[0].value, LEADING0, 2);
  lcd_rect(65, 57, 38, 7);
  uint8_t v = 9*frskyRSSI[0].value/25;
  lcd_filled_rect(66+36-v, 58, v, 5);
}
#endif

#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
void displayAltitudeLine(uint8_t x, uint8_t y, uint8_t flags)
{
  lcd_puts(x, y, STR_ALT);
  int16_t value = frskyHubData.baroAltitude + frskyHubData.baroAltitudeOffset;
  putsTelemetryValue(lcd_lastPos, y, value, UNIT_METERS, flags|LEFT);
}
#endif

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
#if defined(FRSKY)
#if defined(FRSKY_HUB) && defined(WS_HOW_HIGH)
      tabViews[e_telemetry] = (g_model.frsky.usrProto == 0 ? 3 : (g_model.frsky.usrProto == 1 ? 5 : 4));
#elif defined(FRSKY_HUB)
      tabViews[e_telemetry] = (g_model.frsky.usrProto == 1 ? 5 : 3);
#elif defined(WS_HOW_HIGH)
      tabViews[e_telemetry] = (g_model.frsky.usrProto == 2 ? 4 : 3);
#endif
#endif
      g_eeGeneral.view = (view + (event == EVT_KEY_BREAK(KEY_RIGHT) ? ALTERNATE_VIEW : tabViews[view_base]*ALTERNATE_VIEW-ALTERNATE_VIEW)) % (tabViews[view_base]*ALTERNATE_VIEW);
      eeDirty(EE_GENERAL);
      AUDIO_KEYPAD_UP();
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
      g_eeGeneral.view = view+1;
      if(g_eeGeneral.view>=MAX_VIEWS) g_eeGeneral.view=0;
      eeDirty(EE_GENERAL);
      AUDIO_KEYPAD_UP();
      break;
    case EVT_KEY_BREAK(KEY_DOWN):
      if(view>0)
        g_eeGeneral.view = view - 1;
      else
        g_eeGeneral.view = MAX_VIEWS-1;
      eeDirty(EE_GENERAL);
      AUDIO_KEYPAD_UP();
      break;
    case EVT_KEY_LONG(KEY_UP):
      chainMenu(menuProcStatistic);
      killEvents(event);
      break;
    case EVT_KEY_LONG(KEY_DOWN):
#if defined(JETI)
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
      else if (view == e_timer2) {
       resetTimer(1);
      }
#ifdef FRSKY
      else if (view_base == e_telemetry) {
        resetTelemetry();
      }
#endif
      else {
        resetTimer(0);
      }
      AUDIO_KEYPAD_UP();
      break;
    case EVT_KEY_LONG(KEY_EXIT):
      resetTimer(0);
      resetTimer(1);
#ifdef FRSKY
      resetTelemetry();
#endif
      AUDIO_KEYPAD_UP();
      break;
    case EVT_ENTRY:
      killEvents(KEY_EXIT);
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      instantTrimSwLock = true;
      trim2OfsSwLock = true;
      break;
  }

  bool trimSw = isFunctionActive(FUNC_INSTANT_TRIM);
  if (!instantTrimSwLock && trimSw) instantTrim();
  instantTrimSwLock = trimSw;
  
  trimSw = isFunctionActive(FUNC_TRIMS_2_OFS);
  if (!trim2OfsSwLock && trimSw) moveTrimsToOffsets();
  trim2OfsSwLock = trimSw;

#ifdef FRSKY
  if (view_base == e_telemetry && frskyStreaming && view > ALTERNATE_VIEW) {
    putsModelName(0, 0, g_model.name, g_eeGeneral.currModel, 0);
    uint8_t att = (g_vbat100mV < g_eeGeneral.vBatWarn ? BLINK : 0);
    putsVBat(14*FW,0,att);
    if (g_model.timer1.mode) {
      att = (s_timerState[0]==TMR_BEEPING ? BLINK : 0);
      putsTime(17*FW, 0, s_timerVal[0], att, att);
    }
    lcd_filled_rect(0, 0, DISPLAY_W, 8);
  }
  else
#endif
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
#if defined(FRSKY)
  else if (view_base == e_telemetry) {
    if (frskyStreaming) {
      uint8_t y0, x0, blink;

      if (view == e_telemetry+ALTERNATE_VIEW) {
        // The bars
        uint8_t bars_height = 5;
        for (int8_t i=3; i>=0; i--) {
          if (g_model.frsky.bars[i].source && (51-g_model.frsky.bars[i].barMax) > g_model.frsky.bars[i].barMin) {
            lcd_putsnAtt(0, bars_height+bars_height+1+i*(bars_height+6), STR_VTELEMBARS+LEN_VTELEMBARS*g_model.frsky.bars[i].source, LEN_VTELEMBARS, 0);
            lcd_rect(25, bars_height+6+i*(bars_height+6), 101, bars_height+2);
            int16_t value = getValue(CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS+g_model.frsky.bars[i].source-1);
            uint8_t threshold = 0, thresholdX = 0;
            if (g_model.frsky.bars[i].source <= 2)
              threshold = g_model.frsky.channels[g_model.frsky.bars[i].source-1].alarms_value[0];
            else
              threshold = barsThresholds[g_model.frsky.bars[i].source-3];
            if (threshold) {
              thresholdX = (uint8_t)(int16_t)((int16_t)100 * (threshold - g_model.frsky.bars[i].barMin * 4) / ((51 - g_model.frsky.bars[i].barMax) * 5 - g_model.frsky.bars[i].barMin * 4));
              if (thresholdX > 100)
                thresholdX = 0;
            }
            uint8_t width = (uint8_t)limit((int16_t)0, (int16_t)((int16_t)100 * (value - g_model.frsky.bars[i].barMin * 5) / ((51 - g_model.frsky.bars[i].barMax) * 4 - g_model.frsky.bars[i].barMin * 4)), (int16_t)100);
            lcd_filled_rect(26, bars_height+6+1+i*(bars_height+6), width, bars_height, (threshold > value) ? DOTTED : SOLID);
            for (uint8_t j=50; j<125; j+=25)
              if (j>26+thresholdX) lcd_vline(j, bars_height+6+1+i*(bars_height+6), bars_height);
            if (thresholdX) {
              lcd_vlineStip(26+thresholdX, bars_height+4+i*(bars_height+6), bars_height+3, DOTTED);
              lcd_hline(25+thresholdX, bars_height+4+i*(bars_height+6), 3);
            }
          }
          else {
            bars_height += 2;
          }
        }

        displayRssiLine();
      }
      else if (view == e_telemetry+2*ALTERNATE_VIEW) {
        // Big A1 / A2 with min and max
        if (g_model.frsky.channels[0].ratio || g_model.frsky.channels[1].ratio) {
          x0 = 0;
          for (uint8_t i=0; i<2; i++) {
            if (g_model.frsky.channels[i].ratio) {
              blink = (FRSKY_alarmRaised(i) ? INVERS : 0);
              displayA1A2(x0, 2*FH, i);
              x0 += 3*FW;
              putsTelemetryChannel(x0, 2*FH, i, frskyTelemetry[i].value, blink|DBLSIZE|LEFT);
              putsTelemetryChannel(x0+FW, 3*FH, i, frskyTelemetry[i].min, 0);
              putsTelemetryChannel(x0+3*FW, 3*FH, i, frskyTelemetry[i].max, LEFT);
              x0 = 11*FW-2;
            }
          }
        }

        // Cells voltage
        lcd_putsLeft( 5*FH-3, PSTR("V1=\004V2=\004V3=")) ;
        lcd_putsLeft( 6*FH-3, PSTR("V4=\004V5=\004V6="));
        {
          uint8_t x, y;
          x = 6*FW;
          y = 6*FH-3;
          for (uint8_t k=0; k<frskyHubData.cellsCount && k<6; k++) {
            uint8_t attr = (barsThresholds[6/*TODO constant*/] && frskyHubData.cellVolts[k] < barsThresholds[6/*TODO constant*/]) ? BLINK|PREC2 : PREC2;
            if (k == 3) {
              x = 6*FW;
              y = 7*FH-3;
            }
            lcd_outdezNAtt(x, y, frskyHubData.cellVolts[k] * 2, attr, 4);
            x += 7*FW;
          }
        }

        displayRssiLine();
      }

#ifdef WS_HOW_HIGH
      else if (g_model.frsky.usrProto == PROTO_WS_HOW_HIGH && view == e_telemetry+3*ALTERNATE_VIEW) {
        displayAltitudeLine(0, 4*FH, DBLSIZE);
        displayRssiLine();
      }
#endif
#ifdef FRSKY_HUB
      else if (g_model.frsky.usrProto == PROTO_FRSKY_HUB && view == e_telemetry+4*ALTERNATE_VIEW) {
#define DATE_LINE (7*FH+1)
        //lcd_putsLeft( DATE_LINE, PSTR("D/T:"));
        // Date
        lcd_outdezNAtt(3*FW, DATE_LINE, frskyHubData.year+2000, LEFT, 4);
        lcd_putc(7*FW-4, DATE_LINE, '-');
        lcd_outdezNAtt(8*FW-4, DATE_LINE, frskyHubData.month, LEFT|LEADING0, 2);
        lcd_putc(10*FW-6, DATE_LINE, '-');
        lcd_outdezNAtt(11*FW-6, DATE_LINE, frskyHubData.day, LEFT|LEADING0, 2);

        // Time
        lcd_outdezNAtt(12*FW+5, DATE_LINE, frskyHubData.hour, LEFT|LEADING0, 2);
        lcd_putc(14*FW+2, DATE_LINE, ':');
        lcd_outdezNAtt(15*FW+2, DATE_LINE, frskyHubData.min, LEFT|LEADING0, 2);
        lcd_putc(17*FW-1, DATE_LINE, ':');
        lcd_outdezNAtt(18*FW-1, DATE_LINE, frskyHubData.sec, LEFT|LEADING0, 2);
        lcd_filled_rect(0, DATE_LINE-1, DISPLAY_W, 8);

        // Latitude
#define LAT_LINE (2*FH-4)
        lcd_putsLeft( LAT_LINE, PSTR("Lat:"));
        lcd_outdezAtt(lcd_lastPos, LAT_LINE,  frskyHubData.gpsLatitude_bp / 100, LEFT); // ddd before '.'
        lcd_putc(lcd_lastPos, LAT_LINE, '@');
        uint8_t mn = frskyHubData.gpsLatitude_bp % 100;
        lcd_outdezNAtt(lcd_lastPos+FW, LAT_LINE, mn, LEFT|LEADING0, 2); // mm before '.'
        lcd_plot(lcd_lastPos, LAT_LINE+FH-2, 0); // small decimal point
        lcd_outdezNAtt(lcd_lastPos+2, LAT_LINE, frskyHubData.gpsLatitude_ap, LEFT|UNSIGN|LEADING0, 4); // after '.'
        lcd_putc(lcd_lastPos+1, LAT_LINE, frskyHubData.gpsLatitudeNS ? frskyHubData.gpsLatitudeNS : '-');

        // Longitude
#define LONG_LINE (3*FH-3)
        lcd_putsLeft(LONG_LINE, PSTR("Lon:"));
        lcd_outdezAtt(lcd_lastPos, LONG_LINE,  frskyHubData.gpsLongitude_bp / 100, LEFT); // ddd before '.'
        lcd_putc(lcd_lastPos, LONG_LINE, '@');
        mn = frskyHubData.gpsLongitude_bp % 100;
        lcd_outdezNAtt(lcd_lastPos+FW, LONG_LINE, mn, LEFT|LEADING0, 2); // mm before '.'
        lcd_plot(lcd_lastPos, LONG_LINE+FH-2, 0); // small decimal point
        lcd_outdezNAtt(lcd_lastPos+2, LONG_LINE, frskyHubData.gpsLongitude_ap, LEFT|UNSIGN|LEADING0, 4); // after '.'
        lcd_putc(lcd_lastPos+1, LONG_LINE, frskyHubData.gpsLongitudeEW ? frskyHubData.gpsLongitudeEW : '-');

#if 0
        // Course / Heading
        lcd_puts(5, 5*FH, STR_HDG);
        lcd_outdezNAtt(lcd_lastPos, 5*FH, frskyHubData.gpsCourse_bp, LEFT|LEADING0, 3); // before '.'
        lcd_plot(lcd_lastPos, 6*FH-2, 0); // small decimal point
        lcd_outdezAtt(lcd_lastPos+2, 5*FH, frskyHubData.gpsCourse_ap, LEFT); // after '.'
        lcd_putc(lcd_lastPos, 5*FH, '@');
#endif

        // Speed
#define SPEED_LINE (6*FH)
        lcd_putsLeft(SPEED_LINE, STR_SPDnMAX);
        putsTelemetryValue(4*FW, SPEED_LINE, frskyHubData.gpsSpeed_bp, UNIT_KTS, LEFT|DBLSIZE); // before '.'
        putsTelemetryValue(16*FW, SPEED_LINE, frskyHubData.maxGpsSpeed, UNIT_KTS, LEFT); // before '.'

        // Altitude
#define ALTITUDE_LINE (4*FH-1)
        lcd_putsLeft(ALTITUDE_LINE, STR_ALT);
        putsTelemetryValue(4*FW, ALTITUDE_LINE, frskyHubData.gpsAltitude_bp, UNIT_METERS, LEFT); // before '.'
      }
      else if (g_model.frsky.usrProto == PROTO_FRSKY_HUB && view == e_telemetry+3*ALTERNATE_VIEW) {
        // Temperature 1
        lcd_putsLeft( 4*FH, STR_TEMP1nTEMP2);
        putsTelemetryValue(4*FW, 4*FH, frskyHubData.temperature1, UNIT_DEGREES, DBLSIZE|LEFT);

        // Temperature 2
        putsTelemetryValue(15*FW, 4*FH, frskyHubData.temperature2, UNIT_DEGREES, DBLSIZE|LEFT);

        // RPM
        lcd_putsLeft(2*FH, STR_RPMnFUEL);
        lcd_outdezNAtt(4*FW, 1*FH, frskyHubData.rpm, DBLSIZE|LEFT);

        // Fuel
        putsTelemetryValue(15*FW, 2*FH, frskyHubData.fuelLevel, UNIT_PERCENT, DBLSIZE|LEFT);

        // Altitude (barometric)
        displayAltitudeLine(0, 6*FH, DBLSIZE);

        // Accelerometer
#define ACC_LINE (7*FH+1)
        lcd_putsLeft(ACC_LINE, STR_ACCEL);
        // lcd_puts(4*FW, 7*FH, PSTR("x:"));
        lcd_outdezNAtt(4*FW, ACC_LINE, (int32_t)frskyHubData.accelX * 100 / 256, LEFT|PREC2);
        // lcd_putc(lcd_lastPos, 7*FH, 'g');
        // lcd_puts(11*FW, 7*FH, PSTR("y:"));
        lcd_outdezNAtt(10*FW, ACC_LINE, (int32_t)frskyHubData.accelY * 100 / 256, LEFT|PREC2);
        // lcd_putc(lcd_lastPos, 7*FH, 'g');
        // lcd_puts(18*FW, 7*FH, PSTR("z:"));
        lcd_outdezNAtt(16*FW, ACC_LINE, (int32_t)frskyHubData.accelZ * 100 / 256, LEFT|PREC2);
        // lcd_putc(lcd_lastPos, 7*FH, 'g');
        lcd_filled_rect(0, ACC_LINE-1, DISPLAY_W, 8);
      }
#endif
      else {
        y0 = 5*FH;
        x0 = 2*FW-3;
        for (int i=0; i<2; i++) {
          if (g_model.frsky.channels[i].ratio) {
            blink = (FRSKY_alarmRaised(i) ? INVERS+BLINK : 0)|LEFT;
            displayA1A2(x0, y0, i);
            putsTelemetryChannel(x0+3*FW, y0, i, frskyTelemetry[i].value, blink);
            x0 = 12*FW-3;
          }
        }
        y0+=FH;
        lcd_puts(2*FW-3, y0, STR_RX);
        lcd_outdezAtt(5*FW-3, y0, frskyRSSI[0].value, LEFT);
        lcd_puts(12*FW-3, y0, STR_TX);
        lcd_outdezAtt(15*FW-3, y0, frskyRSSI[1].value, LEFT);
      }
    }
    else {
      lcd_putsAtt(22, 40, STR_NODATA, DBLSIZE);
    }
  }
#endif
  else { // timer2
    putsTime(33+FW+2, FH*5, s_timerVal[1], DBLSIZE, DBLSIZE);
    putsTmrMode(s_timerVal[1] >= 0 ? 20-FW/2+5 : 20-FW/2-2, FH*6, g_model.timer2.mode, SHRT_TM_MODE);
    // lcd_outdezNAtt(33+11*FW, FH*6, s_timerVal_10ms[1], LEADING0, 2); // 1/100s
  }

  theFile.DisplayProgressBar(20*FW+1);
}
