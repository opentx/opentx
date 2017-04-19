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

#include <OsConfig.h>
#include "board.h"
#if defined(STM32F2)
  #include "dwt.h"    // the old ST library that we use does not define DWT register for STM32F2xx
#endif

#define SYSTEM_TICKS_1US    ((CFG_CPU_FREQ + 500000)  / 1000000)      // number of system ticks in 1us
#define SYSTEM_TICKS_01US   ((CFG_CPU_FREQ + 5000000) / 10000000)     // number of system ticks in 0.1us (rounding needed for sys frequencies that are not multiple of 10MHz)

void delaysInit(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void delay_01us(uint16_t nb)
{
  volatile uint32_t dwtStart = DWT->CYCCNT;
  volatile uint32_t dwtTotal = (SYSTEM_TICKS_01US * nb) - 10;
  while ((DWT->CYCCNT - dwtStart) < dwtTotal);
}

void delay_us(uint16_t nb)
{
  volatile uint32_t dwtStart = DWT->CYCCNT;
  volatile uint32_t dwtTotal = (SYSTEM_TICKS_1US * nb) - 10;
  while ((DWT->CYCCNT - dwtStart) < dwtTotal);
}

void delay_ms(uint32_t ms)
{
  while (ms--) {
    delay_us(1000);
  }
}
