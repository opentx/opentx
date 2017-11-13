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

void drawStringWithIndex(coord_t x, coord_t y, const char * str, int idx, LcdFlags flags, const char * prefix, const char * suffix)
{
  char s[64];
  char * tmp = (prefix ? strAppend(s, prefix) : s);
  tmp = strAppend(tmp, str);
  tmp = strAppendUnsigned(tmp, abs(idx));
  if (suffix)
    strAppend(tmp, suffix);
  lcdDrawText(x, y, s, flags);
}

void drawValueWithUnit(coord_t x, coord_t y, int32_t val, uint8_t unit, LcdFlags att)
{
  // convertUnit(val, unit);
  if (!(att & NO_UNIT) && unit != UNIT_RAW) {
    char unitStr[8];
    strAppend(unitStr, STR_VTELEMUNIT+1+unit*STR_VTELEMUNIT[0], STR_VTELEMUNIT[0]);
    lcdDrawNumber(x, y, val, att, 0, NULL, unitStr);
  }
  else {
    lcdDrawNumber(x, y, val, att);
  }
}

int editChoice(coord_t x, coord_t y, const char * values, int value, int min, int max, LcdFlags attr, event_t event)
{
  if (attr & INVERS) value = checkIncDec(event, value, min, max, (menuVerticalPositions[0] == 0) ? EE_MODEL : EE_GENERAL);
  if (values) lcdDrawTextAtIndex(x, y, values, value-min, attr);
  return value;
}

uint8_t editCheckBox(uint8_t value, coord_t x, coord_t y, LcdFlags attr, event_t event )
{
  value = editChoice(x, y, NULL, value, 0, 1, attr, event);
  drawCheckBox(x, y, value, attr);
  return value;
}

swsrc_t editSwitch(coord_t x, coord_t y, swsrc_t value, LcdFlags attr, event_t event)
{
  if (attr & INVERS) CHECK_INCDEC_MODELSWITCH(event, value, SWSRC_FIRST_IN_MIXES, SWSRC_LAST_IN_MIXES, isSwitchAvailableInMixes);
  drawSwitch(x, y, value, attr);
  return value;
}

void drawFatalErrorScreen(const char * message)
{
  lcdClear();
  lcdDrawText(LCD_W/2, LCD_H/2-20, message, DBLSIZE|CENTERED|TEXT_BGCOLOR);
  lcdRefresh();
}

void runFatalErrorScreen(const char * message)
{
  while (1) {
    drawFatalErrorScreen(message);
    backlightEnable(100);
    uint8_t refresh = false;
    while (1) {
      uint32_t pwr_check = pwrCheck();
      if (pwr_check == e_power_off) {
        boardOff();
      }
      else if (pwr_check == e_power_press) {
        refresh = true;
      }
      else if (pwr_check == e_power_on && refresh) {
        break;
      }
      SIMU_SLEEP_NORET(1);
    }
  }
}