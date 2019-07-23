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

// TODO duplicated code
#if defined(ROTARY_ENCODER_NAVIGATION)
#define EVT_KEY_NEXT_LINE              EVT_ROTARY_RIGHT
#define EVT_KEY_PREVIOUS_LINE          EVT_ROTARY_LEFT
#else
#define EVT_KEY_NEXT_LINE              EVT_KEY_FIRST(KEY_DOWN)
#define EVT_KEY_PREVIOUS_LINE          EVT_KEY_FIRST(KEY_UP)
#endif

#define MENU_BODY_TOP    (FH + 1)
#define MENU_BODY_BOTTOM (LCD_H)

constexpr uint8_t COLUMN2_X = 10 * FW;

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

void menuRadioFirmwareOptions(event_t event)
{
  SIMPLE_SUBMENU(STR_MENU_FIRM_OPTIONS, ITEM_FW_OPT_MAX);

  coord_t y = (FH + 1) - menuVerticalOffset * FH;
  uint8_t lines = (y - (FH + 1)) / FH + menuVerticalOffset;

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

  for (uint8_t i=0; i<NUM_BODY_LINES; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i * FH;
    int k = i + menuVerticalOffset;
    LcdFlags attr = (menuVerticalPosition == k ? (s_editMode > 0 ? BLINK | INVERS : INVERS) : 0);

    switch(k) {
#if defined(AUTOUPDATE)
      case ITEM_FW_OPT_AUTOUPDATE:
        lcdDrawText(INDENT_WIDTH, y, "autoupdate", attr);
        break;
#endif
#if defined(CROSSFIRE)
      case ITEM_FW_OPT_CROSSFIRE:
        lcdDrawText(INDENT_WIDTH, y, "crossfire", attr);
        break;
#endif
#if defined(MODULE_D16_EU_ONLY_SUPPORT)
      case ITEM_FW_OPT_MODULE_D16_EU_ONLY_SUPPORT:
        lcdDrawText(INDENT_WIDTH, y, "eu", attr);
        break;
#endif
#if defined(FAI)
      case ITEM_FW_OPT_FAI:
        lcdDrawText(INDENT_WIDTH, y, "faimode", attr);
        break;
#endif
#if defined(FAI_CHOICE)
      case ITEM_FW_OPT_FAI_CHOICE:
        lcdDrawText(INDENT_WIDTH, y, "faichoice", attr);
        break;
#endif
#if defined(R9M_PROTO_FLEX)
      case ITEM_FW_OPT_R9M_PROTO_FLEX:
        lcdDrawText(INDENT_WIDTH, y, "flexr9m", attr);
        break;
#endif
#if !defined(GVARS)
      case ITEM_FW_OPT_NO_GVARS:
        lcdDrawText(INDENT_WIDTH, y, "nogvars", attr);
        break;
#endif
#if defined(HAPTIC) && defined(PCBX9D)
      case ITEM_FW_OPT_HAPTIC:
        lcdDrawText(INDENT_WIDTH, y, "haptic", attr);
        break;
#endif
#if !defined(HELI)
      case ITEM_FW_OPT_NO_HELI:
        lcdDrawText(INDENT_WIDTH, y, "noheli", attr);
        break;
#endif
#if defined(HORUS_STICKS)
      case ITEM_FW_OPT_HORUS_STICKS:
        lcdDrawText(INDENT_WIDTH, y, "horussticks", attr);
        break;
#endif
#if defined(INTERNAL_MODULE_PPM)
      case ITEM_FW_OPT_INTERNAL_MODULE_PPM:
        lcdDrawText(INDENT_WIDTH, y, "internalppm", attr);
        break;
#endif
#if defined(MULTIMODULE)
      case ITEM_FW_OPT_MULTIMODULE:
        lcdDrawText(INDENT_WIDTH, y, "multimodule", attr);
        break;
#endif
#if defined(LUA_MODEL_SCRIPTS)
      case ITEM_FW_OPT_LUA_MODEL_SCRIPTS:
        lcdDrawText(INDENT_WIDTH, y, "lua", attr);
        break;
#endif
#if defined(LUA_COMPILER)
      case ITEM_FW_OPT_LUA_COMPILER:
        lcdDrawText(INDENT_WIDTH, y, "luac", attr);
        break;
#endif
#if !defined(OVERRIDE_CHANNEL_FUNCTION)
      case ITEM_FW_OPT_NO_OVERRIDE_CHANNEL_FUNCTION:
        lcdDrawText(INDENT_WIDTH, y, "nooverridech", attr);
        break;
#endif
#if defined(PPM_UNIT_US)
      case ITEM_FW_OPT_PPM_UNIT_US:
        lcdDrawText(INDENT_WIDTH, y, "ppmus", attr);
        break;
#endif
#if defined(NO_RAS)
      case ITEM_FW_OPT_NO_RAS:
        lcdDrawText(INDENT_WIDTH, y, "noras", attr);
        break;
#endif

#if defined(SHUTDOWN_CONFIRMATION)
      case ITEM_FW_OPT_SHUTDOWN_CONFIRMATION:
        lcdDrawText(INDENT_WIDTH, y, "shutdownconfirm", attr);
        break;
#endif
    }
  }
}

