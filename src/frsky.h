/*
 * Author - Bertrand Songis <bsongis@gmail.com>
 *
 * frsky.cpp original authors - Bryan J.Rentoul (Gruvin) <gruvin@gmail.com> and Philip Moss Adapted from jeti.cpp code by Karl
 * Szmutny <shadow@privy.de>* 
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

struct FrskyData {
  uint8_t value;
  uint8_t min;
  uint8_t max;
  void set(uint8_t value);
};

#ifdef FRSKY_HUB
struct FrskyHubData {
  int16_t  gpsAltitude_bp;   // before punct
  int16_t  temperature1;     // -20 .. 250 deg. celcius
  uint16_t rpm;              // 0..60,000 revs. per minute
  uint16_t fuelLevel;        // 0, 25, 50, 75, 100 percent
  int16_t  temperature2;     // -20 .. 250 deg. celcius
  uint16_t volts;            // 1/500V increments (0..4.2V)
  int16_t  gpsAltitude_ap;   // after punct
  uint16_t baroAltitude;     // 0..9,999 meters
  uint16_t gpsSpeed_bp;      // before punct
  uint16_t gpsLongitude_bp;  // before punct
  uint16_t gpsLatitude_bp;   // before punct
  uint16_t gpsCourse_bp;     // before punct (0..359.99 deg. -- seemingly 2-decimal precision)
  uint8_t  day;
  uint8_t  month;
  uint16_t year;
  uint8_t  hour;
  uint8_t  min;
  uint16_t sec;
  uint16_t gpsSpeed_ap;
  uint16_t gpsLongitude_ap;
  uint16_t gpsLatitude_ap;
  uint16_t gpsCourse_ap;
  uint16_t gpsLongitudeEW;   // East/West
  uint16_t gpsLatitudeNS;    // North/South
  int16_t  accelX;           // 1/256th gram (-8g ~ +8g)
  int16_t  accelY;           // 1/256th gram (-8g ~ +8g)
  int16_t  accelZ;           // 1/256th gram (-8g ~ +8g)
};

extern struct FrskyHubData frskyHubData;
#endif

// Global Fr-Sky telemetry data variables
extern uint8_t frskyStreaming; // >0 (true) == data is streaming in. 0 = nodata detected for some time

#define SEND_MODEL_ALARMS 4
#define SEND_RSSI_ALARMS  (SEND_MODEL_ALARMS + 2)
extern uint8_t FrskyAlarmSendState;

extern FrskyData frskyTelemetry[2];
extern FrskyData frskyRSSI[2];

void FRSKY_Init(void);
void FRSKY10mspoll(void);

inline void FRSKY_setModelAlarms(void)
{
  FrskyAlarmSendState = SEND_MODEL_ALARMS;
}

inline void FRSKY_setRSSIAlarms(void)
{
  FrskyAlarmSendState = SEND_RSSI_ALARMS;
}

bool FRSKY_alarmRaised(uint8_t idx);

void resetTelemetry();

#endif

