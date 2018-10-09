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

#ifndef _PULSES_PXX_H_
#define _PULSES_PXX_H_

#include <inttypes.h>

#define PXX_SEND_BIND                      0x01
#define PXX_SEND_FAILSAFE                  (1 << 4)
#define PXX_SEND_RANGECHECK                (1 << 5)

class PxxCrcMixin {
  protected:
    void initCrc()
    {
      crc = 0;
    }

    void addToCrc(uint8_t byte)
    {
      crc = (crc << 8) ^ (CRCTable[((crc >> 8) ^ byte) & 0xFF]);
    }

    uint8_t crc;
    static const uint16_t CRCTable[];
};


class SerialPxxBitTransport {
  protected:
    uint8_t data[64];
    uint8_t * ptr;
    uint8_t byte;
    uint8_t bits_count;
    uint8_t padding[2];

    void initFrame()
    {
      ptr = data;
      byte = 0;
      bits_count = 0;
    }

    void addSerialBit(uint8_t bit)
    {
      byte >>= 1;
      if (bit & 1) {
        byte |= 0x80;
      }
      if (++bits_count >= 8) {
        *ptr++ = byte;
        bits_count = 0;
      }
    }

    // 8uS/bit 01 = 0, 001 = 1
    void addPart(uint8_t value)
    {
      addSerialBit(0);
      if (value) {
        addSerialBit(0);
      }
      addSerialBit(1);
    }

    void addTail()
    {
      while (bits_count != 0) {
        addSerialBit(1);
      }
    }
};

// TODO elsewhere !!!

#if defined(PCBX12S) && PCBREV < 13
  #define pulse_duration_t             uint32_t
  #define trainer_pulse_duration_t     uint16_t
#else
  #define pulse_duration_t             uint16_t
  #define trainer_pulse_duration_t     uint16_t
#endif

#define PXX_PERIOD_HALF_US            (PXX_PERIOD * 2000)

class PwmPxxBitTransport {
  protected:
    pulse_duration_t pulses[200];
    pulse_duration_t * ptr;
    uint16_t rest;
    uint8_t padding[2];

    void initFrame()
    {
      ptr = pulses;
      rest = PXX_PERIOD_HALF_US;
    }

    void addPart(uint8_t value)
    {
      pulse_duration_t duration = value ? 47 : 31;
      *ptr++ = duration;
      rest -= duration + 1;
    }

    void addTail(uint8_t port)
    {
      // rest min value is 18000 - 200 * 48 = 8400 (4.2ms)
      *(ptr - 1) += rest;
    }
};

template <class BitTransport>
class StandardPxxTransport: public PxxCrcMixin, public BitTransport {
  protected:
    uint8_t ones_count;
    uint8_t padding[3];

    void addByte(uint8_t byte)
    {
      PxxCrcMixin::addToCrc(byte);
      addByteWithoutCrc(byte);
    };

    void addByteWithoutCrc(uint8_t byte)
    {
      for (uint8_t i = 0; i < 8; i++) {
        addBit(byte & 0x80);
        byte <<= 1;
      }
    }

    void addBit(uint8_t bit)
    {
      if (bit) {
        BitTransport::addPart(1);
        if (++ones_count == 5) {
          ones_count = 0;
          BitTransport::addPart(0); // Stuff a 0 bit in
        }
      }
      else {
        BitTransport::addPart(0);
        ones_count = 0;
      }
    }

    void initFrame()
    {
      ones_count = 0;
    }
};

class UartPxxTransport: public PxxCrcMixin {
  protected:
    uint8_t  data[64];
    uint8_t  * ptr;

    void initFrame()
    {
      ptr = data;
    }

    void addByte(uint8_t byte)
    {
      PxxCrcMixin::addToCrc(byte);
      addWithByteStuffing(byte);
    }

    void addWithByteStuffing(uint8_t byte)
    {
      if (0x7E == byte) {
        *ptr++ = 0x7D;
        *ptr++ = 0x5E;
      }
      else if (0x7D == byte) {
        *ptr++ = 0x7D;
        *ptr++ = 0x5D;
      }
      else {
        *ptr++ = byte;
      }
    }

    void addTail()
    {
      // nothing
    }
};

template <class PxxTransport>
class PxxPulses: public PxxTransport {
  protected:
    uint8_t addFlag1(uint8_t port);
    void addChannels(uint8_t port, uint8_t sendFailsafe, uint8_t sendUpperChannels);
    void addExtraFlags(uint8_t port);
};

typedef PxxPulses<UartPxxTransport> UartPxxPulses;

template <class PxxTransport>
class StandardPxxPulses: public PxxPulses<PxxTransport>
{
  public:
    void setupFrame(uint8_t port);

  protected:
    void addHead()
    {
      // send 7E, do not CRC
      PxxTransport::addByteWithoutCrc(0x7E);
    }

    void addCrc()
    {
      addByteWithoutCrc(PxxCrcMixin::crc >> 8);
      addByteWithoutCrc(PxxCrcMixin::crc);
    }

    void setup8ChannelsFrame(uint8_t port, uint8_t sendUpperChannels);
};

typedef StandardPxxPulses<StandardPxxTransport<PwmPxxBitTransport>> PwmPxxPxxPulses;


#endif
