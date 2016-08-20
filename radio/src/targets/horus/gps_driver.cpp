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

Fifo<uint8_t, 64> gpsRxFifo;

void gpsInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  // RX and TX pins
  GPIO_PinAFConfig(GPIOA, GPS_TX_GPIO_PinSource, GPS_GPIO_AF);
  GPIO_PinAFConfig(GPIOA, GPS_RX_GPIO_PinSource, GPS_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = (GPS_TX_GPIO_PIN | GPS_RX_GPIO_PIN);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPS_UART_GPIO, &GPIO_InitStructure);
  
  // UART config
  USART_InitStructure.USART_BaudRate = GPS_USART_BAUDRATE;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(GPS_USART, &USART_InitStructure);
  USART_Cmd(GPS_USART, ENABLE);
  
  USART_ITConfig(GPS_USART, USART_IT_RXNE, ENABLE);
  
  NVIC_InitStructure.NVIC_IRQChannel = GPS_USART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x9;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

extern "C" void GPS_USART_IRQHandler(void)
{
#if 0
  // Send
  if (USART_GetITStatus(GPS_USART, USART_IT_TXE) != RESET) {
    uint8_t txchar;
    if (gpsTxFifo.pop(txchar)) {
      /* Write one byte to the transmit data register */
      USART_SendData(GPS_USART, txchar);
    }
    else {
      USART_ITConfig(GPS_USART, USART_IT_TXE, DISABLE);
    }
  }
#endif
  
  // Receive
  uint32_t status = GPS_USART->SR;
  while (status & (USART_FLAG_RXNE | USART_FLAG_ERRORS)) {
    uint8_t data = GPS_USART->DR;
    if (!(status & USART_FLAG_ERRORS)) {
      gpsRxFifo.push(data);
    }
    status = GPS_USART->SR;
  }
}

uint8_t gpsGetByte(uint8_t * byte)
{
  return gpsRxFifo.pop(*byte);
}