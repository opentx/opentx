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

inline void init_soft_power()
{
}

uint32_t check_soft_power()
{
  return e_power_on;
}

// turn off soft power
void soft_power_off()
{
}

uint8_t getTemperature()
{
  return temperature + g_eeGeneral.temperatureCalib;
}

uint32_t keyState(EnumKeys enuk)
{
  register uint32_t a = GPIOB->IDR;

  register uint32_t xxx = 0;

  if (enuk < (int) DIM(keys)) return keys[enuk].state() ? 1 : 0;

  switch ((uint8_t) enuk) {
    case SW_SA0:
      xxx = ~a & (1<<31);
      break;
    case SW_SA2:
      xxx = a & (1<<31);
      break;

    case SW_SB0:
      xxx = !(a & ((1<<26) | (1<<27)));
      break;
    case SW_SB1:
      xxx = (a & (1<<27)) && (~a & (1<<26));
      break;
    case SW_SB2:
      xxx = (~a & (1<<27)) && (a & (1<<26));
      break;

    case SW_SC0:
      xxx = !(a & ((1<<22) | (1<<23)));
      break;
    case SW_SC1:
      xxx = (a & (1<<23)) && (~a & (1<<22));
      break;
    case SW_SC2:
      xxx = (~a & (1<<23)) && (a & (1<<22));
      break;

    case SW_SD0:
      xxx = !(a & ((1<<20) | (1<<21)));
      break;
    case SW_SD1:
      xxx = (a & (1<<21)) && (~a & (1<<20));
      break;
    case SW_SD2:
      xxx = (~a & (1<<21)) && (a & (1<<20));
      break;

    case SW_SE0:
      xxx = !(a & ((1<<18) | (1<<19)));
      break;
    case SW_SE1:
      xxx = (a & (1<<19)) && (~a & (1<<18));
      break;
    case SW_SE2:
      xxx = (~a & (1<<19)) && (a & (1<<18));
      break;

    case SW_SF0:
      xxx = !(a & ((1<<16) | (1<<17)));
      break;
    case SW_SF1:
      xxx = (a & (1<<17)) && (~a & (1<<16));
      break;
    case SW_SF2:
      xxx = (~a & (1<<17)) && (a & (1<<16));
      break;

    case SW_SG0:
      xxx = !(a & ((1<<14) | (1<<15)));
      break;
    case SW_SG1:
      xxx = (a & (1<<15)) && (~a & (1<<14));
      break;
    case SW_SG2:
      xxx = (~a & (1<<15)) && (a & (1<<14));
      break;

    case SW_SH0:
      xxx = ~a & (1<<13);
      break;
    case SW_SH2:
      xxx = a & (1<<13);
      break;

    default:
      break;
  }

  if (xxx) {
    return 1;
  }

  return 0;
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
void board_init()
{
  keysInit();
}
#endif


