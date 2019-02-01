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

#ifndef _PULSES_PXX1_H_
#define _PULSES_PXX1_H_

#include "pxx.h"

template <class PxxTransport>
class Pxx1Pulses: public PxxPulses<PxxTransport>
{
  public:
    void setupFrame(uint8_t port);

  protected:
    void addHead()
    {
      // send 7E, do not CRC
      PxxTransport::addRawByte(0x7E);
    }

    void addCrc()
    {
      PxxTransport::addByteWithoutCrc(PxxCrcMixin::crc >> 8);
      PxxTransport::addByteWithoutCrc(PxxCrcMixin::crc);
    }

    void add8ChannelsFrame(uint8_t port, uint8_t sendUpperChannels);
};

typedef Pxx1Pulses<UartPxxTransport> UartPxxPulses;
typedef Pxx1Pulses<StandardPxxTransport<PwmPxxBitTransport>> PwmPxxPulses;
typedef Pxx1Pulses<StandardPxxTransport<SerialPxxBitTransport>> SerialPxxPulses;

#endif
