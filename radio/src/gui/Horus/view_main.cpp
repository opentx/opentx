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

#define BIGSIZE       MIDSIZE
#define BAR_HEIGHT    (BOX_WIDTH-9)
#define LBOX_CENTERX  (BOX_WIDTH/2 + 17)
#define RBOX_CENTERX  (LCD_W-LBOX_CENTERX)
#define BITMAP_X      ((LCD_W-64)/2)
#define BITMAP_Y      (LCD_H/2)
#define TRIM_LH_X     (32+9)
#define TRIM_LV_X     10
#define TRIM_RV_X     (LCD_W-11)
#define TRIM_RH_X     (LCD_W-32-9)
#define TRIM_LEN 27

void drawPotsBars()
{
  // Optimization by Mike Blandford
  uint8_t x, i, len ;  // declare temporary variables
  for (x=LCD_W/2-9, i=NUM_STICKS; i<NUM_STICKS+NUM_POTS; x+=9, i++) {
    if (IS_POT_AVAILABLE(i)) {
      len = ((calibratedStick[i]+RESX)*BAR_HEIGHT/(RESX*2))+1l;  // calculate once per loop
      lcdDrawFilledRect(x, 170-FH-len, 5, len, TEXT_COLOR);
    }
  }
}

void doMainScreenGraphics()
{
  int16_t calibStickVert = calibratedStick[CONVERT_MODE(1)];
  if (g_model.throttleReversed && CONVERT_MODE(1) == THR_STICK)
    calibStickVert = -calibStickVert;
  drawStick(LBOX_CENTERX, calibratedStick[CONVERT_MODE(0)], calibStickVert);

  calibStickVert = calibratedStick[CONVERT_MODE(2)];
  if (g_model.throttleReversed && CONVERT_MODE(2) == THR_STICK)
    calibStickVert = -calibStickVert;
  drawStick(RBOX_CENTERX, calibratedStick[CONVERT_MODE(3)], calibStickVert);
}

void displayTrims(uint8_t phase)
{
  for (uint8_t i=0; i<4; i++) {
    static coord_t x[4] = {TRIM_LH_X, TRIM_LV_X, TRIM_RV_X, TRIM_RH_X};
    static uint8_t vert[4] = {0,1,1,0};
    coord_t xm, ym;
    xm = x[CONVERT_MODE(i)];

    uint8_t att = ROUND;
    int16_t val = getTrimValue(phase, i);

    int16_t dir = val;
    bool exttrim = false;
    if (val < -125 || val > 125) {
      exttrim = true;
    }

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
      lcd_vline(xm, ym-TRIM_LEN, TRIM_LEN*2);
      if (i!=2 || !g_model.thrTrim) {
        lcd_vline(xm-1, ym-1,  3);
        lcd_vline(xm+1, ym-1,  3);
      }
      ym -= val;

      // TODO lcdDrawFilledRect(xm-3, ym-3, 7, 7, SOLID, att);
      if (dir >= 0) {
        lcd_hline(xm-1, ym-1,  3);
      }
      if (dir <= 0) {
        lcd_hline(xm-1, ym+1,  3);
      }
      if (exttrim) {
        lcd_hline(xm-1, ym,  3);
      }
    }
    else {
      ym = 60;
      lcd_hline(xm-TRIM_LEN, ym, TRIM_LEN*2);
      lcd_hline(xm-1, ym-1,  3);
      lcd_hline(xm-1, ym+1,  3);
      xm += val;

      // TODO lcdDrawFilledRect(xm-3, ym-3, 7, 7, SOLID, att);
      if (dir >= 0) {
        lcd_vline(xm+1, ym-1,  3);
      }
      if (dir <= 0) {
        lcd_vline(xm-1, ym-1,  3);
      }
      if (exttrim) {
        lcd_vline(xm, ym-1,  3);
      }
    }
    lcdDrawSquare(xm-3, ym-3, 7, att);
  }
}

