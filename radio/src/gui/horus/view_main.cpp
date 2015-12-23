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

#include "../../opentx.h"

#define TRIM_LH_X                      90
#define TRIM_LV_X                      24
#define TRIM_RV_X                      (LCD_W-35)
#define TRIM_RH_X                      (LCD_W-95)
#define TRIM_V_Y                       135
#define TRIM_H_Y                       235
#define TRIM_LEN                       80

#define MODELPANEL_LEFT                240
#define MODELPANEL_TOP                 68
#define MODELPANEL_WIDTH               MODEL_BITMAP_WIDTH
#define MODELPANEL_HEIGHT              135

#define TIMER1PANEL_LEFT               46
#define TIMER1PANEL_TOP                55
#define TIMER2PANEL_LEFT               TIMER1PANEL_LEFT
#define TIMER2PANEL_TOP                134

void drawTrimSquare(coord_t x, coord_t y)
{
  lcdDrawSolidFilledRect(x-2, y, 15, 15, TITLE_BGCOLOR);
  lcdDrawBitmapPattern(x-2, y, LBM_TRIM_SHADOW, TEXT_COLOR);
}

void drawHorizontalTrimPosition(coord_t x, coord_t y, int16_t dir)
{
  drawTrimSquare(x, y);
  if (dir >= 0) {
    lcdDrawSolidVerticalLine(x+8, y+3, 9, TEXT_INVERTED_COLOR);
  }
  if (dir <= 0) {
    lcdDrawSolidVerticalLine(x+2, y+3, 9, TEXT_INVERTED_COLOR);
  }
  // if (exttrim) {
  //  lcdDrawSolidVerticalLine(xm, ym, 9, TEXT_INVERTED_COLOR);
  // }
}

void drawVerticalTrimPosition(coord_t x, coord_t y, int16_t dir)
{
  drawTrimSquare(x, y);
  if (dir >= 0) {
    lcdDrawSolidHorizontalLine(x+1, y+4, 9, TEXT_INVERTED_COLOR);
  }
  if (dir <= 0) {
    lcdDrawSolidHorizontalLine(x+1, y+10, 9, TEXT_INVERTED_COLOR);
  }
  // if (exttrim) {
  //   lcdDrawSolidHorizontalLine(xm-1, ym,  3, TEXT_INVERTED_COLOR);
  // }
}

void drawHorizontalStick(coord_t x, int val)
{
  for (int i=0; i<=160; i+=4) {
    if (i==0 || i==80 || i==160)
      lcdDrawSolidVerticalLine(x+i, 250, 13, TEXT_COLOR);
    else
      lcdDrawSolidVerticalLine(x+i, 252, 9, TEXT_COLOR);
  }
  drawHorizontalTrimPosition(x+TRIM_LEN+val*TRIM_LEN/RESX-5, TRIM_H_Y+16, val);
}

void drawVerticalStick(coord_t x, int val)
{
  for (int i=0; i<=160; i+=4) {
    if (i==0 || i==80 || i==160)
      lcdDrawSolidHorizontalLine(x, 56+i, 13, TEXT_COLOR);
    else
      lcdDrawSolidHorizontalLine(x+2, 56+i, 9, TEXT_COLOR);
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

void drawTrims(uint8_t flightMode)
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
          // TODO lcdDrawNumber(trim>0 ? 100 : 200, xm-2, trim, TINSIZE|VERTICAL);
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
          lcdDrawNumber((stickIndex==0 ? TRIM_LH_X : TRIM_RH_X)+(trim>0 ? -20 : 50), ym+2, trim, TINSIZE);
        }
      }
    }
  }
}

void drawTimer(coord_t x, coord_t y, int index)
{
  TimerData & timerData = g_model.timers[index];
  TimerState & timerState = timersStates[index];
  lcdDrawBitmapPattern(x, y, LBM_TIMER_BACKGROUND, TEXT_BGCOLOR);
  if (timerData.start) {
    lcdDrawBitmapPatternPie(x+2, y+3, LBM_RSCALE, TITLE_BGCOLOR, 0, timerState.val <= 0 ? 360 : 360*(timerData.start-timerState.val)/timerData.start);
  }
  putsTimer(x+76, y+31, abs(timerState.val), TEXT_COLOR|DBLSIZE|LEFT);
  if (ZLEN(timerData.name) > 0) {
    lcdDrawSizedText(x+78, y+20, timerData.name, LEN_TIMER_NAME, ZCHAR|SMLSIZE|TEXT_COLOR);
  }
  putsStrIdx(x+137, y+17, "TMR", index+1, SMLSIZE|TEXT_COLOR);
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
      lcdDrawSolidFilledRect(ALTITUDE_X, VOLTS_Y, ALTITUDE_W, ALTITUDE_H, TEXT_BGCOLOR);
      lcdDrawText(ALTITUDE_X+PADDING, VOLTS_Y+2, "Voltage", att);
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
      lcdDrawSolidFilledRect(ALTITUDE_X, ALTITUDE_Y, ALTITUDE_W, ALTITUDE_H, TEXT_BGCOLOR);
      lcdDrawText(ALTITUDE_X+PADDING, ALTITUDE_Y+2, "Alt", att);
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
        lcdDrawSolidFilledRect(x, MENU_FOOTER_TOP+17, 9, 9, TEXT_INVERTED_BGCOLOR);
      }
      else {
        lcdDrawSolidRect(x+1, MENU_FOOTER_TOP+18, 7, 7, TEXT_COLOR);
      }
      x -= 11;
    }
  }
}

