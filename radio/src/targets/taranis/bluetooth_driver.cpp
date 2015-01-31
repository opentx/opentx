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

#define GPIO_EN_PIN          GPIO_Pin_10
#define GPIO_EN_PORT	     GPIOG

#define GPIO_EN_GPIO_CLK     RCC_AHB1Periph_GPIOG

#define GPIO_BRTS_GPIO_PIN   GPIO_Pin_10
#define GPIO_BRTS_GPIO_PORT  GPIOB
#define GPIO_BRTS_GPIO_CLK   RCC_AHB1Periph_GPIOB

#define GPIO_BCTS_GPIO_PIN   GPIO_Pin_11
#define GPIO_BCTS_GPIO_PORT  GPIOB
#define GPIO_BCTS_GPIO_CLK   RCC_AHB1Periph_GPIOB

#define BT_UART              USART1
#define BT_UART_CLK	     RCC_APB2Periph_USART1
#define BT_UART_AF           GPIO_AF_USART1
#define BT_UART_IRQn         USART1_IRQn

#define BT_UART_GPIO_PIN_TX  GPIO_Pin_6
#define BT_UART_GPIO_PIN_RX  GPIO_Pin_7
#define BT_UART_GPIO_TX_PinSource   GPIO_PinSource6
#define BT_UART_GPIO_RX_PinSource   GPIO_PinSource7
#define BT_UART_GPIO_PORT    GPIOB

Fifo<200> btTxFifo;
Fifo<200> btRxFifo;

int bt_open()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  RCC_AHB1PeriphClockCmd( GPIO_BRTS_GPIO_CLK, ENABLE);
  RCC_AHB1PeriphClockCmd( GPIO_EN_GPIO_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(BT_UART_CLK, ENABLE);

  //PG10:as bluetooth EN
  GPIO_InitStructure.GPIO_Pin = GPIO_EN_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_EN_PORT, &GPIO_InitStructure);

  GPIO_SetBits(GPIO_EN_PORT, GPIO_EN_PIN);

  //PB10,PB11
  GPIO_InitStructure.GPIO_Pin = GPIO_BRTS_GPIO_PIN | GPIO_BCTS_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_BRTS_GPIO_PORT, &GPIO_InitStructure);

  //PB6:as TXD
  //PB7:as RXD
  GPIO_PinAFConfig(BT_UART_GPIO_PORT, BT_UART_GPIO_TX_PinSource, BT_UART_AF);
  GPIO_PinAFConfig(BT_UART_GPIO_PORT, BT_UART_GPIO_RX_PinSource, BT_UART_AF);

  GPIO_InitStructure.GPIO_Pin = BT_UART_GPIO_PIN_TX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(BT_UART_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = BT_UART_GPIO_PIN_RX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(BT_UART_GPIO_PORT, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_HardwareFlowControl =
  USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(BT_UART, &USART_InitStructure);

  USART_Cmd(BT_UART, ENABLE);
  USART_ITConfig(BT_UART, USART_IT_RXNE, ENABLE);

  GPIO_ResetBits(GPIO_EN_PORT, GPIO_EN_PIN);	//open bluetooth

  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = BT_UART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  return 0;
}

extern "C" void USART1_IRQHandler(void)
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
    }
  }
#endif
}

int bt_write(void *buffer, int len)
{
  uint8_t *data = (uint8_t *)buffer;
  for (int i=0; i<len; ++i) {
    btTxFifo.push(data[i]);
  }
  GPIO_ResetBits(GPIO_EN_PORT, GPIO_EN_PIN);
  GPIO_ResetBits(GPIO_BRTS_GPIO_PORT, GPIO_BRTS_GPIO_PIN);
  USART_ITConfig(BT_UART, USART_IT_TXE, ENABLE);
  return 0;
}

int bt_read(void *buffer, int len)
{
  int result = 0;
  uint8_t *data = (uint8_t *)buffer;
  GPIO_ResetBits(GPIO_BCTS_GPIO_PORT, GPIO_BCTS_GPIO_PIN);
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
  GPIO_SetBits(GPIO_EN_PORT, GPIO_EN_PIN);
  return 0;
}