#if defined(PXX2)
void drawPXX2Version(coord_t x, coord_t y, PXX2Version version)
{
  if (version.major == 0xFF && version.minor == 0x0F && version.revision == 0x0F) {
    lcdDrawText(x, y, "---");
  }
  else {
    lcdDrawNumber(x, y, 1 + version.major, LEFT);
    lcdDrawChar(lcdNextPos, y, '.');
    lcdDrawNumber(lcdNextPos, y, version.minor, LEFT);
    lcdDrawChar(lcdNextPos, y, '.');
    lcdDrawNumber(lcdNextPos, y, version.revision, LEFT);
  }
}

void drawPXX2FullVersion(coord_t x, coord_t y, PXX2Version hwVersion, PXX2Version swVersion)
{
  drawPXX2Version(x, y, hwVersion);
  lcdDrawText(lcdNextPos, y, "/");
  drawPXX2Version(lcdNextPos, y, swVersion);
}

void menuRadioModulesVersion(event_t event)
{
  if (menuEvent) {
    moduleState[INTERNAL_MODULE].mode = MODULE_MODE_NORMAL;
    moduleState[EXTERNAL_MODULE].mode = MODULE_MODE_NORMAL;
    return;
  }

  title(STR_MENU_MODULES_RX_VERSION);

  if (event == EVT_ENTRY) {
    memclear(&reusableBuffer.hardwareAndSettings.modules, sizeof(reusableBuffer.hardwareAndSettings.modules));
  }

  if (event == EVT_ENTRY || get_tmr10ms() >= reusableBuffer.hardwareAndSettings.updateTime) {
    if (isModulePXX2(INTERNAL_MODULE) && IS_INTERNAL_MODULE_ON()) {
      moduleState[INTERNAL_MODULE].readModuleInformation(&reusableBuffer.hardwareAndSettings.modules[INTERNAL_MODULE], PXX2_HW_INFO_TX_ID, PXX2_MAX_RECEIVERS_PER_MODULE - 1);
    }

    if (isModulePXX2(EXTERNAL_MODULE) && IS_EXTERNAL_MODULE_ON()) {
      moduleState[EXTERNAL_MODULE].readModuleInformation(&reusableBuffer.hardwareAndSettings.modules[EXTERNAL_MODULE], PXX2_HW_INFO_TX_ID, PXX2_MAX_RECEIVERS_PER_MODULE - 1);
    }

    reusableBuffer.hardwareAndSettings.updateTime = get_tmr10ms() + 1000 /* 10s*/;
  }

  coord_t y = (FH + 1) - menuVerticalOffset * FH;

  for (uint8_t module=0; module<NUM_MODULES; module++) {
    // Label
    if (y >= MENU_BODY_TOP && y < MENU_BODY_BOTTOM) {
      if (module == INTERNAL_MODULE)
        lcdDrawTextAlignedLeft(y, STR_INTERNAL_MODULE);
      else if (module == EXTERNAL_MODULE)
        lcdDrawTextAlignedLeft(y, STR_EXTERNAL_MODULE);
    }
    y += FH;

    // Module model
    if (y >= MENU_BODY_TOP && y < MENU_BODY_BOTTOM) {
      lcdDrawText(INDENT_WIDTH, y, STR_MODULE);
      if (module == INTERNAL_MODULE) {
        if (!IS_INTERNAL_MODULE_ON()) {
          lcdDrawText(COLUMN2_X, y, STR_OFF);
          y += FH;
          continue;
        }
        if (!isModulePXX2(INTERNAL_MODULE)) {
          lcdDrawText(COLUMN2_X, y, STR_NO_INFORMATION);
          y += FH;
          continue;
        }
      }
      else if (module == EXTERNAL_MODULE) {
        if (!IS_EXTERNAL_MODULE_ON()) {
          lcdDrawText(COLUMN2_X, y, STR_OFF);
          y += FH;
          continue;
        }
        if (!isModulePXX2(EXTERNAL_MODULE)) {
          lcdDrawText(COLUMN2_X, y, STR_NO_INFORMATION);
          y += FH;
          continue;
        }
      }
      uint8_t modelId = reusableBuffer.hardwareAndSettings.modules[module].information.modelID;
      lcdDrawText(COLUMN2_X, y, getPXX2ModuleName(modelId));
    }
    y += FH;

    // Module version
    if (y >= MENU_BODY_TOP && y < MENU_BODY_BOTTOM) {
      if (reusableBuffer.hardwareAndSettings.modules[module].information.modelID) {
        drawPXX2FullVersion(COLUMN2_X, y, reusableBuffer.hardwareAndSettings.modules[module].information.hwVersion, reusableBuffer.hardwareAndSettings.modules[module].information.swVersion);
        static const char * variants[] = {"FCC", "EU", "FLEX"};
        uint8_t variant = reusableBuffer.hardwareAndSettings.modules[module].information.variant - 1;
        if (variant < DIM(variants)) {
          lcdDrawText(lcdNextPos + 1, y, variants[variant]);
        }
      }
    }
    y += FH;

    for (uint8_t receiver=0; receiver<PXX2_MAX_RECEIVERS_PER_MODULE; receiver++) {
      if (reusableBuffer.hardwareAndSettings.modules[module].receivers[receiver].information.modelID && reusableBuffer.hardwareAndSettings.modules[module].receivers[receiver].timestamp < get_tmr10ms() + 2000) {
        // Receiver model
        if (y >= MENU_BODY_TOP && y < MENU_BODY_BOTTOM) {
          lcdDrawText(INDENT_WIDTH, y, STR_RECEIVER);
          lcdDrawNumber(lcdLastRightPos + 2, y, receiver + 1);
          uint8_t modelId = reusableBuffer.hardwareAndSettings.modules[module].receivers[receiver].information.modelID;
          lcdDrawText(COLUMN2_X, y, getPXX2ReceiverName(modelId));
        }
        y += FH;

        // Receiver version
        if (y >= MENU_BODY_TOP && y < MENU_BODY_BOTTOM) {
          drawPXX2FullVersion(COLUMN2_X, y, reusableBuffer.hardwareAndSettings.modules[module].receivers[receiver].information.hwVersion, reusableBuffer.hardwareAndSettings.modules[module].receivers[receiver].information.swVersion);
        }
        y += FH;
      }
    }
  }

  uint8_t lines = (y - (FH + 1)) / FH + menuVerticalOffset;
  if (lines > NUM_BODY_LINES) {
    drawVerticalScrollbar(LCD_W-1, FH, LCD_H-FH, menuVerticalOffset, lines, NUM_BODY_LINES);
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
#if defined(PCBTARANIS) || defined(PCBHORUS)
  ITEM_RADIO_FIRMWARE_OPTIONS,
#endif
#if defined(PXX2)
  ITEM_RADIO_MODULES_VERSION,
#endif
  ITEM_RADIO_VERSION_COUNT
};

void menuRadioVersion(event_t event)
{
  SIMPLE_MENU(STR_MENUVERSION, menuTabGeneral, MENU_RADIO_VERSION, ITEM_RADIO_VERSION_COUNT);

  coord_t y = MENU_HEADER_HEIGHT + 2;
  lcdDrawText(FW, y, vers_stamp, SMLSIZE);
  y += 4 * (FH - 1);

#if defined(COPROCESSOR)
  lcdDrawText(FW, y, "COPR\037\033: ", SMLSIZE);
  if (Coproc_valid == 1)
    lcdDrawNumber(lcdNextPos, y, Coproc_read, SMLSIZE);
  else
    lcdDrawText(lcdNextPos, y, "---", SMLSIZE);
  y += FH - 1;
#endif
  y += 2;
#if defined(PCBTARANIS) || defined(PCBHORUS)
  lcdDrawText(INDENT_WIDTH, y, BUTTON(TR_FIRMWARE_OPTIONS), menuVerticalPosition == ITEM_RADIO_FIRMWARE_OPTIONS ? INVERS : 0);
  y += FH;
  if (menuVerticalPosition == ITEM_RADIO_FIRMWARE_OPTIONS && event == EVT_KEY_BREAK(KEY_ENTER)) {
    s_editMode = EDIT_SELECT_FIELD;
    pushMenu(menuRadioFirmwareOptions);
  }
#endif
#if defined(PXX2)
  lcdDrawText(INDENT_WIDTH, y, BUTTON(TR_MODULES_RX_VERSION), menuVerticalPosition == ITEM_RADIO_MODULES_VERSION ? INVERS : 0);
  y += FH;
  if (menuVerticalPosition == ITEM_RADIO_MODULES_VERSION && event == EVT_KEY_BREAK(KEY_ENTER)) {
    s_editMode = EDIT_SELECT_FIELD;
    pushMenu(menuRadioModulesVersion);
  }
#endif
}
