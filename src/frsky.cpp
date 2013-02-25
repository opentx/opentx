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

#if defined(TELEMETREZ)
#define PRIVATE         0x1B
#endif

uint8_t frskyRxBuffer[FRSKY_RX_PACKET_SIZE];   // Receive buffer. 9 bytes (full packet), worst case 18 bytes with byte-stuffing (+1)
uint8_t frskyTxBuffer[FRSKY_TX_PACKET_SIZE];   // Ditto for transmit buffer
#if !defined(CPUARM)
uint8_t frskyTxBufferCount = 0;
#endif
int8_t frskyStreaming = -1;
#if defined(WS_HOW_HIGH)
uint8_t frskyUsrStreaming = 0;
#endif
uint8_t link_counter = 0;
FrskyData frskyData;

#if defined(FRSKY_HUB)
uint8_t barsThresholds[THLD_MAX];
#endif

uint16_t getChannelRatio(uint8_t channel)
{
  return (uint16_t)g_model.frsky.channels[channel].ratio << g_model.frsky.channels[channel].multiplier;
}

lcdint_t applyChannelRatio(uint8_t channel, lcdint_t val)
{
  return ((int32_t)val+g_model.frsky.channels[channel].offset) * getChannelRatio(channel) * 2 / 51;
}

#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
void checkMinMaxAltitude()
{
  if (frskyData.hub.baroAltitude_bp > frskyData.hub.maxAltitude)
    frskyData.hub.maxAltitude = frskyData.hub.baroAltitude_bp;
  if (frskyData.hub.baroAltitude_bp < frskyData.hub.minAltitude)
    frskyData.hub.minAltitude = frskyData.hub.baroAltitude_bp;
}

#if defined(VARIO)
void evalVario(int16_t altitude_bp, uint16_t altitude_ap)
{
  int32_t varioAltitude_cm = (int32_t)altitude_bp * 100 + (altitude_bp > 0 ? altitude_ap : -altitude_ap);
  uint8_t varioAltitudeQueuePointer = frskyData.hub.varioAltitudeQueuePointer + 1;
  if (varioAltitudeQueuePointer == VARIO_QUEUE_LENGTH)
    varioAltitudeQueuePointer = 0;
  frskyData.hub.varioAltitudeQueuePointer = varioAltitudeQueuePointer;
  frskyData.hub.varioSpeed -= frskyData.hub.varioAltitudeQueue[varioAltitudeQueuePointer] ;
  frskyData.hub.varioAltitudeQueue[varioAltitudeQueuePointer] = varioAltitude_cm - frskyData.hub.varioAltitude_cm;
  frskyData.hub.varioAltitude_cm = varioAltitude_cm;
  frskyData.hub.varioSpeed += frskyData.hub.varioAltitudeQueue[varioAltitudeQueuePointer] ;
}
#else
#define evalVario(...)
#endif
#endif

#if defined(FRSKY_HUB)
void extractLatitudeLongitude(uint32_t * latitude, uint32_t * longitude)
{
  div_t qr = div(frskyData.hub.gpsLatitude_bp, 100);
  *latitude = ((uint32_t)(qr.quot) * 1000000) + (((uint32_t)(qr.rem) * 10000 + frskyData.hub.gpsLatitude_ap) * 5) / 3;

  qr = div(frskyData.hub.gpsLongitude_bp, 100);
  *longitude = ((uint32_t)(qr.quot) * 1000000) + (((uint32_t)(qr.rem) * 10000 + frskyData.hub.gpsLongitude_ap) * 5) / 3;
}

inline void getGpsPilotPosition()
{
  extractLatitudeLongitude(&frskyData.hub.pilotLatitude, &frskyData.hub.pilotLongitude);
  uint32_t lat = frskyData.hub.pilotLatitude / 10000;
  uint32_t angle2 = (lat*lat) / 10000;
  uint32_t angle4 = angle2 * angle2;
  frskyData.hub.distFromEarthAxis = 139*(((uint32_t)10000000-((angle2*(uint32_t)123370)/81)+(angle4/25))/12500);
  // printf("frskyData.hub.distFromEarthAxis=%d\n", frskyData.hub.distFromEarthAxis); fflush(stdout);
}

