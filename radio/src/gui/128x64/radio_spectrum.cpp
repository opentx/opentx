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
  SIMPLE_MENU("SPECTRUM ANALYSER", menuTabGeneral, MENU_RADIO_SPECTRUM, 1);

  if (menuEvent) {
    moduleSettings[INTERNAL_MODULE].mode = MODULE_MODE_NORMAL;
  }
  else if (event == EVT_ENTRY) {
    memclear(reusableBuffer.spectrum.bars, sizeof(reusableBuffer.spectrum.bars));
    moduleSettings[INTERNAL_MODULE].mode = MODULE_MODE_SPECTRUM_ANALYSER;
  }

  uint8_t peak_y = 1;
  uint8_t peak_x = 0;
  for (uint8_t i=0; i<LCD_W; i++) {
    uint8_t h = reusableBuffer.spectrum.bars[i] >> 1;
    if (h > peak_y) {
      peak_x = i;
      peak_y = h;
    }
    lcdDrawSolidVerticalLine(i, LCD_H - h, h);
  }

  lcdDrawText(1, 10, "F:", 0);
  lcdDrawNumber(lcdLastRightPos + 2, 10, reusableBuffer.spectrum.fq/10000000,PREC2);
  lcdDrawText(lcdLastRightPos + 2, 10, "GHz", 0);

  lcdDrawText(1, 10+FH, "S:", 0);
  lcdDrawNumber(lcdLastRightPos + 2, 10+FH, reusableBuffer.spectrum.span/1000000, 0);
  lcdDrawText(lcdLastRightPos + 2, 10+FH, "MHz", 0);

  int8_t y = max<int8_t>(FH, LCD_H - peak_y - FH);
  lcdDrawNumber(min<uint8_t>(100, peak_x), y, ((reusableBuffer.spectrum.fq - reusableBuffer.spectrum.span / 2) + peak_x * (reusableBuffer.spectrum.span / 128)) / 1000000, TINSIZE);
  lcdDrawText(lcdLastRightPos, y, "M", TINSIZE);
}
