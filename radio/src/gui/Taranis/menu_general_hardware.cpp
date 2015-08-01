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

#include "../../opentx.h"

enum menuGeneralHwItems {
  ITEM_SETUP_HW_LABEL_STICKS,
  ITEM_SETUP_HW_STICK1,
  ITEM_SETUP_HW_STICK2,
  ITEM_SETUP_HW_STICK3,
  ITEM_SETUP_HW_STICK4,
  ITEM_SETUP_HW_LABEL_POTS,
  ITEM_SETUP_HW_POT1,
  ITEM_SETUP_HW_POT2,
  CASE_REVPLUS(ITEM_SETUP_HW_POT3)
  CASE_REV9E(ITEM_SETUP_HW_POT4)
  ITEM_SETUP_HW_LS,
  ITEM_SETUP_HW_RS,
  CASE_REV9E(ITEM_SETUP_HW_LS2)
  CASE_REV9E(ITEM_SETUP_HW_RS2)
  ITEM_SETUP_HW_LABEL_SWITCHES,
  ITEM_SETUP_HW_SA,
  ITEM_SETUP_HW_SB,
  ITEM_SETUP_HW_SC,
  ITEM_SETUP_HW_SD,
  ITEM_SETUP_HW_SE,
  ITEM_SETUP_HW_SF,
  ITEM_SETUP_HW_SG,
  ITEM_SETUP_HW_SH,
  CASE_REV9E(ITEM_SETUP_HW_SI)
  CASE_REV9E(ITEM_SETUP_HW_SJ)
  CASE_REV9E(ITEM_SETUP_HW_SK)
  CASE_REV9E(ITEM_SETUP_HW_SL)
  CASE_REV9E(ITEM_SETUP_HW_SM)
  CASE_REV9E(ITEM_SETUP_HW_SN)
  CASE_REV9E(ITEM_SETUP_HW_SO)
  CASE_REV9E(ITEM_SETUP_HW_SP)
  CASE_REV9E(ITEM_SETUP_HW_SQ)
  CASE_REV9E(ITEM_SETUP_HW_SR)
  CASE_REV9E(ITEM_SETUP_HW_BLUETOOTH)
  ITEM_SETUP_HW_UART3_MODE,
  ITEM_SETUP_HW_MAX
};

#define HW_SETTINGS_COLUMN 15*FW

#if defined(REV9E)
  #define POTS_ROWS      NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, 0, 0, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1
#elif defined(REVPLUS)
  #define POTS_ROWS      NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, 0, 0
#else
  #define POTS_ROWS      NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, 0, 0
#endif

#if defined(REV9E)
  #define SWITCHES_ROWS  NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1
  #define BLUETOOTH_ROWS 1,
#else
  #define SWITCHES_ROWS  NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1
  #define BLUETOOTH_ROWS
#endif

