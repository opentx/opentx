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

enum menuRadioHwItems {
  ITEM_RADIO_HARDWARE_LABEL_STICKS,
  ITEM_RADIO_HARDWARE_STICK1,
  ITEM_RADIO_HARDWARE_STICK2,
  ITEM_RADIO_HARDWARE_STICK3,
  ITEM_RADIO_HARDWARE_STICK4,
  ITEM_RADIO_HARDWARE_LABEL_POTS,
  ITEM_RADIO_HARDWARE_POT1,
  ITEM_RADIO_HARDWARE_POT2,
  CASE_PCBX9E_PCBX9DP(ITEM_RADIO_HARDWARE_POT3)
  CASE_PCBX9E(ITEM_RADIO_HARDWARE_POT4)
  ITEM_RADIO_HARDWARE_LS,
  ITEM_RADIO_HARDWARE_RS,
  CASE_PCBX9E(ITEM_RADIO_HARDWARE_LS2)
  CASE_PCBX9E(ITEM_RADIO_HARDWARE_RS2)
  ITEM_RADIO_HARDWARE_LABEL_SWITCHES,
  ITEM_RADIO_HARDWARE_SA,
  ITEM_RADIO_HARDWARE_SB,
  ITEM_RADIO_HARDWARE_SC,
  ITEM_RADIO_HARDWARE_SD,
  ITEM_RADIO_HARDWARE_SE,
  ITEM_RADIO_HARDWARE_SF,
  ITEM_RADIO_HARDWARE_SG,
  ITEM_RADIO_HARDWARE_SH,
  CASE_PCBX9E(ITEM_RADIO_HARDWARE_SI)
  CASE_PCBX9E(ITEM_RADIO_HARDWARE_SJ)
  CASE_PCBX9E(ITEM_RADIO_HARDWARE_SK)
  CASE_PCBX9E(ITEM_RADIO_HARDWARE_SL)
  CASE_PCBX9E(ITEM_RADIO_HARDWARE_SM)
  CASE_PCBX9E(ITEM_RADIO_HARDWARE_SN)
  CASE_PCBX9E(ITEM_RADIO_HARDWARE_SO)
  CASE_PCBX9E(ITEM_RADIO_HARDWARE_SP)
  CASE_PCBX9E(ITEM_RADIO_HARDWARE_SQ)
  CASE_PCBX9E(ITEM_RADIO_HARDWARE_SR)
  CASE_BLUETOOTH(ITEM_RADIO_HARDWARE_BLUETOOTH_MODE)
#if defined(USEHORUSBT)
  CASE_BLUETOOTH(ITEM_RADIO_HARDWARE_BLUETOOTH_NAME)
#endif
  ITEM_RADIO_HARDWARE_UART3_MODE,
  ITEM_RADIO_HARDWARE_JITTER_FILTER,
  ITEM_RADIO_HARDWARE_MAX
};

#define HW_SETTINGS_COLUMN 15*FW

#if defined(PCBX9E)
  #define POTS_ROWS      NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1
#elif defined(PCBX9DP)
  #define POTS_ROWS      NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1
#else
  #define POTS_ROWS      NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1
#endif

#if defined(PCBX9E)
  #define SWITCHES_ROWS  NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1
#else
  #define SWITCHES_ROWS  NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1
#endif

#if defined(BLUETOOTH) && defined(USEHORUSBT)
  #define BLUETOOTH_ROWS 0, uint8_t(g_eeGeneral.bluetoothMode == BLUETOOTH_OFF ? -1 : 0),
#else
  #define BLUETOOTH_ROWS (uint8_t)0,
#endif

#define SWITCH_TYPE_MAX(sw)   ((MIXSRC_SF-MIXSRC_FIRST_SWITCH == sw || MIXSRC_SH-MIXSRC_FIRST_SWITCH == sw) ? SWITCH_2POS : SWITCH_3POS)


