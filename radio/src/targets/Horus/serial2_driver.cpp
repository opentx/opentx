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
extern Fifo<32> sbusFifo;

void uart3Setup(unsigned int baudrate)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(SERIAL_RCC_AHB1Periph_GPIO, ENABLE);

  GPIO_PinAFConfig(SERIAL_GPIO, SERIAL_GPIO_PinSource_RX, SERIAL_GPIO_AF);
  GPIO_PinAFConfig(SERIAL_GPIO, SERIAL_GPIO_PinSource_TX, SERIAL_GPIO_AF);

  GPIO_InitStructure.GPIO_Pin = SERIAL_GPIO_PIN_TX | SERIAL_GPIO_PIN_RX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SERIAL_GPIO, &GPIO_InitStructure);
  
  RCC_APB1PeriphClockCmd(SERIAL_RCC_APB1Periph_USART, ENABLE);
  
  USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  
  USART_Init(SERIAL_USART, &USART_InitStructure);
  USART_Cmd(SERIAL_USART, ENABLE);

  USART_ITConfig(SERIAL_USART, USART_IT_RXNE, ENABLE);
  USART_ITConfig(SERIAL_USART, USART_IT_TXE, DISABLE);

  NVIC_SetPriority(SERIAL_USART_IRQn, 7);
  NVIC_EnableIRQ(SERIAL_USART_IRQn);
}

void serial2Init(unsigned int mode, unsigned int protocol)
{
  USART_DeInit(SERIAL_USART);

  serial2Mode = mode;

  switch (mode) {
    case UART_MODE_TELEMETRY_MIRROR:
      uart3Setup(FRSKY_SPORT_BAUDRATE);
      break;
#if !defined(USB_SERIAL) && (defined(DEBUG) || defined(CLI))
    case UART_MODE_DEBUG:
      uart3Setup(DEBUG_BAUDRATE);
      break;
#endif
    case UART_MODE_TELEMETRY:
      if (protocol == PROTOCOL_FRSKY_D_SECONDARY) {
        uart3Setup(FRSKY_D_BAUDRATE);
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
  uart3Setup(100000);
  SERIAL_USART->CR1 |= USART_CR1_M | USART_CR1_PCE ;
}

void serial2Stop()
{
  USART_DeInit(SERIAL_USART);
}

uint8_t serial2TracesEnabled()
{
  return 1; // (serial2Mode == 0);
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
#if defined(CLI)
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

