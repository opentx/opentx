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

#include "opentx.h"

void backlightInit()
{
  // PIN init
  GPIO_InitTypeDef GPIO_InitStructure;
  if (IS_HORUS_PROD()) {
    GPIO_InitStructure.GPIO_Pin = PROD_BL_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(BL_GPIO, &GPIO_InitStructure);
    GPIO_PinAFConfig(BL_GPIO, PROD_BL_GPIO_PinSource, PROD_BL_GPIO_AF);
  }
  else {
    GPIO_InitStructure.GPIO_Pin = BETA_BL_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(BL_GPIO, &GPIO_InitStructure);
    GPIO_PinAFConfig(BL_GPIO, BETA_BL_GPIO_PinSource, BETA_BL_GPIO_AF);
  }
  
  // TIMER init
  if (IS_HORUS_PROD()) {
    PROD_BL_TIMER->ARR = 100;
    PROD_BL_TIMER->PSC = PROD_BL_TIMER_FREQ / 10000 - 1; // 1kHz
    PROD_BL_TIMER->CCMR2 = TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2; // PWM
    PROD_BL_TIMER->CCER = TIM_CCER_CC4E;
    PROD_BL_TIMER->CCR4 = 0;
    PROD_BL_TIMER->EGR = 0;
    PROD_BL_TIMER->CR1 = TIM_CR1_CEN; // Counter enable
  }
  else {
    BETA_BL_TIMER->ARR = 100;
    BETA_BL_TIMER->PSC = BETA_BL_TIMER_FREQ / 10000 - 1; // 1kHz
    BETA_BL_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // PWM
    BETA_BL_TIMER->CCER = TIM_CCER_CC1E | TIM_CCER_CC1NE;
    BETA_BL_TIMER->CCR1 = 100;
    BETA_BL_TIMER->EGR = 1;
    BETA_BL_TIMER->CR1 |= TIM_CR1_CEN; // Counter enable
    BETA_BL_TIMER->BDTR |= TIM_BDTR_MOE;
  }
}

void backlightEnable(uint8_t dutyCycle)
{
  if (dutyCycle < 5) {
    dutyCycle = 5;
  }
  
  if (IS_HORUS_PROD()) {
    PROD_BL_TIMER->CCR4 = dutyCycle;
  }
  else {
    BETA_BL_TIMER->CCR1 = (100 - dutyCycle);
  }
}
