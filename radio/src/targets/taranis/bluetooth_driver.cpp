/**
 * @file      bluetooth_driver.c
 * @version   V1.0.0    
 * @date      2014-11-10
 * @brief     bluetooth driver for Tananis.    
 * @author    - Adela 
 *            - Robert Zhang <armner@gmail.com>
 *            - 
 */

#include "board_taranis.h"
#include "string.h"
#include "../../fifo.h"

Fifo<200> btTxFifo;
Fifo<200> btRxFifo;

int bt_open()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_AHB1PeriphClockCmd(BT_RCC_AHB1Periph, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

  GPIO_InitStructure.GPIO_Pin = BT_GPIO_PIN_EN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(BT_GPIO_EN, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = BT_GPIO_PIN_BRTS;
  GPIO_Init(BT_GPIO_BRTS, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = BT_GPIO_PIN_TX|BT_GPIO_PIN_RX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(BT_GPIO_TXRX, &GPIO_InitStructure);

  GPIO_PinAFConfig(BT_GPIO_TXRX, BT_GPIO_PinSource_TX, BT_GPIO_AF);
  GPIO_PinAFConfig(BT_GPIO_TXRX, BT_GPIO_PinSource_RX, BT_GPIO_AF);

  USART_InitStructure.USART_BaudRate = 115200;
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

  GPIO_SetBits(BT_GPIO_BRTS, BT_GPIO_PIN_BRTS);
  GPIO_ResetBits(BT_GPIO_EN, BT_GPIO_PIN_EN); // open bluetooth

  return 0;
}

uint8_t bt_state = 0;

extern "C" void USART6_IRQHandler(void)
{
#if 1
  if (USART_GetITStatus(BT_USART, USART_IT_RXNE) != RESET) {
    USART_ClearITPendingBit(BT_USART, USART_IT_RXNE);
    uint8_t byte = USART_ReceiveData(BT_USART);
    btRxFifo.push(byte);
  }
#endif

#if 1
  if (USART_GetITStatus(BT_USART, USART_IT_TXE) != RESET) {
    uint8_t byte;
    bool result = btTxFifo.pop(byte);
    if (result) {
      USART_SendData(BT_USART, byte);
    }
    else {
      USART_ITConfig(BT_USART, USART_IT_TXE, DISABLE);
      GPIO_SetBits(BT_GPIO_BRTS, BT_GPIO_PIN_BRTS);
      bt_state = 0;
    }
  }
#endif
}

int bt_write(const void *buffer, int len)
{
  uint8_t *data = (uint8_t *)buffer;
  for (int i=0; i<len; ++i) {
    btTxFifo.push(data[i]);
  }
  return 0;
}

void bt_wakeup(void)
{
  if (bt_state < 2 && !btTxFifo.empty()) {
    if (bt_state == 0) {
      GPIO_ResetBits(BT_GPIO_BRTS, BT_GPIO_PIN_BRTS);
      bt_state = 1;
    }
    else {
      USART_ITConfig(BT_USART, USART_IT_TXE, ENABLE);
      bt_state = 2;
    }
  }
}

int bt_read(void *buffer, int len)
{
  int result = 0;
  uint8_t *data = (uint8_t *)buffer;
  while (1) {
    uint8_t byte;
    if (!btRxFifo.pop(byte))
      return result;
    data[result++] = byte;
    if (result >= len)
      return result;
  }
}

int bt_close()
{
  GPIO_SetBits(BT_GPIO_EN, BT_GPIO_PIN_EN); // close bluetooth
  return 0;
}
