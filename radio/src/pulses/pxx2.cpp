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
    if (moduleSettings[module].counter == 0) {
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

bool Pxx2Pulses::setupRegisterFrame(uint8_t module)
{
  addFrameType(PXX2_TYPE_C_MODULE, PXX2_TYPE_ID_REGISTER);

  if (reusableBuffer.modelSetup.pxx2.registerStep == REGISTER_RX_NAME_SELECTED) {
    Pxx2Transport::addByte(0x01);
    for (uint8_t i=0; i<PXX2_LEN_RX_NAME; i++) {
      Pxx2Transport::addByte(reusableBuffer.modelSetup.pxx2.registerRxName[i]);
    }
    for (uint8_t i=0; i<PXX2_LEN_REGISTRATION_ID; i++) {
      Pxx2Transport::addByte(g_model.modelRegistrationID[i]);
    }
  }
  else {
    Pxx2Transport::addByte(0);
  }

  return true; // TODO not always
}

bool Pxx2Pulses::setupBindFrame(uint8_t module)
{
  addFrameType(PXX2_TYPE_C_MODULE, PXX2_TYPE_ID_BIND);

  if (reusableBuffer.modelSetup.pxx2.bindStep == BIND_WAIT) {
    if (get_tmr10ms() > reusableBuffer.modelSetup.pxx2.bindWaitTimeout) {
      moduleSettings[module].mode = MODULE_MODE_NORMAL;
      POPUP_INFORMATION(STR_BIND_OK);
    }
    else {
      return false;
    }
  }
  else if (reusableBuffer.modelSetup.pxx2.bindStep == BIND_RX_NAME_SELECTED) {
    Pxx2Transport::addByte(0x01);
    for (uint8_t i=0; i<PXX2_LEN_RX_NAME; i++) {
      Pxx2Transport::addByte(reusableBuffer.modelSetup.pxx2.bindCandidateReceiversIds[reusableBuffer.modelSetup.pxx2.bindSelectedReceiverIndex][i]);
    }
    Pxx2Transport::addByte(g_model.header.modelId[INTERNAL_MODULE]);
  }
  else {
    Pxx2Transport::addByte(0x00);
    for (uint8_t i=0; i<PXX2_LEN_REGISTRATION_ID; i++) {
      Pxx2Transport::addByte(g_model.modelRegistrationID[i]);
    }
  }

  return true; // TODO not always
}

bool Pxx2Pulses::setupSpectrumAnalyser(uint8_t module)
{
  if (moduleSettings[module].counter > 1000) {
    moduleSettings[module].counter = 1002;
    return false;
  }

  moduleSettings[module].counter = 1002;

  addFrameType(PXX2_TYPE_C_POWER_METER, PXX2_TYPE_ID_SPECTRUM);
  Pxx2Transport::addByte(0x00);

  reusableBuffer.spectrum.fq = 2440000000;  // 2440MHz
  Pxx2Transport::addWord(reusableBuffer.spectrum.fq);

  reusableBuffer.spectrum.span = 40000000;  // 40MHz
  Pxx2Transport::addWord(reusableBuffer.spectrum.span);

  reusableBuffer.spectrum.step = 100000;  // 100KHz
  Pxx2Transport::addWord(reusableBuffer.spectrum.step);

  return true;
}

bool Pxx2Pulses::setupFrame(uint8_t module)
{
  initFrame();

  bool result = true;
  uint8_t mode = moduleSettings[module].mode;

  if (mode == MODULE_MODE_REGISTER)
    result = setupRegisterFrame(module);
  else if (mode == MODULE_MODE_BIND)
    result = setupBindFrame(module);
  else if (mode == MODULE_MODE_SPECTRUM_ANALYSER)
    result = setupSpectrumAnalyser(module);
  else
    setupChannelsFrame(module);

  if (moduleSettings[module].counter-- == 0) {
    moduleSettings[module].counter = 1000;
  }

  endFrame();

  return result;
}

template class PxxPulses<Pxx2Transport>;
