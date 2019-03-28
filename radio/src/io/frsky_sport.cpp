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

#warning "This need to be adapted"

/*void sportOutputPushByte(uint8_t byte)
{
  if (byte == 0x7E || byte == 0x7D) {
    outputTelemetryBuffer.push(0x7D);
    outputTelemetryBuffer.push(0x20 ^ byte);
  }
  else {
    outputTelemetryBuffer.push(byte);
  }
}

void sportOutputPushPacket(SportTelemetryPacket * packet)
{
  uint16_t crc = 0;

  for (uint8_t i=1; i<sizeof(SportTelemetryPacket); i++) {
    uint8_t byte = packet->raw[i];
    sportOutputPushByte(byte);
    crc += byte; // 0-1FF
    crc += crc >> 8; // 0-100
    crc &= 0x00ff;
  }

  outputTelemetryBuffer.push(0xFF - crc);
  outputTelemetryBuffer.setTrigger(packet->raw[0]); // physicalId
  outputTelemetryBuffer.setDestination(SPORT_MODULE);
}
*/