void onMainViewMenu(const char *result)
{
  if (result == STR_MODEL_SELECT) {
    chainMenu(menuModelSelect);
  }
  else if (result == STR_RESET_TIMER1) {
    timerReset(0);
  }
  else if (result == STR_RESET_TIMER2) {
    timerReset(1);
  }
  else if (result == STR_RESET_TIMER3) {
    timerReset(2);
  }
  else if (result == STR_VIEW_NOTES) {
    pushModelNotes();
  }
  else if (result == STR_RESET_SUBMENU) {
    POPUP_MENU_ADD_ITEM(STR_RESET_FLIGHT);
    POPUP_MENU_ADD_ITEM(STR_RESET_TIMER1);
    POPUP_MENU_ADD_ITEM(STR_RESET_TIMER2);
    POPUP_MENU_ADD_ITEM(STR_RESET_TIMER3);
    POPUP_MENU_ADD_ITEM(STR_RESET_TELEMETRY);
  }
  else if (result == STR_RESET_TELEMETRY) {
    telemetryReset();
  }
  else if (result == STR_RESET_FLIGHT) {
    flightReset();
  }
  else if (result == STR_STATISTICS) {
    chainMenu(menuStatisticsView);
  }
  else if (result == STR_ABOUT_US) {
    chainMenu(menuAboutView);
  }
}

