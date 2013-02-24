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
 * open9x is based on code named
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

#include "../open9x.h"

void sportInit(void)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIO_SPORT, ENABLE);

  GPIO_PinAFConfig(GPIO_PIN_SPORT_TXRX, GPIO_PinSource_SPORT_RX, GPIO_AF_SPORT);
  GPIO_PinAFConfig(GPIO_PIN_SPORT_TXRX, GPIO_PinSource_SPORT_TX, GPIO_AF_SPORT);

  GPIO_InitStructure.GPIO_Pin = PIN_SPORT_TX | PIN_SPORT_RX;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_PIN_SPORT_TXRX, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = PIN_SPORT_ON;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIO_PIN_SPORT_ON, &GPIO_InitStructure);
  
  GPIO_ResetBits(GPIO_PIN_SPORT_ON, PIN_SPORT_ON);
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPORT, ENABLE);
  
  USART_InitStructure.USART_BaudRate = SPORT_BAUDRATE;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
  
  USART_Init(SPORT, &USART_InitStructure);
  USART_Cmd(SPORT, ENABLE);
  
  USART_ITConfig(SPORT, USART_IT_RXNE, ENABLE);
  NVIC_EnableIRQ(USART2_IRQn);
}

void sportPutc(const char c)
{
  GPIO_SetBits(GPIO_PIN_SPORT_ON, PIN_SPORT_ON);
  USART_SendData(SPORT, c);
  GPIO_ResetBits(GPIO_PIN_SPORT_ON, PIN_SPORT_ON);
}

#if !defined(SIMU)
extern "C" void USART2_IRQHandler()
{
    uint16_t data;
    
    while(USART_GetFlagStatus(SPORT, USART_FLAG_RXNE)){
      data = USART_ReceiveData(SPORT);
      processSerialData((uint8_t) data);
    }
} 
#endif
