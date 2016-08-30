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
}

void backlightEnable(uint8_t dutyCycle)
{
  static uint8_t existingDutyCycle;
  
  if (dutyCycle == existingDutyCycle || dutyCycle < 5) {
    return;
  }
  else {
    existingDutyCycle = dutyCycle;
  }
  
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / 10000 - 1; // 1KhZ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 100;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  if (IS_HORUS_PROD()) {
    TIM_TimeBaseInit(PROD_BL_TIMER, &TIM_TimeBaseStructure);
    TIM_Cmd(PROD_BL_TIMER, DISABLE);
  }
  else {
    TIM_TimeBaseInit(BETA_BL_TIMER, &TIM_TimeBaseStructure);
    TIM_Cmd(BETA_BL_TIMER, DISABLE);
  }
  
  /* PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  if (IS_HORUS_PROD())
    TIM_OCInitStructure.TIM_Pulse = dutyCycle;
  else
    TIM_OCInitStructure.TIM_Pulse = (100 - dutyCycle);

  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
  if (IS_HORUS_PROD()) {
    TIM_OC4Init(PROD_BL_TIMER, &TIM_OCInitStructure);
    TIM_Cmd(PROD_BL_TIMER, ENABLE);
    TIM_CtrlPWMOutputs(PROD_BL_TIMER, ENABLE);
  }
  else {
    TIM_OC1Init(BETA_BL_TIMER, &TIM_OCInitStructure);
    TIM_Cmd(BETA_BL_TIMER, ENABLE);
    TIM_CtrlPWMOutputs(BETA_BL_TIMER, ENABLE);
  }
}
