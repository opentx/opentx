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
#include "pulses/pxx1.h"

template <class PxxTransport>
uint8_t Pxx1Pulses<PxxTransport>::addFlag1(uint8_t module)
{
  uint8_t flag1 = (g_model.moduleData[module].rfProtocol << 6);
  if (moduleSettings[module].mode == MODULE_MODE_BIND) {
    flag1 |= (g_eeGeneral.countryCode << 1) | PXX_SEND_BIND;
  }
  else if (moduleSettings[module].mode == MODULE_MODE_RANGECHECK) {
    flag1 |= PXX_SEND_RANGECHECK;
  }
  else {
    bool failsafeNeeded = g_model.moduleData[module].failsafeMode != FAILSAFE_NOT_SET && g_model.moduleData[module].failsafeMode != FAILSAFE_RECEIVER;
    if (moduleSettings[module].failsafeCounter-- == 0) {
      // failsafeCounter is also used for knowing if the frame is odd / even
      moduleSettings[module].failsafeCounter = 1000;
      if (failsafeNeeded) {
        flag1 |= PXX_SEND_FAILSAFE;
      }
    }
    if (failsafeNeeded && moduleSettings[module].failsafeCounter == 0 && g_model.moduleData[module].channelsCount > 0) {
      flag1 |= PXX_SEND_FAILSAFE;
    }
  }
  PxxTransport::addByte(flag1);
  return flag1;
}

template <class PxxTransport>
void Pxx1Pulses<PxxTransport>::addExtraFlags(uint8_t module)
{
  // Ext. flag (holds antenna selection on Horus internal module, 0x00 otherwise)
  uint8_t extra_flags = 0;

#if defined(PCBHORUS) || defined(PCBXLITE)
  if (module == INTERNAL_MODULE) {
    extra_flags |= (g_model.moduleData[module].pxx.external_antenna << 0);
  }
#endif

  extra_flags |= (g_model.moduleData[module].pxx.receiver_telem_off << 1);
  extra_flags |= (g_model.moduleData[module].pxx.receiver_channel_9_16 << 2);
  if (isModuleR9M(module)) {
    extra_flags |= (min<uint8_t>(g_model.moduleData[module].pxx.power, isModuleR9M_FCC_VARIANT(module) ? (uint8_t)R9M_FCC_POWER_MAX : (uint8_t)R9M_LBT_POWER_MAX) << 3);
    if (isModuleR9M_EUPLUS(module))
      extra_flags |= (1 << 6);
  }

  // Disable S.PORT if internal module is active
  if (IS_TELEMETRY_INTERNAL_MODULE()) {
    extra_flags |= (1 << 5);
  }
  PxxTransport::addByte(extra_flags);
}

template <class PxxTransport>
void Pxx1Pulses<PxxTransport>::add8ChannelsFrame(uint8_t module, uint8_t sendUpperChannels)
{
  PxxTransport::initCrc();

  // Sync
  addHead();

  // RX Number
  PxxTransport::addByte(g_model.header.modelId[module]);

  // Flag1
  uint8_t flag1 = addFlag1(module);

  // Flag2
  PxxTransport::addByte(0);

  // Channels
  PxxPulses<PxxTransport>::addChannels(module, flag1 & PXX_SEND_FAILSAFE, sendUpperChannels);

  // Extra flags
  addExtraFlags(module);

  // CRC
  addCrc();

  // Sync = HEAD
  addHead();

  // Tail
  PxxTransport::addTail();
}

template <class PxxTransport>
void Pxx1Pulses<PxxTransport>::setupFrame(uint8_t module)
{
  PxxTransport::initFrame();

#if defined(PXX_FREQUENCY_HIGH)
  add8ChannelsFrame(module, 0);
  if (sentModuleChannels(module) > 8) {
    add8ChannelsFrame(module, 8);
  }
#else
  uint8_t sendUpperChannels = 0;
  if (moduleSettings[module].failsafeCounter & 0x01) {
    sendUpperChannels = g_model.moduleData[module].channelsCount;
  }
  add8ChannelsFrame(module, sendUpperChannels);
#endif
}

template class Pxx1Pulses<StandardPxxTransport<PwmPxxBitTransport> >;
template class Pxx1Pulses<StandardPxxTransport<SerialPxxBitTransport> >;
template class Pxx1Pulses<UartPxxTransport>;
