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
  ITEM_SETUP_HW_SI,
  ITEM_SETUP_HW_SJ,
  ITEM_SETUP_HW_SK,
  ITEM_SETUP_HW_SL,
  ITEM_SETUP_HW_SM,
  ITEM_SETUP_HW_SN,
  CASE_REV9E(ITEM_SETUP_HW_SO)
  CASE_REV9E(ITEM_SETUP_HW_SP)
  CASE_REV9E(ITEM_SETUP_HW_SQ)
  CASE_REV9E(ITEM_SETUP_HW_SR)
  ITEM_SETUP_HW_UART3_MODE,
  ITEM_SETUP_HW_MAX
};

#define HW_SETTINGS_COLUMN 15*FW

#if defined(REVPLUS)
  #define POTS_ROWS NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, 0, 0
#else
  #define POTS_ROWS NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, 0, 0
#endif

#if defined(REV9E)
  #define SWITCHES_ROWS  NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1
#else
  #define SWITCH_ROWS(x) uint8_t(IS_2x2POS(x) ? 0 : HIDDEN_ROW)
  #define SWITCHES_ROWS  NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, SWITCH_ROWS(0), SWITCH_ROWS(1), SWITCH_ROWS(2), SWITCH_ROWS(3), SWITCH_ROWS(4), SWITCH_ROWS(6)
#endif

