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

uint8_t jetiRxBuffer[32];
uint8_t jetiReady;
uint16_t jetiKeys;

void telemetryInterrupt10ms()
{
  if (jetiReady) {
    jetiKeys = JETI_KEY_NOCHANGE;
    if (switchState(KEY_UP)) jetiKeys &= JETI_KEY_UP;
    if (switchState(KEY_DOWN)) jetiKeys &= JETI_KEY_DOWN;
    if (switchState(KEY_LEFT)) jetiKeys &= JETI_KEY_LEFT;
    if (switchState(KEY_RIGHT)) jetiKeys &= JETI_KEY_RIGHT;

    jetiReady = 0;    // invalidate buffer

    JETI_EnableTXD();
  }
}

#ifndef SIMU
ISR(USART0_UDRE_vect)
{
  if (jetiKeys != 0xff) {
    UCSR0B &= ~(1 << TXB80);

    if (jetiKeys & 0x0100)
      UCSR0B |= (1 << TXB80);

    UDR0 = jetiKeys;

    jetiKeys = 0xff;
  }
  else {
    JETI_DisableTXD();
  }
}

ISR (USART0_RX_vect)
{
  uint8_t stat;
  uint8_t rh;
  uint8_t rl;
  static uint8_t jbp;

  stat = UCSR0A;
  rh = UCSR0B;
  rl = UDR0;

  if (stat & ((1 << FE0) | (1 << DOR0) | (1 << UPE0))) {
    // discard buffer and start new on any error
    jetiReady = 0;
    jbp = 0;
  }
  else if ((rh & (1 << RXB80)) == 0) {
    // control
    if (rl == 0xfe) {
      // start condition
      jetiReady = 0;
      jbp = 0;
    }
    else if (rl == 0xff) {
      // stop condition
      jetiReady = 1;
    }
  }
  else {
    // data
    if (jbp < 32) {
      if (rl==0xDF)
        jetiRxBuffer[jbp++] = '@'; //@ => °  Issue 163
      else
        jetiRxBuffer[jbp++] = rl;
    }
  }
}
#endif

void JETI_Init (void)
{
  DDRE  &= ~(1 << DDE0);          // set RXD0 pin as input
  PORTE &= ~(1 << PORTE0);        // disable pullup on RXD0 pin

#ifndef SIMU

#undef BAUD
#define BAUD 9600
#include <util/setbaud.h>
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;

  UCSR0A &= ~(1 << U2X0); // disable double speed operation

  // set 9O1
  UCSR0C = (1 << UPM01) | (1 << UPM00) | (1 << UCSZ01) | (1 << UCSZ00);
  UCSR0B = (1 << UCSZ02);
        
  // flush receive buffer
  while ( UCSR0A & (1 << RXC0) ) UDR0;

#endif
}

void JETI_EnableTXD (void)
{
  UCSR0B |=  (1 << TXEN0);        // enable TX
  UCSR0B |=  (1 << UDRIE0);       // enable UDRE0 interrupt
}

void JETI_DisableTXD (void)
{
  UCSR0B &= ~(1 << TXEN0);        // disable TX
  UCSR0B &= ~(1 << UDRIE0);       // disable UDRE0 interrupt
}

void JETI_EnableRXD (void)
{
  UCSR0B |=  (1 << RXEN0);        // enable RX
  UCSR0B |=  (1 << RXCIE0);       // enable Interrupt
}

void JETI_DisableRXD (void)
{
  UCSR0B &= ~(1 << RXEN0);        // disable RX
  UCSR0B &= ~(1 << RXCIE0);       // disable Interrupt
}

#if 0
void JETI_putw (uint16_t c)
{
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UCSR0B &= ~(1 << TXB80);
        if (c & 0x0100)
        {
                UCSR0B |= (1 << TXB80);
        }
        UDR0 = c;
}

void JETI_putc (uint8_t c)
{
        loop_until_bit_is_set(UCSR0A, UDRE0);
        //      UCSRB &= ~(1 << TXB8);
        UCSR0B |= (1 << TXB80);
        UDR0 = c;
}

void JETI_puts (char *s)
{
        while (*s)
        {
                JETI_putc (*s);
                s++;
        }
}

void JETI_put_start (void)
{
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UCSR0B &= ~(1 << TXB80);
        UDR0 = 0xFE;
}

void JETI_put_stop (void)
{
        loop_until_bit_is_set(UCSR0A, UDRE0);
        UCSR0B &= ~(1 << TXB80);
        UDR0 = 0xFF;
}
#endif

void menuViewTelemetryJeti(event_t event)
{
  drawTelemetryTopBar();

  for (uint8_t i=0; i<16; i++) {
    lcdDrawChar((i+2)*FW, 3*FH, jetiRxBuffer[i], BSS);
    lcdDrawChar((i+2)*FW, 4*FH, jetiRxBuffer[i+16], BSS);
  }

  if (event == EVT_KEY_FIRST(KEY_EXIT)) {
    JETI_DisableRXD();
    jetiReady = 0;
    chainMenu(menuMainView);
  }
}