inline void getGpsDistance()
{
  uint32_t lat, lng;

  extractLatitudeLongitude(&lat, &lng);

  // printf("lat=%d (%d), long=%d (%d)\n", lat, abs(lat - frskyData.hub.pilotLatitude), lng, abs(lng - frskyData.hub.pilotLongitude));

  uint32_t angle = (lat > frskyData.hub.pilotLatitude) ? lat - frskyData.hub.pilotLatitude : frskyData.hub.pilotLatitude - lat;
  uint32_t dist = EARTH_RADIUS * angle / 1000000;
  uint32_t result = dist*dist;

  angle = (lng > frskyData.hub.pilotLongitude) ? lng - frskyData.hub.pilotLongitude : frskyData.hub.pilotLongitude - lng;
  dist = frskyData.hub.distFromEarthAxis * angle / 1000000;
  result += dist*dist;

  dist = abs(frskyData.hub.baroAltitudeOffset ? frskyData.hub.baroAltitude_bp : frskyData.hub.gpsAltitude_bp);
  result += dist*dist;

  frskyData.hub.gpsDistance = isqrt32(result);
  if (frskyData.hub.gpsDistance > frskyData.hub.maxGpsDistance)
    frskyData.hub.maxGpsDistance = frskyData.hub.gpsDistance;
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
    if (byte > 0x3f) {
      state = TS_IDLE;
    }
    else {
      structPos = byte*2;
      state = TS_DATA_LOW;
    }
    return;
  }
  if (state == TS_DATA_LOW) {
    lowByte = byte;
    state = TS_DATA_HIGH;
    return;
  }

  state = TS_IDLE;

  if ((uint8_t)structPos == offsetof(FrskyHubData, gpsLatitude_bp)) {
    if (lowByte || byte)
      frskyData.hub.gpsFix = 1;
    else if (frskyData.hub.gpsFix > 0 && frskyData.hub.gpsLatitude_bp > 1)
      frskyData.hub.gpsFix = 0;
  }
  else if ((uint8_t)structPos == offsetof(FrskyHubData, gpsLongitude_bp)) {
    if (lowByte || byte)
      frskyData.hub.gpsFix = 1;
    else if (frskyData.hub.gpsFix > 0 && frskyData.hub.gpsLongitude_bp > 1)
      frskyData.hub.gpsFix = 0;
  }
  
  if ((uint8_t)structPos == offsetof(FrskyHubData, gpsAltitude_bp) ||
      ((uint8_t)structPos >= offsetof(FrskyHubData, gpsAltitude_ap) && (uint8_t)structPos <= offsetof(FrskyHubData, gpsLatitudeNS) && (uint8_t)structPos != offsetof(FrskyHubData, baroAltitude_bp) && (uint8_t)structPos != offsetof(FrskyHubData, baroAltitude_ap))) {
    // If we don't have a fix, we may discard the value
    if (frskyData.hub.gpsFix <= 0)
      return;
  }
  
  ((uint8_t*)&frskyData.hub)[structPos] = lowByte;
  ((uint8_t*)&frskyData.hub)[structPos+1] = byte;

  switch ((uint8_t)structPos) {

    case offsetof(FrskyHubData, rpm):
      frskyData.hub.rpm *= (uint8_t)60/(g_model.frsky.blades+2);
      if (frskyData.hub.rpm > frskyData.hub.maxRpm)
        frskyData.hub.maxRpm = frskyData.hub.rpm;
      break;

    case offsetof(FrskyHubData, temperature1):
      if (frskyData.hub.temperature1 > frskyData.hub.maxTemperature1)
        frskyData.hub.maxTemperature1 = frskyData.hub.temperature1;
      break;

    case offsetof(FrskyHubData, temperature2):
      if (frskyData.hub.temperature2 > frskyData.hub.maxTemperature2)
        frskyData.hub.maxTemperature2 = frskyData.hub.temperature2;
      break;

    case offsetof(FrskyHubData, current):
      if (frskyData.hub.current > frskyData.hub.maxCurrent)
        frskyData.hub.maxCurrent = frskyData.hub.current;
      break;
      
    case offsetof(FrskyHubData, volts_ap):
#if defined(FAS_BSS)
      frskyData.hub.vfas = (frskyData.hub.volts_bp * 10 + frskyData.hub.volts_ap);
#else
      frskyData.hub.vfas = ((frskyData.hub.volts_bp * 100 + frskyData.hub.volts_ap * 10) * 21) / 110;
#endif
      /* TODO later if (!frskyData.hub.minVfas || frskyData.hub.minVfas > frskyData.hub.vfas)
        frskyData.hub.minVfas = frskyData.hub.vfas; */
      break;

    case offsetof(FrskyHubData, baroAltitude_bp):
      // First received barometer altitude => Altitude offset
      if (!frskyData.hub.baroAltitudeOffset)
        frskyData.hub.baroAltitudeOffset = -frskyData.hub.baroAltitude_bp;
      if (g_model.frsky.varioSource == VARIO_SOURCE_ALTI) {
        evalVario(frskyData.hub.baroAltitude_bp, 0);
      }
      frskyData.hub.baroAltitude_bp += frskyData.hub.baroAltitudeOffset;
      checkMinMaxAltitude();
      break;

    case offsetof(FrskyHubData, baroAltitude_ap):
      if (g_model.frsky.varioSource == VARIO_SOURCE_ALTI_PLUS) {
        evalVario(frskyData.hub.baroAltitude_bp-frskyData.hub.baroAltitudeOffset, frskyData.hub.baroAltitude_ap);
      }
      break;

    case offsetof(FrskyHubData, gpsAltitude_ap):
      if (!frskyData.hub.gpsAltitudeOffset)
        frskyData.hub.gpsAltitudeOffset = -frskyData.hub.gpsAltitude_bp;
      frskyData.hub.gpsAltitude_bp += frskyData.hub.gpsAltitudeOffset;
      if (!frskyData.hub.baroAltitudeOffset) {
        if (frskyData.hub.gpsAltitude_bp > frskyData.hub.maxAltitude)
          frskyData.hub.maxAltitude = frskyData.hub.gpsAltitude_bp;
        if (frskyData.hub.gpsAltitude_bp < frskyData.hub.minAltitude)
          frskyData.hub.minAltitude = frskyData.hub.gpsAltitude_bp;
      }

      if (!frskyData.hub.pilotLatitude && !frskyData.hub.pilotLongitude) {
        // First received GPS position => Pilot GPS position
        getGpsPilotPosition();
      }
      else if (frskyData.hub.gpsDistNeeded || g_menuStack[g_menuStackPtr] == menuTelemetryFrsky) {
        getGpsDistance();
      }
      break;

    case offsetof(FrskyHubData, gpsSpeed_bp):
      // Speed => Max speed
      if (frskyData.hub.gpsSpeed_bp > frskyData.hub.maxGpsSpeed)
        frskyData.hub.maxGpsSpeed = frskyData.hub.gpsSpeed_bp;
      break;

    case offsetof(FrskyHubData, volts):
      // Voltage => Cell number + Cell voltage
    {
      uint8_t battnumber = ((frskyData.hub.volts & 0x00F0) >> 4);
      if (battnumber < 12) {
        if (frskyData.hub.cellsCount < battnumber+1) {
          frskyData.hub.cellsCount = battnumber+1;
        }
        uint8_t cellVolts = (uint8_t)(((((frskyData.hub.volts & 0xFF00) >> 8) + ((frskyData.hub.volts & 0x000F) << 8)))/10);
        frskyData.hub.cellVolts[battnumber] = cellVolts;
        if (!frskyData.hub.minCellVolts || cellVolts < frskyData.hub.minCellVolts || battnumber==frskyData.hub.minCellIdx) {
          frskyData.hub.minCellIdx = battnumber;
          frskyData.hub.minCellVolts = cellVolts;
        }
      }
      break;
    }

    case offsetof(FrskyHubData, hour):
      frskyData.hub.hour = ((uint8_t)(frskyData.hub.hour + g_eeGeneral.timezone + 24)) % 24;
      break;

    case offsetof(FrskyHubData, accelX):
    case offsetof(FrskyHubData, accelY):
    case offsetof(FrskyHubData, accelZ):
      *(int16_t*)(&((uint8_t*)&frskyData.hub)[structPos]) /= 10;
      break;

  }
}
#endif

#if defined(WS_HOW_HIGH)
void parseTelemWSHowHighByte(uint8_t byte)
{
  if (frskyUsrStreaming < (FRSKY_TIMEOUT10ms*3 - 10)) {
    ((uint8_t*)&frskyData.hub)[offsetof(FrskyHubData, baroAltitude_bp)] = byte;
    checkMinMaxAltitude();
    if (g_model.frsky.varioSource == VARIO_SOURCE_ALTI) {
      evalVario(frskyData.hub.baroAltitude_bp, 0);
    }
  }
  else {
    // At least 100mS passed since last data received
    ((uint8_t*)&frskyData.hub)[offsetof(FrskyHubData, baroAltitude_bp)+1] = byte;
  }
  // baroAltitude_bp unit here is feet!
  frskyUsrStreaming = FRSKY_TIMEOUT10ms*3; // reset counter
}
#endif  

