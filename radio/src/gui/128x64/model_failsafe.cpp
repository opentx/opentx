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

#include <opentx.h>

extern uint8_t g_moduleIdx;

void menuModelFailsafe(event_t event)
{
  const uint8_t channelStart = g_model.moduleData[g_moduleIdx].channelsStart;
  const int lim = (g_model.extendedLimits ? (512 * LIMIT_EXT_PERCENT / 100) : 512) * 2;
  uint8_t wbar = LCD_W - FW * 4 - FWNUM * 4;
#if defined(PPM_UNIT_PERCENT_PREC1)
  wbar -= 6;
#endif

  if (event == EVT_KEY_LONG(KEY_ENTER)) {
    killEvents(event);
    event = 0;

    if (menuVerticalPosition < sentModuleChannels(g_moduleIdx)) {
      if (s_editMode > 0) {
        g_model.failsafeChannels[menuVerticalPosition] = channelOutputs[menuVerticalPosition+channelStart];
        s_editMode = 0;
      }
      else {
        int16_t & failsafe = g_model.failsafeChannels[menuVerticalPosition];
        if (failsafe < FAILSAFE_CHANNEL_HOLD)
          failsafe = FAILSAFE_CHANNEL_HOLD;
        else if (failsafe == FAILSAFE_CHANNEL_HOLD)
          failsafe = FAILSAFE_CHANNEL_NOPULSE;
        else
          failsafe = 0;
      }
    }
    else {
      // "Outputs => Failsafe" menu item
      setCustomFailsafe(g_moduleIdx);
    }

    storageDirty(EE_MODEL);
    AUDIO_WARNING1();
    SEND_FAILSAFE_NOW(g_moduleIdx);
  }

  SIMPLE_SUBMENU_NOTITLE(sentModuleChannels(g_moduleIdx) + 1);

  lcdDrawTextAlignedCenter(0, FAILSAFESET);
  lcdInvertLine(0);

  const coord_t x = 1;
  coord_t y = FH + 1;
  uint8_t line = (menuVerticalPosition >= sentModuleChannels(g_moduleIdx) ? 2 : 0);
  uint8_t ch = (menuVerticalPosition & ~0x07) + + line;

  // Channels
  for (; line < 8; line++) {
    const int32_t channelValue = channelOutputs[ch+channelStart];
    int32_t failsafeValue = g_model.failsafeChannels[ch];

    if (menuVerticalPosition >= sentModuleChannels(g_moduleIdx)) {
      // Outputs => Failsafe
      lcdDrawText(CENTER_OFS, LCD_H - (FH + 1), STR_OUTPUTS2FAILSAFE, INVERS);
      break;
    }

    // Channel
    putsChn(x+1, y, ch+1, SMLSIZE);

    // Value
    LcdFlags flags = TINSIZE;
    if (menuVerticalPosition == ch) {
      flags |= INVERS;
      if (s_editMode > 0) {
        if (failsafeValue == FAILSAFE_CHANNEL_HOLD || failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
          s_editMode = 0;
        }
        else {
          flags |= BLINK;
          CHECK_INCDEC_MODELVAR(event, g_model.failsafeChannels[ch], -lim, +lim);
        }
      }
    }

    const coord_t xValue = x+LCD_W-4-wbar;
    if (failsafeValue == FAILSAFE_CHANNEL_HOLD) {
      lcdDrawText(xValue, y, STR_HOLD, RIGHT|flags);
      failsafeValue = 0;
    }
    else if (failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
      lcdDrawText(xValue, y, STR_NONE, RIGHT|flags);
      failsafeValue = 0;
    }
    else {
#if defined(PPM_UNIT_US)
      lcdDrawNumber(xValue, y, PPM_CH_CENTER(ch)+failsafeValue/2, RIGHT|flags);
#elif defined(PPM_UNIT_PERCENT_PREC1)
      lcdDrawNumber(xValue, y, calcRESXto1000(failsafeValue), RIGHT|PREC1|flags);
#else
      lcdDrawNumber(xValue, y, calcRESXto1000(failsafeValue)/10, RIGHT|flags);
#endif
    }

    // Gauge
#if !defined(PCBX7) // X7 LCD doesn't like too many horizontal lines
    lcdDrawRect(x+LCD_W-3-wbar, y, wbar+1, 6);
#endif
    const uint8_t lenChannel = limit<uint8_t>(1, (abs(channelValue) * wbar/2 + lim/2) / lim, wbar/2);
    const uint8_t lenFailsafe = limit<uint8_t>(1, (abs(failsafeValue) * wbar/2 + lim/2) / lim, wbar/2);
    const coord_t xChannel = (channelValue>0) ? x+LCD_W-3-wbar/2 : x+LCD_W-2-wbar/2-lenChannel;
    const coord_t xFailsafe = (failsafeValue>0) ? x+LCD_W-3-wbar/2 : x+LCD_W-2-wbar/2-lenFailsafe;
    lcdDrawHorizontalLine(xChannel, y+1, lenChannel, DOTTED, 0);
    lcdDrawHorizontalLine(xChannel, y+2, lenChannel, DOTTED, 0);
    lcdDrawSolidHorizontalLine(xFailsafe, y+3, lenFailsafe);
    lcdDrawSolidHorizontalLine(xFailsafe, y+4, lenFailsafe);

    y += FH - 1;

    if (++ch >= sentModuleChannels(g_moduleIdx))
      break;
  }
}
