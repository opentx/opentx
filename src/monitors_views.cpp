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
 * opentx is based on code named
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

#include "opentx.h"

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

  // Column separator
  lcd_vline(LCD_W/2, FH, LCD_H-FH);

  uint8_t ch = 0;

  for (uint8_t col=0; col<2; col++) {

    uint8_t x = col*LCD_W/2+1;

    // Channels
    for (uint8_t line=0; line<8; line++) {
      uint8_t y = 9+line*7;
      int32_t val = channelOutputs[ch];
      uint8_t ofs = (col ? 0 : 1);

      // Channel name if present, number if not
      uint8_t lenLabel = zlen(g_model.limitData[ch].name, sizeof(g_model.limitData[ch].name));
      if (lenLabel > 0)
        lcd_putsnAtt(x+1-ofs, y, g_model.limitData[ch].name, sizeof(g_model.limitData[ch].name), ZCHAR | SMLSIZE);
      else
        putsChn(x+1-ofs, y, ch+1, SMLSIZE);

      uint8_t wbar = (lenLabel > 4 ? 48 : 58);

      // Value
      lcd_outdezNAtt(x+LCD_W/2-3-wbar-ofs, y+1, calcRESXto1000(val), PREC1 | TINSIZE);

      // Gauge
      lcd_rect(x+LCD_W/2-3-wbar-ofs, y, wbar+1, 6);
      uint16_t lim = g_model.extendedLimits ? 640*2 : 512*2;
      uint8_t len = limit((uint8_t)1, uint8_t((abs(val) * wbar/2 + lim/2) / lim), uint8_t(wbar/2));
      uint8_t x0 = (val>0) ? x+LCD_W/2-ofs-3-wbar/2 : x+LCD_W/2-ofs-2-wbar/2-len;
      lcd_hline(x0, y+1, len);
      lcd_hline(x0, y+2, len);
      lcd_hline(x0, y+3, len);
      lcd_hline(x0, y+4, len);

      ch++;
    }
  }
}
