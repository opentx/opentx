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

#define TRIM_LH_X     90
#define TRIM_LV_X     24
#define TRIM_RV_X     (LCD_W-35)
#define TRIM_RH_X     (LCD_W-95)
#define TRIM_V_Y      135
#define TRIM_H_Y      235
#define TRIM_LEN      80

void drawPotsBars()
{
  // Optimization by Mike Blandford
  uint8_t x, i, len ;  // declare temporary variables
  for (x=LCD_W/2-9, i=NUM_STICKS; i<NUM_STICKS+NUM_POTS; x+=9, i++) {
    if (IS_POT_AVAILABLE(i)) {
      len = ((calibratedStick[i]+RESX)*BAR_HEIGHT/(RESX*2))+1l;  // calculate once per loop
      // TODO 220 constant
      lcdDrawFilledRect(x, 220-FH-len, 5, len, TEXT_COLOR);
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

void drawTrimSquare(coord_t x, coord_t y)
{
  lcdDrawFilledRect(x-2, y, 15, 15, TITLE_BGCOLOR);
  lcdDrawBitmapPattern(x-2, y, LBM_TRIM_SHADOW, TEXT_COLOR);
}

void drawHorizontalTrimPosition(coord_t x, coord_t y, int16_t dir)
{
  drawTrimSquare(x, y);
  if (dir >= 0) {
    lcdDrawVerticalLine(x+8, y+3, 9, TEXT_INVERTED_COLOR);
  }
  if (dir <= 0) {
    lcdDrawVerticalLine(x+2, y+3, 9, TEXT_INVERTED_COLOR);
  }
  // if (exttrim) {
  //  lcdDrawVerticalLine(xm, ym, 9, TEXT_INVERTED_COLOR);
  // }
}

void drawVerticalTrimPosition(coord_t x, coord_t y, int16_t dir)
{
  drawTrimSquare(x, y);
  if (dir >= 0) {
    lcdDrawHorizontalLine(x+1, y+4, 9, TEXT_INVERTED_COLOR);
  }
  if (dir <= 0) {
    lcdDrawHorizontalLine(x+1, y+10, 9, TEXT_INVERTED_COLOR);
  }
  // if (exttrim) {
  //   lcdDrawHorizontalLine(xm-1, ym,  3, TEXT_INVERTED_COLOR);
  // }
}

void drawHorizontalStick(coord_t x, int val)
{
  for (int i=0; i<=160; i+=4) {
    if (i==0 || i==80 || i==160)
      lcdDrawVerticalLine(x+i, 250, 13, TEXT_COLOR);
    else
      lcdDrawVerticalLine(x+i, 252, 9, TEXT_COLOR);
  }
  drawHorizontalTrimPosition(x+TRIM_LEN+val*TRIM_LEN/RESX-4, TRIM_H_Y+16, val);
}

void drawVerticalStick(coord_t x, int val)
{
  for (int i=0; i<=160; i+=4) {
    if (i==0 || i==80 || i==160)
      lcdDrawHorizontalLine(x, 56+i, 13, TEXT_COLOR);
    else
      lcdDrawHorizontalLine(x+2, 56+i, 9, TEXT_COLOR);
  }
  drawVerticalTrimPosition(x, TRIM_V_Y+val*TRIM_LEN/RESX-6, val);
}

void drawSticks()
{
  drawVerticalStick(6, calibratedStick[1]);
  drawHorizontalStick(TRIM_LH_X-TRIM_LEN+1, calibratedStick[0]);
  drawVerticalStick(LCD_W-18, calibratedStick[2]);
  drawHorizontalStick(TRIM_RH_X-TRIM_LEN+1, calibratedStick[3]);
}

void displayTrims(uint8_t flightMode)
{
  g_model.displayTrims = DISPLAY_TRIMS_ALWAYS;

  for (uint8_t i=0; i<4; i++) {
    static coord_t x[4] = {TRIM_LH_X, TRIM_LV_X, TRIM_RV_X, TRIM_RH_X};
    static uint8_t vert[4] = {0, 1, 1, 0};
    unsigned int stickIndex = CONVERT_MODE(i);
    coord_t xm = x[stickIndex];

    int32_t trim = getTrimValue(flightMode, i);
    int32_t val = trim * TRIM_LEN / 125;
    bool exttrim = false;
    if (val < -(TRIM_LEN+1)) {
      val = -(TRIM_LEN+1);
      exttrim = true;
    }
    else if (val > (TRIM_LEN+1)) {
      val = TRIM_LEN+1;
      exttrim = true;
    }

    if (vert[i]) {
      coord_t ym = TRIM_V_Y;
      lcdDrawBitmapPattern(xm, ym-TRIM_LEN, LBM_VTRIM_FRAME, TEXT_COLOR);
      if (i!=2 || !g_model.thrTrim) {
        // TODO
      }
      ym -= val;
      drawVerticalTrimPosition(xm-1, ym-6, val);
      if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && trim != 0) {
        if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
          // TODO lcd_outdezAtt(trim>0 ? 100 : 200, xm-2, trim, TINSIZE|VERTICAL);
        }
      }
    }
    else {
      coord_t ym = TRIM_H_Y;
      lcdDrawBitmapPattern(xm-TRIM_LEN, ym, LBM_HTRIM_FRAME, TEXT_COLOR);
      xm += val;
      drawHorizontalTrimPosition(xm-3, ym-2, val);
      if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && trim != 0) {
        if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
          lcd_outdezAtt((stickIndex==0 ? TRIM_LH_X : TRIM_RH_X)+(trim>0 ? -20 : 50), ym+2, trim, TINSIZE);
        }
      }
    }
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

const uint16_t LBM_MAINVIEW_BACKGROUND[] = {
#include "../../bitmaps/Horus/background.lbm"
};

const uint16_t LBM_MAINVIEW_FLAT[] = {
#include "../../bitmaps/Horus/mainview_flat.lbm"
};

const uint16_t LBM_CORSAIR[] = {
#include "../../bitmaps/Horus/corsair.lbm"
};


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

  lcdDrawBitmap(0, 0, LBM_MAINVIEW_BACKGROUND);
  // lcdDrawBitmap(0, 0, LBM_MAINVIEW_FLAT);

  // Header
  lcdDrawFilledRect(0, 0, LCD_W, MENU_HEADER_HEIGHT, HEADER_BGCOLOR);
  lcdDrawBitmapPattern(0, 0, LBM_TOPMENU_POLYGON, TITLE_BGCOLOR);
  lcdDrawBitmapPattern(4, 10, LBM_TOPMENU_OPENTX, MENU_TITLE_COLOR);
  lcdDrawTopmenuDatetime();

  // Flight Mode Name
  int mode = mixerCurrentFlightMode;
  lcd_putsnAtt(212, 237, g_model.flightModeData[mode].name, sizeof(g_model.flightModeData[mode].name), ZCHAR|SMLSIZE);

  // Sticks
  drawSticks();

  // Trims
  displayTrims(mode);

  // Model panel
  lcdDrawFilledRectWithAttributes(248, 58, 188, 158, SOLID, TEXT_BGCOLOR | (5<<24));
  lcdDrawBitmapPattern(256, 62, LBM_MODEL_ICON, TITLE_BGCOLOR);
  lcd_putsAtt(293, 68, "MyPlane Name", SMLSIZE);
  lcdDrawHorizontalLine(287, 85, 140, TITLE_BGCOLOR);
  lcdDrawBitmap(256, 104, LBM_CORSAIR);


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

#if 0
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
