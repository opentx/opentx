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

#ifndef _IO_PXX2_H_
#define _IO_PXX2_H_

#include "fifo.h"

#define PXX2_BAUDRATE            230400
#define PXX2_PERIOD              4 // 4ms

class ModuleFifo : public Fifo<uint8_t, 32> {
  public:
    bool getFrame(uint8_t * frame)
    {
      while (1) {
        if (isEmpty()) {
          return false;
        }
        else if (fifo[ridx] != 0x7E) {
          skip();
        }
        else {
          break;
        }
      }

      uint32_t next = nextIndex(ridx);
      uint8_t len = fifo[next];
      if (unsigned(len + 4 /* 2 bytes header + 2 bytes CRC */) > size()) {
        return false;
      }

      for (uint32_t i=0; i<=len; i++) {
        frame[i] = fifo[next];
        next = nextIndex(next);
      }

      // TODO CRC CHECK
      next = nextIndex(next);
      ridx = nextIndex(next);
      return true;
    }

    uint32_t errors;
};

#endif
