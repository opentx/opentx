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

#include <opentx.h>
#include "opentx.h"

// TODO duplicated code
#if defined(PCBX7) || defined(PCBX9E) || defined(PCBX3)
#define EVT_KEY_NEXT_LINE              EVT_ROTARY_RIGHT
#define EVT_KEY_PREVIOUS_LINE          EVT_ROTARY_LEFT
#else
#define EVT_KEY_NEXT_LINE              EVT_KEY_FIRST(KEY_DOWN)
#define EVT_KEY_PREVIOUS_LINE          EVT_KEY_FIRST(KEY_UP)
#endif

#define MENU_BODY_TOP    (FH + 1)
#define MENU_BODY_BOTTOM (LCD_H)

#if defined(PXX2)
void drawPXX2Version(coord_t x, coord_t y, PXX2Version version)
{
  lcdDrawNumber(x, y, version.major, LEFT);
  lcdDrawChar(lcdNextPos, y, '.');
  lcdDrawNumber(lcdNextPos, y, version.minor, LEFT);
  lcdDrawChar(lcdNextPos, y, '.');
  lcdDrawNumber(lcdNextPos, y, version.revision, LEFT);
}

void drawPXX2FullVersion(coord_t x, coord_t y, PXX2Version hwVersion, PXX2Version swVersion)
{
  drawPXX2Version(x, y, hwVersion);
  lcdDrawText(lcdNextPos, y, "/");
  drawPXX2Version(lcdNextPos, y, swVersion);
}

static const char * const modulesModels[] = {
  "---",
  "XJT",
  "IXJT",
  "IXJT-PRO",
  "IXJT-S",
  "R9M",
  "R9MLite",
  "R9MLite-PRO",
};

static const char * const receiversModels[] = {
  "---",
  "X8R",
  "RX8R",
  "RX8R-PRO",
  "RX6R",
  "RX4R",
  "G-RX8",
  "G-RX6",
  "X6R",
  "X4R",
  "X4R-SB",
  "XSR",
  "XSR-M",
  "RXSR",
  "S6R",
  "S8R",
  "XM",
  "XM+",
  "XMR",
  "R9",
  "R9-SLIM",
  "R9-SLIM+",
  "R9-MINI",
  "R9-MM",
  "R9-STAB",
};

