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

#include "open9x.h"
#include "menus.h"

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
#define RSSI_REQUEST    0xf1

#define START_STOP      0x7e
#define BYTESTUFF       0x7d
#define STUFF_MASK      0x20

uint8_t frskyRxBuffer[FRSKY_RX_PACKET_SIZE];   // Receive buffer. 9 bytes (full packet), worst case 18 bytes with byte-stuffing (+1)
uint8_t frskyTxBuffer[FRSKY_TX_PACKET_SIZE];   // Ditto for transmit buffer
uint8_t frskyTxBufferCount = 0;
uint8_t FrskyRxBufferReady = 0;
int8_t frskyStreaming = -1;
uint8_t frskyUsrStreaming = 0;

FrskyData frskyTelemetry[2];
FrskyRSSI frskyRSSI[2];

struct FrskyAlarm {
  uint8_t level;    // The alarm's 'urgency' level. 0=disabled, 1=yellow, 2=orange, 3=red
  uint8_t greater;  // 1 = 'if greater than'. 0 = 'if less than'
  uint8_t value;    // The threshold above or below which the alarm will sound
};

struct FrskyAlarm frskyAlarms[4];

#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
FrskyHubData frskyHubData; // TODO initialization?
uint8_t barsThresholds[BAR_MAX-3];
#endif

void frskyPushValue(uint8_t *&ptr, uint8_t value)
{
  // byte stuff the only byte than might need it
  if (value == START_STOP) {
    *ptr++ = 0x5e;
    *ptr++ = BYTESTUFF;
  }
  else if (value == BYTESTUFF) {
    *ptr++ = 0x5d;
    *ptr++ = BYTESTUFF;
  }
  else {
    *ptr++ = value;
  }
}

#ifdef DISPLAY_USER_DATA
/*
  Copies all available bytes (up to max bufsize) from frskyUserData circular 
  buffer into supplied *buffer. Returns number of bytes copied (or zero)
*/
uint8_t frskyGetUserData(char *buffer, uint8_t bufSize)
{
  uint8_t i = 0;
  while (!frskyUserData.isEmpty())
  {
    buffer[i] = frskyUserData.get();
    i++;
  }
  return i;
}
#endif

#ifdef FRSKY_HUB
inline void getGpsPilotPosition()
{
  frskyHubData.pilotLatitude = (((uint32_t)frskyHubData.gpsLatitude_bp / 100) * 1000000) + (((frskyHubData.gpsLatitude_bp % 100) * 10000 + frskyHubData.gpsLatitude_ap) * 5) / 3;
  frskyHubData.pilotLongitude = (((uint32_t)frskyHubData.gpsLongitude_bp / 100) * 1000000) + (((frskyHubData.gpsLongitude_bp % 100) * 10000 + frskyHubData.gpsLongitude_ap) * 5) / 3;
  uint32_t lat = ((uint32_t)frskyHubData.gpsLatitude_bp / 100) * 100+ (((uint32_t)frskyHubData.gpsLatitude_bp % 100) * 5) / 3;
  uint32_t angle2 = (lat*lat) / 10000;
  uint32_t angle4 = angle2 * angle2;
  frskyHubData.distFromEarthAxis = 139*(((uint32_t)10000000-((angle2*(uint32_t)123370)/81)+(angle4/25))/12500);
  // printf("frskyHubData.distFromEarthAxis=%d\n", frskyHubData.distFromEarthAxis); fflush(stdout);
}

