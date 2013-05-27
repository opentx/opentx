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

#ifndef TELEMETRY_FRSKY_H
#define TELEMETRY_FRSKY_H

#define FRSKY_RX_PACKET_SIZE 19
#define FRSKY_TX_PACKET_SIZE 12

#include <inttypes.h>

// .20 seconds
#define FRSKY_TIMEOUT10ms 20

enum AlarmLevel {
  alarm_off = 0,
  alarm_yellow = 1,
  alarm_orange = 2,
  alarm_red = 3
};

#define ALARM_GREATER(channel, alarm) ((g_model.frsky.channels[channel].alarms_greater >> alarm) & 1)
#define ALARM_LEVEL(channel, alarm) ((g_model.frsky.channels[channel].alarms_level >> (2*alarm)) & 3)

class FrskyValueWithMin {
 public:
  uint8_t value;
  uint8_t min;
  uint16_t sum;
  void set(uint8_t value);
};

class FrskyValueWithMinMax: public FrskyValueWithMin {
 public:
  uint8_t max;
  void set(uint8_t value, uint8_t unit);
};

#define VARIO_QUEUE_LENGTH          5

#if defined(FRSKY_HUB)
PACK(struct FrskySerialData {
  int16_t  baroAltitudeOffset;//       spare reused
  int16_t  gpsAltitude_bp;   // 0x01   before punct
  int16_t  temperature1;     // 0x02   -20 .. 250 deg. celcius
  uint16_t rpm;              // 0x03   0..60,000 revs. per minute
  uint16_t fuelLevel;        // 0x04   0, 25, 50, 75, 100 percent
  int16_t  temperature2;     // 0x05   -20 .. 250 deg. celcius
  uint16_t volts;            // 0x06   1/500V increments (0..4.2V)
  uint32_t distFromEarthAxis;//        2 spares reused
  int16_t  gpsAltitude_ap;   // 0x01+8 after punct
  uint8_t  cellVolts[12];    //        6 spares reused
  int16_t  baroAltitude_bp;  // 0x10   0..9,999 meters
  uint16_t gpsSpeed_bp;      // 0x11   before punct
  uint16_t gpsLongitude_bp;  // 0x12   before punct
  uint16_t gpsLatitude_bp;   // 0x13   before punct
  uint16_t gpsCourse_bp;     // 0x14   before punct (0..359.99 deg. -- seemingly 2-decimal precision)
  uint8_t  day;              // 0x15
  uint8_t  month;            // 0x15
  uint16_t year;             // 0x16
  uint8_t  hour;             // 0x17
  uint8_t  min;              // 0x17
  uint16_t sec;              // 0x18
  uint16_t gpsSpeed_ap;      // 0x11+8
  uint16_t gpsLongitude_ap;  // 0x12+8
  uint16_t gpsLatitude_ap;   // 0x13+8
  uint16_t gpsCourse_ap;     // 0x14+8
  uint32_t pilotLatitude;    //        2 spares reused
  uint32_t pilotLongitude;   //        2 spares reused
  uint16_t baroAltitude_ap;  // 0x21   after punct
  uint16_t gpsLongitudeEW;   // 0x1A+8 East/West
  uint16_t gpsLatitudeNS;    // 0x1B+8 North/South
  int16_t  accelX;           // 0x24   1/256th gram (-8g ~ +8g)
  int16_t  accelY;           // 0x25   1/256th gram (-8g ~ +8g)
  int16_t  accelZ;           // 0x26   1/256th gram (-8g ~ +8g)
  uint8_t  gpsDistNeeded:1;  //        1bits out of 16bits spare reused
  int8_t   gpsFix:2;         //        2bits out of 16bits spare reused: -1=never fixed, 0=not fixed now, 1=fixed
  uint8_t  spare1:1;          //
  uint8_t  cellsCount:4;     //        4bits out of 16bits spare reused
  uint8_t  minCellVolts;     //        8bits out of 16bits spare reused
  uint16_t current;          // 0x28   Current
  int16_t  varioAltitudeQueue[VARIO_QUEUE_LENGTH]; //circular buffer
  int32_t  varioAltitude_cm;
  int16_t  varioSpeed;       // 0x30  Vertical speed in cm/s

  uint16_t gpsDistance;
  int16_t  gpsAltitudeOffset;
  uint8_t  varioAltitudeQueuePointer;     // circular-buffer pointer
  uint8_t  minCellIdx;
  int16_t  cellsSum;
  uint16_t currentConsumption;
  uint16_t currentPrescale;
  uint16_t power;
  int16_t  spare2;

  uint16_t vfas;             // 0x39  Added to FrSky protocol for home made sensors with a better precision
  uint16_t volts_bp;         // 0x3A
  uint16_t volts_ap;         // 0x3B
  // end of FrSky Hub data

  /* next fields must keep this order! */
  int16_t  minAltitude;
  int16_t  maxAltitude;
  uint16_t maxRpm;
  int16_t  maxTemperature1;
  int16_t  maxTemperature2;
  uint16_t maxGpsSpeed;
  uint16_t maxGpsDistance;
  uint16_t maxCurrent;
  uint16_t maxPower;
  /* end */
});
#elif defined(WS_HOW_HIGH)
PACK(struct FrskySerialData {
  int16_t  baroAltitude_bp;     // 0..9,999 meters
  int16_t  baroAltitudeOffset;
  int16_t  minAltitude;
  int16_t  maxAltitude;
  uint16_t currentConsumption;
  uint16_t currentPrescale;
  uint16_t power;
  uint16_t maxPower;
#if defined(VARIO)
  int16_t  varioAltitudeQueue[VARIO_QUEUE_LENGTH]; //circular buffer
  uint8_t  varioAltitudeQueuePointer;     // circular-buffer pointer
  int32_t  varioAltitude_cm;
  int16_t  varioSpeed;       // Vertical speed in cm/s
#endif
});
#elif defined(VARIO)
PACK(struct FrskySerialData {
  int16_t  varioAltitudeQueue[VARIO_QUEUE_LENGTH]; //circular buffer
  uint8_t  varioAltitudeQueuePointer;     // circular-buffer pointer
  int32_t  varioAltitude_cm;
  int16_t  varioSpeed;       // Vertical speed in cm/s
  uint16_t currentConsumption;
  uint16_t currentPrescale;
  uint16_t power;
  uint16_t maxPower;
});
#else
PACK(struct FrskySerialData {
  uint16_t currentConsumption;
  uint16_t currentPrescale;
  uint16_t power;
  uint16_t maxPower;
});
#endif

