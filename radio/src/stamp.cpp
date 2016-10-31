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
#include "stamp.h"

#define STR2(s) #s
#define DEFNUMSTR(s)  STR2(s)

#if defined(PCBSTD)
  #define EEPROM_STR DEFNUMSTR(EEPROM_VER) "-" DEFNUMSTR(EEPROM_VARIANT)
#else
  #define EEPROM_STR DEFNUMSTR(EEPROM_VER);
#endif

#if defined(PCBHORUS)
#define TAB "\037\075"
#elif defined(PCBFLAMENCO)
#define TAB "\037\050"
#endif

#if defined(COLORLCD)
  const pm_char vers_stamp[] PROGMEM =   "VERS" TAB ": " "opentx-" FLAVOUR "-" VERSION " (" GIT_STR ")";
  const pm_char date_stamp[] PROGMEM =   "DATE" TAB ": " DATE;
  const pm_char time_stamp[] PROGMEM =   "TIME" TAB ": " TIME;
  const pm_char eeprom_stamp[] PROGMEM = "EEPR" TAB ": " EEPROM_STR;
#elif defined(PCBTARANIS)
  const pm_char vers_stamp[] PROGMEM = "FW\037\033: " "opentx-" FLAVOUR "\036VERS\037\033: " VERSION " (" GIT_STR ")" "\036DATE\037\033: " DATE " " TIME "\036EEPR\037\033: " EEPROM_STR;
#else
  const pm_char vers_stamp[] PROGMEM = "FW\037\033: " "opentx-" FLAVOUR "\036VERS\037\033: " VERSION "\036DATE\037\033: " DATE "\036TIME\037\033: " TIME "\036EEPR\037\033: " EEPROM_STR;
#endif
