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

#include "board_horus.h"

void delaysInit(void)
{
  // Timer13
  RCC->APB1ENR |= RCC_APB1ENR_TIM13EN;           // Enable clock
  TIM13->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 10000000 - 1;      // 0.1uS 
  TIM13->CCER = 0;
  TIM13->CCMR1 = 0;
  TIM13->CR1 = 0x02;
  TIM13->DIER = 0;
}
	
void delay_01us(uint16_t nb)
{
  TIM13->EGR = 1;
  TIM13->CNT = 0;
  TIM13->CR1 = 0x03;
  while (TIM13->CNT < nb);
  TIM13->CR1 = 0x02;
}
