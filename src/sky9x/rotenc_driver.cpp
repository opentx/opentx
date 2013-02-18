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

#include "../open9x.h"

void rotencInit()
{
  configure_pins( PIO_PC19 | PIO_PC21, PIN_ENABLE | PIN_INPUT | PIN_PORTC | PIN_PULLUP ) ;        // 19 and 21 are rotary encoder
  configure_pins( PIO_PB6, PIN_ENABLE | PIN_INPUT | PIN_PORTB | PIN_PULLUP ) ;            // rotary encoder switch
  PIOC->PIO_IER = PIO_PC19 | PIO_PC21 ;
  NVIC_EnableIRQ(PIOC_IRQn) ;
}

void rotencEnd()
{
  NVIC_DisableIRQ(PIOC_IRQn) ;
  PIOC->PIO_IDR = PIO_PC19 | PIO_PC21 ;
}

volatile uint32_t Rotary_position ;
extern "C" void PIOC_IRQHandler()
{
  register uint32_t dummy;

  dummy = PIOC->PIO_ISR ;                 // Read and clear status register
  (void) dummy ;                          // Discard value - prevents compiler warning

  dummy = PIOC->PIO_PDSR ;                // Read Rotary encoder (PC19, PC21)
  dummy >>= 19 ;
  dummy &= 0x05 ;                 // pick out the three bits
  if ( dummy != ( Rotary_position & 0x05 ) )
  {
    if ( ( Rotary_position & 0x01 ) ^ ( ( dummy & 0x04) >> 2 ) )
    {
      incRotaryEncoder(0, +1);
    }
    else
    {
      incRotaryEncoder(0, -1);
    }
    Rotary_position &= ~0x45 ;
    Rotary_position |= dummy ;
  }
}