inline void getGpsDistance()
{
  uint32_t lat = (((uint32_t)frskyHubData.gpsLatitude_bp / 100) * 1000000) + (((frskyHubData.gpsLatitude_bp % 100) * 10000 + frskyHubData.gpsLatitude_ap) * 5) / 3;
  uint32_t lng = (((uint32_t)frskyHubData.gpsLongitude_bp / 100) * 1000000) + (((frskyHubData.gpsLongitude_bp % 100) * 10000 + frskyHubData.gpsLongitude_ap) * 5) / 3;

  // printf("lat=%d (%d), long=%d (%d)\n", lat, abs(lat - frskyHubData.pilotLatitude), lng, abs(lng - frskyHubData.pilotLongitude));

  uint32_t angle = (lat > frskyHubData.pilotLatitude) ? lat - frskyHubData.pilotLatitude : frskyHubData.pilotLatitude - lat;
  uint32_t dist = EARTH_RADIUS * angle / 1000000;
  uint32_t result = dist*dist;

  angle = (lng > frskyHubData.pilotLongitude) ? lng - frskyHubData.pilotLongitude : frskyHubData.pilotLongitude - lng;
  dist = frskyHubData.distFromEarthAxis * angle / 1000000;
  result += dist*dist;

  dist = frskyHubData.baroAltitude_bp + frskyHubData.baroAltitudeOffset;
  result += dist*dist;

  frskyHubData.gpsDistance = isqrt32(result);
}

inline int8_t parseTelemHubIndex(uint8_t index)
{
  if (index > 0x39) {
    if (index > 0x3b)
      return -1; // invalid index
    index -= 17;
  }
  return index*2;
}

typedef enum {
  TS_IDLE = 0,  // waiting for 0x5e frame marker
  TS_DATA_ID,   // waiting for dataID
  TS_DATA_LOW,  // waiting for data low byte
  TS_DATA_HIGH, // waiting for data high byte
  TS_XOR = 0x80 // decode stuffed byte
} TS_STATE;

void parseTelemHubByte(uint8_t byte)
{
  static int8_t structPos;
  static uint8_t lowByte;
  static TS_STATE state = TS_IDLE;

  if (byte == 0x5e) {
    state = TS_DATA_ID;
    return;
  }
  if (state == TS_IDLE) {
    return;
  }
  if (state & TS_XOR) {
    byte = byte ^ 0x60;
    state = (TS_STATE)(state - TS_XOR);
  }
  if (byte == 0x5d) {
    state = (TS_STATE)(state | TS_XOR);
    return;
  }
  if (state == TS_DATA_ID) {
    structPos = parseTelemHubIndex(byte);
    state = TS_DATA_LOW;
    if (structPos < 0)
      state = TS_IDLE;
    return;
  }
  if (state == TS_DATA_LOW) {
    lowByte = byte;
    state = TS_DATA_HIGH;
    return;
  }

  if ((uint8_t)structPos == offsetof(FrskyHubData, gpsLatitude_bp)) {
    if (lowByte || byte)
      frskyHubData.gpsFix = 1;
    else if (frskyHubData.gpsFix > 0 && frskyHubData.gpsLatitude_bp > 1)
      frskyHubData.gpsFix = 0;
  }
  if ((uint8_t)structPos == offsetof(FrskyHubData, gpsLongitude_bp)) {
    if (lowByte || byte)
      frskyHubData.gpsFix = 1;
    else if (frskyHubData.gpsFix > 0 && frskyHubData.gpsLongitude_bp > 1)
      frskyHubData.gpsFix = 0;
  }
  
  if ((uint8_t)structPos == offsetof(FrskyHubData, gpsAltitude_bp) ||
      ((uint8_t)structPos >= offsetof(FrskyHubData, gpsAltitude_ap) && (uint8_t)structPos <= offsetof(FrskyHubData, gpsLatitudeNS) && (uint8_t)structPos != offsetof(FrskyHubData, baroAltitude_bp) && (uint8_t)structPos != offsetof(FrskyHubData, baroAltitude_ap))) {
    // If we don't have a fix, we may discard the value
    if (frskyHubData.gpsFix <= 0) {
      state = TS_IDLE;
      return;
    }
  }
  
  ((uint8_t*)&frskyHubData)[structPos] = lowByte;
  ((uint8_t*)&frskyHubData)[structPos+1] = byte;

  if ((uint8_t)structPos == offsetof(FrskyHubData, baroAltitude_bp) && !frskyHubData.baroAltitudeOffset) {
    // First received altitude => Altitude offset
    frskyHubData.baroAltitudeOffset = -frskyHubData.baroAltitude_bp;
  }

  if ((uint8_t)structPos == offsetof(FrskyHubData, gpsAltitude_ap)) {
    if (!frskyHubData.pilotLatitude && !frskyHubData.pilotLongitude) {
      // First received GPS position => Pilot GPS position
      getGpsPilotPosition();
    }
    else if (frskyHubData.gpsDistNeeded || g_menuStack[0] != menuProcFrsky) {
      getGpsDistance();
    }
  }

  if ((uint8_t)structPos == offsetof(FrskyHubData, gpsSpeed_bp)) {
    // Speed => Max speed
    if (frskyHubData.maxGpsSpeed < frskyHubData.gpsSpeed_bp)
      frskyHubData.maxGpsSpeed = frskyHubData.gpsSpeed_bp;
  }

  if ((uint8_t)structPos == offsetof(FrskyHubData, volts)) {
    // Voltage => Cell number + Cell voltage
    uint8_t battnumber = ((frskyHubData.volts & 0x00F0) >> 4);
    if (battnumber < 12) {
      if (frskyHubData.cellsCount < battnumber+1) {
        frskyHubData.cellsCount = battnumber+1;
      }
      uint8_t cellVolts = (uint8_t)(((((frskyHubData.volts & 0xFF00) >> 8) + ((frskyHubData.volts & 0x000F) << 8)))/10);
      frskyHubData.cellVolts[battnumber] = cellVolts;
      if (!frskyHubData.minCellVolts || cellVolts < frskyHubData.minCellVolts)
        frskyHubData.minCellVolts = cellVolts;
    }
  }

  state = TS_IDLE;
}
#endif

