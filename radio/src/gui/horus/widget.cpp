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

const WidgetFactory * registeredWidgets[MAX_REGISTERED_WIDGETS]; // TODO dynamic
unsigned int countRegisteredWidgets = 0;
void registerWidget(const WidgetFactory * factory)
{
  if (countRegisteredWidgets < MAX_REGISTERED_WIDGETS) {
    registeredWidgets[countRegisteredWidgets++] = factory;
  }
}

const WidgetFactory * getWidgetFactory(const char * name)
{
  for (unsigned int i=0; i<countRegisteredWidgets; i++) {
    const WidgetFactory * factory = registeredWidgets[i];
    if (!strcmp(name, factory->getName())) {
      return factory;
    }
  }
  return NULL;
}

Widget * loadWidget(const char * name, const Zone & zone, Widget::PersistentData * persistentData)
{
  const WidgetFactory * factory = getWidgetFactory(name);
  if (factory) {
    return factory->create(zone, persistentData, false);
  }
  return NULL;
}
