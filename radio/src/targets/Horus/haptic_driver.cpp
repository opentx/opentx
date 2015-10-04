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
  HAPTIC_GPIO_TIMER->CCR1 = 0;
}

void hapticOn(uint32_t pwmPercent)
{
  if (pwmPercent > 100) {
    pwmPercent = 100;
  }
  HAPTIC_GPIO_TIMER->CCR1 = pwmPercent;
}

void hapticInit(void)
{
  RCC_AHB1PeriphClockCmd(HAPTIC_RCC_AHB1Periph_GPIO, ENABLE);
  RCC_APB2PeriphClockCmd(HAPTIC_RCC_APB2Periph_TIMER, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = HAPTIC_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(HAPTIC_GPIO, &GPIO_InitStructure);

  GPIO_PinAFConfig(HAPTIC_GPIO, HAPTIC_GPIO_PinSource, HAPTIC_GPIO_AF);

  HAPTIC_GPIO_TIMER->ARR = 100 ;
  HAPTIC_GPIO_TIMER->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 10000 - 1;
  HAPTIC_GPIO_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 ;    // PWM
  HAPTIC_GPIO_TIMER->CCER = TIM_CCER_CC1E ;

  HAPTIC_GPIO_TIMER->CCR1 = 0 ;
  HAPTIC_GPIO_TIMER->EGR = 0 ;
  HAPTIC_GPIO_TIMER->CR1 = TIM_CR1_CEN ;              // Counter enable
}
