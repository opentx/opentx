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

#include "../../opentx.h"

extern Fifo<512> telemetryFifo;

void telemetryPortInit(uint32_t baudrate)
{
  if (baudrate == 0) {
    USART_DeInit(TELEMETRY_USART);
    return;
  }

  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_PinAFConfig(TELEMETRY_GPIO, TELEMETRY_GPIO_PinSource_RX, TELEMETRY_GPIO_AF);
  GPIO_PinAFConfig(TELEMETRY_GPIO, TELEMETRY_GPIO_PinSource_TX, TELEMETRY_GPIO_AF);

  GPIO_InitStructure.GPIO_Pin = TELEMETRY_TX_GPIO_PIN | TELEMETRY_RX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
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
  USART_Cmd(TELEMETRY_USART, ENABLE);
  USART_ITConfig(TELEMETRY_USART, USART_IT_RXNE, ENABLE);

  NVIC_SetPriority(TELEMETRY_USART_IRQn, 6);
  NVIC_EnableIRQ(TELEMETRY_USART_IRQn);
}

struct SportTxBuffer
{
  uint8_t *ptr;
  uint32_t count;
} sportTxBuffer;

void telemetryPortSetDirectionOutput()
{
  TELEMETRY_GPIO_DIR->BSRRL = TELEMETRY_DIR_GPIO_PIN;     // output enable
  TELEMETRY_USART->CR1 &= ~USART_CR1_RE;           // turn off receiver
}

void sportSendBuffer(uint8_t *buffer, uint32_t count)
{
  sportTxBuffer.ptr = buffer;
  sportTxBuffer.count = count;
  telemetryPortSetDirectionOutput();
  TELEMETRY_USART->CR1 |= USART_CR1_TXEIE ;
}

#if !defined(SIMU)
#define USART_FLAG_ERRORS (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)

extern "C" void TELEMETRY_USART_IRQHandler()
{
  uint32_t status;
  uint8_t data;

  status = TELEMETRY_USART->SR;

  if (status & USART_SR_TXE) {
    if (sportTxBuffer.count) {
      TELEMETRY_USART->DR = *sportTxBuffer.ptr++;
      if (--sportTxBuffer.count == 0) {
        TELEMETRY_USART->CR1 &= ~USART_CR1_TXEIE;   // stop Tx interrupt
        TELEMETRY_USART->CR1 |= USART_CR1_TCIE;     // enable complete interrupt
      }
    }
  }
	
  if ((status & USART_SR_TC) && (TELEMETRY_USART->CR1 & USART_CR1_TCIE)) {
    TELEMETRY_USART->CR1 &= ~USART_CR1_TCIE ;	// stop Complete interrupt
    TELEMETRY_GPIO_DIR->BSRRH = TELEMETRY_DIR_GPIO_PIN ;	// output disable
    TELEMETRY_USART->CR1 |= USART_CR1_RE ;
    while (status & (USART_FLAG_RXNE)) {
      status = TELEMETRY_USART->DR;
      status = TELEMETRY_USART->SR ;
    }
  }
	
  while (status & (USART_FLAG_RXNE | USART_FLAG_ERRORS)) {
    data = TELEMETRY_USART->DR;
    if (!(status & USART_FLAG_ERRORS)) {
      telemetryFifo.push(data);
    }
    status = TELEMETRY_USART->SR;
  }
}
#endif