void displayTimers()
{
  // const int TIMERS_W = 56;
  const int TIMERS_H = 30;
  const int TIMERS_MARGIN = 16;
  const int TIMERS_PADDING = 4;

  for (int i=0; i<TIMERS; i++) {
    if (g_model.timers[i].mode) {
      TimerState & timerState = timersStates[i];
      TimerData & timerData = g_model.timers[i];
      int y = TIMERS_MARGIN + i*(TIMERS_H+TIMERS_MARGIN);
      unsigned int len = zlen(timerData.name, LEN_TIMER_NAME);
      LcdFlags color=TEXT_COLOR, bgColor=TEXT_BGCOLOR;
      if (timerState.val < 0) {
        color = ALARM_COLOR;
      }
      // TODO lcdDrawFilledRect(TIMERS_MARGIN, y, TIMERS_W, TIMERS_H, SOLID, bgColor);
      putsTimer(TIMERS_MARGIN+TIMERS_PADDING, y+12, abs(timerState.val), color|DBLSIZE|LEFT);
      if (len > 0)
        lcd_putsnAtt(TIMERS_MARGIN+TIMERS_PADDING, y+2, timerData.name, LEN_TIMER_NAME, color|ZCHAR);
      else
        putsTimerMode(TIMERS_MARGIN+TIMERS_PADDING, y+2, timerData.mode, color);
    }
  }
}

void displayMainTelemetryFields()
{
  const int ALTITUDE_Y = 16;
  const int VOLTS_Y = 16+16+30;
  const int ALTITUDE_W = 56;
  const int ALTITUDE_X = LCD_W-ALTITUDE_Y-ALTITUDE_W;
  const int ALTITUDE_H = 30;
  const int PADDING = 4;

  if (g_model.frsky.voltsSource) {
    TelemetryItem & item = telemetryItems[g_model.frsky.voltsSource-1];
    if (item.isAvailable()) {
      int32_t value = item.value;
      TelemetrySensor & sensor = g_model.telemetrySensors[g_model.frsky.altitudeSource-1];
      LcdFlags att = 0;
      if (sensor.prec == 2) {
        att |= PREC1;
        value /= 10;
      }
      else if (sensor.prec == 1) {
        att |= PREC1;
      }
      att |= (item.isOld() ? ALARM_COLOR : TEXT_COLOR);
      lcdDrawFilledRect(ALTITUDE_X, VOLTS_Y, ALTITUDE_W, ALTITUDE_H, TEXT_BGCOLOR);
      lcd_putsAtt(ALTITUDE_X+PADDING, VOLTS_Y+2, "Voltage", att);
      putsValueWithUnit(ALTITUDE_X+PADDING, VOLTS_Y+12, value, UNIT_VOLTS, DBLSIZE|LEFT|att);
    }
  }

  if (g_model.frsky.altitudeSource) {
    TelemetryItem & item = telemetryItems[g_model.frsky.altitudeSource-1];
    if (item.isAvailable()) {
      int32_t value = item.value;
      TelemetrySensor & sensor = g_model.telemetrySensors[g_model.frsky.altitudeSource-1];
      if (sensor.prec) value /= sensor.prec == 2 ? 100 : 10;
      LcdFlags att = (item.isOld() ? ALARM_COLOR : TEXT_COLOR);
      lcdDrawFilledRect(ALTITUDE_X, ALTITUDE_Y, ALTITUDE_W, ALTITUDE_H, TEXT_BGCOLOR);
      lcd_putsAtt(ALTITUDE_X+PADDING, ALTITUDE_Y+2, "Alt", att);
      putsValueWithUnit(ALTITUDE_X+PADDING, ALTITUDE_Y+12, value, UNIT_METERS, DBLSIZE|LEFT|att);
    }
  }
}

bool isViewAvailable(int index)
{
  if (index <= VIEW_CHANNELS)
    return true;
  else
    return TELEMETRY_SCREEN_TYPE(index-VIEW_TELEM1) != TELEMETRY_SCREEN_TYPE_NONE;
}

void displayMainViewIndex()
{
  int x = LCD_W - 14;

  for (int i=VIEW_COUNT; i>=0; --i) {
    if (isViewAvailable(i)) {
      if (g_eeGeneral.view == i) {
        lcdDrawFilledRect(x, MENU_FOOTER_TOP+17, 9, 9, TEXT_INVERTED_BGCOLOR);
      }
      else {
        lcdDrawRect(x+1, MENU_FOOTER_TOP+18, 7, 7, TEXT_COLOR);
      }
      x -= 11;
    }
  }
}

