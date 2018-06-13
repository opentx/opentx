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

#define BIGSIZE       DBLSIZE
#if defined (PCBTARANIS)
  #define LBOX_CENTERX  (LCD_W/4 + 14)
  #define RBOX_CENTERX  (3*LCD_W/4 - 13)
#else
  #define LBOX_CENTERX  (LCD_W/4 + 10)
  #define RBOX_CENTERX  (3*LCD_W/4 - 10)
#endif
#define MODELNAME_X   (2*FW-2)
#define MODELNAME_Y   (0)
#define PHASE_X       (6*FW-2)
#define PHASE_Y       (2*FH)
#define PHASE_FLAGS   0
#define VBATT_X       (6*FW-1)
#define VBATT_Y       (2*FH)
#define VBATTUNIT_X   (VBATT_X-1)
#define VBATTUNIT_Y   (3*FH)
#define REBOOT_X      (20*FW-3)
#define BAR_HEIGHT    (BOX_WIDTH-1l) // don't remove the l here to force 16bits maths on 9X
#define TRIM_LH_X     (LCD_W*1/4+2)
#define TRIM_LV_X     3
#define TRIM_RV_X     (LCD_W-4)
#define TRIM_RH_X     (LCD_W*3/4-2)
#define TRIM_LH_NEG   (TRIM_LH_X+1*FW)
#define TRIM_LH_POS   (TRIM_LH_X-4*FW)
#define TRIM_RH_NEG   (TRIM_RH_X+1*FW)
#define TRIM_RH_POS   (TRIM_RH_X-4*FW)
#if defined(TELEMETRY_FRSKY) && defined(CPUARM)
#define RSSSI_X       (30)
#define RSSSI_Y       (31)
#define RSSI_MAX      105
#endif

#define TRIM_LEN      23

#if defined(TELEMETRY_FRSKY) && defined(CPUARM)
void drawRSSIGauge()
{
  uint8_t bar = (RSSI_MAX - g_model.rssiAlarms.getWarningRssi()) / 4;

  for(uint8_t i=1; i<5;  i++) {
    if((TELEMETRY_RSSI() - g_model.rssiAlarms.getWarningRssi()) > bar*(i-1)) {
      lcdDrawFilledRect(RSSSI_X + i*4, RSSSI_Y - 2*i, 3, 2*i, SOLID, 0);
    }
  }
}
#endif

void drawPotsBars()
{
  // Optimization by Mike Blandford
  for (uint8_t x=LCD_W/2 - (NUM_POTS+NUM_SLIDERS-1) * 5 / 2, i=NUM_STICKS; i<NUM_STICKS+NUM_POTS+NUM_SLIDERS; x+=5, i++) {
    if (IS_POT_SLIDER_AVAILABLE(i)) {
      uint8_t len = ((calibratedAnalogs[i]+RESX)*BAR_HEIGHT/(RESX*2))+1l;  // calculate once per loop
      V_BAR(x, LCD_H-8, len);
    }
  }
}

void doMainScreenGraphics()
{
  int16_t calibStickVert = calibratedAnalogs[CONVERT_MODE(1)];
  if (g_model.throttleReversed && CONVERT_MODE(1) == THR_STICK)
    calibStickVert = -calibStickVert;
  drawStick(LBOX_CENTERX, calibratedAnalogs[CONVERT_MODE(0)], calibStickVert);

  calibStickVert = calibratedAnalogs[CONVERT_MODE(2)];
  if (g_model.throttleReversed && CONVERT_MODE(2) == THR_STICK)
    calibStickVert = -calibStickVert;
  drawStick(RBOX_CENTERX, calibratedAnalogs[CONVERT_MODE(3)], calibStickVert);

  drawPotsBars();
}

