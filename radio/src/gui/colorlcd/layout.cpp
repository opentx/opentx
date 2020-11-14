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
  std::list<const LayoutFactory *>::const_iterator it = getRegisteredLayouts().cbegin();
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

void loadCustomScreens()
{
  for (unsigned int i = 0; i < MAX_CUSTOM_SCREENS; i++) {
    delete customScreens[i];
    char name[LAYOUT_NAME_LEN + 1];
    memset(name, 0, sizeof(name));
    strncpy(name, g_model.screenData[i].layoutName, LAYOUT_NAME_LEN);
    customScreens[i] = loadLayout(name, &g_model.screenData[i].layoutData);
  }

  if (customScreens[0] == nullptr && getRegisteredLayouts().size()) {
    customScreens[0] = getRegisteredLayouts().front()->create(&g_model.screenData[0].layoutData);
  }

  customScreens[g_model.view]->attach(ViewMain::instance);
}

void Layout::decorate(bool topbar, bool sliders, bool trims, bool flightMode)
{
  if (topbar) {
    topBar = new TopBar(this);
    topBar->load();
  }

  if (sliders) {
#if defined(HARDWARE_EXT1) || defined(HARDWARE_EXT2)
    coord_t yOffset = (trims ? - TRIM_SQUARE_SIZE : 0) + (topbar ? TOPBAR_HEIGHT / 2 : 0);
#endif

    new MainViewHorizontalSlider(this, {HMARGIN, LCD_H - TRIM_SQUARE_SIZE, HORIZONTAL_SLIDERS_WIDTH, TRIM_SQUARE_SIZE},
                                 [=] { return calibratedAnalogs[CALIBRATED_POT1]; });

    if (IS_POT_MULTIPOS(POT2)) {
      new MainView6POS(this, {LCD_W / 2 - MULTIPOS_W / 2, LCD_H - TRIM_SQUARE_SIZE, MULTIPOS_W + 1, MULTIPOS_H},
                       [=] { return (1 + (potsPos[1] & 0x0f)); });
    }
    else if (IS_POT(POT2)) {
      new MainViewHorizontalSlider(this, {LCD_W - HORIZONTAL_SLIDERS_WIDTH - HMARGIN, LCD_H - TRIM_SQUARE_SIZE, HORIZONTAL_SLIDERS_WIDTH, TRIM_SQUARE_SIZE},
                                   [=] { return calibratedAnalogs[CALIBRATED_POT2]; });
    }

#if defined(HARDWARE_POT3)
    new MainViewHorizontalSlider(this, {LCD_W - HORIZONTAL_SLIDERS_WIDTH - HMARGIN, LCD_H - TRIM_SQUARE_SIZE, HORIZONTAL_SLIDERS_WIDTH, TRIM_SQUARE_SIZE},
                                 [=] { return calibratedAnalogs[CALIBRATED_POT3]; });
#endif

#if defined(HARDWARE_EXT1)
    if (IS_POT_SLIDER_AVAILABLE(EXT1)) {
      new MainViewVerticalSlider(this, {HMARGIN, LCD_H / 2 - VERTICAL_SLIDERS_HEIGHT(topbar) / 2 + yOffset, TRIM_SQUARE_SIZE, VERTICAL_SLIDERS_HEIGHT(topbar) / 2},
                                 [=] { return calibratedAnalogs[CALIBRATED_SLIDER_REAR_LEFT]; });
      new MainViewVerticalSlider(this, {HMARGIN, LCD_H / 2 + yOffset, TRIM_SQUARE_SIZE, VERTICAL_SLIDERS_HEIGHT(topbar) / 2},
                                 [=] { return calibratedAnalogs[CALIBRATED_POT_EXT1]; });
    }
    else {
      new MainViewVerticalSlider(this, {HMARGIN, LCD_H / 2 - VERTICAL_SLIDERS_HEIGHT(topbar) / 2 + yOffset, TRIM_SQUARE_SIZE, VERTICAL_SLIDERS_HEIGHT(topbar)},
                                 [=] { return calibratedAnalogs[CALIBRATED_SLIDER_REAR_LEFT]; });
    }
#endif

#if defined(HARDWARE_EXT2)
    if (IS_POT_SLIDER_AVAILABLE(EXT2)) {
      new MainViewVerticalSlider(this, {LCD_W - HMARGIN - TRIM_SQUARE_SIZE, LCD_H / 2 - VERTICAL_SLIDERS_HEIGHT(topbar) / 2 + yOffset, TRIM_SQUARE_SIZE,
                                        VERTICAL_SLIDERS_HEIGHT(topbar) / 2},
                                 [=] { return calibratedAnalogs[CALIBRATED_SLIDER_REAR_RIGHT]; });
      new MainViewVerticalSlider(this, {LCD_W - HMARGIN - TRIM_SQUARE_SIZE, LCD_H / 2 + yOffset, TRIM_SQUARE_SIZE,
                                        VERTICAL_SLIDERS_HEIGHT(topbar) / 2},
                                 [=] { return calibratedAnalogs[CALIBRATED_POT_EXT2]; });
    }
    else {
      new MainViewVerticalSlider(this, {LCD_W - HMARGIN - TRIM_SQUARE_SIZE, LCD_H / 2 - VERTICAL_SLIDERS_HEIGHT(topbar) / 2 + yOffset, TRIM_SQUARE_SIZE,
                                        VERTICAL_SLIDERS_HEIGHT(topbar)},
                                 [=] { return calibratedAnalogs[CALIBRATED_SLIDER_REAR_RIGHT]; });
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
    new MainViewHorizontalTrim(this, {HMARGIN, LCD_H - TRIM_SQUARE_SIZE + yOffset, HORIZONTAL_SLIDERS_WIDTH, TRIM_SQUARE_SIZE},
                               [=] { return getTrimValue(mixerCurrentFlightMode, 0); });
    // Right
    new MainViewHorizontalTrim(this, {LCD_W - HORIZONTAL_SLIDERS_WIDTH - HMARGIN, LCD_H - TRIM_SQUARE_SIZE + yOffset, HORIZONTAL_SLIDERS_WIDTH, TRIM_SQUARE_SIZE},
                               [=] { return getTrimValue(mixerCurrentFlightMode, 3); });


    // Left
    new MainViewVerticalTrim(this, {HMARGIN + xOffset, LCD_H /2 - VERTICAL_SLIDERS_HEIGHT(topbar) / 2 + yOffset + (topbar ? TOPBAR_HEIGHT / 2 : 0), TRIM_SQUARE_SIZE, VERTICAL_SLIDERS_HEIGHT(topbar)},
                             [=] { return getTrimValue(mixerCurrentFlightMode, 1); });
    // Right
    new MainViewVerticalTrim(this, {LCD_W - HMARGIN - TRIM_SQUARE_SIZE - xOffset, LCD_H /2 - VERTICAL_SLIDERS_HEIGHT(topbar) / 2 + yOffset + (topbar ? TOPBAR_HEIGHT / 2 : 0), TRIM_SQUARE_SIZE, VERTICAL_SLIDERS_HEIGHT(topbar)},
                             [=] { return getTrimValue(mixerCurrentFlightMode, 2); });
  }

  if (flightMode) {
    new DynamicText(this, {50, LCD_H - 4 - (sliders? 2 * TRIM_SQUARE_SIZE: TRIM_SQUARE_SIZE), LCD_W - 100, 20}, [=] {
        return g_model.flightModeData[mixerCurrentFlightMode].name;
    }, CENTERED);
  }
}