struct FrskyData {
  FrskyValueWithMinMax analog[2];
  FrskyValueWithMin    rssi[2];
  FrskySerialData hub;
};

// Global Fr-Sky telemetry data variables
extern uint8_t frskyStreaming; // >0 (true) == data is streaming in. 0 = nodata detected for some time

#if defined(WS_HOW_HIGH)
extern uint8_t frskyUsrStreaming;
#endif

#define SEND_RSSI_ALARMS  6
#define SEND_MODEL_ALARMS 4
extern uint8_t frskyAlarmsSendState;

extern FrskyData frskyData;

extern uint8_t frskyTxBuffer[FRSKY_TX_PACKET_SIZE];
extern uint8_t frskyTxBufferCount;

void FRSKY_Init(void);
#define FRSKY_End()

void telemetryWakeup(void);
void telemetryInterrupt10ms(void);

inline void frskySendAlarms(void)
{
  frskyAlarmsSendState = SEND_RSSI_ALARMS;
}

bool FRSKY_alarmRaised(uint8_t idx);

void resetTelemetry();

#define TELEMETRY_ALT_BP       frskyData.hub.baroAltitude_bp
#define TELEMETRY_ALT_AP       frskyData.hub.baroAltitude_ap
#define TELEMETRY_GPS_ALT_AP   frskyData.hub.gpsAltitude_ap
#define TELEMETRY_GPS_ALT_BP   frskyData.hub.gpsAltitude_bp
#define TELEMETRY_GPS_SPEED_BP frskyData.hub.gpsSpeed_bp
#define TELEMETRY_GPS_SPEED_AP frskyData.hub.gpsSpeed_ap

#define TELEMETRY_STREAMING()  (frskyStreaming > 0)

#endif

