/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
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
#include <stdarg.h>

/*
 * Outputs a character on the UART line.
 *
 * This function is synchronous (i.e. uses polling).
 * c  Character to send.
 */
void debugPutc(const char c)
{
  Uart *pUart = CONSOLE_USART;

  /* Wait for the transmitter to be ready */
  while ( (pUart->UART_SR & UART_SR_TXEMPTY) == 0 ) ;

  /* Send character */
  pUart->UART_THR = c;
}

// Outputs a string to the UART
void debugPuts(const char *format, ...)
{
  va_list arglist;
  char tmp[256];

  va_start(arglist, format);
  vsnprintf(tmp, 256, format, arglist);
  va_end(arglist);

  const char *t = tmp;
  while (*t) {
    debugPutc(*t++);
  }
}

void dump(unsigned char *data, unsigned int size)
{
 debugPuts("DUMP %d bytes ...\n\r", size);
 unsigned int i = 0, j=0;
 while (i*32+j < size) {
   debugPuts("%.2X ", data[i*32+j]);
   j++;
   if (j==32) {
     i++; j=0;
     debugPuts("\n\r");
   }
 }
 debugPuts("\n\r");
}

Fifo32 debugFifo;

extern "C" void UART0_IRQHandler()
{
  debugFifo.push(CONSOLE_USART->UART_RHR);
}


void debugTask(void* pdata)
{
  uint8_t rxchar ;

  for (;;) {
    while (!debugFifo.pop(rxchar))
      CoTickDelay(5); // 10ms

  }
}