#ifdef WS_HOW_HIGH
void parseTelemWSHowHighByte(uint8_t byte)
{
  if (frskyUsrStreaming < (FRSKY_TIMEOUT10ms*3 - 10))  // At least 100mS passed since last data received
    ((uint8_t*)&frskyHubData)[offsetof(FrskyHubData, baroAltitude_bp)] = byte;
  else
    ((uint8_t*)&frskyHubData)[offsetof(FrskyHubData, baroAltitude_bp)+1] = byte;
  frskyUsrStreaming = FRSKY_TIMEOUT10ms*3; // reset counter
}
#endif  

/*
   Called from somewhere in the main loop or a low priority interrupt
   routine perhaps. This function processes FrSky telemetry data packets
   assembled by he USART0_RX_vect) ISR function (below) and stores
   extracted data in global variables for use by other parts of the program.

   Packets can be any of the following:

    - A1/A2/RSSI telemetry data
    - Alarm level/mode/threshold settings for Ch1A, Ch1B, Ch2A, Ch2B
    - User Data packets
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
      frskyStreaming = FRSKY_TIMEOUT10ms; // reset counter only if valid frsky packets are being detected
      break;
#if defined(FRSKY_HUB) || defined (WS_HOW_HIGH)
    case USRPKT: // User Data packet
      uint8_t numBytes = 3 + (packet[1] & 0x07); // sanitize in case of data corruption leading to buffer overflow
      for (uint8_t i=3; i<numBytes; i++) {
#if defined(FRSKY_HUB)
        if (g_model.frsky.usrProto == 1) // FrSky Hub
          parseTelemHubByte(packet[i]);
#endif
#if defined(WS_HOW_HIGH)
        if (g_model.frsky.usrProto == 2) // WS How High
          parseTelemWSHowHighByte(packet[i]);
#endif
      }
      break;
#endif
  }

  FrskyRxBufferReady = 0;
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

NOINLINE void processSerialData(uint8_t stat, uint8_t data)
{
  static uint8_t numPktBytes = 0;
  static uint8_t dataState = frskyDataIdle;

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

            if (numPktBytes < FRSKY_RX_PACKET_SIZE)
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
            if (numPktBytes < FRSKY_RX_PACKET_SIZE)
              frskyRxBuffer[numPktBytes++] = data;
            break;

          case frskyDataXOR:
            if (numPktBytes < FRSKY_RX_PACKET_SIZE)
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
}

ISR(USART0_RX_vect)
{
  uint8_t stat;
  uint8_t data;
  
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

  processSerialData(stat, data);

  cli() ;
  UCSR0B |= (1 << RXCIE0); // enable Interrupt
}

#endif

/******************************************/

