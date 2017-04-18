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

Fifo<uint8_t, 64> btTxFifo;
Fifo<uint8_t, 64> btRxFifo;

enum BluetoothState
{
  BLUETOOTH_INIT,
  BLUETOOTH_WAIT_TTM,
  BLUETOOTH_WAIT_BAUDRATE_CHANGE,
  BLUETOOTH_OK,
};

enum BluetoothWriteState
{
  BLUETOOTH_WRITE_IDLE,
  BLUETOOTH_WRITE_INIT,
  BLUETOOTH_WRITING,
  BLUETOOTH_WRITE_DONE
};

volatile uint8_t bluetoothState = BLUETOOTH_INIT;
volatile uint8_t bluetoothWriteState = BLUETOOTH_WRITE_IDLE;

void bluetoothInit(uint32_t baudrate)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  USART_DeInit(BT_USART);

  RCC_AHB1PeriphClockCmd(BT_RCC_AHB1Periph, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = BT_EN_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(BT_EN_GPIO, &GPIO_InitStructure);

#if defined(BT_BRTS_GPIO_PIN)
  GPIO_InitStructure.GPIO_Pin = BT_BRTS_GPIO_PIN;
  GPIO_Init(BT_BRTS_GPIO, &GPIO_InitStructure);
  GPIO_SetBits(BT_BRTS_GPIO, BT_BRTS_GPIO_PIN);
#endif

#if defined(BT_BCTS_GPIO_PIN)
  GPIO_InitStructure.GPIO_Pin = BT_BCTS_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(BT_BCTS_GPIO, &GPIO_InitStructure);
#endif

  GPIO_InitStructure.GPIO_Pin = BT_TX_GPIO_PIN|BT_RX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(BT_GPIO_TXRX, &GPIO_InitStructure);

  GPIO_PinAFConfig(BT_GPIO_TXRX, BT_TX_GPIO_PinSource, BT_GPIO_AF);
  GPIO_PinAFConfig(BT_GPIO_TXRX, BT_RX_GPIO_PinSource, BT_GPIO_AF);

  USART_DeInit(BT_USART);
  USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(BT_USART, &USART_InitStructure);

  USART_Cmd(BT_USART, ENABLE);
  USART_ITConfig(BT_USART, USART_IT_RXNE, ENABLE);

  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = BT_USART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  GPIO_ResetBits(BT_EN_GPIO, BT_EN_GPIO_PIN); // open bluetooth
}

void bluetoothDone()
{
  GPIO_SetBits(BT_EN_GPIO, BT_EN_GPIO_PIN); // close bluetooth
}

extern "C" void USART6_IRQHandler(void)
{
  DEBUG_INTERRUPT(INT_BLUETOOTH);
  if (USART_GetITStatus(BT_USART, USART_IT_RXNE) != RESET) {
    USART_ClearITPendingBit(BT_USART, USART_IT_RXNE);
    uint8_t byte = USART_ReceiveData(BT_USART);
    btRxFifo.push(byte);
  }

  if (USART_GetITStatus(BT_USART, USART_IT_TXE) != RESET) {
    uint8_t byte;
    bool result = btTxFifo.pop(byte);
    if (result) {
      USART_SendData(BT_USART, byte);
    }
    else {
      USART_ITConfig(BT_USART, USART_IT_TXE, DISABLE);
      bluetoothWriteState = BLUETOOTH_WRITE_DONE;
    }
  }
}

void bluetoothWrite(const void * buffer, int len)
{
  uint8_t * data = (uint8_t *)buffer;
  for (int i=0; i<len; ++i) {
    btTxFifo.push(data[i]);
  }
}

void bluetoothWriteString(const char * str)
{
  while (*str != 0) {
    btTxFifo.push(*str++);
  }
}

void bluetoothWriteWakeup(void)
{
  if (bluetoothWriteState == BLUETOOTH_WRITE_IDLE) {
    if (!btTxFifo.isEmpty()) {
      bluetoothWriteState = BLUETOOTH_WRITE_INIT;
#if defined(BT_BRTS_GPIO_PIN)
      GPIO_ResetBits(BT_BRTS_GPIO, BT_BRTS_GPIO_PIN);
#endif
    }
  }
  else if (bluetoothWriteState == BLUETOOTH_WRITE_INIT) {
    bluetoothWriteState = BLUETOOTH_WRITING;
    USART_ITConfig(BT_USART, USART_IT_TXE, ENABLE);
  }
  else if (bluetoothWriteState == BLUETOOTH_WRITE_DONE) {
    bluetoothWriteState = BLUETOOTH_WRITE_IDLE;
#if defined(BT_BRTS_GPIO_PIN)
    GPIO_SetBits(BT_BRTS_GPIO, BT_BRTS_GPIO_PIN);
#endif
  }
}

void bluetoothWakeup(void)
{
  if (!g_eeGeneral.bluetoothEnable) {
    if (bluetoothState != BLUETOOTH_INIT) {
      bluetoothDone();
      bluetoothState = BLUETOOTH_INIT;
    }
  }
  else {
    if (bluetoothState != BLUETOOTH_OK) {
      static tmr10ms_t waitEnd = 0;

      if (bluetoothState == BLUETOOTH_INIT) {
        bluetoothInit(BLUETOOTH_DEFAULT_BAUDRATE);
        const char btMessage[] = "TTM:REN-";
        bluetoothWriteString(btMessage);
        uint8_t len = ZLEN(g_eeGeneral.bluetoothName);
        for (int i=0; i<len; i++) {
          btTxFifo.push(idx2char(g_eeGeneral.bluetoothName[i]));
        }
        bluetoothState = BLUETOOTH_WAIT_TTM;
        waitEnd = get_tmr10ms() + 25; // 250ms
      }
      else if (bluetoothState == BLUETOOTH_WAIT_TTM) {
        if (get_tmr10ms() > waitEnd) {
          char ttm[] = "TTM:REN";
          int index = 0;
          uint8_t c;
          bool found = false;
          while (btRxFifo.pop(c)) {
            if (c == ttm[index]) {
              index++;
              if (index == sizeof(ttm)-1) {
                found = true;
                break;
              }
            }
            else {
              index = 0;
            }
          }
          if (found) {
            bluetoothState = BLUETOOTH_OK;
          }
          else {
            bluetoothInit(BLUETOOTH_FACTORY_BAUDRATE);
            const char btMessage[] = "TTM:BPS-115200";
            bluetoothWriteString(btMessage);
            bluetoothState = BLUETOOTH_WAIT_BAUDRATE_CHANGE;
            waitEnd = get_tmr10ms() + 250; // 2.5s
          }
        }
      }
      else if (bluetoothState == BLUETOOTH_WAIT_BAUDRATE_CHANGE) {
        if (get_tmr10ms() > waitEnd) {
          bluetoothState = BLUETOOTH_INIT;
        }
      }
    }

    bluetoothWriteWakeup();
  }
}

uint8_t bluetoothReady()
{
  return (bluetoothState == BLUETOOTH_OK);
}

int bluetoothRead(void * buffer, int len)
{
  int result = 0;
  uint8_t * data = (uint8_t *)buffer;
  while (result < len) {
    uint8_t byte;
    if (!btRxFifo.pop(byte)) {
      break;
    }
    data[result++] = byte;
  }
  return result;
}