void displayTrims(uint8_t phase)
{
  for (uint8_t i=0; i<4; i++) {
    static coord_t x[4] = {TRIM_LH_X, TRIM_LV_X, TRIM_RV_X, TRIM_RH_X};
    static uint8_t vert[4] = {0,1,1,0};
    coord_t xm, ym;
    uint8_t stickIndex = CONVERT_MODE(i);
    xm = x[stickIndex];
    uint8_t att = ROUND;
    int16_t val = getTrimValue(phase, i);

#if defined(CPUARM)
    if(getRawTrimValue(phase, i).mode == TRIM_MODE_NONE)
      continue;
#endif

#if !defined(CPUM64) || !defined(TELEMETRY_FRSKY)
    int16_t dir = val;
    bool exttrim = false;
    if (val < TRIM_MIN || val > TRIM_MAX) {
      exttrim = true;
    }
#endif
    if (val < -(TRIM_LEN+1)*4) {
      val = -(TRIM_LEN+1);
    }
    else if (val > (TRIM_LEN+1)*4) {
      val = TRIM_LEN+1;
    }
    else {
      val /= 4;
    }

    if (vert[i]) {
      ym = 31;
      lcdDrawSolidVerticalLine(xm, ym-TRIM_LEN, TRIM_LEN*2);
      if (i!=2 || !g_model.thrTrim) {
        lcdDrawSolidVerticalLine(xm-1, ym-1,  3);
        lcdDrawSolidVerticalLine(xm+1, ym-1,  3);
      }
      ym -= val;
#if !defined(CPUM64) || !defined(TELEMETRY_FRSKY)
      lcdDrawFilledRect(xm-3, ym-3, 7, 7, SOLID, att|ERASE);
      if (dir >= 0) {
        lcdDrawSolidHorizontalLine(xm-1, ym-1,  3);
      }
      if (dir <= 0) {
        lcdDrawSolidHorizontalLine(xm-1, ym+1,  3);
      }
      if (exttrim) {
        lcdDrawSolidHorizontalLine(xm-1, ym,  3);
      }
#endif
#if defined(CPUARM)
      if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && dir != 0) {
        if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
          lcdDrawNumber(dir>0 ? 12 : 40, xm-2, -abs(dir/5), TINSIZE|VERTICAL);
        }
      }
#endif
    }
    else {
      ym = 60;
      lcdDrawSolidHorizontalLine(xm-TRIM_LEN, ym, TRIM_LEN*2);
      lcdDrawSolidHorizontalLine(xm-1, ym-1,  3);
      lcdDrawSolidHorizontalLine(xm-1, ym+1,  3);
      xm += val;
#if !defined(CPUM64) || !defined(TELEMETRY_FRSKY)
      lcdDrawFilledRect(xm-3, ym-3, 7, 7, SOLID, att|ERASE);
      if (dir >= 0) {
        lcdDrawSolidVerticalLine(xm+1, ym-1,  3);
      }
      if (dir <= 0) {
        lcdDrawSolidVerticalLine(xm-1, ym-1,  3);
      }
      if (exttrim) {
        lcdDrawSolidVerticalLine(xm, ym-1,  3);
      }
#endif
#if defined(CPUARM)
      if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && dir != 0) {
        if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
          lcdDrawNumber((stickIndex==0 ? (dir>0 ? TRIM_LH_POS : TRIM_LH_NEG) : (dir>0 ? TRIM_RH_POS : TRIM_RH_NEG)), ym-2, -abs(dir/5), TINSIZE);
        }
      }
#endif
    }
    lcdDrawSquare(xm-3, ym-3, 7, att);
  }
}

FORCEINLINE void drawTimerWithMode(coord_t x, coord_t y, uint8_t index)
{
  const TimerData & timer = g_model.timers[index];
  if (timer.mode) {
    const TimerState & timerState = timersStates[index];
    const uint8_t negative = (timerState.val<0 ? BLINK | INVERS : 0);
    LcdFlags att = RIGHT | DBLSIZE | negative;
    drawTimer(x, y, timerState.val, att, att);
#if defined(CPUARM)
    uint8_t xLabel = (negative ? x-56 : x-49);
    uint8_t len = zlen(timer.name, LEN_TIMER_NAME);
    if (len > 0) {
      lcdDrawSizedText(xLabel, y+FH, timer.name, len, RIGHT | ZCHAR);
    }
    else {
      drawTimerMode(xLabel, y+FH, timer.mode, RIGHT);
    }
#else
    uint8_t xLabel = (negative ? x-76 : x-69);
    drawTimerMode(xLabel, y+FH, timer.mode);
#endif
  }
}

