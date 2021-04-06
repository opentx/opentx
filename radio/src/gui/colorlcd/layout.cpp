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
#include "view_main.h"
#include "layouts/trims.h"
#include "layouts/sliders.h"

std::list<const LayoutFactory *> & getRegisteredLayouts()
{
  static std::list<const LayoutFactory *> layouts;
  return layouts;
}

void registerLayout(const LayoutFactory * factory)
{
  TRACE("register layout %s", factory->getId());
  getRegisteredLayouts().push_back(factory);
}

const LayoutFactory * getLayoutFactory(const char * name)
{
  auto it = getRegisteredLayouts().cbegin();
  for (; it != getRegisteredLayouts().cend(); ++it) {
    if (!strcmp(name, (*it)->getId())) {
      return (*it);
    }
  }
  return nullptr;
}

Layout * loadLayout(const char * name, Layout::PersistentData * persistentData)
{
  const LayoutFactory * factory = getLayoutFactory(name);
  if (factory) {
    return factory->load(persistentData);
  }
  return nullptr;
}

void deleteCustomScreens()
{
  for (unsigned int i = 0; i < MAX_CUSTOM_SCREENS; i++) {
    auto& screen = customScreens[i];
    if (screen) {
      screen->detach();
      delete screen;
      screen = nullptr;
    }
  }
}

extern const LayoutFactory * defaultLayout;

void loadDefaultLayout()
{
  auto& screen = customScreens[0];
  auto& screenData = g_model.screenData[0];

  if (screen == nullptr && defaultLayout != nullptr) {
    strcpy(screenData.layoutName, defaultLayout->getName());
    screen = defaultLayout->create(&screenData.layoutData);
    if (screen) {
      screen->attach(ViewMain::instance());
    }
  }
}

void loadCustomScreens()
{
  for (unsigned int i = 0; i < MAX_CUSTOM_SCREENS; i++) {

    auto& screen = customScreens[i];
    screen = loadLayout(g_model.screenData[i].layoutName,
                        &g_model.screenData[i].layoutData);

    if (screen) {
      screen->attach(ViewMain::instance());
    }
  }
}

