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
 *
 */

#include "opentx.h"

#if defined(TELEMETRY_FRSKY) || defined(TELEMETRY_MAVLINK)

// KEEP IN SYNC WITH GUI
enum SERIAL_BAUDS
{
  BAUD_4800 = 0,
  BAUD_9600,
  BAUD_14400,
  BAUD_19200,
  BAUD_38400,
  BAUD_57600,
  BAUD_58798,
  BAUD_76800
};

void telemetryEnableTx(void)
{
  UCSRB_N(TLM_USART) |= (1 << TXEN_N(TLM_USART)); // enable TX
}

void telemetryEnableRx(void)
{
  UCSRB_N(TLM_USART) |= (1 << RXEN_N(TLM_USART));  // enable RX
  UCSRB_N(TLM_USART) |= (1 << RXCIE_N(TLM_USART)); // enable Interrupt
}

void processTelemetryData(uint8_t data);
extern uint8_t telemetryRxBufferCount;

ISR(USART_RX_vect_N(TLM_USART))
{
  uint8_t stat;
  uint8_t data;

  UCSRB_N(TLM_USART) &= ~(1 << RXCIE_N(TLM_USART)); // disable Interrupt
  sei();

  stat = UCSRA_N(TLM_USART); // USART control and Status Register 0/1 A

  /*
              bit      7      6      5      4      3      2      1      0
                      RxC0  TxC0  UDRE0    FE0   DOR0   UPE0   U2X0  MPCM0

              RxC0:   Receive complete
              TXC0:   Transmit Complete
              UDRE0:  USART Data Register Empty
              FE0:    Frame Error
              DOR0:   Data OverRun
              UPE0:   USART Parity Error
              U2X0:   Double Tx Speed
              PCM0:   MultiProcessor Comms Mode
   */
  // rh = UCSRB_N(TLM_USART); //USART control and Status Register 0/1 B

    /*
              bit      7      6      5      4      3      2      1      0
                   RXCIE0 TxCIE0 UDRIE0  RXEN0  TXEN0 UCSZ02  RXB80  TXB80

              RxCIE0:   Receive Complete int enable
              TXCIE0:   Transmit Complete int enable
              UDRIE0:   USART Data Register Empty int enable
              RXEN0:    Rx Enable
              TXEN0:    Tx Enable
              UCSZ02:   Character Size bit 2
              RXB80:    Rx data bit 8
              TXB80:    Tx data bit 8
    */

  data = UDR_N(TLM_USART); // USART data register 0

  if (stat & ((1 << FE_N(TLM_USART)) | (1 << DOR_N(TLM_USART)) | (1 << UPE_N(TLM_USART)))) {
    // discard buffer and start fresh on any comms error
    telemetryRxBufferCount = 0;
  }
  else {
    processTelemetryData(data);
  }

  cli() ;
  UCSRB_N(TLM_USART) |= (1 << RXCIE_N(TLM_USART)); // enable Interrupt
}

static void uart_4800(void)
{
  #undef BAUD
  #define BAUD 4800
  #include <util/setbaud.h>
  UBRRH_N(TLM_USART) = UBRRH_VALUE;
  UBRRL_N(TLM_USART) = UBRRL_VALUE;
#if defined USE_2X
  UCSRA_N(TLM_USART) |= (1 << U2X_N(TLM_USART)); // enable double speed operation.
#else
  UCSRA_N(TLM_USART) &= ~(1 << U2X_N(TLM_USART)); // disable double speed operation.
#endif
}

FORCEINLINE void uart_9600(void)
{
  #undef BAUD
  #define BAUD 9600
  #include <util/setbaud.h>
  UBRRH_N(TLM_USART) = UBRRH_VALUE;
  UBRRL_N(TLM_USART) = UBRRL_VALUE;
#if defined USE_2X
  UCSRA_N(TLM_USART) |= (1 << U2X_N(TLM_USART)); // enable double speed operation.
#else
  UCSRA_N(TLM_USART) &= ~(1 << U2X_N(TLM_USART)); // disable double speed operation.
#endif
}

static void uart_14400(void)
{
  #undef BAUD
  #define BAUD 14400
  #include <util/setbaud.h>
  UBRRH_N(TLM_USART) = UBRRH_VALUE;
  UBRRL_N(TLM_USART) = UBRRL_VALUE;
#if defined USE_2X
  UCSRA_N(TLM_USART) |= (1 << U2X_N(TLM_USART)); // enable double speed operation.
#else
  UCSRA_N(TLM_USART) &= ~(1 << U2X_N(TLM_USART)); // disable double speed operation.
#endif
}

