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
#include "menu_model.h"
#include "menu_radio.h"
#include "menu_screen.h"
#include "model_select.h"
#include "view_channels.h"
#include "view_statistics.h"
#include "topbar.h"
#include "layouts/sliders.h"
#include "layouts/trims.h"
#include "view_main_decoration.h"
#include "opentx.h"

Layout * customScreens[MAX_CUSTOM_SCREENS] = {};

int getMainViewsCount()
{
  for (int index=1; index<MAX_CUSTOM_SCREENS; index++) {
    if (!customScreens[index]) {
      return index + 1;
    }
  }
  return MAX_CUSTOM_SCREENS;
}

ViewMain * ViewMain::_instance = nullptr;

ViewMain::ViewMain():
  FormWindow(MainWindow::instance(), { 0, 0, LCD_W, LCD_H })
{
#if defined(HARDWARE_TOUCH) && !defined(HARDWARE_KEYS)
  new FabButton(this, 50, 100, ICON_MODEL,
                [=]() -> uint8_t {
                  new ModelMenu();
                  return 0;
                });

  new FabButton(this, LCD_W / 2, 100, ICON_RADIO,
                [=]() -> uint8_t {
                  new RadioMenu();
                  return 0;
                });

  new FabButton(this, LCD_W - 50, 100, ICON_THEME,
                [=]() -> uint8_t {
                  new ScreenMenu();
                  return 0;
                });

  new IconButton(this, {0, 0, MENU_HEADER_BUTTON_WIDTH, MENU_HEADER_BUTTON_WIDTH}, ICON_BACK,
                 [=]() -> uint8_t {
                   openMenu();
                   return 0;
                 }, NO_FOCUS);
#endif
  createDecoration();
  focusWindow = this;
}

ViewMain::~ViewMain()
{
}

void ViewMain::setTopbarVisible(bool visible)
{
  topbar->setVisible(visible);
}

void ViewMain::setTrimsVisible(bool visible)
{
  decoration->setTrimsVisible(visible);
}

void ViewMain::setSlidersVisible(bool visible)
{
  decoration->setSlidersVisible(visible);
}

void ViewMain::setFlightModeVisible(bool visible)
{
  decoration->setFlightModeVisible(visible);
}

void ViewMain::adjustDecoration()
{
  // Topbar does not need any computation
  // (height() has been set in setTopbarVisible())

  decoration->adjustDecoration();
}

rect_t ViewMain::getMainZone() const
{
  rect_t zone = decoration->getMainZone();
  zone.y += topbar->bottom();
  zone.h -= topbar->height();
  
  return zone;
}

#if defined(HARDWARE_KEYS)
void ViewMain::onEvent(event_t event)
{
  switch (event) {
    case EVT_KEY_LONG(KEY_MODEL):
      killEvents(event);
      new ModelMenu();
      break;

    case EVT_KEY_LONG(KEY_RADIO):
      killEvents(event);
      new RadioMenu();
      break;

    case EVT_KEY_LONG(KEY_TELEM):
      killEvents(event);
      new ScreenMenu();
      break;

    case EVT_KEY_LONG(KEY_ENTER):
      killEvents(event);
      openMenu();
      break;
  }
}
#endif

void ViewMain::openMenu()
{
  Menu * menu = new Menu(this);
  menu->addLine(STR_MODEL_SELECT, []() {
      new ModelSelectMenu();
  });
  if (modelHasNotes()) {
    menu->addLine(STR_VIEW_NOTES, [=]() {
        // TODO
    });
  }
  menu->addLine(STR_MONITOR_SCREENS, []() {
      new ChannelsViewMenu();
  });
  menu->addLine(STR_RESET_SUBMENU, [this]() {
      Menu * resetMenu = new Menu(this);
      resetMenu->addLine(STR_RESET_FLIGHT, []() {
          flightReset();
      });
      resetMenu->addLine(STR_RESET_TIMER1, []() {
          timerReset(0);
      });
      resetMenu->addLine(STR_RESET_TIMER2, []() {
          timerReset(1);
      });
      resetMenu->addLine(STR_RESET_TIMER3, []() {
          timerReset(2);
      });
      resetMenu->addLine(STR_RESET_TELEMETRY, []() {
          telemetryReset();
      });
  });
  menu->addLine(STR_STATISTICS, []() {
      new StatisticsViewPageGroup();
  });
  menu->addLine(STR_ABOUT_US, []() {
      // TODO
  });
}

void ViewMain::paint(BitmapBuffer * dc)
{
  OpenTxTheme::instance()->drawBackground(dc);

  if (g_model.view >= getMainViewsCount()) {
    g_model.view = 0;
  }
}

void ViewMain::createDecoration()
{
  createTopbar();
  decoration = new ViewMainDecoration(this, getRect());
}

void ViewMain::createTopbar()
{
  topbar = new TopBar(this);
  topbar->load();
}

