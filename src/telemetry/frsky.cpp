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

#include "../opentx.h"

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
uint8_t frskyStreaming = 0;
#if defined(WS_HOW_HIGH)
uint8_t frskyUsrStreaming = 0;
#endif
uint8_t link_counter = 0;
FrskyData frskyData;

#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
void checkMinMaxAltitude()
{
  if (TELEMETRY_ALT_BP > frskyData.hub.maxAltitude)
    frskyData.hub.maxAltitude = TELEMETRY_ALT_BP;
  if (TELEMETRY_ALT_BP < frskyData.hub.minAltitude)
    frskyData.hub.minAltitude = TELEMETRY_ALT_BP;
}
#endif

#if defined(VARIO) && !defined(FRSKY_SPORT) && (defined(FRSKY_HUB) || defined(WS_HOW_HIGH))
void evalVario(int16_t altitude_bp, uint16_t altitude_ap)
{
  int32_t varioAltitude_cm = (int32_t)altitude_bp * 100 + (altitude_bp > 0 ? altitude_ap : -altitude_ap);
  uint8_t varioAltitudeQueuePointer = frskyData.hub.varioAltitudeQueuePointer + 1;
  if (varioAltitudeQueuePointer >= VARIO_QUEUE_LENGTH)
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

#if defined(FRSKY_HUB)
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

#if defined(GPS)
  if ((uint8_t)structPos == offsetof(FrskySerialData, gpsLatitude_bp)) {
    if (lowByte || byte)
      frskyData.hub.gpsFix = 1;
    else if (frskyData.hub.gpsFix > 0 && frskyData.hub.gpsLatitude_bp > 1)
      frskyData.hub.gpsFix = 0;
  }
  else if ((uint8_t)structPos == offsetof(FrskySerialData, gpsLongitude_bp)) {
    if (lowByte || byte)
      frskyData.hub.gpsFix = 1;
    else if (frskyData.hub.gpsFix > 0 && frskyData.hub.gpsLongitude_bp > 1)
      frskyData.hub.gpsFix = 0;
  }
  
  if ((uint8_t)structPos == offsetof(FrskySerialData, gpsAltitude_bp) ||
      ((uint8_t)structPos >= offsetof(FrskySerialData, gpsAltitude_ap) && (uint8_t)structPos <= offsetof(FrskySerialData, gpsLatitudeNS) && (uint8_t)structPos != offsetof(FrskySerialData, baroAltitude_bp) && (uint8_t)structPos != offsetof(FrskySerialData, baroAltitude_ap))) {
    // If we don't have a fix, we may discard the value
    if (frskyData.hub.gpsFix <= 0)
      return;
  }
#endif
  
  ((uint8_t*)&frskyData.hub)[structPos] = lowByte;
  ((uint8_t*)&frskyData.hub)[structPos+1] = byte;

  switch ((uint8_t)structPos) {

    case offsetof(FrskySerialData, rpm):
      frskyData.hub.rpm *= (uint8_t)60/(g_model.frsky.blades+2);
      if (frskyData.hub.rpm > frskyData.hub.maxRpm)
        frskyData.hub.maxRpm = frskyData.hub.rpm;
      break;

    case offsetof(FrskySerialData, temperature1):
      if (frskyData.hub.temperature1 > frskyData.hub.maxTemperature1)
        frskyData.hub.maxTemperature1 = frskyData.hub.temperature1;
      break;

    case offsetof(FrskySerialData, temperature2):
      if (frskyData.hub.temperature2 > frskyData.hub.maxTemperature2)
        frskyData.hub.maxTemperature2 = frskyData.hub.temperature2;
      break;

    case offsetof(FrskySerialData, current):
      if (frskyData.hub.current > frskyData.hub.maxCurrent)
        frskyData.hub.maxCurrent = frskyData.hub.current;
      break;
      
    case offsetof(FrskySerialData, volts_ap):
#if defined(FAS_BSS)
      frskyData.hub.vfas = (frskyData.hub.volts_bp * 10 + frskyData.hub.volts_ap);
#else
      frskyData.hub.vfas = ((frskyData.hub.volts_bp * 100 + frskyData.hub.volts_ap * 10) * 21) / 110;
#endif
      /* TODO later if (!frskyData.hub.minVfas || frskyData.hub.minVfas > frskyData.hub.vfas)
        frskyData.hub.minVfas = frskyData.hub.vfas; */
      break;

    case offsetof(FrskySerialData, baroAltitude_bp):
      // First received barometer altitude => Altitude offset
      if (!frskyData.hub.baroAltitudeOffset)
        frskyData.hub.baroAltitudeOffset = -frskyData.hub.baroAltitude_bp;
      if (g_model.frsky.varioSource == VARIO_SOURCE_ALTI) {
        evalVario(frskyData.hub.baroAltitude_bp, 0);
      }
      frskyData.hub.baroAltitude_bp += frskyData.hub.baroAltitudeOffset;
      checkMinMaxAltitude();
      break;

    case offsetof(FrskySerialData, baroAltitude_ap):
      if (g_model.frsky.varioSource == VARIO_SOURCE_ALTI_PLUS) {
        evalVario(frskyData.hub.baroAltitude_bp-frskyData.hub.baroAltitudeOffset, frskyData.hub.baroAltitude_ap);
      }
      break;

#if defined(GPS)
    case offsetof(FrskySerialData, gpsAltitude_ap):
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

    case offsetof(FrskySerialData, gpsSpeed_bp):
      // Speed => Max speed
      if (frskyData.hub.gpsSpeed_bp > frskyData.hub.maxGpsSpeed)
        frskyData.hub.maxGpsSpeed = frskyData.hub.gpsSpeed_bp;
      break;
#endif

    case offsetof(FrskySerialData, volts):
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

#if defined(GPS)
    case offsetof(FrskySerialData, hour):
      frskyData.hub.hour = ((uint8_t)(frskyData.hub.hour + g_eeGeneral.timezone + 24)) % 24;
      break;
#endif

    case offsetof(FrskySerialData, accelX):
    case offsetof(FrskySerialData, accelY):
    case offsetof(FrskySerialData, accelZ):
      *(int16_t*)(&((uint8_t*)&frskyData.hub)[structPos]) /= 10;
      break;

  }
}
#endif

