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

void ViewMain::setTrimsVisible(bool visible)
{
  trims[TRIMS_LH]->setHeight(visible ? TRIM_SQUARE_SIZE : 0);
  trims[TRIMS_RH]->setHeight(visible ? TRIM_SQUARE_SIZE : 0);

  trims[TRIMS_LV]->setWidth(visible ? TRIM_SQUARE_SIZE : 0);
  trims[TRIMS_RV]->setWidth(visible ? TRIM_SQUARE_SIZE : 0);
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

void ViewMain::setFlightModeVisible(bool visible)
{
  flightMode->setHeight(visible ? 20 : 0);
}

void ViewMain::adjustDecoration()
{
  // TODO:
  //  -> re-compute all the components' positions
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
  
#if defined(HARDWARE_EXT1) || defined(HARDWARE_EXT2)
  coord_t yOffset = /*(hasTrims() ?*/ - TRIM_SQUARE_SIZE /*: 0)*/ + /*(hasTopbar() ?*/ TOPBAR_HEIGHT / 2 /*: 0)*/;
#endif

  // fixed size array, so that works
  memset(sliders, 0, sizeof(sliders));
    
  rect_t r = {
    HMARGIN,
    LCD_H - TRIM_SQUARE_SIZE,
    HORIZONTAL_SLIDERS_WIDTH,
    TRIM_SQUARE_SIZE
  };
    
  sliders[SLIDERS_POT1] = new MainViewHorizontalSlider(this, r, CALIBRATED_POT1);

  if (IS_POT_MULTIPOS(POT2)) {

    r = rect_t {
      LCD_W / 2 - MULTIPOS_W / 2,
      LCD_H - TRIM_SQUARE_SIZE,
      MULTIPOS_W + 1,
      MULTIPOS_H
    };

    sliders[SLIDERS_POT2] = new MainView6POS(this, r, 1);
  }
  else if (IS_POT(POT2)) {

    r = {
      LCD_W - HORIZONTAL_SLIDERS_WIDTH - HMARGIN,
      LCD_H - TRIM_SQUARE_SIZE,
      HORIZONTAL_SLIDERS_WIDTH,
      TRIM_SQUARE_SIZE
    };
      
    sliders[SLIDERS_POT2] = new MainViewHorizontalSlider(this, r, CALIBRATED_POT2);
  }

#if defined(HARDWARE_POT3)
  r = rect_t {
    LCD_W - HORIZONTAL_SLIDERS_WIDTH - HMARGIN,
    LCD_H - TRIM_SQUARE_SIZE,
    HORIZONTAL_SLIDERS_WIDTH,
    TRIM_SQUARE_SIZE
  };

  sliders[SLIDERS_POT3] = new MainViewHorizontalSlider(this, r, CALIBRATED_POT3);
#endif

#if defined(HARDWARE_EXT1)
  if (IS_POT_SLIDER_AVAILABLE(EXT1)) {
    r = rect_t {
      HMARGIN,
      LCD_H / 2 - VERTICAL_SLIDERS_HEIGHT(true/*hasTopbar()*/) / 2 + yOffset,
      TRIM_SQUARE_SIZE,
      VERTICAL_SLIDERS_HEIGHT(true/*hasTopbar()*/) / 2
    };

    sliders[SLIDERS_REAR_LEFT] = new MainViewVerticalSlider(this, r, CALIBRATED_SLIDER_REAR_LEFT);

    r = rect_t {
      HMARGIN,
      LCD_H / 2 + yOffset,
      TRIM_SQUARE_SIZE,
      VERTICAL_SLIDERS_HEIGHT(true/*hasTopbar()*/) / 2
    };

    sliders[SLIDERS_EXT1] = new MainViewVerticalSlider(this, r, CALIBRATED_POT_EXT1);
  }
  else {
    r = rect_t {
      HMARGIN,
      LCD_H / 2 - VERTICAL_SLIDERS_HEIGHT(true/*hasTopbar()*/) / 2 + yOffset,
      TRIM_SQUARE_SIZE,
      VERTICAL_SLIDERS_HEIGHT(true/*hasTopbar()*/)
    }; 

    sliders[SLIDERS_REAR_LEFT] = new MainViewVerticalSlider(this, r, CALIBRATED_SLIDER_REAR_LEFT);
  }
#endif

#if defined(HARDWARE_EXT2)
  if (IS_POT_SLIDER_AVAILABLE(EXT2)) {
    r = rect_t {
      LCD_W - HMARGIN - TRIM_SQUARE_SIZE,
      LCD_H / 2 - VERTICAL_SLIDERS_HEIGHT(true/*hasTopbar()*/) / 2 + yOffset,
      TRIM_SQUARE_SIZE,
      VERTICAL_SLIDERS_HEIGHT(true/*hasTopbar()*/) / 2
    };

    sliders[SLIDERS_REAR_RIGHT] = new MainViewVerticalSlider(this, r, CALIBRATED_SLIDER_REAR_RIGHT);
      
    r = rect_t {
      LCD_W - HMARGIN - TRIM_SQUARE_SIZE,
      LCD_H / 2 + yOffset,
      TRIM_SQUARE_SIZE,
      VERTICAL_SLIDERS_HEIGHT(true/*hasTopbar()*/) / 2
    };

    sliders[SLIDERS_EXT2] = new MainViewVerticalSlider(this, r, CALIBRATED_POT_EXT2);
  }
  else {
    r = rect_t {
      LCD_W - HMARGIN - TRIM_SQUARE_SIZE,
      LCD_H / 2 - VERTICAL_SLIDERS_HEIGHT(true/*hasTopbar()*/) / 2 + yOffset,
      TRIM_SQUARE_SIZE,
      VERTICAL_SLIDERS_HEIGHT(true/*hasTopbar()*/)
    };

    sliders[SLIDERS_REAR_RIGHT] = new MainViewVerticalSlider(this, r, CALIBRATED_SLIDER_REAR_RIGHT);
  }
#endif
}

void ViewMain::createTrims()
{
#if defined(HARDWARE_POT3) || defined(HARDWARE_EXT1)
  coord_t xOffset = /*hasSliders() ?*/ TRIM_SQUARE_SIZE /*: 0*/;
#else
  coord_t xOffset = 0;
#endif
  coord_t yOffset = /*hasTrims() ?*/ - TRIM_SQUARE_SIZE /*: 0)*/;

  // Trim order TRIM_LH, TRIM_LV, TRIM_RV, TRIM_RH

  rect_t r = {
    HMARGIN,
    LCD_H - TRIM_SQUARE_SIZE + yOffset,
    HORIZONTAL_SLIDERS_WIDTH,
    TRIM_SQUARE_SIZE
  };
  
  trims[TRIMS_LH] = new MainViewHorizontalTrim(this, r, 0);

  r = rect_t {
    LCD_W - HORIZONTAL_SLIDERS_WIDTH - HMARGIN,
    LCD_H - TRIM_SQUARE_SIZE + yOffset,
    HORIZONTAL_SLIDERS_WIDTH,
    TRIM_SQUARE_SIZE
  };

  trims[TRIMS_RH] = new MainViewHorizontalTrim(this, r, 3);

  r = rect_t {
    HMARGIN + xOffset,
    LCD_H /2 - VERTICAL_SLIDERS_HEIGHT(true/*hasTopbar()*/) / 2 + yOffset + /*(hasTopbar() ?*/ TOPBAR_HEIGHT / 2 /*: 0)*/,
    TRIM_SQUARE_SIZE,
    VERTICAL_SLIDERS_HEIGHT(true/*hasTopbar()*/)
  }; 

  trims[TRIMS_LV] = new MainViewVerticalTrim(this, r, 1);

  r = rect_t {
    LCD_W - HMARGIN - TRIM_SQUARE_SIZE - xOffset,
    LCD_H /2 - VERTICAL_SLIDERS_HEIGHT(true/*hasTopbar()*/) / 2 + yOffset + /*(hasTopbar() ?*/ TOPBAR_HEIGHT / 2 /*: 0)*/,
    TRIM_SQUARE_SIZE,
    VERTICAL_SLIDERS_HEIGHT(true/*hasTopbar()*/)
  };

  trims[TRIMS_RV] = new MainViewVerticalTrim(this, r, 2);
}

void ViewMain::createFlightMode()
{
  rect_t r = {
    50,
    LCD_H - 4 - /*(hasSliders() ?*/ 2 * TRIM_SQUARE_SIZE /*: TRIM_SQUARE_SIZE)*/,
    LCD_W - 100,
    20
  };

  std::function<std::string()> getFM = []() -> std::string {
    return g_model.flightModeData[mixerCurrentFlightMode].name;
  };

  flightMode = new DynamicText(this, r, getFM, CENTERED);
}
