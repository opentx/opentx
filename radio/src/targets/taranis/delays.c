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

#include "board_taranis.h"

void delaysInit(void)
{
  // Timer13
  RCC->APB1ENR |= RCC_APB1ENR_TIM13EN;           // Enable clock
  TIM13->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 10000000 - 1;      // 0.1uS 
  TIM13->ARR = 0xFFFF;
  TIM13->EGR = TIM_EGR_UG;    // generate update event
}

void delay_01us(uint16_t nb)
{
  TIM13->EGR = TIM_EGR_UG;    // generate update event (this also resets counter)
  TIM13->SR &= ~TIM_SR_CC1IF; // clear CC flag
  TIM13->CCR1 = nb;           // set CC value
  TIM13->CR1 |= TIM_CR1_CEN;  // start timer
  while((TIM13->SR & TIM_SR_CC1IF) == 0);
  TIM13->CR1 &= ~TIM_CR1_CEN; // stop timer
}

void delay_ms(uint32_t ms)
{
  while(ms--) {
    delay_01us(10000);
  }
}
