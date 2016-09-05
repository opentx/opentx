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

#ifndef _IO_ARM_H_
#define _IO_ARM_H_

#include "dataconstants.h"

#if defined(TELEMETRY_FRSKY_SPORT)
PACK(union SportTelemetryPacket
{
  struct {
    uint8_t physicalId;
    uint8_t primId;
    uint16_t dataId;
    uint32_t value;
  };
  uint8_t raw[8];
});

void sportProcessPacket(uint8_t * packet);
bool isSportOutputBufferAvailable();
void sportOutputPushPacket(SportTelemetryPacket * packet);
void sportFlashDevice(ModuleIndex module, const char * filename);
#endif

#if defined(STM32)
bool isBootloader(const char * filename);
void bootloaderFlash(const char * filename);
#endif
  
#endif // _IO_ARM_H_
