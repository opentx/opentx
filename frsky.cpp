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
#if !defined(PCBARM)
uint8_t frskyTxBufferCount = 0;
#endif
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
FrskyHubData frskyHubData;
enum BarThresholdIdx {
  THLD_ALT,
  THLD_RPM,
  THLD_FUEL,
  THLD_T1,
  THLD_T2,
  THLD_SPEED,
  THLD_CELL,
  THLD_DIST,
  THLD_MAX,
};
uint8_t barsThresholds[THLD_MAX];
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

  dist = abs(frskyHubData.baroAltitudeOffset ? frskyHubData.baroAltitude_bp : frskyHubData.gpsAltitude_bp);
  result += dist*dist;

  frskyHubData.gpsDistance = isqrt32(result);
  if (frskyHubData.gpsDistance > frskyHubData.maxGpsDistance)
    frskyHubData.maxGpsDistance = frskyHubData.gpsDistance;
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
    if (byte > 0x3b) {
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
      frskyHubData.gpsFix = 1;
    else if (frskyHubData.gpsFix > 0 && frskyHubData.gpsLatitude_bp > 1)
      frskyHubData.gpsFix = 0;
  }
  else if ((uint8_t)structPos == offsetof(FrskyHubData, gpsLongitude_bp)) {
    if (lowByte || byte)
      frskyHubData.gpsFix = 1;
    else if (frskyHubData.gpsFix > 0 && frskyHubData.gpsLongitude_bp > 1)
      frskyHubData.gpsFix = 0;
  }
  
  if ((uint8_t)structPos == offsetof(FrskyHubData, gpsAltitude_bp) ||
      ((uint8_t)structPos >= offsetof(FrskyHubData, gpsAltitude_ap) && (uint8_t)structPos <= offsetof(FrskyHubData, gpsLatitudeNS) && (uint8_t)structPos != offsetof(FrskyHubData, baroAltitude_bp) && (uint8_t)structPos != offsetof(FrskyHubData, baroAltitude_ap))) {
    // If we don't have a fix, we may discard the value
    if (frskyHubData.gpsFix <= 0)
      return;
  }
  
  ((uint8_t*)&frskyHubData)[structPos] = lowByte;
  ((uint8_t*)&frskyHubData)[structPos+1] = byte;

  switch ((uint8_t)structPos) {

    case offsetof(FrskyHubData, rpm):
      frskyHubData.rpm *= (uint8_t)60/(g_model.frsky.blades+2);
      if (frskyHubData.rpm > frskyHubData.maxRpm)
        frskyHubData.maxRpm = frskyHubData.rpm;
      break;

    case offsetof(FrskyHubData, temperature1):
      if (frskyHubData.temperature1 > frskyHubData.maxTemperature1)
        frskyHubData.maxTemperature1 = frskyHubData.temperature1;
      break;

    case offsetof(FrskyHubData, temperature2):
      if (frskyHubData.temperature2 > frskyHubData.maxTemperature2)
        frskyHubData.maxTemperature2 = frskyHubData.temperature2;
      break;
#if defined(VARIO_EXTENDED)
    case offsetof(FrskyHubData, baroAltitude_bp):
    case offsetof(FrskyHubData, baroAltitude_ap):
      if((!g_model.frsky.use_baroAltitude_ap & (offsetof(FrskyHubData, baroAltitude_bp) == (uint8_t)structPos)) |
          (g_model.frsky.use_baroAltitude_ap & (offsetof(FrskyHubData, baroAltitude_ap) == (uint8_t)structPos))){
        //here is baro altitude suggested 0.01m accuracy, i.e. baroAltitude_ap in range [0:99]
        if(g_model.frsky.use_baroAltitude_ap){
          uint16_t alt_sign = frskyHubData.baroAltitude_bp/abs(frskyHubData.baroAltitude_bp);
          frskyHubData.baroAltitude_full = frskyHubData.baroAltitude_bp*100 + frskyHubData.baroAltitude_ap*alt_sign;
        }else{
          frskyHubData.baroAltitude_full = frskyHubData.baroAltitude_bp;
        }
        if(++frskyHubData.queuePointer>=5)
          frskyHubData.queuePointer = 0;

        frskyHubData.baroAltitudeQueue_Acc[frskyHubData.queuePointer] = 
          frskyHubData.baroAltitude_full - frskyHubData.baroAltitude_full_prev;

        frskyHubData.baroAltitude_full_prev = frskyHubData.baroAltitude_full;
          
        frskyHubData.varioSpeed = 0;
		    for(uint8_t vi=0; vi<5; vi++){
          frskyHubData.varioSpeed += frskyHubData.baroAltitudeQueue_Acc[vi];
			  }		  

        // First received barometer altitude => Altitude offset
        if (!frskyHubData.baroAltitudeOffset)
          frskyHubData.baroAltitudeOffset = -frskyHubData.baroAltitude_bp;

        frskyHubData.baroAltitude_bp += frskyHubData.baroAltitudeOffset;
		
        if(g_model.frsky.use_baroAltitude_only){
    	    frskyHubData.Altitude_show = frskyHubData.gpsAltitude_bp;
          if (frskyHubData.baroAltitude_bp > frskyHubData.maxAltitude)
            frskyHubData.maxAltitude = frskyHubData.baroAltitude_bp;
          if (frskyHubData.baroAltitude_bp < frskyHubData.minAltitude)
            frskyHubData.minAltitude = frskyHubData.baroAltitude_bp;
        }

      }
      break;
#else
    case offsetof(FrskyHubData, baroAltitude_bp):
      // First received barometer altitude => Altitude offset
      if (!frskyHubData.baroAltitudeOffset)
        frskyHubData.baroAltitudeOffset = -frskyHubData.baroAltitude_bp;
      frskyHubData.baroAltitude_bp += frskyHubData.baroAltitudeOffset;
      if (frskyHubData.baroAltitude_bp > frskyHubData.maxAltitude)
        frskyHubData.maxAltitude = frskyHubData.baroAltitude_bp;
      if (frskyHubData.baroAltitude_bp < frskyHubData.minAltitude)
        frskyHubData.minAltitude = frskyHubData.baroAltitude_bp;

      {
        int16_t actVario = frskyHubData.baroAltitude_bp - frskyHubData.lastBaroAltitude_bp;
        frskyHubData.varioAcc2 = frskyHubData.varioAcc2 - frskyHubData.varioQueue[frskyHubData.queuePointer];
        frskyHubData.varioQueue[frskyHubData.queuePointer] = actVario;
        uint8_t tmp = frskyHubData.queuePointer + 5;
        if (tmp >= 10)
          tmp -= 10;
        tmp = (uint8_t)frskyHubData.varioQueue[tmp];
        frskyHubData.varioAcc2 = frskyHubData.varioAcc2 + (int8_t)tmp;
        frskyHubData.varioAcc1 = frskyHubData.varioAcc1 + actVario - (int8_t)tmp;
        frskyHubData.varioSpeed = frskyHubData.varioAcc1 - frskyHubData.varioAcc2;
        if (++frskyHubData.queuePointer >= 10)
          frskyHubData.queuePointer = 0;
        frskyHubData.lastBaroAltitude_bp = frskyHubData.baroAltitude_bp;
      }
      break;
#endif
#if defined(VARIO_EXTENDED)
    case offsetof(FrskyHubData, gpsAltitude_ap):
      if(g_model.frsky.use_baroAltitude_ap){
        frskyHubData.gpsAltitude_full = frskyHubData.gpsAltitude_bp*100;
      } else {
        frskyHubData.gpsAltitude_full = frskyHubData.gpsAltitude_bp;
      }
      if (!frskyHubData.gpsAltitudeOffset)
        frskyHubData.gpsAltitudeOffset = -frskyHubData.gpsAltitude_bp;

      frskyHubData.gpsAltitude_bp += frskyHubData.gpsAltitudeOffset;

      if(!g_model.frsky.use_baroAltitude_only){
  	    frskyHubData.Altitude_show = frskyHubData.gpsAltitude_bp;
        if (frskyHubData.gpsAltitude_bp > frskyHubData.maxAltitude)
          frskyHubData.maxAltitude = frskyHubData.gpsAltitude_bp;
        if (frskyHubData.gpsAltitude_bp < frskyHubData.minAltitude)
          frskyHubData.minAltitude = frskyHubData.gpsAltitude_bp;
      }      
      if (!frskyHubData.pilotLatitude && !frskyHubData.pilotLongitude) {
        // First received GPS position => Pilot GPS position
        getGpsPilotPosition();
      }
      else if (frskyHubData.gpsDistNeeded || g_menuStack[0] == menuProcFrsky) {
        getGpsDistance();
      }
      break;
#else
    case offsetof(FrskyHubData, gpsAltitude_ap):
      if (!frskyHubData.gpsAltitudeOffset)
        frskyHubData.gpsAltitudeOffset = -frskyHubData.gpsAltitude_bp;
      frskyHubData.gpsAltitude_bp += frskyHubData.gpsAltitudeOffset;
      if (frskyHubData.gpsAltitude_bp > frskyHubData.maxAltitude)
        frskyHubData.maxAltitude = frskyHubData.gpsAltitude_bp;
      if (frskyHubData.gpsAltitude_bp < frskyHubData.minAltitude)
        frskyHubData.minAltitude = frskyHubData.gpsAltitude_bp;

      if (!frskyHubData.pilotLatitude && !frskyHubData.pilotLongitude) {
        // First received GPS position => Pilot GPS position
        getGpsPilotPosition();
      }
      else if (frskyHubData.gpsDistNeeded || g_menuStack[0] == menuProcFrsky) {
        getGpsDistance();
      }
      break;
#endif
    case offsetof(FrskyHubData, gpsSpeed_bp):
      // Speed => Max speed
      if (frskyHubData.gpsSpeed_bp < frskyHubData.maxGpsSpeed)
        frskyHubData.maxGpsSpeed = frskyHubData.gpsSpeed_bp;
      break;

    case offsetof(FrskyHubData, volts):
      // Voltage => Cell number + Cell voltage
    {
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
      break;
    }

    case offsetof(FrskyHubData, hour):
      frskyHubData.hour = ((uint8_t)(frskyHubData.hour + g_eeGeneral.timezone + 24)) % 24;
      break;

    case offsetof(FrskyHubData, accelX):
    case offsetof(FrskyHubData, accelY):
    case offsetof(FrskyHubData, accelZ):
      *(int16_t*)(&((uint8_t*)&frskyHubData)[structPos]) /= 10;
      break;

  }
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

