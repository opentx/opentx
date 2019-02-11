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

#define TRIM_LH_X                      10
#define TRIM_LV_X                      24
#define TRIM_RV_X                      (LCD_W-35)
#define TRIM_RH_X                      (LCD_W-175)
#define TRIM_V_Y                       55
#define TRIM_H_Y                       (LCD_H-37)
#define TRIM_LEN                       80
#define POTS_LINE_Y                    (LCD_H-20)

Layout * customScreens[MAX_CUSTOM_SCREENS] = { 0, 0, 0, 0, 0 };
Topbar * topbar;

void drawMainPots()
{
  // The 3 pots
  drawHorizontalSlider(TRIM_LH_X, POTS_LINE_Y, 160, calibratedAnalogs[CALIBRATED_POT1], -RESX, RESX, 40, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_SQUARE_BUTTON);
  drawHorizontalSlider(LCD_W/2-20, POTS_LINE_Y, XPOTS_MULTIPOS_COUNT*5, 1 + (potsPos[1] & 0x0f), 1, XPOTS_MULTIPOS_COUNT + 1, XPOTS_MULTIPOS_COUNT, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_NUMBER_BUTTON);
  drawHorizontalSlider(TRIM_RH_X, POTS_LINE_Y, 160, calibratedAnalogs[CALIBRATED_POT3], -RESX, RESX, 40, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_SQUARE_BUTTON);

  // The 2 rear sliders
  drawVerticalSlider(6, TRIM_V_Y, 160, calibratedAnalogs[CALIBRATED_SLIDER_REAR_LEFT], -RESX, RESX, 40, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_SQUARE_BUTTON);
  drawVerticalSlider(LCD_W-18, TRIM_V_Y, 160, calibratedAnalogs[CALIBRATED_SLIDER_REAR_RIGHT], -RESX, RESX, 40, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_SQUARE_BUTTON);
}

void drawTrims(uint8_t flightMode)
{
  for (uint8_t i=0; i<4; i++) {
    static const coord_t x[4] = { TRIM_LH_X, TRIM_LV_X, TRIM_RV_X, TRIM_RH_X };
    static uint8_t vert[4] = {0, 1, 1, 0};
    unsigned int stickIndex = CONVERT_MODE(i);
    coord_t xm = x[stickIndex];
    int32_t trim = getTrimValue(flightMode, i);


    if(getRawTrimValue(flightMode, i).mode == TRIM_MODE_NONE)
      continue;

    if (vert[i]) {
      if (g_model.extendedTrims == 1) {
        drawVerticalSlider(xm, TRIM_V_Y, 160, trim, TRIM_EXTENDED_MIN, TRIM_EXTENDED_MAX, 0, OPTION_SLIDER_EMPTY_BAR|OPTION_SLIDER_TRIM_BUTTON);
      }
      else {
        drawVerticalSlider(xm, TRIM_V_Y, 160, trim, TRIM_MIN, TRIM_MAX, 0, OPTION_SLIDER_EMPTY_BAR|OPTION_SLIDER_TRIM_BUTTON);
      }
      if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && trim != 0) {
        if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
          uint16_t y = TRIM_V_Y + TRIM_LEN + (trim<0 ? -TRIM_LEN/2 : TRIM_LEN/2);
          lcdDrawNumber(xm+2, y, trim, TINSIZE | CENTERED | VERTICAL);
        }
      }
    }
    else {
      if (g_model.extendedTrims == 1) {
        drawHorizontalSlider(xm, TRIM_H_Y, 160, trim, TRIM_EXTENDED_MIN, TRIM_EXTENDED_MAX, 0, OPTION_SLIDER_EMPTY_BAR|OPTION_SLIDER_TRIM_BUTTON);
      }
      else {
        drawHorizontalSlider(xm, TRIM_H_Y, 160, trim, TRIM_MIN, TRIM_MAX, 0, OPTION_SLIDER_EMPTY_BAR|OPTION_SLIDER_TRIM_BUTTON);
      }
      if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && trim != 0) {
        if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
          uint16_t x = xm + TRIM_LEN + (trim>0 ? -TRIM_LEN/2 : TRIM_LEN/2);
          lcdDrawNumber(x, TRIM_H_Y+2, trim, TINSIZE | CENTERED);
        }
      }
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
    pushMenu(menuTabStats[0]);
  }
  else if (result == STR_SETUP_SCREENS) {
    pushMenu(menuTabScreensSetup[1]);
  }
  else if (result == STR_ABOUT_US) {
    chainMenu(menuAboutView);
  }
  else if (result == STR_MONITOR_SCREENS) {
    pushMenu(menuTabMonitors[lastMonitorPage]);
  }
}