void frskyTransmitBuffer()
{
  UCSR0B |= (1 << UDRIE0); // enable  UDRE0 interrupt
}

uint8_t FrskyAlarmSendState = 0 ;
void FRSKY10mspoll(void)
{
  if (frskyTxBufferCount)
    return; // we only have one buffer. If it's in use, then we can't send yet.

  uint8_t *ptr = &frskyTxBuffer[0];

  *ptr++ = START_STOP;        // End of packet
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;

  // Now send a packet
  FrskyAlarmSendState -= 1;
  uint8_t alarm = 1 - (FrskyAlarmSendState % 2);
  if (FrskyAlarmSendState < 4) {
    uint8_t channel = 1 - (FrskyAlarmSendState / 2);
    *ptr++ = (g_eeGeneral.beeperMode != -2/*TODO constant*/ ? ALARM_LEVEL(channel, alarm) : alarm_off);
    *ptr++ = ALARM_GREATER(channel, alarm);
    frskyPushValue(ptr, g_model.frsky.channels[channel].alarms_value[alarm]);
    *ptr++ = (A22PKT + FrskyAlarmSendState); // fc - fb - fa - f9
  }
  else {
    *ptr++ = (g_eeGeneral.beeperMode != -2/*TODO constant*/ ? ((2+alarm+g_model.frskyRssiAlarms[alarm].level) % 4) : alarm_off);
    *ptr++ = 0x00 ;
    frskyPushValue(ptr, 50+g_model.frskyRssiAlarms[alarm].value);
    *ptr++ = (RSSI1PKT-alarm);  // f7 - f6
  }

  *ptr++ = START_STOP; // Start of packet

  frskyTxBufferCount = ptr - &frskyTxBuffer[0];
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
  UCSR0B |= (1 << TXEN0); // enable TX
}

inline void FRSKY_EnableRXD(void)
{
  UCSR0B |= (1 << RXEN0);  // enable RX
  UCSR0B |= (1 << RXCIE0); // enable Interrupt
}

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

  // set 8N1
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

// TODO change names
// Optimize?
void FrskyRSSI::set(uint8_t value)
{
   this->value = (((uint16_t)this->value * 7) + value + 4) / 8;
   if (!min || min > value)
     min = value;
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

#ifdef SIMU
  frskyHubData.gpsLatitude_bp = 4401;
  frskyHubData.gpsLatitude_ap = 7710;
  frskyHubData.gpsLongitude_bp = 1006;
  frskyHubData.gpsLongitude_ap = 8872;
#endif

#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
  frskyHubData.baroAltitudeOffset = -frskyHubData.baroAltitude_bp;
#endif


#if defined(FRSKY_HUB)
  frskyHubData.maxGpsSpeed = 0;
  frskyHubData.cellsCount = 0;
  frskyHubData.minCellVolts = 0;
  frskyHubData.pilotLatitude = 0;
  frskyHubData.pilotLongitude = 0;
  frskyHubData.gpsLatitude_bp = 2;
  frskyHubData.gpsLongitude_bp = 2;
  frskyHubData.gpsFix = -1;
  frskyHubData.gpsDistNeeded = 0;
  frskyHubData.gpsDistance = 0;
#endif

#ifdef SIMU
  frskyTelemetry[0].set(120);
  frskyRSSI[0].set(75);
  frskyHubData.fuelLevel = 75;
  frskyHubData.gpsLatitude_bp = 4401;
  frskyHubData.gpsLatitude_ap = 7455;
  frskyHubData.gpsLongitude_bp = 1006;
  frskyHubData.gpsLongitude_ap = 9533;
  frskyHubData.gpsFix = 1;
  frskyHubData.cellsCount = 6;
#endif
}

