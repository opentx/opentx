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

void menuChannelsView(event_t event)
{
  static bool longNames = false;
  bool newLongNames = false;
  static bool secondPage = false;
  static bool mixersView = false;

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
    case EVT_KEY_FIRST(KEY_ENTER):
      mixersView = !mixersView;
      break;
  }

  if (secondPage)
    ch = 16;
  else
    ch = 0;

  if (mixersView) {
    lcdDrawTextAlignedCenter(0, MIXERS_MONITOR);
  }
  else {
    lcdDrawTextAlignedCenter(0, CHANNELS_MONITOR);
  }

  lcdInvertLine(0);

  // Column separator
  lcdDrawSolidVerticalLine(LCD_W/2, FH, LCD_H-FH);

  for (uint8_t col=0; col<2; col++) {

    uint8_t x = col*LCD_W/2+1;

    // Channels
    for (uint8_t line=0; line<8; line++) {
      uint8_t y = 9+line*7;
      int32_t val = (mixersView) ? ex_chans[ch] : channelOutputs[ch];
      uint8_t ofs = (col ? 0 : 1);

      // Channel name if present, number if not
      uint8_t lenLabel = ZLEN(g_model.limitData[ch].name);
      if (lenLabel > 4) {
        newLongNames = longNames = true;
      }

      if (lenLabel > 0)
        lcdDrawSizedText(x+1-ofs, y, g_model.limitData[ch].name, sizeof(g_model.limitData[ch].name), ZCHAR | SMLSIZE);
      else
        putsChn(x+1-ofs, y, ch+1, SMLSIZE);

      // Value
#if defined(PPM_UNIT_US)
      uint8_t wbar = (longNames ? 54 : 64);
      lcdDrawNumber(x+LCD_W/2-3-wbar-ofs, y+1, PPM_CH_CENTER(ch)+val/2, TINSIZE|RIGHT);
#elif defined(PPM_UNIT_PERCENT_PREC1)
      uint8_t wbar = (longNames ? 48 : 58);
      lcdDrawNumber(x+LCD_W/2-3-wbar-ofs, y+1, calcRESXto1000(val), PREC1|TINSIZE|RIGHT);
#else
      uint8_t wbar = (longNames ? 54 : 64);
      lcdDrawNumber(x+LCD_W/2-3-wbar-ofs, y+1, calcRESXto1000(val)/10, TINSIZE|RIGHT);
#endif

      // Gauge
//      uint16_t lim = (g_model.extendedLimits ? (512 * (long)LIMIT_EXT_PERCENT / 100) : 512) * 2;
//#ifdef MIXERS_MONITOR
//      if (mixersView)
//        lim = 512 * 2 * 2;
//#endif
      // TODO ? drawGauge(x+LCD_W/2-3-wbar-ofs, y, wbar, 6, val, lim);

      ch++;
    }
  }

  longNames = newLongNames;
}
