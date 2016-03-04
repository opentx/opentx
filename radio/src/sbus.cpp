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
#include "sbus.h"

#define SBUS_FRAME_GAP_DELAY   1000 // 500uS

#define SBUS_START_BYTE        0x0F
#define SBUS_FLAGS_IDX         23
#define SBUS_FRAMELOST_BIT     2
#define SBUS_FAILSAFE_BIT      3

#define SBUS_CH_BITS          11
#define SBUS_CH_MASK          ((1<<SBUS_CH_BITS)-1)

#define SBUS_CH_CENTER        0x3E0

Fifo<uint8_t, 32> sbusFifo;
uint8_t SbusFrame[SBUS_MAX_FRAME_SIZE];
uint16_t SbusTimer ;
uint8_t SbusIndex = 0 ;

// Range for pulses (ppm input) is [-512:+512]
void processSbusFrame(uint8_t * sbus, int16_t * pulses, uint32_t size)
{
  uint32_t inputbitsavailable = 0;
  uint32_t inputbits = 0;

  if (sbus[0] != SBUS_START_BYTE) {
    return; // not a valid SBUS frame
  }

  if (size < SBUS_MIN_FRAME_SIZE) {
    return;
  }

  if (size > SBUS_FLAGS_IDX && ((sbus[SBUS_FLAGS_IDX] & (1<<SBUS_FAILSAFE_BIT)) || (sbus[SBUS_FLAGS_IDX] & (1<<SBUS_FRAMELOST_BIT)))) {
    return; // SBUS invalid frame or failsafe mode
  }

  // Skip start byte
  sbus++;

  for (uint32_t i=0; i<NUM_TRAINER; i++) {
    while (inputbitsavailable < SBUS_CH_BITS) {
      inputbits |= *sbus++ << inputbitsavailable;
      inputbitsavailable += 8;
    }
    *pulses++ = ((int32_t) (inputbits & SBUS_CH_MASK) - SBUS_CH_CENTER) * 5 / 8;
    inputbitsavailable -= SBUS_CH_BITS;
    inputbits >>= SBUS_CH_BITS;
  }

  ppmInputValidityTimer = PPM_IN_VALID_TIMEOUT;
}

void processSbusInput()
{
  uint8_t rxchar;
  uint32_t active = 0;
  while (sbusFifo.pop(rxchar)) {
    active = 1;
    SbusFrame[SbusIndex++] = rxchar;
    if (SbusIndex > SBUS_MAX_FRAME_SIZE-1) {
      SbusIndex = SBUS_MAX_FRAME_SIZE-1;
    }
  }
  if (active) {
    SbusTimer = getTmr2MHz();
    return;
  }
  else {
    if (SbusIndex) {
      if ((uint16_t) (getTmr2MHz() - SbusTimer) > SBUS_FRAME_GAP_DELAY) {
        processSbusFrame(SbusFrame, ppmInput, SbusIndex);
        SbusIndex = 0;
      }
    }
  }
}