void displayBattVoltage()
{
#if defined(BATTGRAPH)
  putsVBat(VBATT_X-8, VBATT_Y+1, RIGHT);
  lcdDrawSolidFilledRect(VBATT_X-25, VBATT_Y+9, 21, 5);
  lcdDrawSolidVerticalLine(VBATT_X-4, VBATT_Y+10, 3);
  uint8_t count = GET_TXBATT_BARS();
  for (uint8_t i=0; i<count; i+=2)
    lcdDrawSolidVerticalLine(VBATT_X-24+i, VBATT_Y+10, 3);
  if (!IS_TXBATT_WARNING() || BLINK_ON_PHASE)
    lcdDrawSolidFilledRect(VBATT_X-26, VBATT_Y, 24, 15);
#else
  LcdFlags att = (IS_TXBATT_WARNING() ? BLINK|INVERS : 0) | BIGSIZE;
  putsVBat(VBATT_X-1, VBATT_Y, att|NO_UNIT);
  lcdDrawChar(VBATT_X, VBATTUNIT_Y, 'V');
#endif
}

#if defined(PCBSKY9X)
void displayVoltageOrAlarm()
{
  if (g_eeGeneral.temperatureWarn && getTemperature() >= g_eeGeneral.temperatureWarn) {
    drawValueWithUnit(6*FW-1, 2*FH, getTemperature(), UNIT_TEMPERATURE, BLINK|INVERS|DBLSIZE|RIGHT);
  }
  else if (g_eeGeneral.mAhWarn && (g_eeGeneral.mAhUsed + Current_used * (488 + g_eeGeneral.txCurrentCalibration)/8192/36) / 500 >= g_eeGeneral.mAhWarn) {
    drawValueWithUnit(7*FW-1, 2*FH, (g_eeGeneral.mAhUsed + Current_used*(488 + g_eeGeneral.txCurrentCalibration)/8192/36)/10, UNIT_MAH, BLINK|INVERS|DBLSIZE|RIGHT);
  }
  else {
    displayBattVoltage();
  }
}
#else
  #define displayVoltageOrAlarm() displayBattVoltage()
#endif

#if defined(PCBX7)
#define EVT_KEY_CONTEXT_MENU           EVT_KEY_LONG(KEY_ENTER)
#define EVT_KEY_NEXT_VIEW              EVT_KEY_BREAK(KEY_PAGE)
#define EVT_KEY_NEXT_PAGE              EVT_ROTARY_RIGHT
#define EVT_KEY_PREVIOUS_PAGE          EVT_ROTARY_LEFT
#define EVT_KEY_MODEL_MENU             EVT_KEY_BREAK(KEY_MENU)
#define EVT_KEY_GENERAL_MENU           EVT_KEY_LONG(KEY_MENU)
#define EVT_KEY_TELEMETRY              EVT_KEY_LONG(KEY_PAGE)
#elif defined(PCBXLITE)
#define EVT_KEY_CONTEXT_MENU           EVT_KEY_LONG(KEY_ENTER)
#define EVT_KEY_PREVIOUS_VIEW          EVT_KEY_BREAK(KEY_UP)
#define EVT_KEY_NEXT_VIEW              EVT_KEY_BREAK(KEY_DOWN)
#define EVT_KEY_NEXT_PAGE              EVT_KEY_BREAK(KEY_RIGHT)
#define EVT_KEY_PREVIOUS_PAGE          EVT_KEY_BREAK(KEY_LEFT)
#define EVT_KEY_MODEL_MENU             EVT_KEY_LONG(KEY_RIGHT)
#define EVT_KEY_GENERAL_MENU           EVT_KEY_LONG(KEY_LEFT)
#define EVT_KEY_TELEMETRY              EVT_KEY_LONG(KEY_DOWN)
#define EVT_KEY_STATISTICS             EVT_KEY_LONG(KEY_UP)
#else
#define EVT_KEY_CONTEXT_MENU           EVT_KEY_BREAK(KEY_MENU)
#define EVT_KEY_PREVIOUS_VIEW          EVT_KEY_BREAK(KEY_UP)
#define EVT_KEY_NEXT_VIEW              EVT_KEY_BREAK(KEY_DOWN)
#define EVT_KEY_NEXT_PAGE              EVT_KEY_BREAK(KEY_RIGHT)
#define EVT_KEY_PREVIOUS_PAGE          EVT_KEY_BREAK(KEY_LEFT)
#define EVT_KEY_MODEL_MENU             EVT_KEY_LONG(KEY_RIGHT)
#define EVT_KEY_GENERAL_MENU           EVT_KEY_LONG(KEY_LEFT)
#define EVT_KEY_LAST_MENU              EVT_KEY_LONG(KEY_MENU)
#define EVT_KEY_TELEMETRY              EVT_KEY_LONG(KEY_DOWN)
#define EVT_KEY_STATISTICS             EVT_KEY_LONG(KEY_UP)
#endif

