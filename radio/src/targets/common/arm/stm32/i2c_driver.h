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

#pragma once

#include "board.h"

class I2CDriver
{
  public:
    I2CDriver(I2C_TypeDef * i2c):
      i2c(i2c)
    {
    }

    void setOutputsLow(GPIO_TypeDef * i2cGpio, uint32_t sclPin, uint32_t sdaPin);
    void init(GPIO_TypeDef * i2cGpio, uint32_t sclPin, uint32_t sdaPin, uint32_t alternate, uint32_t timingr);

    void stop();
    void abort(bool error = true);
    bool waitIdle();
    bool waitEvent(uint32_t event, bool allowError = false);
    
    bool read(uint8_t addr, uint8_t * data, uint8_t len, bool allowNoAnswer = false);
    bool write(uint8_t addr, const uint8_t * data, uint8_t len);

  protected:
    I2C_TypeDef * i2c;
};

extern I2CDriver i2cDriver;

#if defined(EXTEND_I2C)
extern I2CDriver extendI2cDriver;
#endif
