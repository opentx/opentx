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
#include "opentx.h"

extern uint8_t g_moduleIdx;

enum PowerMeterFields {
  POWER_METER_FREQ_RANGE,
  POWER_METER_ATTENUATOR,
  POWER_METER_POWER,
  POWER_METER_PEAK,
  POWER_METER_FIELDS_MAX
};


void menuRadioPowerMeter(event_t event)
{
  SUBMENU(STR_MENU_POWER_METER, POWER_METER_FIELDS_MAX-1, {0, 0, READONLY_ROW, READONLY_ROW});

  if (TELEMETRY_STREAMING()) {
    lcdDrawCenteredText(LCD_H/2, STR_TURN_OFF_RECEIVER);
    if (event == EVT_KEY_FIRST(KEY_EXIT)) {
      killEvents(event);
      popMenu();
    }
    return;
  }

  if (menuEvent) {
    lcdDrawCenteredText(LCD_H/2, STR_STOPPING);
    lcdRefresh();
    moduleSettings[g_moduleIdx].mode = MODULE_MODE_NORMAL;
    /* wait 1s to resume normal operation before leaving */
    watchdogSuspend(1000);
    RTOS_WAIT_MS(1000);
    return;
  }

  if (moduleSettings[g_moduleIdx].mode != MODULE_MODE_POWER_METER) {
    memclear(&reusableBuffer.powerMeter, sizeof(reusableBuffer.powerMeter));
    reusableBuffer.powerMeter.freq = 2400000000;
#if defined(FRSKY_RELEASE)
    reusableBuffer.powerMeter.attn = 4;
#endif
    moduleSettings[g_moduleIdx].mode = MODULE_MODE_POWER_METER;
  }

  // The warning
  lcdDrawText(3 * FW, FH + 3, STR_MAX, BOLD);
  lcdDrawText(lcdLastRightPos, FH + 3, ": ", BOLD);
  lcdDrawNumber(lcdLastRightPos, FH + 3, -10 + 10 * reusableBuffer.powerMeter.attn, BOLD);
  lcdDrawText(lcdLastRightPos, FH + 3, "dBm ", BOLD);
  lcdDrawText(lcdLastRightPos, FH + 3, "(", BOLD);
  drawPower(lcdLastRightPos, FH + 3, -10 + 10 * reusableBuffer.powerMeter.attn, BOLD);
  lcdDrawText(lcdLastRightPos, FH + 3, ")", BOLD);

  for (uint8_t i=0; i<POWER_METER_FIELDS_MAX; i++) {
    LcdFlags attr = (menuVerticalPosition == i ? (s_editMode > 0 ? INVERS | BLINK : INVERS) : 0);
    coord_t y = MENU_HEADER_HEIGHT + 2 * FH + i * FH;

    switch (i) {
      case POWER_METER_FREQ_RANGE:
        lcdDrawText(0, y, STR_POWERMETER_FREQ);
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

      case POWER_METER_ATTENUATOR:
        lcdDrawText(0, y, STR_POWERMETER_ATTN);
        lcdDrawNumber(8 * FW, y, -10 * reusableBuffer.powerMeter.attn, LEFT | attr);
        lcdDrawText(lcdNextPos, y, " dB");
        if (attr) {
          reusableBuffer.powerMeter.attn = checkIncDec(event, reusableBuffer.powerMeter.attn, 0, 5, 0);
        }
        break;

      case POWER_METER_POWER:
        lcdDrawText(0, y, STR_POWERMETER_POWER);
        if (reusableBuffer.powerMeter.power) {
          lcdDrawNumber(8 * FW, y, reusableBuffer.powerMeter.power + reusableBuffer.powerMeter.attn * 1000, LEFT | PREC2);
          lcdDrawText(lcdNextPos, y, "dBm");
        }
        break;

      case POWER_METER_PEAK:
        lcdDrawText(0, y, STR_POWERMETER_PEAK);
        if (reusableBuffer.powerMeter.peak) {
          lcdDrawNumber(8 * FW, y, reusableBuffer.powerMeter.peak + reusableBuffer.powerMeter.attn * 1000, LEFT | PREC2);
          lcdDrawText(lcdNextPos, y, "dBm");
        }
        break;
    }
  }
}