void processFrskyPacket(uint8_t *packet)
{
  // What type of packet?
  switch (packet[0])
  {
    case LINKPKT: // A1/A2/RSSI values
    {
      link_counter += 32;
      frskyData.analog[0].set(packet[1], g_model.frsky.channels[0].type);
      frskyData.analog[1].set(packet[2], g_model.frsky.channels[1].type);
      frskyData.rssi[0].set(packet[3]);
      frskyData.rssi[1].set(packet[4] / 2);
      frskyStreaming = FRSKY_TIMEOUT10ms; // reset counter only if valid frsky packets are being detected
#if defined(VARIO)
      uint8_t varioSource = g_model.frsky.varioSource - VARIO_SOURCE_A1;
      if (varioSource < 2)
        frskyData.hub.varioSpeed = applyChannelRatio(varioSource, frskyData.analog[varioSource].value);
#endif
      break;
    }
#if defined(FRSKY_HUB) || defined (WS_HOW_HIGH)
    case USRPKT: // User Data packet
      uint8_t numBytes = 3 + (packet[1] & 0x07); // sanitize in case of data corruption leading to buffer overflow
      for (uint8_t i=3; i<numBytes; i++) {
#if defined(FRSKY_HUB)
        if (g_model.frsky.usrProto == USR_PROTO_FRSKY)
          parseTelemHubByte(packet[i]);
#endif
#if defined(WS_HOW_HIGH)
        if (g_model.frsky.usrProto == USR_PROTO_WS_HOW_HIGH)
          parseTelemWSHowHighByte(packet[i]);
#endif
      }
      break;
#endif
  }
}

// Receive buffer state machine state enum
enum FrSkyDataState {
  STATE_DATA_IDLE,
  STATE_DATA_START,
  STATE_DATA_IN_FRAME,
  STATE_DATA_XOR,
#if defined(TELEMETREZ)
  STATE_DATA_PRIVATE_LEN,
  STATE_DATA_PRIVATE_VALUE
#endif
};

#if defined(TELEMETREZ)
uint8_t privateDataLen;
uint8_t privateDataPos;
#endif

#if defined(ROTARY_ENCODER_NAVIGATION) && defined(TELEMETREZ)
extern uint8_t TrotCount;
extern uint8_t TezRotary;
#endif

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

#if defined(CPUARM)
void processSerialData(uint8_t data)
#else
NOINLINE void processSerialData(uint8_t stat, uint8_t data)
#endif
{
  static uint8_t numPktBytes = 0;
  static uint8_t dataState = STATE_DATA_IDLE;

#if defined(BLUETOOTH)
  // TODO if (g_model.bt_telemetry)
  btPushByte(data);
#endif

#if !defined(CPUARM)
  if (stat & ((1 << FE0) | (1 << DOR0) | (1 << UPE0))) {
    // discard buffer and start fresh on any comms error
    numPktBytes = 0;
  }
  else
#endif
  {
    switch (dataState)
    {
      case STATE_DATA_START:
        if (data == START_STOP) break; // Remain in userDataStart if possible 0x7e,0x7e doublet found.

        if (numPktBytes < FRSKY_RX_PACKET_SIZE)
          frskyRxBuffer[numPktBytes++] = data;
        dataState = STATE_DATA_IN_FRAME;
        break;

      case STATE_DATA_IN_FRAME:
        if (data == BYTESTUFF)
        {
            dataState = STATE_DATA_XOR; // XOR next byte
            break;
        }
        if (data == START_STOP) // end of frame detected
        {
          processFrskyPacket(frskyRxBuffer);
          dataState = STATE_DATA_IDLE;
          break;
        }
        if (numPktBytes < FRSKY_RX_PACKET_SIZE)
          frskyRxBuffer[numPktBytes++] = data;
        break;

      case STATE_DATA_XOR:
        if (numPktBytes < FRSKY_RX_PACKET_SIZE)
          frskyRxBuffer[numPktBytes++] = data ^ STUFF_MASK;
        dataState = STATE_DATA_IN_FRAME;
        break;

      case STATE_DATA_IDLE:
        if (data == START_STOP) {
          numPktBytes = 0;
          dataState = STATE_DATA_START;
        }
#if defined(TELEMETREZ)
        if (data == PRIVATE) {
          dataState = STATE_DATA_PRIVATE_LEN;
        }
#endif
        break;

#if defined(TELEMETREZ)
      case STATE_DATA_PRIVATE_LEN:
        dataState = STATE_DATA_PRIVATE_VALUE;
        privateDataLen = data; // Count of bytes to receive
        privateDataPos = 0;
        break;

      case STATE_DATA_PRIVATE_VALUE :
        if (privateDataPos == 0) {
          // Process first private data byte
          // PC6, PC7
          if ((data & 0x3F) == 0) {// Check byte is valid
            DDRC |= 0xC0;          // Set as outputs
            PORTC = ( PORTC & 0x3F ) | ( data & 0xC0 ); // update outputs
          }
        }
#if defined(ROTARY_ENCODER_NAVIGATION)
        if (privateDataPos == 1) {
          TrotCount = data;
        }
        if (privateDataPos == 2) { // rotary encoder switch
          RotEncoder = data;
        }
#endif
        if (++privateDataPos == privateDataLen) {
          dataState = STATE_DATA_IDLE;
        }
        break;
#endif
    } // switch
  }
}

#if !defined(CPUARM) && !defined(SIMU)
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
#if defined(CPUARM)
void frskyTransmitBuffer( uint32_t size )
{
#if defined(PCBSKY9X)
  txPdcUsart( frskyTxBuffer, size ) ;
#endif
}
#else
void frskyTransmitBuffer()
{
  UCSR0B |= (1 << UDRIE0); // enable  UDRE0 interrupt
}
#endif

uint8_t frskyAlarmsSendState = 0 ;

#if defined(CPUARM)
void frskyPushValue(uint8_t *&ptr, uint8_t value)
{
  // byte stuff the only byte than might need it
  if (value == START_STOP) {
    *ptr++ = BYTESTUFF;
    *ptr++ = 0x5e;
  }
  else if (value == BYTESTUFF) {
    *ptr++ = BYTESTUFF;
    *ptr++ = 0x5d;
  }
  else {
    *ptr++ = value;
  }
}

