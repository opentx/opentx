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

#define RECEIVER_OPTIONS_2ND_COLUMN 200

extern uint8_t g_moduleIdx;

enum {
  RECEIVER_SETTINGS_OK = 0,
  RECEIVER_SETTINGS_DIRTY = 1,
  RECEIVER_SETTINGS_WRITING = 2,
};

void onRxOptionsUpdateConfirm(const char * result)
{
  if (result == STR_OK) {
    reusableBuffer.hardwareAndSettings.receiverSettings.state = PXX2_SETTINGS_WRITE;
    reusableBuffer.hardwareAndSettings.receiverSettings.dirty = RECEIVER_SETTINGS_WRITING;
    reusableBuffer.hardwareAndSettings.receiverSettings.timeout = 0;
    moduleState[g_moduleIdx].mode = MODULE_MODE_RECEIVER_SETTINGS;
  }
  else {
    popMenu();
  }
}

enum {
  ITEM_RECEIVER_SETTINGS_PWM_RATE,
  ITEM_RECEIVER_SETTINGS_TELEMETRY,
  ITEM_RECEIVER_SETTINGS_TELEMETRY_25MW,
  ITEM_RECEIVER_SETTINGS_SPORT_MODE,
  ITEM_RECEIVER_SETTINGS_CAPABILITY_NOT_SUPPORTED1,
  ITEM_RECEIVER_SETTINGS_CAPABILITY_NOT_SUPPORTED2,
  ITEM_RECEIVER_SETTINGS_PINMAP_FIRST
};

#define IS_RECEIVER_CAPABILITY_ENABLED(capability)    (reusableBuffer.hardwareAndSettings.modules[g_moduleIdx].receivers[receiverId].information.capabilities & (1 << capability))
#define IF_RECEIVER_CAPABILITY(capability, count)     uint8_t(IS_RECEIVER_CAPABILITY_ENABLED(capability) ? count : HIDDEN_ROW)

#define CH_ENABLE_SPORT   4
#define CH_ENABLE_SBUS    5

bool isSPortModeAvailable(int mode)
{
  uint8_t receiverId = reusableBuffer.hardwareAndSettings.receiverSettings.receiverId;
  if (mode == 2 && !IS_RECEIVER_CAPABILITY_ENABLED(RECEIVER_CAPABILITY_FPORT2)) {
    return false;
  }
  return true;
}