#if defined(NAVIGATION_MENUS)
void onMainViewMenu(const char *result)
{
  if (result == STR_RESET_TIMER1) {
    timerReset(0);
  }
  else if (result == STR_RESET_TIMER2) {
    timerReset(1);
  }
#if TIMERS > 2
  else if (result == STR_RESET_TIMER3) {
    timerReset(2);
  }
#endif
#if defined(CPUARM)
  else if (result == STR_VIEW_NOTES) {
    pushModelNotes();
  }
  else if (result == STR_RESET_SUBMENU) {
    POPUP_MENU_ADD_ITEM(STR_RESET_FLIGHT);
    POPUP_MENU_ADD_ITEM(STR_RESET_TIMER1);
    POPUP_MENU_ADD_ITEM(STR_RESET_TIMER2);
    POPUP_MENU_ADD_ITEM(STR_RESET_TIMER3);
#if defined(TELEMETRY_FRSKY)
    POPUP_MENU_ADD_ITEM(STR_RESET_TELEMETRY);
#endif
  }
#endif
#if defined(TELEMETRY_FRSKY)
  else if (result == STR_RESET_TELEMETRY) {
    telemetryReset();
  }
#endif
  else if (result == STR_RESET_FLIGHT) {
    flightReset();
  }
  else if (result == STR_STATISTICS) {
    chainMenu(menuStatisticsView);
  }
#if defined(CPUARM)
  else if (result == STR_ABOUT_US) {
    chainMenu(menuAboutView);
  }
#endif
}
#endif

