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

std::list<const LayoutFactory *> & getRegisteredLayouts()
{
  static std::list<const LayoutFactory *> layouts;
  return layouts;
}

void registerLayout(const LayoutFactory * factory)
{
  TRACE("register layout %s", factory->getName());
  getRegisteredLayouts().push_back(factory);
}

const LayoutFactory * getLayoutFactory(const char * name)
{
  std::list<const LayoutFactory *>::const_iterator it = getRegisteredLayouts().cbegin();
  for (; it != getRegisteredLayouts().cend(); ++it) {
    if (!strcmp(name, (*it)->getName())) {
      return (*it);
    }
  }
  return NULL;
}

Layout * loadLayout(const char * name, Layout::PersistentData * persistentData)
{
  const LayoutFactory * factory = getLayoutFactory(name);
  if (factory) {
    return factory->load(persistentData);
  }
  return NULL;
}

void loadCustomScreens()
{
  for (unsigned int i=0; i<MAX_CUSTOM_SCREENS; i++) {
    delete customScreens[i];
    char name[sizeof(g_model.screenData[i].layoutName)+1];
    memset(name, 0, sizeof(name));
    strncpy(name, g_model.screenData[i].layoutName, sizeof(g_model.screenData[i].layoutName));
    customScreens[i] = loadLayout(name, &g_model.screenData[i].layoutData);
  }

  if (customScreens[0] == NULL && getRegisteredLayouts().size()) {
    customScreens[0] = getRegisteredLayouts().front()->create(&g_model.screenData[0].layoutData);
  }

  topbar->load();
}