static void uart_19200(void)
{
  #undef BAUD
  #define BAUD 19200
  #include <util/setbaud.h>
  UBRRH_N(TLM_USART) = UBRRH_VALUE;
  UBRRL_N(TLM_USART) = UBRRL_VALUE;
#if defined USE_2X
  UCSRA_N(TLM_USART) |= (1 << U2X_N(TLM_USART)); // enable double speed operation.
#else
  UCSRA_N(TLM_USART) &= ~(1 << U2X_N(TLM_USART)); // disable double speed operation.
#endif
}

static void uart_38400(void)
{
  #undef BAUD
  #define BAUD 38400
  #include <util/setbaud.h>
  UBRRH_N(TLM_USART) = UBRRH_VALUE;
  UBRRL_N(TLM_USART) = UBRRL_VALUE;
#if defined USE_2X
  UCSRA_N(TLM_USART) |= (1 << U2X_N(TLM_USART)); // enable double speed operation.
#else
  UCSRA_N(TLM_USART) &= ~(1 << U2X_N(TLM_USART)); // disable double speed operation.
#endif
}

static void uart_57600(void)
{
  #undef BAUD
  #define BAUD 57600
  #include <util/setbaud.h>
  UBRRH_N(TLM_USART) = UBRRH_VALUE;
  UBRRL_N(TLM_USART) = UBRRL_VALUE;
#if defined USE_2X
  UCSRA_N(TLM_USART) |= (1 << U2X_N(TLM_USART)); // enable double speed operation.
#else
  UCSRA_N(TLM_USART) &= ~(1 << U2X_N(TLM_USART)); // disable double speed operation.
#endif
}

static void uart_58798(void)
{
  UBRRH_N(TLM_USART) = 0;
  UBRRL_N(TLM_USART) = 0x010;
  UCSRA_N(TLM_USART) &= ~(1 << U2X_N(TLM_USART)); // disable double speed operation.
}

static void uart_76800(void)
{
  #undef BAUD
  #define BAUD 76800
  #include <util/setbaud.h>
  UBRRH_N(TLM_USART) = UBRRH_VALUE;
  UBRRL_N(TLM_USART) = UBRRL_VALUE;
#if defined USE_2X
  UCSRA_N(TLM_USART) |= (1 << U2X_N(TLM_USART)); // enable double speed operation.
#else
  UCSRA_N(TLM_USART) &= ~(1 << U2X_N(TLM_USART)); // disable double speed operation.
#endif
}

#if !defined(SIMU)
FORCEINLINE void telemetryPortInit(uint8_t baudrate)
{
  RXD_DDR_N(TLM_USART) &= ~(1 << RXD_DDR_PIN_N(TLM_USART));   // set RXD pin as input
  RXD_PORT_N(TLM_USART) &= ~(1 << RXD_PORT_PIN_N(TLM_USART)); // disable pullup on RXD pin
  switch (baudrate) {
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
    case BAUD_58798:
      uart_58798();
      break;
    case BAUD_76800:
      uart_76800();
      break;
    default:
      uart_57600();
      break;
  }

  // set 8N1
  UCSRB_N(TLM_USART) = 0 | (0 << RXCIE_N(TLM_USART)) | (0 << TXCIE_N(TLM_USART)) | (0 << UDRIE_N(TLM_USART)) | (0 << RXEN_N(TLM_USART)) | (0 << TXEN_N(TLM_USART)) | (0 << UCSZ2_N(TLM_USART));
  UCSRC_N(TLM_USART) = 0 | (1 << UCSZ1_N(TLM_USART)) | (1 << UCSZ0_N(TLM_USART));

  while (UCSRA_N(TLM_USART) & (1 << RXC_N(TLM_USART))) UDR_N(TLM_USART); // flush receive buffer

  // These should be running right from power up on a FrSky enabled '9X.
#if defined(TELEMETRY_FRSKY)
  telemetryEnableTx(); // enable FrSky-Telemetry emission
#endif
  telemetryEnableRx(); // enable FrSky-Telemetry reception
}
#endif

void telemetryTransmitBuffer()
{
  UCSRB_N(TLM_USART) |= (1 << UDRIE_N(TLM_USART)); // enable UDRE1 interrupt
}

void telemetryPortInit()
{
  telemetryPortInit(BAUD_9600);
}

#endif
