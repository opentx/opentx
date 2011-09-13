/*
 * Authors (alpahbetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 *
 * Original contributors
 * - Philip Moss Adapted first frsky functions from jeti.cpp code by
 * - Karl Szmutny <shadow@privy.de>

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

#include "gruvin9x.h"
#include "frsky.h"

// Enumerate FrSky packet codes
#define LINKPKT         0xfe
#define USRPKT          0xfd
#define A11PKT          0xfc
#define A12PKT          0xfb
#define A21PKT          0xfa
#define A22PKT          0xf9
#define ALRM_REQUEST    0xf8
#define RSSI1PKT        0xf7
#define RSSI2PKT        0xf6

#define START_STOP      0x7e
#define BYTESTUFF       0x7d
#define STUFF_MASK      0x20

uint8_t frskyRxBuffer[19];   // Receive buffer. 9 bytes (full packet), worst case 18 bytes with byte-stuffing (+1)
uint8_t frskyTxBuffer[19];   // Ditto for transmit buffer
uint8_t frskyTxBufferCount = 0;
uint8_t FrskyRxBufferReady = 0;
uint8_t frskyStreaming = 0;
uint8_t frskyTxISRIndex = 0;

FrskyData frskyTelemetry[2];
FrskyData frskyRSSI[2];

struct FrskyAlarm {
  uint8_t level;    // The alarm's 'urgency' level. 0=disabled, 1=yellow, 2=orange, 3=red
  uint8_t greater;  // 1 = 'if greater than'. 0 = 'if less than'
  uint8_t value;    // The threshold above or below which the alarm will sound
};
struct FrskyAlarm frskyAlarms[4];

void frskyPushValue(uint8_t & i, uint8_t value);

/*
   Called from somewhere in the main loop or a low prioirty interrupt
   routine perhaps. This funtcion processes Fr-Sky telemetry data packets
   assembled byt he USART0_RX_vect) ISR function (below) and stores
   extracted data in global variables for use by other parts of the program.

   Packets can be any of the following:

    - A1/A2/RSSI telemtry data
    - Alarm level/mode/threshold settings for Ch1A, Ch1B, Ch2A, Ch2B
    - User Data packets

   User Data packets are not yet implementedi (they are simply ignored), 
   but will likely one day contain the likes of GPS long/lat/alt/speed, 
   AoA, airspeed, etc.
*/

void processFrskyPacket(uint8_t *packet)
{
  // What type of packet?
  switch (packet[0])
  {
    case A22PKT:
    case A21PKT:
    case A12PKT:
    case A11PKT:
      {
        struct FrskyAlarm *alarmptr ;
        alarmptr = &frskyAlarms[(packet[0]-A22PKT)] ;
        alarmptr->value = packet[1];
        alarmptr->greater = packet[2] & 0x01;
        alarmptr->level = packet[3] & 0x03;
      }
      break;
    case LINKPKT: // A1/A2/RSSI values
      frskyTelemetry[0].set(packet[1]);
      frskyTelemetry[1].set(packet[2]);
      frskyRSSI[0].set(packet[3]);
      frskyRSSI[1].set(packet[4] / 2);
      break;

    case USRPKT: // User Data packet -- not yet implemented
      break;
  }

  FrskyRxBufferReady = 0;
  frskyStreaming = FRSKY_TIMEOUT10ms; // reset counter only if valid frsky packets are being detected
}

