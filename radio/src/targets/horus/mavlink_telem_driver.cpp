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

Fifo<uint8_t, MAVLINK_TELEM_TX_FIFO_SIZE> mavlinkTelemTxFifo;
Fifo<uint8_t, MAVLINK_TELEM_RX_FIFO_SIZE> mavlinkTelemRxFifo;

uint8_t mavlinkTelemUart = 0;

void _mavlinkTelemUsartSetup(const char uart, uint32_t baudrate)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  if (uart == 'A') {
    mavlinkTelemUart = 1;

    GPIO_PinAFConfig(MAVLINK_TELEM_USART_GPIO, MAVLINK_TELEM_USART_RX_GPIO_PinSource, MAVLINK_TELEM_USART_GPIO_AF);
    GPIO_PinAFConfig(MAVLINK_TELEM_USART_GPIO, MAVLINK_TELEM_USART_TX_GPIO_PinSource, MAVLINK_TELEM_USART_GPIO_AF);

    GPIO_InitStructure.GPIO_Pin = MAVLINK_TELEM_USART_TX_GPIO_PIN | MAVLINK_TELEM_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(MAVLINK_TELEM_USART_GPIO, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(MAVLINK_TELEM_USART, &USART_InitStructure);

    USART_Cmd(MAVLINK_TELEM_USART, ENABLE);

    USART_ITConfig(MAVLINK_TELEM_USART, USART_IT_RXNE, ENABLE);
    USART_ITConfig(MAVLINK_TELEM_USART, USART_IT_TXE, DISABLE);
    NVIC_SetPriority(MAVLINK_TELEM_USART_IRQn, 6);
    NVIC_EnableIRQ(MAVLINK_TELEM_USART_IRQn);
  }
}

void _mavlinkTelemUsartStop(void)
{
  if (mavlinkTelemUart == 1) {
    USART_ITConfig(MAVLINK_TELEM_USART, USART_IT_RXNE, DISABLE);
    USART_ITConfig(MAVLINK_TELEM_USART, USART_IT_TXE, DISABLE);
    USART_DeInit(MAVLINK_TELEM_USART);
    mavlinkTelemUart = 0;
  }
}

void mavlinkTelemInit(const char uart, uint32_t baudrate)
{
  _mavlinkTelemUsartStop();
  mavlinkTelemRxFifo.clear();
  mavlinkTelemTxFifo.clear();
  _mavlinkTelemUsartSetup(uart, baudrate);
}

void mavlinkTelemDeInit(void)
{
  _mavlinkTelemUsartStop();
}

uint32_t mavlinkTelemAvailable(void)
{
  return mavlinkTelemRxFifo.size();
}

bool mavlinkTelemPutc(char c)
{
  if (mavlinkTelemTxFifo.isFull()) {
    return false;
  }
  mavlinkTelemTxFifo.push(c);
  USART_ITConfig(MAVLINK_TELEM_USART, USART_IT_TXE, ENABLE);
  return true;
}

bool mavlinkTelemPutBuf(const uint8_t *buf, const uint16_t count)
{
  if (!buf || !mavlinkTelemTxFifo.hasSpace(count)) {
    return false;
  }
  for (uint16_t i = 0; i < count; i++) {
    uint8_t c = buf[i];
    mavlinkTelemTxFifo.push(c);
  }
  USART_ITConfig(MAVLINK_TELEM_USART, USART_IT_TXE, ENABLE);
  return true;
}

uint8_t mavlinkTelemGetc(uint8_t *c)
{
  return mavlinkTelemRxFifo.pop(*c);
}

void mavlinkTelemWriteWakeup(void)
{
  if (!mavlinkTelemTxFifo.isEmpty()) {
    USART_ITConfig(MAVLINK_TELEM_USART, USART_IT_TXE, ENABLE);
  }
}

extern "C" void MAVLINK_TELEM_USART_IRQHandler(void)
{
  if (USART_GetITStatus(MAVLINK_TELEM_USART, USART_IT_RXNE) != RESET) {
    USART_ClearITPendingBit(MAVLINK_TELEM_USART, USART_IT_RXNE);
    uint8_t c = USART_ReceiveData(MAVLINK_TELEM_USART);
    mavlinkTelemRxFifo.push(c);
  }

  if (USART_GetITStatus(MAVLINK_TELEM_USART, USART_IT_TXE) != RESET) {
    uint8_t c;
    if (mavlinkTelemTxFifo.pop(c)) {
      USART_SendData(MAVLINK_TELEM_USART, c);
    }
    else {
      USART_ITConfig(MAVLINK_TELEM_USART, USART_IT_TXE, DISABLE);
    }
  }
}
