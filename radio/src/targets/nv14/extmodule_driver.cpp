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

void extmoduleSendNextFrame();


void extmoduleStop()
{
  EXTERNAL_MODULE_OFF();

  NVIC_DisableIRQ(EXTMODULE_DMA_IRQn);
  NVIC_DisableIRQ(EXTMODULE_TIMER_IRQn);

  EXTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
  EXTMODULE_TIMER->DIER &= ~(TIM_DIER_CC2IE | TIM_DIER_UDE);
  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
}

void extmoduleNoneStart()
{
  EXTERNAL_MODULE_OFF();

  GPIO_PinAFConfig(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PinSource, 0);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_TX_GPIO, &GPIO_InitStructure);
  GPIO_SetBits(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PIN); // Set high

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  EXTMODULE_TIMER->ARR = 36000; // 18mS
  EXTMODULE_TIMER->CCR2 = 32000; // Update time
  EXTMODULE_TIMER->EGR = 1; // Restart
  EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF;
  EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE; // Enable this interrupt
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN;

  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn);
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}

void extmodulePpmStart()
{
  EXTERNAL_MODULE_ON();

  GPIO_PinAFConfig(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PinSource, EXTMODULE_TX_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_TX_GPIO, &GPIO_InitStructure);

  // PPM generation principle:
  //
  // Hardware timer in PWM mode is used for PPM generation
  // Output is OFF if CNT<CCR1(delay) and ON if bigger
  // CCR1 register defines duration of pulse length and is constant
  // AAR register defines duration of each pulse, it is
  // updated after every pulse in Update interrupt handler.
  // CCR2 register defines duration of no pulses (time between two pulse trains)
  // it is calculated every round to have PPM period constant.
  // CC2 interrupt is then used to setup new PPM values for the
  // next PPM pulses train.

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN; // Stop timer
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  EXTMODULE_TIMER->ARR = 45000;
#if defined(PCBX10) || PCBREV >= 13
  EXTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2; // PWM mode 1
  EXTMODULE_TIMER->BDTR = TIM_BDTR_MOE;
  EXTMODULE_TIMER->EGR = 1; // Reloads register values now
  EXTMODULE_TIMER->DIER = TIM_DIER_UDE; // Update DMA request
  EXTMODULE_TIMER->CR1 = TIM_CR1_CEN; // Start timer
#elif defined(PCBNV14)
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // PWM mode 1
  EXTMODULE_TIMER->BDTR = TIM_BDTR_MOE;
  EXTMODULE_TIMER->EGR = 1; // Reloads register values now
  EXTMODULE_TIMER->DIER = TIM_DIER_UDE; // Update DMA request
  EXTMODULE_TIMER->CR1 = TIM_CR1_CEN; // Start timer
#else
  EXTMODULE_TIMER->CCR1 = GET_PPM_DELAY(EXTERNAL_MODULE)*2;
  EXTMODULE_TIMER->CCER = TIM_CCER_CC1E | (GET_PPM_POLARITY(EXTERNAL_MODULE) ? TIM_CCER_CC1P : 0);
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0; // Force O/P high
  EXTMODULE_TIMER->EGR = 1; // Reloads register values now
  EXTMODULE_TIMER->DIER |= TIM_DIER_UDE; // Update DMA request
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC2PE; // PWM mode 1
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN; // Start timer
#endif

  extmoduleSendNextFrame();

  NVIC_EnableIRQ(EXTMODULE_DMA_IRQn);
  NVIC_SetPriority(EXTMODULE_DMA_IRQn, 7);
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn);
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}

