/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
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

#include "board_horus.h"
#include "ff.h"

FIL eepromFile = {0};
#define EEPROM_EMULATION_FILENAME   "/eeprom.bin"

void eepromEmulationInit()
{
  f_open(&eepromFile, EEPROM_EMULATION_FILENAME, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
}

void eepromEmulationStop()
{
  f_close(&eepromFile);
}


void eepromReadBlock(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
  f_lseek(&eepromFile, ReadAddr);
  UINT* bytesRead;
  f_read(&eepromFile, pBuffer, NumByteToRead, &bytesRead); 
}

void eepromWriteBlock(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
  f_lseek(&eepromFile, WriteAddr);
  UINT* bytesWritten;
  f_write(&eepromFile, pBuffer, NumByteToWrite, &bytesWritten); 
}

