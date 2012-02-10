/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
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

#include "open9x.h"

uint8_t g_beepCnt;
uint8_t beepAgain = 0;
uint8_t beepAgainOrig = 0;
uint8_t beepOn = false;
bool warble = false;
bool warbleC;
#if defined(HAPTIC)
uint8_t hapticTick = 0;
#endif

// The various "beep" tone lengths
static const pm_uint8_t beepTab[] PROGMEM = {
    // 0   1   2   3    4
    0,  1,  8, 30, 100, //silent
    1,  1,  8, 30, 100, //xShort
    1,  1,  8, 30, 100, //short
    1,  1,  8, 30, 100, //normal
    1,  1, 15, 50, 150, //long
    10, 10, 30, 50, 150, //xLong
};

void beep(uint8_t val)
{
  if (g_eeGeneral.beeperVal>0) {
    _beep(pgm_read_byte(beepTab+5*g_eeGeneral.beeperVal-5+val));
  }
}
