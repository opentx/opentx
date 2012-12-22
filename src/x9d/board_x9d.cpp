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

#include "open9x.h"

uint16_t Analog_values[NUMBER_ANALOG] ;
uint8_t temperature = 0;          // Raw temp reading
uint8_t maxTemperature = 0 ;       // Raw temp reading
volatile uint32_t Tenms ; // TODO to remove everywhere / use a #define

void keysInit();
void pwrInit();

uint8_t getTemperature()
{
  return temperature + g_eeGeneral.temperatureCalib;
}

// TODO change this function!
void read_9_adc()
{
}

void start_ppm_capture()
{
}

void usbBootloader()
{
}

void usbMassStorage()
{
}

#if !defined(SIMU)
void boardInit()
{
  pwrInit();
  keysInit();
}
#endif