inline void frskySendNextAlarm(void)
{
#if defined(PCBSKY9X)
  if (txPdcPending())
    return; // we only have one buffer. If it's in use, then we can't send yet.
#endif

  uint8_t *ptr = &frskyTxBuffer[0];

  *ptr++ = START_STOP;        // Start of packet

  // Now send a packet
  frskyAlarmsSendState -= 1;
  uint8_t alarm = 1 - (frskyAlarmsSendState % 2);
  if (frskyAlarmsSendState < SEND_MODEL_ALARMS) {
    uint8_t channel = 1 - (frskyAlarmsSendState / 2);
    *ptr++ = (A22PKT + frskyAlarmsSendState); // fc - fb - fa - f9
    frskyPushValue(ptr, g_model.frsky.channels[channel].alarms_value[alarm]);
    *ptr++ = ALARM_GREATER(channel, alarm);
    *ptr++ = (g_eeGeneral.beeperMode != e_mode_quiet ? ALARM_LEVEL(channel, alarm) : alarm_off);
  }
  else {
    *ptr++ = (RSSI1PKT-alarm);  // f7 - f6
    frskyPushValue(ptr, getRssiAlarmValue(alarm));
    *ptr++ = 0x00;
    *ptr++ = (g_eeGeneral.beeperMode != e_mode_quiet ? ((2+alarm+g_model.frsky.rssiAlarms[alarm].level) % 4) : alarm_off);
  }

  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = START_STOP; // End of packet

  frskyTransmitBuffer(ptr - &frskyTxBuffer[0]);
}

#else

void frskyPushValue(uint8_t *&ptr, uint8_t value)
{
  // byte stuff the only byte than might need it
  bool bytestuff = false;

  if (value == START_STOP) {
    bytestuff = true;
    value = 0x5e;
  }
  else if (value == BYTESTUFF) {
    bytestuff = true;
    value = 0x5d;
  }

  *ptr++ = value;
  if (bytestuff)
    *ptr = BYTESTUFF;
}

void frskySendPacket(uint8_t type, uint8_t value, uint8_t p1, uint8_t p2)
{
  uint8_t *ptr = &frskyTxBuffer[0];

  *ptr++ = START_STOP;        // End of packet
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = (g_eeGeneral.beeperMode == e_mode_quiet ? alarm_off : p2);
  *ptr++ = p1;
  frskyPushValue(ptr, value);
  *ptr++ = type;
  *ptr++ = START_STOP; // Start of packet

  frskyTxBufferCount = ptr - &frskyTxBuffer[0];
  frskyTransmitBuffer();
}

inline void frskySendNextAlarm(void)
{
  if (frskyTxBufferCount)
    return; // we only have one buffer. If it's in use, then we can't send yet.

  // Now send a packet
  frskyAlarmsSendState -= 1;
  uint8_t alarm = 1 - (frskyAlarmsSendState % 2);
  if (frskyAlarmsSendState < SEND_MODEL_ALARMS) {
    uint8_t channel = 1 - (frskyAlarmsSendState / 2);
    frskySendPacket(A22PKT + frskyAlarmsSendState, g_model.frsky.channels[channel].alarms_value[alarm], ALARM_GREATER(channel, alarm), ALARM_LEVEL(channel, alarm));
  }
  else {
    frskySendPacket(RSSI1PKT-alarm, getRssiAlarmValue(alarm), 0, (2+alarm+g_model.frsky.rssiAlarms[alarm].level) % 4);
  }
}
#endif

NOINLINE void telemetryPoll10ms()
{
#if defined(PCBSKY9X)
  rxPdcUsart(processSerialData);              // Receive serial data here
#endif

  // Attempt to transmit any waiting Fr-Sky alarm set packets every 50ms (subject to packet buffer availability)
  static uint8_t frskyTxDelay = 5;
  if (frskyAlarmsSendState && (--frskyTxDelay == 0)) {
    frskyTxDelay = 5; // 50ms
    frskySendNextAlarm();
  }

#ifndef SIMU
#if defined(WS_HOW_HIGH)
  if (frskyUsrStreaming > 0) {
    frskyUsrStreaming--;
  }
#endif

  if (frskyStreaming > 0) {
    frskyStreaming--;
  }
  else {
    frskyData.rssi[0].set(0);
    frskyData.rssi[1].set(0);
  }
#endif

  uint16_t voltage = 0; /* unit: 1/10 volts */

#if defined(FRSKY_HUB)
  for (uint8_t i=0; i<frskyData.hub.cellsCount; i++)
    voltage += frskyData.hub.cellVolts[i];
  voltage /= 5;
  frskyData.hub.cellsSum = voltage;
#endif

  uint8_t channel = g_model.frsky.voltsSource;
  if (channel <= 1) {
    voltage = applyChannelRatio(channel, frskyData.analog[channel].value) / 10;
  }
#if defined(FRSKY_HUB)
  else if (channel == 2) {
    voltage = frskyData.hub.vfas;
  }
#endif

#if defined(FRSKY_HUB)
  uint16_t current = frskyData.hub.current; /* unit: 1/10 amps */
#else
  uint16_t current = 0;
#endif

  channel = g_model.frsky.currentSource - FRSKY_SOURCE_A1;
  if (channel <= 1) {
    current = applyChannelRatio(channel, frskyData.analog[channel].value) / 10;
  }

  frskyData.power = (current * voltage) / 100;

  frskyData.currentPrescale += current;
  if (frskyData.currentPrescale >= 3600) {
    frskyData.currentConsumption += 1;
    frskyData.currentPrescale -= 3600;
  }

#if defined(VARIO)
  static uint16_t s_varioTmr = 0;
  if (isFunctionActive(FUNC_VARIO)) {
#if defined(AUDIO)
    // conversion in cm/s
    do {
      int16_t verticalSpeed;
      int16_t varioCenterMax = (int16_t)g_model.frsky.varioCenterMax * 10 + 50;
      if (frskyData.hub.varioSpeed >= varioCenterMax) {
        verticalSpeed = frskyData.hub.varioSpeed - varioCenterMax;
        int16_t varioMax = (10+(int16_t)g_model.frsky.varioMax) * 100;
        if (verticalSpeed > varioMax) verticalSpeed = varioMax;
        verticalSpeed = (verticalSpeed * 10) / ((varioMax-varioCenterMax) / 100);
      }
      else {
        int16_t varioCenterMin = (int16_t)g_model.frsky.varioCenterMin * 10 - 50;
        if (frskyData.hub.varioSpeed <= varioCenterMin) {
          verticalSpeed = frskyData.hub.varioSpeed - varioCenterMin;
          int16_t varioMin = (-10+(int16_t)g_model.frsky.varioMin) * 100;
          if (verticalSpeed < varioMin) verticalSpeed = varioMin;
          verticalSpeed = (verticalSpeed * 10) / ((varioCenterMin-varioMin) / 100);
        }
        else {
          break;
        }
      }

      uint16_t tmr10ms = g_tmr10ms;
      if (verticalSpeed < 0 || tmr10ms > s_varioTmr) {
        uint8_t SoundVarioBeepTime = (1600 - verticalSpeed) / 100;
        uint8_t SoundVarioBeepFreq = (verticalSpeed * 10 + 16000) >> 8;
        s_varioTmr = tmr10ms + (SoundVarioBeepTime*2);
        AUDIO_VARIO(SoundVarioBeepFreq, SoundVarioBeepTime);
      }

    } while(0);
#else
    int8_t verticalSpeed = limit((int16_t)-100, (int16_t)(frskyData.hub.varioSpeed/10), (int16_t)+100);

    uint16_t interval;
    if (verticalSpeed == 0) {
      interval = 300;
    }
    else {
      if (verticalSpeed < 0) {
        verticalSpeed = -verticalSpeed;
        warble = 1;
      }
      interval = (uint8_t)200 / verticalSpeed;
    }
    if (g_tmr10ms - s_varioTmr > interval) {
      s_varioTmr = g_tmr10ms;
      if (warble)
        AUDIO_VARIO_DOWN();
      else
        AUDIO_VARIO_UP();
    }
#endif
  }
#endif
}

