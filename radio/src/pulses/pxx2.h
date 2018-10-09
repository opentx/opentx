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

#ifndef _PULSES_PXX2_H_
#define _PULSES_PXX2_H_

#include "./pxx.h"

class Pxx2Transport: public DataBuffer<uint8_t, 64>, public PxxCrcMixin {
  protected:
    void addByte(uint8_t byte)
    {
      PxxCrcMixin::addToCrc(byte);
      *ptr++ = byte;
    }

    void addTail()
    {
      // nothing
    }
};

class Pxx2Pulses: public PxxPulses<Pxx2Transport> {
  public:
    void setupFrame(uint8_t port);

  protected:
    uint8_t data[64];
    uint8_t * ptr;

    void initFrame()
    {
      Pxx2Transport::initBuffer();
    }
};

#endif
