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
#include "opentx.h"

Layout * customScreens[MAX_CUSTOM_SCREENS] = {};

int getMainViewsCount()
{
  for (int index=1; index<MAX_CUSTOM_SCREENS; index++) {
    if (!customScreens[index]) {
      return index;
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

void ViewMain::setSlidersVisible(bool visible)
{
  //
  // Horizontal Sliders
  //
  sliders[SLIDERS_POT1]->setHeight(visible ? TRIM_SQUARE_SIZE : 0);

  //TODO: might change depending on hardware settings
  if (IS_POT_MULTIPOS(POT2)) {
    sliders[SLIDERS_POT2]->setHeight(visible ? MULTIPOS_H : 0);
  }
  else if (IS_POT(POT2)) {
    sliders[SLIDERS_POT2]->setHeight(visible ? TRIM_SQUARE_SIZE : 0);
  }

#if defined(HARDWARE_POT3)
  sliders[SLIDERS_POT3]->setHeight(visible ? TRIM_SQUARE_SIZE : 0);
#endif

  //
  // Vertical sliders
  //
  sliders[SLIDERS_REAR_LEFT]->setWidth(visible ? TRIM_SQUARE_SIZE : 0);

#if defined(HARDWARE_EXT1)
  //TODO: might change depending on hardware settings
  if (IS_POT_SLIDER_AVAILABLE(EXT1)) {
    sliders[SLIDERS_EXT1]->setWidth(visible ? TRIM_SQUARE_SIZE : 0);
  }
#endif

  sliders[SLIDERS_REAR_RIGHT]->setWidth(visible ? TRIM_SQUARE_SIZE : 0);
    
#if defined(HARDWARE_EXT2)
  //TODO: might change depending on hardware settings
  if (IS_POT_SLIDER_AVAILABLE(EXT2)) {
    sliders[SLIDERS_EXT2]->setWidth(visible ? TRIM_SQUARE_SIZE : 0);
  }
#endif
}

void ViewMain::setTrimsVisible(bool visible)
{
  trims[TRIMS_LH]->setHeight(visible ? TRIM_SQUARE_SIZE : 0);
  trims[TRIMS_RH]->setHeight(visible ? TRIM_SQUARE_SIZE : 0);

  trims[TRIMS_LV]->setWidth(visible ? TRIM_SQUARE_SIZE : 0);
  trims[TRIMS_RV]->setWidth(visible ? TRIM_SQUARE_SIZE : 0);
}

void ViewMain::setFlightModeVisible(bool visible)
{
  flightMode->setHeight(visible ? 20 : 0);
}

void ViewMain::adjustDecoration()
{
  // Topbar does not need any computation
  // (height() has been set in setTopbarVisible())
  
  // Sliders are closer to the edge and must be computed first
  // (vertical sliders depends on topbar)

  // These are located on the bottom
  auto pos = height() - sliders[SLIDERS_POT1]->height();
  sliders[SLIDERS_POT1]->setTop(pos);

  if (sliders[SLIDERS_POT2]) {
    auto sl = sliders[SLIDERS_POT2];
    sl->setTop(pos);
    if (IS_POT_MULTIPOS(POT2)) {
      sl->setWidth(MULTIPOS_W);
    }
    else { // if !IS_POT(POT2) -> sliders[SLIDERS_POT2] == nullptr
      sl->setWidth(HORIZONTAL_SLIDERS_WIDTH);
    }
    sl->setLeft((width() - sl->width()) / 2);
  }

#if defined(HARDWARE_POT3)
  sliders[SLIDERS_POT3]->setTop(pos);
#endif

  // Horizontal trims are on top of horizontal sliders
  pos -= trims[TRIMS_LH]->height();
  trims[TRIMS_LH]->setTop(pos);
  trims[TRIMS_RH]->setTop(pos);

  // Vertical trims/slider are on top of horizontal sliders with a small margin
  auto vertTop = pos - HMARGIN - VERTICAL_SLIDERS_HEIGHT;
  
  // Left side (vertical)
  pos = left();
  sliders[SLIDERS_REAR_LEFT]->setLeft(pos);
  sliders[SLIDERS_REAR_LEFT]->setTop(vertTop);

#if defined(HARDWARE_EXT1)
  sliders[SLIDERS_EXT1]->setLeft(pos);
  if (IS_POT_SLIDER_AVAILABLE(EXT1)) {
    auto rl = sliders[SLIDERS_REAR_LEFT];
    auto e1 = sliders[SLIDERS_EXT1];

    // If EXT1 is configured as a slider,
    // place it bellow rear-left slider (and make them smaller)
    rl->setHeight(VERTICAL_SLIDERS_HEIGHT / 2);
    e1->setTop(vertTop + rl->height() + HMARGIN/2);
    e1->setHeight(rl->height());
  }
  else {
    auto rl = sliders[SLIDERS_REAR_LEFT];
    auto e1 = sliders[SLIDERS_EXT1];

    // Otherwise hide the extra slider and make rear-left fullsize
    rl->setHeight(VERTICAL_SLIDERS_HEIGHT);
    e1->setHeight(0);
  }
#endif

  // Right side (vertical)
  pos = right() - sliders[SLIDERS_REAR_RIGHT]->width();
  sliders[SLIDERS_REAR_RIGHT]->setLeft(pos);
  sliders[SLIDERS_REAR_RIGHT]->setTop(vertTop);

#if defined(HARDWARE_EXT2)
  sliders[SLIDERS_EXT2]->setLeft(pos);
  if (IS_POT_SLIDER_AVAILABLE(EXT2)) {
    auto rr = sliders[SLIDERS_REAR_RIGHT];
    auto e2 = sliders[SLIDERS_EXT2];

    // If EXT2 is configured as a slider,
    // place it bellow rear-left slider (and make them smaller)
    rr->setHeight(VERTICAL_SLIDERS_HEIGHT / 2);
    e2->setTop(vertTop + rr->height() + HMARGIN/2);
    e2->setHeight(rr->height());
  }
  else {
    auto rr = sliders[SLIDERS_REAR_RIGHT];
    auto e2 = sliders[SLIDERS_EXT2];

    // Otherwise hide the extra slider and make rear-left fullsize
    rr->setHeight(VERTICAL_SLIDERS_HEIGHT);
    e2->setHeight(0);
  }
#endif

  // Finally place the vertical trims further from the edge
  trims[TRIMS_LV]->setLeft(sliders[SLIDERS_REAR_LEFT]->right());
  trims[TRIMS_LV]->setTop(vertTop);
  trims[TRIMS_RV]->setLeft(sliders[SLIDERS_REAR_RIGHT]->left() - trims[TRIMS_RV]->width());
  trims[TRIMS_RV]->setTop(vertTop);

  //TODO: find a proper place for the flight-mode text box
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
  createSliders();
  createTrims();
  createFlightMode();
}

void ViewMain::createTopbar()
{
  topbar = new TopBar(this);
  topbar->load();
}

void ViewMain::createSliders()
{
  // TODO:
  //  - layout must become really dynamic (see adjustDecoration())
  //  - components should be created in "folded" state
  //  - it should not be required to call hasXXXX() at all
  
  // fixed size array, so that works
  memset(sliders, 0, sizeof(sliders));
    
  rect_t r = {
    // left
    HMARGIN, 0,
    HORIZONTAL_SLIDERS_WIDTH, 0
  };
    
  sliders[SLIDERS_POT1] = new MainViewHorizontalSlider(this, r, CALIBRATED_POT1);

  r = rect_t { 0, 0, 0, 0 };
  if (IS_POT_MULTIPOS(POT2)) {
    sliders[SLIDERS_POT2] = new MainView6POS(this, r, 1);
  }
  else if (IS_POT(POT2)) {
    sliders[SLIDERS_POT2] = new MainViewHorizontalSlider(this, r, CALIBRATED_POT2);
  }

#if defined(HARDWARE_POT3)
  r = rect_t {
    // right
    width() - HORIZONTAL_SLIDERS_WIDTH - HMARGIN, 0,
    HORIZONTAL_SLIDERS_WIDTH, 0
  };

  sliders[SLIDERS_POT3] = new MainViewHorizontalSlider(this, r, CALIBRATED_POT3);
#endif

  r = rect_t { 0, 0, 0, 0 };
  sliders[SLIDERS_REAR_LEFT] = new MainViewVerticalSlider(this, r, CALIBRATED_SLIDER_REAR_LEFT);
  sliders[SLIDERS_REAR_RIGHT] = new MainViewVerticalSlider(this, r, CALIBRATED_SLIDER_REAR_RIGHT);

#if defined(HARDWARE_EXT1)
  sliders[SLIDERS_EXT1] = new MainViewVerticalSlider(this, r, CALIBRATED_POT_EXT1);
#endif

#if defined(HARDWARE_EXT2)
  sliders[SLIDERS_EXT2] = new MainViewVerticalSlider(this, r, CALIBRATED_POT_EXT2);
#endif
}

void ViewMain::createTrims()
{
  // Trim order TRIM_LH, TRIM_LV, TRIM_RV, TRIM_RH

  rect_t r = {
    left() + HMARGIN, 0,
    HORIZONTAL_SLIDERS_WIDTH, 0
  };
  
  trims[TRIMS_LH] = new MainViewHorizontalTrim(this, r, 0);

  r = rect_t {
    right() - HORIZONTAL_SLIDERS_WIDTH - HMARGIN, 0,
    HORIZONTAL_SLIDERS_WIDTH, 0
  };

  trims[TRIMS_RH] = new MainViewHorizontalTrim(this, r, 3);

  r = rect_t {
    0, 0, 0,
    VERTICAL_SLIDERS_HEIGHT
  }; 

  //r.x = left() + HMARGIN + TRIM_SQUARE_SIZE; // DBG
  trims[TRIMS_LV] = new MainViewVerticalTrim(this, r, 1);

  //r.x = right() - HMARGIN - TRIM_SQUARE_SIZE; // DBG
  trims[TRIMS_RV] = new MainViewVerticalTrim(this, r, 2);
}

void ViewMain::createFlightMode()
{
  rect_t r = {
    // centered text box (50 pixels from either edge)
    // -> re-size once the other components are set
    50, 0, width() - 100, 0
  };

  std::function<std::string()> getFM = []() -> std::string {
    return g_model.flightModeData[mixerCurrentFlightMode].name;
  };

  flightMode = new DynamicText(this, r, getFM, CENTERED);
}
