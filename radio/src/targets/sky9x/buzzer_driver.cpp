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

#include "../opentx.h"

volatile uint8_t buzzerCount ;

#if defined(REVA)
void buzzerOn()
{
  PIOA->PIO_SODR = 0x00010000L ;        // Set bit A16 ON
}

void buzzerOff()
{
  PIOA->PIO_CODR = 0x00010000L ;        // Set bit A16 ON
}
#else
void buzzerOn()
{
  PIOA->PIO_SODR = 0x02000000L ;        // Set bit A25 ON
}

void buzzerOff()
{
  PIOA->PIO_CODR = 0x02000000L ;        // Set bit A25 ON
}
#endif

void buzzerSound(uint8_t duration)
{
  buzzerOn();
  buzzerCount = duration;
}

void buzzerHeartbeat()
{
  if (buzzerCount) {
    if (--buzzerCount == 0)
      buzzerOff();
  }
}
