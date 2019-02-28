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

#include "pulses_common.h"

#define PXX_SEND_BIND                      0x01
#define PXX_SEND_FAILSAFE                  (1 << 4)
#define PXX_SEND_RANGECHECK                (1 << 5)

#if defined(PXX_FREQUENCY_HIGH)
  #define EXTMODULE_USART_PXX_BAUDRATE     450000
  #define INTMODULE_USART_PXX_BAUDRATE     450000
  #define PXX_PERIOD                       4/*ms*/
#else
  #define EXTMODULE_USART_PXX_BAUDRATE     115200
  #define INTMODULE_USART_PXX_BAUDRATE     115200
  #define PXX_PERIOD                       9/*ms*/
#endif

#if defined(PXX_FREQUENCY_HIGH) && (!defined(INTMODULE_USART) || !defined(EXTMODULE_USART))
/* PXX uses 20 bytes (as of Rev 1.1 document) with 8 changes per byte + stop bit ~= 162 max pulses */
/* DSM2 uses 2 header + 12 channel bytes, with max 10 changes (8n2) per byte + 16 bits trailer ~= 156 max pulses */
/* Multimodule uses 3 bytes header + 22 channel bytes with max 11 changes per byte (8e2) + 16 bits trailer ~= 291 max pulses */
/* Multimodule reuses some of the DSM2 function and structs since the protocols are similar enough */
/* sbus is 1 byte header, 22 channel bytes (11bit * 16ch) + 1 byte flags */

#error "Pulses array needs to be increased (PXX_FREQUENCY=HIGH)"
#endif


#define PXX_PERIOD_HALF_US                 (PXX_PERIOD * 2000)

// Used by the Sky9x family boards
class SerialPxxBitTransport: public DataBuffer<uint8_t, 64> {
  protected:
    uint8_t byte;
    uint8_t bits_count;

    void initFrame()
    {
      initBuffer();
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

class PwmPxxBitTransport: public PulsesBuffer<pulse_duration_t, 200> {
  protected:
    uint16_t rest;

    void initFrame()
    {
      initBuffer();
      rest = PXX_PERIOD_HALF_US;
    }

    void addPart(uint8_t value)
    {
      pulse_duration_t duration = value ? 47 : 31;
      *ptr++ = duration;
      rest -= duration + 1;
    }

    void addTail()
    {
      // rest min value is 18000 - 200 * 48 = 8400 (4.2ms)
      *(ptr - 1) += rest;
    }
};

template <class PxxTransport>
class PxxPulses: public PxxTransport {
  protected:
    void addChannels(uint8_t port, uint8_t sendFailsafe, uint8_t sendUpperChannels);
};

#endif
