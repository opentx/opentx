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

bool isChannelUsed(int channel);
int getChannelsUsed();

void menuChannelsView(evt_t event)
{
  const int CHANNEL_MARGIN = 16;
  const int CHANNEL_HEIGHT = 25;
  const int CHANNEL_WIDTH = (LCD_W - 3*CHANNEL_MARGIN) / 2;
  const int CHANNEL_PADDING_HORZ = 4;
  const int CHANNEL_PADDING_VERT = 2;
  const int BAR_WIDTH = CHANNEL_WIDTH-2*CHANNEL_PADDING_HORZ;

  static int view = 0;
  int x=CHANNEL_MARGIN, y=CHANNEL_MARGIN;

  int viewMax = (getChannelsUsed() - 1) / 8;
  if (viewMax <= 0 || view > viewMax) {
    view = 0;
  }
  else {
    switch (event) {
      case EVT_KEY_FIRST(KEY_RIGHT):
        if (++view > viewMax)
          view = 0;
        break;
      case EVT_KEY_FIRST(KEY_LEFT):
        if (--view < 0)
          view = viewMax;
        break;
    }
  }

  int skipCount = view*8;
  for (int ch=0, index=0; index<skipCount+8 && ch<NUM_CHNOUT; ++ch) {
    if (isChannelUsed(ch)) {
      if (++index > skipCount) {
        // The black background
        lcdDrawFilledRect(x, y, CHANNEL_WIDTH, CHANNEL_HEIGHT, TEXT_BGCOLOR);

        // The label
        unsigned int lenLabel = ZLEN(g_model.limitData[ch].name);
        if (lenLabel > 0)
          lcd_putsnAtt(x+CHANNEL_PADDING_HORZ, y+CHANNEL_PADDING_VERT, g_model.limitData[ch].name, sizeof(g_model.limitData[ch].name), ZCHAR);
        else
          putsChn(x+CHANNEL_PADDING_HORZ, y+CHANNEL_PADDING_VERT, ch+1, 0);

        int32_t val = channelOutputs[ch];

        // The bar
        lcdDrawFilledRect(x+CHANNEL_PADDING_HORZ, y+15, BAR_WIDTH, 6, TEXT_INVERTED_BGCOLOR);
        unsigned int lim = g_model.extendedLimits ? 640*2 : 512*2;
        unsigned int len = limit<unsigned int>(1, (abs(val) * BAR_WIDTH/2 + lim/2) / lim, BAR_WIDTH/2);
        unsigned int x0 = (val>0) ? x+CHANNEL_PADDING_HORZ-1+BAR_WIDTH/2 : x+CHANNEL_PADDING_HORZ+BAR_WIDTH/2+1-len;
        lcdDrawFilledRect(x0, y+16, len, 4, TEXT_BGCOLOR);

        y += CHANNEL_HEIGHT + CHANNEL_MARGIN;
        if (y >= 4*(CHANNEL_HEIGHT + CHANNEL_MARGIN)) {
          x += CHANNEL_WIDTH + CHANNEL_MARGIN;
          y = CHANNEL_MARGIN;
        }
      }
    }
  }

#if 0
  
#if defined(CHANNELS_MONITOR_INV_HIDE)
      //if channel output is inverted, show it with oposite sign
      if (g_model.limitData[ch].revert) val = -val;
#endif

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
#endif
}
