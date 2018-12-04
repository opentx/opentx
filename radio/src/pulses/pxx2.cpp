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
#include "pulses/pxx2.h"

void Pxx2Pulses::setupFrame(uint8_t port)
{
  initFrame();

  static uint8_t pass[NUM_MODULES] = { MODULES_INIT(0) };
  uint8_t sendUpperChannels = 0;
  if (pass[port]++ & 0x01) {
    sendUpperChannels = g_model.moduleData[port].channelsCount;
  }

  // Model ID
  addByte(g_model.header.modelId[port]);

  // Flag1
  uint8_t flag1 = addFlag1(port);

  // Flag2 = Extra flags
  addExtraFlags(port);

  // Channels
  addChannels(port, flag1 & PXX_SEND_FAILSAFE, sendUpperChannels);

#if defined(LUA)
  if (outputTelemetryBufferTrigger != 0x00 && outputTelemetryBufferSize > 0) {
    TRACE("SENDING TELEM");
    // primID (1 byte) + dataID (2 bytes) + value (4 bytes)
    for (uint8_t i=0; i<7; i++) {
      addByte(outputTelemetryBuffer[i]);
    }
    outputTelemetryBufferTrigger = 0x00;
    outputTelemetryBufferSize = 0;
  }
#endif

  endFrame();
}