bool menuModelReceiverOptions(event_t event)
{
  const int lim = (g_model.extendedLimits ? (512 * LIMIT_EXT_PERCENT / 100) : 512) * 2;
  uint8_t wbar = LCD_W / 2 - 20;
  auto outputsCount = min<uint8_t>(16, reusableBuffer.hardwareAndSettings.receiverSettings.outputsCount);

  if (event == EVT_ENTRY) {
    // reusableBuffer.hardwareSettings should have been cleared before calling this menu
#if defined(SIMU)
    reusableBuffer.hardwareAndSettings.receiverSettings.state = PXX2_SETTINGS_OK;
    reusableBuffer.hardwareAndSettings.receiverSettings.outputsCount = 16;
#endif
  }

  uint8_t receiverId = reusableBuffer.hardwareAndSettings.receiverSettings.receiverId;
  uint8_t receiverModelId = reusableBuffer.hardwareAndSettings.modules[g_moduleIdx].receivers[receiverId].information.modelID;
  uint8_t receiverVariant = reusableBuffer.hardwareAndSettings.modules[g_moduleIdx].receivers[receiverId].information.variant;

  SUBMENU(STR_RECEIVER_OPTIONS, ICON_MODEL_SETUP, ITEM_RECEIVER_SETTINGS_PINMAP_FIRST + outputsCount, {
    0, // PWM rate
    isModuleR9MAccess(g_moduleIdx) && receiverVariant == PXX2_VARIANT_EU && reusableBuffer.hardwareAndSettings.moduleSettings.txPower > 14 /*25mW*/ ? READONLY_ROW : (uint8_t)0, // Telemetry
    IF_RECEIVER_CAPABILITY(RECEIVER_CAPABILITY_TELEMETRY_25MW, 0),
    uint8_t((IS_RECEIVER_CAPABILITY_ENABLED(RECEIVER_CAPABILITY_FPORT) || IS_RECEIVER_CAPABILITY_ENABLED(RECEIVER_CAPABILITY_FPORT2)) ? 0 : HIDDEN_ROW),
    uint8_t(reusableBuffer.hardwareAndSettings.modules[g_moduleIdx].receivers[receiverId].information.capabilityNotSupported ? READONLY_ROW : HIDDEN_ROW),
    uint8_t(reusableBuffer.hardwareAndSettings.modules[g_moduleIdx].receivers[receiverId].information.capabilityNotSupported ? READONLY_ROW : HIDDEN_ROW),
    0 // channels ...
  });

  if (g_model.moduleData[g_moduleIdx].pxx2.receiverName[receiverId][0] != '\0')
    lcdDrawSizedText(50, 3 + FH, g_model.moduleData[g_moduleIdx].pxx2.receiverName[receiverId], effectiveLen(g_model.moduleData[g_moduleIdx].pxx2.receiverName[receiverId], PXX2_LEN_RX_NAME), MENU_TITLE_COLOR);
  else
    lcdDrawText(50, 3 + FH, "---", MENU_TITLE_COLOR);

  if (menuEvent) {
    killEvents(KEY_EXIT);
    moduleState[g_moduleIdx].mode = MODULE_MODE_NORMAL;
    if (reusableBuffer.hardwareAndSettings.receiverSettings.dirty) {
      abortPopMenu();
      POPUP_CONFIRMATION(STR_UPDATE_RX_OPTIONS, onRxOptionsUpdateConfirm);
    }
    else {
      return false;
    }
  }

  if (reusableBuffer.hardwareAndSettings.receiverSettings.state == PXX2_HARDWARE_INFO && moduleState[g_moduleIdx].mode == MODULE_MODE_NORMAL) {
    if (!receiverModelId)
      moduleState[g_moduleIdx].readModuleInformation(&reusableBuffer.hardwareAndSettings.modules[g_moduleIdx], receiverId, receiverId);
    else if (isModuleR9MAccess(g_moduleIdx) && receiverVariant == PXX2_VARIANT_EU && !reusableBuffer.hardwareAndSettings.moduleSettings.txPower)
      moduleState[g_moduleIdx].readModuleSettings(&reusableBuffer.hardwareAndSettings.moduleSettings);
    else
      moduleState[g_moduleIdx].readReceiverSettings(&reusableBuffer.hardwareAndSettings.receiverSettings);
  }

  if (event == EVT_KEY_LONG(KEY_ENTER) && reusableBuffer.hardwareAndSettings.receiverSettings.dirty) {
    killEvents(event);
    reusableBuffer.hardwareAndSettings.receiverSettings.dirty = RECEIVER_SETTINGS_OK;
    moduleState[g_moduleIdx].writeReceiverSettings(&reusableBuffer.hardwareAndSettings.receiverSettings);
  }

  if (reusableBuffer.hardwareAndSettings.receiverSettings.dirty == RECEIVER_SETTINGS_WRITING && reusableBuffer.hardwareAndSettings.receiverSettings.state == PXX2_SETTINGS_OK) {
    popMenu();
    return false;
  }

  if (receiverModelId && mstate_tab[menuVerticalPosition] == HIDDEN_ROW) {
    menuVerticalPosition = 0;
    while (menuVerticalPosition < ITEM_RECEIVER_SETTINGS_PINMAP_FIRST && mstate_tab[menuVerticalPosition] == HIDDEN_ROW) {
      ++menuVerticalPosition;
    }
  }

  int8_t sub = menuVerticalPosition;

  if (reusableBuffer.hardwareAndSettings.receiverSettings.state == PXX2_SETTINGS_OK) {
    for (uint8_t k=0; k<NUM_BODY_LINES + 1/*plus one line in submenus*/; k++) {
      coord_t y = MENU_HEADER_HEIGHT + 1 + k*FH;
      uint8_t i = k + menuVerticalOffset;
      for (int j=0; j<=i; ++j) {
        if (j<(int)DIM(mstate_tab) && mstate_tab[j] == HIDDEN_ROW) {
          ++i;
        }
      }
      LcdFlags attr = (sub==i ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);

      switch (i) {
        case ITEM_RECEIVER_SETTINGS_PWM_RATE:
          lcdDrawText(MENUS_MARGIN_LEFT, y, isModuleR9MAccess(g_moduleIdx) ? "6.67ms PWM": "9ms PWM");
          reusableBuffer.hardwareAndSettings.receiverSettings.pwmRate = editCheckBox(reusableBuffer.hardwareAndSettings.receiverSettings.pwmRate, RECEIVER_OPTIONS_2ND_COLUMN, y, attr, event);
          if (attr && checkIncDec_Ret) {
            reusableBuffer.hardwareAndSettings.receiverSettings.dirty = RECEIVER_SETTINGS_DIRTY;
          }
          break;

        case ITEM_RECEIVER_SETTINGS_TELEMETRY:
          lcdDrawText(MENUS_MARGIN_LEFT, y, STR_TELEMETRY_DISABLED);
          reusableBuffer.hardwareAndSettings.receiverSettings.telemetryDisabled = editCheckBox(reusableBuffer.hardwareAndSettings.receiverSettings.telemetryDisabled, RECEIVER_OPTIONS_2ND_COLUMN, y, attr, event);
          if (attr && checkIncDec_Ret) {
            reusableBuffer.hardwareAndSettings.receiverSettings.dirty = RECEIVER_SETTINGS_DIRTY;
          }
          break;

        case ITEM_RECEIVER_SETTINGS_TELEMETRY_25MW:
          lcdDrawText(MENUS_MARGIN_LEFT, y, "25mw Tele");
          reusableBuffer.hardwareAndSettings.receiverSettings.telemetry25mw = editCheckBox(reusableBuffer.hardwareAndSettings.receiverSettings.telemetry25mw, RECEIVER_OPTIONS_2ND_COLUMN, y, attr, event);
          if (attr && checkIncDec_Ret) {
            reusableBuffer.hardwareAndSettings.receiverSettings.dirty = RECEIVER_SETTINGS_DIRTY;
          }
          break;

        case ITEM_RECEIVER_SETTINGS_SPORT_MODE:
        {
          lcdDrawText(0, y, STR_PROTOCOL);
          uint8_t portType = reusableBuffer.hardwareAndSettings.receiverSettings.fport | (reusableBuffer.hardwareAndSettings.receiverSettings.fport2 << 1);
          lcdDrawTextAtIndex(LCD_W/2, y, STR_SPORT_MODES, portType, attr);
          portType = checkIncDec(event, portType, 0, 2, EE_MODEL, isSPortModeAvailable);
          if (checkIncDec_Ret) {
            reusableBuffer.hardwareAndSettings.receiverSettings.fport = portType & 0x01;
            reusableBuffer.hardwareAndSettings.receiverSettings.fport2 = (portType & 0x02) >> 1;
            reusableBuffer.hardwareAndSettings.receiverSettings.dirty = RECEIVER_SETTINGS_DIRTY;
          }
          break;
        }

        case ITEM_RECEIVER_SETTINGS_CAPABILITY_NOT_SUPPORTED1:
          lcdDrawText(LCD_W/2, y+1, STR_MORE_OPTIONS_AVAILABLE, SMLSIZE|CENTERED);
          break;

        case ITEM_RECEIVER_SETTINGS_CAPABILITY_NOT_SUPPORTED2:
          lcdDrawText(LCD_W/2, y+1, STR_OPENTX_UPGRADE_REQUIRED, SMLSIZE|CENTERED);
          break;

        default:
        // Pin
        {
          uint8_t pin = i - ITEM_RECEIVER_SETTINGS_PINMAP_FIRST;
          if (pin < reusableBuffer.hardwareAndSettings.receiverSettings.outputsCount) {
            uint8_t & mapping = reusableBuffer.hardwareAndSettings.receiverSettings.outputsMapping[pin];
            uint8_t channel = g_model.moduleData[g_moduleIdx].channelsStart + mapping;
            int32_t channelValue = channelOutputs[channel];
            lcdDrawText(MENUS_MARGIN_LEFT, y, STR_PIN);
            lcdDrawNumber(lcdNextPos + 1, y, pin + 1);

            uint8_t channelMax = sentModuleChannels(g_moduleIdx) - 1;
            uint8_t selectionMax = channelMax;

            if (IS_RECEIVER_CAPABILITY_ENABLED(RECEIVER_CAPABILITY_ENABLE_PWM_CH5_CH6)) {
              if (CH_ENABLE_SPORT == pin || CH_ENABLE_SBUS == pin) {
                selectionMax += 1;
              }
              if (CH_ENABLE_SPORT == pin && selectionMax == channel) {
                lcdDrawText(100, y,  "S.PORT", attr);
              }
              else if (CH_ENABLE_SBUS == pin && selectionMax == channel) {
                lcdDrawText(100, y,  "SBUS", attr);
              }
              else {
                putsChn(100, y, channel + 1, attr);
              }
            }
            else {
              putsChn(100, y, channel + 1, attr);
            }

            // Channel
            if (attr) {
              mapping = checkIncDec(event, mapping, 0, selectionMax);
              if (checkIncDec_Ret) {
                reusableBuffer.hardwareAndSettings.receiverSettings.dirty = RECEIVER_SETTINGS_DIRTY;
              }
            }

            // Bargraph
            if (channel <= channelMax) {
              lcdDrawRect(RECEIVER_OPTIONS_2ND_COLUMN, y + 4, wbar + 1, 10);
              auto lenChannel = limit<uint8_t>(1, (abs(channelValue) * wbar / 2 + lim / 2) / lim, wbar / 2);
              auto xChannel = (channelValue > 0) ? RECEIVER_OPTIONS_2ND_COLUMN + wbar / 2 : RECEIVER_OPTIONS_2ND_COLUMN + wbar / 2 + 1 - lenChannel;
              lcdDrawSolidFilledRect(xChannel, y + 5, lenChannel, 8, TEXT_INVERTED_BGCOLOR);
            }
          }
          break;
        }
      }
    }
  }
  else {
    lcdDrawCenteredText(LCD_H/2, STR_WAITING_FOR_RX);
  }

  return true;
}
