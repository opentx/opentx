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

enum menuRadioFirmwareOptionsItems {
#if defined(AUTOUPDATE)
  ITEM_FW_OPT_AUTOUPDATE,
#endif
#if defined(CROSSFIRE)
  ITEM_FW_OPT_CROSSFIRE,
#endif
#if defined(MODULE_D16_EU_ONLY_SUPPORT)
  ITEM_FW_OPT_MODULE_D16_EU_ONLY_SUPPORT,
#endif
#if defined(FAI)
  ITEM_FW_OPT_FAI,
#endif
#if defined(FAI_CHOICE)
  ITEM_FW_OPT_FAI_CHOICE,
#endif
#if defined(R9M_PROTO_FLEX)
  ITEM_FW_OPT_R9M_PROTO_FLEX,
#endif
#if !defined(GVARS)
  ITEM_FW_OPT_NO_GVARS,
#endif
#if defined(HAPTIC) && defined(PCBX9D)
  ITEM_FW_OPT_HAPTIC,
#endif
#if !defined(HELI)
  ITEM_FW_OPT_NO_HELI,
#endif
#if defined(HORUS_STICKS)
  ITEM_FW_OPT_HORUS_STICKS,
#endif
#if defined(INTERNAL_MODULE_PPM)
  ITEM_FW_OPT_INTERNAL_MODULE_PPM,
#endif
#if defined(MULTIMODULE)
  ITEM_FW_OPT_MULTIMODULE,
#endif
#if defined(LUA_MODEL_SCRIPTS)
  ITEM_FW_OPT_LUA_MODEL_SCRIPTS,
#endif
#if defined(LUA_COMPILER)
  ITEM_FW_OPT_LUA_COMPILER,
#endif
#if !defined(OVERRIDE_CHANNEL_FUNCTION)
  ITEM_FW_OPT_NO_OVERRIDE_CHANNEL_FUNCTION,
#endif
#if defined(PPM_UNIT_US)
  ITEM_FW_OPT_PPM_UNIT_US,
#endif
#if defined(NO_RAS)
  ITEM_FW_OPT_NO_RAS,
#endif
#if defined(SHUTDOWN_CONFIRMATION)
  ITEM_FW_OPT_SHUTDOWN_CONFIRMATION,
#endif
// TODO : SQT5 font
    ITEM_FW_OPT_MAX
};

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

  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP, vers_stamp);
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + FH, date_stamp);
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 2*FH, time_stamp);
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 3*FH, eeprom_stamp);
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_CONTENT_TOP + 4*FH, "UID:");
  lcdDrawText(MENUS_MARGIN_LEFT + 64, MENU_CONTENT_TOP + 4*FH, id);

#if 0
  if (event == EVT_KEY_LONG(KEY_ENTER)) {
    killEvents(event);
    // POPUP_MENU_ADD_ITEM(STR_EEBACKUP);
    POPUP_MENU_ADD_ITEM(STR_FACTORYRESET);
    POPUP_MENU_START(onVersionMenu);
  }
#endif

  coord_t y = MENU_CONTENT_TOP + 5*FH;
  lcdDrawText(MENUS_MARGIN_LEFT, y, "OPTS:");
  lcdNextPos = MENUS_MARGIN_LEFT + 64;

  for(uint8_t i=0; i < ITEM_FW_OPT_MAX; i++) {
    switch (i) {
#if defined(CROSSFIRE)
      case ITEM_FW_OPT_CROSSFIRE:
        lcdDrawText(lcdNextPos, y, "crossfire", 0);
        break;
#endif
#if defined(MODULE_D16_EU_ONLY_SUPPORT)
      case ITEM_FW_OPT_MODULE_D16_EU_ONLY_SUPPORT:
        lcdDrawText(lcdNextPos,  y, "eu", 0);
        break;
#endif
#if defined(FAI)
      case ITEM_FW_OPT_FAI:
        lcdDrawText(lcdNextPos, y, "faimode", 0);
        break;
#endif
#if defined(FAI_CHOICE)
      case ITEM_FW_OPT_FAI_CHOICE:
        lcdDrawText(lcdNextPos, y, "faichoice", 0);
        break;
#endif
#if defined(R9M_PROTO_FLEX)
      case ITEM_FW_OPT_R9M_PROTO_FLEX:
        lcdDrawText(lcdNextPos, y, "flexr9m", 0);
        break;
#endif
#if !defined(GVARS)
      case ITEM_FW_OPT_NO_GVARS:
        lcdDrawText(lcdNextPos, y, "nogvars", 0);
        break;
#endif
#if defined(HAPTIC) && defined(PCBX9D)
      case ITEM_FW_OPT_HAPTIC:
        lcdDrawText(lcdNextPos, y, "haptic", 0);
        break;
#endif
#if !defined(HELI)
      case ITEM_FW_OPT_NO_HELI:
        lcdDrawText(lcdNextPos, y, "noheli", 0);
        break;
#endif
#if defined(HORUS_STICKS)
      case ITEM_FW_OPT_HORUS_STICKS:
        lcdDrawText(lcdNextPos, y, "horussticks", 0);
        break;
#endif
#if defined(INTERNAL_MODULE_PPM)
      case ITEM_FW_OPT_INTERNAL_MODULE_PPM:
        lcdDrawText(lcdNextPos, y, "internalppm", 0);
        break;
#endif
#if defined(MULTIMODULE)
      case ITEM_FW_OPT_MULTIMODULE:
        lcdDrawText(lcdNextPos, y, "multimodule", 0);
        break;
#endif
#if defined(LUA_MODEL_SCRIPTS)
      case ITEM_FW_OPT_LUA_MODEL_SCRIPTS:
        lcdDrawText(lcdNextPos, y, "lua", 0);
        break;
#endif
#if defined(LUA_COMPILER)
      case ITEM_FW_OPT_LUA_COMPILER:
        lcdDrawText(lcdNextPos, y, "luac", 0);
        break;
#endif
#if !defined(OVERRIDE_CHANNEL_FUNCTION)
      case ITEM_FW_OPT_NO_OVERRIDE_CHANNEL_FUNCTION:
        lcdDrawText(lcdNextPos, y, "nooverridech", 0);
        break;
#endif
#if defined(PPM_UNIT_US)
      case ITEM_FW_OPT_PPM_UNIT_US:
        lcdDrawText(lcdNextPos, y, "ppmus", 0);
        break;
#endif
#if defined(NO_RAS)
      case ITEM_FW_OPT_NO_RAS:
        lcdDrawText(lcdNextPos, y, "noras", 0);
        break;
#endif
#if defined(SHUTDOWN_CONFIRMATION)
      case ITEM_FW_OPT_SHUTDOWN_CONFIRMATION:
        lcdDrawText(lcdNextPos, y, "shutdownconfirm", 0);
        break;
#endif
    }
    if (lcdNextPos > 420) {
      lcdNextPos = MENUS_MARGIN_LEFT;
      y += FH;
     }
     else if (i != ITEM_FW_OPT_MAX - 1 ) {
       lcdDrawText(lcdNextPos, y, ", ");
     }
  }
  return true;
}
