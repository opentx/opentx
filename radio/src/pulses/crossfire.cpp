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

#include "../opentx.h"

#define CROSSFIRE_START_BYTE        0x0F
#define CROSSFIRE_CH_CENTER         0x3E0
#define CROSSFIRE_CH_BITS           11

// Range for pulses (channels output) is [-1024:+1024]
void createCrossfireFrame(uint8_t * frame, int16_t * pulses)
{
  uint8_t * buf = frame;
  *buf++ = CROSSFIRE_START_BYTE;

  uint32_t bits = 0;
  uint8_t bitsavailable = 0;
  for (int i=0; i<CROSSFIRE_CHANNELS_COUNT; i++) {
    bits |= (CROSSFIRE_CH_CENTER + (((pulses[i]) * 4) / 5)) << bitsavailable;
    bitsavailable += CROSSFIRE_CH_BITS;
    while (bitsavailable >= 8) {
      *buf++ = bits;
      bits >>= 8;
      bitsavailable -= 8;
    }
  }
  *buf++ = 0;
  *buf++ = crc16(frame, 24);
}
