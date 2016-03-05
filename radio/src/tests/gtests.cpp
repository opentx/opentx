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
  menuLevel = 0;
  menuHandlers[0] = menuMainView;
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
