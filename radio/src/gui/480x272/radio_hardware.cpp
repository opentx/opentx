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
  ITEM_RADIO_HARDWARE_CALIBRATION,
  ITEM_RADIO_HARDWARE_LABEL_STICKS,
  ITEM_RADIO_HARDWARE_STICK1,
  ITEM_RADIO_HARDWARE_STICK2,
  ITEM_RADIO_HARDWARE_STICK3,
  ITEM_RADIO_HARDWARE_STICK4,
  ITEM_RADIO_HARDWARE_LABEL_POTS,
  ITEM_RADIO_HARDWARE_POT1,
  ITEM_RADIO_HARDWARE_POT2,
  ITEM_RADIO_HARDWARE_POT3,
#if defined(PCBX10)
  ITEM_RADIO_HARDWARE_EXT1,
  ITEM_RADIO_HARDWARE_EXT2,
#endif
  ITEM_RADIO_HARDWARE_LS,
  ITEM_RADIO_HARDWARE_RS,
#if defined(PCBX12S)
  ITEM_RADIO_HARDWARE_LS2,
  ITEM_RADIO_HARDWARE_RS2,
#endif
  ITEM_RADIO_HARDWARE_LABEL_SWITCHES,
  ITEM_RADIO_HARDWARE_SA,
  ITEM_RADIO_HARDWARE_SB,
  ITEM_RADIO_HARDWARE_SC,
  ITEM_RADIO_HARDWARE_SD,
  ITEM_RADIO_HARDWARE_SE,
  ITEM_RADIO_HARDWARE_SF,
  ITEM_RADIO_HARDWARE_SG,
  ITEM_RADIO_HARDWARE_SH,
  ITEM_RADIO_HARDWARE_SI, // Gimbal switch left
  ITEM_RADIO_HARDWARE_SJ, // Gimbal switch right
  ITEM_RADIO_HARDWARE_BATTERY_CALIB,
  ITEM_RADIO_HARDWARE_RTC_BATTERY,
  ITEM_RADIO_HARDWARE_RTC_CHECK,
  ITEM_RADIO_HARDWARE_SERIAL_BAUDRATE,
#if defined(BLUETOOTH)
  ITEM_RADIO_HARDWARE_BLUETOOTH_MODE,
  ITEM_RADIO_HARDWARE_BLUETOOTH_PAIRING_CODE,
  ITEM_RADIO_HARDWARE_BLUETOOTH_LOCAL_ADDR,
  ITEM_RADIO_HARDWARE_BLUETOOTH_DISTANT_ADDR,
  ITEM_RADIO_HARDWARE_BLUETOOTH_NAME,
#endif
#if defined(INTERNAL_MODULE_PXX1) && defined(EXTERNAL_ANTENNA)
  ITEM_RADIO_HARDWARE_EXTERNAL_ANTENNA,
#endif

#if defined(AUX_SERIAL)
  ITEM_RADIO_HARDWARE_AUX_SERIAL_MODE,
#endif
#if defined(AUX2_SERIAL)
  ITEM_RADIO_HARDWARE_AUX2_SERIAL_MODE,
#endif
  ITEM_RADIO_HARDWARE_JITTER_FILTER,
  ITEM_RADIO_HARDWARE_RAS,

#if defined(SPORT_UPDATE_PWR_GPIO)
  ITEM_RADIO_HARDWARE_SPORT_UPDATE_POWER,
#endif
  ITEM_RADIO_HARDWARE_DEBUG,
  ITEM_RADIO_HARDWARE_MAX
};

#define HW_SETTINGS_COLUMN1            150
#define HW_SETTINGS_COLUMN2            200

#if defined(AUX_SERIAL)
  #define AUX_SERIAL_ROW                0,
#else
  #define AUX_SERIAL_ROW
#endif
#if defined(AUX2_SERIAL)
#define AUX2_SERIAL_ROW                0,
#else
#define AUX2_SERIAL_ROW
#endif

#define POTS_ROWS                      NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1
#define SWITCHES_ROWS                  NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1, NAVIGATION_LINE_BY_LINE|1

#if defined(BLUETOOTH)
  #define BLUETOOTH_ROWS                 0, uint8_t(g_eeGeneral.bluetoothMode == BLUETOOTH_TELEMETRY ? READONLY_ROW : HIDDEN_ROW), uint8_t(g_eeGeneral.bluetoothMode == BLUETOOTH_OFF ? HIDDEN_ROW : READONLY_ROW), uint8_t(g_eeGeneral.bluetoothMode == BLUETOOTH_OFF ? HIDDEN_ROW : READONLY_ROW), uint8_t(g_eeGeneral.bluetoothMode == BLUETOOTH_OFF ? HIDDEN_ROW : 0),