bool menuMainView(evt_t event)
{
  switch (event) {
    case EVT_ENTRY:
      killEvents(KEY_EXIT);
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      break;

    case EVT_ENTRY_UP:
      LOAD_MODEL_BITMAP();
      break;

    case EVT_KEY_LONG(KEY_ENTER):
      killEvents(event);
      POPUP_MENU_ADD_ITEM(STR_MODEL_SELECT);
      if (modelHasNotes()) {
        POPUP_MENU_ADD_ITEM(STR_VIEW_NOTES);
      }
      POPUP_MENU_ADD_ITEM(STR_RESET_SUBMENU);
      POPUP_MENU_ADD_ITEM(STR_STATISTICS);
      POPUP_MENU_ADD_ITEM(STR_ABOUT_US);
      popupMenuHandler = onMainViewMenu;
      break;

    case EVT_KEY_BREAK(KEY_MENU):
      pushMenu(menuModelSetup);
      break;

    case EVT_KEY_LONG(KEY_MENU):
      killEvents(event);
      pushMenu(menuGeneralSetup);
      break;

    case EVT_KEY_BREAK(KEY_DOWN):
      storageDirty(EE_GENERAL);
      g_eeGeneral.view = circularIncDec(g_eeGeneral.view, +1, 0, VIEW_COUNT-1, isViewAvailable);
      break;

    case EVT_KEY_BREAK(KEY_UP):
      killEvents(event);
      storageDirty(EE_GENERAL);
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

  // 23ms if 24bits per pixel (with transparency) 5/6/5/8
  // 6ms if 16bits per pixel 5/6/5 no DMA
  // 1.2ms with the DMA
  TIME_MEASURE_START(backgroundbitmap);
  lcdDrawBitmap(0, 0, LBM_MAINVIEW_BACKGROUND);
  TIME_MEASURE_STOP(backgroundbitmap);

  // Header
  lcdDrawSolidFilledRect(0, 0, LCD_W, MENU_HEADER_HEIGHT, HEADER_BGCOLOR);
  lcdDrawBitmapPattern(0, 0, LBM_TOPMENU_POLYGON, TITLE_BGCOLOR);
  lcdDrawBitmapPattern(4, 10, LBM_TOPMENU_OPENTX, MENU_TITLE_COLOR);
  drawTopmenuDatetime();
  if (usbPlugged()) {
    lcdDrawBitmapPattern(378, 8, LBM_TOPMENU_USB, MENU_TITLE_COLOR);
  }
  const uint8_t rssiBarsValue[] = { 30, 40, 50, 60, 80 };
  const uint8_t rssiBarsHeight[] = { 5, 10, 15, 21, 31 };
  for (unsigned int i=0; i<DIM(rssiBarsHeight); i++) {
    uint8_t height = rssiBarsHeight[i];
    lcdDrawSolidFilledRect(390+i*6, 38-height, 4, height, TELEMETRY_RSSI() >= rssiBarsValue[i] ? MENU_TITLE_COLOR : MENU_TITLE_DISABLE_COLOR);
  }

  // Flight Mode Name
  int mode = mixerCurrentFlightMode;
  for (int i=0; i<MAX_FLIGHT_MODES; i++) {
    lcdDrawSolidVerticalLine(LCD_W/2-20+5*i, 254, 13, TEXT_COLOR);
  }
  drawTrimSquare(LCD_W/2-25+5*mixerCurrentFlightMode, 253);
  lcdDrawChar(LCD_W/2-23+5*mixerCurrentFlightMode, 254, '0'+mixerCurrentFlightMode, SMLSIZE|TEXT_INVERTED_COLOR);
  lcdDrawSizedText(LCD_W/2-getTextWidth(g_model.flightModeData[mode].name, sizeof(g_model.flightModeData[mode].name), ZCHAR|SMLSIZE)/2, 237, g_model.flightModeData[mode].name, sizeof(g_model.flightModeData[mode].name), ZCHAR|SMLSIZE);

  // Sticks
  drawSticks();

  // Trims
  drawTrims(mode);

  // Model panel
  TIME_MEASURE_START(filledRect);
  lcdDrawFilledRect(MODELPANEL_LEFT, MODELPANEL_TOP, MODELPANEL_WIDTH, MODELPANEL_HEIGHT, SOLID, TEXT_BGCOLOR | OPACITY(5));
  TIME_MEASURE_STOP(filledRect); // 9ms !

  lcdDrawBitmapPattern(MODELPANEL_LEFT+6, MODELPANEL_TOP+4, LBM_MODEL_ICON, TITLE_BGCOLOR);
  lcdDrawSizedText(MODELPANEL_LEFT+45, MODELPANEL_TOP+10, g_model.header.name, LEN_MODEL_NAME, ZCHAR|SMLSIZE);
  lcdDrawSolidHorizontalLine(MODELPANEL_LEFT+39, MODELPANEL_TOP+27, MODELPANEL_WIDTH-48, TITLE_BGCOLOR);
  int scale = getBitmapScale(modelBitmap, MODEL_BITMAP_WIDTH, MODEL_BITMAP_HEIGHT);
  int width = getBitmapScaledSize(getBitmapWidth(modelBitmap), scale);
  int height = getBitmapScaledSize(getBitmapHeight(modelBitmap), scale);
  lcdDrawBitmap(MODELPANEL_LEFT+(MODEL_BITMAP_WIDTH-width)/2, MODELPANEL_TOP+MODELPANEL_HEIGHT-MODEL_BITMAP_HEIGHT/2-height/2, modelBitmap, 0, 0, scale);

  // Timers
  if (g_model.timers[0].mode) {
    drawTimer(TIMER1PANEL_LEFT, TIMER1PANEL_TOP, 0);
  }
  if (g_model.timers[1].mode) {
    drawTimer(TIMER2PANEL_LEFT, TIMER2PANEL_TOP, 1);
  }

#if 0
  if (s_gvar_timer > 0) {
    s_gvar_timer--;
    displayMessageBox();
    putsStrIdx(WARNING_LINE_X, WARNING_LINE_Y, STR_GV, s_gvar_last+1, DBLSIZE|YELLOW);
    lcdDrawSizedText(WARNING_LINE_X+45, WARNING_LINE_Y, g_model.gvars[s_gvar_last].name, LEN_GVAR_NAME, DBLSIZE|YELLOW|ZCHAR);
    lcdDrawNumber(WARNING_LINE_X, WARNING_INFOLINE_Y, GVAR_VALUE(s_gvar_last, getGVarFlightPhase(mixerCurrentFlightMode, s_gvar_last)), DBLSIZE|LEFT);
  }
#endif

  return true;
}

bool menuMainViewChannelsMonitor(evt_t event)
{
  switch (event) {
    case EVT_KEY_BREAK(KEY_EXIT):
      chainMenu(menuMainView);
      event = 0;
      return false;
  }

  return menuChannelsView(event);
}
