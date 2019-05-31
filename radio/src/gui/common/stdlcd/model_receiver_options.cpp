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

#define RECEIVER_OPTIONS_2ND_COLUMN 80

extern uint8_t g_moduleIdx;

void onRxOptionsUpdateConfirm(const char * result)
{
  if (result == STR_OK) {
    reusableBuffer.hardwareAndSettings.receiverSettings.state = PXX2_SETTINGS_WRITE;
    reusableBuffer.hardwareAndSettings.receiverSettings.dirty = 2;
    reusableBuffer.hardwareAndSettings.receiverSettings.timeout = 0;
    moduleState[g_moduleIdx].mode = MODULE_MODE_RECEIVER_SETTINGS;
  }
  else {
    popMenu();
  }
}

enum {
  ITEM_RECEIVER_TELEMETRY,
  ITEM_RECEIVER_PWM_RATE,
  ITEM_RECEIVER_PINMAP_FIRST
};

void menuModelReceiverOptions(event_t event)
{
  const int lim = (g_model.extendedLimits ? (512 * LIMIT_EXT_PERCENT / 100) : 512) * 2;
  uint8_t wbar = LCD_W / 2 - 20;
  auto outputsCount = min<uint8_t>(16, reusableBuffer.hardwareAndSettings.receiverSettings.outputsCount);

  SIMPLE_SUBMENU_NOTITLE(ITEM_RECEIVER_PINMAP_FIRST + outputsCount);

  if (event == EVT_ENTRY) {
#if defined(SIMU)
    reusableBuffer.hardwareAndSettings.receiverSettings.state = PXX2_SETTINGS_OK;
    reusableBuffer.hardwareAndSettings.receiverSettings.outputsCount = 8;
#else
    // no need to initialize reusableBuffer.hardwareAndSettings.receiverSettings.state to PXX2_HARDWARE_INFO
    moduleState[g_moduleIdx].readModuleInformation(&reusableBuffer.hardwareAndSettings.modules[g_moduleIdx], reusableBuffer.hardwareAndSettings.receiverSettings.receiverId, reusableBuffer.hardwareAndSettings.receiverSettings.receiverId);
#endif
  }

  if (reusableBuffer.hardwareAndSettings.receiverSettings.state == PXX2_HARDWARE_INFO && moduleState[g_moduleIdx].mode == MODULE_MODE_NORMAL) {
    reusableBuffer.hardwareAndSettings.receiverSettings.state = PXX2_SETTINGS_READ;
    moduleState[g_moduleIdx].mode = MODULE_MODE_RECEIVER_SETTINGS;
  }

  if (menuEvent) {
    killEvents(KEY_EXIT);
    moduleState[g_moduleIdx].mode = MODULE_MODE_NORMAL;
    if (reusableBuffer.hardwareAndSettings.receiverSettings.dirty) {
      abortPopMenu();
      POPUP_CONFIRMATION(STR_UPDATE_RX_OPTIONS, onRxOptionsUpdateConfirm);
    }
    else {
      return;
    }
  }

  if (event == EVT_KEY_LONG(KEY_ENTER) && reusableBuffer.hardwareAndSettings.receiverSettings.dirty) {
    killEvents(event);
    reusableBuffer.hardwareAndSettings.receiverSettings.state = PXX2_SETTINGS_WRITE;
    reusableBuffer.hardwareAndSettings.receiverSettings.dirty = 0;
    reusableBuffer.hardwareAndSettings.receiverSettings.timeout = 0;
    moduleState[g_moduleIdx].mode = MODULE_MODE_RECEIVER_SETTINGS;
  }

  if (reusableBuffer.hardwareAndSettings.receiverSettings.dirty == 2 && reusableBuffer.hardwareAndSettings.receiverSettings.state == PXX2_SETTINGS_OK) {
    popMenu();
    return;
  }

  int8_t sub = menuVerticalPosition;
  lcdDrawTextAlignedLeft(0, STR_RECEIVER_OPTIONS);
  drawReceiverName(FW * 13, 0, g_moduleIdx, reusableBuffer.hardwareAndSettings.receiverSettings.receiverId);
  lcdInvertLine(0);

  if (reusableBuffer.hardwareAndSettings.receiverSettings.state == PXX2_SETTINGS_OK) {
    for (uint8_t k=0; k<LCD_LINES-1; k++) {
      coord_t y = MENU_HEADER_HEIGHT + 1 + k*FH;
      uint8_t i = k + menuVerticalOffset;
      LcdFlags attr = (sub==i ? (s_editMode>0 ? BLINK|INVERS : INVERS) : 0);

      switch (i) {
        case ITEM_RECEIVER_TELEMETRY:
          reusableBuffer.hardwareAndSettings.receiverSettings.telemetryDisabled = editCheckBox(reusableBuffer.hardwareAndSettings.receiverSettings.telemetryDisabled, RECEIVER_OPTIONS_2ND_COLUMN, y, "Telem. disabled", attr, event);
          if (attr && checkIncDec_Ret) {
            reusableBuffer.hardwareAndSettings.receiverSettings.dirty = true;
          }
          break;

        case ITEM_RECEIVER_PWM_RATE:
          reusableBuffer.hardwareAndSettings.receiverSettings.pwmRate = editCheckBox(reusableBuffer.hardwareAndSettings.receiverSettings.pwmRate, RECEIVER_OPTIONS_2ND_COLUMN, y, isModuleR9M2(g_moduleIdx) ? "6.67ms PWM": "9ms PWM", attr, event);
          if (attr && checkIncDec_Ret) {
            reusableBuffer.hardwareAndSettings.receiverSettings.dirty = true;
          }
          break;

        default:
          // Pin
        {
          uint8_t pin = i - ITEM_RECEIVER_PINMAP_FIRST;
          if (pin < reusableBuffer.hardwareAndSettings.receiverSettings.outputsCount) {
            uint8_t & mapping = reusableBuffer.hardwareAndSettings.receiverSettings.outputsMapping[pin];
            uint8_t channel = g_model.moduleData[g_moduleIdx].channelsStart + mapping;
            int32_t channelValue = channelOutputs[channel];
            lcdDrawText(0, y, STR_PIN);
            lcdDrawNumber(lcdLastRightPos + 1, y, pin + 1);
            putsChn(7 * FW, y, channel + 1, attr);

            // Channel
            if (attr) {
              mapping = checkIncDec(event, mapping, 0, sentModuleChannels(g_moduleIdx) - 1);
              if (checkIncDec_Ret) {
                reusableBuffer.hardwareAndSettings.receiverSettings.dirty = true;
              }
            }

            // Bargraph
#if !defined(PCBX7) // X7 LCD doesn't like too many horizontal lines
            lcdDrawRect(LCD_W - 3 - wbar, y + 1, wbar + 1, 4);
#endif
            const uint8_t lenChannel = limit<uint8_t>(1, (abs(channelValue) * wbar / 2 + lim / 2) / lim, wbar / 2);
            const coord_t xChannel = (channelValue > 0) ? LCD_W - 3 - wbar / 2 : LCD_W - 2 - wbar / 2 - lenChannel;
            lcdDrawHorizontalLine(xChannel, y + 2, lenChannel, SOLID, 0);
            lcdDrawHorizontalLine(xChannel, y + 3, lenChannel, SOLID, 0);
          }
          break;
        }
      }
    }
  }
  else {
    lcdDrawCenteredText(LCD_H/2, STR_WAITING_FOR_RX);
  }
}
