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

volatile XjtHeartbeatCapture heartbeatCapture;

void init_xjt_heartbeat()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = HEARTBEAT_GPIO_PIN;
  GPIO_Init(HEARTBEAT_GPIO, &GPIO_InitStructure);

  SYSCFG_EXTILineConfig(HEARTBEAT_EXTI_PortSource, HEARTBEAT_EXTI_PinSource);

  EXTI_InitTypeDef EXTI_InitStructure;
  EXTI_StructInit(&EXTI_InitStructure);
  EXTI_InitStructure.EXTI_Line = HEARTBEAT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_SetPriority(HEARTBEAT_EXTI_IRQn, 0); // Highest priority interrupt
  NVIC_EnableIRQ(HEARTBEAT_EXTI_IRQn);
  heartbeatCapture.valid = true;
}

void stop_xjt_heartbeat()
{
  heartbeatCapture.valid = false;

#if !defined(HEARTBEAT_ROTARY_ENCODER_SAME_EXTI_IRQHandler)
  NVIC_DisableIRQ(HEARTBEAT_EXTI_IRQn);
#endif

  EXTI_InitTypeDef EXTI_InitStructure;
  EXTI_StructInit(&EXTI_InitStructure);
  EXTI_InitStructure.EXTI_Line = HEARTBEAT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = HEARTBEAT_TRIGGER;
  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
  EXTI_Init(&EXTI_InitStructure);
}

void check_xjt_heartbeat()
{
  if (EXTI_GetITStatus(HEARTBEAT_EXTI_LINE) != RESET) {
    heartbeatCapture.timestamp = TIMER_2MHz_TIMER->CNT;
    heartbeatCapture.count++;
    EXTI_ClearITPendingBit(HEARTBEAT_EXTI_LINE);
  }
}

#if !defined(HEARTBEAT_ROTARY_ENCODER_SAME_EXTI_IRQHandler)
extern "C" void HEARTBEAT_EXTI_IRQHandler()
{
  check_xjt_heartbeat();
}
#endif
