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

void menuRadioSpectrum(event_t event)
{
  MENU("SPECTRUM", menuTabGeneral, MENU_RADIO_SPECTRUM, HEADER_LINE, { HEADER_LINE_COLUMNS });

  if (event == EVT_ENTRY) {
    memclear(reusableBuffer.spectrum.bars, sizeof(reusableBuffer.spectrum.bars));
    moduleSettings[INTERNAL_MODULE].mode = MODULE_MODE_SPECTRUM;
  }

  for (uint8_t i=0; i<LCD_W; i++) {
    uint8_t h = reusableBuffer.spectrum.bars[i] >> 2;
    lcdDrawSolidVerticalLine(i, LCD_H - h, h);
  }
}
