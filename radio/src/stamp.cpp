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

#include "opentx.h"
#include "stamp-opentx.h"

#define STR2(s) #s
#define DEFNUMSTR(s)  STR2(s)

#if defined(PCBSTD)
  #define EEPROM_STR DEFNUMSTR(EEPROM_VER) "-" DEFNUMSTR(EEPROM_VARIANT)
#else
  #define EEPROM_STR DEFNUMSTR(EEPROM_VER);
#endif

#if defined(COLORLCD)
  const pm_char vers_stamp[] PROGMEM =   "VERS\037\050: " "opentx-" FLAVOUR "-" VERS_STR;
  const pm_char date_stamp[] PROGMEM =   "DATE\037\050: " DATE_STR;
  const pm_char time_stamp[] PROGMEM =   "TIME\037\050: " TIME_STR;
  const pm_char eeprom_stamp[] PROGMEM = "EEPR\037\050: " EEPROM_STR;
#else
  const pm_char vers_stamp[] PROGMEM = "FW\037\033: " "opentx-" FLAVOUR "\036VERS\037\033: " VERS_STR "\036DATE\037\033: " DATE_STR"\036TIME\037\033: " TIME_STR "\036EEPR\037\033: " EEPROM_STR;
#endif
