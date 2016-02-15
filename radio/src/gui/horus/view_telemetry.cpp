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

#include "../../opentx.h"

#define STATUS_BAR_Y     (7*FH+1)
#define TELEM_2ND_COLUMN (11*10)

void displayTelemetryScreen(int index, unsigned int evt)
{
#if defined(LUA)
  if (TELEMETRY_SCREEN_TYPE(index) == TELEMETRY_SCREEN_TYPE_SCRIPT) {
    luaTask(evt, RUN_TELEM_FG_SCRIPT, true);
    return;
  }
#endif

  if (TELEMETRY_SCREEN_TYPE(index) == TELEMETRY_SCREEN_TYPE_NONE) {
    return;
  }
}

NOINLINE uint8_t getRssiAlarmValue(uint8_t alarm)
{
  return (45 - 3*alarm + g_model.frsky.rssiAlarms[alarm].value);
}
