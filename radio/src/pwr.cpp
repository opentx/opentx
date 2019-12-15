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

#include "board.h"

bool pwrOnShouldBeImmediate()
{
#if defined(WAS_RESET_BY_WATCHDOG_OR_SOFTWARE)
  if (WAS_RESET_BY_WATCHDOG_OR_SOFTWARE()) {
    return true;
  }
#endif
  // We need to power on immediately when USB is plugged, so
  // we can get out of ST's bootloader without user intervention.
  return usbPlugged();
}

bool pwrOnIfImmediate()
{
  if (pwrOnShouldBeImmediate()) {
    pwrOn();
    return true;
  }
  return false;
}
