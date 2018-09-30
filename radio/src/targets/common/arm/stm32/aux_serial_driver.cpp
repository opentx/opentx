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

uint8_t auxSerialMode = 0;
Fifo<uint8_t, 512> auxSerialTxFifo;

#if defined(SERIAL_DMA_Stream_RX)
DMAFifo<32> auxSerialRxFifo __DMA (SERIAL_DMA_Stream_RX);
#else
Fifo<uint8_t, 32> auxSerialRxFifo;
#endif

void auxSerialSetup(unsigned int baudrate, bool dma)
{
  GPIO_PinAFConfig(AUX_SERIAL_GPIO, AUX_SERIAL_RX_GPIO_PinSource, AUX_SERIAL_GPIO_AF);
  GPIO_PinAFConfig(AUX_SERIAL_GPIO, AUX_SERIAL_TX_GPIO_PinSource, AUX_SERIAL_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = AUX_SERIAL_TX_GPIO_PIN | AUX_SERIAL_RX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(AUX_SERIAL_GPIO, &GPIO_InitStructure);

  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(AUX_SERIAL_USART, &USART_InitStructure);

#if defined(SERIAL_DMA_Stream_RX)
  if (dma) {
    DMA_InitTypeDef DMA_InitStructure;
    auxSerialRxFifo.clear();
    USART_ITConfig(AUX_SERIAL_USART, USART_IT_RXNE, DISABLE);
    USART_ITConfig(AUX_SERIAL_USART, USART_IT_TXE, DISABLE);
    DMA_InitStructure.DMA_Channel = SERIAL_DMA_Channel_RX;
    DMA_InitStructure.DMA_PeripheralBaseAddr = CONVERT_PTR_UINT(&AUX_SERIAL_USART->DR);
    DMA_InitStructure.DMA_Memory0BaseAddr = CONVERT_PTR_UINT(auxSerialRxFifo.buffer());
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = auxSerialRxFifo.size();
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
    USART_DMACmd(AUX_SERIAL_USART, USART_DMAReq_Rx, ENABLE);
    USART_Cmd(AUX_SERIAL_USART, ENABLE);
    DMA_Cmd(SERIAL_DMA_Stream_RX, ENABLE);
    return;
  }
#endif

  // no DMA ...
  USART_Cmd(AUX_SERIAL_USART, ENABLE);
  //USART_ITConfig(AUX_SERIAL_USART, USART_IT_RXNE, ENABLE);
  USART_ITConfig(AUX_SERIAL_USART, USART_IT_TXE, DISABLE);
  NVIC_SetPriority(AUX_SERIAL_USART_IRQn, 7);
  NVIC_EnableIRQ(AUX_SERIAL_USART_IRQn);

#if defined(AUX_SERIAL_TX_INVERT_GPIO_PIN)
  GPIO_InitStructure.GPIO_Pin = AUX_SERIAL_TX_INVERT_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(AUX_SERIAL_TX_INVERT_GPIO, &GPIO_InitStructure);
  GPIO_ResetBits(AUX_SERIAL_TX_INVERT_GPIO, AUX_SERIAL_TX_INVERT_GPIO_PIN);
#endif

#if defined(AUX_SERIAL_RX_INVERT_GPIO_PIN)
  GPIO_InitStructure.GPIO_Pin = AUX_SERIAL_RX_INVERT_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(AUX_SERIAL_RX_INVERT_GPIO, &GPIO_InitStructure);
  GPIO_ResetBits(AUX_SERIAL_RX_INVERT_GPIO, AUX_SERIAL_RX_INVERT_GPIO_PIN);
#endif
}

void auxSerialInit(unsigned int mode, unsigned int protocol)
{
  auxSerialStop();

  auxSerialMode = mode;

  switch (mode) {
    case UART_MODE_TELEMETRY_MIRROR:
      auxSerialSetup(FRSKY_SPORT_BAUDRATE, false);
      break;
#if defined(DEBUG) || defined(CLI)
    case UART_MODE_DEBUG:
      auxSerialSetup(DEBUG_BAUDRATE, false);
      break;
#endif
    case UART_MODE_TELEMETRY:
      if (protocol == PROTOCOL_FRSKY_D_SECONDARY) {
        auxSerialSetup(FRSKY_D_BAUDRATE, true);
      }
      break;
  }
}


void delay(int count)
{
   for (int i = 50000; i > 0; i--)
   {
       for (; count > 0; count--);
   }
}
void auxSerialPutc(char c)
{
#if !defined(SIMU)
  int n = 0;
  while (auxSerialTxFifo.isFull()) {
    delay_ms(1);
    //delay(10);
    if (++n > 100) return;
  }
  auxSerialTxFifo.push(c);
  USART_ITConfig(AUX_SERIAL_USART, USART_IT_TXE, ENABLE);
#endif
}

void auxSerialSbusInit()
{
  auxSerialSetup(SBUS_BAUDRATE, true);
  AUX_SERIAL_USART->CR1 |= USART_CR1_M | USART_CR1_PCE ;
}

void auxSerialStop()
{
#if defined(SERIAL_DMA_Stream_RX)
  DMA_DeInit(SERIAL_DMA_Stream_RX);
#endif
  USART_DeInit(AUX_SERIAL_USART);
}

uint8_t auxSerialTracesEnabled()
{
#if defined(DEBUG)
  return (auxSerialMode == UART_MODE_DEBUG);
#else
  return false;
#endif
}

extern "C" void AUX_SERIAL_USART_IRQHandler(void)
{
  DEBUG_INTERRUPT(INT_SER2);
  // Send
  if (USART_GetITStatus(AUX_SERIAL_USART, USART_IT_TXE) != RESET) {
    uint8_t txchar;
    if (auxSerialTxFifo.pop(txchar)) {
      /* Write one byte to the transmit data register */
      USART_SendData(AUX_SERIAL_USART, txchar);
    }
    else {
      USART_ITConfig(AUX_SERIAL_USART, USART_IT_TXE, DISABLE);
    }
  }

#if defined(CLI)
  if (!(getSelectedUsbMode() == USB_SERIAL_MODE)) {
    // Receive
    uint32_t status = AUX_SERIAL_USART->SR;
    while (status & (USART_FLAG_RXNE | USART_FLAG_ERRORS)) {
      uint8_t data = AUX_SERIAL_USART->DR;
      if (!(status & USART_FLAG_ERRORS)) {
        switch (auxSerialMode) {
          case UART_MODE_DEBUG:
            cliRxFifo.push(data);
            break;
        }
      }
      status = AUX_SERIAL_USART->SR;
    }
  }
#endif
}
