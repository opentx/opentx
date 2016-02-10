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

enum menuGeneralHwItems {
  ITEM_SETUP_HW_CALIBRATION,
  ITEM_SETUP_HW_LABEL_STICKS,
  ITEM_SETUP_HW_STICK1,
  ITEM_SETUP_HW_STICK2,
  ITEM_SETUP_HW_STICK3,
  ITEM_SETUP_HW_STICK4,
  ITEM_SETUP_HW_LABEL_POTS,
  ITEM_SETUP_HW_POT1,
  ITEM_SETUP_HW_POT2,
  ITEM_SETUP_HW_POT3,
  ITEM_SETUP_HW_LS,
  ITEM_SETUP_HW_RS,
  ITEM_SETUP_HW_LS2,
  ITEM_SETUP_HW_RS2,
  ITEM_SETUP_HW_LABEL_SWITCHES,
  ITEM_SETUP_HW_SA,
  ITEM_SETUP_HW_SB,
  ITEM_SETUP_HW_SC,
  ITEM_SETUP_HW_SD,
  ITEM_SETUP_HW_SE,
  ITEM_SETUP_HW_SF,
  ITEM_SETUP_HW_SG,
  ITEM_SETUP_HW_SH,
  ITEM_SETUP_HW_BLUETOOTH,
  // ITEM_SETUP_HW_UART3_MODE,
  ITEM_SETUP_HW_MAX
};

#define HW_SETTINGS_COLUMN             150
#define POTS_ROWS                      NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, 0, 0, 0, 0
#define SWITCHES_ROWS                  NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1
#define BLUETOOTH_ROWS                 1,
#define SWITCH_TYPE_MAX(sw)            ((MIXSRC_SF-MIXSRC_FIRST_SWITCH == sw || MIXSRC_SH-MIXSRC_FIRST_SWITCH == sw) ? SWITCH_2POS : SWITCH_3POS)

