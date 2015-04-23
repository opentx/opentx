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

// ADC driver
void adcInit();
void adcPrepareBandgap();
void getADC();
void getADC_bandgap();

// USART driver (static register dispatcher)
#define RXD_DDR1 DDRD
#define RXD_DDR_PIN1 DDD2
#define RXD_PORT1 PORTD
#define RXD_PORT_PIN1 PORTD2
#define RXD_DDR0 DDRE
#define RXD_DDR_PIN0 DDE0
#define RXD_PORT0 PORTE
#define RXD_PORT_PIN0 PORTE0

#define _DOR_N(usart_no) DOR ## usart_no
#define _FE_N(usart_no) FE ## usart_no
#define _RXCIE_N(usart_no) RXCIE ## usart_no
#define _RXC_N(usart_no) RXC ## usart_no
#define _RXD_DDR_N(usart_no) RXD_DDR ## usart_no
#define _RXD_DDR_PIN_N(usart_no) RXD_DDR ## usart_no
#define _RXD_PORT_N(usart_no) RXD_DDR ## usart_no
#define _RXD_PORT_PIN_N(usart_no) RXD_DDR ## usart_no
#define _RXEN_N(usart_no) RXEN ## usart_no
#define _TXCIE_N(usart_no) TXCIE ## usart_no
#define _TXEN_N(usart_no) TXEN ## usart_no
#define _U2X_N(usart_no) U2X ## usart_no
#define _UBRRH_N(usart_no) UBRR ## usart_no ## H
#define _UBRRL_N(usart_no) UBRR ## usart_no ## L
#define _UCSRA_N(usart_no) UCSR ## usart_no ## A
#define _UCSRB_N(usart_no) UCSR ## usart_no ## B
#define _UCSRC_N(usart_no) UCSR ## usart_no ## C
#define _UCSZ0_N(usart_no) UCSZ ## usart_no ## 0
#define _UCSZ1_N(usart_no) UCSZ ## usart_no ## 1
#define _UCSZ2_N(usart_no) UCSZ ## usart_no ## 2
#define _UDRIE_N(usart_no) UDRIE ## usart_no
#define _UDR_N(usart_no) UDR ## usart_no
#define _UPE_N(usart_no) UPE ## usart_no
#define _USART_RX_vect_N(usart_no) USART ## usart_no ## _RX_vect
#define _USART_UDRE_vect_N(usart_no) USART ## usart_no ## _UDRE_vect

#define DOR_N(usart_no) _DOR_N(usart_no)
#define FE_N(usart_no) _FE_N(usart_no)
#define RXCIE_N(usart_no) _RXCIE_N(usart_no)
#define RXC_N(usart_no) _RXC_N(usart_no)
#define RXD_DDR_N(usart_no) _RXD_DDR_N(usart_no)
#define RXD_DDR_PIN_N(usart_no) _RXD_DDR_PIN_N(usart_no)
#define RXD_PORT_N(usart_no) _RXD_PORT_N(usart_no)
#define RXD_PORT_PIN_N(usart_no) _RXD_PORT_PIN_N(usart_no)
#define RXEN_N(usart_no) _RXEN_N(usart_no)
#define TXCIE_N(usart_no) _TXCIE_N(usart_no)
#define TXEN_N(usart_no) _TXEN_N(usart_no)
#define U2X_N(usart_no) _U2X_N(usart_no)
#define UBRRH_N(usart_no) _UBRRH_N(usart_no)
#define UBRRL_N(usart_no) _UBRRL_N(usart_no)
#define UCSRA_N(usart_no) _UCSRA_N(usart_no)
#define UCSRB_N(usart_no) _UCSRB_N(usart_no)
#define UCSRC_N(usart_no) _UCSRC_N(usart_no)
#define UCSZ0_N(usart_no) _UCSZ0_N(usart_no)
#define UCSZ1_N(usart_no) _UCSZ1_N(usart_no)
#define UCSZ2_N(usart_no) _UCSZ2_N(usart_no)
#define UDRIE_N(usart_no) _UDRIE_N(usart_no)
#define UDR_N(usart_no) _UDR_N(usart_no)
#define UPE_N(usart_no) _UPE_N(usart_no)
#define USART_RX_vect_N(usart_no) _USART_RX_vect_N(usart_no)
#define USART_UDRE_vect_N(usart_no) _USART_UDRE_vect_N(usart_no)

// Telemetry driver
#if defined(TELEMETRY_MOD_14051_SWAPPED)
#define TLM_USART 1
#else
#define TLM_USART 0
#endif
void telemetryPortInit();
void telemetryTransmitBuffer();
