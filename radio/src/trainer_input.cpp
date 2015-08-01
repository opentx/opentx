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

#include "trainer_input.h"

int16_t ppmInput[NUM_TRAINER];
uint8_t ppmInputValidityTimer;


#if defined(CPUARM)
#include "audio_arm.h"

void checkTrainerSignalWarning()
{
  enum PpmInValidState_t {
    PPM_IN_IS_NOT_USED=0,
    PPM_IN_IS_VALID,
    PPM_IN_INVALID
  };

  static uint8_t ppmInputValidState = PPM_IN_IS_NOT_USED;

  if(ppmInputValidityTimer && (ppmInputValidState == PPM_IN_IS_NOT_USED)) {
    ppmInputValidState = PPM_IN_IS_VALID;
  }
  else if (!ppmInputValidityTimer && (ppmInputValidState == PPM_IN_IS_VALID)) {
    ppmInputValidState = PPM_IN_INVALID;
    AUDIO_TRAINER_LOST();
  }
  else if (ppmInputValidityTimer && (ppmInputValidState == PPM_IN_INVALID)) {
    ppmInputValidState = PPM_IN_IS_VALID;
    AUDIO_TRAINER_BACK();
  }
}

#endif
