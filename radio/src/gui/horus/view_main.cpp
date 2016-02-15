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

#define TRIM_LH_X                      10
#define TRIM_LV_X                      24
#define TRIM_RV_X                      (LCD_W-35)
#define TRIM_RH_X                      (LCD_W-175)
#define TRIM_V_Y                       55
#define TRIM_H_Y                       235
#define TRIM_LEN                       80
#define POTS_LINE_Y                    252

Layout * customScreens[MAX_CUSTOM_SCREENS];

void drawMainPots()
{
  // The 3 pots
  drawHorizontalSlider(TRIM_LH_X, POTS_LINE_Y, 160, calibratedStick[4], -RESX, RESX, 40, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_SQUARE_BUTTON);
  drawHorizontalSlider(LCD_W/2-20, POTS_LINE_Y, XPOTS_MULTIPOS_COUNT*5, 1 + (potsPos[1] & 0x0f), 1, XPOTS_MULTIPOS_COUNT + 1, XPOTS_MULTIPOS_COUNT, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_NUMBER_BUTTON);
  drawHorizontalSlider(TRIM_RH_X, POTS_LINE_Y, 160, calibratedStick[6], -RESX, RESX, 40, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_SQUARE_BUTTON);

  // The 2 rear sliders
  drawVerticalSlider(6, TRIM_V_Y, 160, calibratedStick[9], -RESX, RESX, 40, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_SQUARE_BUTTON);
  drawVerticalSlider(LCD_W-18, TRIM_V_Y, 160, calibratedStick[10], -RESX, RESX, 40, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_SQUARE_BUTTON);
}

void drawTrims(uint8_t flightMode)
{
  g_model.displayTrims = DISPLAY_TRIMS_ALWAYS;

  for (uint8_t i=0; i<4; i++) {
    static const coord_t x[4] = { TRIM_LH_X, TRIM_LV_X, TRIM_RV_X, TRIM_RH_X };
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
      drawVerticalSlider(xm, TRIM_V_Y, 160, trim, -125, 125, 0, OPTION_SLIDER_EMPTY_BAR|OPTION_SLIDER_TRIM_BUTTON);
    }
    else {
      drawHorizontalSlider(xm, TRIM_H_Y, 160, trim, -125, 125, 0, OPTION_SLIDER_EMPTY_BAR|OPTION_SLIDER_TRIM_BUTTON);
  /*    if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && trim != 0) {
        if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
          lcdDrawNumber((stickIndex==0 ? TRIM_LH_X : TRIM_RH_X)+(trim>0 ? -20 : 50), ym+1, trim, TINSIZE);
        }
      }*/
    }
  }
}

void drawMainViewTopBar()
{
  const int ALTITUDE_Y = 16;
  const int VOLTS_Y = 16+16+30;
  const int ALTITUDE_W = 56;
  const int ALTITUDE_X = LCD_W-ALTITUDE_Y-ALTITUDE_W;
  const int ALTITUDE_H = 30;
  const int PADDING = 4;

  theme->drawTopbarBackground(NULL);

  // USB icon
  if (usbPlugged()) {
    lcdDrawBitmapPattern(378, 8, LBM_TOPMENU_USB, MENU_TITLE_COLOR);
  }

  // RSSI
  const uint8_t rssiBarsValue[] = { 30, 40, 50, 60, 80 };
  const uint8_t rssiBarsHeight[] = { 5, 10, 15, 21, 31 };
  for (unsigned int i=0; i<DIM(rssiBarsHeight); i++) {
    uint8_t height = rssiBarsHeight[i];
    lcdDrawSolidFilledRect(390+i*6, 38-height, 4, height, TELEMETRY_RSSI() >= rssiBarsValue[i] ? MENU_TITLE_COLOR : MENU_TITLE_DISABLE_COLOR);
  }

  // Radio battery - TODO
  // putsValueWithUnit(370, 8, g_vbat100mV, UNIT_VOLTS, PREC1|SMLSIZE|MENU_TITLE_COLOR);
  // lcdDrawSolidRect(300, 3, 20, 50, MENU_TITLE_COLOR);
  // lcdDrawRect(batt_icon_x+FW, BAR_Y+1, 13, 7);
  // lcdDrawSolidVerticalLine(batt_icon_x+FW+13, BAR_Y+2, 5);

  // Rx battery
  if (g_model.frsky.voltsSource) { // TODO should not be in frsky struct
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

  // Model altitude
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
  else if (result == STR_SETUP_SCREENS) {
    pushMenu(menuTabMainviews[1]);
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
      POPUP_MENU_ADD_ITEM(STR_SETUP_SCREENS);
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
      if (gvarDisplayTimer > 0) {
        gvarDisplayTimer = 0;
      }
#endif
      AUDIO_KEYPAD_UP();
      break;
  }

  if (customScreens[0]) {
    customScreens[0]->refresh();
  }

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