void menuGeneralHardware(uint8_t event)
{
  MENU(STR_HARDWARE, menuTabGeneral, e_Hardware, ITEM_SETUP_HW_MAX, { LABEL(Sticks), 0, 0, 0, 0, LABEL(Pots), POTS_ROWS, LABEL(Switches), SWITCHES_ROWS, BLUETOOTH_ROWS 0 });

  uint8_t sub = m_posVert;

  for (int i=0; i<NUM_BODY_LINES; ++i) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    int k = i + s_pgOfs;
    for (int j=0; j<=k; j++) {
      if (mstate_tab[j] == HIDDEN_ROW)
        k++;
    }
    LcdFlags attr = (sub == k ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
    switch (k) {
      case ITEM_SETUP_HW_LABEL_STICKS:
        lcd_putsLeft(y, STR_STICKS);
        break;
      case ITEM_SETUP_HW_STICK1:
      case ITEM_SETUP_HW_STICK2:
      case ITEM_SETUP_HW_STICK3:
      case ITEM_SETUP_HW_STICK4:
      case ITEM_SETUP_HW_LS:
      case ITEM_SETUP_HW_RS:
      {
        int idx = (k<=ITEM_SETUP_HW_STICK4 ? k-ITEM_SETUP_HW_STICK1 : k-ITEM_SETUP_HW_LS+MIXSRC_SLIDER1-MIXSRC_Rud);
        lcd_putsiAtt(INDENT_WIDTH, y, STR_VSRCRAW, idx+1, 0);
        if (ZEXIST(g_eeGeneral.anaNames[idx]) || attr)
          editName(HW_SETTINGS_COLUMN, y, g_eeGeneral.anaNames[idx], LEN_ANA_NAME, event, attr);
        else
          lcd_putsiAtt(HW_SETTINGS_COLUMN, y, STR_MMMINV, 0, 0);
        break;
      }
#if defined(REV9E)
      case ITEM_SETUP_HW_LS2:
      case ITEM_SETUP_HW_RS2:
      {
        int idx = k - ITEM_SETUP_HW_LS2;
        uint8_t mask = (0x01 << idx);
        lcd_putsiAtt(INDENT_WIDTH, y, STR_VSRCRAW, NUM_STICKS+NUM_XPOTS+2+idx+1, m_posHorz < 0 ? attr : 0);
        if (ZEXIST(g_eeGeneral.anaNames[NUM_STICKS+NUM_XPOTS+2+idx]) || (attr && m_posHorz == 0))
          editName(HW_SETTINGS_COLUMN, y, g_eeGeneral.anaNames[NUM_STICKS+NUM_XPOTS+2+idx], LEN_ANA_NAME, event, attr && m_posHorz == 0);
        else
          lcd_putsiAtt(HW_SETTINGS_COLUMN, y, STR_MMMINV, 0, 0);
        uint8_t potType = (g_eeGeneral.slidersConfig & mask) >> idx;
        potType = selectMenuItem(HW_SETTINGS_COLUMN+5*FW, y, "", STR_SLIDERTYPES, potType, SLIDER_NONE, SLIDER_WITH_DETENT, m_posHorz == 1 ? attr : 0, event);
        g_eeGeneral.slidersConfig &= ~mask;
        g_eeGeneral.slidersConfig |= (potType << idx);
        break;
      }
#endif
      case ITEM_SETUP_HW_LABEL_POTS:
        lcd_putsLeft(y, STR_POTS);
        break;
      case ITEM_SETUP_HW_POT1:
      case ITEM_SETUP_HW_POT2:
#if defined(REVPLUS)
      case ITEM_SETUP_HW_POT3:
#endif
#if defined(REV9E)
      case ITEM_SETUP_HW_POT4:
#endif
      {
        int idx = k - ITEM_SETUP_HW_POT1;
        uint8_t shift = (2*idx);
        uint8_t mask = (0x03 << shift);
        lcd_putsiAtt(INDENT_WIDTH, y, STR_VSRCRAW, NUM_STICKS+idx+1, m_posHorz < 0 ? attr : 0);
        if (ZEXIST(g_eeGeneral.anaNames[NUM_STICKS+idx]) || (attr && m_posHorz == 0))
          editName(HW_SETTINGS_COLUMN, y, g_eeGeneral.anaNames[NUM_STICKS+idx], LEN_ANA_NAME, event, attr && m_posHorz == 0);
        else
          lcd_putsiAtt(HW_SETTINGS_COLUMN, y, STR_MMMINV, 0, 0);
        uint8_t potType = (g_eeGeneral.potsConfig & mask) >> shift;
        potType = selectMenuItem(HW_SETTINGS_COLUMN+5*FW, y, "", STR_POTTYPES, potType, POT_NONE, POT_WITHOUT_DETENT, m_posHorz == 1 ? attr : 0, event);
        g_eeGeneral.potsConfig &= ~mask;
        g_eeGeneral.potsConfig |= (potType << shift);
        break;
      }
      case ITEM_SETUP_HW_LABEL_SWITCHES:
        lcd_putsLeft(y, STR_SWITCHES);
        break;
      case ITEM_SETUP_HW_SA:
      case ITEM_SETUP_HW_SB:
      case ITEM_SETUP_HW_SC:
      case ITEM_SETUP_HW_SD:
      case ITEM_SETUP_HW_SE:
      case ITEM_SETUP_HW_SF:
      case ITEM_SETUP_HW_SG:
      case ITEM_SETUP_HW_SH:
#if defined(REV9E)
      case ITEM_SETUP_HW_SI:
      case ITEM_SETUP_HW_SJ:
      case ITEM_SETUP_HW_SK:
      case ITEM_SETUP_HW_SL:
      case ITEM_SETUP_HW_SM:
      case ITEM_SETUP_HW_SN:
      case ITEM_SETUP_HW_SO:
      case ITEM_SETUP_HW_SP:
      case ITEM_SETUP_HW_SQ:
      case ITEM_SETUP_HW_SR:
#endif
      {
        int index = k-ITEM_SETUP_HW_SA;
        int config = SWITCH_CONFIG(index);
        lcd_putsiAtt(INDENT_WIDTH, y, STR_VSRCRAW, MIXSRC_FIRST_SWITCH-MIXSRC_Rud+index+1, m_posHorz < 0 ? attr : 0);
        if (ZEXIST(g_eeGeneral.switchNames[index]) || (attr && m_posHorz == 0))
          editName(HW_SETTINGS_COLUMN, y, g_eeGeneral.switchNames[index], LEN_SWITCH_NAME, event, m_posHorz == 0 ? attr : 0);
        else
          lcd_putsiAtt(HW_SETTINGS_COLUMN, y, STR_MMMINV, 0, 0);
        config = selectMenuItem(HW_SETTINGS_COLUMN+5*FW, y, "", STR_SWTYPES, config, SWITCH_NONE, SWITCH_3POS, m_posHorz == 1 ? attr : 0, event);
        if (attr && checkIncDec_Ret) {
          swconfig_t mask = (swconfig_t)0x03 << (2*index);
          g_eeGeneral.switchConfig = (g_eeGeneral.switchConfig & ~mask) | ((swconfig_t(config) & 0x03) << (2*index));
        }
        break;
      }
#if defined(REV9E)
      case ITEM_SETUP_HW_BLUETOOTH:
        lcd_putsLeft(y, "Bluetooth");
        menu_lcd_onoff(HW_SETTINGS_COLUMN, y, g_eeGeneral.bluetoothEnable, m_posHorz == 0 ? attr : 0);
        if (attr && m_posHorz == 0) {
          g_eeGeneral.bluetoothEnable = checkIncDecGen(event, g_eeGeneral.bluetoothEnable, 0, 1);
        }
        editName(HW_SETTINGS_COLUMN+5*FW, y, g_eeGeneral.bluetoothName, LEN_BLUETOOTH_NAME, event, m_posHorz == 1 ? attr : 0);
        break;
#endif
      case ITEM_SETUP_HW_UART3_MODE:
        g_eeGeneral.uart3Mode = selectMenuItem(HW_SETTINGS_COLUMN, y, STR_UART3MODE, STR_UART3MODES, g_eeGeneral.uart3Mode, 0, UART_MODE_MAX, attr, event);
        if (attr && checkIncDec_Ret) {
          uart3Init(g_eeGeneral.uart3Mode, MODEL_TELEMETRY_PROTOCOL());
        }
        break;
    }
  }
}