#else
  #define BLUETOOTH_ROWS
#endif

// TODO should be moved to the HAL
#define SWITCH_TYPE_MAX(sw)            ((MIXSRC_SF-MIXSRC_FIRST_SWITCH == sw || MIXSRC_SH-MIXSRC_FIRST_SWITCH == sw || MIXSRC_SI-MIXSRC_FIRST_SWITCH == sw || MIXSRC_SJ-MIXSRC_FIRST_SWITCH == sw) ? SWITCH_2POS : SWITCH_3POS)

#if defined(INTERNAL_MODULE_PXX1) && defined(EXTERNAL_ANTENNA)
#define EXTERNAL_ANTENNA_ROW         0,
void onHardwareAntennaSwitchConfirm(const char * result)
{
  if (result == STR_OK) {
    // Switch to external antenna confirmation
    g_eeGeneral.antennaMode = reusableBuffer.radioHardware.antennaMode;
    storageDirty(EE_GENERAL);
  }
  else {
    reusableBuffer.radioHardware.antennaMode = g_eeGeneral.antennaMode;
  }
}
#else
#define EXTERNAL_ANTENNA_ROW
#endif

#if (defined(CROSSFIRE) || defined(GHOST))
  #define MAX_BAUDRATE_ROW          0
#else
  #define MAX_BAUDRATE_ROW          HIDDEN_ROW
#endif

#if defined(SPORT_UPDATE_PWR_GPIO)
  #define SPORT_POWER_ROWS 0,
#else
  #define SPORT_POWER_ROWS
#endif

bool menuRadioHardware(event_t event)
{
  MENU(STR_HARDWARE, RADIO_ICONS, menuTabGeneral, MENU_RADIO_HARDWARE, ITEM_RADIO_HARDWARE_MAX, {
    0 /* calibration button */,

    LABEL(Sticks),
      0 /* stick 1 */,
      0 /* stick 2 */,
      0 /* stick 3 */,
      0 /* stick 4 */,

    LABEL(Pots),
      POTS_ROWS,

    LABEL(Switches),
      SWITCHES_ROWS,

    0, /* battery */
    READONLY_ROW, /* RTC */
    0, /* RTC check */

    MAX_BAUDRATE_ROW, /* max baudrate */

    BLUETOOTH_ROWS

    EXTERNAL_ANTENNA_ROW

    AUX_SERIAL_ROW /* aux serial mode */
    AUX2_SERIAL_ROW /* aux2 serial mode */
    0, /* ADC filter */
    READONLY_ROW /* RAS */,
    SPORT_POWER_ROWS
    1, /* Debug */
  });

  if (menuEvent) {
    disableVBatBridge();
  }
  else if (event == EVT_ENTRY) {
    enableVBatBridge();
#if defined(INTERNAL_MODULE_PXX1) && defined(EXTERNAL_ANTENNA)
    reusableBuffer.radioHardware.antennaMode = g_eeGeneral.antennaMode;
#endif
  }

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
      case ITEM_RADIO_HARDWARE_CALIBRATION:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_CALIBRATION, attr);
        if (attr && s_editMode>0) {
          pushMenu(menuRadioCalibration);
        }
        break;

      case ITEM_RADIO_HARDWARE_LABEL_STICKS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_STICKS);
        break;

      case ITEM_RADIO_HARDWARE_STICK1:
      case ITEM_RADIO_HARDWARE_STICK2:
      case ITEM_RADIO_HARDWARE_STICK3:
      case ITEM_RADIO_HARDWARE_STICK4:
        editStickHardwareSettings(HW_SETTINGS_COLUMN1, y, k - ITEM_RADIO_HARDWARE_STICK1, event, attr);
        break;

      case ITEM_RADIO_HARDWARE_LS:
      case ITEM_RADIO_HARDWARE_RS:
#if defined(PCBX12S)
      case ITEM_RADIO_HARDWARE_LS2:
      case ITEM_RADIO_HARDWARE_RS2:
