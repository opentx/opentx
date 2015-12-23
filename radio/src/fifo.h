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

#ifndef _FIFO_H_
#define _FIFO_H_

template <int N>
class Fifo
{
  public:
    Fifo():
      widx(0),
      ridx(0)
    {
    }

    void clear()
    {
      widx = ridx = 0;
    }

    void push(uint8_t byte) {
      uint32_t next = (widx+1) & (N-1);
      if (next != ridx) {
        fifo[widx] = byte;
        widx = next;
      }
    }

    bool pop(uint8_t & byte) {
      if (isEmpty()) {
        return false;
      }
      else {
        byte = fifo[ridx];
        ridx = (ridx+1) & (N-1);
        return true;
      }
    }

    bool isEmpty() {
      return (ridx == widx);
    }

    bool isFull() {
      uint32_t next = (widx+1) & (N-1);
      return (next == ridx);
    }

    void flush() {
      while (!isEmpty()) {};
    }

  protected:
    uint8_t fifo[N];
    volatile uint32_t widx;
    volatile uint32_t ridx;
};

#endif // _FIFO_H_
