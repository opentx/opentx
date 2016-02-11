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

const Theme * registeredThemes[MAX_REGISTERED_THEMES]; // TODO dynamic
unsigned int countRegisteredThemes = 0;
void registerTheme(const Theme * theme)
{
  if (countRegisteredThemes < MAX_REGISTERED_THEMES) {
    registeredThemes[countRegisteredThemes++] = theme;
  }
}

const Theme * getTheme(const char * name)
{
  for (unsigned int i=0; i<countRegisteredThemes; i++) {
    const Theme * theme = registeredThemes[i];
    if (!strcmp(name, theme->getName())) {
      return theme;
    }
  }
  return NULL;
}