void Layout::decorate(bool topbar, bool sliders, bool trims, bool flightMode)
{
  // check if deco setting are still up-to-date
  uint8_t checkSettings =
    (topbar ? 1 << 0 : 0) |
    (sliders ? 1 << 1 : 0) |
    (trims ? 1 << 2 : 0) |
    (flightMode ? 1 << 3 : 0);

  if (checkSettings == decorationSettings) {
    // everything ok, exit!
    return;
  }

  // kill all decorations & re-decorate
  for (auto deco: decorations) {
    deco->deleteLater();
  }
  decorations.clear();
  topBar = nullptr;

  // effect a full redraw
  invalidate();

  // save settings
  decorationSettings = checkSettings;
  
  if (topbar) {
    topBar = new TopBar(this);
    topBar->load();
    decorations.push_back(topBar);
  }

  if (sliders) {
#if defined(HARDWARE_EXT1) || defined(HARDWARE_EXT2)
    coord_t yOffset = (trims ? - TRIM_SQUARE_SIZE : 0) + (topbar ? TOPBAR_HEIGHT / 2 : 0);
#endif

    decorations.push_back(
      new MainViewHorizontalSlider(this, {HMARGIN, LCD_H - TRIM_SQUARE_SIZE, HORIZONTAL_SLIDERS_WIDTH, TRIM_SQUARE_SIZE},
                                   [=] { return calibratedAnalogs[CALIBRATED_POT1]; })
    );

    if (IS_POT_MULTIPOS(POT2)) {
      decorations.push_back(
        new MainView6POS(this, {LCD_W / 2 - MULTIPOS_W / 2, LCD_H - TRIM_SQUARE_SIZE, MULTIPOS_W + 1, MULTIPOS_H},
                         [=] { return (1 + (potsPos[1] & 0x0f)); })
      );
    }
    else if (IS_POT(POT2)) {
      decorations.push_back(
        new MainViewHorizontalSlider(this, {LCD_W - HORIZONTAL_SLIDERS_WIDTH - HMARGIN, LCD_H - TRIM_SQUARE_SIZE, HORIZONTAL_SLIDERS_WIDTH, TRIM_SQUARE_SIZE},
                                     [=] { return calibratedAnalogs[CALIBRATED_POT2]; })
      );
    }

#if defined(HARDWARE_POT3)
    decorations.push_back(
      new MainViewHorizontalSlider(this, {LCD_W - HORIZONTAL_SLIDERS_WIDTH - HMARGIN, LCD_H - TRIM_SQUARE_SIZE, HORIZONTAL_SLIDERS_WIDTH, TRIM_SQUARE_SIZE},
                                   [=] { return calibratedAnalogs[CALIBRATED_POT3]; })
    );
#endif

#if defined(HARDWARE_EXT1)
    if (IS_POT_SLIDER_AVAILABLE(EXT1)) {
      decorations.push_back(
        new MainViewVerticalSlider(this, {HMARGIN, LCD_H / 2 - VERTICAL_SLIDERS_HEIGHT(topbar) / 2 + yOffset, TRIM_SQUARE_SIZE, VERTICAL_SLIDERS_HEIGHT(topbar) / 2},
                                   [=] { return calibratedAnalogs[CALIBRATED_SLIDER_REAR_LEFT]; })
      );
      decorations.push_back(
        new MainViewVerticalSlider(this, {HMARGIN, LCD_H / 2 + yOffset, TRIM_SQUARE_SIZE, VERTICAL_SLIDERS_HEIGHT(topbar) / 2},
                                   [=] { return calibratedAnalogs[CALIBRATED_POT_EXT1]; })
      );
    }
    else {
      decorations.push_back(
        new MainViewVerticalSlider(this, {HMARGIN, LCD_H / 2 - VERTICAL_SLIDERS_HEIGHT(topbar) / 2 + yOffset, TRIM_SQUARE_SIZE, VERTICAL_SLIDERS_HEIGHT(topbar)},
                                   [=] { return calibratedAnalogs[CALIBRATED_SLIDER_REAR_LEFT]; })
      );
    }
#endif

#if defined(HARDWARE_EXT2)
    if (IS_POT_SLIDER_AVAILABLE(EXT2)) {
      decorations.push_back(
        new MainViewVerticalSlider(this, {LCD_W - HMARGIN - TRIM_SQUARE_SIZE, LCD_H / 2 - VERTICAL_SLIDERS_HEIGHT(topbar) / 2 + yOffset, TRIM_SQUARE_SIZE,
                                        VERTICAL_SLIDERS_HEIGHT(topbar) / 2},
                                   [=] { return calibratedAnalogs[CALIBRATED_SLIDER_REAR_RIGHT]; })
      );
      decorations.push_back(
        new MainViewVerticalSlider(this, {LCD_W - HMARGIN - TRIM_SQUARE_SIZE, LCD_H / 2 + yOffset, TRIM_SQUARE_SIZE,
                                        VERTICAL_SLIDERS_HEIGHT(topbar) / 2},
                                   [=] { return calibratedAnalogs[CALIBRATED_POT_EXT2]; })
      );
    }
    else {
      decorations.push_back(
        new MainViewVerticalSlider(this, {LCD_W - HMARGIN - TRIM_SQUARE_SIZE, LCD_H / 2 - VERTICAL_SLIDERS_HEIGHT(topbar) / 2 + yOffset, TRIM_SQUARE_SIZE,
                                        VERTICAL_SLIDERS_HEIGHT(topbar)},
                                   [=] { return calibratedAnalogs[CALIBRATED_SLIDER_REAR_RIGHT]; })
      );
    }
#endif
  }

  if (trims) {
#if defined(HARDWARE_POT3) || defined(HARDWARE_EXT1)
    coord_t xOffset = sliders? TRIM_SQUARE_SIZE : 0;
#else
    coord_t xOffset = 0;
#endif
    coord_t yOffset = (trims ? - TRIM_SQUARE_SIZE : 0);

    // Trim order TRIM_LH, TRIM_LV, TRIM_RV, TRIM_RH

    // Left
    decorations.push_back(
      new MainViewHorizontalTrim(this, {HMARGIN, LCD_H - TRIM_SQUARE_SIZE + yOffset, HORIZONTAL_SLIDERS_WIDTH, TRIM_SQUARE_SIZE},
                                 [=] { return getTrimValue(mixerCurrentFlightMode, 0); })
    );

    // Right
    decorations.push_back(
      new MainViewHorizontalTrim(this, {LCD_W - HORIZONTAL_SLIDERS_WIDTH - HMARGIN, LCD_H - TRIM_SQUARE_SIZE + yOffset, HORIZONTAL_SLIDERS_WIDTH, TRIM_SQUARE_SIZE},
                                 [=] { return getTrimValue(mixerCurrentFlightMode, 3); })
    );

    // Left
    decorations.push_back(
      new MainViewVerticalTrim(this, {HMARGIN + xOffset, LCD_H /2 - VERTICAL_SLIDERS_HEIGHT(topbar) / 2 + yOffset + (topbar ? TOPBAR_HEIGHT / 2 : 0), TRIM_SQUARE_SIZE, VERTICAL_SLIDERS_HEIGHT(topbar)},
                               [=] { return getTrimValue(mixerCurrentFlightMode, 1); })
    );

    // Right
    decorations.push_back(
      new MainViewVerticalTrim(this, {LCD_W - HMARGIN - TRIM_SQUARE_SIZE - xOffset, LCD_H /2 - VERTICAL_SLIDERS_HEIGHT(topbar) / 2 + yOffset + (topbar ? TOPBAR_HEIGHT / 2 : 0), TRIM_SQUARE_SIZE, VERTICAL_SLIDERS_HEIGHT(topbar)},
                               [=] { return getTrimValue(mixerCurrentFlightMode, 2); })
    );
  }

  if (flightMode) {
    decorations.push_back(
      new DynamicText(this, {50, LCD_H - 4 - (sliders? 2 * TRIM_SQUARE_SIZE: TRIM_SQUARE_SIZE), LCD_W - 100, 20}, [=] {
        return g_model.flightModeData[mixerCurrentFlightMode].name;
      }, CENTERED)
    );
  }
}
