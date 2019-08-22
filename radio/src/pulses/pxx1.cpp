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
  uint8_t flag1 = (g_model.moduleData[module].subType << 6);
  if (moduleState[module].mode == MODULE_MODE_BIND) {
    flag1 |= (g_eeGeneral.countryCode << 1) | PXX_SEND_BIND;
  }
  else if (moduleState[module].mode == MODULE_MODE_RANGECHECK) {
    flag1 |= PXX_SEND_RANGECHECK;
  }
  else {
    bool failsafeNeeded = g_model.moduleData[module].failsafeMode != FAILSAFE_NOT_SET && g_model.moduleData[module].failsafeMode != FAILSAFE_RECEIVER;
    if (moduleState[module].counter-- == 0) {
      // counter is also used for knowing if the frame is odd / even
      moduleState[module].counter = 1000;
      if (failsafeNeeded) {
        flag1 |= PXX_SEND_FAILSAFE;
      }
    }
    if (failsafeNeeded && moduleState[module].counter == 0 && g_model.moduleData[module].channelsCount > 0) {
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
  uint8_t extraFlags = 0;

#if defined(PCBHORUS) || defined(PCBXLITE)
  if (module == INTERNAL_MODULE && isExternalAntennaEnabled()) {
    extraFlags |= (1 << 0);
  }
#endif

  extraFlags |= (g_model.moduleData[module].pxx.receiverTelemetryOff << 1);
  extraFlags |= (g_model.moduleData[module].pxx.receiverHigherChannels << 2);
  if (isModuleR9MNonAccess(module)) {
    extraFlags |= (min<uint8_t>(g_model.moduleData[module].pxx.power, isModuleR9M_FCC_VARIANT(module) ? (uint8_t)R9M_FCC_POWER_MAX : (uint8_t)R9M_LBT_POWER_MAX) << 3);
    if (isModuleR9M_EUPLUS(module))
      extraFlags |= (1 << 6);
  }

  // Disable S.PORT if internal module is active
  if (module == EXTERNAL_MODULE && isSportLineUsedByInternalModule()) {
    extraFlags |= (1 << 5);
  }
  PxxTransport::addByte(extraFlags);
}

template <class PxxTransport>
void Pxx1Pulses<PxxTransport>::addChannels(uint8_t port, uint8_t sendFailsafe, uint8_t sendUpperChannels)
{
  uint16_t pulseValue = 0;
  uint16_t pulseValueLow = 0;

  for (int8_t i=0; i<8; i++) {
    if (sendFailsafe) {
      if (g_model.moduleData[port].failsafeMode == FAILSAFE_HOLD) {
        pulseValue = (i < sendUpperChannels ? 4095 : 2047);
      }
      else if (g_model.moduleData[port].failsafeMode == FAILSAFE_NOPULSES) {
        pulseValue = (i < sendUpperChannels ? 2048 : 0);
      }
      else {
        if (i < sendUpperChannels) {
          int16_t failsafeValue = g_model.failsafeChannels[8+i];
          if (failsafeValue == FAILSAFE_CHANNEL_HOLD) {
            pulseValue = 4095;
          }
          else if (failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
            pulseValue = 2048;
          }
          else {
            failsafeValue += 2*PPM_CH_CENTER(8+g_model.moduleData[port].channelsStart+i) - 2*PPM_CENTER;
            pulseValue = limit(2049, (failsafeValue * 512 / 682) + 3072, 4094);
          }
        }
        else {
          int16_t failsafeValue = g_model.failsafeChannels[i];
          if (failsafeValue == FAILSAFE_CHANNEL_HOLD) {
            pulseValue = 2047;
          }
          else if (failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
            pulseValue = 0;
          }
          else {
            failsafeValue += 2*PPM_CH_CENTER(g_model.moduleData[port].channelsStart+i) - 2*PPM_CENTER;
            pulseValue = limit(1, (failsafeValue * 512 / 682) + 1024, 2046);
          }
        }
      }
    }
    else {
      if (i < sendUpperChannels) {
        int channel = 8 + g_model.moduleData[port].channelsStart + i;
        int value = channelOutputs[channel] + 2*PPM_CH_CENTER(channel) - 2*PPM_CENTER;
        pulseValue = limit(2049, (value * 512 / 682) + 3072, 4094);
      }
      else if (i < sentModuleChannels(port)) {
        int channel = g_model.moduleData[port].channelsStart + i;
        int value = channelOutputs[channel] + 2*PPM_CH_CENTER(channel) - 2*PPM_CENTER;
        pulseValue = limit(1, (value * 512 / 682) + 1024, 2046);
      }
      else {
        pulseValue = 1024;
      }
    }

    if (i & 1) {
      PxxTransport::addByte(pulseValueLow); // Low byte of channel
      PxxTransport::addByte(((pulseValueLow >> 8) & 0x0F) | (pulseValue << 4));  // 4 bits each from 2 channels
      PxxTransport::addByte(pulseValue >> 4);  // High byte of channel
    }
    else {
      pulseValueLow = pulseValue;
    }
  }
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
  addChannels(module, flag1 & PXX_SEND_FAILSAFE, sendUpperChannels);

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
  PxxTransport::initFrame(PXX_PULSES_PERIOD);

#if defined(PXX_FREQUENCY_HIGH)
  if (moduleState[module].protocol == PROTOCOL_CHANNELS_PXX1_SERIAL) {
    add8ChannelsFrame(module, 0);
    if (sentModuleChannels(module) > 8) {
      add8ChannelsFrame(module, 8);
    }
    return;
  }
#endif

  uint8_t sendUpperChannels = 0;
  if (moduleState[module].counter & 0x01) {
    sendUpperChannels = g_model.moduleData[module].channelsCount;
  }

  add8ChannelsFrame(module, sendUpperChannels);
}

template class Pxx1Pulses<StandardPxx1Transport<PwmPxxBitTransport> >;
template class Pxx1Pulses<StandardPxx1Transport<SerialPxxBitTransport> >;
template class Pxx1Pulses<UartPxx1Transport>;