#if 0
// not used any more
bool FRSKY_alarmRaised(uint8_t idx)
{
  for (int i=0; i<2; i++) {
    if (ALARM_LEVEL(idx, i) != alarm_off) {
      if (ALARM_GREATER(idx, i)) {
        if (frskyData.analog[idx].value > g_model.frsky.channels[idx].alarms_value[i])
          return true;
      }
      else {
        if (frskyData.analog[idx].value < g_model.frsky.channels[idx].alarms_value[i])
          return true;
      }
    }
  }
  return false;
}
#endif

#if !defined(CPUARM)
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
#endif

void FRSKY_Init(void)
{
  // clear frsky variables
  resetTelemetry();

#if defined(PCBX9D)
  // TODO
#elif defined(PCBSKY9X)
  startPdcUsartReceive() ;
#elif !defined(SIMU)

  DDRE &= ~(1 << DDE0);    // set RXD0 pin as input
  PORTE &= ~(1 << PORTE0); // disable pullup on RXD0 pin

#undef BAUD
#define BAUD 9600
#include <util/setbaud.h>

  UBRR0H = UBRRH_VALUE;
  UBRR0L = UBRRL_VALUE;
  UCSR0A &= ~(1 << U2X0); // disable double speed operation.

  // set 8N1
  UCSR0B = 0 | (0 << RXCIE0) | (0 << TXCIE0) | (0 << UDRIE0) | (0 << RXEN0) | (0 << TXEN0) | (0 << UCSZ02);
  UCSR0C = 0 | (1 << UCSZ01) | (1 << UCSZ00);

  
  while (UCSR0A & (1 << RXC0)) UDR0; // flush receive buffer

  // These should be running right from power up on a FrSky enabled '9X.
  FRSKY_EnableTXD(); // enable FrSky-Telemetry reception
  FRSKY_EnableRXD(); // enable FrSky-Telemetry reception
#endif
}

void FrskyValueWithMin::set(uint8_t value)
{
  if (this->value == 0)
    this->value = value;

  sum += value;
  if (link_counter == 0) {
    this->value = sum / 8;
    sum = 0;
  }

  if (value && (!min || value < min))
    min = value;
}

void FrskyValueWithMinMax::set(uint8_t value, uint8_t unit)
{
  FrskyValueWithMin::set(value);
  if (unit != UNIT_VOLTS) {
    this->value = value;
  }
  if (!max || value > max)
    max = value;
}

void resetTelemetry()
{
  memclear(&frskyData, sizeof(frskyData));

#if defined(FRSKY_HUB)
  frskyData.hub.gpsLatitude_bp = 2;
  frskyData.hub.gpsLongitude_bp = 2;
  frskyData.hub.gpsFix = -1;
#endif

#ifdef SIMU
  frskyData.analog[0].set(120, UNIT_VOLTS);
  frskyData.analog[1].set(240, UNIT_VOLTS);
  frskyData.rssi[0].set(75);
  frskyData.hub.fuelLevel = 75;
  frskyData.hub.rpm = 12000;

  frskyData.hub.gpsFix = 1;
  frskyData.hub.gpsLatitude_bp = 4401;
  frskyData.hub.gpsLatitude_ap = 7710;
  frskyData.hub.gpsLongitude_bp = 1006;
  frskyData.hub.gpsLongitude_ap = 8872;
  frskyData.hub.gpsSpeed_bp = (100 * 250) / 463;
  getGpsPilotPosition();

  frskyData.hub.gpsLatitude_bp = 4401;
  frskyData.hub.gpsLatitude_ap = 7455;
  frskyData.hub.gpsLongitude_bp = 1006;
  frskyData.hub.gpsLongitude_ap = 9533;
  getGpsDistance();

  frskyData.hub.cellsCount = 6;

  frskyData.hub.gpsAltitude_bp = 50;
  frskyData.hub.baroAltitude_bp = 50;
  frskyData.hub.minAltitude = 10;
  frskyData.hub.maxAltitude = 500;

  frskyData.hub.accelY = 100;
  frskyData.hub.temperature1 = -30;
  frskyData.hub.maxTemperature1 = 100;
  
  frskyData.hub.current = 5;
  frskyData.hub.maxCurrent = 56;
#endif
}

uint8_t maxTelemValue(uint8_t channel)
{
  switch (channel) {
    case TELEM_FUEL:
    case TELEM_RSSI_TX:
    case TELEM_RSSI_RX:
      return 100;
    case TELEM_HDG:
      return 180;
    default:
      return 255;
  }
}

int16_t convertTelemValue(uint8_t channel, uint8_t value)
{
  int16_t result;
  switch (channel) {
    case TELEM_TM1:
    case TELEM_TM2:
      result = value * 3;
      break;
    case TELEM_ALT:
    case TELEM_GPSALT:
    case TELEM_MAX_ALT:
    case TELEM_MIN_ALT:
      result = value * 8 - 500;
      break;
    case TELEM_RPM:
    case TELEM_MAX_RPM:
      result = value * 50;
      break;
    case TELEM_T1:
    case TELEM_T2:
    case TELEM_MAX_T1:
    case TELEM_MAX_T2:
      result = (int16_t)value - 30;
      break;
    case TELEM_CELL:
    case TELEM_HDG:
      result = value * 2;
      break;
    case TELEM_DIST:
    case TELEM_MAX_DIST:
      result = value * 8;
      break;
    case TELEM_CURRENT:
    case TELEM_POWER:
      result = value * 5;
      break;
    case TELEM_CONSUMPTION:
      result = value * 20;
      break;
    default:
      result = value;
      break;
  }
  return result;
}

