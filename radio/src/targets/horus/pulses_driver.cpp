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

void intmoduleStop(void);
void extmoduleStop(void);

void intmoduleNoneStart(void);
void intmodulePxxStart(void);

void extmoduleNoneStart(void);
void extmodulePpmStart(void);
void extmodulePxxStart(void);
#if defined(DSM2)
void extmoduleDsm2Start(void);
#endif
void extmoduleCrossfireStart(void);

void init_no_pulses(uint32_t port)
{
  if (port == INTERNAL_MODULE)
    intmoduleNoneStart();
  else
    extmoduleNoneStart();
}

void disable_no_pulses(uint32_t port)
{
  if (port == INTERNAL_MODULE)
    intmoduleStop();
  else
    extmoduleStop();
}

void init_ppm(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmodulePpmStart();
  }
}

void disable_ppm(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmoduleStop();
  }
}

void init_pxx(uint32_t port)
{
  if (port == INTERNAL_MODULE)
    intmodulePxxStart();
  else
    extmodulePxxStart();
}

void disable_pxx(uint32_t port)
{
  if (port == INTERNAL_MODULE)
    intmoduleStop();
  else
    extmoduleStop();
}

#if defined(DSM2)
void init_dsm2(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmoduleDsm2Start();
  }
}

void disable_dsm2(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmoduleStop();
  }
}
#endif

void init_sbusOut(uint32_t port)
{
  init_dsm2(port);
}

void disable_sbusOut(uint32_t port)
{
  disable_dsm2(port);
}

void init_crossfire(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmoduleCrossfireStart();
  }
}

void disable_crossfire(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmoduleStop();
  }
}

void intmoduleStop()
{
  INTERNAL_MODULE_OFF();

  NVIC_DisableIRQ(INTMODULE_TIMER_IRQn);

  INTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE;
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
  INTMODULE_TIMER->ARR = 36000;             // 18mS
  INTMODULE_TIMER->CCR2 = 32000;            // Update time
  INTMODULE_TIMER->PSC = INTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)

  INTMODULE_TIMER->CCER = TIM_CCER_CC3E;

  INTMODULE_TIMER->CCMR2 = 0;
  INTMODULE_TIMER->EGR = 1;                                                         // Restart

  INTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0;                     // Toggle CC1 o/p
  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF;                             // Clear flag
  INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE;  // Enable this interrupt
  INTMODULE_TIMER->CR1 |= TIM_CR1_CEN;
  NVIC_EnableIRQ(INTMODULE_TIMER_IRQn);
  NVIC_SetPriority(INTMODULE_TIMER_IRQn, 7);
}

void intmodulePxxStart()
{
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  INTERNAL_MODULE_ON();

  NVIC_InitStructure.NVIC_IRQChannel = INTMODULE_DMA_STREAM_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; /* Not used as 4 bits are used for the pre-emption priority. */;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // TX Pin
  GPIO_PinAFConfig(INTMODULE_TX_GPIO, INTMODULE_TX_GPIO_PinSource, INTMODULE_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = INTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(INTMODULE_TX_GPIO, &GPIO_InitStructure);

  // RX Pin
  GPIO_PinAFConfig(INTMODULE_RX_GPIO, INTMODULE_RX_GPIO_PinSource, INTMODULE_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = INTMODULE_RX_GPIO_PIN;
  GPIO_Init(INTMODULE_RX_GPIO, &GPIO_InitStructure);

  // UART config
  USART_DeInit(INTMODULE_USART);
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(INTMODULE_USART, &USART_InitStructure);
  USART_Cmd(INTMODULE_USART, ENABLE);

  // Timer
  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  INTMODULE_TIMER->ARR = 18000; // 9mS
  INTMODULE_TIMER->CCR2 = 15000; // Update time
  INTMODULE_TIMER->PSC = INTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  INTMODULE_TIMER->CCER = TIM_CCER_CC3E;
  INTMODULE_TIMER->CCMR2 = 0;
  INTMODULE_TIMER->EGR = 1; // Restart

  INTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0; // Toggle CC1 o/p
  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF; // Clear flag
  INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE;  // Enable this interrupt
  INTMODULE_TIMER->CR1 |= TIM_CR1_CEN;

  NVIC_EnableIRQ(INTMODULE_TIMER_IRQn);
  NVIC_SetPriority(INTMODULE_TIMER_IRQn, 7);
}

extern "C" void INTMODULE_DMA_STREAM_IRQHandler(void)
{
  DEBUG_INTERRUPT(INT_DMA2S7);
  if (DMA_GetITStatus(INTMODULE_DMA_STREAM, INTMODULE_DMA_FLAG_TC)) {
    // TODO we could send the 8 next channels here (when needed)
    DMA_ClearITPendingBit(INTMODULE_DMA_STREAM, INTMODULE_DMA_FLAG_TC);
  }
}

void intmoduleSendNextFrame()
{
  if (s_current_protocol[INTERNAL_MODULE] == PROTO_PXX) {
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(INTMODULE_DMA_STREAM);
    DMA_InitStructure.DMA_Channel = INTMODULE_DMA_CHANNEL;
    DMA_InitStructure.DMA_PeripheralBaseAddr = CONVERT_PTR_UINT(&INTMODULE_USART->DR);
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_Memory0BaseAddr = CONVERT_PTR_UINT(modulePulsesData[INTERNAL_MODULE].pxx_uart.pulses);
    DMA_InitStructure.DMA_BufferSize = modulePulsesData[INTERNAL_MODULE].pxx_uart.ptr - modulePulsesData[INTERNAL_MODULE].pxx_uart.pulses;
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
    DMA_Init(INTMODULE_DMA_STREAM, &DMA_InitStructure);

    DMA_Cmd(INTMODULE_DMA_STREAM, ENABLE);
    USART_DMACmd(INTMODULE_USART, USART_DMAReq_Tx, ENABLE);
  }
}

extern "C" void INTMODULE_TIMER_IRQHandler()
{
  DEBUG_INTERRUPT(INT_TIM1CC);
  DEBUG_TIMER_SAMPLE(debugTimerIntPulses);
  DEBUG_TIMER_START(debugTimerIntPulsesDuration);

  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF;           // clear flag
  setupPulses(INTERNAL_MODULE);
  intmoduleSendNextFrame();
  
  DEBUG_TIMER_STOP(debugTimerIntPulsesDuration);
}
