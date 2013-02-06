/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
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

#ifndef FRSKY_H
#define FRSKY_H

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

#define EARTH_RADIUSKM ((uint32_t)6371)
#define EARTH_RADIUS ((uint32_t)111194)

#define VARIO_QUEUE_LENGTH          5

#if defined(FRSKY_HUB)
PACK(struct FrskyHubData {
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

  /* next fields must keep this order! */
  int16_t  minAltitude;
  int16_t  maxAltitude;
  uint16_t maxRpm;
  int16_t  maxTemperature1;
  int16_t  maxTemperature2;
  uint16_t maxGpsSpeed;
  uint16_t maxGpsDistance;
  uint16_t maxCurrent;
  /* end */

  uint16_t vfas;             // 0x39  Added to FrSky protocol for home made sensors with a better precision
  uint16_t volts_bp;         // 0x3A
  uint16_t volts_ap;         // 0x3B
  // end of FrSky Hub data

  uint16_t gpsDistance;
  int16_t  gpsAltitudeOffset;
  uint8_t  varioAltitudeQueuePointer;     // circular-buffer pointer
  uint8_t  minCellIdx;
  int16_t  cellsSum;

  // TODO later uint16_t minVfas;
});

#elif defined(WS_HOW_HIGH)

PACK(struct FrskyHubData {
  int16_t  baroAltitude_bp;     // 0..9,999 meters
  int16_t  baroAltitudeOffset;
  int16_t  minAltitude;
  int16_t  maxAltitude;
});

#endif

struct FrskyData {
  FrskyValueWithMinMax analog[2];
  FrskyValueWithMin    rssi[2];
#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
  FrskyHubData         hub;
#endif
  uint16_t             currentConsumption;
  uint16_t             currentPrescale;
  uint16_t             power;
};

#if defined(FRSKY_HUB)
enum BarThresholdIdx {
  THLD_ALT,
  THLD_RPM,
  THLD_FUEL,
  THLD_T1,
  THLD_T2,
  THLD_SPEED,
  THLD_DIST,
  THLD_GPSALT,
  THLD_CELL,
  THLD_CURRENT,
  THLD_CONSUMPTION,
  THLD_MAX,
};
extern uint8_t barsThresholds[THLD_MAX];
#endif

// Global Fr-Sky telemetry data variables
extern int8_t frskyStreaming; // >0 (true) == data is streaming in. 0 = nodata detected for some time
#if defined(WS_HOW_HIGH)
extern uint8_t frskyUsrStreaming;
#endif

#define SEND_MODEL_ALARMS 6
extern uint8_t frskyAlarmsSendState;

extern FrskyData frskyData;

extern uint8_t frskyTxBuffer[FRSKY_TX_PACKET_SIZE];
extern uint8_t frskyTxBufferCount;

void FRSKY_Init(void);
NOINLINE void check_frsky(void);

inline void FRSKY_setModelAlarms(void)
{
  frskyAlarmsSendState = SEND_MODEL_ALARMS;
}

bool FRSKY_alarmRaised(uint8_t idx);

void resetTelemetry();
uint8_t maxTelemValue(uint8_t channel);
int16_t convertTelemValue(uint8_t channel, uint8_t value);
int16_t convertCswTelemValue(CustomSwData * cs);
NOINLINE uint8_t getRssiAlarmValue(uint8_t alarm);

extern const pm_uint8_t bchunit_ar[];

#if defined(CPUARM)
#define FRSKY_MULTIPLIER_MAX 5
#else
#define FRSKY_MULTIPLIER_MAX 3
#endif

lcdint_t applyChannelRatio(uint8_t channel, lcdint_t val);
void putsTelemetryChannel(xcoord_t x, uint8_t y, uint8_t channel, lcdint_t val, uint8_t att);

#define IS_BARS_SCREEN(screenIndex) (g_model.frsky.screensType & (1<<(screenIndex)))

#endif

