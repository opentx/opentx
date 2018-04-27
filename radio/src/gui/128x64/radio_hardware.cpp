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

#if defined(PCBSKY9X)
#define HW_SETTINGS_COLUMN (2+(15*FW))
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

void menuRadioHardware(event_t event)
{
  MENU(STR_HARDWARE, menuTabGeneral, MENU_RADIO_HARDWARE, ITEM_RADIO_HARDWARE_MAX+1, {0, 0, (uint8_t)-1, 0, 0, 0, IF_ROTARY_ENCODERS(0) CASE_BLUETOOTH(0)});

  uint8_t sub = menuVerticalPosition - 1;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);

    switch(k) {
      case ITEM_RADIO_HARDWARE_OPTREX_DISPLAY:
        g_eeGeneral.optrexDisplay = editChoice(HW_SETTINGS_COLUMN, y, STR_LCD, STR_VLCD, g_eeGeneral.optrexDisplay, 0, 1, attr, event);
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
        lcdDrawChar(HW_SETTINGS_COLUMN, y, val ? '2' : '1', attr);
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
        g_eeGeneral.rotarySteps = editChoice(HW_SETTINGS_COLUMN, y, PSTR("Rotary Encoder"), PSTR("\0062steps4steps"), g_eeGeneral.rotarySteps, 0, 1, attr, event);
        break;
#endif

#if defined(BLUETOOTH)
      case ITEM_RADIO_HARDWARE_BT_BAUDRATE:
        g_eeGeneral.bluetoothBaudrate = editChoice(HW_SETTINGS_COLUMN, y, STR_BAUDRATE, PSTR("\005115k 9600 19200"), g_eeGeneral.bluetoothBaudrate, 0, 2, attr, event);
        if (attr && checkIncDec_Ret) {
          btInit();
        }
        break;
#endif
    }
  }
}
#endif // PCBSKY9X

#if defined(PCBTARANIS)
enum MenuRadioHardwareItems {
  ITEM_RADIO_HARDWARE_LABEL_STICKS,
  ITEM_RADIO_HARDWARE_STICK1,
  ITEM_RADIO_HARDWARE_STICK2,
  ITEM_RADIO_HARDWARE_STICK3,
  ITEM_RADIO_HARDWARE_STICK4,
  ITEM_RADIO_HARDWARE_LABEL_POTS,
  ITEM_RADIO_HARDWARE_POT1,
  ITEM_RADIO_HARDWARE_POT2,
  ITEM_RADIO_HARDWARE_LABEL_SWITCHES,
  ITEM_RADIO_HARDWARE_SA,
  ITEM_RADIO_HARDWARE_SB,
  ITEM_RADIO_HARDWARE_SC,
  ITEM_RADIO_HARDWARE_SD,
#if NUM_SWITCHES > 4
  ITEM_RADIO_HARDWARE_SF,
  ITEM_RADIO_HARDWARE_SH,
#endif
  ITEM_RADIO_HARDWARE_SERIAL_BAUDRATE,
#if defined(BLUETOOTH)
  ITEM_RADIO_HARDWARE_BLUETOOTH_MODE,
  ITEM_RADIO_HARDWARE_BLUETOOTH_PAIRING_CODE,
  ITEM_RADIO_HARDWARE_BLUETOOTH_LOCAL_ADDR,
  ITEM_RADIO_HARDWARE_BLUETOOTH_DISTANT_ADDR,
  ITEM_RADIO_HARDWARE_BLUETOOTH_NAME,
#endif
  ITEM_RADIO_HARDWARE_JITTER_FILTER,
  ITEM_RADIO_HARDWARE_MAX
};

#define POTS_ROWS                      NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1

#if NUM_SWITCHES > 4
#define SWITCHES_ROWS                  NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1
#else
#define SWITCHES_ROWS                  NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1
#endif

#if defined(PCBTARANIS)
#define BLUETOOTH_ROWS                 uint8_t(IS_BLUETOOTH_CHIP_PRESENT() ? 0 : HIDDEN_ROW), uint8_t(g_eeGeneral.bluetoothMode == BLUETOOTH_TELEMETRY ? -1 : HIDDEN_ROW), uint8_t(g_eeGeneral.bluetoothMode == BLUETOOTH_OFF ? HIDDEN_ROW : -1), uint8_t(g_eeGeneral.bluetoothMode == BLUETOOTH_OFF ? HIDDEN_ROW : -1), uint8_t(g_eeGeneral.bluetoothMode == BLUETOOTH_OFF ? HIDDEN_ROW : 0),
#elif defined(BLUETOOTH)
#define BLUETOOTH_ROWS                 0, uint8_t(g_eeGeneral.bluetoothMode == BLUETOOTH_OFF ? HIDDEN_ROW : -1), uint8_t(g_eeGeneral.bluetoothMode == BLUETOOTH_OFF ? HIDDEN_ROW : -1), uint8_t(g_eeGeneral.bluetoothMode == BLUETOOTH_OFF ? HIDDEN_ROW : 0),
#else
#define BLUETOOTH_ROWS
#endif
#if defined(PCBXLITE)
#define SWITCH_TYPE_MAX(sw)            (SWITCH_3POS)
#else
#define SWITCH_TYPE_MAX(sw)            ((MIXSRC_SF-MIXSRC_FIRST_SWITCH == sw || MIXSRC_SH-MIXSRC_FIRST_SWITCH == sw) ? SWITCH_2POS : SWITCH_3POS)
#endif

