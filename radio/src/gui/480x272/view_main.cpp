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

#include "view_main.h"
// #include "menu_model.h"
// #include "menu_radio.h"
// #include "menu_screens.h"
// #include "model_select.h"
// #include "view_channels.h"
// #include "view_statistics.h"
#include "opentx.h"
#include "libopenui.h"

#define TRIM_WIDTH                     121
#define TRIM_LH_X                      10
#define TRIM_LV_X                      14
#define TRIM_RV_X                      (LCD_W-25)
#define TRIM_RH_X                      (LCD_W-TRIM_LH_X-TRIM_WIDTH)
#define TRIM_V_Y                       285
#define TRIM_H_Y                       (LCD_H-37)
#define TRIM_LEN                       80
#define POTS_LINE_Y                    (LCD_H-20)

Layout * customScreens[MAX_CUSTOM_SCREENS] = { 0, 0, 0, 0, 0 };
Topbar * topbar;

void drawMainPots()
{
  // The 3 pots
  drawHorizontalSlider(TRIM_LH_X, POTS_LINE_Y, TRIM_WIDTH, calibratedAnalogs[CALIBRATED_POT1], -RESX, RESX, 40, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_SQUARE_BUTTON);
  drawHorizontalSlider(LCD_W/2-20, POTS_LINE_Y, XPOTS_MULTIPOS_COUNT*5, 1 + (potsPos[1] & 0x0f), 1, XPOTS_MULTIPOS_COUNT + 1, XPOTS_MULTIPOS_COUNT, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_NUMBER_BUTTON);
  drawHorizontalSlider(TRIM_RH_X, POTS_LINE_Y, TRIM_WIDTH, calibratedAnalogs[CALIBRATED_POT3], -RESX, RESX, 40, OPTION_SLIDER_TICKS | OPTION_SLIDER_BIG_TICKS | OPTION_SLIDER_SQUARE_BUTTON);

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

    if (vert[i]) {
      if (g_model.extendedTrims == 1) {
        drawVerticalSlider(xm, TRIM_V_Y, 120, trim, TRIM_EXTENDED_MIN, TRIM_EXTENDED_MAX, 0, OPTION_SLIDER_EMPTY_BAR|OPTION_SLIDER_TRIM_BUTTON);
      }
      else {
        drawVerticalSlider(xm, TRIM_V_Y, 120, trim, TRIM_MIN, TRIM_MAX, 0, OPTION_SLIDER_EMPTY_BAR|OPTION_SLIDER_TRIM_BUTTON);
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
        drawHorizontalSlider(xm, TRIM_H_Y, 120, trim, TRIM_EXTENDED_MIN, TRIM_EXTENDED_MAX, 0, OPTION_SLIDER_EMPTY_BAR|OPTION_SLIDER_TRIM_BUTTON);
      }
      else {
        drawHorizontalSlider(xm, TRIM_H_Y, 120, trim, TRIM_MIN, TRIM_MAX, 0, OPTION_SLIDER_EMPTY_BAR|OPTION_SLIDER_TRIM_BUTTON);
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
  /*if (result == STR_VIEW_NOTES) {
    pushModelNotes();
  }
  else if (result == STR_STATISTICS) {
    pushMenu(menuTabStats[0]);
  }
  else if (result == STR_ABOUT_US) {
    chainMenu(menuAboutView);
  }
  else if (result == STR_MONITOR_SCREENS) {
    pushMenu(menuTabMonitors[lastMonitorPage]);
  }*/
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

ViewMain::ViewMain(bool icons):
  Window(&mainWindow, { 0, 0, LCD_W, LCD_H })
{
}

ViewMain::~ViewMain()
{
  deleteChildren();
}

bool ViewMain::onKeyEvent(event_t event)
{
  switch (event) {
    case EVT_KEY_LONG(KEY_MODEL):
      TRACE("ICI NEW MODELMENU ....");
      // TODO new ModelMenu();
      break;

    case EVT_KEY_LONG(KEY_ENTER):
      Menu * menu = new Menu();
      menu->addLine(STR_MODEL_SELECT, [=]() {
          // new ModelselectMenu();
      });
      if (1/*modelHasNotes()*/) {
        menu->addLine(STR_VIEW_NOTES, [=]() {
            // TODO
        });
      }
      menu->addLine(STR_MONITOR_SCREENS, [=]() {
          // new ChannelsMonitorMenu();
      });
      menu->addLine(STR_RESET_SUBMENU, [=]() {
          Menu * menu = new Menu();
          menu->addLine(STR_RESET_FLIGHT, [=]() {
              flightReset();
          });
          menu->addLine(STR_RESET_TIMER1, [=]() {
              timerReset(0);
          });
          menu->addLine(STR_RESET_TIMER2, [=]() {
              timerReset(1);
          });
          menu->addLine(STR_RESET_TIMER3, [=]() {
              timerReset(2);
          });
          menu->addLine(STR_RESET_TELEMETRY, [=]() {
              telemetryReset();
          });
      });
      menu->addLine(STR_STATISTICS, [=]() {
          // new StatisticsMenu();
      });
      menu->addLine(STR_ABOUT_US, [=]() {
          // TODO
      });
      return true;
  }

  return true;
}

void ViewMain::checkEvents()
{
  Window::checkEvents();

  // TODO temporary for refreshing the trims
  invalidate();
}

void ViewMain::paint(BitmapBuffer * dc)
{
  theme->drawBackground();

  lcdSetColor(ALARM_COLOR_INDEX);
  lcdDrawSolidFilledRect(0, 0, LCD_W, LCD_H, CUSTOM_COLOR);

  drawMainPots();

  if (g_model.view >= getMainViewsCount()) {
    g_model.view = 0;
  }

  for (uint8_t i=0; i<MAX_CUSTOM_SCREENS; i++) {
    if (customScreens[i]) {
      if (i == g_model.view) {
        customScreens[i]->refresh(); }
      else
        customScreens[i]->background();
    }
  }
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