#if defined(FRSKY)
void displayRssiLine()
{
  if (frskyStreaming > 0) {
    lcd_hline(0, 54, 128, 0); // separator
    lcd_putsLeft(7*FH+1, STR_TX); lcd_outdezNAtt(4*FW, 7*FH+1, frskyRSSI[1].value, LEADING0, 2);
    lcd_rect(25, 57, 38, 7);
    lcd_filled_rect(26, 58, 9*frskyRSSI[1].value/25, 5);
    lcd_puts(105, 7*FH+1, STR_RX); lcd_outdezNAtt(105+4*FW-1, 7*FH+1, frskyRSSI[0].value, LEADING0, 2);
    lcd_rect(65, 57, 38, 7);
    uint8_t v = 9*frskyRSSI[0].value/25;
    lcd_filled_rect(66+36-v, 58, v, 5);
  }
  else {
    lcd_puts(7*FW, 7*FH+1, STR_NODATA);
    lcd_filled_rect(0, DISPLAY_H-8, DISPLAY_W, 8);
  }
}
#endif

#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
void displayAltitudeLine(uint8_t x, uint8_t y, uint8_t flags)
{
  lcd_putsn(x, y, STR_ALTnDST, 4);
  int16_t value = frskyHubData.baroAltitude_bp + frskyHubData.baroAltitudeOffset;
  putsTelemetryValue(lcd_lastPos, y, value, UNIT_METERS, flags|LEFT);
}
#endif

enum FrskyViews {
  e_frsky_bars,
  e_frsky_a1a2,
#ifdef WS_HOW_HIGH
  e_frsky_ws_how_high,
#endif
#ifdef FRSKY_HUB
  e_frsky_hub = e_frsky_a1a2+1,
  e_frsky_gps
#endif
};

#if defined(FRSKY_HUB) && defined(WS_HOW_HIGH)
#define FRSKY_VIEW_MAX (g_model.frsky.usrProto == 0 ? 1 : ((g_model.frsky.usrProto == 1 && frskyHubData.gpsFix >= 0) ? 3 : 2))
#elif defined(FRSKY_HUB)
#define FRSKY_VIEW_MAX (g_model.frsky.usrProto == 0 ? 1 : (frskyHubData.gpsFix >= 0 ? 3 : 2))
#elif defined(WS_HOW_HIGH)
#define FRSKY_VIEW_MAX (g_model.frsky.usrProto == 0 ? 1 : 2)
#endif

