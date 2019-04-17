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

enum PowerMeterFields {
  POWER_METER_FREQ_RANGE,
  POWER_METER_FREQ_ATTENUATOR,
  POWER_METER_FREQ_POWER,
  POWER_METER_FREQ_PEAK,
  POWER_METER_FIELDS_MAX
};


void menuRadioPowerMeter(event_t event)
{
  if (TELEMETRY_STREAMING()) {
    lcdDrawCenteredText(LCD_H/2, "Turn off receiver");
    if (event == EVT_KEY_FIRST(KEY_EXIT)) {
      killEvents(event);
      popMenu();
    }
    return;
  }

  SUBMENU("POWER METER", POWER_METER_FIELDS_MAX-1, {0, 0, READONLY_ROW, READONLY_ROW});

  if (menuEvent) {
    const char * message = "Stopping...";
    lcdDrawText(LCD_W / 2 - getTextWidth(message) / 2, 4*FH, message);
    lcdRefresh();
    pausePulses();
    moduleSettings[g_moduleIdx].mode = MODULE_MODE_NORMAL;
    /* wait 500ms off */
    watchdogSuspend(500);
    RTOS_WAIT_MS(500);
    resumePulses();
    /* wait 500ms to resume normal operation before leaving */
    watchdogSuspend(500);
    RTOS_WAIT_MS(500);
    return;
  }

  if (moduleSettings[g_moduleIdx].mode != MODULE_MODE_POWER_METER) {
    memclear(&reusableBuffer.powerMeter, sizeof(reusableBuffer.powerMeter));
    reusableBuffer.powerMeter.freq = 2400000000;
    moduleSettings[g_moduleIdx].mode = MODULE_MODE_POWER_METER;
  }

  for (uint8_t i=0; i<POWER_METER_FIELDS_MAX; i++) {
    LcdFlags attr = (menuVerticalPosition == i ? (s_editMode > 0 ? INVERS | BLINK : INVERS) : 0);
    coord_t y = MENU_HEADER_HEIGHT + FH + i * FH;

    switch (i) {
      case POWER_METER_FREQ_RANGE:
        lcdDrawText(0, y, "Freq.");
        lcdDrawNumber(8 * FW, y, reusableBuffer.powerMeter.freq / 1000000, LEFT | attr);
        lcdDrawText(lcdNextPos, y, " MHz band");
        if (attr) {
          reusableBuffer.powerMeter.freq = checkIncDec(event, reusableBuffer.powerMeter.freq == 900000000, 0, 1) ? 900000000 : 2400000000;
          if (checkIncDec_Ret) {
            reusableBuffer.powerMeter.power = 0;
            reusableBuffer.powerMeter.peak = 0;
          }
        }
        break;

      case POWER_METER_FREQ_ATTENUATOR:
      {
        lcdDrawText(0, y, "Attn");
        lcdDrawNumber(8 * FW, y, -10 * reusableBuffer.powerMeter.attn, LEFT | attr);
        lcdDrawText(lcdNextPos, y, " dB");
        if (attr) {
          reusableBuffer.powerMeter.attn = checkIncDec(event, reusableBuffer.powerMeter.attn, 0, 5, 0);
        }
        break;
      }

      case POWER_METER_FREQ_POWER:
        lcdDrawText(0, y, "Power");
        if (reusableBuffer.powerMeter.power) {
          lcdDrawNumber(8 * FW, y, reusableBuffer.powerMeter.power + reusableBuffer.powerMeter.attn * 1000, LEFT | PREC2);
          lcdDrawText(lcdNextPos, y, "dBm");
        }
        break;

      case POWER_METER_FREQ_PEAK:
        lcdDrawText(0, y, "Peak");
        if (reusableBuffer.powerMeter.peak) {
          lcdDrawNumber(8 * FW, y, reusableBuffer.powerMeter.peak + reusableBuffer.powerMeter.attn * 1000, LEFT | PREC2);
          lcdDrawText(lcdNextPos, y, "dBm");
        }
        break;
    }
  }
}
