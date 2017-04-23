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
#include "stamp.h"

#define RADIO_SETUP_2ND_COLUMN  (LCD_W-11*FW)

enum MenuRadioSetupItems {
  ITEM_SETUP_SWITCH_SA,
  ITEM_SETUP_SWITCH_SB,
  ITEM_SETUP_SWITCH_SC,
  ITEM_SETUP_BATT_RANGE,
  ITEM_SETUP_BATTERY_WARNING,
  ITEM_SETUP_CONTRAST,
  ITEM_SETUP_INACTIVITY_ALARM,
  ITEM_SETUP_BACKLIGHT_MODE,
  ITEM_SETUP_BIND,
  ITEM_SETUP_MAX
};

void menuRadioSetup(event_t event)
{
  MENU(STR_MENURADIOSETUP, menuTabModel, MENU_RADIO_SETUP, HEADER_LINE+ITEM_SETUP_MAX, {
    HEADER_LINE_COLUMNS
    0,
    0,
    0,
    1,
    0,
    0,
    0,
    0,
    2
  });

  lcdDrawText(lcdLastRightPos + FW, 0, VERSION);

  uint8_t sub = menuVerticalPosition - HEADER_LINE;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i + menuVerticalOffset;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);

    switch (k) {
      case ITEM_SETUP_SWITCH_SA:
      case ITEM_SETUP_SWITCH_SB:
      case ITEM_SETUP_SWITCH_SC:
      {
        int index = k - ITEM_SETUP_SWITCH_SA;
        int config = SWITCH_CONFIG(index);
        lcdDrawTextAtIndex(0, y, STR_VSRCRAW, MIXSRC_FIRST_SWITCH - MIXSRC_Rud + index + 1,
                           menuHorizontalPosition < 0 ? attr : 0);
        config = editChoice(RADIO_SETUP_2ND_COLUMN, y, "", STR_SWTYPES, config, SWITCH_NONE, SWITCH_3POS, attr, event);
        if (attr && checkIncDec_Ret) {
          swconfig_t mask = (swconfig_t) 0x03 << (2 * index);
          g_eeGeneral.switchConfig = (g_eeGeneral.switchConfig & ~mask) | ((swconfig_t(config) & 0x03) << (2 * index));
        }
        break;
      }

      case ITEM_SETUP_BATT_RANGE:
        lcdDrawTextAlignedLeft(y, STR_BATTERY_RANGE);
        putsVolts(RADIO_SETUP_2ND_COLUMN, y, 90 + g_eeGeneral.vBatMin,
                  (menuHorizontalPosition == 0 ? attr : 0) | LEFT | NO_UNIT);
        lcdDrawChar(lcdLastRightPos, y, '-');
        putsVolts(lcdLastRightPos + FW, y, 120 + g_eeGeneral.vBatMax,
                  (menuHorizontalPosition > 0 ? attr : 0) | LEFT | NO_UNIT);
        if (attr && s_editMode > 0) {
          if (menuHorizontalPosition == 0)
            CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatMin, -50, g_eeGeneral.vBatMax + 29); // min=4.0V
          else
            CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatMax, g_eeGeneral.vBatMin - 29, +40); // max=16.0V
        }
        break;

      case ITEM_SETUP_BATTERY_WARNING:
        lcdDrawTextAlignedLeft(y, NO_INDENT(STR_BATTERYWARNING));
        putsVolts(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.vBatWarn, attr | LEFT);
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.vBatWarn, 40, 120); //4-12V
        break;

      case ITEM_SETUP_CONTRAST:
        lcdDrawTextAlignedLeft(y, STR_CONTRAST);
        lcdDrawNumber(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.contrast, attr | LEFT);
        if (attr) {
          CHECK_INCDEC_GENVAR(event, g_eeGeneral.contrast, LCD_CONTRAST_MIN, LCD_CONTRAST_MAX);
          lcdSetContrast();
        }
        break;

      case ITEM_SETUP_INACTIVITY_ALARM:
        lcdDrawTextAlignedLeft(y, NO_INDENT(STR_INACTIVITYALARM));
        lcdDrawNumber(RADIO_SETUP_2ND_COLUMN, y, g_eeGeneral.inactivityTimer, attr | LEFT);
        lcdDrawChar(lcdLastRightPos, y, 'm');
        if (attr)
          g_eeGeneral.inactivityTimer = checkIncDec(event, g_eeGeneral.inactivityTimer, 0, 250,
                                                    EE_GENERAL); //0..250minutes
        break;

      case ITEM_SETUP_BACKLIGHT_MODE:
        g_eeGeneral.backlightMode = editChoice(RADIO_SETUP_2ND_COLUMN, y, NO_INDENT(STR_MODE), STR_VBLMODE,
                                               g_eeGeneral.backlightMode, e_backlight_mode_off, e_backlight_mode_on,
                                               attr, event);
        break;

      case ITEM_SETUP_BIND: {
        coord_t xOffsetBind = 2*FW+1;
        lcdDrawTextAlignedLeft(y, NO_INDENT(STR_RECEIVER_NUM));
        if (xOffsetBind)
          lcdDrawNumber(RADIO_SETUP_2ND_COLUMN, y, g_model.header.modelId[0],
                        (menuHorizontalPosition == 0 ? attr : 0) | LEADING0 | LEFT, 2);
        if (attr && menuHorizontalPosition == 0) {
          if (s_editMode > 0) {
            CHECK_INCDEC_MODELVAR_ZERO(event, g_model.header.modelId[0], MAX_RX_NUM(0));
            if (checkIncDec_Ret) {
              modelHeaders[g_eeGeneral.currModel].modelId[0] = g_model.header.modelId[0];
            }
          }
        }
        lcdDrawText(RADIO_SETUP_2ND_COLUMN + xOffsetBind, y, STR_MODULE_BIND, menuHorizontalPosition == 1 ? attr : 0);
        lcdDrawText(RADIO_SETUP_2ND_COLUMN + 4*FW+3 + xOffsetBind, y, STR_MODULE_RANGE,
                    menuHorizontalPosition == 2 ? attr : 0);
        uint8_t newFlag = 0;
        if (attr && s_editMode > 0) {
          if (menuHorizontalPosition == 1)
            newFlag = MODULE_BIND;
          else if (menuHorizontalPosition == 2) {
            newFlag = MODULE_RANGECHECK;
          }
        }
        moduleFlag[0] = newFlag;
      }
    }
  }
}