void menuRadioHardware(event_t event)
{
  MENU(STR_HARDWARE, menuTabGeneral, MENU_RADIO_HARDWARE, ITEM_RADIO_HARDWARE_MAX, { LABEL(Sticks), 0, 0, 0, 0, LABEL(Pots), POTS_ROWS, LABEL(Switches), SWITCHES_ROWS, BLUETOOTH_ROWS 0 });

  uint8_t sub = menuVerticalPosition;

  for (int i=0; i<NUM_BODY_LINES; ++i) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    int k = i + menuVerticalOffset;
    for (int j=0; j<=k; j++) {
      if (mstate_tab[j] == HIDDEN_ROW)
        k++;
    }
    LcdFlags attr = (sub == k ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
    switch (k) {
      case ITEM_RADIO_HARDWARE_LABEL_STICKS:
        lcdDrawTextAlignedLeft(y, STR_STICKS);
        break;
      case ITEM_RADIO_HARDWARE_STICK1:
      case ITEM_RADIO_HARDWARE_STICK2:
      case ITEM_RADIO_HARDWARE_STICK3:
      case ITEM_RADIO_HARDWARE_STICK4:
        editStickHardwareSettings(HW_SETTINGS_COLUMN, y, k - ITEM_RADIO_HARDWARE_STICK1, event, attr);
        break;

      case ITEM_RADIO_HARDWARE_LS:
      case ITEM_RADIO_HARDWARE_RS:
#if defined(PCBX9E)
      case ITEM_RADIO_HARDWARE_LS2:
      case ITEM_RADIO_HARDWARE_RS2:
#endif
      {
        int idx = k - ITEM_RADIO_HARDWARE_LS;
        lcdDrawTextAtIndex(INDENT_WIDTH, y, STR_VSRCRAW, NUM_STICKS+NUM_POTS+idx+1, menuHorizontalPosition < 0 ? attr : 0);
        if (ZEXIST(g_eeGeneral.anaNames[NUM_STICKS+NUM_POTS+idx]) || (attr && menuHorizontalPosition == 0 && s_editMode > 0))
          editName(HW_SETTINGS_COLUMN, y, g_eeGeneral.anaNames[NUM_STICKS+NUM_POTS+idx], LEN_ANA_NAME, event, attr && menuHorizontalPosition == 0);
        else
          lcdDrawMMM(HW_SETTINGS_COLUMN, y, menuHorizontalPosition == 0 ? attr : 0);
        uint8_t mask = (0x01 << idx);
        uint8_t potType = (g_eeGeneral.slidersConfig & mask) >> idx;
        potType = editChoice(HW_SETTINGS_COLUMN+5*FW, y, "", STR_SLIDERTYPES, potType, SLIDER_NONE, SLIDER_WITH_DETENT, menuHorizontalPosition == 1 ? attr : 0, event);
        g_eeGeneral.slidersConfig &= ~mask;
        g_eeGeneral.slidersConfig |= (potType << idx);
        break;
      }

      case ITEM_RADIO_HARDWARE_LABEL_POTS:
        lcdDrawTextAlignedLeft(y, STR_POTS);
        break;
      case ITEM_RADIO_HARDWARE_POT1:
      case ITEM_RADIO_HARDWARE_POT2:
#if defined(PCBX9DP) || defined(PCBX9E)
      case ITEM_RADIO_HARDWARE_POT3:
#endif
#if defined(PCBX9E)
      case ITEM_RADIO_HARDWARE_POT4:
#endif
      {
        int idx = k - ITEM_RADIO_HARDWARE_POT1;
        uint8_t shift = (2*idx);
        uint8_t mask = (0x03 << shift);
        lcdDrawTextAtIndex(INDENT_WIDTH, y, STR_VSRCRAW, NUM_STICKS+idx+1, menuHorizontalPosition < 0 ? attr : 0);
        if (ZEXIST(g_eeGeneral.anaNames[NUM_STICKS+idx]) || (attr && menuHorizontalPosition == 0 && s_editMode > 0))
          editName(HW_SETTINGS_COLUMN, y, g_eeGeneral.anaNames[NUM_STICKS+idx], LEN_ANA_NAME, event, attr && menuHorizontalPosition == 0);
        else
          lcdDrawMMM(HW_SETTINGS_COLUMN, y, menuHorizontalPosition == 0 ? attr : 0);
        uint8_t potType = (g_eeGeneral.potsConfig & mask) >> shift;
        potType = editChoice(HW_SETTINGS_COLUMN+5*FW, y, "", STR_POTTYPES, potType, POT_NONE, POT_WITHOUT_DETENT, menuHorizontalPosition == 1 ? attr : 0, event);
        g_eeGeneral.potsConfig &= ~mask;
        g_eeGeneral.potsConfig |= (potType << shift);
        break;
      }

      case ITEM_RADIO_HARDWARE_LABEL_SWITCHES:
        lcdDrawTextAlignedLeft(y, STR_SWITCHES);
        break;
      case ITEM_RADIO_HARDWARE_SA:
      case ITEM_RADIO_HARDWARE_SB:
      case ITEM_RADIO_HARDWARE_SC:
      case ITEM_RADIO_HARDWARE_SD:
      case ITEM_RADIO_HARDWARE_SE:
      case ITEM_RADIO_HARDWARE_SF:
      case ITEM_RADIO_HARDWARE_SG:
      case ITEM_RADIO_HARDWARE_SH:
#if defined(PCBX9E)
      case ITEM_RADIO_HARDWARE_SI:
      case ITEM_RADIO_HARDWARE_SJ:
      case ITEM_RADIO_HARDWARE_SK:
      case ITEM_RADIO_HARDWARE_SL:
      case ITEM_RADIO_HARDWARE_SM:
      case ITEM_RADIO_HARDWARE_SN:
      case ITEM_RADIO_HARDWARE_SO:
      case ITEM_RADIO_HARDWARE_SP:
      case ITEM_RADIO_HARDWARE_SQ:
      case ITEM_RADIO_HARDWARE_SR:
#endif
      {
        int index = k-ITEM_RADIO_HARDWARE_SA;
        int config = SWITCH_CONFIG(index);
        lcdDrawTextAtIndex(INDENT_WIDTH, y, STR_VSRCRAW, MIXSRC_FIRST_SWITCH-MIXSRC_Rud+index+1, menuHorizontalPosition < 0 ? attr : 0);
        if (ZEXIST(g_eeGeneral.switchNames[index]) || (attr && menuHorizontalPosition == 0 && s_editMode > 0))
          editName(HW_SETTINGS_COLUMN, y, g_eeGeneral.switchNames[index], LEN_SWITCH_NAME, event, menuHorizontalPosition == 0 ? attr : 0);
        else
          lcdDrawMMM(HW_SETTINGS_COLUMN, y, menuHorizontalPosition == 0 ? attr : 0);
        config = editChoice(HW_SETTINGS_COLUMN+5*FW, y, "", STR_SWTYPES, config, SWITCH_NONE, SWITCH_TYPE_MAX(index), menuHorizontalPosition == 1 ? attr : 0, event);
        if (attr && checkIncDec_Ret) {
          swconfig_t mask = (swconfig_t)0x03 << (2*index);
          g_eeGeneral.switchConfig = (g_eeGeneral.switchConfig & ~mask) | ((swconfig_t(config) & 0x03) << (2*index));
        }
        break;
      }

#if defined(BLUETOOTH)
        case ITEM_RADIO_HARDWARE_BLUETOOTH_MODE:
          lcdDrawText(INDENT_WIDTH, y, STR_BLUETOOTH);
          lcdDrawTextAtIndex(HW_SETTINGS_COLUMN, y, STR_BLUETOOTH_MODES, g_eeGeneral.bluetoothMode, attr);
          if (attr) {
#if defined(USEHORUSBT)
            g_eeGeneral.bluetoothMode = checkIncDecGen(event, g_eeGeneral.bluetoothMode, BLUETOOTH_OFF, BLUETOOTH_TRAINER);
#else
            g_eeGeneral.bluetoothMode = checkIncDecGen(event, g_eeGeneral.bluetoothMode, BLUETOOTH_OFF, BLUETOOTH_TELEMETRY);
#endif
          }
          break;
#if defined(USEHORUSBT)
        case ITEM_RADIO_HARDWARE_BLUETOOTH_NAME:
          lcdDrawText(INDENT_WIDTH, y, STR_NAME);
          editName(HW_SETTINGS_COLUMN, y, g_eeGeneral.bluetoothName, LEN_BLUETOOTH_NAME, event, attr);
          break;
#endif
#endif

      case ITEM_RADIO_HARDWARE_UART3_MODE:
        g_eeGeneral.serial2Mode = editChoice(HW_SETTINGS_COLUMN, y, STR_UART3MODE, STR_UART3MODES, g_eeGeneral.serial2Mode, 0, UART_MODE_MAX, attr, event);
        if (attr && checkIncDec_Ret) {
          serial2Init(g_eeGeneral.serial2Mode, modelTelemetryProtocol());
        }
        break;

      case ITEM_RADIO_HARDWARE_JITTER_FILTER:
      {
        uint8_t b = 1-g_eeGeneral.jitterFilter;
        g_eeGeneral.jitterFilter = 1 - editCheckBox(b, HW_SETTINGS_COLUMN, y, STR_JITTER_FILTER, attr, event);
        break;
      }
    }
  }
}