#if defined(PCBARM)
void processSerialData(uint8_t data)
#else
NOINLINE void processSerialData(uint8_t stat, uint8_t data)
#endif
{
  static uint8_t numPktBytes = 0;
  static uint8_t dataState = frskyDataIdle;

#if !defined(PCBARM)
  if (stat & ((1 << FE0) | (1 << DOR0) | (1 << UPE0))) {
    // discard buffer and start fresh on any comms error
    FrskyRxBufferReady = 0;
    numPktBytes = 0;
  }
  else
#endif
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

#if !defined(PCBARM) && !defined(SIMU)
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
#if defined(PCBARM)
void frskyTransmitBuffer( uint32_t size )
{
  txPdcUsart( frskyTxBuffer, size ) ;
}
#else
// TODO inline ? pass parameter? to avoid #ifdef?
void frskyTransmitBuffer()
{
  UCSR0B |= (1 << UDRIE0); // enable  UDRE0 interrupt
}
#endif

uint8_t FrskyAlarmSendState = 0 ;
inline void FRSKY10mspoll(void)
{
#if defined(PCBARM)
  if (txPdcPending())
#else
  if (frskyTxBufferCount)
#endif
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
    *ptr++ = (g_eeGeneral.beeperMode != e_mode_quiet ? ALARM_LEVEL(channel, alarm) : alarm_off);
    *ptr++ = ALARM_GREATER(channel, alarm);
    frskyPushValue(ptr, g_model.frsky.channels[channel].alarms_value[alarm]);
    *ptr++ = (A22PKT + FrskyAlarmSendState); // fc - fb - fa - f9
  }
  else {
    *ptr++ = (g_eeGeneral.beeperMode != e_mode_quiet ? ((2+alarm+g_model.frsky.rssiAlarms[alarm].level) % 4) : alarm_off);
    *ptr++ = 0x00 ;
    frskyPushValue(ptr, getRssiAlarmValue(alarm));
    *ptr++ = (RSSI1PKT-alarm);  // f7 - f6
  }

  *ptr++ = START_STOP; // Start of packet

#if defined(PCBARM)
  frskyTransmitBuffer(ptr - &frskyTxBuffer[0]);
#else
  frskyTxBufferCount = ptr - &frskyTxBuffer[0];
  frskyTransmitBuffer();
#endif
}

