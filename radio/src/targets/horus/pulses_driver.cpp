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

#include "opentx.h"

void intmoduleStop();
void intmodulePxxStart();
void intmoduleSerialStart(uint32_t baudrate, uint8_t rxEnable);

void extmoduleStop();
void extmodulePpmStart();
void extmodulePxxStart();
void extmodulePxx2Start();
void extmoduleSerialStart(uint32_t baudrate, uint32_t period_half_us, bool inverted);


void init_ppm(uint8_t module)
{
  if (module == EXTERNAL_MODULE) {
    extmodulePpmStart();
  }
}

void disable_ppm(uint8_t module)
{
  if (module == EXTERNAL_MODULE) {
    extmoduleStop();
  }
}

void init_pxx2(uint8_t module)
{
  if (module == INTERNAL_MODULE)
    intmoduleSerialStart(INTMODULE_PXX_BAUDRATE, true);
  else
    extmodulePxx2Start();
}

void disable_pxx2(uint8_t module)
{
  if (module == INTERNAL_MODULE)
    intmoduleStop();
  else
    extmoduleStop();
}

void init_pxx1_pulses(uint8_t module)
{
  if (module == INTERNAL_MODULE)
    intmodulePxxStart();
  else
    extmodulePxxStart();
}

void disable_pxx1_pulses(uint8_t module)
{
  if (module == INTERNAL_MODULE)
    intmoduleStop();
  else
    extmoduleStop();
}

#if defined(DSM2)
void init_serial(uint8_t module, uint32_t baudrate, uint32_t period_half_us, bool inverted)
{
  if (module == EXTERNAL_MODULE) {
    extmoduleSerialStart(baudrate, period_half_us, inverted);
  }
}

void disable_serial(uint8_t module)
{
  if (module == EXTERNAL_MODULE) {
    extmoduleStop();
  }
}
#endif

