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

#define X_OFF                          15
#define Y_OFF                          55
#define HLINE_Y_OFF                    215
#define LS_COL_WIDTH                   50
#define LS_LINE_HEIGHT                 17
#define X_FUNC                         45
#define Y_FUNC                         225


bool menuLogicalSwitchesMonitor(evt_t event, uint8_t page){
  char lsString[] = "L64";
  uint8_t col, lsIdx, line;
  LcdFlags attr;

  if (navigate(event, 64, 0)) {
    //putEvent(EVT_REFRESH);
  }
  for (line = 1, lsIdx=1; line < 9; line++){   
    for(col=1; col < 9 ; col++, lsIdx++){
      strAppend(lsString, "L");
      strAppendSigned(&lsString[1], lsIdx, 2);
      LogicalSwitchData * cs = lswAddress(lsIdx-1);
      if (cs->func != LS_FUNC_NONE) attr = ( BOLD | CENTERED);
      else attr = (CENTERED);
      if (lsIdx == (menuVerticalPosition+1)) attr+= (INVERS);
      lcdDrawText(X_OFF + col * LS_COL_WIDTH, Y_OFF + line * LS_LINE_HEIGHT, lsString, attr);
    }
  }
  lcdDrawHorizontalLine(0,HLINE_Y_OFF, LCD_W, SOLID);
  LogicalSwitchData * cs = lswAddress(menuVerticalPosition);
  lcdDrawTextAtIndex(X_FUNC, Y_FUNC, STR_VCSWFUNC, cs->func, 0);
  return true;
}

bool menuLogicalSwitches(evt_t event)
{
  MENU("Logical Switches monitor", MONITOR_ICONS, menuTabMonitors, e_LogicSwitches, 0, { 0 });
  lastMonitorPage = e_LogicSwitches;
  return menuLogicalSwitchesMonitor(event, 4);
}