void onMainViewMenu(const char *result)
{
  if (result == STR_RESET_TIMER1) {
    timerReset(0);
  }
  else if (result == STR_RESET_TIMER2) {
    timerReset(1);
  }
#if defined(CPUARM)
  else if (result == STR_RESET_TIMER3) {
    timerReset(2);
  }
  else if (result == STR_VIEW_NOTES) {
    pushModelNotes();
  }
  else if (result == STR_RESET_SUBMENU) {
    MENU_ADD_ITEM(STR_RESET_FLIGHT);
    MENU_ADD_ITEM(STR_RESET_TIMER1);
    MENU_ADD_ITEM(STR_RESET_TIMER2);
    MENU_ADD_ITEM(STR_RESET_TIMER3);
#if defined(FRSKY)
    MENU_ADD_ITEM(STR_RESET_TELEMETRY);
#endif
  }
#endif
#if defined(FRSKY)
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

void displayTelemetryScreen(int index, unsigned int evt);

void menuMainView(evt_t event)
{
  // clear the screen
  lcdDrawFilledRect(0, 0, LCD_W, LCD_H, TEXT_BGCOLOR);

  switch (event) {
    case EVT_ENTRY:
      killEvents(KEY_EXIT);
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      break;

    case EVT_KEY_LONG(KEY_ENTER):
      killEvents(event);
      if (modelHasNotes()) {
        MENU_ADD_ITEM(STR_VIEW_NOTES);
      }
      MENU_ADD_ITEM(STR_RESET_SUBMENU);
      MENU_ADD_ITEM(STR_STATISTICS);
      MENU_ADD_ITEM(STR_ABOUT_US);
      menuHandler = onMainViewMenu;
      break;

    case EVT_KEY_BREAK(KEY_MENU):
      pushMenu(menuModelSetup);
      break;

    case EVT_KEY_LONG(KEY_MENU):
      killEvents(event);
      pushMenu(menuGeneralSetup);
      break;

    case EVT_KEY_BREAK(KEY_DOWN):
      eeDirty(EE_GENERAL);
      g_eeGeneral.view = circularIncDec(g_eeGeneral.view, +1, 0, VIEW_COUNT-1, isViewAvailable);
      break;

    case EVT_KEY_BREAK(KEY_UP):
      killEvents(event);
      eeDirty(EE_GENERAL);
      g_eeGeneral.view = circularIncDec(g_eeGeneral.view, -1, 0, VIEW_COUNT-1, isViewAvailable);
      break;

    case EVT_KEY_FIRST(KEY_EXIT):
#if defined(GVARS)
      if (s_gvar_timer > 0) {
        s_gvar_timer = 0;
      }
#endif
      AUDIO_KEYPAD_UP();
      break;
  }

#if 0
  displayMainViewIndex();

  if (g_eeGeneral.view == VIEW_TIMERS_ALTITUDE) {
    displayTimers();
    displayMainTelemetryFields();
  }
  else if (g_eeGeneral.view == VIEW_CHANNELS) {
    menuChannelsView(event);
  }
  else {
    displayTelemetryScreen(g_eeGeneral.view - VIEW_TELEM1, event);
  }
#endif

#if defined(GVARS)
  if (s_gvar_timer > 0) {
    s_gvar_timer--;
    displayMessageBox();
    putsStrIdx(WARNING_LINE_X, WARNING_LINE_Y, STR_GV, s_gvar_last+1, DBLSIZE|YELLOW);
    lcd_putsnAtt(WARNING_LINE_X+45, WARNING_LINE_Y, g_model.gvars[s_gvar_last].name, LEN_GVAR_NAME, DBLSIZE|YELLOW|ZCHAR);
    lcd_outdezAtt(WARNING_LINE_X, WARNING_INFOLINE_Y, GVAR_VALUE(s_gvar_last, getGVarFlightPhase(mixerCurrentFlightMode, s_gvar_last)), DBLSIZE|LEFT);
  }
#endif
}

void menuMainViewChannelsMonitor(evt_t event)
{
  switch(event) {
    case EVT_KEY_BREAK(KEY_EXIT):
      chainMenu(menuMainView);
      event = 0;
      break;
  }

  return menuChannelsView(event);
}