#endif
      {
        int idx = k - ITEM_RADIO_HARDWARE_LS;
        uint8_t mask = (0x01 << idx);
        lcdDrawTextAtIndex(INDENT_WIDTH, y, STR_VSRCRAW, NUM_STICKS+NUM_POTS+idx+1, menuHorizontalPosition < 0 ? attr : 0);
        if (ZEXIST(g_eeGeneral.anaNames[NUM_STICKS+NUM_POTS+idx]) || (attr && menuHorizontalPosition == 0))
          editName(HW_SETTINGS_COLUMN1, y, g_eeGeneral.anaNames[NUM_STICKS+NUM_POTS+idx], LEN_ANA_NAME, event, attr && menuHorizontalPosition == 0);
        else
          lcdDrawMMM(HW_SETTINGS_COLUMN1, y, 0);
        uint8_t potType = (g_eeGeneral.slidersConfig & mask) >> idx;
        potType = editChoice(HW_SETTINGS_COLUMN2, y, STR_SLIDERTYPES, potType, SLIDER_NONE, SLIDER_WITH_DETENT, menuHorizontalPosition == 1 ? attr : 0, event);
        g_eeGeneral.slidersConfig &= ~mask;
        g_eeGeneral.slidersConfig |= (potType << idx);
        break;
      }

      case ITEM_RADIO_HARDWARE_LABEL_POTS:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_POTS);
        break;

      case ITEM_RADIO_HARDWARE_POT1:
      case ITEM_RADIO_HARDWARE_POT2:
      case ITEM_RADIO_HARDWARE_POT3:
#if defined(PCBX10)
      case ITEM_RADIO_HARDWARE_EXT1:
      case ITEM_RADIO_HARDWARE_EXT2:
#endif
      {
        int index = k - ITEM_RADIO_HARDWARE_POT1;
        lcdDrawTextAtIndex(INDENT_WIDTH, y, STR_VSRCRAW, NUM_STICKS+index+1, menuHorizontalPosition < 0 ? attr : 0);
        if (ZEXIST(g_eeGeneral.anaNames[NUM_STICKS+index]) || (attr && menuHorizontalPosition == 0))
          editName(HW_SETTINGS_COLUMN1, y, g_eeGeneral.anaNames[NUM_STICKS+index], LEN_ANA_NAME, event, attr && menuHorizontalPosition == 0);
        else
          lcdDrawMMM(HW_SETTINGS_COLUMN1, y, 0);
        uint32_t potType = bfGet<uint32_t>(g_eeGeneral.potsConfig, 2*(index), 2);
        potType = editChoice(HW_SETTINGS_COLUMN2, y, STR_POTTYPES, potType, POT_NONE, POT_WITHOUT_DETENT, menuHorizontalPosition == 1 ? attr : 0, event);
        g_eeGeneral.potsConfig = bfSet<uint32_t>(g_eeGeneral.potsConfig, potType, 2*index, 2);
        break;
      }

      case ITEM_RADIO_HARDWARE_LABEL_SWITCHES:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_SWITCHES);
        break;

      case ITEM_RADIO_HARDWARE_SA:
      case ITEM_RADIO_HARDWARE_SB:
      case ITEM_RADIO_HARDWARE_SC:
      case ITEM_RADIO_HARDWARE_SD:
      case ITEM_RADIO_HARDWARE_SE:
      case ITEM_RADIO_HARDWARE_SF:
      case ITEM_RADIO_HARDWARE_SG:
      case ITEM_RADIO_HARDWARE_SH:
      case ITEM_RADIO_HARDWARE_SI:
      case ITEM_RADIO_HARDWARE_SJ:
      {
        int index = k - ITEM_RADIO_HARDWARE_SA;
        int config = SWITCH_CONFIG(index);
        lcdDrawTextAtIndex(INDENT_WIDTH, y, STR_VSRCRAW, MIXSRC_FIRST_SWITCH-MIXSRC_Rud+index+1, menuHorizontalPosition < 0 ? attr : 0);
        if (ZEXIST(g_eeGeneral.switchNames[index]) || (attr && menuHorizontalPosition == 0))
          editName(HW_SETTINGS_COLUMN1, y, g_eeGeneral.switchNames[index], LEN_SWITCH_NAME, event, menuHorizontalPosition == 0 ? attr : 0);
        else
          lcdDrawMMM(HW_SETTINGS_COLUMN1, y, 0);
        config = editChoice(HW_SETTINGS_COLUMN2, y, STR_SWTYPES, config, SWITCH_NONE, SWITCH_TYPE_MAX(index), menuHorizontalPosition == 1 ? attr : 0, event);
        if (attr && checkIncDec_Ret) {
          g_eeGeneral.switchConfig = bfSet<swconfig_t>(g_eeGeneral.switchConfig, config, 2*index, 2);
        }
        break;
      }

      case ITEM_RADIO_HARDWARE_BATTERY_CALIB:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_BATT_CALIB);
        lcdDrawNumber(HW_SETTINGS_COLUMN2, y, getBatteryVoltage(), attr|PREC2, 0, NULL, "V");
        if (attr) CHECK_INCDEC_GENVAR(event, g_eeGeneral.txVoltageCalibration, -127, 127);
        break;

      case ITEM_RADIO_HARDWARE_RTC_BATTERY:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_RTC_BATT);
        lcdDrawNumber(HW_SETTINGS_COLUMN2, y, getRTCBatteryVoltage(), attr|PREC2, 0, NULL, "V");
        break;

      case ITEM_RADIO_HARDWARE_SERIAL_BAUDRATE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_MAXBAUDRATE);
        lcdDrawNumber(HW_SETTINGS_COLUMN2, y, CROSSFIRE_BAUDRATES[g_eeGeneral.telemetryBaudrate], attr|LEFT);
        if (attr) {
          g_eeGeneral.telemetryBaudrate = DIM(CROSSFIRE_BAUDRATES) - 1 - checkIncDecModel(event, DIM(CROSSFIRE_BAUDRATES) - 1 - g_eeGeneral.telemetryBaudrate, 0, DIM(CROSSFIRE_BAUDRATES) - 1);
          if (checkIncDec_Ret && IS_EXTERNAL_MODULE_ON()) {
            pauseMixerCalculations();
            pausePulses();
            EXTERNAL_MODULE_OFF();
            RTOS_WAIT_MS(20); // 20ms so that the pulses interrupt will reinit the frame rate
            telemetryProtocol = 255; // force telemetry port + module reinitialization
            EXTERNAL_MODULE_ON();
            resumePulses();
            resumeMixerCalculations();
          }
        }
        break;

      case ITEM_RADIO_HARDWARE_RTC_CHECK:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_RTC_CHECK);
        uint8_t b = 1 - g_eeGeneral.disableRtcWarning;
        g_eeGeneral.disableRtcWarning = 1 - editCheckBox(b, HW_SETTINGS_COLUMN2, y, attr, event);
        break;
      }