void menuRadioModulesVersion(event_t event)
{
  if (menuEvent) {
    moduleSettings[INTERNAL_MODULE].mode = MODULE_MODE_NORMAL;
    moduleSettings[EXTERNAL_MODULE].mode = MODULE_MODE_NORMAL;
    return;
  }

  TITLE("MODULES / RX VERSION");

  if (event == EVT_ENTRY || get_tmr10ms() >= reusableBuffer.hardwareAndSettings.updateTime) {
    // menuVerticalOffset = 0;

    memclear(&reusableBuffer.hardwareAndSettings.modules, sizeof(reusableBuffer.hardwareAndSettings.modules));

    if (isModulePXX2(INTERNAL_MODULE) && IS_INTERNAL_MODULE_ON()) {
      moduleSettings[INTERNAL_MODULE].mode = MODULE_MODE_GET_HARDWARE_INFO;
      reusableBuffer.hardwareAndSettings.modules[INTERNAL_MODULE].current = PXX2_HW_INFO_TX_ID;
      reusableBuffer.hardwareAndSettings.modules[INTERNAL_MODULE].maximum = MAX_RECEIVERS_PER_MODULE - 1;
    }

    if (isModulePXX2(EXTERNAL_MODULE) && IS_EXTERNAL_MODULE_ON()) {
      moduleSettings[EXTERNAL_MODULE].mode = MODULE_MODE_GET_HARDWARE_INFO;
      reusableBuffer.hardwareAndSettings.modules[EXTERNAL_MODULE].current = PXX2_HW_INFO_TX_ID;
      reusableBuffer.hardwareAndSettings.modules[EXTERNAL_MODULE].maximum = MAX_RECEIVERS_PER_MODULE - 1;
    }

    reusableBuffer.hardwareAndSettings.updateTime = get_tmr10ms() + 1000 /* 10s*/;
  }

  coord_t y = (FH + 1) - menuVerticalOffset * FH;

  for (uint8_t module=0; module<NUM_MODULES; module++) {
    if (module == INTERNAL_MODULE) {
      if (!isModulePXX2(INTERNAL_MODULE) || !IS_INTERNAL_MODULE_ON()) {
        continue;
      }
    }
    else if (module == EXTERNAL_MODULE) {
      if (!isModulePXX2(EXTERNAL_MODULE) || !IS_EXTERNAL_MODULE_ON()) {
        continue;
      }
    }

    // Label
    if (y >= MENU_BODY_TOP && y < MENU_BODY_BOTTOM) {
      if (module == INTERNAL_MODULE)
        lcdDrawTextAlignedLeft(y, "Internal module");
      else if (module == EXTERNAL_MODULE)
        lcdDrawTextAlignedLeft(y, "External module");
    }
    y += FH;

    // Module model
    if (y >= MENU_BODY_TOP && y < MENU_BODY_BOTTOM) {
      lcdDrawText(INDENT_WIDTH, y, "Model");
      uint8_t modelId = reusableBuffer.hardwareAndSettings.modules[module].information.modelID;
      if (modelId >= DIM(modulesModels))
        modelId = 0;
      lcdDrawText(12 * FW, y, modulesModels[modelId]);
    }
    y += FH;

    // Module version
    if (y >= MENU_BODY_TOP && y < MENU_BODY_BOTTOM) {
      lcdDrawText(INDENT_WIDTH, y, "Version");
      if (reusableBuffer.hardwareAndSettings.modules[module].information.modelID) {
        drawPXX2FullVersion(12 * FW, y, reusableBuffer.hardwareAndSettings.modules[module].information.hwVersion, reusableBuffer.hardwareAndSettings.modules[module].information.swVersion);
      }
    }
    y += FH;

    for (uint8_t receiver=0; receiver<PXX2_MAX_RECEIVERS_PER_MODULE; receiver++) {
      if (reusableBuffer.hardwareAndSettings.modules[module].receivers[receiver].information.modelID) {
        // Receiver model
        if (y >= MENU_BODY_TOP && y < MENU_BODY_BOTTOM) {
          lcdDrawText(INDENT_WIDTH, y, "Receiver");
          lcdDrawNumber(lcdLastRightPos + 2, y, receiver + 1);
          uint8_t modelId = reusableBuffer.hardwareAndSettings.modules[module].receivers[receiver].information.modelID;
          if (modelId >= DIM(receiversModels))
            modelId = 0;
          lcdDrawText(12 * FW, y, receiversModels[modelId]);
        }
        y += FH;

        // Receiver version
        if (y >= MENU_BODY_TOP && y < MENU_BODY_BOTTOM) {
          drawPXX2FullVersion(12 * FW, y, reusableBuffer.hardwareAndSettings.modules[module].receivers[receiver].information.hwVersion, reusableBuffer.hardwareAndSettings.modules[module].receivers[receiver].information.swVersion);
        }
        y += FH;
      }
    }
  }

  uint8_t lines = (y - (FH + 1)) / FH + menuVerticalOffset;
  if (lines > NUM_BODY_LINES) {
    drawVerticalScrollbar(LCD_W-1, FH, LCD_H-FH, menuVerticalOffset, lines, NUM_BODY_LINES);
  }
  else {
    menuVerticalOffset = 0;
  }

  switch(event) {
    case EVT_KEY_PREVIOUS_LINE:
      if (lines > NUM_BODY_LINES) {
        if (menuVerticalOffset-- == 0)
          menuVerticalOffset = lines - 1;
      }
      break;

    case EVT_KEY_NEXT_LINE:
      if (lines > NUM_BODY_LINES) {
        if (++menuVerticalOffset + NUM_BODY_LINES > lines)
          menuVerticalOffset = 0;
      }
      break;

    case EVT_KEY_BREAK(KEY_EXIT):
      if (menuVerticalOffset != 0)
        menuVerticalOffset = 0;
      else
        popMenu();
      break;
  }
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
  lcdDrawText(INDENT_WIDTH, y, BUTTON("Modules / RX version"), menuVerticalPosition == ITEM_RADIO_MODULES_VERSION ? INVERS : 0);
  y += FH;
  if (menuVerticalPosition == ITEM_RADIO_MODULES_VERSION && event == EVT_KEY_BREAK(KEY_ENTER)) {
    s_editMode = EDIT_SELECT_FIELD;
    pushMenu(menuRadioModulesVersion);
  }
#endif

#if defined(EEPROM_RLC)
  lcdDrawText(INDENT_WIDTH, y, BUTTON(STR_EEBACKUP), menuVerticalPosition == ITEM_RADIO_BACKUP_EEPROM ? INVERS : 0);
  y += FH;
  if (menuVerticalPosition == ITEM_RADIO_BACKUP_EEPROM && event == EVT_KEY_BREAK(KEY_ENTER)) {
    s_editMode = EDIT_SELECT_FIELD;
    eepromBackup();
  }

  lcdDrawText(INDENT_WIDTH, y, BUTTON(STR_FACTORYRESET), menuVerticalPosition == ITEM_RADIO_FACTORY_RESET ? INVERS : 0);
  // y += FH;
  if (menuVerticalPosition == ITEM_RADIO_FACTORY_RESET && event == EVT_KEY_BREAK(KEY_ENTER)) {
    s_editMode = EDIT_SELECT_FIELD;
    POPUP_CONFIRMATION(STR_CONFIRMRESET, nullptr);
  }
#endif
}
