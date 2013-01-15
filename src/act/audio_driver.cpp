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

volatile uint8_t Buzzer_count ;

// Must NOT be in flash, PDC needs a RAM source.
// Amplitude reduced to 30% to allow for voice volume
uint16_t Sine_values[] =
{
  2048,2085,2123,2160,2197,2233,2268,2303,2336,2369,
  2400,2430,2458,2485,2510,2533,2554,2573,2590,2605,
  2618,2629,2637,2643,2646,2648,2646,2643,2637,2629,
  2618,2605,2590,2573,2554,2533,2510,2485,2458,2430,
  2400,2369,2336,2303,2268,2233,2197,2160,2123,2085,
  2048,2010,1972,1935,1898,1862,1826,1792,1758,1726,
  1695,1665,1637,1610,1585,1562,1541,1522,1505,1490,
  1477,1466,1458,1452,1448,1448,1448,1452,1458,1466,
  1477,1490,1505,1522,1541,1562,1585,1610,1637,1665,
  1695,1726,1758,1792,1826,1862,1898,1935,1972,2010
};

// Sound routines
void startSound()
{
}

uint32_t currentFrequency = 0;

uint32_t getFrequency()
{
  return currentFrequency;
}

void setFrequency(uint32_t frequency)
{
  if (currentFrequency != frequency) {
    currentFrequency = frequency;
  }
}

// Start TIMER1 at 100000Hz, used for DACC trigger
void start_timer1()
{
}


// Configure DAC0 (or DAC1 for REVA)
// Not sure why PB14 has not be allocated to the DAC, although it is an EXTRA function
// So maybe it is automatically done
void initDac()
{
}

void end_sound()
{
}

// Set up for volume control (TWI0)
// Need PA3 and PA4 set to peripheral A
void init_twi()
{
}

void setVolume( unsigned char volume )
{
}
