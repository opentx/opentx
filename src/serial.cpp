/*
 * Authors - Gerard Valade <gerard.valade@gmail.com>
 *
 * Adapted from frsky
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 */

#include "opentx.h"
#include "serial.h"

/*
 Receive serial (RS-232) characters, detecting and storing each Fr-Sky
 0x7e-framed packet as it arrives.  When a complete packet has been
 received, process its data into storage variables.  NOTE: This is an
 interrupt routine and should not get too lengthy. I originally had
 the buffer being checked in the perMain function (because per10ms
 isn't quite often enough for data streaming at 9600baud) but alas
 that scheme lost packets also. So each packet is parsed as it arrives,
 directly at the ISR function (through a call to frskyProcessPacket).

 If this proves a problem in the future, then I'll just have to implement
 a second buffer to receive data while one buffer is being processed (slowly).
 */

#ifndef SIMU
ISR (USART0_RX_vect)
{
	uint8_t iostat; //USART control and Status Register 0 A
	// uint8_t rh; //USART control and Status Register 0 B
	UCSR0B &= ~(1 << RXCIE0); // disable Interrupt
	sei();
	iostat = UCSR0A; //USART control and Status Register 0 A
	uint8_t byte = UDR0;

	/*
	 bit 	7		6		5		4	3		2		1		0
	 RxC0	TxC0	UDRE0	FE0	DOR0	UPE0	U2X0	MPCM0

	 RxC0: 	Receive complete
	 TXC0: 	Transmit Complete
	 UDRE0: 	USART Data Register Empty
	 FE0:	Frame Error
	 DOR0:	Data OverRun
	 UPE0:	USART Parity Error
	 U2X0:	Double Tx Speed
	 MPCM0:	MultiProcessor Comms Mode
	 */
	if (iostat & ((1 << FE0) | (1 << DOR0) | (1 << UPE0))) {
		byte = 0;
	}
	//rh = UCSR0B; //USART control and Status Register 0 B
#ifdef MAVLINK
	(RXHandler)(byte);
#endif

	cli();
	UCSR0B |= (1 << RXCIE0); // enable Interrupt

}
#endif

inline void SERIAL_EnableRXD(void) {
	UCSR0B |= (1 << RXEN0); // enable RX
	UCSR0B |= (1 << RXCIE0); // enable Interrupt
}

#if 0
void SERIAL_DisableRXD(void) {
	UCSR0B &= ~(1 << RXEN0); // disable RX
	UCSR0B &= ~(1 << RXCIE0); // disable Interrupt
}
#endif

/*
 USART0 (transmit) Data Register Emtpy ISR
 Usef to transmit FrSky data packets, which are buffered in frskyTXBuffer.
 */
uint8_t serialTxBuffer[MAX_TX_BUFFER]; // 32 characters
uint8_t serialTxBufferCount = 0;
uint8_t * ptrTxISR = 0;
serial_tx_state_t serialTxState = TX_STATE_EMPTY;

#ifndef SIMU
ISR(USART0_UDRE_vect)
{
	if (serialTxBufferCount > 0) {
		UDR0 = *ptrTxISR++;
		serialTxBufferCount--;
	} else {
		UCSR0B &= ~(1 << UDRIE0); // disable UDRE0 interrupt
		serialTxState = TX_STATE_EMPTY;
	}
}
#endif

void SERIAL_start_uart_send() {
	ptrTxISR = serialTxBuffer;
	serialTxBufferCount = 0;
}

void SERIAL_end_uart_send() {
	ptrTxISR = serialTxBuffer;
	//UCSR0B |= (1 << UDRIE0); // enable  UDRE0 interrupt
	serialTxState = TX_STATE_READY;
}

void SERIAL_send_uart_bytes(const uint8_t * buf, uint16_t len) {
	while (len--) {
		*ptrTxISR++ = *buf++;
		serialTxBufferCount++;
	}

}

#if 0
void SERIAL_transmitBuffer(uint8_t len) {
	serialTxBufferCount = len;
	ptrTxISR = serialTxBuffer;
	//UCSR0B |= (1 << UDRIE0); // enable  UDRE0 interrupt
	serialTxState = TX_STATE_READY;
}
#endif

