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

void processFrskyTelemetryData(uint8_t data)
{
  static uint8_t dataState = STATE_DATA_IDLE;

#if defined(PCBSKY9X) && defined(BLUETOOTH)
  // TODO if (g_model.bt_telemetry)
  btPushByte(data);
#endif

#if defined(AUX_SERIAL)
  if (g_eeGeneral.auxSerialMode == UART_MODE_TELEMETRY_MIRROR) {
    auxSerialPutc(data);
  }
#endif

#if defined(BLUETOOTH)
  if (g_eeGeneral.bluetoothMode == BLUETOOTH_TELEMETRY && bluetoothState == BLUETOOTH_STATE_CONNECTED) {
    bluetoothForwardTelemetry(data);
  }
#endif

  switch (dataState) {
    case STATE_DATA_START:
      if (data == START_STOP) {
        if (IS_FRSKY_SPORT_PROTOCOL()) {
          dataState = STATE_DATA_IN_FRAME ;
          telemetryRxBufferCount = 0;
        }
      }
      else {
        if (telemetryRxBufferCount < TELEMETRY_RX_PACKET_SIZE) {
          telemetryRxBuffer[telemetryRxBufferCount++] = data;
        }
        dataState = STATE_DATA_IN_FRAME;
      }
      break;

    case STATE_DATA_IN_FRAME:
      if (data == BYTESTUFF) {
        dataState = STATE_DATA_XOR; // XOR next byte
      }
      else if (data == START_STOP) {
        if (IS_FRSKY_SPORT_PROTOCOL()) {
          dataState = STATE_DATA_IN_FRAME ;
          telemetryRxBufferCount = 0;
        }
        else {
          // end of frame detected
          frskyDProcessPacket(telemetryRxBuffer);
          dataState = STATE_DATA_IDLE;
        }
        break;
      }
      else if (telemetryRxBufferCount < TELEMETRY_RX_PACKET_SIZE) {
        telemetryRxBuffer[telemetryRxBufferCount++] = data;
      }
      break;

    case STATE_DATA_XOR:
      if (telemetryRxBufferCount < TELEMETRY_RX_PACKET_SIZE) {
        telemetryRxBuffer[telemetryRxBufferCount++] = data ^ STUFF_MASK;
      }
      dataState = STATE_DATA_IN_FRAME;
      break;

    case STATE_DATA_IDLE:
      if (data == START_STOP) {
        telemetryRxBufferCount = 0;
        dataState = STATE_DATA_START;
      }
      break;

  } // switch

#if defined(TELEMETRY_FRSKY_SPORT)
  if (IS_FRSKY_SPORT_PROTOCOL() && telemetryRxBufferCount >= FRSKY_SPORT_PACKET_SIZE) {
    sportProcessPacket(telemetryRxBuffer);
    dataState = STATE_DATA_IDLE;
  }
#endif
}

