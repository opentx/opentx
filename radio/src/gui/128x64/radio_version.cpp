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

#if defined(PXX2)
void menuRadioModulesVersion(event_t event)
{
  if (event == EVT_ENTRY) {
    moduleSettings[INTERNAL_MODULE].mode = MODULE_MODE_GET_HARDWARE_INFO;
    reusableBuffer.hardware.modules[INTERNAL_MODULE].step = -1;
    reusableBuffer.hardware.modules[INTERNAL_MODULE].timeout = 0;
  }

  SIMPLE_SUBMENU("MODULES / RX VERSION", 0);
}
#endif

enum MenuRadioVersionItems
{
  ITEM_RADIO_VERSION_FIRST = HEADER_LINE - 1,
#if defined(PXX2)
  ITEM_RADIO_MODULES_VERSION,
#endif
#if defined(EEPROM_RLC)
  ITEM_RADIO_BACKUP_EEPROM,
  ITEM_RADIO_FACTORY_RESET,
#endif
  ITEM_RADIO_VERSION_COUNT
};

void menuRadioVersion(event_t event)
{
#if defined(EEPROM_RLC)
  if (warningResult) {
    warningResult = 0;
    showMessageBox(STR_STORAGE_FORMAT);
    storageEraseAll(false);
    NVIC_SystemReset();
  }
#endif

  SIMPLE_MENU(STR_MENUVERSION, menuTabGeneral, MENU_RADIO_VERSION, ITEM_RADIO_VERSION_COUNT);

  coord_t y = MENU_HEADER_HEIGHT + 1;
  lcdDrawTextAlignedLeft(y, vers_stamp);
  y += 4*FH;
  // TODO this was good on AVR radios, but horrible now ...

#if defined(COPROCESSOR)
  if (Coproc_valid == 1) {
    lcdDrawTextAlignedLeft(y, "CoPr:");
    lcdDraw8bitsNumber(10*FW, y, Coproc_read);
  }
  else {
    lcdDrawTextAlignedLeft(y, "CoPr: ---");
  }
  y += FH;
#endif

#if defined(PXX2)
  lcdDrawText(0, y, BUTTON("Modules / RX version"), menuVerticalPosition == ITEM_RADIO_MODULES_VERSION ? INVERS : 0);
  y += FH;
  if (menuVerticalPosition == ITEM_RADIO_MODULES_VERSION && event == EVT_KEY_BREAK(KEY_ENTER)) {
    s_editMode = EDIT_SELECT_FIELD;
    pushMenu(menuRadioModulesVersion);
  }
#endif

#if defined(EEPROM_RLC)
  lcdDrawText(0, y, BUTTON(TR_EEBACKUP), menuVerticalPosition == ITEM_RADIO_BACKUP_EEPROM ? INVERS : 0);
  y += FH;
  if (menuVerticalPosition == ITEM_RADIO_BACKUP_EEPROM && event == EVT_KEY_BREAK(KEY_ENTER)) {
    s_editMode = EDIT_SELECT_FIELD;
    eepromBackup();
  }

  lcdDrawText(0, y, BUTTON(TR_FACTORYRESET), menuVerticalPosition == ITEM_RADIO_FACTORY_RESET ? INVERS : 0);
  // y += FH;
  if (menuVerticalPosition == ITEM_RADIO_FACTORY_RESET && event == EVT_KEY_BREAK(KEY_ENTER)) {
    s_editMode = EDIT_SELECT_FIELD;
    POPUP_CONFIRMATION(STR_CONFIRMRESET);
  }
#endif
}
