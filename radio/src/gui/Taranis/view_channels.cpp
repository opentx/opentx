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

#include "../../opentx.h"

void menuChannelsView(uint8_t event)
{
  static bool longNames = false;
  bool newLongNames = false;
  static bool secondPage = false;
#ifdef MIXERS_MONITOR
  static bool mixersView = false;
#endif
  uint8_t ch;

  switch(event)
  {
    case EVT_KEY_BREAK(KEY_EXIT):
      popMenu();
      break;
    case EVT_KEY_FIRST(KEY_RIGHT):
    case EVT_KEY_FIRST(KEY_LEFT):
      secondPage = !secondPage;
      break;
#ifdef MIXERS_MONITOR
    case EVT_KEY_FIRST(KEY_ENTER):
      mixersView = !mixersView;
      break;
#endif
  }

  if (secondPage)
    ch = 16;
  else
    ch = 0;

#ifdef MIXERS_MONITOR
  if (mixersView)
  lcd_putsCenter(0*FH, MIXERS_MONITOR);
  else
#endif
  lcd_putsCenter(0*FH, CHANNELS_MONITOR);

  lcd_invert_line(0);

  // Column separator
  lcd_vline(LCD_W/2, FH, LCD_H-FH);

  for (uint8_t col=0; col<2; col++) {

    uint8_t x = col*LCD_W/2+1;

    // Channels
    for (uint8_t line=0; line<8; line++) {
      uint8_t y = 9+line*7;
#ifdef MIXERS_MONITOR
      int32_t val = (mixersView) ? ex_chans[ch] : channelOutputs[ch];
#else
      int32_t val = channelOutputs[ch];
#endif
      uint8_t ofs = (col ? 0 : 1);

      // Channel name if present, number if not
      uint8_t lenLabel = ZLEN(g_model.limitData[ch].name);
      if (lenLabel > 4) {
        newLongNames = longNames = true;
      }

      if (lenLabel > 0)
        lcd_putsnAtt(x+1-ofs, y, g_model.limitData[ch].name, sizeof(g_model.limitData[ch].name), ZCHAR | SMLSIZE);
      else
        putsChn(x+1-ofs, y, ch+1, SMLSIZE);

      // Value
#if defined(PPM_UNIT_US)
      uint8_t wbar = (longNames ? 54 : 64);
      lcd_outdezAtt(x+LCD_W/2-3-wbar-ofs, y+1, PPM_CH_CENTER(ch)+val/2, TINSIZE);
#elif defined(PPM_UNIT_PERCENT_PREC1)
      uint8_t wbar = (longNames ? 48 : 58);
      lcd_outdezAtt(x+LCD_W/2-3-wbar-ofs, y+1, calcRESXto1000(val), PREC1|TINSIZE);
#else
      uint8_t wbar = (longNames ? 54 : 64);
      lcd_outdezAtt(x+LCD_W/2-3-wbar-ofs, y+1, calcRESXto1000(val)/10, TINSIZE); // G: Don't like the decimal part*
#endif

      // Gauge
#ifdef MIXERS_MONITOR
      uint16_t lim = mixersView ? 512*2*2 : (g_model.extendedLimits ? 640*2 : 512*2);
#else
      uint16_t lim = g_model.extendedLimits ? 640*2 : 512*2;
#endif
      drawGauge(x+LCD_W/2-3-wbar-ofs, y, wbar, 6, val, lim);

      ch++;
    }
  }

  longNames = newLongNames;
}
