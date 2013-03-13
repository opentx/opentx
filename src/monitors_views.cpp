/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "open9x.h"

void menuChannelsMonitor(uint8_t event)
{
  switch(event)
  {
    case EVT_KEY_BREAK(KEY_EXIT):
      popMenu();
      return;
  }

  lcd_putsCenter(0*FH, CHANNELS_MONITOR);
  lcd_invert_line(0);

  uint8_t ch = 0;

  for (uint8_t col=0; col<2; col++) {

    uint8_t x = col*LCD_W/2+1;

    // Column separator
    if (col == 1) 
      lcd_vline(x-1, FH, LCD_H-FH);

    // Channels
    for (uint8_t line=0; line<8; line++) {
      uint8_t y = 9+line*7;
      int16_t val = g_chans512[ch];
      uint8_t ofs = (col ? 0 : 1);

      // Channel name if present, number if not
      if (*g_model.limitData[ch].name != '\0')
        lcd_putsnAtt(x+1-ofs, y, g_model.limitData[ch].name, sizeof(g_model.limitData[ch].name), ZCHAR | SMLSIZE);
      else {
        putsChn(x+1-ofs, y, ch+1, SMLSIZE);
      }
      lcd_outdezNAtt(x+18+37-ofs, y+1, calcRESXto1000(val), PREC1 | TINSIZE);

#define WBAR 48

      lcd_rect(x+18+37-ofs, y, WBAR+2, 6);
      uint16_t lim = g_model.extendedLimits ? 640*2 : 512*2;
      uint8_t len = limit((uint8_t)1, uint8_t((abs(val) * WBAR/2 + lim/2) / lim), uint8_t(WBAR/2));
      uint8_t x0 = (val>0) ? x+19+WBAR/2 : x+19+WBAR/2-len;
      x0 = x0 + 37 - ofs;
      lcd_hline(x0, y+1, len);
      lcd_hline(x0, y+2, len);
      lcd_hline(x0, y+3, len);
      lcd_hline(x0, y+4, len);
      ch++;

#undef WBAR
    }
  }
}
