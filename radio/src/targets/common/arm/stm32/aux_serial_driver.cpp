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
#include "targets/horus/board.h"

#if defined(AUX_SERIAL)
uint8_t auxSerialMode = 0;
Fifo<uint8_t, 512> auxSerialTxFifo;
AuxSerialRxFifo auxSerialRxFifo __DMA (AUX_SERIAL_DMA_Stream_RX);

void auxSerialSetup(unsigned int baudrate, bool dma)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_PinAFConfig(AUX_SERIAL_GPIO, AUX_SERIAL_GPIO_PinSource_RX, AUX_SERIAL_GPIO_AF);
  GPIO_PinAFConfig(AUX_SERIAL_GPIO, AUX_SERIAL_GPIO_PinSource_TX, AUX_SERIAL_GPIO_AF);

  GPIO_InitStructure.GPIO_Pin = AUX_SERIAL_GPIO_PIN_TX | AUX_SERIAL_GPIO_PIN_RX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(AUX_SERIAL_GPIO, &GPIO_InitStructure);

#if defined(AUX_SERIAL_PWR_GPIO)
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = AUX_SERIAL_PWR_GPIO_PIN;
  GPIO_Init(AUX_SERIAL_PWR_GPIO, &GPIO_InitStructure);
#endif

  USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(AUX_SERIAL_USART, &USART_InitStructure);

  if (dma) {
    DMA_InitTypeDef DMA_InitStructure;
    auxSerialRxFifo.clear();
    USART_ITConfig(AUX_SERIAL_USART, USART_IT_RXNE, DISABLE);
    USART_ITConfig(AUX_SERIAL_USART, USART_IT_TXE, DISABLE);
    DMA_InitStructure.DMA_Channel = AUX_SERIAL_DMA_Channel_RX;
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
    DMA_Init(AUX_SERIAL_DMA_Stream_RX, &DMA_InitStructure);
    USART_DMACmd(AUX_SERIAL_USART, USART_DMAReq_Rx, ENABLE);
    USART_Cmd(AUX_SERIAL_USART, ENABLE);
    DMA_Cmd(AUX_SERIAL_DMA_Stream_RX, ENABLE);
  }
  else {
    USART_Cmd(AUX_SERIAL_USART, ENABLE);
    USART_ITConfig(AUX_SERIAL_USART, USART_IT_RXNE, ENABLE);
    USART_ITConfig(AUX_SERIAL_USART, USART_IT_TXE, DISABLE);
    NVIC_SetPriority(AUX_SERIAL_USART_IRQn, 7);
    NVIC_EnableIRQ(AUX_SERIAL_USART_IRQn);
  }
}

void auxSerialInit(unsigned int mode, unsigned int protocol)
{
  auxSerialStop();

  auxSerialMode = mode;

  switch (mode) {
    case UART_MODE_TELEMETRY_MIRROR:
#if defined(CROSSFIRE)
      if (protocol == PROTOCOL_TELEMETRY_CROSSFIRE) {
        auxSerialSetup(CROSSFIRE_TELEM_MIRROR_BAUDRATE, false);
        break;
      }
#endif
      auxSerialSetup(FRSKY_SPORT_BAUDRATE, false);
      break;

#if defined(DEBUG) || defined(CLI)
    case UART_MODE_DEBUG:
      auxSerialSetup(DEBUG_BAUDRATE, false);
      break;
#endif

    case UART_MODE_TELEMETRY:
      if (protocol == PROTOCOL_TELEMETRY_FRSKY_D_SECONDARY) {
        auxSerialSetup(FRSKY_D_BAUDRATE, true);
      }
      break;

    case UART_MODE_LUA:
      auxSerialSetup(DEBUG_BAUDRATE, false);
  }
}

void auxSerialPutc(char c)
{
#if !defined(SIMU)
  int n = 0;
  while (auxSerialTxFifo.isFull()) {
    delay_ms(1);
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
  AUX_SERIAL_POWER_ON();
}

void auxSerialStop()
{
  DMA_DeInit(AUX_SERIAL_DMA_Stream_RX);
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

#if !defined(SIMU)
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
  if (getSelectedUsbMode() != USB_SERIAL_MODE) {
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
#endif
#endif // AUX_SERIAL
