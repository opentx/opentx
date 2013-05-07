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

void hapticOff()
{
  PWM->PWM_DIS = PWM_DIS_CHID2 ;                                          // Disable channel 2
  PWM->PWM_OOV &= ~0x00040000 ;   // Force low
  PWM->PWM_OSS |= 0x00040000 ;    // Force low
}

// pwmPercent 0-100
void hapticOn( uint32_t pwmPercent )
{
  register Pwm *pwmptr ;

  pwmptr = PWM ;

  if (pwmPercent > 100)
    pwmPercent = 100 ;

  pwmptr->PWM_CH_NUM[2].PWM_CDTYUPD = pwmPercent ;                // Duty
  pwmptr->PWM_ENA = PWM_ENA_CHID2 ;                                               // Enable channel 2
  pwmptr->PWM_OSC = 0x00040000 ;  // Enable output
}
