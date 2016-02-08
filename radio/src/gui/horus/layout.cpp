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

void Layout::setWidget(unsigned int index, const char * name)
{
  memset(persistentData->zones[index].widgetName, 0, sizeof(persistentData->zones[index].widgetName));
  if (name) {
    strncpy(persistentData->zones[index].widgetName, name, sizeof(persistentData->zones[index].widgetName));
    widgets[index] = createWidget(name, getZone(index), &persistentData->zones[index].widgetData);
  }
  else {
    widgets[index] = NULL;
  }
}

void Layout::create()
{
  memset(persistentData, 0, sizeof(PersistentData));
}

void Layout::load()
{
  unsigned int count = getZonesCount();
  for (unsigned int i=0; i<count; i++) {
    char name[sizeof(persistentData->zones[i].widgetName)+1];
    memset(name, 0, sizeof(name));
    strncpy(name, persistentData->zones[i].widgetName, sizeof(persistentData->zones[i].widgetName));
    widgets[i] = loadWidget(name, getZone(i), &persistentData->zones[i].widgetData);
  }
}

void Layout::refresh(bool setup)
{
  if (setup) {
    lcdDrawBlackOverlay();
  }

  for (int i = 0; i < MAX_LAYOUT_ZONES; i++) {
    if (widgets[i]) {
      widgets[i]->refresh();
    }
  }
}

const LayoutFactory * registeredLayouts[MAX_REGISTERED_LAYOUTS]; // TODO dynamic
unsigned int countRegisteredLayouts = 0;
void registerLayout(const LayoutFactory * factory)
{
  if (countRegisteredLayouts < MAX_REGISTERED_LAYOUTS) {
    registeredLayouts[countRegisteredLayouts++] = factory;
  }
}

const LayoutFactory * getLayoutFactory(const char * name)
{
  for (unsigned int i=0; i<countRegisteredLayouts; i++) {
    const LayoutFactory * factory = registeredLayouts[i];
    if (!strcmp(name, factory->getName())) {
      return factory;
    }
  }
  return NULL;
}

Layout * createLayout(const char * name, Layout::PersistentData * persistentData)
{
  const LayoutFactory * factory = getLayoutFactory(name);
  if (factory) {
    return factory->create(persistentData);
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

