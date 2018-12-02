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

bool checkPXX2PacketCRC(const uint8_t * packet)
{
  // TODO ...

  return true;
  // TRACE("checkPXX2PacketCRC(): checksum error ");
  // DUMP(packet, FRSKY_SPORT_PACKET_SIZE);
}

void processFrskyPXX2Data(uint8_t data)
{
  static uint8_t dataState = STATE_DATA_IDLE;

  switch (dataState) {
    case STATE_DATA_IDLE:
      if (data == START_STOP) {
        telemetryRxBufferCount = 0;
        dataState = STATE_DATA_START;
      }
      break;

    case STATE_DATA_START:
      if (telemetryRxBufferCount == 0 && data > 0x0C) {
        // wrong length
        telemetryRxBufferCount = 0;
        dataState = STATE_DATA_IDLE;
      }
      else if (telemetryRxBufferCount < TELEMETRY_RX_PACKET_SIZE) {
        telemetryRxBuffer[telemetryRxBufferCount++] = data;
        if (telemetryRxBuffer[0] + 3 /* 1 byte for length, 2 bytes for CRC */ == telemetryRxBufferCount) {
          if (checkPXX2PacketCRC(telemetryRxBuffer)) {
            if (telemetryRxBuffer[2] & 0x80) {
              telemetryData.rssi.set(telemetryRxBuffer[2] & 0x7f);
            }
            else {
              // TODO : vbat handling
            }
            sportProcessTelemetryPacketWithoutCrc(telemetryRxBuffer + 6 /* LEN, TYPE, RSSI/BAT, TP/SS/FW_T, FW_VER, Data ID */);
          }
          telemetryRxBufferCount = 0;
          dataState = STATE_DATA_IDLE;
        }
      }
      else {
        // overflow guard
        telemetryRxBufferCount = 0;
        dataState = STATE_DATA_IDLE;
      }
      break;
  }
}

void processFrskyTelemetryData(uint8_t data)
{
  static uint8_t dataState = STATE_DATA_IDLE;

#if defined(PCBSKY9X) && defined(BLUETOOTH)
  // TODO if (g_model.bt_telemetry)
  btPushByte(data);
#endif

#if defined(SERIAL2)
  if (g_eeGeneral.serial2Mode == UART_MODE_TELEMETRY_MIRROR) {
    serial2Putc(data);
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

