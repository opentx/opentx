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

uint16_t Analog_values[NUMBER_ANALOG];

// Settings for mode register ADC_MR
// USEQ off - silicon problem, doesn't work
// TRANSFER = 1
// TRACKTIM = 4 (5 clock periods)
// ANACH = 0
// SETTLING = 1 (not used if ANACH = 0)
// STARTUP = 1 (8 clock periods)
// PRESCAL = 3.6 MHz clock (between 1 and 20MHz)
// FREERUN = 0
// FWUP = 0
// SLEEP = 0
// LOWRES = 0
// TRGSEL = 0
// TRGEN = 0 (software trigger only)
void adcInit()
{
  register Adc *padc ;
  register uint32_t timer ;

  timer = ( Master_frequency / (3600000*2) ) << 8 ;
  // Enable peripheral clock ADC = bit 29
  PMC->PMC_PCER0 |= 0x20000000L ;               // Enable peripheral clock to ADC
  padc = ADC ;
  padc->ADC_MR = 0x14110000 | timer ;  // 0001 0100 0001 0001 xxxx xxxx 0000 0000
  padc->ADC_ACR = ADC_ACR_TSON ;                        // Turn on temp sensor
#if defined(REVA)
  padc->ADC_CHER = 0x0000E23E ;  // channels 1,2,3,4,5,9,13,14,15
#else
  padc->ADC_CHER = 0x0000E33E ;  // channels 1,2,3,4,5,8,9,13,14,15
#endif
  padc->ADC_CGR = 0 ;  // Gain = 1, all channels
  padc->ADC_COR = 0 ;  // Single ended, 0 offset, all channels
}

// Read 8 (9 for REVB) ADC channels
// Documented bug, must do them 1 by 1
void adcRead()
{
  register Adc *padc;
  register uint32_t y;
  register uint32_t x;

  padc = ADC;
  y = padc->ADC_ISR; // Clear EOC flags
  for (y = NUMBER_ANALOG+1; --y > 0;) {
    padc->ADC_CR = 2; // Start conversion
    x = 0;
    while ((padc->ADC_ISR & 0x01000000) == 0) {
      // wait for DRDY flag
      if (++x > 1000000) {
        break; // Software timeout
      }
    }
    x = padc->ADC_LCDR; // Clear DRSY flag
  }
  // Next bit may be done using the PDC
  Analog_values[0] = ADC->ADC_CDR1;
  Analog_values[1] = ADC->ADC_CDR2;
  Analog_values[2] = ADC->ADC_CDR3;
  Analog_values[3] = ADC->ADC_CDR4;
  Analog_values[4] = ADC->ADC_CDR5;
  Analog_values[5] = ADC->ADC_CDR9;
  Analog_values[6] = ADC->ADC_CDR13;
  Analog_values[7] = ADC->ADC_CDR14;

#if !defined(REVA)
  Analog_values[8] = ADC->ADC_CDR8 ;
#endif

  temperature = (((int32_t)temperature * 7) + ((((int32_t)ADC->ADC_CDR15 - 838) * 621) >> 11)) >> 3; // Filter it
  if (get_tmr10ms() >= 100 && temperature > maxTemperature) {
    maxTemperature = temperature;
  }
}
