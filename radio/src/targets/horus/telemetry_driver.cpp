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

DMAFifo<512> telemetryFifo __DMA (TELEMETRY_DMA_Stream_RX);

void telemetryPortInit(uint32_t baudrate)
{
  if (baudrate == 0) {
    USART_DeInit(TELEMETRY_USART);
    return;
  }

  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = TELEMETRY_DMA_TX_Stream_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; /* Not used as 4 bits are used for the pre-emption priority. */;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_PinAFConfig(TELEMETRY_GPIO, TELEMETRY_GPIO_PinSource_RX, TELEMETRY_GPIO_AF);
  GPIO_PinAFConfig(TELEMETRY_GPIO, TELEMETRY_GPIO_PinSource_TX, TELEMETRY_GPIO_AF);

  GPIO_InitStructure.GPIO_Pin = TELEMETRY_TX_GPIO_PIN | TELEMETRY_RX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(TELEMETRY_GPIO, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = TELEMETRY_DIR_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(TELEMETRY_GPIO_DIR, &GPIO_InitStructure);
  GPIO_ResetBits(TELEMETRY_GPIO_DIR, TELEMETRY_DIR_GPIO_PIN);

  USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

  USART_Init(TELEMETRY_USART, &USART_InitStructure);

  DMA_InitTypeDef DMA_InitStructure;
  telemetryFifo.clear();
  USART_ITConfig(TELEMETRY_USART, USART_IT_RXNE, DISABLE);
  USART_ITConfig(TELEMETRY_USART, USART_IT_TXE, DISABLE);
  DMA_InitStructure.DMA_Channel = TELEMETRY_DMA_Channel_RX;
  DMA_InitStructure.DMA_PeripheralBaseAddr = CONVERT_PTR_UINT(&TELEMETRY_USART->DR);
  DMA_InitStructure.DMA_Memory0BaseAddr = CONVERT_PTR_UINT(telemetryFifo.buffer());
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = telemetryFifo.size();
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
  DMA_Init(TELEMETRY_DMA_Stream_RX, &DMA_InitStructure);
  USART_DMACmd(TELEMETRY_USART, USART_DMAReq_Rx, ENABLE);
  USART_Cmd(TELEMETRY_USART, ENABLE);
  DMA_Cmd(TELEMETRY_DMA_Stream_RX, ENABLE);
}

void telemetryPortSetDirectionOutput()
{
  TELEMETRY_GPIO_DIR->BSRRL = TELEMETRY_DIR_GPIO_PIN;     // output enable
  TELEMETRY_USART->CR1 &= ~USART_CR1_RE;                  // turn off receiver
}

void telemetryPortSetDirectionInput()
{
  TELEMETRY_GPIO_DIR->BSRRH = TELEMETRY_DIR_GPIO_PIN;     // output disable
  TELEMETRY_USART->CR1 |= USART_CR1_RE;                   // turn on receiver
}

void sportSendBuffer(uint8_t * buffer, uint32_t count)
{
  #if 0
  telemetryPortSetDirectionOutput();

  DMA_InitTypeDef DMA_InitStructure;
  DMA_DeInit(TELEMETRY_DMA_Stream_TX);
  DMA_InitStructure.DMA_Channel = TELEMETRY_DMA_Channel_TX;
  DMA_InitStructure.DMA_PeripheralBaseAddr = CONVERT_PTR_UINT(&TELEMETRY_USART->DR);
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_Memory0BaseAddr = CONVERT_PTR_UINT(buffer);
  DMA_InitStructure.DMA_BufferSize = count;
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
  DMA_Init(TELEMETRY_DMA_Stream_TX, &DMA_InitStructure);
  DMA_Cmd(TELEMETRY_DMA_Stream_TX, ENABLE);
  USART_DMACmd(TELEMETRY_USART, USART_DMAReq_Tx, ENABLE);
  DMA_ITConfig(TELEMETRY_DMA_Stream_TX, DMA_IT_TC, ENABLE);

  // enable interrupt and set it's priority
  NVIC_EnableIRQ(TELEMETRY_DMA_TX_Stream_IRQ) ;
  NVIC_SetPriority(TELEMETRY_DMA_TX_Stream_IRQ, 7);
    #endif
}

extern "C" void TELEMETRY_DMA_TX_IRQHandler(void)
{
  DEBUG_INTERRUPT(INT_TELEM_DMA);
  if (DMA_GetITStatus(TELEMETRY_DMA_Stream_TX, TELEMETRY_DMA_TX_FLAG_TC)) {
    DMA_ClearITPendingBit(TELEMETRY_DMA_Stream_TX, TELEMETRY_DMA_TX_FLAG_TC);
    telemetryPortSetDirectionInput();
  }
}

// TODO we should have telemetry in an higher layer, functions above should move to a sport_driver.cpp
int telemetryGetByte(uint8_t * byte)
{
  // TODO I am not sure it's possible to have this cable on Horus ...
#if 0
  if (telemetryProtocol == PROTOCOL_FRSKY_D_SECONDARY) {
    if (serial2Mode == UART_MODE_TELEMETRY)
      return serial2RxFifo.pop(*byte);
    else
      return 0;
  }
  else {
    return telemetryFifo.pop(*byte);
  }
#else
  return telemetryFifo.pop(*byte);
#endif


}
