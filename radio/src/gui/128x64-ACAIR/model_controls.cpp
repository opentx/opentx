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

void menuModelControls(event_t event)
{
  MENU("CONTROL SOURCES", menuTabModel, MENU_MODEL_CONTROLS, 5, {0, 0, 1, 1, 1});

  coord_t y = MENU_HEADER_HEIGHT + 2;
  lcdDrawText(0, y, "X1", 0);
  LcdFlags attr = (menuVerticalPosition == 0 ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);
  lcdDrawNumber(40, y, g_model.xValue[0], attr);
  if (attr) {
    CHECK_INCDEC_MODELVAR(event, g_model.xValue[0], 0, g_model.xValue[1] - 1);
  }

  y += (FH+2);
  attr = (menuVerticalPosition == 1 ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);
  lcdDrawText(0, y, "X2", 0);
  lcdDrawNumber(40, y, g_model.xValue[1], attr);
  if (attr) {
    CHECK_INCDEC_MODELVAR(event, g_model.xValue[1], g_model.xValue[0] + 1, 250);
  }

  for (uint8_t i = 4; i < 7; i++) {
    MixData * md = mixAddress(2+i);
    y += (FH+2);
    attr = (menuVerticalPosition==(i-2) ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);
    putsChn(0, y, i+1, 0);
    drawSource(40, y, md->srcRaw, STREXPANDED|(menuHorizontalPosition == 0 ? attr : 0));
    drawSwitch(80, y, md->swtch, STREXPANDED|(menuHorizontalPosition == 1 ? attr : 0));
    if (attr) {
      if (menuHorizontalPosition == 0)
        CHECK_INCDEC_MODELSOURCE(event, md->srcRaw, 0, MIXSRC_SC);
      else
        CHECK_INCDEC_MODELSWITCH(event, md->swtch, 0, SWSRC_SC2, isSwitchAvailableInMixes);
    }
  }
}
