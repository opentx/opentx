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

#ifndef _PULSES_COMMON_H_
#define _PULSES_COMMON_H_

#include <inttypes.h>

#if defined(PCBX12S) && PCBREV < 13
  #define pulse_duration_t             uint32_t
  #define trainer_pulse_duration_t     uint16_t
#else
  #define pulse_duration_t             uint16_t
  #define trainer_pulse_duration_t     uint16_t
#endif

template <class T, int SIZE>
class DataBuffer {
  public:
    const T * getData()
    {
      return data;
    }

    uint8_t getSize()
    {
      return ptr - data;
    }

  protected:
    T data[SIZE];
    T * ptr;

    void initBuffer()
    {
      ptr = data;
    }
};

template <class T, int SIZE>
class PulsesBuffer: public DataBuffer<T, SIZE> {
  public:
    T getLast() {
      return *(DataBuffer<T, SIZE>::ptr - 1);
    };

    void setLast(T value) {
      *(DataBuffer<T, SIZE>::ptr - 1) = value;
    }
};

#endif
