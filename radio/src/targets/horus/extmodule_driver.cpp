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

void extmoduleStop()
{
  EXTERNAL_MODULE_OFF();
  
  NVIC_DisableIRQ(EXTMODULE_DMA_IRQn);
  NVIC_DisableIRQ(EXTMODULE_TIMER_IRQn);

  EXTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
  EXTMODULE_TIMER->DIER &= ~(TIM_DIER_CC2IE | TIM_DIER_UDE);
  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
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
#else
  EXTMODULE_TIMER->CCR1 = GET_MODULE_PPM_DELAY(EXTERNAL_MODULE)*2;
  EXTMODULE_TIMER->CCER = TIM_CCER_CC1E | (GET_MODULE_PPM_POLARITY(EXTERNAL_MODULE) ? TIM_CCER_CC1P : 0);
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0; // Force O/P high
  EXTMODULE_TIMER->EGR = 1; // Reloads register values now
  EXTMODULE_TIMER->DIER |= TIM_DIER_UDE; // Update DMA request
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC2PE; // PWM mode 1
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN; // Start timer
#endif

  NVIC_EnableIRQ(EXTMODULE_DMA_IRQn);
  NVIC_SetPriority(EXTMODULE_DMA_IRQn, 7);
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn);
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}

void extmodulePxx1PulsesStart()
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

  NVIC_EnableIRQ(EXTMODULE_DMA_IRQn);
  NVIC_SetPriority(EXTMODULE_DMA_IRQn, 7);
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn);
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}

#if defined(DSM2)
void extmoduleSerialStart(uint32_t /*baudrate*/, uint32_t period_half_us, bool inverted)
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
  EXTMODULE_TIMER->ARR = period_half_us;
  
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

  NVIC_EnableIRQ(EXTMODULE_DMA_IRQn);
  NVIC_SetPriority(EXTMODULE_DMA_IRQn, 7);
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn);
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}
#endif

#if defined(EXTMODULE_USART)
void extmoduleSendBuffer(const uint8_t * data, uint8_t size)
{
  DMA_InitTypeDef DMA_InitStructure;
  DMA_DeInit(EXTMODULE_DMA_STREAM);
  DMA_InitStructure.DMA_Channel = EXTMODULE_DMA_CHANNEL;
  DMA_InitStructure.DMA_PeripheralBaseAddr = CONVERT_PTR_UINT(&EXTMODULE_USART->DR);
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_Memory0BaseAddr = CONVERT_PTR_UINT(data);
  DMA_InitStructure.DMA_BufferSize = size;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(EXTMODULE_DMA_STREAM, &DMA_InitStructure);
  DMA_Cmd(EXTMODULE_DMA_STREAM, ENABLE);
  USART_DMACmd(EXTMODULE_USART, USART_DMAReq_Tx, ENABLE);
}
#endif