#define HW_SETTINGS_COLUMN1            30
#define HW_SETTINGS_COLUMN2            (30 + 5*FW)

void menuRadioHardware(event_t event)
{
  MENU(STR_HARDWARE, menuTabGeneral, MENU_RADIO_HARDWARE, HEADER_LINE+ITEM_RADIO_HARDWARE_MAX, { HEADER_LINE_COLUMNS LABEL(Sticks), 0, 0, 0, 0, LABEL(Pots), POTS_ROWS, LABEL(Switches), SWITCHES_ROWS, 0/*max bauds*/, BLUETOOTH_ROWS 0/*jitter filter*/ });

  uint8_t sub = menuVerticalPosition - HEADER_LINE;

  for (uint8_t i=0; i<LCD_LINES-1; i++) {
    coord_t y = MENU_HEADER_HEIGHT + 1 + i*FH;
    uint8_t k = i+menuVerticalOffset;
    for (int j=0; j<=k; j++) {
      if (mstate_tab[j+HEADER_LINE] == HIDDEN_ROW)
        k++;
    }
    uint8_t blink = ((s_editMode>0) ? BLINK|INVERS : INVERS);
    uint8_t attr = (sub == k ? blink : 0);

    switch(k) {
      case ITEM_RADIO_HARDWARE_LABEL_STICKS:
        lcdDrawTextAlignedLeft(y, STR_STICKS);
        break;
      case ITEM_RADIO_HARDWARE_STICK1:
      case ITEM_RADIO_HARDWARE_STICK2:
      case ITEM_RADIO_HARDWARE_STICK3:
      case ITEM_RADIO_HARDWARE_STICK4:
        editStickHardwareSettings(HW_SETTINGS_COLUMN1, y, k - ITEM_RADIO_HARDWARE_STICK1, event, attr);
        break;

      case ITEM_RADIO_HARDWARE_LABEL_POTS:
        lcdDrawTextAlignedLeft(y, STR_POTS);
        break;

      case ITEM_RADIO_HARDWARE_POT1:
      case ITEM_RADIO_HARDWARE_POT2:
      {
        int idx = k - ITEM_RADIO_HARDWARE_POT1;
        uint8_t shift = (2*idx);
        uint8_t mask = (0x03 << shift);
        lcdDrawTextAtIndex(INDENT_WIDTH, y, STR_VSRCRAW, NUM_STICKS+idx+1, menuHorizontalPosition < 0 ? attr : 0);
        if (ZEXIST(g_eeGeneral.anaNames[NUM_STICKS+idx]) || (attr && s_editMode > 0 && menuHorizontalPosition == 0))
          editName(HW_SETTINGS_COLUMN1, y, g_eeGeneral.anaNames[NUM_STICKS+idx], LEN_ANA_NAME, event, attr && menuHorizontalPosition == 0);
        else
          lcdDrawMMM(HW_SETTINGS_COLUMN1, y, menuHorizontalPosition==0 ? attr : 0);
        uint8_t potType = (g_eeGeneral.potsConfig & mask) >> shift;
        potType = editChoice(HW_SETTINGS_COLUMN2, y, "", STR_POTTYPES, potType, POT_NONE, POT_WITHOUT_DETENT, menuHorizontalPosition == 1 ? attr : 0, event);
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
#if NUM_SWITCHES > 4
      case ITEM_RADIO_HARDWARE_SF:
      case ITEM_RADIO_HARDWARE_SH:
#endif
      {
        int index = k-ITEM_RADIO_HARDWARE_SA;
        int config = SWITCH_CONFIG(index);
        lcdDrawTextAtIndex(INDENT_WIDTH, y, STR_VSRCRAW, MIXSRC_FIRST_SWITCH-MIXSRC_Rud+index+1, menuHorizontalPosition < 0 ? attr : 0);
        if (ZEXIST(g_eeGeneral.switchNames[index]) || (attr && s_editMode > 0 && menuHorizontalPosition == 0))
          editName(HW_SETTINGS_COLUMN1, y, g_eeGeneral.switchNames[index], LEN_SWITCH_NAME, event, menuHorizontalPosition == 0 ? attr : 0);
        else
          lcdDrawMMM(HW_SETTINGS_COLUMN1, y, menuHorizontalPosition == 0 ? attr : 0);
        config = editChoice(HW_SETTINGS_COLUMN2, y, "", STR_SWTYPES, config, SWITCH_NONE, SWITCH_TYPE_MAX(index), menuHorizontalPosition == 1 ? attr : 0, event);
        if (attr && checkIncDec_Ret) {
          swconfig_t mask = (swconfig_t)0x03 << (2*index);
          g_eeGeneral.switchConfig = (g_eeGeneral.switchConfig & ~mask) | ((swconfig_t(config) & 0x03) << (2*index));
        }
        break;
      }

      case ITEM_RADIO_HARDWARE_SERIAL_BAUDRATE:
        lcdDrawTextAlignedLeft(y, STR_MAXBAUDRATE);
        lcdDrawNumber(HW_SETTINGS_COLUMN2, y, CROSSFIRE_BAUDRATES[g_eeGeneral.telemetryBaudrate], attr|LEFT);
        if (attr) {
          g_eeGeneral.telemetryBaudrate = DIM(CROSSFIRE_BAUDRATES) - 1 - checkIncDecModel(event, DIM(CROSSFIRE_BAUDRATES) - 1 - g_eeGeneral.telemetryBaudrate, 0, DIM(CROSSFIRE_BAUDRATES) - 1);
          if (checkIncDec_Ret) {
            pauseMixerCalculations();
            pausePulses();
            EXTERNAL_MODULE_OFF();
            CoTickDelay(10); // 20ms so that the pulses interrupt will reinit the frame rate
            telemetryProtocol = 255; // force telemetry port + module reinitialization
            EXTERNAL_MODULE_ON();
            resumePulses();
            resumeMixerCalculations();
          }
        }
        break;

#if defined(BLUETOOTH)
      case ITEM_RADIO_HARDWARE_BLUETOOTH_MODE:
        lcdDrawTextAlignedLeft(y, STR_BLUETOOTH);
        lcdDrawTextAtIndex(HW_SETTINGS_COLUMN2, y, STR_BLUETOOTH_MODES, g_eeGeneral.bluetoothMode, attr);
        if (g_eeGeneral.bluetoothMode != BLUETOOTH_OFF && !IS_BLUETOOTH_CHIP_PRESENT()) {
          g_eeGeneral.bluetoothMode = BLUETOOTH_OFF;
        }
        if (attr) {
          g_eeGeneral.bluetoothMode = checkIncDecGen(event, g_eeGeneral.bluetoothMode, BLUETOOTH_OFF, BLUETOOTH_TRAINER);
        }
        break;

      case ITEM_RADIO_HARDWARE_BLUETOOTH_PAIRING_CODE:
        lcdDrawTextAlignedLeft(y, STR_BLUETOOTH_PIN_CODE);
        lcdDrawText(HW_SETTINGS_COLUMN2, y, "0000");
        break;

      case ITEM_RADIO_HARDWARE_BLUETOOTH_LOCAL_ADDR:
        lcdDrawTextAlignedLeft(y, STR_BLUETOOTH_LOCAL_ADDR);
        lcdDrawText(HW_SETTINGS_COLUMN2, y, bluetoothLocalAddr[0] == '\0' ? "---" : bluetoothLocalAddr);
        break;

      case ITEM_RADIO_HARDWARE_BLUETOOTH_DISTANT_ADDR:
        lcdDrawTextAlignedLeft(y, STR_BLUETOOTH_DIST_ADDR);
        lcdDrawText(HW_SETTINGS_COLUMN2, y, bluetoothDistantAddr[0] == '\0' ? "---" : bluetoothDistantAddr);
        break;

      case ITEM_RADIO_HARDWARE_BLUETOOTH_NAME:
        lcdDrawText(INDENT_WIDTH, y, STR_NAME);
        editName(HW_SETTINGS_COLUMN2, y, g_eeGeneral.bluetoothName, LEN_BLUETOOTH_NAME, event, attr);
        break;
#endif

      case ITEM_RADIO_HARDWARE_JITTER_FILTER:
        g_eeGeneral.jitterFilter = 1 - editCheckBox(1 - g_eeGeneral.jitterFilter, HW_SETTINGS_COLUMN2, y, STR_JITTER_FILTER, attr, event);
        break;
    }
  }
}
#endif // PCBSKY9X