int getMainViewsCount()
{
  for (int index=1; index<MAX_CUSTOM_SCREENS; index++) {
    if (!customScreens[index]) {
      return index;
    }
  }
  return MAX_CUSTOM_SCREENS;
}

bool menuMainView(event_t event)
{
  switch (event) {
    case EVT_ENTRY:
      killEvents(KEY_EXIT);
      killEvents(KEY_UP);
      killEvents(KEY_DOWN);
      break;

    case EVT_KEY_LONG(KEY_ENTER):
      killEvents(event);
      POPUP_MENU_ADD_ITEM(STR_MODEL_SELECT);
      if (modelHasNotes()) {
        POPUP_MENU_ADD_ITEM(STR_VIEW_NOTES);
      }
      POPUP_MENU_ADD_ITEM(STR_MONITOR_SCREENS);
      POPUP_MENU_ADD_ITEM(STR_RESET_SUBMENU);
      POPUP_MENU_ADD_ITEM(STR_STATISTICS);
      POPUP_MENU_ADD_ITEM(STR_ABOUT_US);
      POPUP_MENU_START(onMainViewMenu);
      break;

    case EVT_KEY_LONG(KEY_MODEL):
      killEvents(event);
      pushMenu(menuTabModel[0]);
      return false;

    case EVT_KEY_LONG(KEY_RADIO):
      killEvents(event);
      pushMenu(menuTabGeneral[0]);
      return false;

    case EVT_KEY_LONG(KEY_TELEM):
      killEvents(event);
      pushMenu(menuTabScreensSetup[1]);
      return false;

#if defined(PCBX12S)
    case EVT_KEY_FIRST(KEY_PGDN):
#elif defined(PCBX10)
    case EVT_KEY_BREAK(KEY_PGDN):
#endif
      storageDirty(EE_MODEL);
      g_model.view = circularIncDec(g_model.view, +1, 0, getMainViewsCount()-1);
      break;

    case EVT_KEY_FIRST(KEY_PGUP):
#if defined(PCBX10)
    case EVT_KEY_LONG(KEY_PGDN):
#endif
      killEvents(event);
      storageDirty(EE_MODEL);
      g_model.view = circularIncDec(g_model.view, -1, 0, getMainViewsCount()-1);
      break;

    case EVT_KEY_FIRST(KEY_EXIT):
#if defined(GVARS)
      if (gvarDisplayTimer > 0) {
        gvarDisplayTimer = 0;
      }
#endif
      break;
  }

  if (g_model.view >= getMainViewsCount()) {
    g_model.view = 0;
  }

  for (uint8_t i=0; i<MAX_CUSTOM_SCREENS; i++) {
    if (customScreens[i]) {
      if (i == g_model.view)
        customScreens[i]->refresh();
      else
        customScreens[i]->background();
    }
  }

  return true;
}

#if 0
bool menuMainViewChannelsMonitor(event_t event)
{
  switch (event) {
    case EVT_KEY_BREAK(KEY_EXIT):
      chainMenu(menuMainView);
      event = 0;
      return false;
  }

  return menuChannelsView(event);
}
#endif