int16_t convertCswTelemValue(CustomSwData * cs)
{
  int16_t val;
  if (CS_STATE(cs->func)==CS_VOFS)
    val = convertTelemValue(cs->v1 - MIXSRC_FIRST_TELEM + 1, 128+cs->v2);
  else
    val = convertTelemValue(cs->v1 - MIXSRC_FIRST_TELEM + 1, 128+cs->v2) - convertTelemValue(cs->v1 - MIXSRC_FIRST_TELEM + 1, 128);
  return val;
}

const pm_uint8_t bchunit_ar[] PROGMEM = {
  UNIT_METERS,  // Alt
  UNIT_RAW,     // Rpm
  UNIT_PERCENT, // Fuel
  UNIT_DEGREES, // T1
  UNIT_DEGREES, // T2
  UNIT_KTS,     // Speed
  UNIT_METERS,  // Dist
  UNIT_METERS,  // GPS Alt
};

void putsTelemetryChannel(xcoord_t x, uint8_t y, uint8_t channel, lcdint_t val, uint8_t att)
{
  switch (channel) {
    case TELEM_TM1-1:
    case TELEM_TM2-1:
      putsTime(x-3*FW, y, val, att, att);
      break;
    case TELEM_MIN_A1-1:
    case TELEM_MIN_A2-1:
      channel -= TELEM_MIN_A1-TELEM_A1;
      // no break
    case TELEM_A1-1:
    case TELEM_A2-1:
      channel -= TELEM_A1-1;
      // A1 and A2
    {
      lcdint_t converted_value = applyChannelRatio(channel, val);
      if (g_model.frsky.channels[channel].type >= UNIT_RAW) {
        converted_value /= 10;
      }
      else {
        if (abs(converted_value) < 1000) {
          att |= PREC2;
        }
        else {
          converted_value /= 10;
          att |= PREC1;
        }
      }
      putsTelemetryValue(x, y, converted_value, g_model.frsky.channels[channel].type, att);
      break;
    }

    case TELEM_CELL-1:
      putsTelemetryValue(x, y, val, UNIT_VOLTS, att|PREC2);
      break;

    case TELEM_TX_VOLTAGE-1:
    case TELEM_VFAS-1:
    case TELEM_CELLS_SUM-1:
      putsTelemetryValue(x, y, val, UNIT_VOLTS, att|PREC1);
      break;

    case TELEM_CURRENT-1:
    case TELEM_MAX_CURRENT-1:
      putsTelemetryValue(x, y, val, UNIT_AMPS, att|PREC1);
      break;

    case TELEM_CONSUMPTION-1:
      putsTelemetryValue(x, y, val, UNIT_MAH, att);
      break;

    case TELEM_POWER-1:
      putsTelemetryValue(x, y, val, UNIT_WATTS, att);
      break;

    case TELEM_ACCx-1:
    case TELEM_ACCy-1:
    case TELEM_ACCz-1:
    case TELEM_VSPD-1:
      putsTelemetryValue(x, y, val, UNIT_RAW, att|PREC2);
      break;

    case TELEM_RSSI_TX-1:
    case TELEM_RSSI_RX-1:
      putsTelemetryValue(x, y, val, UNIT_RAW, att);
      break;

#if defined(IMPERIAL_UNITS)
    case TELEM_ALT-1:
    case TELEM_MIN_ALT-1:
    case TELEM_MAX_ALT-1:
      if (g_model.frsky.usrProto == USR_PROTO_WS_HOW_HIGH) {
        putsTelemetryValue(x, y, val, UNIT_FEET, att);
        break;
      }
      // no break
#endif

    default:
    {
      uint8_t unit = 1;
      if (channel >= TELEM_MAX_T1-1 && channel <= TELEM_MAX_DIST-1)
        channel -= TELEM_MAX_T1 - TELEM_T1;
      if (channel <= TELEM_GPSALT-1)
        unit = channel + 1 - TELEM_ALT;
      if (channel >= TELEM_MIN_ALT-1 && channel <= TELEM_MAX_ALT-1)
        unit = 0;
      if (channel == TELEM_HDG-1)
        unit = 3;
      putsTelemetryValue(x, y, val, pgm_read_byte(bchunit_ar+unit), att);
      break;
    }
  }
}

enum FrskyViews {
  e_frsky_custom_screen_1,
  e_frsky_custom_screen_2,
  IF_CPUARM(e_frsky_custom_screen_3)
  e_frsky_voltages,
  e_frsky_after_flight,
  FRSKY_VIEW_MAX = e_frsky_after_flight
};

static uint8_t s_frsky_view = e_frsky_custom_screen_1;

#if LCD_W == 212
void displayRssiLine()
{
  if (frskyStreaming > 0) {
    lcd_hline(0, 55, 212, 0); // separator
    uint8_t rssi = min((uint8_t)99, frskyData.rssi[1].value);
    lcd_putsLeft(7*FH+1, STR_TX); lcd_outdezNAtt(4*FW, 7*FH+1, rssi, LEADING0, 2);
    lcd_rect(25, 57, 78, 7);
    lcd_filled_rect(26, 58, 17*rssi/33, 5, (rssi < getRssiAlarmValue(0)) ? DOTTED : SOLID);
    rssi = min((uint8_t)99, frskyData.rssi[0].value);
    lcd_puts(190, 7*FH+1, STR_RX); lcd_outdezNAtt(189+4*FW-1, 7*FH+1, rssi, LEADING0, 2);
    lcd_rect(110, 57, 78, 7);
    uint8_t v = 17*rssi/33;
    lcd_filled_rect(111+76-v, 58, v, 5, (rssi < getRssiAlarmValue(0)) ? DOTTED : SOLID);
  }
  else {
    lcd_putsAtt(7*FW, 7*FH+1, STR_NODATA, BLINK);
    lcd_status_line();
  }
}
#else
void displayRssiLine()
{
  if (frskyStreaming > 0) {
    lcd_hline(0, 55, 128, 0); // separator
    uint8_t rssi = min((uint8_t)99, frskyData.rssi[1].value);
    lcd_putsLeft(7*FH+1, STR_TX); lcd_outdezNAtt(4*FW, 7*FH+1, rssi, LEADING0, 2);
    lcd_rect(25, 57, 38, 7);
    lcd_filled_rect(26, 58, 4*rssi/11, 5, (rssi < getRssiAlarmValue(0)) ? DOTTED : SOLID);
    rssi = min((uint8_t)99, frskyData.rssi[0].value);
    lcd_puts(105, 7*FH+1, STR_RX); lcd_outdezNAtt(105+4*FW-1, 7*FH+1, rssi, LEADING0, 2);
    lcd_rect(65, 57, 38, 7);
    uint8_t v = 4*rssi/11;
    lcd_filled_rect(66+36-v, 58, v, 5, (rssi < getRssiAlarmValue(0)) ? DOTTED : SOLID);
  }
  else {
    lcd_putsAtt(7*FW, 7*FH+1, STR_NODATA, BLINK);
    lcd_status_line();
  }
}
#endif