void extmoduleSendNextFrame()
{
  if (moduleState[EXTERNAL_MODULE].protocol == PROTOCOL_CHANNELS_PPM) {
#if defined(PCBX10) || PCBREV >= 13
    EXTMODULE_TIMER->CCR3 = GET_MODULE_PPM_DELAY(EXTERNAL_MODULE)*2;
    EXTMODULE_TIMER->CCER = TIM_CCER_CC3E | (GET_MODULE_PPM_POLARITY(EXTERNAL_MODULE) ? TIM_CCER_CC3P : 0);
    EXTMODULE_TIMER->CCR2 = *(extmodulePulsesData.ppm.ptr - 1) - 4000; // 2mS in advance
    EXTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
    EXTMODULE_DMA_STREAM->CR |= EXTMODULE_DMA_CHANNEL | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_0 | DMA_SxCR_MSIZE_0 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
#else
    EXTMODULE_TIMER->CCR1 = GET_MODULE_PPM_DELAY(EXTERNAL_MODULE)*2;
    EXTMODULE_TIMER->CCER = TIM_CCER_CC1E | (GET_MODULE_PPM_POLARITY(EXTERNAL_MODULE) ? TIM_CCER_CC1P : 0);
    EXTMODULE_TIMER->CCR2 = *(extmodulePulsesData.ppm.ptr - 1) - 4000; // 2mS in advance
    EXTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
    EXTMODULE_DMA_STREAM->CR |= EXTMODULE_DMA_CHANNEL | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_1 | DMA_SxCR_MSIZE_1 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
#endif
    EXTMODULE_DMA_STREAM->PAR = CONVERT_PTR_UINT(&EXTMODULE_TIMER->ARR);
    EXTMODULE_DMA_STREAM->M0AR = CONVERT_PTR_UINT(extmodulePulsesData.ppm.pulses);
    EXTMODULE_DMA_STREAM->NDTR = extmodulePulsesData.ppm.ptr - extmodulePulsesData.ppm.pulses;
    EXTMODULE_DMA_STREAM->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE; // Enable DMA
  }
#if defined(PXX1)
  else if (moduleState[EXTERNAL_MODULE].protocol == PROTOCOL_CHANNELS_PXX1_PULSES) {
    EXTMODULE_TIMER->CCR2 = extmodulePulsesData.pxx.getLast() - 4000; // 2mS in advance
    EXTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
#if defined(PCBX10) || PCBREV >= 13
    EXTMODULE_DMA_STREAM->CR |= EXTMODULE_DMA_CHANNEL | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_0 | DMA_SxCR_MSIZE_0 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
#else
    EXTMODULE_DMA_STREAM->CR |= EXTMODULE_DMA_CHANNEL | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_1 | DMA_SxCR_MSIZE_1 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
#endif
    EXTMODULE_DMA_STREAM->PAR = CONVERT_PTR_UINT(&EXTMODULE_TIMER->ARR);
    EXTMODULE_DMA_STREAM->M0AR = CONVERT_PTR_UINT(extmodulePulsesData.pxx.getData());
    EXTMODULE_DMA_STREAM->NDTR = extmodulePulsesData.pxx.getSize();
    EXTMODULE_DMA_STREAM->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE; // Enable DMA
  }
#endif
#if defined(PXX2)
  else if (moduleState[EXTERNAL_MODULE].protocol == PROTOCOL_CHANNELS_PXX2) {
    sportSendBuffer(extmodulePulsesData.pxx2.getData(), extmodulePulsesData.pxx2.getSize());
  }
#endif
#if defined(DSM2)
  else if (IS_DSM2_PROTOCOL(moduleState[EXTERNAL_MODULE].protocol) || IS_MULTIMODULE_PROTOCOL(moduleState[EXTERNAL_MODULE].protocol) || IS_SBUS_PROTOCOL(moduleState[EXTERNAL_MODULE].protocol)) {
    EXTMODULE_TIMER->CCR2 = *(extmodulePulsesData.dsm2.ptr - 1) - 4000; // 2mS in advance
    EXTMODULE_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
#if defined(PCBX10) || PCBREV >= 13
    EXTMODULE_DMA_STREAM->CR |= EXTMODULE_DMA_CHANNEL | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_0 | DMA_SxCR_MSIZE_0 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
    if (IS_SBUS_PROTOCOL(moduleState[EXTERNAL_MODULE].protocol))
      EXTMODULE_TIMER->CCER = TIM_CCER_CC3E | (GET_SBUS_POLARITY(EXTERNAL_MODULE) ? TIM_CCER_CC3P : 0); // reverse polarity for Sbus if needed
#else
    EXTMODULE_DMA_STREAM->CR |= EXTMODULE_DMA_CHANNEL | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_1 | DMA_SxCR_MSIZE_1 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
    if (IS_SBUS_PROTOCOL(moduleState[EXTERNAL_MODULE].protocol))
      EXTMODULE_TIMER->CCER = TIM_CCER_CC1E | (GET_SBUS_POLARITY(EXTERNAL_MODULE) ? TIM_CCER_CC1P : 0); // reverse polarity for Sbus if needed
#endif
    EXTMODULE_DMA_STREAM->PAR = CONVERT_PTR_UINT(&EXTMODULE_TIMER->ARR);
    EXTMODULE_DMA_STREAM->M0AR = CONVERT_PTR_UINT(extmodulePulsesData.dsm2.pulses);
    EXTMODULE_DMA_STREAM->NDTR = extmodulePulsesData.dsm2.ptr - extmodulePulsesData.dsm2.pulses;
    EXTMODULE_DMA_STREAM->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE; // Enable DMA
  }
#endif
  else {
    EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE;
  }
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
  if (setupPulsesExternalModule())
    extmoduleSendNextFrame();
}