void menuMainView(event_t event)
{
  STICK_SCROLL_DISABLE();

  uint8_t view = g_eeGeneral.view;
  uint8_t view_base = view & 0x0f;

  switch (event) {
    case EVT_ENTRY:
      killEvents(KEY_EXIT);
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      break;

    /* TODO if timer2 is OFF, it's possible to use this timer2 as in er9x...
    case EVT_KEY_BREAK(KEY_MENU):
      if (view_base == VIEW_TIMER2) {
        Timer2_running = !Timer2_running;
        AUDIO_KEY_PRESS();
      }
    break;
    */

    case EVT_KEY_NEXT_PAGE:
    case EVT_KEY_PREVIOUS_PAGE:
      if (view_base <= VIEW_INPUTS) {
#if defined(CPUARM)
        if (view_base == VIEW_INPUTS)
          g_eeGeneral.view ^= ALTERNATE_VIEW;
        else
          g_eeGeneral.view = (g_eeGeneral.view + (4*ALTERNATE_VIEW) + ((event==EVT_KEY_PREVIOUS_PAGE) ? -ALTERNATE_VIEW : ALTERNATE_VIEW)) % (4*ALTERNATE_VIEW);
#else
        g_eeGeneral.view ^= ALTERNATE_VIEW;
#endif
        storageDirty(EE_GENERAL);
        AUDIO_KEY_PRESS();
      }
      break;

#if defined(NAVIGATION_MENUS)
    case EVT_KEY_CONTEXT_MENU:
      killEvents(event);

#if defined(CPUARM)
      if (modelHasNotes()) {
        POPUP_MENU_ADD_ITEM(STR_VIEW_NOTES);
      }
#endif

#if defined(CPUARM)
      POPUP_MENU_ADD_ITEM(STR_RESET_SUBMENU);
#else
      POPUP_MENU_ADD_ITEM(STR_RESET_TIMER1);
      POPUP_MENU_ADD_ITEM(STR_RESET_TIMER2);
#if defined(TELEMETRY_FRSKY)
      POPUP_MENU_ADD_ITEM(STR_RESET_TELEMETRY);
#endif
      POPUP_MENU_ADD_ITEM(STR_RESET_FLIGHT);
#endif

      POPUP_MENU_ADD_ITEM(STR_STATISTICS);
#if defined(CPUARM)
      POPUP_MENU_ADD_ITEM(STR_ABOUT_US);
#endif
      POPUP_MENU_START(onMainViewMenu);
      break;
#endif

#if MENUS_LOCK != 2 /*no menus*/
#if defined(EVT_KEY_LAST_MENU)
    case EVT_KEY_LAST_MENU:
      pushMenu(lastPopMenu());
      killEvents(event);
      break;
#endif

    CASE_EVT_ROTARY_BREAK
    case EVT_KEY_MODEL_MENU:
      pushMenu(menuModelSelect);
      killEvents(event);
      break;

    CASE_EVT_ROTARY_LONG
    case EVT_KEY_GENERAL_MENU:
      pushMenu(menuRadioSetup);
      killEvents(event);
      break;
#endif

#if defined(EVT_KEY_PREVIOUS_VIEW)
      // TODO try to split those 2 cases on 9X
    case EVT_KEY_PREVIOUS_VIEW:
    case EVT_KEY_NEXT_VIEW:
      // TODO try to split those 2 cases on 9X
      g_eeGeneral.view = (event == EVT_KEY_PREVIOUS_VIEW ? (view_base == VIEW_COUNT-1 ? 0 : view_base+1) : (view_base == 0 ? VIEW_COUNT-1 : view_base-1));
      storageDirty(EE_GENERAL);
      break;
#else
    case EVT_KEY_NEXT_VIEW:
      g_eeGeneral.view = (view_base == 0 ? VIEW_COUNT-1 : view_base-1);
      storageDirty(EE_GENERAL);
      break;
#endif

#if defined(EVT_KEY_STATISTICS)
    case EVT_KEY_STATISTICS:
      chainMenu(menuStatisticsView);
      killEvents(event);
      break;
#endif

    case EVT_KEY_TELEMETRY:
#if defined(TELEMETRY_FRSKY)
      chainMenu(menuViewTelemetryFrsky);
#elif defined(TELEMETRY_JETI)
      JETI_EnableRXD(); // enable JETI-Telemetry reception
      chainMenu(menuViewTelemetryJeti);
#elif defined(TELEMETRY_ARDUPILOT)
      ARDUPILOT_EnableRXD(); // enable ArduPilot-Telemetry reception
      chainMenu(menuViewTelemetryArduPilot);
#elif defined(TELEMETRY_NMEA)
      NMEA_EnableRXD(); // enable NMEA-Telemetry reception
      chainMenu(menuViewTelemetryNMEA);
#elif defined(TELEMETRY_MAVLINK)
      chainMenu(menuViewTelemetryMavlink);
#else
      chainMenu(menuStatisticsDebug);
#endif
      killEvents(event);
      break;

    case EVT_KEY_FIRST(KEY_EXIT):
#if defined(GVARS) && !defined(PCBSTD)
      if (gvarDisplayTimer > 0) {
        gvarDisplayTimer = 0;
      }
#endif
#if !defined(NAVIGATION_MENUS)
      if (view == VIEW_TIMER2) {
        timerReset(1);
      }
#endif
      break;

#if !defined(NAVIGATION_MENUS)
    case EVT_KEY_LONG(KEY_EXIT):
      flightReset();
      break;
#endif
  }

  {
    // Flight Mode Name
    uint8_t mode = mixerCurrentFlightMode;
    lcdDrawSizedText(PHASE_X, PHASE_Y, g_model.flightModeData[mode].name, sizeof(g_model.flightModeData[mode].name), ZCHAR|PHASE_FLAGS);

    // Model Name
    putsModelName(MODELNAME_X, MODELNAME_Y, g_model.header.name, g_eeGeneral.currModel, BIGSIZE);

    // Main Voltage (or alarm if any)
    displayVoltageOrAlarm();

    // Timer 1
    drawTimerWithMode(125, 2*FH, 0);

    // Trims sliders
    displayTrims(mode);

#if defined(TELEMETRY_FRSKY) && defined(CPUARM)
    // RSSI gauge
    if (TELEMETRY_RSSI() > 0) {
      drawRSSIGauge();
    }
#endif
  }

  if (view_base < VIEW_INPUTS) {
    // scroll bar
    lcdDrawHorizontalLine(38, 34, 54, DOTTED);
#if defined(CPUARM)
    lcdDrawSolidHorizontalLine(38 + (g_eeGeneral.view / ALTERNATE_VIEW) * 13, 34, 13, SOLID);
#else
    lcdDrawSolidHorizontalLine((g_eeGeneral.view & ALTERNATE_VIEW) ? 64 : 38, 34, 26, SOLID);
#endif

    for (uint8_t i=0; i<8; i++) {
      uint8_t x0,y0;
#if defined(CPUARM)
      uint8_t chan = 8*(g_eeGeneral.view / ALTERNATE_VIEW) + i;
#else
      uint8_t chan = (g_eeGeneral.view & ALTERNATE_VIEW) ? 8+i : i;
#endif

      int16_t val = channelOutputs[chan];

      switch (view_base) {
        case VIEW_OUTPUTS_VALUES:
          x0 = (i%4*9+3)*FW/2;
          y0 = i/4*FH+40;
#if defined(PPM_UNIT_US)
          lcdDrawNumber(x0+4*FW , y0, PPM_CH_CENTER(chan)+val/2, RIGHT);
#elif defined(PPM_UNIT_PERCENT_PREC1)
          lcdDrawNumber(x0+4*FW , y0, calcRESXto1000(val), RIGHT|PREC1);
#else
          lcdDrawNumber(x0+4*FW , y0, calcRESXto1000(val)/10, RIGHT); // G: Don't like the decimal part*
#endif
          break;

        case VIEW_OUTPUTS_BARS:
#define WBAR2 (50/2)
          x0 = i<4 ? LCD_W/4+2 : LCD_W*3/4-2;
          y0 = 38+(i%4)*5;

          const uint16_t lim = (g_model.extendedLimits ? (512 * (long)LIMIT_EXT_PERCENT / 100) : 512) * 2;
          int8_t len = (abs(val) * WBAR2 + lim/2) / lim;

          if (len>WBAR2)
            len = WBAR2; // prevent bars from going over the end - comment for debugging
          lcdDrawHorizontalLine(x0-WBAR2, y0, WBAR2*2+1, DOTTED);
          lcdDrawSolidVerticalLine(x0, y0-2,5 );
          if (val > 0)
            x0 += 1;
          else
            x0 -= len;
          lcdDrawSolidHorizontalLine(x0, y0+1, len);
          lcdDrawSolidHorizontalLine(x0, y0-1, len);
          break;
      }
    }
  }
  else if (view_base == VIEW_INPUTS) {
    if (view == VIEW_INPUTS) {
      // Sticks + Pots
      doMainScreenGraphics();

      // Switches
#if defined(PCBTARANIS)
      for (int i=0; i<NUM_SWITCHES; ++i) {
        if (SWITCH_EXISTS(i)) {
          uint8_t x = 2*FW-2, y = 4*FH+i*FH+1;
          if (i >= NUM_SWITCHES/2) {
            x = 16*FW+1;
            y -= (NUM_SWITCHES/2)*FH;
          }
          getvalue_t val = getValue(MIXSRC_FIRST_SWITCH+i);
          getvalue_t sw = ((val < 0) ? 3*i+1 : ((val == 0) ? 3*i+2 : 3*i+3));
          drawSwitch(x, y, sw, 0);
        }
      }
#else
      // The ID0 3-POS switch is merged with the TRN switch
      for (uint8_t i=SWSRC_THR; i<=SWSRC_TRN; i++) {
        int8_t sw = (i == SWSRC_TRN ? (switchState(SW_ID0) ? SWSRC_ID0 : (switchState(SW_ID1) ? SWSRC_ID1 : SWSRC_ID2)) : i);
        uint8_t x = 2*FW-2, y = i*FH+1;
        if (i >= SWSRC_AIL) {
          x = 17*FW-1;
          y -= 3*FH;
        }
        drawSwitch(x, y, sw, getSwitch(i) ? INVERS : 0);
      }
#endif
    }
    else {
#if defined(PCBMEGA2560) && defined(ROTARY_ENCODERS)
      for (uint8_t i=0; i<NUM_ROTARY_ENCODERS; i++) {
        int16_t val = getRotaryEncoder(i);
        int8_t len = limit((int16_t)0, (int16_t)(((val+1024) * BAR_HEIGHT) / 2048), (int16_t)BAR_HEIGHT);
#if ROTARY_ENCODERS > 2
#define V_BAR_W 5
        V_BAR(LCD_W/2-8+V_BAR_W*i, LCD_H-8, len);
#else
#define V_BAR_W 5
        V_BAR(LCD_W/2-3+V_BAR_W*i, LCD_H-8, len);
#endif
      }
#endif // PCBGRUVIN9X && ROTARY_ENCODERS

      // Logical Switches
#if defined(CPUARM)
      uint8_t index = 0;
      uint8_t y = LCD_H-20;
      for (uint8_t line=0; line<2; line++) {
        for (uint8_t column=0; column<MAX_LOGICAL_SWITCHES/2; column++) {
          int8_t len = getSwitch(SWSRC_SW1+index) ? 10 : 1;
          uint8_t x = (16 + 3*column);
          lcdDrawSolidVerticalLine(x-1, y-len, len);
          lcdDrawSolidVerticalLine(x,   y-len, len);
          index++;
        }
        y += 12;
      }
#elif defined(CPUM2560)
      for (uint8_t i=0; i<MAX_LOGICAL_SWITCHES; i++) {
        drawSwitch(2*FW-3 + (i/3)*(i/3>2 ? 3*FW+2 : (3*FW-1)) + (i/3>2 ? 2*FW : 0), 4*FH+1 + (i%3)*FH, SWSRC_SW1+i, getSwitch(SWSRC_SW1+i) ? INVERS : 0);
      }
#elif !defined(PCBSTD)
      for (uint8_t i=0; i<MAX_LOGICAL_SWITCHES; i++) {
        drawSwitch(2*FW-2 + (i/3)*(4*FW-1), 4*FH+1 + (i%3)*FH, SWSRC_SW1+i, getSwitch(SWSRC_SW1+i) ? INVERS : 0);
      }
#else
      for (uint8_t i=0; i<MAX_LOGICAL_SWITCHES; i++) {
        drawSwitch(2*FW-3 + (i/3)*(4*FW), 4*FH+1 + (i%3)*FH, SWSRC_SW1+i, getSwitch(SWSRC_SW1+i) ? INVERS : 0);
      }
#endif
    }
  }
  else {
    // Timer2
    drawTimerWithMode(87, 5*FH, 1);
  }

  // And ! in case of unexpected shutdown
#if defined(LOG_TELEMETRY) || defined(WATCHDOG_DISABLED)
  lcdDrawChar(REBOOT_X, 0*FH, '!', INVERS);
#else
  if (unexpectedShutdown) {
    lcdDrawChar(REBOOT_X, 0*FH, '!', INVERS);
  }
#endif

#if defined(GVARS) && !defined(PCBSTD)
  if (gvarDisplayTimer > 0) {
    gvarDisplayTimer--;
    warningText = STR_GLOBAL_VAR;
    drawMessageBox();
    lcdDrawSizedText(16, 5*FH, g_model.gvars[gvarLastChanged].name, LEN_GVAR_NAME, ZCHAR);
    lcdDrawText(16+6*FW, 5*FH, PSTR("["), BOLD);
#if defined(CPUARM)
    drawGVarValue(lcdLastRightPos, 5*FH, gvarLastChanged, GVAR_VALUE(gvarLastChanged, getGVarFlightMode(mixerCurrentFlightMode, gvarLastChanged)), LEFT|BOLD);
    if (g_model.gvars[gvarLastChanged].unit) {
      lcdDrawText(lcdLastRightPos, 5*FH, "%", BOLD);
    }
#else
    lcdDrawNumber(lcdLastRightPos, 5*FH, GVAR_VALUE(gvarLastChanged, getGVarFlightMode(mixerCurrentFlightMode, gvarLastChanged)), BOLD);
#endif
    lcdDrawText(lcdLastRightPos, 5*FH, PSTR("]"), BOLD);
    warningText = NULL;
  }
#endif

#if defined(DSM2)
  if (moduleFlag[0] == MODULE_BIND) {
    // Issue 98
    lcdDrawText(15*FW, 0, PSTR("BIND"), 0);
  }
#endif
}

#undef EVT_KEY_CONTEXT_MENU
#undef EVT_KEY_PREVIOUS_VIEW
#undef EVT_KEY_NEXT_VIEW
#undef EVT_KEY_NEXT_PAGE
#undef EVT_KEY_PREVIOUS_PAGE
#undef EVT_KEY_MODEL_MENU
#undef EVT_KEY_GENERAL_MENU
#undef EVT_KEY_LAST_MENU
#undef EVT_KEY_TELEMETRY
#undef EVT_KEY_STATISTICS