void menuProcFrsky(uint8_t event)
{
  static uint8_t s_frsky_view = e_frsky_bars;

  switch (event) {
    case EVT_KEY_BREAK(KEY_UP):
      if (s_frsky_view-- == 0)
        s_frsky_view = FRSKY_VIEW_MAX;
      break;

    case EVT_KEY_BREAK(KEY_DOWN):
      if (s_frsky_view++ == FRSKY_VIEW_MAX)
        s_frsky_view = 0;
      break;

    case EVT_KEY_FIRST(KEY_EXIT):
      chainMenu(menuMainView);
      break;

    case EVT_KEY_FIRST(KEY_MENU):
      resetTelemetry();
      break;
  }

  // The top black bar
  putsModelName(0, 0, g_model.name, g_eeGeneral.currModel, 0);
  uint8_t att = (g_vbat100mV < g_eeGeneral.vBatWarn ? BLINK : 0);
  putsVBat(14*FW,0,att);
  if (g_model.timer1.mode) {
    att = (s_timerState[0]==TMR_BEEPING ? BLINK : 0);
    putsTime(17*FW, 0, s_timerVal[0], att, att);
  }
  lcd_filled_rect(0, 0, DISPLAY_W, 8);

  if (frskyStreaming >= 0) {
    if (s_frsky_view == e_frsky_bars) {
      // The bars
      uint8_t bars_height = 5;
      for (int8_t i=3; i>=0; i--) {
        if (g_model.frsky.bars[i].source && (51-g_model.frsky.bars[i].barMax) > g_model.frsky.bars[i].barMin) {
          lcd_putsiAtt(0, bars_height+bars_height+1+i*(bars_height+6), STR_VTELEMBARS, g_model.frsky.bars[i].source, 0);
          lcd_rect(25, bars_height+6+i*(bars_height+6), 101, bars_height+2);
          int16_t value = getValue(CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS+g_model.frsky.bars[i].source-1);
          uint8_t threshold = 0, thresholdX = 0;
          if (g_model.frsky.bars[i].source <= 2)
            threshold = g_model.frsky.channels[g_model.frsky.bars[i].source-1].alarms_value[0];
          else
            threshold = barsThresholds[g_model.frsky.bars[i].source-3];
          if (threshold) {
            thresholdX = (uint8_t)(int16_t)((int16_t)100 * (threshold - g_model.frsky.bars[i].barMin * 4) / ((51 - g_model.frsky.bars[i].barMax) * 5 - g_model.frsky.bars[i].barMin * 4));
            if (thresholdX > 100)
              thresholdX = 0;
          }
          uint8_t width = (uint8_t)limit((int16_t)0, (int16_t)((int16_t)100 * (value - g_model.frsky.bars[i].barMin * 5) / ((51 - g_model.frsky.bars[i].barMax) * 4 - g_model.frsky.bars[i].barMin * 4)), (int16_t)100);
          lcd_filled_rect(26, bars_height+6+1+i*(bars_height+6), width, bars_height, (threshold > value) ? DOTTED : SOLID);
          for (uint8_t j=50; j<125; j+=25)
            if (j>26+thresholdX) lcd_vline(j, bars_height+6+1+i*(bars_height+6), bars_height);
          if (thresholdX) {
            lcd_vlineStip(26+thresholdX, bars_height+4+i*(bars_height+6), bars_height+3, DOTTED);
            lcd_hline(25+thresholdX, bars_height+4+i*(bars_height+6), 3);
          }
        }
        else {
          bars_height += 2;
        }
      }
      displayRssiLine();
    }
    else if (s_frsky_view == e_frsky_a1a2) {
      // Big A1 / A2 with min and max
      uint8_t x0, blink;
      if (g_model.frsky.channels[0].ratio || g_model.frsky.channels[1].ratio) {
        x0 = 0;
        for (uint8_t i=0; i<2; i++) {
          if (g_model.frsky.channels[i].ratio) {
            blink = (FRSKY_alarmRaised(i) ? INVERS : 0);
            putsStrIdx(x0, 2*FH, STR_A, i+1, TWO_DOTS);
            x0 += 3*FW;
            putsTelemetryChannel(x0, 2*FH, i, frskyTelemetry[i].value, blink|DBLSIZE|LEFT);
            putsTelemetryChannel(x0+FW, 3*FH, i, frskyTelemetry[i].min, 0);
            putsTelemetryChannel(x0+3*FW, 3*FH, i, frskyTelemetry[i].max, LEFT);
            x0 = 11*FW-2;
          }
        }
      }

#ifdef FRSKY_HUB
      // Cells voltage
      {
        uint8_t x, y;
        x = 3*FW;
        y = 5*FH-3;
        for (uint8_t k=0; k<frskyHubData.cellsCount && k<6; k++) {
          uint8_t attr = (barsThresholds[6/*TODO constant*/] && frskyHubData.cellVolts[k] < barsThresholds[6/*TODO constant*/]) ? BLINK|PREC2|LEFT : PREC2|LEFT;
          if (k == 3) {
            x = 3*FW;
            y = 6*FH-3;
          }
          putsStrIdx(x-3*FW, y, STR_V, k+1, TWO_DOTS);
          lcd_outdezNAtt(x, y, frskyHubData.cellVolts[k] * 2, attr, 4);
          x += 7*FW;
        }
      }
#endif
      displayRssiLine();
    }

#ifdef WS_HOW_HIGH
    else if (g_model.frsky.usrProto == PROTO_WS_HOW_HIGH && s_frsky_view == e_frsky_ws_how_high) {
      displayAltitudeLine(0, 4*FH, DBLSIZE);
      displayRssiLine();
    }
#endif

#ifdef FRSKY_HUB
    else if (s_frsky_view == e_frsky_hub) {
      // Temperature 1
      lcd_putsLeft( 4*FH, STR_TEMP1nTEMP2);
      putsTelemetryValue(4*FW, 4*FH, frskyHubData.temperature1, UNIT_DEGREES, DBLSIZE|LEFT);

      // Temperature 2
      putsTelemetryValue(15*FW, 4*FH, frskyHubData.temperature2, UNIT_DEGREES, DBLSIZE|LEFT);

      // RPM
      lcd_putsLeft(2*FH, STR_RPMnFUEL);
      lcd_outdezNAtt(4*FW, 1*FH, frskyHubData.rpm/(2+g_model.frsky.blades), DBLSIZE|LEFT);

      // Fuel
      putsTelemetryValue(15*FW, 2*FH, frskyHubData.fuelLevel, UNIT_PERCENT, DBLSIZE|LEFT);

      // Altitude (barometric)
      displayAltitudeLine(0, 6*FH, DBLSIZE);

      // Accelerometer
#define ACC_LINE (7*FH+1)
      lcd_putsLeft(ACC_LINE, STR_ACCEL);
      // lcd_puts(4*FW, 7*FH, PSTR("x:"));
      lcd_outdezNAtt(4*FW, ACC_LINE, (int32_t)frskyHubData.accelX/10, LEFT|PREC2);
      // lcd_putc(lcd_lastPos, 7*FH, 'g');
      // lcd_puts(11*FW, 7*FH, PSTR("y:"));
      lcd_outdezNAtt(10*FW, ACC_LINE, (int32_t)frskyHubData.accelY/10, LEFT|PREC2);
      // lcd_putc(lcd_lastPos, 7*FH, 'g');
      // lcd_puts(18*FW, 7*FH, PSTR("z:"));
      lcd_outdezNAtt(16*FW, ACC_LINE, (int32_t)frskyHubData.accelZ/10, LEFT|PREC2);
      // lcd_putc(lcd_lastPos, 7*FH, 'g');
      lcd_filled_rect(0, ACC_LINE-1, DISPLAY_W, 8);
    }
    else if (s_frsky_view == e_frsky_gps) {
#define DATE_LINE (7*FH+1)
      //lcd_putsLeft( DATE_LINE, PSTR("D/T:"));
      // Date
      lcd_outdezNAtt(3*FW, DATE_LINE, frskyHubData.year+2000, LEFT, 4);
      lcd_putc(7*FW-4, DATE_LINE, '-');
      lcd_outdezNAtt(8*FW-4, DATE_LINE, frskyHubData.month, LEFT|LEADING0, 2);
      lcd_putc(10*FW-6, DATE_LINE, '-');
      lcd_outdezNAtt(11*FW-6, DATE_LINE, frskyHubData.day, LEFT|LEADING0, 2);

      // Time
      lcd_outdezNAtt(12*FW+5, DATE_LINE, frskyHubData.hour, LEFT|LEADING0, 2);
      lcd_putc(14*FW+2, DATE_LINE, ':');
      lcd_outdezNAtt(15*FW+2, DATE_LINE, frskyHubData.min, LEFT|LEADING0, 2);
      lcd_putc(17*FW-1, DATE_LINE, ':');
      lcd_outdezNAtt(18*FW-1, DATE_LINE, frskyHubData.sec, LEFT|LEADING0, 2);
      lcd_filled_rect(0, DATE_LINE-1, DISPLAY_W, 8);

      // Latitude
#define LAT_LINE (2*FH-4)
      lcd_putsLeft( LAT_LINE, PSTR("Lat:"));
      lcd_outdezAtt(lcd_lastPos, LAT_LINE,  frskyHubData.gpsLatitude_bp / 100, LEFT); // ddd before '.'
      lcd_putc(lcd_lastPos, LAT_LINE, '@');
      uint8_t mn = frskyHubData.gpsLatitude_bp % 100;
      lcd_outdezNAtt(lcd_lastPos+FW, LAT_LINE, mn, LEFT|LEADING0, 2); // mm before '.'
      lcd_plot(lcd_lastPos, LAT_LINE+FH-2, 0); // small decimal point
      lcd_outdezNAtt(lcd_lastPos+2, LAT_LINE, frskyHubData.gpsLatitude_ap, LEFT|UNSIGN|LEADING0, 4); // after '.'
      lcd_putc(lcd_lastPos+1, LAT_LINE, frskyHubData.gpsLatitudeNS ? frskyHubData.gpsLatitudeNS : '-');

      // Longitude
#define LONG_LINE (3*FH-3)
      lcd_putsLeft(LONG_LINE, PSTR("Lon:"));
      lcd_outdezAtt(lcd_lastPos, LONG_LINE,  frskyHubData.gpsLongitude_bp / 100, LEFT); // ddd before '.'
      lcd_putc(lcd_lastPos, LONG_LINE, '@');
      mn = frskyHubData.gpsLongitude_bp % 100;
      lcd_outdezNAtt(lcd_lastPos+FW, LONG_LINE, mn, LEFT|LEADING0, 2); // mm before '.'
      lcd_plot(lcd_lastPos, LONG_LINE+FH-2, 0); // small decimal point
      lcd_outdezNAtt(lcd_lastPos+2, LONG_LINE, frskyHubData.gpsLongitude_ap, LEFT|UNSIGN|LEADING0, 4); // after '.'
      lcd_putc(lcd_lastPos+1, LONG_LINE, frskyHubData.gpsLongitudeEW ? frskyHubData.gpsLongitudeEW : '-');

#if 0
      // Course / Heading
      lcd_puts(5, 5*FH, STR_HDG);
      lcd_outdezNAtt(lcd_lastPos, 5*FH, frskyHubData.gpsCourse_bp, LEFT|LEADING0, 3); // before '.'
      lcd_plot(lcd_lastPos, 6*FH-2, 0); // small decimal point
      lcd_outdezAtt(lcd_lastPos+2, 5*FH, frskyHubData.gpsCourse_ap, LEFT); // after '.'
      lcd_putc(lcd_lastPos, 5*FH, '@');
#endif

      // Speed
#define SPEED_LINE (6*FH)
      lcd_putsLeft(SPEED_LINE, STR_SPDnMAX);
      putsTelemetryValue(4*FW, SPEED_LINE, frskyHubData.gpsSpeed_bp, UNIT_KTS, LEFT|DBLSIZE); // before '.'
      putsTelemetryValue(16*FW, SPEED_LINE, frskyHubData.maxGpsSpeed, UNIT_KTS, LEFT); // before '.'

      // GPS altitude and distance
#define ALTITUDE_LINE (4*FH-1)
      lcd_putsLeft(ALTITUDE_LINE, STR_ALTnDST);
      putsTelemetryValue(4*FW, ALTITUDE_LINE, frskyHubData.gpsAltitude_bp, UNIT_METERS, LEFT); // before '.'
      putsTelemetryValue(16*FW, ALTITUDE_LINE, frskyHubData.gpsDistance, UNIT_METERS, LEFT); // before '.'
    }
#endif
  }
  else {
    lcd_putsAtt(22, 40, STR_NODATA, DBLSIZE);
  }
}