void SERIAL_startTX(void) {
	if (serialTxState == TX_STATE_READY) {
		serialTxState = TX_STATE_BUSY;
		UCSR0B |= (1 << UDRIE0); // enable  UDRE0 interrupt
	}
}

static void uart_4800(void) {
#ifndef SIMU
#undef BAUD  // avoid compiler warning
#define BAUD 4800
#include <util/setbaud.h>
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
#if USE_2X
  UCSR0A |= (1 << U2X0);
#else
	UCSR0A &= ~(1 << U2X0);
#endif
#endif
}

static void uart_9600(void) {
#ifndef SIMU
#undef BAUD  // avoid compiler warning
#define BAUD 9600
#include <util/setbaud.h>
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
#if USE_2X
  UCSR0A |= (1 << U2X0);
#else
	UCSR0A &= ~(1 << U2X0);
#endif
#endif
}

static void uart_14400(void) {
#ifndef SIMU
#undef BAUD  // avoid compiler warning
#define BAUD 14400
#include <util/setbaud.h>
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
#if USE_2X
  UCSR0A |= (1 << U2X0);
#else
	UCSR0A &= ~(1 << U2X0);
#endif
#endif
}

static void uart_19200(void) {
#ifndef SIMU
#undef BAUD  // avoid compiler warning
#define BAUD 19200
#include <util/setbaud.h>
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
#if USE_2X
  UCSR0A |= (1 << U2X0);
#else
	UCSR0A &= ~(1 << U2X0);
#endif
#endif
}

static void uart_38400(void) {
#ifndef SIMU
#undef BAUD  // avoid compiler warning
#define BAUD 38400
#include <util/setbaud.h>
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
#if USE_2X
  UCSR0A |= (1 << U2X0);
#else
  UCSR0A &= ~(1 << U2X0);
#endif
#endif
}

static void uart_57600(void) {
#ifndef SIMU
#undef BAUD  // avoid compiler warning
#define BAUD 57600
#include <util/setbaud.h>
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
#if USE_2X
  UCSR0A |= (1 << U2X0);
#else
	UCSR0A &= ~(1 << U2X0);
#endif
#endif
}


static void uart_76800(void) {
#ifndef SIMU
#undef BAUD  // avoid compiler warning
#define BAUD 76800
#include <util/setbaud.h>
  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
#if USE_2X
  UCSR0A |= (1 << U2X0);
#else
	UCSR0A &= ~(1 << U2X0);
#endif
#endif
}

inline void SERIAL_EnableTXD(void) {
	//UCSR0B |= (1 << TXEN0); // enable TX
	UCSR0B |= (1 << TXEN0) | (1 << UDRIE0); // enable TX and TX interrupt
}

#if 0
void SERIAL_DisableTXD(void) {
	UCSR0B &= ~(1 << TXEN0); // disable TX
	UCSR0B &= ~(1 << UDRIE0); // disable Interrupt
}
#endif

void SERIAL_Init(void) {
	DDRE &= ~(1 << DDE0); // set RXD0 pin as input
	PORTE &= ~(1 << PORTE0); // disable pullup on RXD0 pin

	switch (g_eeGeneral.mavbaud) {
	case BAUD_4800:
		uart_4800();
		break;
	case BAUD_9600:
		uart_9600();
		break;
	case BAUD_14400:
		uart_14400();
		break;
	case BAUD_19200:
		uart_19200();
		break;
	case BAUD_38400:
		uart_38400();
		break;
	case BAUD_57600:
		uart_57600();
		break;
	case BAUD_76800:
		uart_76800();
		break;
	}


	// UCSR0A &= ~(1 << U2X0); // disable double speed operation
	// set 8N1
	UCSR0B = 0 | (0 << RXCIE0) | (0 << TXCIE0) | (0 << UDRIE0) | (0 << RXEN0) | (0 << TXEN0) | (0 << UCSZ02);
	UCSR0C = 0 | (1 << UCSZ01) | (1 << UCSZ00);
	while (UCSR0A & (1 << RXC0))
	  UDR0; // flush receive buffer

	SERIAL_EnableTXD();
	SERIAL_EnableRXD();
}

