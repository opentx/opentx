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

#include "../../opentx.h"

void rotencInit()
{
   configure_pins( GPIO_Pin_12|GPIO_Pin_13, PIN_INPUT | PIN_PULLUP | PIN_PORTD ) ;
}

void rotencEnd()
{
}

uint32_t Rotary_position;
rotenc_t x9de_rotenc; 
void checkRotaryEncoder()
{
  register uint32_t dummy ;
   
   dummy = GPIOD->IDR ;   // Read Rotary encoder ( PE6, PE5 )
   dummy >>= 12 ;
   dummy &= 0x03 ;         // pick out the two bits
   if ( dummy != ( Rotary_position & 0x03 ) )
   {
      if ( ( Rotary_position & 0x01 ) ^ ( ( dummy & 0x02) >> 1 ) )
      {
         --x9de_rotenc;
      }
      else
      {
         ++x9de_rotenc;
      }
      Rotary_position &= ~0x03 ;
      Rotary_position |= dummy ;
   }
}
