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
    strcpy(screenData.LayoutId, defaultLayout->getId());
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
    screen = loadLayout(g_model.screenData[i].LayoutId,
                        &g_model.screenData[i].layoutData);

    if (screen) {
      screen->attach(ViewMain::instance());
    }
  }
}

void Layout::decorate()
{
  // Check if deco setting are still up-to-date
  uint8_t checkSettings =
    (hasTopbar() ? 1 << 0 : 0) |
    (hasSliders() ? 1 << 1 : 0) |
    (hasTrims() ? 1 << 2 : 0) |
    (hasFlightMode() ? 1 << 3 : 0);

  if (checkSettings == decorationSettings) {
    // everything ok, exit!
    return;
  }

  // Save settings
  decorationSettings = checkSettings;

  auto viewMain = ViewMain::instance();

  // Set visible decoration
  viewMain->setTopbarVisible(hasTopbar());
  viewMain->setSlidersVisible(hasSliders());
  viewMain->setTrimsVisible(hasTrims());
  viewMain->setFlightModeVisible(hasFlightMode());

  // Re-compute positions
  viewMain->adjustDecoration();
  viewMain->invalidate();
}

rect_t Layout::getMainZone() const
{
  return ViewMain::instance()->getMainZone();
}    
