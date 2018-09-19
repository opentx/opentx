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
#include "debug.h"

Fifo<uint8_t, 64> intmoduleRxFifo;
DMAFifo<512> intmoduleDMAFifo __DMA (INTMODULE_RX_DMA_STREAM);

void intmoduleStop()
{
  INTERNAL_MODULE_OFF();

  NVIC_DisableIRQ(INTMODULE_TIMER_IRQn);

  INTMODULE_TX_DMA_STREAM->CR &= ~DMA_SxCR_EN; // Disable DMA
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
  INTMODULE_TIMER->PSC = INTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  INTMODULE_TIMER->ARR = 36000; // 18mS
  INTMODULE_TIMER->CCR2 = 32000; // Update time
  INTMODULE_TIMER->EGR = 1; // Restart
  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF; // Clear flag
  INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE; // Enable this interrupt
  INTMODULE_TIMER->CR1 |= TIM_CR1_CEN;
  NVIC_EnableIRQ(INTMODULE_TIMER_IRQn);
  NVIC_SetPriority(INTMODULE_TIMER_IRQn, 7);
}

//#define INTMODULE_RX_INT
static uint8_t intmodule_hal_inited = 0;
void intmodulePxxStart()
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = INTMODULE_TX_DMA_Stream_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; /* Not used as 4 bits are used for the pre-emption priority. */;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  GPIO_PinAFConfig(INTMODULE_TX_GPIO, INTMODULE_TX_GPIO_PinSource, INTMODULE_TX_GPIO_AF);
  GPIO_PinAFConfig(INTMODULE_TX_GPIO, INTMODULE_RX_GPIO_PinSource, INTMODULE_TX_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = INTMODULE_TX_GPIO_PIN | INTMODULE_RX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(INTMODULE_TX_GPIO, &GPIO_InitStructure);

  // Init Module PWR
  GPIO_ResetBits(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN);
  GPIO_InitStructure.GPIO_Pin = INTMODULE_PWR_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(INTMODULE_PWR_GPIO, &GPIO_InitStructure);
  INTERNAL_MODULE_ON();

  USART_DeInit(INTMODULE_USART);
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = INTERNAL_MODULE_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(INTMODULE_USART, &USART_InitStructure);

#ifdef INTMODULE_RX_INT
  USART_Cmd(INTMODULE_USART, ENABLE);
  USART_ITConfig(INTMODULE_USART, USART_IT_RXNE, ENABLE);
  USART_ITConfig(INTMODULE_USART, USART_IT_TXE, DISABLE);
  NVIC_SetPriority(INTMODULE_USART_IRQn, 7);
  NVIC_EnableIRQ(INTMODULE_USART_IRQn);
#else // RX by DMA
  DMA_Cmd(INTMODULE_RX_DMA_STREAM, DISABLE);
  USART_DMACmd(INTMODULE_USART, USART_DMAReq_Rx, DISABLE);
  DMA_DeInit(INTMODULE_RX_DMA_STREAM);

  DMA_InitTypeDef DMA_InitStructure;
  intmoduleDMAFifo.clear();

  DMA_InitStructure.DMA_Channel = INTMODULE_DMA_CHANNEL;
  DMA_InitStructure.DMA_PeripheralBaseAddr = CONVERT_PTR_UINT(&INTMODULE_USART->DR);
  DMA_InitStructure.DMA_Memory0BaseAddr = CONVERT_PTR_UINT(intmoduleDMAFifo.buffer());
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = intmoduleDMAFifo.size();
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
  DMA_Init(INTMODULE_RX_DMA_STREAM, &DMA_InitStructure);
  USART_DMACmd(INTMODULE_USART, USART_DMAReq_Rx, ENABLE);
  USART_ITConfig(INTMODULE_USART, USART_IT_RXNE, DISABLE);
  USART_ITConfig(INTMODULE_USART, USART_IT_TXE, DISABLE);
  USART_Cmd(INTMODULE_USART, ENABLE);
  DMA_Cmd(INTMODULE_RX_DMA_STREAM, ENABLE); // TRACE("RF DMA receive started...");
 #endif

  // Timer
  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN;
  INTMODULE_TIMER->PSC = INTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  INTMODULE_TIMER->ARR = 18000; // 9mS
  INTMODULE_TIMER->CCR2 = 15000; // Update time
  INTMODULE_TIMER->CCER = TIM_CCER_CC3E;
  INTMODULE_TIMER->CCMR2 = 0;
  INTMODULE_TIMER->EGR = 1; // Restart
  INTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0; // Toggle CC1 o/p
  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF; // Clear flag
  INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE;  // Enable this interrupt
  INTMODULE_TIMER->CR1 |= TIM_CR1_CEN;

  NVIC_SetPriority(INTMODULE_TIMER_IRQn, 7);
  NVIC_EnableIRQ(INTMODULE_TIMER_IRQn);

  intmodule_hal_inited = 1;
}

