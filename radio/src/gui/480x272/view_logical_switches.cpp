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

#define X_OFF                          45
#define Y_OFF                          70
#define HLINE_Y_OFF                    215
#define LS_COL_WIDTH                   50
#define LS_LINE_HEIGHT                 17
#define X_FUNC                         50
#define Y_FUNC                         225

#define CSW_1ST_COLUMN                 50
#define CSW_2ND_COLUMN                 120
#define CSW_3RD_COLUMN                 200
#define CSW_4TH_COLUMN                 280
#define CSW_5TH_COLUMN                 340
#define CSW_6TH_COLUMN                 390
#define MAX_LOGICAL_SWITCHES             64

extern void putsEdgeDelayParam(coord_t, coord_t, LogicalSwitchData *, uint8_t, uint8_t);

void displayLogicalSwitchedDetails(coord_t x, coord_t y, uint8_t idx)
{
  LogicalSwitchData * cs = lswAddress(idx);
  lcdDrawTextAtIndex(x, y, STR_VCSWFUNC, cs->func, 0);
  // CSW params
  unsigned int cstate = lswFamily(cs->func);

  if (cstate == LS_FAMILY_BOOL || cstate == LS_FAMILY_STICKY) {
    drawSwitch(CSW_2ND_COLUMN, y, cs->v1, 0);
    drawSwitch(CSW_3RD_COLUMN, y, cs->v2, 0);
  }
  else if (cstate == LS_FAMILY_EDGE) {
    drawSwitch(CSW_2ND_COLUMN, y, cs->v1, 0);
    putsEdgeDelayParam(CSW_3RD_COLUMN, y, cs, 0, 0);
  }
  else if (cstate == LS_FAMILY_COMP) {
    drawSource(CSW_2ND_COLUMN, y, cs->v1, 0);
    drawSource(CSW_3RD_COLUMN, y, cs->v2, 0);
  }
  else if (cstate == LS_FAMILY_TIMER) {
    lcdDrawNumber(CSW_2ND_COLUMN, y, lswTimerValue(cs->v1), LEFT | PREC1);
    lcdDrawNumber(CSW_3RD_COLUMN, y, lswTimerValue(cs->v2), LEFT | PREC1);
  }
  else {
    drawSource(CSW_2ND_COLUMN, y, cs->v1, 0);
    drawSourceCustomValue(CSW_3RD_COLUMN, y, cs->v1, cs->v1 <= MIXSRC_LAST_CH ? calc100toRESX(cs->v2) : cs->v2, LEFT);
  }

  // CSW AND switch
  drawSwitch(CSW_4TH_COLUMN, y, cs->andsw, 0);

  // CSW duration
  if (cs->duration > 0)
    lcdDrawNumber(CSW_5TH_COLUMN, y, cs->duration, PREC1 | LEFT);
  else
    lcdDrawMMM(CSW_5TH_COLUMN, y, 0);

  // CSW delay
  if (cstate == LS_FAMILY_EDGE) {
    lcdDrawText(CSW_6TH_COLUMN, y, STR_NA);
  }
  else if (cs->delay > 0) {
    lcdDrawNumber(CSW_6TH_COLUMN, y, cs->delay, PREC1 | LEFT);
  }
  else {
    lcdDrawMMM(CSW_6TH_COLUMN, y, 0);
  }
}

bool menuLogicalSwitchesMonitor(event_t event)
{
  char lsString[] = "L64";
  lcdColorTable[CUSTOM_COLOR_INDEX] = RGB(160, 160, 160);
  for (uint8_t i = 0; i < MAX_LOGICAL_SWITCHES; i++) {
    LcdFlags attr = (menuHorizontalPosition == i ? INVERS : 0) | LEFT;
    LogicalSwitchData * cs = lswAddress(i);
    strAppendSigned(&lsString[1], i + 1, 2);
    if (cs->func == LS_FUNC_NONE)
      attr += CUSTOM_COLOR;
    else if (getSwitch(SWSRC_SW1 + i))
      attr += BOLD;
    lcdDrawText(X_OFF + (i & 0x07) * LS_COL_WIDTH, Y_OFF + (i >> 3) * LS_LINE_HEIGHT, lsString, attr);
  }
  lcdDrawHorizontalLine(0, HLINE_Y_OFF, LCD_W, SOLID);
  if (lswAddress(menuHorizontalPosition)->func != LS_FUNC_NONE) {
    displayLogicalSwitchedDetails(X_FUNC, Y_FUNC, menuHorizontalPosition);
  }
  s_editMode = 0;
  return true;
}

bool menuLogicalSwitches(event_t event)
{
  MENU(STR_MONITOR_SWITCHES, MONITOR_ICONS, menuTabMonitors, e_MonLogicalSwitches, 1, { MAX_LOGICAL_SWITCHES - 1 });
  lastMonitorPage = e_MonLogicalSwitches;
  return menuLogicalSwitchesMonitor(event);
}