#if defined(WS_HOW_HIGH)
void parseTelemWSHowHighByte(uint8_t byte)
{
  if (frskyUsrStreaming < (FRSKY_TIMEOUT10ms*3 - 10)) {
    ((uint8_t*)&frskyData.hub)[offsetof(FrskySerialData, baroAltitude_bp)] = byte;
    checkMinMaxAltitude();
    if (g_model.frsky.varioSource == VARIO_SOURCE_ALTI) {
      evalVario(frskyData.hub.baroAltitude_bp, 0);
    }
  }
  else {
    // At least 100mS passed since last data received
    ((uint8_t*)&frskyData.hub)[offsetof(FrskySerialData, baroAltitude_bp)+1] = byte;
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
        if (IS_USR_PROTO_FRSKY_HUB())
          parseTelemHubByte(packet[i]);
#endif
#if defined(WS_HOW_HIGH)
        if (IS_USR_PROTO_WS_HOW_HIGH())
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
  sei();

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
    *ptr++ = (IS_SOUND_OFF() ? alarm_off : ALARM_LEVEL(channel, alarm));
  }
  else {
    *ptr++ = (RSSI1PKT-alarm);  // f7 - f6
    frskyPushValue(ptr, getRssiAlarmValue(alarm));
    *ptr++ = 0x00;
    *ptr++ = (IS_SOUND_OFF() ? alarm_off : ((2+alarm+g_model.frsky.rssiAlarms[alarm].level) % 4));
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
  *ptr++ = (IS_SOUND_OFF() ? alarm_off : p2);
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

void telemetryInterrupt10ms()
{
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

#if defined(CPUARM)
  frskyData.hub.power = (current * voltage) / 100;
#else
  frskyData.hub.power = ((current>>1) * (voltage>>1)) / 25;
#endif

  if (frskyData.hub.power > frskyData.hub.maxPower)
    frskyData.hub.maxPower = frskyData.hub.power;

  frskyData.hub.currentPrescale += current;
  if (frskyData.hub.currentPrescale >= 3600) {
    frskyData.hub.currentConsumption += 1;
    frskyData.hub.currentPrescale -= 3600;
  }
}

void telemetryWakeup()
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

#if defined(VARIO)
  if (!IS_FAI_ENABLED())
    varioWakeup();
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

#if defined(PCBTARANIS)
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
  if (this->value == 0) {
    this->value = value;
  }
  else {
    sum += value;
    if (link_counter == 0) {
      this->value = sum / 8;
      sum = 0;
    }
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
  frskyData.rssi[0].value = 75;
  frskyData.rssi[1].value = 75;
  frskyData.hub.fuelLevel = 75;
  frskyData.hub.rpm = 12000;

#if defined(GPS)
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
#endif

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
