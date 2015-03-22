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

#define BT_UART                     USART6
#define BT_UART_AF                  GPIO_AF_USART6
#define BT_UART_IRQn                USART6_IRQn

#define BT_UART_GPIO_CLK_TXRX       RCC_AHB1Periph_GPIOG
#define BT_UART_GPIO_PORT_TXRX      GPIOG
#define BT_UART_GPIO_PIN_TX         GPIO_Pin_14
#define BT_UART_GPIO_PIN_RX         GPIO_Pin_9
#define BT_UART_GPIO_CLK_EN         RCC_AHB1Periph_GPIOD
#define BT_UART_GPIO_PORT_EN        GPIOD
#define BT_UART_GPIO_PIN_EN         GPIO_Pin_11
#define BT_UART_GPIO_CLK_BRTS       RCC_AHB1Periph_GPIOB
#define BT_UART_GPIO_PORT_BRTS      GPIOB
#define BT_UART_GPIO_PIN_BRTS       GPIO_Pin_10
#define BT_UART_GPIO_TX_PinSource   GPIO_PinSource14
#define BT_UART_GPIO_RX_PinSource   GPIO_PinSource9

Fifo<200> btTxFifo;
Fifo<200> btRxFifo;

int bt_open()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_AHB1PeriphClockCmd(BT_UART_GPIO_CLK_TXRX, ENABLE);
  RCC_AHB1PeriphClockCmd(BT_UART_GPIO_CLK_EN, ENABLE);
  RCC_AHB1PeriphClockCmd(BT_UART_GPIO_CLK_BRTS, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

  GPIO_InitStructure.GPIO_Pin = BT_UART_GPIO_PIN_EN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(BT_UART_GPIO_PORT_EN, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = BT_UART_GPIO_PIN_BRTS;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(BT_UART_GPIO_PORT_BRTS, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = BT_UART_GPIO_PIN_TX|BT_UART_GPIO_PIN_RX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(BT_UART_GPIO_PORT_TXRX, &GPIO_InitStructure);

  GPIO_PinAFConfig(BT_UART_GPIO_PORT_TXRX, BT_UART_GPIO_TX_PinSource, BT_UART_AF);
  GPIO_PinAFConfig(BT_UART_GPIO_PORT_TXRX, BT_UART_GPIO_RX_PinSource, BT_UART_AF);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(BT_UART, &USART_InitStructure);

  USART_Cmd(BT_UART, ENABLE);
  USART_ITConfig(BT_UART, USART_IT_RXNE, ENABLE);

  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = BT_UART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  GPIO_SetBits(BT_UART_GPIO_PORT_BRTS, BT_UART_GPIO_PIN_BRTS);
  GPIO_ResetBits(BT_UART_GPIO_PORT_EN, BT_UART_GPIO_PIN_EN); // open bluetooth

  return 0;
}

uint8_t bt_state = 0;

extern "C" void USART6_IRQHandler(void)
{
#if 1
  if (USART_GetITStatus(BT_UART, USART_IT_RXNE) != RESET) {
    USART_ClearITPendingBit(BT_UART, USART_IT_RXNE);
    uint8_t byte = USART_ReceiveData(BT_UART);
    btRxFifo.push(byte);
  }
#endif

#if 1
  if (USART_GetITStatus(BT_UART, USART_IT_TXE) != RESET) {
    uint8_t byte;
    bool result = btTxFifo.pop(byte);
    if (result) {
      USART_SendData(BT_UART, byte);
    }
    else {
      USART_ITConfig(BT_UART, USART_IT_TXE, DISABLE);
      GPIO_SetBits(BT_UART_GPIO_PORT_BRTS, BT_UART_GPIO_PIN_BRTS);
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
      GPIO_ResetBits(BT_UART_GPIO_PORT_BRTS, BT_UART_GPIO_PIN_BRTS);
      bt_state = 1;
    }
    else {
      USART_ITConfig(BT_UART, USART_IT_TXE, ENABLE);
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
  GPIO_SetBits(BT_UART_GPIO_PORT_EN, BT_UART_GPIO_PIN_EN); // close bluetooth
  return 0;
}
