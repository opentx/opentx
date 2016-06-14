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

bool menuChannels1(evt_t event);
bool menuChannels2(evt_t event);

const MenuHandlerFunc menuTabChannels[] PROGMEM = {
  menuChannels1,
  menuChannels2,
};

uint8_t lastChannelsPage = 0;

bool menuChannelsMonitor(evt_t event, uint8_t page)
{
  return true;
}

bool menuChannels1(evt_t event)
{
  MENU("Channels monitor 1-16", STATS_ICONS, menuTabChannels, e_Channels1, 0, { 0 });
  lastChannelsPage = e_Channels1;
  return menuChannelsMonitor(event, 0);
}

bool menuChannels2(evt_t event)
{
  MENU("Channels monitor 17-32", STATS_ICONS, menuTabChannels, e_Channels2, 0, { 0 });
  lastChannelsPage = e_Channels2;
  return menuChannelsMonitor(event, 1);
}
