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

#include <QtGui/QApplication>
#include "gtests.h"

int32_t lastAct = 0;
uint16_t anaInValues[NUM_STICKS+NUM_POTS] = { 0 };
uint16_t anaIn(uint8_t chan)
{
  if (chan < NUM_STICKS+NUM_POTS)
    return anaInValues[chan];
  else
    return 0;
}

static char _zchar2stringResult[200];
const char * zchar2string(const char * zstring, int size)
{
  if (size > (int)sizeof(_zchar2stringResult) ) {
    return 0;
  }
  zchar2str(_zchar2stringResult, zstring, size);
  return _zchar2stringResult;
}

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  simuInit();
  StartEepromThread(NULL);
  g_menuStackPtr = 0;
  g_menuStack[0] = menuMainView;
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