#if defined(BLUETOOTH)
      case ITEM_RADIO_HARDWARE_BLUETOOTH_MODE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_BLUETOOTH);
        g_eeGeneral.bluetoothMode = editChoice(HW_SETTINGS_COLUMN2, y, STR_BLUETOOTH_MODES, g_eeGeneral.bluetoothMode, BLUETOOTH_OFF, BLUETOOTH_MAX, attr, event);
        break;

      case ITEM_RADIO_HARDWARE_BLUETOOTH_PAIRING_CODE:
        lcdDrawText(INDENT_WIDTH, y, STR_BLUETOOTH_PIN_CODE);
        lcdDrawText(HW_SETTINGS_COLUMN2, y, "000000", 0);
        break;

      case ITEM_RADIO_HARDWARE_BLUETOOTH_LOCAL_ADDR:
        lcdDrawText(INDENT_WIDTH, y, STR_BLUETOOTH_LOCAL_ADDR);
        lcdDrawText(HW_SETTINGS_COLUMN2, y, bluetooth.localAddr[0] == '\0' ? "---" : bluetooth.localAddr);
        break;

      case ITEM_RADIO_HARDWARE_BLUETOOTH_DISTANT_ADDR:
        lcdDrawText(INDENT_WIDTH, y, STR_BLUETOOTH_DIST_ADDR);
        lcdDrawText(HW_SETTINGS_COLUMN2, y, bluetooth.distantAddr[0] == '\0' ? "---" : bluetooth.distantAddr);
        break;

      case ITEM_RADIO_HARDWARE_BLUETOOTH_NAME:
        lcdDrawText(INDENT_WIDTH, y, STR_NAME);
        editName(HW_SETTINGS_COLUMN2, y, g_eeGeneral.bluetoothName, LEN_BLUETOOTH_NAME, event, attr);
        break;
#endif

#if defined(INTERNAL_MODULE_PXX1) && defined(EXTERNAL_ANTENNA)
      case ITEM_RADIO_HARDWARE_EXTERNAL_ANTENNA:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_ANTENNA);
        reusableBuffer.radioHardware.antennaMode = editChoice(HW_SETTINGS_COLUMN2, y, STR_ANTENNA_MODES, reusableBuffer.radioHardware.antennaMode, ANTENNA_MODE_INTERNAL, ANTENNA_MODE_EXTERNAL, attr, event);
        if (!s_editMode && reusableBuffer.radioHardware.antennaMode != g_eeGeneral.antennaMode) {
          if (!isExternalAntennaEnabled() && (reusableBuffer.radioHardware.antennaMode == ANTENNA_MODE_EXTERNAL || (reusableBuffer.radioHardware.antennaMode == ANTENNA_MODE_PER_MODEL && g_model.moduleData[INTERNAL_MODULE].pxx.antennaMode == ANTENNA_MODE_EXTERNAL))) {
            POPUP_CONFIRMATION(STR_ANTENNACONFIRM1, onHardwareAntennaSwitchConfirm);
            SET_WARNING_INFO(STR_ANTENNACONFIRM2, sizeof(TR_ANTENNACONFIRM2), 0);
          }
          else {
            g_eeGeneral.antennaMode = reusableBuffer.radioHardware.antennaMode;
            checkExternalAntenna();
          }
        }
        break;