#if defined(FRSKY_HUB)
void displayGpsTime()
{
#define TIME_LINE (7*FH+1)
  uint8_t att = (frskyStreaming > 0 ? LEFT|LEADING0 : LEFT|LEADING0|BLINK);
  lcd_outdezNAtt(6*FW+5, TIME_LINE, frskyData.hub.hour, att, 2);
  lcd_putcAtt(8*FW+2, TIME_LINE, ':', att);
  lcd_outdezNAtt(9*FW+2, TIME_LINE, frskyData.hub.min, att, 2);
  lcd_putcAtt(11*FW-1, TIME_LINE, ':', att);
  lcd_outdezNAtt(12*FW-1, TIME_LINE, frskyData.hub.sec, att, 2);
  lcd_status_line();
}

void displayGpsCoord(uint8_t y, char direction, int16_t bp, int16_t ap)
{
  if (frskyData.hub.gpsFix >= 0) {
    if (!direction) direction = '-';
    lcd_outdezAtt(10*FW, y, bp / 100, LEFT); // ddd before '.'
    lcd_putc(lcdLastPos, y, '@');
    uint8_t mn = bp % 100;
    if (g_eeGeneral.gpsFormat == 0) {
      lcd_putc(lcdLastPos+FWNUM, y, direction);
      lcd_outdezNAtt(lcdLastPos+FW+FW+1, y, mn, LEFT|LEADING0, 2); // mm before '.'
      lcd_vline(lcdLastPos, y, 2);
      uint16_t ss = ap * 6;
      lcd_outdezAtt(lcdLastPos+3, y, ss / 1000, LEFT); // ''
      lcd_plot(lcdLastPos, y+FH-2, 0); // small decimal point
      lcd_outdezAtt(lcdLastPos+2, y, ss % 1000, LEFT); // ''
      lcd_vline(lcdLastPos, y, 2);
      lcd_vline(lcdLastPos+2, y, 2);
    }
    else {
      lcd_outdezNAtt(lcdLastPos+FW, y, mn, LEFT|LEADING0, 2); // mm before '.'
      lcd_plot(lcdLastPos, y+FH-2, 0); // small decimal point
      lcd_outdezNAtt(lcdLastPos+2, y, ap, LEFT|UNSIGN|LEADING0, 4); // after '.'
      lcd_putc(lcdLastPos+1, y, direction);
    }
  }
  else {
    // no fix
    lcd_puts(10*FW, y, STR_VCSWFUNC+1/*----*/);
  }
}
#endif

NOINLINE uint8_t getRssiAlarmValue(uint8_t alarm)
{
  return (50 + g_model.frsky.rssiAlarms[alarm].value);
}

void displayVoltageScreenLine(uint8_t y, uint8_t index)
{
  putsStrIdx(0, y, STR_A, index+1, 0);
  putsTelemetryChannel(3*FW+6*FW+4, y, index+TELEM_A1-1, frskyData.analog[index].value, DBLSIZE);
  lcd_putc(12*FW-1, y-FH, '<'); putsTelemetryChannel(17*FW, y-FH, index+TELEM_A1-1, frskyData.analog[index].min, NO_UNIT);
  lcd_putc(12*FW, y, '>');      putsTelemetryChannel(17*FW, y, index+TELEM_A1-1, frskyData.analog[index].max, NO_UNIT);
}

uint8_t barCoord(int16_t value, int16_t min, int16_t max)
{
  return limit((uint8_t)0, (uint8_t)(((int32_t)99 * (value - min)) / (max - min)), (uint8_t)100);
}

