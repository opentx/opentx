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

DMAFifo<32> heartbeatFifo __DMA (HEARTBEAT_DMA_Stream);

void trainerSendNextFrame();

void init_trainer_ppm()
{
  GPIO_PinAFConfig(TRAINER_GPIO, TRAINER_OUT_GPIO_PinSource, TRAINER_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = TRAINER_OUT_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(TRAINER_GPIO, &GPIO_InitStructure);

  TRAINER_TIMER->CR1 &= ~TIM_CR1_CEN;
  TRAINER_TIMER->PSC = TRAINER_TIMER_FREQ / 2000000 - 1; // 0.5uS
  TRAINER_TIMER->ARR = 45000;
  TRAINER_TIMER->CCMR2 = TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4PE; // PWM mode 1
  TRAINER_TIMER->BDTR = TIM_BDTR_MOE;
  TRAINER_TIMER->EGR = 1;
  TRAINER_TIMER->DIER |= TIM_DIER_UDE;
  TRAINER_TIMER->CR1 |= TIM_CR1_CEN;

  setupPulsesPPMTrainer();
  trainerSendNextFrame();

  NVIC_EnableIRQ(TRAINER_DMA_IRQn);
  NVIC_SetPriority(TRAINER_DMA_IRQn, 7);
  NVIC_EnableIRQ(TRAINER_TIMER_IRQn);
  NVIC_SetPriority(TRAINER_TIMER_IRQn, 7);
}

void stop_trainer_ppm()
{
  NVIC_DisableIRQ(TRAINER_DMA_IRQn);
  NVIC_DisableIRQ(TRAINER_TIMER_IRQn);

  TRAINER_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
  TRAINER_TIMER->DIER = 0;
  TRAINER_TIMER->CR1 &= ~TIM_CR1_CEN; // Stop counter
}

void init_trainer_capture()
{
  GPIO_PinAFConfig(TRAINER_GPIO, TRAINER_IN_GPIO_PinSource, TRAINER_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = TRAINER_IN_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(TRAINER_GPIO, &GPIO_InitStructure);

  TRAINER_TIMER->ARR = 0xFFFF;
  TRAINER_TIMER->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 2000000 - 1; // 0.5uS
  TRAINER_TIMER->CR2 = 0;
  TRAINER_TIMER->CCMR2 = TIM_CCMR2_IC3F_0 | TIM_CCMR2_IC3F_1 | TIM_CCMR2_CC3S_0;
  TRAINER_TIMER->CCER = TIM_CCER_CC3E;
  TRAINER_TIMER->SR &= ~TIM_SR_CC3IF & ~TIM_SR_CC2IF & ~TIM_SR_UIF; // Clear flags
  TRAINER_TIMER->DIER |= TIM_DIER_CC3IE;
  TRAINER_TIMER->CR1 = TIM_CR1_CEN;

  NVIC_EnableIRQ(TRAINER_TIMER_IRQn);
  NVIC_SetPriority(TRAINER_TIMER_IRQn, 7);
}

void stop_trainer_capture()
{
  NVIC_DisableIRQ(TRAINER_TIMER_IRQn); // Stop Interrupt
  TRAINER_TIMER->CR1 &= ~TIM_CR1_CEN; // Stop counter
  TRAINER_TIMER->DIER = 0; // Stop Interrupt
}

void trainerSendNextFrame()
{
  TRAINER_TIMER->CCR4 = GET_PPM_DELAY(TRAINER_MODULE)*2;
  TRAINER_TIMER->CCER = TIM_CCER_CC4E | (GET_PPM_POLARITY(TRAINER_MODULE) ? 0 : TIM_CCER_CC4P);
  TRAINER_TIMER->CCR1 = *(trainerPulsesData.ppm.ptr - 1) - 4000; // 2mS in advance

  TRAINER_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
  TRAINER_DMA_STREAM->CR |= TRAINER_DMA_CHANNEL | DMA_SxCR_DIR_0 | DMA_SxCR_MINC | DMA_SxCR_PSIZE_0 | DMA_SxCR_MSIZE_0 | DMA_SxCR_PL_0 | DMA_SxCR_PL_1;
  TRAINER_DMA_STREAM->PAR = CONVERT_PTR_UINT(&TRAINER_TIMER->ARR);
  TRAINER_DMA_STREAM->M0AR = CONVERT_PTR_UINT(trainerPulsesData.ppm.pulses);
  TRAINER_DMA_STREAM->NDTR = trainerPulsesData.ppm.ptr - trainerPulsesData.ppm.pulses;
  TRAINER_DMA_STREAM->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE; // Enable DMA
}

extern "C" void TRAINER_DMA_IRQHandler()
{
  if (!DMA_GetITStatus(TRAINER_DMA_STREAM, TRAINER_DMA_FLAG_TC))
    return;

  DMA_ClearITPendingBit(TRAINER_DMA_STREAM, TRAINER_DMA_FLAG_TC);

  TRAINER_TIMER->SR &= ~TIM_SR_CC1IF; // Clear flag
  TRAINER_TIMER->DIER |= TIM_DIER_CC1IE; // Enable this interrupt
}

extern "C" void TRAINER_TIMER_IRQHandler()
{
  DEBUG_INTERRUPT(INT_TRAINER);

  uint16_t capture = 0;
  bool doCapture = false;

  // What mode? in or out?
  if ((TRAINER_TIMER->DIER & TIM_DIER_CC3IE) && (TRAINER_TIMER->SR & TIM_SR_CC3IF)) {
    // capture mode on trainer jack
    capture = TRAINER_TIMER->CCR3;
    if (TRAINER_CONNECTED() && currentTrainerMode == TRAINER_MODE_MASTER_TRAINER_JACK) {
      doCapture = true;
    }
  }

  if ((TRAINER_TIMER->DIER & TIM_DIER_CC2IE) && (TRAINER_TIMER->SR & TIM_SR_CC2IF)) {
    // capture mode on heartbeat pin (external module)
    capture = TRAINER_TIMER->CCR2;
    if (currentTrainerMode == TRAINER_MODE_MASTER_CPPM_EXTERNAL_MODULE) {
      doCapture = true;
    }
  }

  if (doCapture) {
    captureTrainerPulses(capture);
  }

  // PPM out compare interrupt
  if ((TRAINER_TIMER->DIER & TIM_DIER_CC1IE) && (TRAINER_TIMER->SR & TIM_SR_CC1IF)) {
    // compare interrupt
    TRAINER_TIMER->DIER &= ~TIM_DIER_CC1IE; // stop this interrupt
    TRAINER_TIMER->SR &= ~TIM_SR_CC1IF; // Clear flag
    setupPulsesPPMTrainer();
    trainerSendNextFrame();
  }
}

void init_cppm_on_heartbeat_capture(void)
{
  EXTERNAL_MODULE_ON();

  GPIO_PinAFConfig(HEARTBEAT_GPIO, HEARTBEAT_GPIO_PinSource, HEARTBEAT_GPIO_AF_CAPTURE);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = HEARTBEAT_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(HEARTBEAT_GPIO, &GPIO_InitStructure);

  TRAINER_TIMER->ARR = 0xFFFF;
  TRAINER_TIMER->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 2000000 - 1; // 0.5uS
  TRAINER_TIMER->CR2 = 0;
  TRAINER_TIMER->CCMR1 = TIM_CCMR1_IC2F_0 | TIM_CCMR1_IC2F_1 | TIM_CCMR1_CC2S_0;
  TRAINER_TIMER->CCER = TIM_CCER_CC2E;
  TRAINER_TIMER->SR &= ~TIM_SR_CC2IF; // Clear flag
  TRAINER_TIMER->DIER |= TIM_DIER_CC2IE;
  TRAINER_TIMER->CR1 = TIM_CR1_CEN;

  NVIC_SetPriority(TRAINER_TIMER_IRQn, 7);
  NVIC_EnableIRQ(TRAINER_TIMER_IRQn);
}

void stop_cppm_on_heartbeat_capture()
{
  TRAINER_TIMER->DIER = 0;
  TRAINER_TIMER->CR1 &= ~TIM_CR1_CEN;                             // Stop counter
  NVIC_DisableIRQ(TRAINER_TIMER_IRQn);                            // Stop Interrupt

  if (!IS_EXTERNAL_MODULE_ENABLED()) {
    EXTERNAL_MODULE_OFF();
  }
}

void init_sbus_on_heartbeat_capture()
{
  EXTERNAL_MODULE_ON();

  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_PinAFConfig(GPIOC, HEARTBEAT_GPIO_PinSource, HEARTBEAT_GPIO_AF_SBUS);

  GPIO_InitStructure.GPIO_Pin = HEARTBEAT_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 100000;
  USART_InitStructure.USART_WordLength = USART_WordLength_9b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_Even;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx;
  USART_Init(HEARTBEAT_USART, &USART_InitStructure);

  DMA_InitTypeDef DMA_InitStructure;
  heartbeatFifo.clear();
  USART_ITConfig(HEARTBEAT_USART, USART_IT_RXNE, DISABLE);
  USART_ITConfig(HEARTBEAT_USART, USART_IT_TXE, DISABLE);
  DMA_InitStructure.DMA_Channel = HEARTBEAT_DMA_Channel;
  DMA_InitStructure.DMA_PeripheralBaseAddr = CONVERT_PTR_UINT(&HEARTBEAT_USART->DR);
  DMA_InitStructure.DMA_Memory0BaseAddr = CONVERT_PTR_UINT(heartbeatFifo.buffer());
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = heartbeatFifo.size();
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(HEARTBEAT_DMA_Stream, &DMA_InitStructure);
  USART_DMACmd(HEARTBEAT_USART, USART_DMAReq_Rx, ENABLE);
  USART_Cmd(HEARTBEAT_USART, ENABLE);
  DMA_Cmd(HEARTBEAT_DMA_Stream, ENABLE);
}

void stop_sbus_on_heartbeat_capture()
{
  DMA_Cmd(HEARTBEAT_DMA_Stream, DISABLE);
  USART_Cmd(HEARTBEAT_USART, DISABLE);
  USART_DMACmd(HEARTBEAT_USART, USART_DMAReq_Rx, DISABLE);
  DMA_DeInit(HEARTBEAT_DMA_Stream);
  NVIC_DisableIRQ(HEARTBEAT_USART_IRQn);

  if (!IS_EXTERNAL_MODULE_ENABLED()) {
    EXTERNAL_MODULE_OFF();
  }
}

int sbusGetByte(uint8_t * byte)
{
  switch (currentTrainerMode) {
    case TRAINER_MODE_MASTER_SBUS_EXTERNAL_MODULE:
      return heartbeatFifo.pop(*byte);
#if !defined(PCBX7) && !defined(PCBX9E)
    case TRAINER_MODE_MASTER_BATTERY_COMPARTMENT:
      return serial2RxFifo.pop(*byte);
#endif
    default:
      return false;
  }
}
