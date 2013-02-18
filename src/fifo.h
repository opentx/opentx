/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef fifo_h
#define fifo_h

class Fifo32
{
  public:
    Fifo32():
      widx(0),
      ridx(0)
    {
    }

    void push(uint8_t byte) {
      uint32_t next = (widx+1) & 0x1f;
      if (next != ridx) {
        fifo[widx] = byte;
        widx = next;
      }
    }

    bool pop(uint8_t & byte) {
      if (ridx == widx) {
        return false;
      }
      else {
        byte = fifo[ridx];
        ridx = (ridx+1) & 0x1f;
        return true;
      }
    }

  protected:
    uint8_t fifo[32];
    volatile uint32_t widx;
    volatile uint32_t ridx;
};

#endif
