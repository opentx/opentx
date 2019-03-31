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

void pxx2ModuleRequiredScreen(event_t event);

void menuRadioPowerMeter(event_t event)
{
  if(!isModulePXX2(INTERNAL_MODULE)) {
    pxx2ModuleRequiredScreen(event);
    return;
  }

  if(TELEMETRY_STREAMING()) {
    lcdDrawCenteredText(LCD_H/2, "Turn off receiver");
    return;
  }

  SIMPLE_SUBMENU("POWER METER", 1);

  if (menuEvent) {
    pausePulses();
    moduleSettings[INTERNAL_MODULE].mode = MODULE_MODE_NORMAL;
    /* wait 500ms off */
    watchdogSuspend(500);
    RTOS_WAIT_MS(500);
    resumePulses();
    return;
  }
  else if (moduleSettings[INTERNAL_MODULE].mode != MODULE_MODE_POWER_METER) {
    memclear(&reusableBuffer.powerMeter, sizeof(reusableBuffer.powerMeter));
    reusableBuffer.powerMeter.freq = 2400;
    moduleSettings[INTERNAL_MODULE].mode = MODULE_MODE_POWER_METER;
  }

  coord_t y = MENU_HEADER_HEIGHT + 1 + FH;
  LcdFlags attr = (menuVerticalPosition == 0 ? INVERS : 0);
  lcdDrawText(0, y, "Freq.");
  lcdDrawNumber(8*FW, y, reusableBuffer.powerMeter.freq, LEFT|attr|(s_editMode > 0 ? BLINK : 0));
  lcdDrawText(lcdNextPos, y, "MHz");
  if (attr) {
    CHECK_INCDEC_MODELVAR(event, reusableBuffer.powerMeter.freq, 2300, 2500);
    if (checkIncDec_Ret) {
      reusableBuffer.powerMeter.power = 0;
      reusableBuffer.powerMeter.peak = 0;
    }
  }

  y += FH + 1;
  lcdDrawText(0, y, "Power");
  if (reusableBuffer.powerMeter.power) {
    lcdDrawNumber(8 * FW, y, reusableBuffer.powerMeter.power, LEFT | PREC2);
    lcdDrawText(lcdNextPos, y, "dBm");
  }

  y += FH + 1;
  lcdDrawText(0, y, "Peak");
  if (reusableBuffer.powerMeter.peak) {
    lcdDrawNumber(8 * FW, y, reusableBuffer.powerMeter.peak, LEFT | PREC2);
    lcdDrawText(lcdNextPos, y, "dBm");
  }
}
