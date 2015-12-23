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

#include "../../opentx.h"

void menuGeneralVersion(uint8_t event)
{
  SIMPLE_MENU(STR_MENUVERSION, menuTabGeneral, e_Vers, 1);

  lcd_putsLeft(MENU_HEADER_HEIGHT+FH, vers_stamp);

#if defined(COPROCESSOR)
  if (Coproc_valid == 1) {
     lcd_putsLeft(6*FH, PSTR("CoPr:"));
     lcdDraw8bitsNumber(10*FW, 6*FH, Coproc_read);
  }
  else {
     lcd_putsLeft(6*FH, PSTR("CoPr: ---"));
  }
#endif
}
