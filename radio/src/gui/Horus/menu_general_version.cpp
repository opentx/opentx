/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "../../opentx.h"

void menuGeneralVersion(evt_t event)
{
  SIMPLE_MENU(STR_MENUVERSION, menuTabGeneral, e_Vers, 1, DEFAULT_SCROLLBAR_X);

  lcd_putsLeft(MENU_CONTENT_TOP + FH, vers_stamp);
  lcd_putsLeft(MENU_CONTENT_TOP + 2*FH, date_stamp);
  lcd_putsLeft(MENU_CONTENT_TOP + 3*FH, time_stamp);
  lcd_putsLeft(MENU_CONTENT_TOP + 4*FH, eeprom_stamp);

  // TODO EEPROM erase + backup
  // lcd_putsCenter(MENU_HEADER_HEIGHT+6*FH, STR_EEBACKUP);
  // if (event == EVT_KEY_LONG(KEY_ENTER)) {
  //   backupEeprom();
  // }
}
