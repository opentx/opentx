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

#ifndef TELEMETRY_SPORT_H
#define TELEMETRY_SPORT_H

#include <inttypes.h>

#define FRSKY_RX_PACKET_SIZE 9
#define FRSKY_TX_PACKET_SIZE 12

#define TELEMETRY_INIT    0
#define TELEMETRY_OK      1
#define TELEMETRY_KO      2
extern uint8_t telemetryState;

enum AlarmLevel {
  alarm_off = 0,
  alarm_yellow = 1,
  alarm_orange = 2,
  alarm_red = 3
};

#define ALARM_GREATER(channel, alarm) ((g_model.frsky.channels[channel].alarms_greater >> alarm) & 1)
#define ALARM_LEVEL(channel, alarm) ((g_model.frsky.channels[channel].alarms_level >> (2*alarm)) & 3)

class FrskyValueWithMinMax {
 public:
  uint8_t value;
  uint8_t min;
  uint8_t max;
  void set(uint8_t value);
};

PACK(struct FrskySerialData {
    int16_t  spare1;
    int16_t  gpsAltitude_bp;   // 0x01   before punct
    int16_t  temperature1;     // 0x02   -20 .. 250 deg. celcius
    uint16_t rpm;              // 0x03   0..60,000 revs. per minute
    uint16_t fuelLevel;        // 0x04   0, 25, 50, 75, 100 percent
    int16_t  temperature2;     // 0x05   -20 .. 250 deg. celcius
    uint16_t volts;            // 0x06   1/500V increments (0..4.2V)
    uint32_t distFromEarthAxis;//        2 spares reused
    int16_t  gpsAltitude_ap;   // 0x01+8 after punct
    uint16_t spare2[6];
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
    int16_t  spare3;
    uint16_t current;          // 0x28   Current
    int16_t  spare4[7];
    int16_t  varioSpeed;       // 0x30  Vertical speed in cm/s

    int32_t  baroAltitudeOffset;
    int32_t  baroAltitude;
    int32_t  gpsAltitudeOffset;
    uint32_t gpsDistance;

    uint16_t vfas;             // 0x39  Added to FrSky protocol for home made sensors with a better precision
    uint16_t volts_bp;         // 0x3A
    uint16_t volts_ap;         // 0x3B
    uint16_t spare5[4];
    // end of FrSky Hub data

    /* these fields must keep this order! */
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

    uint8_t  gpsDistNeeded;
    int8_t   gpsFix;           // -1=never fixed, 0=fix lost, 1=fixed

    uint16_t currentConsumption;
    uint16_t currentPrescale;
    uint16_t power;

    uint8_t  cellsCount;
    uint16_t cellVolts[12];
    int16_t  cellsSum;
    uint8_t  minCellIdx;
    uint16_t minCellVolts;

    bool     varioHighPrecision;
});

struct FrskyData {
  FrskyValueWithMinMax analog[2];
  FrskyValueWithMinMax rssi[2];
  FrskySerialData hub;
};

#define SEND_RSSI_ALARMS  6
#define SEND_MODEL_ALARMS 4
extern uint8_t frskyAlarmsSendState;

extern FrskyData frskyData;

extern uint8_t frskyTxBuffer[FRSKY_TX_PACKET_SIZE];
extern uint8_t frskyTxBufferCount;

void FRSKY_Init(void);
void FRSKY_End(void);

void telemetryWakeup();
void telemetryInterrupt10ms(void);

inline void frskySendAlarms(void)
{
  frskyAlarmsSendState = SEND_RSSI_ALARMS;
}

bool FRSKY_alarmRaised(uint8_t idx);

void resetTelemetry();

#define TELEMETRY_ALT_BP          (frskyData.hub.baroAltitude / 100)
#define TELEMETRY_ALT_AP          (frskyData.hub.baroAltitude % 100)
#define TELEMETRY_GPS_SPEED_BP    frskyData.hub.gpsSpeed_bp
#define TELEMETRY_GPS_SPEED_AP    frskyData.hub.gpsSpeed_ap
#define TELEMETRY_GPS_ALT_AP      frskyData.hub.gpsAltitude_ap
#define TELEMETRY_GPS_ALT_BP      frskyData.hub.gpsAltitude_bp
#define TELEMETRY_ALT             frskyData.hub.baroAltitude < 0 ? '-' : ' ', abs(frskyData.hub.baroAltitude / 100), abs(frskyData.hub.baroAltitude % 100)
#define TELEMETRY_ALT_FORMAT      "%c%d.%02d,"
#define TELEMETRY_CELLS           frskyData.hub.cellsSum / 10, frskyData.hub.cellsSum % 10, frskyData.hub.cellVolts[0]*2/100, frskyData.hub.cellVolts[0]*2%100, frskyData.hub.cellVolts[1]*2/100, frskyData.hub.cellVolts[1]*2%100, frskyData.hub.cellVolts[2]*2/100, frskyData.hub.cellVolts[2]*2%100, frskyData.hub.cellVolts[3]*2/100, frskyData.hub.cellVolts[3]*2%100, frskyData.hub.cellVolts[4]*2/100, frskyData.hub.cellVolts[4]*2%100, frskyData.hub.cellVolts[5]*2/100, frskyData.hub.cellVolts[5]*2%100
#define TELEMETRY_CELLS_FORMAT    "%d.%d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,"
#define TELEMETRY_CURRENT         frskyData.hub.current / 10, frskyData.hub.current % 10
#define TELEMETRY_CURRENT_FORMAT  "%d.%d,"
#define TELEMETRY_VFAS            frskyData.hub.vfas / 10, frskyData.hub.vfas % 10
#define TELEMETRY_VFAS_FORMAT     "%d.%d,"
#define TELEMETRY_VSPEED          frskyData.hub.varioSpeed < 0 ? '-' : ' ', abs(frskyData.hub.varioSpeed / 100), abs(frskyData.hub.varioSpeed % 100)
#define TELEMETRY_VSPEED_FORMAT   "%c%d.%02d,"

/* Would be great, but f_printf() doesn't take floats...
#define TELEMETRY_ALT             float(frskyData.hub.baroAltitude)/100
#define TELEMETRY_ALT_FORMAT      "%.2f,"
#define TELEMETRY_CELLS           float(frskyData.hub.cellsSum)/10, float(frskyData.hub.cellVolts[0])/100, float(frskyData.hub.cellVolts[1])/100, float(frskyData.hub.cellVolts[2])/100, float(frskyData.hub.cellVolts[3])/100, float(frskyData.hub.cellVolts[4])/100, float(frskyData.hub.cellVolts[5])/100
#define TELEMETRY_CELLS_FORMAT    "%.1f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,"
#define TELEMETRY_CURRENT         float(frskyData.hub.current)/100
#define TELEMETRY_CURRENT_FORMAT  "%.2f,"
#define TELEMETRY_VFAS            float(frskyData.hub.vfas)/10
#define TELEMETRY_VFAS_FORMAT     "%.1f,"
#define TELEMETRY_VSPEED          float(frskyData.hub.varioSpeed)/100
#define TELEMETRY_VSPEED_FORMAT   "%.2f,"
*/

#define TELEMETRY_STREAMING()  (frskyData.rssi[0].value > 0)

#endif

