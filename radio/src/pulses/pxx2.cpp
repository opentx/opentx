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

uint8_t Pxx2Pulses::addFlag1(uint8_t module)
{
  uint8_t flag1 = g_model.header.modelId[module] & 0x3F;
  if (g_model.moduleData[module].failsafeMode != FAILSAFE_NOT_SET && g_model.moduleData[module].failsafeMode != FAILSAFE_RECEIVER) {
    if (failsafeCounter[module]-- == 0) {
      failsafeCounter[module] = 1000;
      flag1 |= PXX2_FLAG1_FAILSAFE;
    }
  }
  Pxx2Transport::addByte(flag1);
  return flag1;
}

void Pxx2Pulses::setupFrame(uint8_t module)
{
  initFrame();

  addFrameType(PXX2_TYPE_C_MODULE, PXX2_TYPE_ID_CHANNELS);

  // Model ID
  // TODO addByte(g_model.header.modelId[port]);

  // Flag1
  uint8_t flag1 = addFlag1(module);

  // Flag2 = Extra flags
  // TODO addExtraFlags(module);

  // Channels
  addChannels(module, flag1 & PXX2_FLAG1_FAILSAFE, 0);
  addChannels(module, flag1 & PXX2_FLAG1_FAILSAFE, 1);

#if 0
  if (outputTelemetryBufferTrigger != 0x7E && outputTelemetryBufferSize > 0) {
    // primID (1 byte) + dataID (2 bytes) + value (4 bytes)
    addByte(outputTelemetryBufferTrigger);
    for (uint8_t i=0; i<7; i++) {
      addByte(outputTelemetryBuffer[i]);
    }
    outputTelemetryBufferTrigger = 0x00;
    outputTelemetryBufferSize = 0;
  }
#endif

  endFrame();
}