void menuGeneralHardware(uint8_t event)
{
  MENU(STR_HARDWARE, menuTabGeneral, e_Hardware, ITEM_SETUP_HW_MAX+1, {0, LABEL(Sticks), 0, 0, 0, 0, LABEL(Pots), POTS_ROWS, LABEL(Switches), SWITCHES_ROWS, 0});

  uint8_t sub = m_posVert - 1;

  for (uint8_t i=0; i<LCD_LINES-1; ++i) {
    coord_t y = MENU_TITLE_HEIGHT + 1 + i*FH;
    uint8_t k = i + s_pgOfs;
    for (int j=0; j<=k; j++) {
      if (mstate_tab[j+1] == HIDDEN_ROW)
        k++;
    }
    uint8_t attr = (sub == k ? ((s_editMode>0) ? BLINK|INVERS : INVERS) : 0);
    switch (k) {
      case ITEM_SETUP_HW_LABEL_STICKS:
        lcd_putsLeft(y, "Sticks");
        break;
      case ITEM_SETUP_HW_STICK1:
      case ITEM_SETUP_HW_STICK2:
      case ITEM_SETUP_HW_STICK3:
      case ITEM_SETUP_HW_STICK4:
      case ITEM_SETUP_HW_LS:
      case ITEM_SETUP_HW_RS:
#if defined(REV9E)
      case ITEM_SETUP_HW_LS2:
      case ITEM_SETUP_HW_RS2:
#endif
      {
        int idx = (k<=ITEM_SETUP_HW_STICK4 ? k-ITEM_SETUP_HW_STICK1 : k-ITEM_SETUP_HW_LS+7);
        lcd_putsiAtt(INDENT_WIDTH, y, STR_VSRCRAW, idx+1, 0);
        if (ZEXIST(g_eeGeneral.anaNames[idx]) || attr)
          editName(HW_SETTINGS_COLUMN, y, g_eeGeneral.anaNames[idx], LEN_ANA_NAME, event, attr);
        else
          lcd_putsiAtt(HW_SETTINGS_COLUMN, y, STR_MMMINV, 0, 0);
        break;
      }
      case ITEM_SETUP_HW_LABEL_POTS:
        lcd_putsLeft(y, "Pots");
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

        uint8_t potType = (g_eeGeneral.potsType & mask) >> shift;
        if (potType == POT_TYPE_NONE && k <= ITEM_SETUP_HW_POT2)
          potType = POT_TYPE_DETENT;
        potType = selectMenuItem(HW_SETTINGS_COLUMN+5*FW, y, "", STR_POTTYPES, potType, 0, POT_TYPE_MAX, m_posHorz == 1 ? attr : 0, event);
        if (potType == POT_TYPE_DETENT && k <= ITEM_SETUP_HW_POT2)
          potType = POT_TYPE_NONE;
        g_eeGeneral.potsType &= ~mask;
        g_eeGeneral.potsType |= (potType << shift);
        break;
      }
      case ITEM_SETUP_HW_LABEL_SWITCHES:
        lcd_putsLeft(y, "Switches");
        break;
      case ITEM_SETUP_HW_SA:
      case ITEM_SETUP_HW_SB:
      case ITEM_SETUP_HW_SC:
      case ITEM_SETUP_HW_SD:
      case ITEM_SETUP_HW_SE:
      case ITEM_SETUP_HW_SF:
      case ITEM_SETUP_HW_SG:
      case ITEM_SETUP_HW_SH:
      case ITEM_SETUP_HW_SI:
      case ITEM_SETUP_HW_SJ:
      case ITEM_SETUP_HW_SK:
      case ITEM_SETUP_HW_SL:
      case ITEM_SETUP_HW_SM:
      case ITEM_SETUP_HW_SN:
#if defined(REV9E)
      case ITEM_SETUP_HW_SO:
      case ITEM_SETUP_HW_SP:
      case ITEM_SETUP_HW_SQ:
      case ITEM_SETUP_HW_SR:
#endif
      {
        int index = k-ITEM_SETUP_HW_SA;
        char label[] = INDENT "S*";
        label[2] = 'A' + index;
        int config = SWITCH_CONFIG(index);
        lcd_putsAtt(0, y, label, m_posHorz < 0 ? attr : 0);
        if (ZEXIST(g_eeGeneral.switchNames[index]) || (attr && m_posHorz == 0))
          editName(HW_SETTINGS_COLUMN, y, g_eeGeneral.switchNames[index], LEN_SWITCH_NAME, event, m_posHorz == 0 ? attr : 0);
        else
          lcd_putsiAtt(HW_SETTINGS_COLUMN, y, STR_MMMINV, 0, 0);
#if defined(REV9E)
        config = selectMenuItem(HW_SETTINGS_COLUMN+5*FW, y, "", "\007None\0  DefaultToggle\0""2POS\0  3POS\0", config, SWITCH_NONE, SWITCH_3POS, m_posHorz == 1 ? attr : 0, event);
        if (attr && checkIncDec_Ret) {
          uint32_t mask = 0x0f << (4*index);
          TRACE("avant %x", g_eeGeneral.switchConfig);
          g_eeGeneral.switchConfig = (g_eeGeneral.switchConfig & ~mask) | ((uint32_t(config)&0xf) << (4*index));
          TRACE("apres %x", g_eeGeneral.switchConfig);
        }
#else
        if (k <= ITEM_SETUP_HW_SH) {
          config = selectMenuItem(HW_SETTINGS_COLUMN+5*FW, y, "", "\007DefaultToggle\0""2POS\0  3POS\0  2x2POS\0", config, SWITCH_NONE, SWITCH_2x2POS, m_posHorz == 1 ? attr : 0, event);
          if (attr && checkIncDec_Ret) {
            uint32_t mask = 0x0f << (4*index);
            g_eeGeneral.switchConfig = (g_eeGeneral.switchConfig & ~mask) | (((uint32_t)(config)&0xf) << (4*index));
          }
        }
#endif
        break;
      }
      case ITEM_SETUP_HW_UART3_MODE:
        g_eeGeneral.uart3Mode = selectMenuItem(HW_SETTINGS_COLUMN, y, STR_UART3MODE, STR_UART3MODES, g_eeGeneral.uart3Mode, 0, UART_MODE_MAX, attr, event);
        if (attr && checkIncDec_Ret) {
          uart3Init(g_eeGeneral.uart3Mode, MODEL_TELEMETRY_PROTOCOL());
        }
        break;
    }
  }
}