#endif

#if defined(AUX_SERIAL)
      case ITEM_RADIO_HARDWARE_AUX_SERIAL_MODE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_AUX_SERIAL_MODE);
#if defined(RADIO_TX16S)
        lcdDrawText(lcdNextPos, y, " (TTL)");
#endif
        g_eeGeneral.auxSerialMode = editChoice(HW_SETTINGS_COLUMN2, y, STR_AUX_SERIAL_MODES, g_eeGeneral.auxSerialMode, 0, UART_MODE_MAX, attr, event, isAux1ModeAvailable);
        if (attr && checkIncDec_Ret) {
          auxSerialInit(g_eeGeneral.auxSerialMode, modelTelemetryProtocol());
        }
        break;
#endif

#if defined(AUX2_SERIAL)
      case ITEM_RADIO_HARDWARE_AUX2_SERIAL_MODE:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_AUX2_SERIAL_MODE);
#if defined(RADIO_TX16S)
        lcdDrawText(lcdNextPos, y, " (TTL)");
#endif
        g_eeGeneral.aux2SerialMode = editChoice(HW_SETTINGS_COLUMN2, y, STR_AUX_SERIAL_MODES, g_eeGeneral.aux2SerialMode, 0, UART_MODE_MAX, attr, event, isAux2ModeAvailable);
        if (attr && checkIncDec_Ret) {
          aux2SerialInit(g_eeGeneral.aux2SerialMode, modelTelemetryProtocol());
        }
        break;
#endif

      case ITEM_RADIO_HARDWARE_JITTER_FILTER:
      {
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_JITTER_FILTER);
        uint8_t b = 1 - g_eeGeneral.jitterFilter;
        g_eeGeneral.jitterFilter = 1 - editCheckBox(b, HW_SETTINGS_COLUMN2, y, attr, event);
        break;
      }

      case ITEM_RADIO_HARDWARE_RAS:
#if defined(HARDWARE_INTERNAL_RAS)
        lcdDrawText(MENUS_MARGIN_LEFT, y, "RAS");
        if (telemetryData.swrInternal.isFresh())
          lcdDrawNumber(HW_SETTINGS_COLUMN2, y, telemetryData.swrInternal.value());
        else
          lcdDrawText(HW_SETTINGS_COLUMN2, y, "---");
        lcdDrawText(lcdNextPos, y, "/");
#else
        lcdDrawText(MENUS_MARGIN_LEFT, y, "Ext. RAS");
        lcdNextPos = HW_SETTINGS_COLUMN2;
#endif
        if (telemetryData.swrExternal.isFresh())
          lcdDrawNumber(lcdNextPos, y, telemetryData.swrExternal.value());
        else
          lcdDrawText(lcdNextPos, y, "---");
        break;

#if defined(SPORT_UPDATE_PWR_GPIO)
      case ITEM_RADIO_HARDWARE_SPORT_UPDATE_POWER:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_SPORT_UPDATE_POWER_MODE);
        g_eeGeneral.sportUpdatePower = editChoice(HW_SETTINGS_COLUMN2, y, STR_SPORT_UPDATE_POWER_MODES, g_eeGeneral.sportUpdatePower, 0, 1, attr, event);
        if (attr && checkIncDec_Ret) {
          SPORT_UPDATE_POWER_INIT();
        }
        break;
#endif

      case ITEM_RADIO_HARDWARE_DEBUG:
        lcdDrawText(MENUS_MARGIN_LEFT, y, STR_DEBUG);
        lcdDrawText(HW_SETTINGS_COLUMN2, y, STR_ANALOGS_BTN, menuHorizontalPosition == 0 ? attr : 0);
        lcdDrawText(lcdNextPos + 10, y, STR_KEYS_BTN, menuHorizontalPosition == 1 ? attr : 0);
        if (attr && event == EVT_KEY_BREAK(KEY_ENTER)) {
          if (menuHorizontalPosition == 0)
            pushMenu(menuRadioDiagAnalogs);
          else
            pushMenu(menuRadioDiagKeys);
        }
        break;
    }
  }

  return true;
}