extern "C" void INTMODULE_USART_IRQHandler(void)
{
  DEBUG_INTERRUPT(INT_SER2);

  // Receive
  uint32_t status = INTMODULE_USART->SR;
  while (status & (USART_FLAG_RXNE | USART_FLAG_ERRORS)) {
    uint8_t data = INTMODULE_USART->DR;
    if (!(status & USART_FLAG_ERRORS)) {
      intmoduleRxFifo.push(data);
    }
    status = INTMODULE_USART->SR;
  }
}

extern "C" void INTMODULE_TX_DMA_Stream_IRQHandler(void)
{
  DEBUG_INTERRUPT(INT_DMA2S7);
  if (DMA_GetITStatus(INTMODULE_TX_DMA_STREAM, INTMODULE_TX_DMA_FLAG_TC)) {
    // TODO we could send the 8 next channels here (when needed)
    DMA_ClearITPendingBit(INTMODULE_TX_DMA_STREAM, INTMODULE_TX_DMA_FLAG_TC);
  }
}

uint8_t intmoduleGetByte(uint8_t * byte)
{
    if (intmodule_hal_inited == 0) {
        return 0; // incase of call before initialization
    }
#ifdef INTMODULE_RX_INT
    return intmoduleRxFifo.pop(*byte);
#else
    return intmoduleDMAFifo.pop(*byte);
#endif
}

void intmoduleSendBufferDMA(uint8_t * data, uint8_t size)
{
  if (IS_PXX_PROTOCOL(s_current_protocol[INTERNAL_MODULE]) || IS_FLYSKY_PROTOCOL(s_current_protocol[INTERNAL_MODULE])) {
    if (size) {
#if !defined(SIMU)
      if (data[0] == 0x55 || data[3] == 0x0C) { // Firmware update printf
          TRACE("TX: %02X %02X %02X ...%02X %02X; CRC:%04X", data[0], data[1], data[2],
                data[size-2], data[size-1], calc_crc16(data, size - 2));
      }
#endif
      DMA_InitTypeDef DMA_InitStructure;
      DMA_DeInit(INTMODULE_TX_DMA_STREAM);
      DMA_InitStructure.DMA_Channel = INTMODULE_DMA_CHANNEL;
      DMA_InitStructure.DMA_PeripheralBaseAddr = CONVERT_PTR_UINT(&INTMODULE_USART->DR);
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
      DMA_Init(INTMODULE_TX_DMA_STREAM, &DMA_InitStructure);
      DMA_Cmd(INTMODULE_TX_DMA_STREAM, ENABLE);
      USART_DMACmd(INTMODULE_USART, USART_DMAReq_Tx, ENABLE);
    }
  }
}

void intmoduleSendNextFrame()
{
    uint8_t * data = modulePulsesData[INTERNAL_MODULE].pxx_uart.pulses;
    uint8_t size = modulePulsesData[INTERNAL_MODULE].pxx_uart.ptr - data;
    intmoduleSendBufferDMA(data, size);
}

extern "C" void INTMODULE_TIMER_IRQHandler()
{
  DEBUG_INTERRUPT(INT_TIM1CC);
  DEBUG_TIMER_SAMPLE(debugTimerIntPulses);
  DEBUG_TIMER_START(debugTimerIntPulsesDuration);

  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF; // clear flag
  setupPulses(INTERNAL_MODULE);
  intmoduleSendNextFrame();

  DEBUG_TIMER_STOP(debugTimerIntPulsesDuration);
}