// Receive buffer state machine state defs
#define frskyDataIdle    0
#define frskyDataStart   1
#define frskyDataInFrame 2
#define frskyDataXOR     3
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
ISR(USART0_RX_vect)
{
  uint8_t stat;
  uint8_t data;
  
  static uint8_t numPktBytes = 0;
  static uint8_t dataState = frskyDataIdle;
  
	UCSR0B &= ~(1 << RXCIE0); // disable Interrupt
	sei() ;
  stat = UCSR0A; // USART control and Status Register 0 A

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
    // rh = UCSR0B; //USART control and Status Register 0 B

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

  data = UDR0; // USART data register 0

  if (stat & ((1 << FE0) | (1 << DOR0) | (1 << UPE0)))
  { // discard buffer and start fresh on any comms error
    FrskyRxBufferReady = 0;
    numPktBytes = 0;
  } 
  else
  {
    if (FrskyRxBufferReady == 0) // can't get more data if the buffer hasn't been cleared
    {
      switch (dataState) 
      {
        case frskyDataStart:
          if (data == START_STOP) break; // Remain in userDataStart if possible 0x7e,0x7e doublet found.

          if (numPktBytes < 19)
            frskyRxBuffer[numPktBytes++] = data;
          dataState = frskyDataInFrame;
          break;

        case frskyDataInFrame:
          if (data == BYTESTUFF)
          { 
              dataState = frskyDataXOR; // XOR next byte
              break; 
          }
          if (data == START_STOP) // end of frame detected
          {
            processFrskyPacket(frskyRxBuffer); // FrskyRxBufferReady = 1;
            dataState = frskyDataIdle;
            break;
          }
          frskyRxBuffer[numPktBytes++] = data;
          break;

        case frskyDataXOR:
          if (numPktBytes < 19)
            frskyRxBuffer[numPktBytes++] = data ^ STUFF_MASK;
          dataState = frskyDataInFrame;
          break;

        case frskyDataIdle:
          if (data == START_STOP)
          {
            numPktBytes = 0;
            dataState = frskyDataStart;
          }
          break;

      } // switch
    } // if (FrskyRxBufferReady == 0)
  }
  cli() ;
  UCSR0B |= (1 << RXCIE0); // enable Interrupt
}

/*
   USART0 (transmit) Data Register Emtpy ISR
   Usef to transmit FrSky data packets, which are buffered in frskyTXBuffer. 
*/

ISR(USART0_UDRE_vect)
{
  if (frskyTxBufferCount > 0) {
    UDR0 = frskyTxBuffer[frskyTxISRIndex++];
    frskyTxBufferCount--;
  }
  else {
    UCSR0B &= ~(1 << UDRIE0); // disable UDRE0 interrupt
  }
}
#endif

/******************************************/

void frskyTransmitBuffer()
{
  frskyTxISRIndex = 0;
  UCSR0B |= (1 << UDRIE0); // enable  UDRE0 interrupt
}


uint8_t FrskyAlarmSendState = 0 ;
uint8_t FrskyDelay = 0 ;


void FRSKY10mspoll(void)
{
  if (FrskyDelay)
  {
    FrskyDelay -= 1 ;
    return ;
  }

  if (frskyTxBufferCount)
  {
    return; // we only have one buffer. If it's in use, then we can't send yet.
  }

  // Now send a packet
  {
    FrskyAlarmSendState -= 1 ;
    uint8_t channel = 1 - (FrskyAlarmSendState / 2);
    uint8_t alarm = 1 - (FrskyAlarmSendState % 2);
    
    uint8_t i = 0;
    frskyTxBuffer[i++] = START_STOP; // Start of packet
    frskyTxBuffer[i++] = (A22PKT + FrskyAlarmSendState); // fc - fb - fa - f9
    frskyPushValue(i, g_model.frsky.channels[channel].alarms_value[alarm]);
    {
      uint8_t *ptr ;
      ptr = &frskyTxBuffer[i] ;
      *ptr++ = ALARM_GREATER(channel, alarm);
      *ptr++ = ALARM_LEVEL(channel, alarm);
      *ptr++ = 0x00 ;
      *ptr++ = 0x00 ;
      *ptr++ = 0x00 ;
      *ptr++ = 0x00 ;
      *ptr++ = 0x00 ;
      *ptr++ = START_STOP;        // End of packet
      i += 8 ;
    }
    FrskyDelay = 5 ; // 50mS
    frskyTxBufferCount = i;
    frskyTransmitBuffer(); 
  }
}

// Send packet requesting all alarm settings be sent back to us
void FRSKY_setRSSIAlarms(void)
{
  if (frskyTxBufferCount) return; // we only have one buffer. If it's in use, then we can't send. Sorry.

  uint8_t i = 0;

  for (int alarm=0; alarm<2; alarm++) {
    frskyTxBuffer[i++] = START_STOP;        // Start of packet
    frskyTxBuffer[i++] = (RSSI1PKT-alarm);  // f7 - f6
    frskyPushValue(i, g_eeGeneral.frskyRssiAlarms[alarm].value+50-(10*i));
    {
      uint8_t *ptr ;
      ptr = &frskyTxBuffer[i] ;
      *ptr++ = 0x00 ;
      *ptr++ = g_eeGeneral.frskyRssiAlarms[alarm].level;
      *ptr++ = 0x00 ;
      *ptr++ = 0x00 ;
      *ptr++ = 0x00 ;
      *ptr++ = 0x00 ;
      *ptr++ = 0x00 ;
      *ptr++ = START_STOP;        // End of packet
      i += 8 ;
    }
  }

  frskyTxBufferCount = i;
  frskyTransmitBuffer(); 
}

