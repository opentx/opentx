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

enum MenuRadioHardwareItems {
  ITEM_RADIO_HARDWARE_OPTREX_DISPLAY,
  ITEM_RADIO_HARDWARE_STICKS_GAINS_LABELS,
  ITEM_RADIO_HARDWARE_STICK_LV_GAIN,
  ITEM_RADIO_HARDWARE_STICK_LH_GAIN,
  ITEM_RADIO_HARDWARE_STICK_RV_GAIN,
  ITEM_RADIO_HARDWARE_STICK_RH_GAIN,
  IF_ROTARY_ENCODERS(ITEM_RADIO_HARDWARE_ROTARY_ENCODER)
  CASE_BLUETOOTH(ITEM_RADIO_HARDWARE_BT_BAUDRATE)
  ITEM_RADIO_HARDWARE_MAX
};

#define GENERAL_HW_PARAM_OFS (2+(15*FW))
void menuRadioHardware(event_t event)
{
#if defined(PCBX7)
#else
  MENU(STR_HARDWARE, menuTabGeneral, MENU_RADIO_HARDWARE, ITEM_RADIO_HARDWARE_MAX+1, {0, 0, (uint8_t)-1, 0, 0, 0, IF_ROTARY_ENCODERS(0) CASE_BLUETOOTH(0)});

  uint8_t sub = menuVerticalPosition - 1;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);

    switch(k) {
      case ITEM_RADIO_HARDWARE_OPTREX_DISPLAY:
        g_eeGeneral.optrexDisplay = editChoice(GENERAL_HW_PARAM_OFS, y, STR_LCD, STR_VLCD, g_eeGeneral.optrexDisplay, 0, 1, attr, event);
        break;

      case ITEM_RADIO_HARDWARE_STICKS_GAINS_LABELS:
        lcdDrawTextAlignedLeft(y, PSTR("Sticks"));
        break;

      case ITEM_RADIO_HARDWARE_STICK_LV_GAIN:
      case ITEM_RADIO_HARDWARE_STICK_LH_GAIN:
      case ITEM_RADIO_HARDWARE_STICK_RV_GAIN:
      case ITEM_RADIO_HARDWARE_STICK_RH_GAIN:
      {
        lcdDrawTextAtIndex(INDENT_WIDTH, y, PSTR("\002LVLHRVRH"), k-ITEM_RADIO_HARDWARE_STICK_LV_GAIN, 0);
        lcdDrawText(INDENT_WIDTH+3*FW, y, PSTR("Gain"));
        uint8_t mask = (1<<(k-ITEM_RADIO_HARDWARE_STICK_LV_GAIN));
        uint8_t val = (g_eeGeneral.sticksGain & mask ? 1 : 0);
        lcdDrawChar(GENERAL_HW_PARAM_OFS, y, val ? '2' : '1', attr);
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
      case ITEM_RADIO_HARDWARE_ROTARY_ENCODER:
        g_eeGeneral.rotarySteps = editChoice(GENERAL_HW_PARAM_OFS, y, PSTR("Rotary Encoder"), PSTR("\0062steps4steps"), g_eeGeneral.rotarySteps, 0, 1, attr, event);
        break;
#endif

#if defined(BLUETOOTH)
      case ITEM_RADIO_HARDWARE_BT_BAUDRATE:
        g_eeGeneral.btBaudrate = editChoice(GENERAL_HW_PARAM_OFS, y, STR_BAUDRATE, PSTR("\005115k 9600 19200"), g_eeGeneral.btBaudrate, 0, 2, attr, event);
        if (attr && checkIncDec_Ret) {
          btInit();
        }
        break;
#endif
    }
  }
#endif
}
