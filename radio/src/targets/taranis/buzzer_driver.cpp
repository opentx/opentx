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

volatile uint8_t buzzerCount;

void buzzerInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = BUZZER_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(BUZZER_GPIO, &GPIO_InitStructure);

  GPIO_PinAFConfig(BUZZER_GPIO, BUZZER_GPIO_PinSource, BUZZER_GPIO_AF);

  BUZZER_TIMER->ARR = 100;
  BUZZER_TIMER->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 250000 - 1; // 250kHz
  BUZZER_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // PWM
  BUZZER_TIMER->CCER = TIM_CCER_CC1E;

  BUZZER_TIMER->CCR1 = 0;
  BUZZER_TIMER->EGR = 0;
  BUZZER_TIMER->CR1 = TIM_CR1_CEN; // Counter enable
}

void buzzerOn()
{
  BUZZER_TIMER->CCR1 = 50;
}

void buzzerOff()
{
  BUZZER_TIMER->CCR1 = 0;
}

void buzzerSound(uint8_t duration)
{
  // They don't like short beeps
  if (duration < 2)
    duration = 2;

  buzzerOn();
  buzzerCount = duration;
}

void buzzerHeartbeat()
{
  if (buzzerCount) {
    if (--buzzerCount == 0)
      buzzerOff();
  }
}
