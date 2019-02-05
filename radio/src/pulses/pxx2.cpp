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

uint8_t Pxx2Pulses::addFlag0(uint8_t module)
{
  uint8_t flag0 = g_model.header.modelId[module] & 0x3F;
  if (g_model.moduleData[module].failsafeMode != FAILSAFE_NOT_SET && g_model.moduleData[module].failsafeMode != FAILSAFE_RECEIVER) {
    if (moduleSettings[module].counter-- == 0) {
      moduleSettings[module].counter = 1000;
      flag0 |= PXX2_FLAG0_FAILSAFE;
    }
  }
  Pxx2Transport::addByte(flag0);
  return flag0;
}

void Pxx2Pulses::addFlag1(uint8_t module)
{
  uint8_t flag1 = 0;
  Pxx2Transport::addByte(flag1);
}

void Pxx2Pulses::setupChannelsFrame(uint8_t module)
{
  addFrameType(PXX2_TYPE_C_MODULE, PXX2_TYPE_ID_CHANNELS);

  // FLAG0
  uint8_t flag0 = addFlag0(module);

  // FLAG1
  addFlag1(module);

  // Channels
  addChannels(module, flag0 & PXX2_FLAG0_FAILSAFE, 0);
  addChannels(module, flag0 & PXX2_FLAG0_FAILSAFE, 1);
}

void Pxx2Pulses::setupRegisterFrame(uint8_t module)
{
  unsigned counter = moduleSettings[module].counter;

  addFrameType(PXX2_TYPE_C_MODULE, PXX2_TYPE_ID_REGISTER);

  if (counter == REGISTER_COUNTER_ID_RECEIVED) {
    Pxx2Transport::addByte(0x01);
    for (uint8_t i=0; i<LEN_REGISTRATION_ID; i++) {
      Pxx2Transport::addByte(g_model.modelRegistrationID[i]);
    }
  }
  else {
    Pxx2Transport::addByte(0);
  }
}

void Pxx2Pulses::setupBindFrame(uint8_t module)
{
  addFrameType(PXX2_TYPE_C_MODULE, PXX2_TYPE_ID_BIND);
}

void Pxx2Pulses::setupFrame(uint8_t module)
{
  initFrame();

  uint8_t mode = moduleSettings[module].mode;

  if (mode == MODULE_MODE_REGISTER)
    setupRegisterFrame(module);
  else if (mode == MODULE_MODE_BIND)
    setupBindFrame(module);
  else
    setupChannelsFrame(module);

#if 0
  // TODO PXX15
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
