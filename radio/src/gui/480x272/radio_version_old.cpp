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

bool menuRadioVersion(event_t event)
{
  char id[27];
#if 0
  if (warningResult) {
    warningResult = 0;
    showMessageBox(STR_STORAGE_FORMAT);
    storageEraseAll(false);
    NVIC_SystemReset();
  }
#endif

  getCPUUniqueID(id);

  SIMPLE_MENU(STR_MENUVERSION, RADIO_ICONS, menuTabGeneral, MENU_RADIO_VERSION, 0);

  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + FH, vers_stamp);
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 2*FH, date_stamp);
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 3*FH, time_stamp);
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 4*FH, eeprom_stamp);
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 5*FH, "UID:");
  lcdDrawText(MENUS_MARGIN_LEFT + 64, MENU_CONTENT_TOP + 5*FH, id);

#if 0
  if (event == EVT_KEY_LONG(KEY_ENTER)) {
    killEvents(event);
    // POPUP_MENU_ADD_ITEM(STR_EEBACKUP);
    POPUP_MENU_ADD_ITEM(STR_FACTORYRESET);
    POPUP_MENU_START(onVersionMenu);
  }
#endif

  return true;
}
