/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
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

  for (uint8_t col=0; col<4; col++) {

    uint8_t x = col*LCD_W/4;

    // Column title
    lcd_outdezNAtt(col==0 ? 28 : (col == 1 ? 81 : x+LCD_W/8-5*FWNUM/2+9), 1*FH+1, ch+1, LEFT|SMLSIZE);
    lcd_putcAtt(lcdLastPos-1, 1*FH+1, '-', SMLSIZE);
    lcd_outdezNAtt(lcdLastPos+FW-1, 1*FH+1, ch+8, LEFT|SMLSIZE);

    // Column separator
    if (col != 0)
      lcd_vline(x, FH, LCD_H-FH);

    // Channels
    for (uint8_t line=0; line<8; line++) {
      uint8_t y = 16+line*6;
      int16_t val = g_chans512[ch];

      lcd_outdezNAtt(x+18, y, calcRESXto100(val), TINSIZE);

#define WBAR 32

      lcd_rect(x+18, y, WBAR+2, 5);
      uint16_t lim = g_model.extendedLimits ? 640*2 : 512*2;
      uint8_t len = limit((uint8_t)1, uint8_t((abs(val) * WBAR/2 + lim/2) / lim), uint8_t(WBAR/2));
      uint8_t x0 = (val>0) ? x+19+WBAR/2 : x+19+WBAR/2-len;
      lcd_hline(x0, y+1, len);
      lcd_hline(x0, y+2, len);
      lcd_hline(x0, y+3, len);
      ch++;

#undef WBAR
    }
  }
}
