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

#include "opentx.h"

Fifo<32> sbusFifo;
uint8_t SbusFrame[28] ;
uint16_t SbusTimer ;
uint8_t SbusIndex = 0 ;

void processSbusFrame(uint8_t *sbus, int16_t *pulses, uint32_t size)
{
  uint32_t inputbitsavailable = 0;
  uint32_t inputbits = 0;

  if (*sbus++ != 0x0F) {
    return; // not a valid SBUS frame
  }

  if (size < 23) {
    return;
  }

  for (uint32_t i=0; i<NUM_TRAINER; i++) {
    while (inputbitsavailable < 11) {
      inputbits |= *sbus++ << inputbitsavailable;
      inputbitsavailable += 8;
    }
    *pulses++ = ((int32_t) (inputbits & 0x7FF) - 0x3E0) * 5 / 8;
    inputbitsavailable -= 11;
    inputbits >>= 11;
  }

  ppmInValid = PPM_IN_VALID_TIMEOUT;
}

#define SBUS_DELAY 1000 // 500uS
void processSbusInput()
{
  uint8_t rxchar;
  uint32_t active = 0;
  while (sbusFifo.pop(rxchar)) {
    active = 1;
    SbusFrame[SbusIndex++] = rxchar;
    if (SbusIndex > 27) {
      SbusIndex = 27;
    }
  }
  if (active) {
    SbusTimer = getTmr2MHz();
    return;
  }
  else {
    if (SbusIndex) {
      if ((uint16_t) (getTmr2MHz() - SbusTimer) > SBUS_DELAY) {
        processSbusFrame(SbusFrame, g_ppmIns, SbusIndex);
        SbusIndex = 0;
      }
    }
  }
}
