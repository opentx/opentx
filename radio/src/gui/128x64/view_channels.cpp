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

constexpr coord_t CHANNEL_NAME_OFFSET = 1;
constexpr coord_t CHANNEL_VALUE_OFFSET = CHANNEL_NAME_OFFSET + 42;
constexpr coord_t CHANNEL_GAUGE_OFFSET = CHANNEL_VALUE_OFFSET;
constexpr coord_t CHANNEL_BAR_WIDTH = 54;
constexpr coord_t CHANNEL_PROPERTIES_OFFSET = CHANNEL_GAUGE_OFFSET + CHANNEL_BAR_WIDTH + 2;

void menuChannelsView(event_t event)
{
  bool newLongNames = false;

  uint8_t ch;

  switch (event) {
    case EVT_ENTRY:
      memclear(&reusableBuffer.viewChannels, sizeof(reusableBuffer.viewChannels));
      break;

    case EVT_KEY_BREAK(KEY_EXIT):
      popMenu();
      break;

    case EVT_KEY_FIRST(KEY_RIGHT):
    case EVT_ROTARY_RIGHT:
      g_eeGeneral.view = (g_eeGeneral.view + (4*ALTERNATE_VIEW) + ALTERNATE_VIEW) % (4*ALTERNATE_VIEW);
      break;

    case EVT_KEY_FIRST(KEY_LEFT):
    case EVT_ROTARY_LEFT:
      g_eeGeneral.view = (g_eeGeneral.view + (4*ALTERNATE_VIEW) - ALTERNATE_VIEW) % (4*ALTERNATE_VIEW);
      break;

    case EVT_KEY_FIRST(KEY_ENTER):
      reusableBuffer.viewChannels.mixersView = !reusableBuffer.viewChannels.mixersView;
      break;

    case EVT_KEY_FIRST(KEY_PAGE):
      g_eeGeneral.view = VIEW_INPUTS;
      killEvents(KEY_PAGE);
      chainMenu(menuMainView);
      break;
  }

  ch = 8 * (g_eeGeneral.view / ALTERNATE_VIEW);

  if (reusableBuffer.viewChannels.mixersView) {
    lcdDrawTextAlignedCenter(0, TR_MIXERS_MONITOR);
  }
  else {
    lcdDrawTextAlignedCenter(0, TR_CHANNELS_MONITOR);
  }

  lcdInvertLine(0);

  int16_t limits = 512 * 2;

  // Channels
  for (uint8_t line=0; line < 8; line++) {
    LimitData * ld = limitAddress(ch);
    const uint8_t y = 9 + line * 7;
    const int32_t val = reusableBuffer.viewChannels.mixersView ? ex_chans[ch] : channelOutputs[ch];
    const uint8_t lenLabel = ZLEN(g_model.limitData[ch].name);

    // Channel name if present, number if not
    if (lenLabel > 0) {
      if (lenLabel > 4)
        reusableBuffer.viewChannels.longNames = true;
      lcdDrawSizedText(CHANNEL_NAME_OFFSET, y, g_model.limitData[ch].name, sizeof(g_model.limitData[ch].name), ZCHAR | SMLSIZE);
    }
    else {
      putsChn(CHANNEL_NAME_OFFSET, y, ch+1, SMLSIZE);
    }

    // Value
#if defined(PPM_UNIT_US)
    lcdDrawNumber(CHANNEL_VALUE_OFFSET, y+1, PPM_CH_CENTER(ch)+val/2, TINSIZE|RIGHT);
#elif defined(PPM_UNIT_PERCENT_PREC1)
    lcdDrawNumber(CHANNEL_VALUE_OFFSET, y+1, calcRESXto1000(val), PREC1|TINSIZE|RIGHT);
#else
    lcdDrawNumber(CHANNEL_VALUE_OFFSET, y+1, calcRESXto1000(val)/10, TINSIZE|RIGHT);
#endif

    // Gauge
    drawGauge(CHANNEL_GAUGE_OFFSET, y, CHANNEL_BAR_WIDTH + (reusableBuffer.viewChannels.mixersView ? 25 : 0), 6, val, limits);

    if (!reusableBuffer.viewChannels.mixersView) {
      // Properties
#if defined(OVERRIDE_CHANNEL_FUNCTION)
      if (safetyCh[ch] != OVERRIDE_CHANNEL_UNDEFINED)
        lcdDrawText(CHANNEL_PROPERTIES_OFFSET, y, "OVERIDE", TINSIZE);
      else
#endif
      if (ld && ld->revert) {
        lcdDrawText(CHANNEL_PROPERTIES_OFFSET, y, "INV", TINSIZE);
      }
    }

    ++ch;
  }

  reusableBuffer.viewChannels.longNames = newLongNames;
}
