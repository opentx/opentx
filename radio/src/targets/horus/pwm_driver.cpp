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

#define TIMESAMPLE_COUNT 6

uint8_t analogs_pwm_disabled = 0;
volatile uint32_t pwm_interrupt_count;
volatile uint8_t  timer_capture_states[4];
volatile uint32_t timer_capture_rising_time[4];
volatile uint32_t timer_capture_values[4][TIMESAMPLE_COUNT];
volatile uint8_t  timer_capture_indexes[4];

void pwmInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_ICInitTypeDef TIM5_ICInitStructure;

  GPIO_InitStructure.GPIO_Pin = PWM_GPIOA_PINS;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM5);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM5);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM5);

  TIM_TimeBaseStructure.TIM_Period = 0xffff;
  TIM_TimeBaseStructure.TIM_Prescaler = 80;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM5_ICInitStructure.TIM_ICFilter = 0x00;

  TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1;
  TIM_ICInit(TIM5, &TIM5_ICInitStructure);

  TIM5_ICInitStructure.TIM_Channel = TIM_Channel_2;
  TIM_ICInit(TIM5, &TIM5_ICInitStructure);

  TIM5_ICInitStructure.TIM_Channel = TIM_Channel_3;
  TIM_ICInit(TIM5, &TIM5_ICInitStructure);

  TIM5_ICInitStructure.TIM_Channel = TIM_Channel_4;
  TIM_ICInit(TIM5, &TIM5_ICInitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_Cmd(TIM5, ENABLE);

  TIM_ITConfig(TIM5, TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4, ENABLE);
}

inline uint32_t TIM_GetCaptureN(uint8_t n)
{
  switch (n) {
    case 0:
      return TIM_GetCapture1(TIM5);
    case 1:
      return TIM_GetCapture2(TIM5);
    case 2:
      return TIM_GetCapture3(TIM5);
    case 3:
      return TIM_GetCapture4(TIM5);
    default:
      return 0;
  }
}

inline void TIM_OCPolarityConfig(uint8_t n, uint16_t TIM_OCPolarity)
{
  switch (n) {
    case 0:
      TIM_OC1PolarityConfig(TIM5, TIM_OCPolarity);
      break;
    case 1:
      TIM_OC2PolarityConfig(TIM5, TIM_OCPolarity);
      break;
    case 2:
      TIM_OC3PolarityConfig(TIM5, TIM_OCPolarity);
      break;
    case 3:
      TIM_OC4PolarityConfig(TIM5, TIM_OCPolarity);
      break;
  }
}

inline void TIM_ClearITPendingBitN(uint8_t n)
{
  switch (n) {
    case 0:
      TIM_ClearITPendingBit(TIM5, TIM_IT_CC1);
      break;
    case 1:
      TIM_ClearITPendingBit(TIM5, TIM_IT_CC2);
      break;
    case 2:
      TIM_ClearITPendingBit(TIM5, TIM_IT_CC3);
      break;
    case 3:
      TIM_ClearITPendingBit(TIM5, TIM_IT_CC4);
      break;
  }
}

inline uint32_t diff_with_16bits_overflow(uint32_t a, uint32_t b)
{
  if (b > a)
    return b - a;
  else
    return b + 0xffff - a;
}

extern "C" void TIM5_IRQHandler(void)
{
  for (int i=0; i<4; i++) {
    if (TIM_GetITStatus(TIM5, TIM_IT_CC1<<i) != RESET) {
      uint32_t capture = TIM_GetCaptureN(i);
      if (timer_capture_states[i] != 0) {
        uint32_t value = diff_with_16bits_overflow(timer_capture_rising_time[i], capture);
        if (value < 10000) {
          pwm_interrupt_count++;
          timer_capture_values[i][timer_capture_indexes[i]++] = value;
          timer_capture_indexes[i] %= TIMESAMPLE_COUNT;
        }
        TIM_OCPolarityConfig(i, TIM_ICPolarity_Rising);
        timer_capture_states[i] = 0;
      }
      else {
        timer_capture_rising_time[i] = capture;
        TIM_OCPolarityConfig(i, TIM_ICPolarity_Falling);
        timer_capture_states[i] = 0x80;
      }
      TIM_ClearITPendingBitN(i);
    }
  }
}

void pwmRead(uint16_t * values)
{
  uint32_t tmp[4];

  for (int i=0; i<4; i++) {
    uint32_t mycount = 0;
    uint32_t mymax = 0;
    uint32_t mymin = 4095;
    for (uint32_t j=0; j<TIMESAMPLE_COUNT; j++) {
      uint32_t value = timer_capture_values[i][j];
      mycount += value;
      if (value > mymax)
        mymax = value;
      if (value < mymin)
        mymin = value;
    }
    // from the 6 values, remove the min and max and divide by 4
    tmp[i] = (mycount - mymax - mymin) >> 2;
  }

  values[0] = tmp[2];
  values[1] = tmp[3];
  values[2] = tmp[1];
  values[3] = tmp[0];
}

void pwmCheck()
{
  // I have ~1860 interrupts with only one stick
  if (pwm_interrupt_count < 1000) {
    analogs_pwm_disabled = true;
    adcInit();
  }
}
