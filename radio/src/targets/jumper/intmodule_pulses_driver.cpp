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

void intmoduleStop()
{
  INTERNAL_MODULE_OFF();

  NVIC_DisableIRQ(INTMODULE_DMA_STREAM_IRQn);
  NVIC_DisableIRQ(INTMODULE_TIMER_CC_IRQn);

  INTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
  INTMODULE_TIMER->DIER &= ~(TIM_DIER_CC2IE | TIM_DIER_UDE);
  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
}

void intmoduleNoneStart()
{
  INTERNAL_MODULE_OFF();

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = INTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(INTMODULE_TX_GPIO, &GPIO_InitStructure);
  GPIO_SetBits(INTMODULE_TX_GPIO, INTMODULE_TX_GPIO_PIN); // Set high

  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  INTMODULE_TIMER->PSC = INTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS from 30MHz
  INTMODULE_TIMER->ARR = 36000; // 18mS
  INTMODULE_TIMER->CCR2 = 32000; // Update time
  INTMODULE_TIMER->EGR = 1; // Restart
  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF; // Clear flag
  INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE; // Enable this interrupt
  INTMODULE_TIMER->CR1 |= TIM_CR1_CEN;

  NVIC_EnableIRQ(INTMODULE_TIMER_CC_IRQn);
  NVIC_SetPriority(INTMODULE_TIMER_CC_IRQn, 7);
}

void intmoduleSendNextFrame()
{
  if (s_current_protocol[INTERNAL_MODULE] == PROTO_PXX) {
    INTMODULE_TIMER->CCR2 = *(modulePulsesData[INTERNAL_MODULE].pxx.ptr - 1) - 4000; // 2mS in advance
    INTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
    INTMODULE_DMA_STREAM->CR |= INTMODULE_DMA_CHANNEL | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_0 | DMA_SxCR_MSIZE_0 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
    INTMODULE_DMA_STREAM->PAR = CONVERT_PTR_UINT(&INTMODULE_TIMER->ARR);
    INTMODULE_DMA_STREAM->M0AR = CONVERT_PTR_UINT(modulePulsesData[INTERNAL_MODULE].pxx.pulses);
    INTMODULE_DMA_STREAM->NDTR = modulePulsesData[INTERNAL_MODULE].pxx.ptr - modulePulsesData[INTERNAL_MODULE].pxx.pulses;
    INTMODULE_DMA_STREAM->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE; // Enable DMA
  }
#if defined(TARANIS_INTERNAL_PPM)
  else if (s_current_protocol[INTERNAL_MODULE] == PROTO_PPM) {
    INTMODULE_TIMER->CCR3 = GET_PPM_DELAY(INTERNAL_MODULE)*2;
    INTMODULE_TIMER->CCER = TIM_CCER_CC3E | (GET_PPM_POLARITY(INTERNAL_MODULE) ? 0 : TIM_CCER_CC3P);
    INTMODULE_TIMER->CCR2 = *(modulePulsesData[INTERNAL_MODULE].ppm.ptr - 1) - 4000; // 2mS in advance
    INTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
    INTMODULE_DMA_STREAM->CR |= INTMODULE_DMA_CHANNEL | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_0 | DMA_SxCR_MSIZE_0 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
    INTMODULE_DMA_STREAM->PAR = CONVERT_PTR_UINT(&INTMODULE_TIMER->ARR);
    INTMODULE_DMA_STREAM->M0AR = CONVERT_PTR_UINT(modulePulsesData[INTERNAL_MODULE].ppm.pulses);
    INTMODULE_DMA_STREAM->NDTR = modulePulsesData[INTERNAL_MODULE].ppm.ptr - modulePulsesData[INTERNAL_MODULE].ppm.pulses;
    INTMODULE_DMA_STREAM->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE; // Enable DMA
  }
#endif
  else {
    INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE;
  }
}

void intmodulePxxStart()
{
  INTERNAL_MODULE_ON();

  GPIO_PinAFConfig(INTMODULE_TX_GPIO, INTMODULE_TX_GPIO_PinSource, INTMODULE_TX_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = INTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(INTMODULE_TX_GPIO, &GPIO_InitStructure);

  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  INTMODULE_TIMER->PSC = INTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  INTMODULE_TIMER->ARR = 18000;
  INTMODULE_TIMER->CCER = TIM_CCER_CC3E | TIM_CCER_CC3NE;
  INTMODULE_TIMER->BDTR = TIM_BDTR_MOE; // Enable outputs
  INTMODULE_TIMER->CCR3 = 16;
  INTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_0; // Force O/P high
  INTMODULE_TIMER->EGR = 1; // Restart
  INTMODULE_TIMER->DIER |= TIM_DIER_UDE; // Enable DMA on update
  INTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
  INTMODULE_TIMER->CR1 |= TIM_CR1_CEN;

  intmoduleSendNextFrame();

  NVIC_EnableIRQ(INTMODULE_DMA_STREAM_IRQn);
  NVIC_SetPriority(INTMODULE_DMA_STREAM_IRQn, 7);
  NVIC_EnableIRQ(INTMODULE_TIMER_CC_IRQn);
  NVIC_SetPriority(INTMODULE_TIMER_CC_IRQn, 7);
}

#if defined(TARANIS_INTERNAL_PPM)
void intmodulePpmStart()
{
  INTERNAL_MODULE_ON();

  GPIO_PinAFConfig(INTMODULE_TX_GPIO, INTMODULE_TX_GPIO_PinSource, INTMODULE_TX_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = INTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(INTMODULE_TX_GPIO, &GPIO_InitStructure);

  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  INTMODULE_TIMER->PSC = INTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS from 30MHz
  INTMODULE_TIMER->ARR = 45000;
  INTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2; // PWM mode 1
  INTMODULE_TIMER->BDTR = TIM_BDTR_MOE;
  INTMODULE_TIMER->EGR = 1;
  INTMODULE_TIMER->DIER = TIM_DIER_UDE;
  INTMODULE_TIMER->CR1 = TIM_CR1_CEN;

  intmoduleSendNextFrame();

  NVIC_EnableIRQ(INTMODULE_DMA_STREAM_IRQn);
  NVIC_SetPriority(INTMODULE_DMA_STREAM_IRQn, 7);
  NVIC_EnableIRQ(INTMODULE_TIMER_CC_IRQn);
  NVIC_SetPriority(INTMODULE_TIMER_CC_IRQn, 7);
}
#endif // defined(TARANIS_INTERNAL_PPM)

extern "C" void INTMODULE_DMA_STREAM_IRQHandler()
{
  if (!DMA_GetITStatus(INTMODULE_DMA_STREAM, INTMODULE_DMA_FLAG_TC))
    return;

  DMA_ClearITPendingBit(INTMODULE_DMA_STREAM, INTMODULE_DMA_FLAG_TC);

  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF; // Clear flag
  INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE; // Enable this interrupt
}

extern "C" void INTMODULE_TIMER_CC_IRQHandler()
{
  INTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE; // Stop this interrupt
  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF;
  setupPulses(INTERNAL_MODULE);
  intmoduleSendNextFrame();
}
