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

void btInit()
{
  uint32_t baudrate = (g_eeGeneral.btBaudrate==0 ? 115200 : (g_eeGeneral.btBaudrate==1 ? 9600 : 19200));
  UART3_Configure(baudrate, Master_frequency);
}

class Fifo32
{
  public:
    Fifo32():
      widx(0),
      ridx(0)
    {
    }

    void push(uint8_t byte) {
      uint32_t next = (widx+1) & 0x1f;
      if (next != ridx) {
        fifo[widx] = byte;
        widx = next;
      }
    }

    bool pop(uint8_t & byte) {
      if (ridx == widx) {
        return false;
      }
      else {
        byte = fifo[ridx];
        ridx = (ridx+1) & 0x1f;
        return true;
      }
    }

  protected:
    uint8_t fifo[32];
    volatile uint32_t widx;
    volatile uint32_t ridx;
};

OS_FlagID btFlag;
Fifo32 btFifo;

struct t_serial_tx btTx ;
uint8_t btTxBuffer[32] ;

extern struct t_serial_tx * Current_bt ;

uint32_t txPdcBt(struct t_serial_tx *data)
{
#ifndef SIMU
  Uart *pUart = BT_USART ;

  if (pUart->UART_TNCR == 0) {
    Current_bt = data;
    data->ready = 1;
    pUart->UART_TPR = (uint32_t) data->buffer;
    pUart->UART_TCR = data->size;
    pUart->UART_PTCR = US_PTCR_TXTEN;
    pUart->UART_IER = UART_IER_TXBUFE;
    NVIC_EnableIRQ(UART1_IRQn);
    return 1; // Sent OK
  }
#endif
  return 0 ;  // Busy
}


void btSendBuffer()
{
  btTx.buffer = btTxBuffer;
  txPdcBt(&btTx);
  while (btTx.ready == 1) {
    CoTickDelay(1); // 2ms for now
  }
  btTx.size = 0;
}

void btPollDevice()
{
#if 0
  uint16_t x;
  uint32_t y;
  uint16_t rxchar;

  x = 'O';
#endif

  btTxBuffer[0] = 'A';
  btTxBuffer[1] = 'T';
  btTx.size = 2;
  btSendBuffer();

#if 0
  for (y = 0; y < 300; y += 1) {
    if ((rxchar = rxBtuart()) != 0xFFFF) {
      if (rxchar == x) {
        if (x == 'O') {
          x = 'K';
        }
        else {
          break; // Found "OK"
        }
      }
    }
    else {
      CoTickDelay(1); // 2mS
    }
  }
  if (y < 300) {
    return 1;
  }
  else {
    return 0;
  }
#endif
}


/*
Commands to BT module
AT+VERSION      Returns the software version of the module
AT+BAUDx        Sets the baud rate of the module:
1       1200
2       2400
3       4800
4       9600 (Default)
5       19200
6       38400
7       57600
8       115200
9       230400
AT+NAME<name here>      Sets the name of the module

Any name can be specified up to 20 characters
AT+PINxxxx      Sets the pairing password of the device

Any 4 digit number can be used, the default pincode is 1234
AT+PN   Sets the parity of the module

So we could send AT+VERSION at different baudrates until we get a response
Then we can change the baudrate to the required value.
Or maybe just AT and get OK back
*/

void btTask(void* pdata)
{
  uint8_t byte;

  btFlag = CoCreateFlag(true, false);
  btTx.size = 0;

// Look for BT module baudrate, try 115200, and 9600
// Already initialised to g_eeGeneral.bt_baudrate
// 0 : 115200, 1 : 9600, 2 : 19200

  btPollDevice(); // Do we get a response?

  while (1) {
    uint32_t x = CoWaitForSingleFlag(btFlag, 10); // Wait for data in Fifo
    if (x == E_OK) {
      // We have some data in the Fifo
      while (btFifo.pop(byte)) {
        btTxBuffer[btTx.size++] = byte;
        if (btTx.size > 31) {
          btSendBuffer();
        }
      }
    }
    else if (btTx.size) {
      btSendBuffer();
    }
  }
}

void btPushByte(uint8_t data)
{
  btFifo.push(data);
  CoSetFlag(btFlag); // Tell the Bt task something to do
}
