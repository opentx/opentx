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
  ITEM_SETUP_HW_OPTREX_DISPLAY,
  ITEM_SETUP_HW_STICKS_GAINS_LABELS,
  ITEM_SETUP_HW_STICK_LV_GAIN,
  ITEM_SETUP_HW_STICK_LH_GAIN,
  ITEM_SETUP_HW_STICK_RV_GAIN,
  ITEM_SETUP_HW_STICK_RH_GAIN,
  IF_ROTARY_ENCODERS(ITEM_SETUP_HW_ROTARY_ENCODER)
  CASE_BLUETOOTH(ITEM_SETUP_HW_BT_BAUDRATE)
  ITEM_SETUP_HW_MAX
};

#define GENERAL_HW_PARAM_OFS (2+(15*FW))
void menuGeneralHardware(uint8_t event)
{
  MENU(STR_HARDWARE, menuTabGeneral, e_Hardware, ITEM_SETUP_HW_MAX+1, {0, 0, (uint8_t)-1, 0, 0, 0, IF_ROTARY_ENCODERS(0) CASE_BLUETOOTH(0)});

  uint8_t sub = m_posVert - 1;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+s_pgOfs;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);

    switch(k) {
      case ITEM_SETUP_HW_OPTREX_DISPLAY:
        g_eeGeneral.optrexDisplay = selectMenuItem(GENERAL_HW_PARAM_OFS, y, STR_LCD, STR_VLCD, g_eeGeneral.optrexDisplay, 0, 1, attr, event);
        break;

      case ITEM_SETUP_HW_STICKS_GAINS_LABELS:
        lcd_putsLeft(y, PSTR("Sticks"));
        break;

      case ITEM_SETUP_HW_STICK_LV_GAIN:
      case ITEM_SETUP_HW_STICK_LH_GAIN:
      case ITEM_SETUP_HW_STICK_RV_GAIN:
      case ITEM_SETUP_HW_STICK_RH_GAIN:
      {
        lcd_putsiAtt(INDENT_WIDTH, y, PSTR("\002LVLHRVRH"), k-ITEM_SETUP_HW_STICK_LV_GAIN, 0);
        lcd_puts(INDENT_WIDTH+3*FW, y, PSTR("Gain"));
        uint8_t mask = (1<<(k-ITEM_SETUP_HW_STICK_LV_GAIN));
        uint8_t val = (g_eeGeneral.sticksGain & mask ? 1 : 0);
        lcd_putcAtt(GENERAL_HW_PARAM_OFS, y, val ? '2' : '1', attr);
        if (attr) {
          CHECK_INCDEC_GENVAR(event, val, 0, 1);
          if (checkIncDec_Ret) {
            g_eeGeneral.sticksGain ^= mask;
            setSticksGain(g_eeGeneral.sticksGain);
          }
        }
        break;
      }

#if defined(ROTARY_ENCODERS)
      case ITEM_SETUP_HW_ROTARY_ENCODER:
        g_eeGeneral.rotarySteps = selectMenuItem(GENERAL_HW_PARAM_OFS, y, PSTR("Rotary Encoder"), PSTR("\0062steps4steps"), g_eeGeneral.rotarySteps, 0, 1, attr, event);
        break;
#endif

#if defined(BLUETOOTH)
      case ITEM_SETUP_HW_BT_BAUDRATE:
        g_eeGeneral.btBaudrate = selectMenuItem(GENERAL_HW_PARAM_OFS, y, STR_BAUDRATE, PSTR("\005115k 9600 19200"), g_eeGeneral.btBaudrate, 0, 2, attr, event);
        if (attr && checkIncDec_Ret) {
          btInit();
        }
        break;
#endif

    }
  }
}