void check_frsky()
{
#if defined(PCBARM)
  rxPdcUsart(processSerialData);              // Send serial data here
#endif

  // Attempt to transmit any waiting Fr-Sky alarm set packets every 50ms (subject to packet buffer availability)
  static uint8_t FrskyDelay = 5;
  if (FrskyAlarmSendState && (--FrskyDelay == 0)) {
    FrskyDelay = 5; // 50ms
    FRSKY10mspoll();
  }

#ifndef SIMU
  if (frskyUsrStreaming > 0) {
    frskyUsrStreaming--;
  }

  if (frskyStreaming > 0) {
    frskyStreaming--;
  }
  else if (g_eeGeneral.enableTelemetryAlarm && (g_model.frsky.channels[0].ratio || g_model.frsky.channels[1].ratio)) {
#if defined (AUDIO)
    if (!(g_tmr10ms % 30)) {
      audioDefevent(AU_WARNING1);
    }
#else
    if (!(g_tmr10ms % 30)) {
      warble = !(g_tmr10ms % 60);
      AUDIO_WARNING2();
    }
#endif
  }
#endif

#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
  static uint16_t s_varioTmr = 0;

  if (isFunctionActive(FUNC_VARIO)) {
#if defined(AUDIO)

#if defined(VARIO_EXTENDED)

#define VARIO_SPEED_LIMIT 10 //m/s
    int16_t verticalSpeed = 0;
    //vertical speed in 0.01m/s now
    if(g_model.frsky.use_baroAltitude_ap)//means if additional data enabled then _ap unit is 0.01
      verticalSpeed = limit((int16_t)(-VARIO_SPEED_LIMIT*100), (int16_t)frskyHubData.varioSpeed, (int16_t)(+VARIO_SPEED_LIMIT*100));
    else
      verticalSpeed = limit((int16_t)-VARIO_SPEED_LIMIT, (int16_t)(frskyHubData.varioSpeed), (int16_t)+VARIO_SPEED_LIMIT)*100;

    uint8_t SoundAltBeepNextFreq = (0);
    uint8_t SoundAltBeepNextTime = (0);
    if(verticalSpeed < g_model.varioSpeedUpMin*VARIO_LIM_MUL && verticalSpeed > g_model.varioSpeedDownMin*(-VARIO_LIM_MUL)) //check thresholds here in cm/s
    {
      SoundAltBeepNextFreq = (0);
      SoundAltBeepNextTime = (0);
    }else{
      SoundAltBeepNextFreq = (((((int32_t)verticalSpeed * 84) + 125000)/10)/10)/20;//;150000)/10)/10)/20;
      SoundAltBeepNextTime = ((1600 - verticalSpeed) / 100);
      if(verticalSpeed > 0){
        if ((int16_t)(g_tmr10ms - s_varioTmr) > (int16_t)(SoundAltBeepNextTime*2)) {
          s_varioTmr = g_tmr10ms;
          audio.playVario(SoundAltBeepNextFreq, SoundAltBeepNextTime);
        }
      } else {//negative vertical speed gives sound without pauses
          audio.playVario(SoundAltBeepNextFreq, 1);
      }
    }  
#else //VARIO_EXTENDED
    uint8_t warble = 0;
    int8_t verticalSpeed = limit((int16_t)-100, (int16_t)(frskyHubData.varioSpeed/10), (int16_t)+100);

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
#endif //VARIO_EXTENDED

#endif //AUDIO
#endif //FRSKY_HUB || WS_HOW_HIGH
  }
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

#if !defined(PCBARM)
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
  memset(frskyAlarms, 0, sizeof(frskyAlarms));
  resetTelemetry();

#if defined(PCBARM)
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

void FrskyRSSI::set(uint8_t value)
{
   if (this->value == 0)
     this->value = value;
   else
     this->value = (((uint16_t)this->value * 7) + value + 4) / 8;
   if (!min || value < min)
     min = value;
}

void FrskyData::set(uint8_t value)
{
  FrskyRSSI::set(value);
  if (!max || value > max)
    max = value;
}

void resetTelemetry()
{
  memset(frskyTelemetry, 0, sizeof(frskyTelemetry));
  memset(frskyRSSI, 0, sizeof(frskyRSSI));
  memset(&frskyHubData, 0, sizeof(frskyHubData));

#if defined(FRSKY_HUB)
  frskyHubData.gpsLatitude_bp = 2;
  frskyHubData.gpsLongitude_bp = 2;
  frskyHubData.gpsFix = -1;
#endif

#ifdef SIMU
  frskyTelemetry[0].set(120);
  frskyRSSI[0].set(75);
  frskyHubData.fuelLevel = 75;
  frskyHubData.rpm = 12000;

  frskyHubData.gpsFix = 1;
  frskyHubData.gpsLatitude_bp = 4401;
  frskyHubData.gpsLatitude_ap = 7710;
  frskyHubData.gpsLongitude_bp = 1006;
  frskyHubData.gpsLongitude_ap = 8872;
  getGpsPilotPosition();

  frskyHubData.gpsLatitude_bp = 4401;
  frskyHubData.gpsLatitude_ap = 7455;
  frskyHubData.gpsLongitude_bp = 1006;
  frskyHubData.gpsLongitude_ap = 9533;
  getGpsDistance();

  frskyHubData.cellsCount = 6;

  frskyHubData.gpsAltitude_bp = 50;
  frskyHubData.gpsAltitude_full = 5000;
  frskyHubData.baroAltitude_bp = 50;
  frskyHubData.baroAltitude_full = 5000;
  frskyHubData.minAltitude = 10;
  frskyHubData.maxAltitude = 500;

  frskyHubData.accelY = 100;
  frskyHubData.temperature1 = -30;
  frskyHubData.maxTemperature1 = 100;
#endif
}

uint8_t maxTelemValue(uint8_t channel)
{
  switch (channel) {
    case TELEM_FUEL:
    case TELEM_RSSI_TX:
    case TELEM_RSSI_RX:
      return 100;
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
      result = value * 4;
      break;
    case TELEM_RPM:
      result = value * 50;
      break;
    case TELEM_T1:
    case TELEM_T2:
      result = (int16_t)value - 30;
      break;
    case TELEM_SPEED:
    case TELEM_CELL:
      result = value * 2;
      break;
    case TELEM_DIST:
      result = value * 8;
      break;
    default:
      result = value;
      break;
  }
  return result;
}

void putsTelemetryValue(uint8_t x, uint8_t y, int16_t val, uint8_t unit, uint8_t att)
{
  lcd_outdezAtt(x, (att & DBLSIZE ? y - FH : y), val, att & (~NO_UNIT)); // TODO we could add this test inside lcd_outdezAtt!
  if (~att & NO_UNIT && unit != UNIT_RAW)
    lcd_putsiAtt(lcd_lastPos/*+1*/, y, STR_VTELEMUNIT, unit, 0);
}

static const pm_uint8_t bchunit_ar[] PROGMEM = {
  UNIT_METERS,  // Alt
  UNIT_RAW,     // Rpm
  UNIT_PERCENT, // Fuel
  UNIT_DEGREES, // T1
  UNIT_DEGREES, // T2
  UNIT_KMH,     // Speed
  UNIT_METERS,  // Dist
};

void putsTelemetryChannel(uint8_t x, uint8_t y, uint8_t channel, int16_t val, uint8_t att)
{
  switch (channel) {
    case TELEM_TM1-1:
    case TELEM_TM2-1:
      putsTime(x-3*FW, y, val, att, att);
      break;
    case TELEM_MIN_A1-1:
    case TELEM_MIN_A2-1:
      channel -= TELEM_MIN_A1-1-MAX_TIMERS;
      // no break
    case TELEM_A1-1:
    case TELEM_A2-1:
      channel -= MAX_TIMERS;
      // A1 and A2
    {
      int16_t converted_value = ((int32_t)val+g_model.frsky.channels[channel].offset) * (g_model.frsky.channels[channel].ratio << g_model.frsky.channels[channel].multiplier) * 2 / 51;
      if (g_model.frsky.channels[channel].type >= UNIT_RAW) {
        converted_value /= 10;
      }
      else {
        if (converted_value < 1000) {
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

    case TELEM_ACCx-1:
    case TELEM_ACCy-1:
    case TELEM_ACCz-1:
      putsTelemetryValue(x, y, val, UNIT_RAW, att|PREC2);
      break;

    case TELEM_RSSI_TX-1:
    case TELEM_RSSI_RX-1:
      putsTelemetryValue(x, y, val, UNIT_RAW, att);
      break;

    default:
      putsTelemetryValue(x, y, val, pgm_read_byte(bchunit_ar+channel-6), att);
      break;
  }
}

enum FrskyViews {
  e_frsky_custom,
  e_frsky_bars,
  e_frsky_a1a2,
  e_frsky_after_flight,
  FRSKY_VIEW_MAX = e_frsky_after_flight
};

static uint8_t s_frsky_view = e_frsky_custom;

void displayRssiLine()
{
  if (frskyStreaming > 0) {
    lcd_hline(0, 55, 128, 0); // separator
    lcd_putsLeft(7*FH+1, STR_TX); lcd_outdezNAtt(4*FW, 7*FH+1, frskyRSSI[1].value, LEADING0, 2);
    lcd_rect(25, 57, 38, 7);
    lcd_filled_rect(26, 58, 9*frskyRSSI[1].value/25, 5, (frskyRSSI[1].value < getRssiAlarmValue(0)) ? DOTTED : SOLID);
    lcd_puts(105, 7*FH+1, STR_RX); lcd_outdezNAtt(105+4*FW-1, 7*FH+1, frskyRSSI[0].value, LEADING0, 2);
    lcd_rect(65, 57, 38, 7);
    uint8_t v = 9*frskyRSSI[0].value/25;
    lcd_filled_rect(66+36-v, 58, v, 5, (frskyRSSI[0].value < getRssiAlarmValue(0)) ? DOTTED : SOLID);
  }
  else {
    lcd_putsAtt(7*FW, 7*FH+1, STR_NODATA, BLINK);
    lcd_status_line();
  }
}

#if defined(FRSKY_HUB)
void displayGpsTime()
{
#define TIME_LINE (7*FH+1)
  uint8_t att = (frskyStreaming > 0 ? LEFT|LEADING0 : LEFT|LEADING0|BLINK);
  lcd_outdezNAtt(6*FW+5, TIME_LINE, frskyHubData.hour, att, 2);
  lcd_putcAtt(8*FW+2, TIME_LINE, ':', att);
  lcd_outdezNAtt(9*FW+2, TIME_LINE, frskyHubData.min, att, 2);
  lcd_putcAtt(11*FW-1, TIME_LINE, ':', att);
  lcd_outdezNAtt(12*FW-1, TIME_LINE, frskyHubData.sec, att, 2);
  lcd_status_line();
}

void displayGpsCoord(uint8_t y, char direction, int16_t bp, int16_t ap)
{
  if (frskyHubData.gpsFix >= 0) {
    if (!direction) direction = '-';

    lcd_outdezAtt(10*FW, y, bp / 100, LEFT); // ddd before '.'
    lcd_putc(lcd_lastPos, y, '@');
    uint8_t mn = bp % 100;
    if (g_eeGeneral.gpsFormat == 0) {
      lcd_putc(lcd_lastPos+FWNUM, y, direction);
      lcd_outdezNAtt(lcd_lastPos+FW+FW+1, y, mn, LEFT|LEADING0, 2); // mm before '.'
      lcd_vline(lcd_lastPos, y, 2);
      uint16_t ss = ap * 6;
      lcd_outdezAtt(lcd_lastPos+3, y, ss / 1000, LEFT); // ''
      lcd_plot(lcd_lastPos, y+FH-2, 0); // small decimal point
      lcd_outdezAtt(lcd_lastPos+2, y, ss % 1000, LEFT); // ''
      lcd_vline(lcd_lastPos, y, 2);
      lcd_vline(lcd_lastPos+2, y, 2);
    }
    else {
      lcd_outdezNAtt(lcd_lastPos+FW, y, mn, LEFT|LEADING0, 2); // mm before '.'
      lcd_plot(lcd_lastPos, y+FH-2, 0); // small decimal point
      lcd_outdezNAtt(lcd_lastPos+2, y, ap, LEFT|UNSIGN|LEADING0, 4); // after '.'
      lcd_putc(lcd_lastPos+1, y, direction);
    }
  }
  else {
    // no fix
    lcd_puts(10*FW, y, STR_VCSWFUNC+1/*----*/);
  }
}
#endif

uint8_t getTelemCustomField(uint8_t line, uint8_t col)
{
  uint8_t result = (col==0 ? (g_model.frskyLines[line] & 0x0f) : ((g_model.frskyLines[line] & 0xf0) / 16));
  result += (((g_model.frskyLinesXtra >> (4*line+2*col)) & 0x03) * 16);
  return result;
}

NOINLINE uint8_t getRssiAlarmValue(uint8_t alarm)
{
  return (50 + g_model.frsky.rssiAlarms[alarm].value);
}

void menuProcFrsky(uint8_t event)
{
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
  if (g_model.timers[0].mode) {
    att = (s_timerState[0]==TMR_BEEPING ? BLINK : 0);
    putsTime(17*FW, 0, s_timerVal[0], att, att);
  }
  lcd_filled_rect(0, 0, DISPLAY_W, 8);

  if (frskyStreaming >= 0) {
    if (s_frsky_view == e_frsky_custom) {
      // The custom view
      uint8_t fields_count = 0;
      for (uint8_t i=0; i<4; i++) {
        for (uint8_t j=0; j<2; j++) {
          uint8_t field = getTelemCustomField(i, j);
          if (i==3 && j==0) {
            lcd_vline(63, 8, 48);
            if (frskyStreaming > 0) {
              if (field == TELEM_ACC) {
                lcd_putsLeft(7*FH+1, STR_ACCEL);
                lcd_outdezNAtt(4*FW, 7*FH+1, frskyHubData.accelX, LEFT|PREC2);
                lcd_outdezNAtt(10*FW, 7*FH+1, frskyHubData.accelY, LEFT|PREC2);
                lcd_outdezNAtt(16*FW, 7*FH+1, frskyHubData.accelZ, LEFT|PREC2);
                break;
              }
              else if (field == TELEM_GPS_TIME) {
                displayGpsTime();
                return;
              }
            }
            else {
              displayRssiLine();
              return;
            }
          }
          if (field) {
            fields_count++;
            int16_t value = getValue(CSW_CHOUT_BASE+NUM_CHNOUT+field-1);
            uint8_t att = (i==3 ? NO_UNIT : DBLSIZE|NO_UNIT);
            if (field <= TELEM_TM2) {
              uint8_t x = (i==3 ? j?80:20 : j?74:10);
              putsTime(x, 1+FH+2*FH*i, value, att, att);
            }
            else {
#if defined(PCBV4)
              if(g_model.frsky.use_baroAltitude_ap & (field == TELEM_VSPD)){
                putsTelemetryChannel(j ? 128 : 63, i==3 ? 1+7*FH : 1+2*FH+2*FH*i, field-1, value, att|PREC2);
              }
              else 
#endif
              {
                putsTelemetryChannel(j ? 128 : 63, i==3 ? 1+7*FH : 1+2*FH+2*FH*i, field-1, value, att);
              }
              lcd_putsiAtt(j*65, 1+FH+2*FH*i, STR_VTELEMCHNS, field, 0);
            }
          }
        }
      }
      if (fields_count == 0) {
        // No bars at all!
        putEvent(event == EVT_KEY_BREAK(KEY_UP) ? event : EVT_KEY_BREAK(KEY_DOWN));
      }
      lcd_status_line();
    }
    else if (s_frsky_view == e_frsky_bars) {
      // The bars
      uint8_t bars_height = 5;
      for (int8_t i=3; i>=0; i--) {
        uint8_t source = g_model.frsky.bars[i].source;
        uint8_t bmin = g_model.frsky.bars[i].barMin * 5;
        uint8_t bmax = (51 - g_model.frsky.bars[i].barMax) * 5;
        if (source && bmax > bmin) {
          lcd_putsiAtt(0, bars_height+bars_height+1+i*(bars_height+6), STR_VTELEMCHNS, source, 0);
          lcd_rect(25, bars_height+6+i*(bars_height+6), 101, bars_height+2);
          int16_t value = getValue(CSW_CHOUT_BASE+NUM_CHNOUT+source-1);
          int16_t threshold = 0;
          uint8_t thresholdX = 0;
          if (source <= TELEM_TM2)
            threshold = 0;
          else if (source <= TELEM_A2)
            threshold = g_model.frsky.channels[source-TELEM_A1].alarms_value[0];
          else if (source <= TELEM_RSSI_RX)
            threshold = getRssiAlarmValue(source-TELEM_RSSI_TX);
          else
            threshold = convertTelemValue(source, barsThresholds[source-TELEM_ALT]);
          int16_t barMin = convertTelemValue(source, bmin);
          int16_t barMax = convertTelemValue(source, bmax);
          if (threshold) {
            thresholdX = (uint8_t)(((int32_t)(threshold - barMin) * (int32_t)100) / (barMax - barMin));
            if (thresholdX > 100)
              thresholdX = 0;
          }
          uint8_t width = (uint8_t)limit((int16_t)0, (int16_t)(((int32_t)100 * (value - barMin)) / (barMax - barMin)), (int16_t)100);

     // reversed barshade for T1/T2
    uint8_t barShade;
    if (source == TELEM_T1 || source == TELEM_T2)
    barShade = ((threshold < value) ? DOTTED : SOLID);
    else
    barShade = ((threshold > value) ? DOTTED : SOLID);
    lcd_filled_rect(26, bars_height+6+1+i*(bars_height+6), width, bars_height, barShade);
    
          for (uint8_t j=50; j<125; j+=25)
            if (j>26+thresholdX || j>26+width) lcd_vline(j, bars_height+6+1+i*(bars_height+6), bars_height);
          if (thresholdX) {
            lcd_vlineStip(26+thresholdX, bars_height+4+i*(bars_height+6), bars_height+3, DOTTED);
            lcd_hline(25+thresholdX, bars_height+4+i*(bars_height+6), 3);
          }
        }
        else {
          bars_height += 2;
        }
      }
      if (bars_height == 13) {
        // No bars at all!
        putEvent(event == EVT_KEY_BREAK(KEY_UP) ? event : EVT_KEY_BREAK(KEY_DOWN));
      }
      displayRssiLine();
    }
    else if (s_frsky_view == e_frsky_a1a2) {
      // Big A1 / A2 with min and max
      uint8_t blink;
      uint8_t y = 2*FH;
      for (uint8_t i=0; i<2; i++) {
        if (g_model.frsky.channels[i].ratio) {
          blink = (FRSKY_alarmRaised(i) ? INVERS : 0);
          putsStrIdx(0, y, STR_A, i+1, TWO_DOTS);
          putsTelemetryChannel(3*FW, y, i+MAX_TIMERS, frskyTelemetry[i].value, blink|DBLSIZE|LEFT);
          lcd_putc(12*FW-1, y-FH, '<'); putsTelemetryChannel(17*FW, y-FH, i+MAX_TIMERS, frskyTelemetry[i].min, NO_UNIT);
          lcd_putc(12*FW, y, '>');      putsTelemetryChannel(17*FW, y, i+MAX_TIMERS, frskyTelemetry[i].max, NO_UNIT);
          y += 3*FH;
        }
      }
#ifdef FRSKY_HUB
      // Cells voltage
      if (frskyHubData.cellsCount > 0) {
        uint8_t y = 1*FH;
        for (uint8_t k=0; k<frskyHubData.cellsCount && k<6; k++) {
          uint8_t attr = (barsThresholds[THLD_CELL] && frskyHubData.cellVolts[k] < barsThresholds[THLD_CELL]) ? BLINK|PREC2 : PREC2;
          lcd_outdezNAtt(21*FW, y, frskyHubData.cellVolts[k] * 2, attr, 4);
          y += 1*FH;
        }
        lcd_vline(17*FW+4, 8, 47);
      }
#endif

      displayRssiLine();
    }
#ifdef FRSKY_HUB
    else if (s_frsky_view == e_frsky_after_flight) {
      uint8_t line=1*FH+1;
      if (g_model.frsky.usrProto == 1) { // FrSky Hub
        // Latitude
        lcd_putsLeft(line, STR_LATITUDE);
        displayGpsCoord(line, frskyHubData.gpsLatitudeNS, frskyHubData.gpsLatitude_bp, frskyHubData.gpsLatitude_ap);
        // Longitude
        line+=1*FH+1;
        lcd_putsLeft(line, STR_LONGITUDE);
        displayGpsCoord(line, frskyHubData.gpsLongitudeEW, frskyHubData.gpsLongitude_bp, frskyHubData.gpsLongitude_ap);
        displayGpsTime();
        line+=1*FH+1;
      }
      // Rssi
      lcd_putsLeft(line, STR_MINRSSI);
      lcd_puts(10*FW, line, STR_TX);
      lcd_outdezNAtt(lcd_lastPos, line, frskyRSSI[1].min, LEFT|LEADING0, 2);
      lcd_puts(16*FW, line, STR_RX);
      lcd_outdezNAtt(lcd_lastPos, line, frskyRSSI[0].min, LEFT|LEADING0, 2);
    }
#endif    
  }
  else {
    lcd_putsAtt(22, 40, STR_NODATA, DBLSIZE);
  }
}
