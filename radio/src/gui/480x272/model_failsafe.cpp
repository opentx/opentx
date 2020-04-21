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

extern uint8_t g_moduleIdx;

bool menuModelFailsafe(event_t event)
{
  uint8_t ch = 0;
  const uint8_t channelStart = g_model.moduleData[g_moduleIdx].channelsStart;
  const int lim = (g_model.extendedLimits ? (512 * LIMIT_EXT_PERCENT / 100) : 512) * 2;
  const uint8_t SLIDER_W = 128;
  const uint8_t cols = sentModuleChannels(g_moduleIdx) > 8 ? 2 : 1;

  if (event == EVT_KEY_LONG(KEY_ENTER)) {
    killEvents(event);
    event = 0;

    if (menuVerticalPosition < sentModuleChannels(g_moduleIdx)) {
      if (s_editMode) {
        g_model.failsafeChannels[menuVerticalPosition] = channelOutputs[menuVerticalPosition+channelStart];
        s_editMode = 0;
      }
      else {
        int16_t * failsafe = &g_model.failsafeChannels[menuVerticalPosition];
        if (*failsafe < FAILSAFE_CHANNEL_HOLD)
          *failsafe = FAILSAFE_CHANNEL_HOLD;
        else if (*failsafe == FAILSAFE_CHANNEL_HOLD)
          *failsafe = FAILSAFE_CHANNEL_NOPULSE;
        else
          *failsafe = 0;
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

  SIMPLE_SUBMENU_WITH_OPTIONS("FAILSAFE", ICON_STATS_ANALOGS, sentModuleChannels(g_moduleIdx)+1, OPTION_MENU_NO_SCROLLBAR);
  drawStringWithIndex(50, 3+FH, STR_MODULE, g_moduleIdx+1, MENU_TITLE_COLOR);

  for (uint8_t col=0; col < cols; col++) {
    for (uint8_t line=0; line < 8; line++) {
      coord_t x = col*(LCD_W/2);
      const coord_t y = MENU_CONTENT_TOP - FH + line*(FH+2);
      const int32_t channelValue = channelOutputs[ch+channelStart];
      int32_t failsafeValue = g_model.failsafeChannels[8*col+line];

      // Channel name if present, number if not
      if (g_model.limitData[ch+channelStart].name[0] != '\0') {
        putsChn(x+MENUS_MARGIN_LEFT, y-3, ch+1, TINSIZE);
        lcdDrawSizedText(x+MENUS_MARGIN_LEFT, y+5, g_model.limitData[ch+channelStart].name, sizeof(g_model.limitData[ch+channelStart].name), ZCHAR|SMLSIZE);
      }
      else {
        putsChn(x+MENUS_MARGIN_LEFT, y, ch+1, 0);
      }

      // Value
      LcdFlags flags = RIGHT;
      if (menuVerticalPosition == ch) {
        flags |= INVERS;
        if (s_editMode) {
          if (failsafeValue == FAILSAFE_CHANNEL_HOLD || failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
            s_editMode = 0;
          }
          else {
            flags |= BLINK;
            CHECK_INCDEC_MODELVAR(event, g_model.failsafeChannels[8*col+line], -lim, +lim);
          }
        }
      }

      x += (LCD_W/2)-4-MENUS_MARGIN_LEFT-SLIDER_W;

      if (failsafeValue == FAILSAFE_CHANNEL_HOLD) {
        lcdDrawText(x, y+2, STR_HOLD, flags|SMLSIZE);
        failsafeValue = 0;
      }
      else if (failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
        lcdDrawText(x, y+2, STR_NONE, flags|SMLSIZE);
        failsafeValue = 0;
      }
      else {
#if defined(PPM_UNIT_US)
        lcdDrawNumber(x, y, PPM_CH_CENTER(ch)+failsafeValue/2, flags);
#elif defined(PPM_UNIT_PERCENT_PREC1)
        lcdDrawNumber(x, y, calcRESXto1000(failsafeValue), PREC1|flags);
#else
        lcdDrawNumber(x, y, calcRESXto1000(failsafeValue)/10, flags);
#endif
      }

      // Gauge
      x += 4;
      lcdDrawRect(x, y+3, SLIDER_W+1, 12);
      const coord_t lenChannel = limit((uint8_t)1, uint8_t((abs(channelValue) * SLIDER_W/2 + lim/2) / lim), uint8_t(SLIDER_W/2));
      const coord_t lenFailsafe = limit((uint8_t)1, uint8_t((abs(failsafeValue) * SLIDER_W/2 + lim/2) / lim), uint8_t(SLIDER_W/2));
      x += SLIDER_W/2;
      const coord_t xChannel = (channelValue>0) ? x : x+1-lenChannel;
      const coord_t xFailsafe = (failsafeValue>0) ? x : x+1-lenFailsafe;
      lcdDrawSolidFilledRect(xChannel, y+4, lenChannel, 5, TEXT_COLOR);
      lcdDrawSolidFilledRect(xFailsafe, y+9, lenFailsafe, 5, ALARM_COLOR);

      if (++ch >= sentModuleChannels(g_moduleIdx)) {

        // draw that menu at the bottom center
        flags = CENTERED | (menuVerticalPosition == ch ? INVERS : 0);
        lcdDrawSizedText(LCD_W/2, MENU_CONTENT_TOP - FH + 8*(FH+2), STR_CHANNELS2FAILSAFE, 20, flags);
        break;
      }
    }
  }

  return true;
}