void menuTelemetryFrsky(uint8_t event)
{
  if (event == EVT_KEY_FIRST(KEY_EXIT)) {
    chainMenu(menuMainView);
    return;
  }

  switch (event) {
    case EVT_KEY_BREAK(KEY_UP):
      if (s_frsky_view-- == 0)
        s_frsky_view = FRSKY_VIEW_MAX;
      break;

    case EVT_KEY_BREAK(KEY_DOWN):
      if (s_frsky_view++ == FRSKY_VIEW_MAX)
        s_frsky_view = 0;
      break;

    case EVT_KEY_FIRST(KEY_MENU):
      resetTelemetry();
      break;
  }

  lcdDrawTelemetryTopBar();

  if (frskyStreaming >= 0) {
    if (s_frsky_view < MAX_FRSKY_SCREENS) {
      FrSkyScreenData & screen = g_model.frsky.screens[s_frsky_view];
      if (g_model.frsky.screensType & (1<<s_frsky_view)) {
        // Custom Screen with gauges
        uint8_t barHeight = 5;
        for (int8_t i=3; i>=0; i--) {
          FrSkyBarData & bar = screen.bars[i];
          uint8_t source = bar.source;
          int16_t barMin = convertTelemValue(source, bar.barMin);
          int16_t barMax = convertTelemValue(source, 255-bar.barMax);
          if (source && barMax > barMin) {
            uint8_t y = barHeight+6+i*(barHeight+6);
            lcd_putsiAtt(0, y+barHeight-5, STR_VTELEMCHNS, source, 0);
            lcd_rect(25, y, 101, barHeight+2);
            int16_t value = getValue(MIXSRC_FIRST_TELEM+source-2);
            int16_t threshold = 0;
            uint8_t thresholdX = 0;
            if (source <= TELEM_TM2)
              threshold = 0;
            else if (source <= TELEM_RSSI_RX)
              threshold = getRssiAlarmValue(source-TELEM_RSSI_TX);
            else if (source <= TELEM_A2)
              threshold = g_model.frsky.channels[source-TELEM_A1].alarms_value[0];
  #if defined(FRSKY_HUB)
            else
              threshold = convertTelemValue(source, barsThresholds[source-TELEM_ALT]);
  #endif

            if (threshold) {
              thresholdX = barCoord(threshold, barMin, barMax);
              if (thresholdX == 100)
                thresholdX = 0;
            }

            uint8_t width = barCoord(value, barMin, barMax);

            // reversed barshade for T1/T2
            uint8_t barShade = ((threshold > value) ? DOTTED : SOLID);
            if (source == TELEM_T1 || source == TELEM_T2)
              barShade = -barShade;

            lcd_filled_rect(26, y+1, width, barHeight, barShade);

            for (uint8_t j=50; j<125; j+=25)
              if (j>26+thresholdX || j>26+width) lcd_vline(j, y+1, barHeight);

            if (thresholdX) {
              lcd_vlineStip(26+thresholdX, y-2, barHeight+3, DOTTED);
              lcd_hline(25+thresholdX, y-2, 3);
            }
          }
          else {
            barHeight += 2;
          }
        }
        displayRssiLine();
      }
      else {
        // Custom Screen with numbers
        uint8_t fields_count = 0;
        for (uint8_t i=0; i<4; i++) {
          for (uint8_t j=0; j<2; j++) {
            uint8_t field = screen.lines[i].sources[j];
            if (i==3 && j==0) {
              lcd_vline(63, 8, 48);
              if (frskyStreaming > 0) {
#if defined(FRSKY_HUB)
                if (field == TELEM_ACC) {
                  lcd_putsLeft(7*FH+1, STR_ACCEL);
                  lcd_outdezNAtt(4*FW, 7*FH+1, frskyData.hub.accelX, LEFT|PREC2);
                  lcd_outdezNAtt(10*FW, 7*FH+1, frskyData.hub.accelY, LEFT|PREC2);
                  lcd_outdezNAtt(16*FW, 7*FH+1, frskyData.hub.accelZ, LEFT|PREC2);
                  break;
                }
                else if (field == TELEM_GPS_TIME) {
                  displayGpsTime();
                  return;
                }
#endif
              }
              else {
                displayRssiLine();
                return;
              }
            }
            if (field) {
              fields_count++;
              int16_t value = getValue(MIXSRC_FIRST_TELEM+field-2);
              uint8_t att = (i==3 ? NO_UNIT : DBLSIZE|NO_UNIT);
              if (field >= TELEM_TM1 && field <= TELEM_TM2) {
                uint8_t x = (i==3 ? j?80:20 : j?74:10);
                putsTime(x, 1+FH+2*FH*i, value, att, att);
              }
              else {
                putsTelemetryChannel(j ? 128 : 63, i==3 ? 1+7*FH : 1+2*FH+2*FH*i, field-1, value, att);
                lcd_putsiAtt(j*65, 1+FH+2*FH*i, STR_VTELEMCHNS, field, 0);
              }
            }
          }
        }
        lcd_status_line();
        if (fields_count == 0)
          putEvent(event == EVT_KEY_BREAK(KEY_UP) ? event : EVT_KEY_BREAK(KEY_DOWN));
      }
    }
    else if (s_frsky_view == e_frsky_voltages) {
      // Volts / Amps / Watts / mAh
      uint8_t other = 0;
      lcd_putsiAtt(0, 2*FH, STR_VOLTSRC, g_model.frsky.voltsSource+1, 0);
      switch(g_model.frsky.voltsSource) {
        case 0:
        case 1:
          displayVoltageScreenLine(2*FH, g_model.frsky.voltsSource);
          other = !g_model.frsky.voltsSource;
          break;
#if defined(FRSKY_HUB)
        case 2:
          putsTelemetryChannel(3*FW+6*FW+4, 2*FH, TELEM_VFAS-1, frskyData.hub.vfas, DBLSIZE);
          break;
        case 3:
          putsTelemetryChannel(3*FW+6*FW+4, 2*FH, TELEM_CELLS_SUM-1, frskyData.hub.cellsSum, DBLSIZE);
          break;
#endif
      }

      if (g_model.frsky.currentSource) {
        lcd_putsiAtt(0, 4*FH, STR_VOLTSRC, g_model.frsky.currentSource, 0);
        switch(g_model.frsky.currentSource) {
          case 1:
          case 2:
            displayVoltageScreenLine(4*FH, g_model.frsky.currentSource-1);
            break;
#if defined(FRSKY_HUB)
          case 3:
            putsTelemetryChannel(3*FW+6*FW+4, 4*FH, TELEM_CURRENT-1, frskyData.hub.current, DBLSIZE);
            break;
#endif
        }

        putsTelemetryChannel(4, 6*FH, TELEM_POWER-1, frskyData.power, LEFT|DBLSIZE);
        putsTelemetryChannel(3*FW+4+4*FW+6*FW+FW, 6*FH, TELEM_CONSUMPTION-1, frskyData.currentConsumption, DBLSIZE);
      }
      else {
        displayVoltageScreenLine(other ? 5*FH : 4*FH, other);
        if (!other) displayVoltageScreenLine(6*FH, 1);
      }

#if defined(FRSKY_HUB)
      // Cells voltage
      if (frskyData.hub.cellsCount > 0) {
        uint8_t y = 1*FH;
        for (uint8_t k=0; k<frskyData.hub.cellsCount && k<6; k++) {
          uint8_t attr = (barsThresholds[THLD_CELL] && frskyData.hub.cellVolts[k] < barsThresholds[THLD_CELL]) ? BLINK|PREC2 : PREC2;
          lcd_outdezNAtt(21*FW, y, frskyData.hub.cellVolts[k] * 2, attr, 4);
          y += 1*FH;
        }
        lcd_vline(17*FW+4, 8, 47);
      }
#endif

      displayRssiLine();
    }
#if defined(FRSKY_HUB)
    else if (s_frsky_view == e_frsky_after_flight) {
      uint8_t line=1*FH+1;
      if (g_model.frsky.usrProto == USR_PROTO_FRSKY) {
        // Latitude
        lcd_putsLeft(line, STR_LATITUDE);
        displayGpsCoord(line, frskyData.hub.gpsLatitudeNS, frskyData.hub.gpsLatitude_bp, frskyData.hub.gpsLatitude_ap);
        // Longitude
        line+=1*FH+1;
        lcd_putsLeft(line, STR_LONGITUDE);
        displayGpsCoord(line, frskyData.hub.gpsLongitudeEW, frskyData.hub.gpsLongitude_bp, frskyData.hub.gpsLongitude_ap);
        displayGpsTime();
        line+=1*FH+1;
      }
      // Rssi
      lcd_putsLeft(line, STR_MINRSSI);
      lcd_puts(10*FW, line, STR_TX);
      lcd_outdezNAtt(lcdLastPos, line, frskyData.rssi[1].min, LEFT|LEADING0, 2);
      lcd_puts(16*FW, line, STR_RX);
      lcd_outdezNAtt(lcdLastPos, line, frskyData.rssi[0].min, LEFT|LEADING0, 2);
    }
#endif    
  }
  else {
    lcd_putsAtt(22, 40, STR_NODATA, DBLSIZE);
  }
}