bool menuGeneralHardware(evt_t event)
{
  MENU(STR_HARDWARE, LBM_RADIO_ICONS, menuTabGeneral, e_Hardware, ITEM_SETUP_HW_MAX, { 0, LABEL(Sticks), 0, 0, 0, 0, LABEL(Pots), POTS_ROWS, LABEL(Switches), SWITCHES_ROWS, BLUETOOTH_ROWS 0 });

  uint8_t sub = menuVerticalPosition;

  for (int i=0; i<NUM_BODY_LINES; ++i) {
    coord_t y = MENU_CONTENT_TOP + i*FH;
    int k = i + menuVerticalOffset;
    for (int j=0; j<=k; j++) {
      if (mstate_tab[j] == HIDDEN_ROW)
        k++;
    }
    LcdFlags attr = (sub == k ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
    switch (k) {
      case ITEM_SETUP_HW_CALIBRATION:
        lcdDrawText(MENUS_MARGIN_LEFT, y, "Calibration", attr);
        if (attr && s_editMode>0) {
          pushMenu(menuGeneralCalib);
        }
        break;
      case ITEM_SETUP_HW_LABEL_STICKS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_STICKS);
        break;
      case ITEM_SETUP_HW_STICK1:
      case ITEM_SETUP_HW_STICK2:
      case ITEM_SETUP_HW_STICK3:
      case ITEM_SETUP_HW_STICK4:
      {
        int idx = k - ITEM_SETUP_HW_STICK1;
        lcdDrawTextAtIndex(INDENT_WIDTH, y, STR_VSRCRAW, idx+1, 0);
        if (ZEXIST(g_eeGeneral.anaNames[idx]) || attr)
          editName(HW_SETTINGS_COLUMN, y, g_eeGeneral.anaNames[idx], LEN_ANA_NAME, event, attr);
        else
          lcdDrawTextAtIndex(HW_SETTINGS_COLUMN, y, STR_MMMINV, 0, 0);
        break;
      }
      case ITEM_SETUP_HW_LS:
      case ITEM_SETUP_HW_RS:
      case ITEM_SETUP_HW_LS2:
      case ITEM_SETUP_HW_RS2:
      {
        int idx = k - ITEM_SETUP_HW_LS;
        uint8_t mask = (0x01 << idx);
        lcdDrawTextAtIndex(INDENT_WIDTH, y, STR_VSRCRAW, NUM_STICKS+NUM_XPOTS+idx+1, menuHorizontalPosition < 0 ? attr : 0);
        if (ZEXIST(g_eeGeneral.anaNames[NUM_STICKS+NUM_XPOTS+2+idx]) || (attr && menuHorizontalPosition == 0))
          editName(HW_SETTINGS_COLUMN, y, g_eeGeneral.anaNames[NUM_STICKS+NUM_XPOTS+idx], LEN_ANA_NAME, event, attr && menuHorizontalPosition == 0);
        else
          lcdDrawTextAtIndex(HW_SETTINGS_COLUMN, y, STR_MMMINV, 0, 0);
        uint8_t potType = (g_eeGeneral.slidersConfig & mask) >> idx;
        potType = selectMenuItem(HW_SETTINGS_COLUMN+50, y, STR_SLIDERTYPES, potType, SLIDER_NONE, SLIDER_WITH_DETENT, menuHorizontalPosition == 1 ? attr : 0, event);
        g_eeGeneral.slidersConfig &= ~mask;
        g_eeGeneral.slidersConfig |= (potType << idx);
        break;
      }
      case ITEM_SETUP_HW_LABEL_POTS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_POTS);
        break;
      case ITEM_SETUP_HW_POT1:
      case ITEM_SETUP_HW_POT2:
      case ITEM_SETUP_HW_POT3:
      {
        int idx = k - ITEM_SETUP_HW_POT1;
        uint8_t shift = (2*idx);
        uint8_t mask = (0x03 << shift);
        lcdDrawTextAtIndex(INDENT_WIDTH, y, STR_VSRCRAW, NUM_STICKS+idx+1, menuHorizontalPosition < 0 ? attr : 0);
        if (ZEXIST(g_eeGeneral.anaNames[NUM_STICKS+idx]) || (attr && menuHorizontalPosition == 0))
          editName(HW_SETTINGS_COLUMN, y, g_eeGeneral.anaNames[NUM_STICKS+idx], LEN_ANA_NAME, event, attr && menuHorizontalPosition == 0);
        else
          lcdDrawTextAtIndex(HW_SETTINGS_COLUMN, y, STR_MMMINV, 0, 0);
        uint8_t potType = (g_eeGeneral.potsConfig & mask) >> shift;
        potType = selectMenuItem(HW_SETTINGS_COLUMN+50, y, STR_POTTYPES, potType, POT_NONE, POT_WITHOUT_DETENT, menuHorizontalPosition == 1 ? attr : 0, event);
        g_eeGeneral.potsConfig &= ~mask;
        g_eeGeneral.potsConfig |= (potType << shift);
        break;
      }
      case ITEM_SETUP_HW_LABEL_SWITCHES:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_SWITCHES);
        break;
      case ITEM_SETUP_HW_SA:
      case ITEM_SETUP_HW_SB:
      case ITEM_SETUP_HW_SC:
      case ITEM_SETUP_HW_SD:
      case ITEM_SETUP_HW_SE:
      case ITEM_SETUP_HW_SF:
      case ITEM_SETUP_HW_SG:
      case ITEM_SETUP_HW_SH:
      {
        int index = k-ITEM_SETUP_HW_SA;
        int config = SWITCH_CONFIG(index);
        lcdDrawTextAtIndex(INDENT_WIDTH, y, STR_VSRCRAW, MIXSRC_FIRST_SWITCH-MIXSRC_Rud+index+1, menuHorizontalPosition < 0 ? attr : 0);
        if (ZEXIST(g_eeGeneral.switchNames[index]) || (attr && menuHorizontalPosition == 0))
          editName(HW_SETTINGS_COLUMN, y, g_eeGeneral.switchNames[index], LEN_SWITCH_NAME, event, menuHorizontalPosition == 0 ? attr : 0);
        else
          lcdDrawTextAtIndex(HW_SETTINGS_COLUMN, y, STR_MMMINV, 0, 0);
        config = selectMenuItem(HW_SETTINGS_COLUMN+50, y, STR_SWTYPES, config, SWITCH_NONE, SWITCH_TYPE_MAX(index), menuHorizontalPosition == 1 ? attr : 0, event);
        if (attr && checkIncDec_Ret) {
          swconfig_t mask = (swconfig_t)0x03 << (2*index);
          g_eeGeneral.switchConfig = (g_eeGeneral.switchConfig & ~mask) | ((swconfig_t(config) & 0x03) << (2*index));
        }
        break;
      }

      case ITEM_SETUP_HW_BLUETOOTH:
        lcdDrawText(MENUS_MARGIN_LEFT, y, "Bluetooth");
        drawCheckBox(HW_SETTINGS_COLUMN, y, g_eeGeneral.bluetoothEnable, menuHorizontalPosition == 0 ? attr : 0);
        if (attr && menuHorizontalPosition == 0) {
          g_eeGeneral.bluetoothEnable = checkIncDecGen(event, g_eeGeneral.bluetoothEnable, 0, 1);
        }
        editName(HW_SETTINGS_COLUMN+50, y, g_eeGeneral.bluetoothName, LEN_BLUETOOTH_NAME, event, menuHorizontalPosition == 1 ? attr : 0);
        break;

#if 0
      case ITEM_SETUP_HW_UART3_MODE:
        g_eeGeneral.serial2Mode = selectMenuItem(HW_SETTINGS_COLUMN, y, STR_UART3MODE, STR_UART3MODES, g_eeGeneral.serial2Mode, 0, UART_MODE_MAX, attr, event);
        if (attr && checkIncDec_Ret) {
          serial2Init(g_eeGeneral.serial2Mode, MODEL_TELEMETRY_PROTOCOL());
        }
        break;
#endif
    }
  }

  return true;
}