bool FRSKY_alarmRaised(uint8_t idx)
{
  for (int i=0; i<2; i++) {
    if (ALARM_LEVEL(idx, i) != alarm_off) {
      if (ALARM_GREATER(idx, i)) {
        if (frskyTelemetry[idx].value > g_model.frsky.channels[idx].alarms_value[i])
          return true;
      }
      else {
        if (frskyTelemetry[idx].value < g_model.frsky.channels[idx].alarms_value[i])
          return true;
      }
    }
  }
  return false;
}

inline void FRSKY_EnableTXD(void)
{
  frskyTxBufferCount = 0;
  UCSR0B |= (1 << TXEN0) | (1 << UDRIE0); // enable TX and TX interrupt
}

inline void FRSKY_EnableRXD(void)
{

  UCSR0B |= (1 << RXEN0);  // enable RX
  UCSR0B |= (1 << RXCIE0); // enable Interrupt
}

#if 0
void FRSKY_DisableTXD(void)
{
  UCSR0B &= ~((1 << TXEN0) | (1 << UDRIE0)); // disable TX pin and interrupt
}

void FRSKY_DisableRXD(void)
{
  UCSR0B &= ~(1 << RXEN0);  // disable RX
  UCSR0B &= ~(1 << RXCIE0); // disable Interrupt
}
#endif

void FRSKY_Init(void)
{
  // clear frsky variables
  memset(frskyAlarms, 0, sizeof(frskyAlarms));
  resetTelemetry();

  DDRE &= ~(1 << DDE0);    // set RXD0 pin as input
  PORTE &= ~(1 << PORTE0); // disable pullup on RXD0 pin

#undef BAUD
#define BAUD 9600
#ifndef SIMU
#include <util/setbaud.h>

  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
  UCSR0A &= ~(1 << U2X0); // disable double speed operation.

  // set 8 N1
  UCSR0B = 0 | (0 << RXCIE0) | (0 << TXCIE0) | (0 << UDRIE0) | (0 << RXEN0) | (0 << TXEN0) | (0 << UCSZ02);
  UCSR0C = 0 | (1 << UCSZ01) | (1 << UCSZ00);

  
  while (UCSR0A & (1 << RXC0)) UDR0; // flush receive buffer

#endif

  // These should be running right from power up on a FrSky enabled '9X.
  FRSKY_EnableTXD(); // enable FrSky-Telemetry reception
  FRSKY_EnableRXD(); // enable FrSky-Telemetry reception
}

#if 0
// Send packet requesting all alarm settings be sent back to us
void frskyAlarmsRefresh()
{

  if (frskyTxBufferCount) return; // we only have one buffer. If it's in use, then we can't send. Sorry.

  {
    uint8_t *ptr ;
    ptr = &frskyTxBuffer[0] ;
    *ptr++ = START_STOP; // Start of packet
    *ptr++ = ALRM_REQUEST;
    *ptr++ = 0x00 ;
    *ptr++ = 0x00 ;
    *ptr++ = 0x00 ;
    *ptr++ = 0x00 ;
    *ptr++ = 0x00 ;
    *ptr++ = 0x00 ;
    *ptr++ = 0x00 ;
    *ptr++ = 0x00 ;
    *ptr++ = START_STOP;        // End of packet
  }

  frskyTxBufferCount = 11;
  frskyTransmitBuffer();
}
#endif

void frskyPushValue(uint8_t & i, uint8_t value)
{
  // byte stuff the only byte than might need it
  if (value == START_STOP) {
    frskyTxBuffer[i++] = BYTESTUFF;
    frskyTxBuffer[i++] = 0x5e;
  }
  else if (value == BYTESTUFF) {
    frskyTxBuffer[i++] = BYTESTUFF;
    frskyTxBuffer[i++] = 0x5d;
  }
  else {
    frskyTxBuffer[i++] = value;
  }
}

void FrskyData::set(uint8_t value)
{
   this->value = value;
   if (!max || max < value)
     max = value;
   if (!min || min > value)
     min = value;
 }

void resetTelemetry()
{
  memset(frskyTelemetry, 0, sizeof(frskyTelemetry));
  memset(frskyRSSI, 0, sizeof(frskyRSSI));
}