void extmodulePxxStart()
{

  EXTERNAL_MODULE_ON();

  GPIO_PinAFConfig(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PinSource, EXTMODULE_TX_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_TX_GPIO, &GPIO_InitStructure);

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  EXTMODULE_TIMER->ARR = 18000;

#if defined(PCBX10) || PCBREV >= 13
  EXTMODULE_TIMER->CCER = TIM_CCER_CC3E | TIM_CCER_CC3NE;
  EXTMODULE_TIMER->BDTR = TIM_BDTR_MOE; // Enable outputs
  EXTMODULE_TIMER->CCR3 = 18;
  EXTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_0; // Force O/P high
  EXTMODULE_TIMER->EGR = 1; // Restart
  EXTMODULE_TIMER->DIER |= TIM_DIER_UDE; // Enable DMA on update
  EXTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN;
#elif defined(PCBNV14)
{
    EXTMODULE_TIMER->CCER = TIM_CCER_CC1E | TIM_CCER_CC1NE;
    EXTMODULE_TIMER->BDTR = TIM_BDTR_MOE; // Enable outputs
    EXTMODULE_TIMER->CCR1 = 18;
    EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0; // Force O/P high
    EXTMODULE_TIMER->EGR = 1; // Restart
    EXTMODULE_TIMER->DIER |= TIM_DIER_UDE; // Enable DMA on update
    EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
    EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN;
}
#else
  EXTMODULE_TIMER->CCER = TIM_CCER_CC1E | TIM_CCER_CC1P | TIM_CCER_CC1NE | TIM_CCER_CC1NP; //  TIM_CCER_CC1E | TIM_CCER_CC1P;
  EXTMODULE_TIMER->BDTR = TIM_BDTR_MOE; // Enable outputs
  EXTMODULE_TIMER->CCR1 = 18;
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0; // Force O/P high
  EXTMODULE_TIMER->EGR = 1; // Restart
  EXTMODULE_TIMER->DIER |= TIM_DIER_UDE; // Enable DMA on update
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN;
#endif

  extmoduleSendNextFrame();

  NVIC_EnableIRQ(EXTMODULE_DMA_IRQn);
  NVIC_SetPriority(EXTMODULE_DMA_IRQn, 7);
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn);
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}

#if defined(DSM2)
void extmoduleDsm2Start()
{
  EXTERNAL_MODULE_ON();

  GPIO_PinAFConfig(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PinSource, EXTMODULE_TX_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_TX_GPIO, &GPIO_InitStructure);

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  EXTMODULE_TIMER->ARR = 44000; // 22mS

#if defined(PCBX10) || PCBREV >= 13
  EXTMODULE_TIMER->CCER = TIM_CCER_CC3E | TIM_CCER_CC3P;
  EXTMODULE_TIMER->BDTR = TIM_BDTR_MOE; // Enable outputs
  EXTMODULE_TIMER->CCR3 = 0;
  EXTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_0; // Force O/P high
  EXTMODULE_TIMER->EGR = 1; // Restart
  EXTMODULE_TIMER->DIER |= TIM_DIER_UDE; // Enable DMA on update
  EXTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0;
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN;
#else
  EXTMODULE_TIMER->CCER = TIM_CCER_CC1E | TIM_CCER_CC1P;
  EXTMODULE_TIMER->BDTR = TIM_BDTR_MOE; // Enable outputs
  EXTMODULE_TIMER->CCR1 = 0;
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0; // Force O/P high
  EXTMODULE_TIMER->EGR = 1; // Restart
  EXTMODULE_TIMER->DIER |= TIM_DIER_UDE; // Enable DMA on update
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0;
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN;
#endif

  extmoduleSendNextFrame();

  NVIC_EnableIRQ(EXTMODULE_DMA_IRQn);
  NVIC_SetPriority(EXTMODULE_DMA_IRQn, 7);
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn);
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}
#endif

void extmoduleCrossfireStart()
{
  EXTERNAL_MODULE_ON();

  GPIO_PinAFConfig(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PinSource, 0);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_TX_GPIO, &GPIO_InitStructure);
  GPIO_SetBits(EXTMODULE_TX_GPIO, EXTMODULE_TX_GPIO_PIN); // Set high

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  EXTMODULE_TIMER->ARR = (2000 * CROSSFIRE_FRAME_PERIOD);
  EXTMODULE_TIMER->CCR2 = (2000 * CROSSFIRE_FRAME_PERIOD) - 1000;
  EXTMODULE_TIMER->EGR = 1; // Restart
  EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF;
  EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE; // Enable this interrupt
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN;

  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn);
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}

void extmoduleSendNextFrame()
{
  // TODO
}


extern "C" void EXTMODULE_DMA_IRQHandler()
{
  if (!DMA_GetITStatus(EXTMODULE_DMA_STREAM, EXTMODULE_DMA_FLAG_TC))
    return;

  DMA_ClearITPendingBit(EXTMODULE_DMA_STREAM, EXTMODULE_DMA_FLAG_TC);

  EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF; // Clear flag
  EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE; // Enable this interrupt
}

extern "C" void EXTMODULE_TIMER_IRQHandler()
{
  EXTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE; // Stop this interrupt
  EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF;
  setupPulses(EXTERNAL_MODULE);
  extmoduleSendNextFrame();
}
