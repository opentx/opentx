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
void Pxx1Pulses<PxxTransport>::add8ChannelsFrame(uint8_t port, uint8_t sendUpperChannels)
{
  PxxTransport::initCrc();

  // Sync
  addHead();

  // RX Number
  PxxTransport::addByte(g_model.header.modelId[port]);

  // Flag1
  uint8_t flag1 = PxxPulses<PxxTransport>::addFlag1(port);

  // Flag2
  PxxTransport::addByte(0);

  // Channels
  PxxPulses<PxxTransport>::addChannels(port, flag1 & PXX_SEND_FAILSAFE, sendUpperChannels);

  // Extra flags
  PxxPulses<PxxTransport>::addExtraFlags(port);

  // CRC
  addCrc();

  // Sync = HEAD
  addHead();

  // Tail
  PxxTransport::addTail();
}

template <class PxxTransport>
void Pxx1Pulses<PxxTransport>::setupFrame(uint8_t port)
{
  PxxTransport::initFrame();

#if defined(PXX_FREQUENCY_HIGH)
  add8ChannelsFrame(port, 0);
  if (sentModuleChannels(port) > 8) {
    add8ChannelsFrame(port, 8);
  }
#else
  static uint8_t pass[NUM_MODULES] = { MODULES_INIT(0) };
  uint8_t sendUpperChannels = 0;
  if (pass[port]++ & 0x01) {
    sendUpperChannels = g_model.moduleData[port].channelsCount;
  }
  add8ChannelsFrame(port, sendUpperChannels);
#endif
}

template class Pxx1Pulses<StandardPxxTransport<PwmPxxBitTransport> >;
template class Pxx1Pulses<StandardPxxTransport<SerialPxxBitTransport> >;
template class Pxx1Pulses<UartPxxTransport>;
