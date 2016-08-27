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

uint16_t eeprom_pointer;
uint8_t * eeprom_buffer_data;
volatile int8_t eeprom_buffer_size = 0;

inline void eeprom_write_byte()
{
  EEAR = eeprom_pointer;
  EEDR = *eeprom_buffer_data;
// TODO add some define here
#if defined (CPUM2560) || defined(CPUM2561)
  EECR |= 1<<EEMPE;
  EECR |= 1<<EEPE;
#else
  EECR |= 1<<EEMWE;
  EECR |= 1<<EEWE;
#endif
  eeprom_pointer++;
  eeprom_buffer_data++;
}

ISR(EE_READY_vect)
{
  if (--eeprom_buffer_size > 0) {
    eeprom_write_byte();
  }
  else {
#if defined(CPUM2560) || defined(CPUM2561)
    EECR &= ~(1<<EERIE);
#else
    EECR &= ~(1<<EERIE);
#endif
  }
}

void eepromStartWrite(uint8_t * buffer, size_t address, size_t size)
{
  assert(eeprom_buffer_size == 0);

  eeprom_pointer = address;
  eeprom_buffer_data = buffer;
  eeprom_buffer_size = size+1;

#if defined (CPUM2560) || defined(CPUM2561)
  EECR |= (1<<EERIE);
#else
  EECR |= (1<<EERIE);
#endif
}

uint8_t eepromIsTransferComplete()
{
  return eeprom_buffer_size == 0;
}

void eepromWriteBlock(uint8_t * buffer, size_t address, size_t size)
{
  eepromStartWrite(buffer, address, size);

  if (s_sync_write) {
    while (!eepromIsTransferComplete()) {
      wdt_reset();
    }
  }
}
