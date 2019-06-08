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

uint32_t rotencPosition;

void rotaryEncoderInit()
{
  SYSCFG_EXTILineConfig(ROTARY_ENCODER_EXTI_PortSource, ROTARY_ENCODER_EXTI_PinSource1);

#if defined(ROTARY_ENCODER_EXTI_LINE2)
  SYSCFG_EXTILineConfig(ROTARY_ENCODER_EXTI_PortSource, ROTARY_ENCODER_EXTI_PinSource2);
#endif

  EXTI_InitTypeDef EXTI_InitStructure;
  EXTI_StructInit(&EXTI_InitStructure);
  EXTI_InitStructure.EXTI_Line = ROTARY_ENCODER_EXTI_LINE1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

#if defined(ROTARY_ENCODER_EXTI_LINE2)
  EXTI_InitStructure.EXTI_Line = ROTARY_ENCODER_EXTI_LINE2;
  EXTI_Init(&EXTI_InitStructure);
#endif

  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = ROTARY_ENCODER_EXTI_IRQn1;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; /* Not used as 4 bits are used for the pre-emption priority. */;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

#if defined(ROTARY_ENCODER_EXTI_IRQn2)
  NVIC_InitStructure.NVIC_IRQChannel = ROTARY_ENCODER_EXTI_IRQn2;
  NVIC_Init(&NVIC_InitStructure);
#endif

  rotencPosition = ROTARY_ENCODER_POSITION();
}

void rotaryEncoderCheck()
{
  uint32_t newpos = ROTARY_ENCODER_POSITION();
  if (newpos != rotencPosition && !keyState(KEY_ENTER)) {
    if ((rotencPosition & 0x01) ^ ((newpos & 0x02) >> 1)) {
      --rotencValue;
    }
    else {
      ++rotencValue;
    }
    rotencPosition = newpos;
#if !defined(BOOT)
    if (g_eeGeneral.backlightMode & e_backlight_mode_keys) {
      backlightOn();
    }
#endif
  }
}

extern "C" void ROTARY_ENCODER_EXTI_IRQHandler1(void)
{
  if (EXTI_GetITStatus(ROTARY_ENCODER_EXTI_LINE1) != RESET) {
    rotaryEncoderCheck();
    EXTI_ClearITPendingBit(ROTARY_ENCODER_EXTI_LINE1);
  }

#if !defined(ROTARY_ENCODER_EXTI_IRQn2)
  if (EXTI_GetITStatus(ROTARY_ENCODER_EXTI_LINE2) != RESET) {
    rotaryEncoderCheck();
    EXTI_ClearITPendingBit(ROTARY_ENCODER_EXTI_LINE2);
  }
#endif

#if !defined(BOOT) && defined(INTMODULE_HEARTBEAT_REUSE_INTERRUPT_ROTARY_ENCODER)
  check_xjt_heartbeat();
#endif
}

#if defined(ROTARY_ENCODER_EXTI_IRQn2)
extern "C" void ROTARY_ENCODER_EXTI_IRQHandler2(void)
{
  if (EXTI_GetITStatus(ROTARY_ENCODER_EXTI_LINE2) != RESET) {
    rotaryEncoderCheck();
    EXTI_ClearITPendingBit(ROTARY_ENCODER_EXTI_LINE2);
  }
}
#endif
