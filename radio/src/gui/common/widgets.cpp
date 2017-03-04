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

void lcdDrawMMM(coord_t x, coord_t y, LcdFlags flags)
{
  lcdDrawTextAtIndex(x, y, STR_MMMINV, 0, flags);
}

#if defined(FLIGHT_MODES)
void drawFlightMode(coord_t x, coord_t y, int8_t idx, LcdFlags att)
{
  if (idx==0) {
    lcdDrawMMM(x, y, att);
    return;
  }
  // TODO this code was not included in Taranis! and used with abs(...) on Horus
  if (idx < 0) {
    lcdDrawChar(x-2, y, '!', att);
    idx = -idx;
  }
#if defined(CONDENSED)
  if (att & CONDENSED) {
    lcdDrawNumber(x+FW*1, y, idx-1, (att & ~CONDENSED), 1);
    return;
  }
#endif
  drawStringWithIndex(x, y, STR_FP, idx-1, att);
}
#endif
