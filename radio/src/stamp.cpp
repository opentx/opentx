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

#define EEPROM_STR DEFNUMSTR(EEPROM_VER);

#if defined(PCBHORUS)
#define TAB "\037\075"
#else
#define TAB "\037\033"
#endif

#if defined(FRSKY_RELEASE)
#define DISPLAY_VERSION "FrSky"
#elif defined(JUMPER_RELEASE)
#define DISPLAY_VERSION "JumperRC"
#elif defined(RADIOMASTER_RELEASE)
#define DISPLAY_VERSION "Radiomaster"
#elif defined(TBS_RELEASE)
#define DISPLAY_VERSION "TBS"
#else
#define DISPLAY_VERSION VERSION
#endif

#if defined(COLORLCD)
  const char vers_stamp[]  =   "VERS" TAB ": " "opentx-" FLAVOUR "-" DISPLAY_VERSION " (" GIT_STR ")";
  const char date_stamp[]  =   "DATE" TAB ": " DATE;
  const char time_stamp[]  =   "TIME" TAB ": " TIME;
  const char eeprom_stamp[]  = "EEPR" TAB ": " EEPROM_STR;
#elif defined(BOARD_NAME)
  const char vers_stamp[]  = "FW" TAB ": opentx-" BOARD_NAME "\036VERS" TAB ": " DISPLAY_VERSION " (" GIT_STR ")" "\036DATE" TAB ": " DATE " " TIME "\036EEPR" TAB ": " EEPROM_STR;
#else
  const char vers_stamp[]  = "FW" TAB ": opentx-" FLAVOUR    "\036VERS" TAB ": " DISPLAY_VERSION " (" GIT_STR ")" "\036DATE" TAB ": " DATE " " TIME "\036EEPR" TAB ": " EEPROM_STR;
#endif

/**
 * Retrieves the version of the bootloader or firmware
 * @return
 */
#if defined(STM32) && !defined(SIMU)

__SECTION_USED(".fwversiondata")   const char firmware_version[] = "opentx-" FLAVOUR "-" DISPLAY_VERSION " (" GIT_STR ")";
__SECTION_USED(".bootversiondata") const char boot_version[] =     "opentx-" FLAVOUR "-" DISPLAY_VERSION " (" GIT_STR ")";

/**
 * Tries to find opentx version in the first 1024 byte of either firmware/bootloader (the one not running) or the buffer
 * @param buffer If non-null find the firmware version in the buffer instead
 */
const char * getOtherVersion(char* buffer)
{
#if defined(BOOT)
  const char * startother = (char*)(FIRMWARE_ADDRESS+BOOTLOADER_SIZE);
#else
  const char * startother = (char*)(FIRMWARE_ADDRESS);
#endif
  if (buffer != nullptr)
    startother = buffer;

  const char * other_str = nullptr;
  for (int i=0; i<1024;i++) {
    if (memcmp(startother+i, "opentx-", 7) == 0) {
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
