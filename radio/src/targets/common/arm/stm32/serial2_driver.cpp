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

uint8_t serial2Mode = 0;
Fifo<uint8_t, 512> serial2TxFifo;
DMAFifo<32> serial2RxFifo __DMA (SERIAL_DMA_Stream_RX);

void uart3Setup(unsigned int baudrate, bool dma)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_PinAFConfig(SERIAL_GPIO, SERIAL_GPIO_PinSource_RX, SERIAL_GPIO_AF);
  GPIO_PinAFConfig(SERIAL_GPIO, SERIAL_GPIO_PinSource_TX, SERIAL_GPIO_AF);

  GPIO_InitStructure.GPIO_Pin = SERIAL_GPIO_PIN_TX | SERIAL_GPIO_PIN_RX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(SERIAL_GPIO, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(SERIAL_USART, &USART_InitStructure);

  if (dma) {
    DMA_InitTypeDef DMA_InitStructure;
    serial2RxFifo.clear();
    USART_ITConfig(SERIAL_USART, USART_IT_RXNE, DISABLE);
    USART_ITConfig(SERIAL_USART, USART_IT_TXE, DISABLE);
    DMA_InitStructure.DMA_Channel = SERIAL_DMA_Channel_RX;
    DMA_InitStructure.DMA_PeripheralBaseAddr = CONVERT_PTR_UINT(&SERIAL_USART->DR);
    DMA_InitStructure.DMA_Memory0BaseAddr = CONVERT_PTR_UINT(serial2RxFifo.buffer());
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = serial2RxFifo.size();
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
    DMA_Init(SERIAL_DMA_Stream_RX, &DMA_InitStructure);
    USART_DMACmd(SERIAL_USART, USART_DMAReq_Rx, ENABLE);
    USART_Cmd(SERIAL_USART, ENABLE);
    DMA_Cmd(SERIAL_DMA_Stream_RX, ENABLE);
  }
  else {
    USART_Cmd(SERIAL_USART, ENABLE);
    USART_ITConfig(SERIAL_USART, USART_IT_RXNE, ENABLE);
    USART_ITConfig(SERIAL_USART, USART_IT_TXE, DISABLE);
    NVIC_SetPriority(SERIAL_USART_IRQn, 7);
    NVIC_EnableIRQ(SERIAL_USART_IRQn);
  }
}

void serial2Init(unsigned int mode, unsigned int protocol)
{
  serial2Stop();

  serial2Mode = mode;

  switch (mode) {
    case UART_MODE_TELEMETRY_MIRROR:
      uart3Setup(FRSKY_SPORT_BAUDRATE, false);
      break;
#if defined(DEBUG) || defined(CLI)
    case UART_MODE_DEBUG:
      uart3Setup(DEBUG_BAUDRATE, false);
      break;
#endif
    case UART_MODE_TELEMETRY:
      if (protocol == PROTOCOL_FRSKY_D_SECONDARY) {
        uart3Setup(FRSKY_D_BAUDRATE, true);
      }
      break;
  }
}

void serial2Putc(char c)
{
#if !defined(SIMU)
  int n = 0;
  while (serial2TxFifo.isFull()) {
    delay_ms(1);
    if (++n > 100) return;
  }
  serial2TxFifo.push(c);
  USART_ITConfig(SERIAL_USART, USART_IT_TXE, ENABLE);
#endif
}

void serial2SbusInit()
{
  uart3Setup(SBUS_BAUDRATE, true);
  SERIAL_USART->CR1 |= USART_CR1_M | USART_CR1_PCE ;
}

void serial2Stop()
{
  DMA_DeInit(SERIAL_DMA_Stream_RX);
  USART_DeInit(SERIAL_USART);
}

uint8_t serial2TracesEnabled()
{
#if defined(DEBUG)
  return (serial2Mode == UART_MODE_DEBUG);
#else
  return false;
#endif
}

extern "C" void SERIAL_USART_IRQHandler(void)
{
  DEBUG_INTERRUPT(INT_SER2);
  // Send
  if (USART_GetITStatus(SERIAL_USART, USART_IT_TXE) != RESET) {
    uint8_t txchar;
    if (serial2TxFifo.pop(txchar)) {
      /* Write one byte to the transmit data register */
      USART_SendData(SERIAL_USART, txchar);
    }
    else {
      USART_ITConfig(SERIAL_USART, USART_IT_TXE, DISABLE);
    }
  }

#if defined(CLI)
  if (!(getSelectedUsbMode() == USB_SERIAL_MODE)) {
    // Receive
    uint32_t status = SERIAL_USART->SR;
    while (status & (USART_FLAG_RXNE | USART_FLAG_ERRORS)) {
      uint8_t data = SERIAL_USART->DR;
      if (!(status & USART_FLAG_ERRORS)) {
        switch (serial2Mode) {
          case UART_MODE_DEBUG:
            cliRxFifo.push(data);
            break;
        }
      }
      status = SERIAL_USART->SR;
    }
  }
#endif
}
