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

bool menuGeneralVersion(evt_t event)
{
  DEBUG_TIMER_START(debugTimerVersion);
  DEBUG_TIMER_START(debugTimerSimpleMenu);
  SIMPLE_MENU(STR_MENUVERSION, LBM_RADIO_ICONS, menuTabGeneral, e_Vers, 0);
  DEBUG_TIMER_STOP(debugTimerSimpleMenu);

  DEBUG_TIMER_START(debugTimerDrawText);
  DEBUG_TIMER_START(debugTimerDrawText1);
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + FH, vers_stamp);
  DEBUG_TIMER_STOP(debugTimerDrawText1);
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 2*FH, date_stamp);
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 3*FH, time_stamp);
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 4*FH, eeprom_stamp);
  DEBUG_TIMER_STOP(debugTimerDrawText);

  // TODO EEPROM erase + backup
  // lcd_putsCenter(MENU_HEADER_HEIGHT+6*FH, STR_EEBACKUP);
  // if (event == EVT_KEY_LONG(KEY_ENTER)) {
  //   backupEeprom();
  // }
  DEBUG_TIMER_STOP(debugTimerVersion);
  return true;
}
