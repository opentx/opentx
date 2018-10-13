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
#endif

#if defined(COLORLCD)
  const pm_char vers_stamp[] PROGMEM =   "VERS" TAB ": " "opentx-" FLAVOUR "-" VERSION " (" GIT_STR ")";
  const pm_char date_stamp[] PROGMEM =   "DATE" TAB ": " DATE;
  const pm_char time_stamp[] PROGMEM =   "TIME" TAB ": " TIME;
  const pm_char eeprom_stamp[] PROGMEM = "EEPR" TAB ": " EEPROM_STR;
#elif defined(PCBTARANIS)
  const pm_char vers_stamp[] PROGMEM = "FW\037\033: " "opentx-" FLAVOUR "\036VERS\037\033: " VERSION " (schumixmd)" "\036DATE\037\033: " DATE " " TIME "\036EEPR\037\033: " EEPROM_STR;
#else
  const pm_char vers_stamp[] PROGMEM = "FW\037\033: " "opentx-" FLAVOUR "\036VERS\037\033: " VERSION "\036DATE\037\033: " DATE "\036TIME\037\033: " TIME "\036EEPR\037\033: " EEPROM_STR;
#endif

/**
 * Retrieves the version of the bootloader or firmware
 * @return
 */
#if defined(STM32)

__SECTION_USED(".fwversiondata")   const char firmware_version[] = "opentx-" FLAVOUR "-" VERSION " (" GIT_STR ")";
__SECTION_USED(".bootversiondata") const char boot_version[] =     "opentx-" FLAVOUR "-" VERSION " (" GIT_STR ")";

/**
 * Tries to find opentx version in the first 1024 byte of either firmware/bootloader (the one not running) or the buffer
 * @param buffer If non-null find the firmware version in the buffer instead
 */
const char* getOtherVersion(char* buffer)
{
#if defined(BOOT)
  const char* startother = (char*)(FIRMWARE_ADDRESS+BOOTLOADER_SIZE);
#else
  const char* startother = (char*)(FIRMWARE_ADDRESS);
#endif
  if (buffer != nullptr)
    startother=buffer;

  const char* other_str = nullptr;
  for (int i=0; i< 1024;i++) {
    if (memcmp(startother+i, "opentx-", 7)==0) {
      other_str = startother + i;
      break;
    }
  }
  if (other_str != nullptr)
    return other_str;
  else
    return "no version found";
}
#endif
