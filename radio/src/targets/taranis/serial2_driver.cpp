/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "../../opentx.h"

uint8_t serial2Mode = 0;
Fifo<512> serial2TxFifo;
extern Fifo<512> telemetryFifo;

static uint8_t dma_buf[32];
static uint8_t dma_read_pos;

#define USART3DMAStream DMA1_Stream1

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
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SERIAL_GPIO, &GPIO_InitStructure);
  
  USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  
  USART_Init(SERIAL_USART, &USART_InitStructure);
  if (!dma) {
    USART_Cmd(SERIAL_USART, ENABLE);
    USART_ITConfig(SERIAL_USART, USART_IT_RXNE, ENABLE);
    USART_ITConfig(SERIAL_USART, USART_IT_TXE, DISABLE);
    NVIC_EnableIRQ(SERIAL_USART_IRQn);
    NVIC_SetPriority(SERIAL_USART_IRQn, 7);
  }
  else {
    DMA_InitTypeDef DMA_InitStructure;
    dma_read_pos = 0;
    USART_ITConfig(SERIAL_USART, USART_IT_RXNE, DISABLE);
    USART_ITConfig(SERIAL_USART, USART_IT_TXE, DISABLE);
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr = CONVERT_PTR_UINT(&SERIAL_USART->DR);
    DMA_InitStructure.DMA_Memory0BaseAddr = CONVERT_PTR_UINT(dma_buf);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = sizeof(dma_buf);
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
    DMA_Init(USART3DMAStream, &DMA_InitStructure);
    USART_DMACmd(SERIAL_USART, USART_DMAReq_Rx, ENABLE);
    USART_Cmd(SERIAL_USART, ENABLE);
    DMA_Cmd(USART3DMAStream, ENABLE);
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
#if !defined(USB_SERIAL) && (defined(DEBUG) || defined(CLI))
    case UART_MODE_DEBUG:
      uart3Setup(DEBUG_BAUDRATE, false);
      break;
#endif
    case UART_MODE_TELEMETRY:
      if (protocol == PROTOCOL_FRSKY_D_SECONDARY) {
        uart3Setup(FRSKY_D_BAUDRATE, false);
      }
      break;
  }
}

void serial2Putc(char c)
{
  while (serial2TxFifo.isFull());
  serial2TxFifo.push(c);
  USART_ITConfig(SERIAL_USART, USART_IT_TXE, ENABLE);
}

void serial2SbusInit()
{
  dma_read_pos = 0;
  uart3Setup(SBUS_BAUDRATE, true);
  SERIAL_USART->CR1 |= USART_CR1_M | USART_CR1_PCE ;
}

void serial2Stop()
{
  DMA_DeInit(USART3DMAStream);
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

int serial2DMAPoll(uint8_t *ch)
{
  uint32_t n;
  uint32_t pos;

  n = sizeof(dma_buf)-USART3DMAStream->NDTR;
  pos = dma_read_pos;
  if (pos == n) return false;
  *ch = dma_buf[pos];
  pos ++;
  if (pos == sizeof(dma_buf)) pos = 0;
  dma_read_pos = pos;
  return true;
}

#if !defined(SIMU)
extern "C" void SERIAL_USART_IRQHandler(void)
{
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

  // Receive
  uint32_t status = SERIAL_USART->SR;
  while (status & (USART_FLAG_RXNE | USART_FLAG_ERRORS)) {
    uint8_t data = SERIAL_USART->DR;

    if (!(status & USART_FLAG_ERRORS)) {
      switch (serial2Mode) {
        case UART_MODE_TELEMETRY:
          telemetryFifo.push(data);
          break;
        case UART_MODE_SBUS_TRAINER:
          sbusFifo.push(data);
          break;
#if !defined(USB_SERIAL) && defined(CLI)
        case UART_MODE_DEBUG:
          cliRxFifo.push(data);
          break;
#endif
      }
    }

    status = SERIAL_USART->SR;
  }
}
#endif

