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

#include "../../opentx.h"

void hapticOff(void)
{
#if defined(REVPLUS)
  TIM10->CCR1 = 0;
#else
  GPIO_ResetBits(GPIO_HAPTIC, PIN_HAPTIC);
#endif
}

#if defined(REVPLUS)
void hapticOn(uint32_t pwmPercent)
{
  if (pwmPercent > 100) {
    pwmPercent = 100;
  }
  TIM10->CCR1 = pwmPercent;
}
#else
void hapticOn()
{
  // No PWM before REVPLUS
  GPIO_SetBits(GPIO_HAPTIC, PIN_HAPTIC);
}
#endif

void hapticInit(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOHAPTIC, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = PIN_HAPTIC;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_HAPTIC, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIO_HAPTIC, GPIO_PinSource_HAPTIC, GPIO_AF_TIM10);

  RCC->APB2ENR |= RCC_APB2ENR_TIM10EN ;       // Enable clock
  TIM10->ARR = 100 ;
  TIM10->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 10000 - 1;
  TIM10->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 ;    // PWM
  TIM10->CCER = TIM_CCER_CC1E ;

  TIM10->CCR1 = 0 ;
  TIM10->EGR = 0 ;
  TIM10->CR1 = TIM_CR1_CEN ;              // Counter enable
}
