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

#ifndef FRSKY_H
#define FRSKY_H

#include <inttypes.h>

#define FRSKY_SPORT_BAUDRATE      57600
#define FRSKY_D_BAUDRATE          9600

#define FRSKY_SPORT_PACKET_SIZE   9

#define FRSKY_TIMEOUT10ms         100 // 1 second
#define WSHH_TIMEOUT10ms          60  // 600ms

#define FRSKY_SPORT_AVERAGING     4
#define FRSKY_D_AVERAGING         8

// Global Fr-Sky telemetry data variables
extern uint8_t frskyStreaming; // >0 (true) == data is streaming in. 0 = nodata detected for some time

#if defined(WS_HOW_HIGH)
extern uint8_t frskyUsrStreaming;
#endif

extern uint8_t link_counter;

#if defined(CPUARM)
enum TelemetryStates {
  TELEMETRY_INIT,
  TELEMETRY_OK,
  TELEMETRY_KO
};

extern uint8_t telemetryState;
#endif

#if defined(CPUARM)
#define TELEMETRY_AVERAGE_COUNT 3     //we actually average one more reading!
#define RAW_FRSKY_MINMAX(v)     v.values[TELEMETRY_AVERAGE_COUNT-1]
class FrskyValueWithMin {
  public:
    uint8_t value;      //fitered value (average of last TELEMETRY_AVERAGE_COUNT+1 values)
    uint8_t min;        
    uint8_t values[TELEMETRY_AVERAGE_COUNT];
    void set(uint8_t value);
};
#else
#define RAW_FRSKY_MINMAX(v)     v.value
class FrskyValueWithMin {
  public:
    uint8_t value;
    uint8_t min;
    uint16_t sum;
    void set(uint8_t value);
};
#endif

class FrskyValueWithMinMax: public FrskyValueWithMin {
  public:
    uint8_t max;
    void set(uint8_t value, uint8_t unit);
};

#if defined(CPUARM)
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
    uint16_t maxAirSpeed;
    int16_t  minCell;
    int16_t  minCells;
    int16_t  minVfas;
    uint16_t maxCurrent;
    uint16_t maxPower;
    /* end */

    uint8_t  gpsDistNeeded:1;  //        1bits out of 16bits spare reused
    int8_t   gpsFix:2;         //        2bits out of 16bits spare reused: -1=never fixed, 0=not fixed now, 1=fixed
    uint8_t  openXsensor:1;    //        1bits out of 16bits spare reused: we receive data from the openXsensor
    uint8_t  varioHighPrecision:1;
    uint8_t  spare6:3;

    int32_t  gpsAltitude;

    uint16_t currentConsumption;
    uint16_t currentPrescale;
    uint16_t power;

    uint8_t  cellsCount;
    uint8_t  sensorCellsCount[2];
    uint16_t cellVolts[12];
    int16_t  cellsSum;
    uint16_t cellsState;
    uint16_t minCellVolts;

    uint16_t airSpeed;
    uint16_t dTE;
});

#elif defined(FRSKY_HUB)

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
  uint8_t  openXsensor:1;    //        1bits out of 16bits spare reused: we receive data from the openXsensor
  uint8_t  cellsCount:4;     //        4bits out of 16bits spare reused
  uint8_t  minCellVolts;     //        8bits out of 16bits spare reused
  uint16_t current;          // 0x28   Current
  int16_t  spare[5];
  int32_t  varioAltitude_cm;
  int16_t  varioSpeed;       // 0x30  Vertical speed in cm/s

  uint16_t gpsDistance;
  int16_t  gpsAltitudeOffset;
  uint8_t  varioAltitudeQueuePointer;     // circular-buffer pointer
  uint8_t  minCellIdx;
  int16_t  cellsSum;
  uint16_t currentConsumption; // 0x35 openXsensor only! Otherwise calculated by the Tx from current
  uint16_t currentPrescale;
  uint16_t power;              // 0x37 openXsensor only! Otherwise calculated by the Tx from current and voltage
  int16_t  airSpeed;

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
  uint16_t maxAirSpeed;
  int16_t  minCell;
  int16_t  minCells;
  int16_t  minVfas;
  uint16_t maxCurrent;
  uint16_t maxPower;
  /* end */

  int16_t  dTE;
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
  int16_t  varioSpeed;       // Vertical speed in cm/s
#endif
});
#elif defined(VARIO)
PACK(struct FrskySerialData {
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

enum TelemAnas {
  TELEM_ANA_A1,
  TELEM_ANA_A2,
#if defined(CPUARM)
  TELEM_ANA_A3,
  TELEM_ANA_A4,
#endif
  TELEM_ANA_COUNT
};

struct FrskyData {
  FrskyValueWithMinMax analog[TELEM_ANA_COUNT];
  FrskyValueWithMin    rssi[2];
#if defined(CPUARM)
  FrskyValueWithMin    swr;
  uint16_t xjtVersion;
#endif
  FrskySerialData hub;  
};

#define IS_VALID_XJT_VERSION() (frskyData.xjtVersion != 0 && frskyData.xjtVersion != 0xff)

enum AlarmLevel {
  alarm_off = 0,
  alarm_yellow = 1,
  alarm_orange = 2,
  alarm_red = 3
};

#define ALARM_GREATER(channel, alarm)     ((g_model.frsky.channels[channel].alarms_greater >> alarm) & 1)
#define ALARM_LEVEL(channel, alarm)       ((g_model.frsky.channels[channel].alarms_level >> (2*alarm)) & 3)

#if defined(CPUARM)
  #define TELEMETRY_STREAMING()           (frskyData.rssi[0].value > 0)

  #define TELEMETRY_CELL_VOLTAGE_MUTLIPLIER  1

  #define TELEMETRY_BARO_ALT_AVAILABLE()  (frskyData.hub.baroAltitudeOffset)
  #define TELEMETRY_BARO_ALT_UNIT         (IS_IMPERIAL_ENABLE() ? LENGTH_UNIT_IMP : LENGTH_UNIT_METR)

  #define TELEMETRY_GPS_SPEED_BP          frskyData.hub.gpsSpeed_bp
  #define TELEMETRY_GPS_SPEED_AP          frskyData.hub.gpsSpeed_ap

  #define TELEMETRY_ABSOLUTE_GPS_ALT      (frskyData.hub.gpsAltitude)
  #define TELEMETRY_RELATIVE_GPS_ALT      (frskyData.hub.gpsAltitude + frskyData.hub.gpsAltitudeOffset)
  #define TELEMETRY_RELATIVE_GPS_ALT_BP   (TELEMETRY_RELATIVE_GPS_ALT / 100)

  #define TELEMETRY_RELATIVE_BARO_ALT_BP  (frskyData.hub.baroAltitude / 100)
  #define TELEMETRY_RELATIVE_BARO_ALT_AP  (frskyData.hub.baroAltitude % 100)

  #define TELEMETRY_BARO_ALT_PREPARE()    div_t baroAltitudeDivision = div(getConvertedTelemetryValue(frskyData.hub.baroAltitude, UNIT_DIST), 100)
  #define TELEMETRY_BARO_ALT_FORMAT       "%c%d.%02d,"
  #define TELEMETRY_BARO_ALT_ARGS         frskyData.hub.baroAltitude < 0 ? '-' : ' ', abs(baroAltitudeDivision.quot), abs(baroAltitudeDivision.rem),
  #define TELEMETRY_GPS_ALT_FORMAT        "%c%d.%02d,"
  #define TELEMETRY_GPS_ALT_ARGS          frskyData.hub.gpsAltitude < 0 ? '-' : ' ', abs(frskyData.hub.gpsAltitude / 100), abs(frskyData.hub.gpsAltitude % 100),
  #define TELEMETRY_SPEED_UNIT            (IS_IMPERIAL_ENABLE() ? SPEED_UNIT_IMP : SPEED_UNIT_METR)
  #define TELEMETRY_GPS_SPEED_FORMAT      "%d,"
  #define TELEMETRY_GPS_SPEED_ARGS        frskyData.hub.gpsSpeed_bp,
  #if defined(CPUARM)
    #define TELEMETRY_CELLS_ARGS          frskyData.hub.cellsSum / 10, frskyData.hub.cellsSum % 10, TELEMETRY_CELL_VOLTAGE(0)/100, TELEMETRY_CELL_VOLTAGE(0)%100, TELEMETRY_CELL_VOLTAGE(1)/100, TELEMETRY_CELL_VOLTAGE(1)%100, TELEMETRY_CELL_VOLTAGE(2)/100, TELEMETRY_CELL_VOLTAGE(2)%100, TELEMETRY_CELL_VOLTAGE(3)/100, TELEMETRY_CELL_VOLTAGE(3)%100, TELEMETRY_CELL_VOLTAGE(4)/100, TELEMETRY_CELL_VOLTAGE(4)%100, TELEMETRY_CELL_VOLTAGE(5)/100, TELEMETRY_CELL_VOLTAGE(5)%100, TELEMETRY_CELL_VOLTAGE(6)/100, TELEMETRY_CELL_VOLTAGE(6)%100, TELEMETRY_CELL_VOLTAGE(7)/100, TELEMETRY_CELL_VOLTAGE(7)%100, TELEMETRY_CELL_VOLTAGE(8)/100, TELEMETRY_CELL_VOLTAGE(8)%100, TELEMETRY_CELL_VOLTAGE(9)/100, TELEMETRY_CELL_VOLTAGE(9)%100, TELEMETRY_CELL_VOLTAGE(10)/100, TELEMETRY_CELL_VOLTAGE(10)%100, TELEMETRY_CELL_VOLTAGE(11)/100, TELEMETRY_CELL_VOLTAGE(11)%100,
    #define TELEMETRY_CELLS_FORMAT        "%d.%d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,"
    #define TELEMETRY_CELLS_LABEL         "Cell volts,Cell 1,Cell 2,Cell 3,Cell 4,Cell 5,Cell 6,Cell 7,Cell 8,Cell 9,Cell 10,Cell 11,Cell 12,"
  #else
    #define TELEMETRY_CELLS_ARGS          frskyData.hub.cellsSum / 10, frskyData.hub.cellsSum % 10, TELEMETRY_CELL_VOLTAGE(0)/100, TELEMETRY_CELL_VOLTAGE(0)%100, TELEMETRY_CELL_VOLTAGE(1)/100, TELEMETRY_CELL_VOLTAGE(1)%100, TELEMETRY_CELL_VOLTAGE(2)/100, TELEMETRY_CELL_VOLTAGE(2)%100, TELEMETRY_CELL_VOLTAGE(3)/100, TELEMETRY_CELL_VOLTAGE(3)%100, TELEMETRY_CELL_VOLTAGE(4)/100, TELEMETRY_CELL_VOLTAGE(4)%100, TELEMETRY_CELL_VOLTAGE(5)/100, TELEMETRY_CELL_VOLTAGE(5)%100,
    #define TELEMETRY_CELLS_FORMAT        "%d.%d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,"
    #define TELEMETRY_CELLS_LABEL         "Cell volts,Cell 1,Cell 2,Cell 3,Cell 4,Cell 5,Cell 6,"
  #endif
  #define TELEMETRY_CURRENT_FORMAT        "%d.%d,"
  #define TELEMETRY_CURRENT_ARGS          frskyData.hub.current / 10, frskyData.hub.current % 10,
  #define TELEMETRY_VFAS_FORMAT           "%d.%d,"
  #define TELEMETRY_VFAS_ARGS             frskyData.hub.vfas / 10, frskyData.hub.vfas % 10,
  #define TELEMETRY_VSPEED_FORMAT         "%c%d.%02d,"
  #define TELEMETRY_VSPEED_ARGS           frskyData.hub.varioSpeed < 0 ? '-' : ' ', abs(frskyData.hub.varioSpeed / 100), abs(frskyData.hub.varioSpeed % 100),
  #define TELEMETRY_ASPEED_FORMAT         "%d.%d,"
  #define TELEMETRY_ASPEED_ARGS           frskyData.hub.airSpeed / 10, frskyData.hub.airSpeed % 10,
  #define TELEMETRY_OPENXSENSOR()         (0)
#else
  #define TELEMETRY_STREAMING()           (frskyStreaming > 0)

  #define TELEMETRY_CELL_VOLTAGE_MUTLIPLIER  2

  #define TELEMETRY_BARO_ALT_AVAILABLE()  (frskyData.hub.baroAltitudeOffset)
  #define TELEMETRY_BARO_ALT_UNIT         (IS_IMPERIAL_ENABLE() ? LENGTH_UNIT_IMP : LENGTH_UNIT_METR)

  #define TELEMETRY_RELATIVE_BARO_ALT_BP  frskyData.hub.baroAltitude_bp
  #define TELEMETRY_RELATIVE_BARO_ALT_AP  frskyData.hub.baroAltitude_ap
  #define TELEMETRY_RELATIVE_GPS_ALT_BP   frskyData.hub.gpsAltitude_bp
  #define TELEMETRY_GPS_SPEED_BP          frskyData.hub.gpsSpeed_bp
  #define TELEMETRY_GPS_SPEED_AP          frskyData.hub.gpsSpeed_ap

  #define TELEMETRY_BARO_ALT_PREPARE()
  #define TELEMETRY_BARO_ALT_FORMAT       "%d,"
  #define TELEMETRY_BARO_ALT_ARGS         frskyData.hub.baroAltitude_bp,
  #define TELEMETRY_GPS_ALT_FORMAT        "%d,"
  #define TELEMETRY_GPS_ALT_ARGS          frskyData.hub.gpsAltitude_bp,
  #define TELEMETRY_SPEED_UNIT            (IS_IMPERIAL_ENABLE() ? SPEED_UNIT_IMP : SPEED_UNIT_METR)
  #define TELEMETRY_GPS_SPEED_FORMAT      "%d,"
  #define TELEMETRY_GPS_SPEED_ARGS        frskyData.hub.gpsSpeed_bp,
  #if defined(CPUARM)
    #define TELEMETRY_CELLS_ARGS          frskyData.hub.cellsSum / 10, frskyData.hub.cellsSum % 10, frskyData.hub.cellVolts[0]*2/100, frskyData.hub.cellVolts[0]*2%100, frskyData.hub.cellVolts[1]*2/100, frskyData.hub.cellVolts[1]*2%100, frskyData.hub.cellVolts[2]*2/100, frskyData.hub.cellVolts[2]*2%100, frskyData.hub.cellVolts[3]*2/100, frskyData.hub.cellVolts[3]*2%100, frskyData.hub.cellVolts[4]*2/100, frskyData.hub.cellVolts[4]*2%100, frskyData.hub.cellVolts[5]*2/100, frskyData.hub.cellVolts[5]*2%100, frskyData.hub.cellVolts[6]*2/100, frskyData.hub.cellVolts[6]*2%100, frskyData.hub.cellVolts[7]*2/100, frskyData.hub.cellVolts[7]*2%100, frskyData.hub.cellVolts[8]*2/100, frskyData.hub.cellVolts[8]*2%100, frskyData.hub.cellVolts[9]*2/100, frskyData.hub.cellVolts[9]*2%100, frskyData.hub.cellVolts[10]*2/100, frskyData.hub.cellVolts[10]*2%100, frskyData.hub.cellVolts[11]*2/100, frskyData.hub.cellVolts[11]*2%100,
    #define TELEMETRY_CELLS_FORMAT        "%d.%d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,"
    #define TELEMETRY_CELLS_LABEL         "Cell volts,Cell 1,Cell 2,Cell 3,Cell 4,Cell 5,Cell 6,Cell 7,Cell 8,Cell 9,Cell 10,Cell 11,Cell 12,"
  #else
    #define TELEMETRY_CELLS_ARGS          frskyData.hub.cellsSum / 10, frskyData.hub.cellsSum % 10, frskyData.hub.cellVolts[0]*2/100, frskyData.hub.cellVolts[0]*2%100, frskyData.hub.cellVolts[1]*2/100, frskyData.hub.cellVolts[1]*2%100, frskyData.hub.cellVolts[2]*2/100, frskyData.hub.cellVolts[2]*2%100, frskyData.hub.cellVolts[3]*2/100, frskyData.hub.cellVolts[3]*2%100, frskyData.hub.cellVolts[4]*2/100, frskyData.hub.cellVolts[4]*2%100, frskyData.hub.cellVolts[5]*2/100, frskyData.hub.cellVolts[5]*2%100,
    #define TELEMETRY_CELLS_FORMAT        "%d.%d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,%d.%02d,"
    #define TELEMETRY_CELLS_LABEL         "Cell volts,Cell 1,Cell 2,Cell 3,Cell 4,Cell 5,Cell 6,"
  #endif
  #define TELEMETRY_CURRENT_FORMAT        "%d.%02d,"
  #define TELEMETRY_CURRENT_ARGS          frskyData.hub.current / 100, frskyData.hub.current % 100,
  #define TELEMETRY_VFAS_FORMAT           "%d.%d,"
  #define TELEMETRY_VFAS_ARGS             frskyData.hub.vfas / 10, frskyData.hub.vfas % 10,
  #define TELEMETRY_VSPEED_FORMAT         "%c%d.%02d,"
  #define TELEMETRY_VSPEED_ARGS           frskyData.hub.varioSpeed < 0 ? '-' : ' ', frskyData.hub.varioSpeed / 100, frskyData.hub.varioSpeed % 100,
  #define TELEMETRY_ASPEED_FORMAT         "%d.%d,"
  #define TELEMETRY_ASPEED_ARGS           frskyData.hub.airSpeed / 10, frskyData.hub.airSpeed % 10,

  #if defined(FRSKY_HUB)
    #define TELEMETRY_OPENXSENSOR()       (frskyData.hub.openXsensor)
  #else
    #define TELEMETRY_OPENXSENSOR()       (0)
  #endif
#endif

#define TELEMETRY_CELL_VOLTAGE(k)         (frskyData.hub.cellVolts[k] * TELEMETRY_CELL_VOLTAGE_MUTLIPLIER)
#define TELEMETRY_MIN_CELL_VOLTAGE        (frskyData.hub.minCellVolts * TELEMETRY_CELL_VOLTAGE_MUTLIPLIER)

extern FrskyData frskyData;

#define START_STOP              0x7e
#define BYTESTUFF               0x7d
#define STUFF_MASK              0x20

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

#if defined(PCBTARANIS)
  #define frskySendAlarms()
#else
  #define SEND_RSSI_ALARMS  6
  #define SEND_MODEL_ALARMS 4
  extern uint8_t frskyAlarmsSendState;
  #define FRSKY_TX_PACKET_SIZE 12
  extern uint8_t frskyTxBuffer[FRSKY_TX_PACKET_SIZE];
  extern uint8_t frskyTxBufferCount;
  inline void frskySendAlarms(void)
  {
    frskyAlarmsSendState = SEND_RSSI_ALARMS;
  }
#endif

// FrSky D Protocol
void frskyDSendNextAlarm(void);
void frskyDProcessPacket(uint8_t *packet);

// FrSky S.PORT Protocol
void frskySportProcessPacket(uint8_t *packet);

void telemetryWakeup(void);
void telemetryReset();
void telemetryInit(void);
void telemetryInterrupt10ms(void);

#if defined(CPUARM)
  typedef uint16_t frskyCellVoltage_t;
#elif defined(FRSKY_HUB)
  typedef uint8_t frskyCellVoltage_t;
#endif

#if defined(CPUARM) || defined(FRSKY_HUB)
void frskySetCellsCount(uint8_t cellscount);
void frskySetCellVoltage(uint8_t battnumber, frskyCellVoltage_t cellVolts);
void frskyUpdateCells(void);
#endif

#if defined(PCBTARANIS)
  #define MODEL_TELEMETRY_PROTOCOL() ((g_model.moduleData[INTERNAL_MODULE].rfProtocol == RF_PROTO_OFF && g_model.externalModule == MODULE_TYPE_PPM) ? g_model.telemetryProtocol : PROTOCOL_FRSKY_SPORT)
#elif defined(CPUARM)
  #define MODEL_TELEMETRY_PROTOCOL() g_model.telemetryProtocol
#endif


// FrSky SPORT constants

// FrSky PRIM IDs (1 byte)
#define DATA_FRAME              0x10

// FrSky old DATA IDs (1 byte)
#define GPS_ALT_BP_ID           0x01
#define TEMP1_ID                0x02
#define RPM_ID                  0x03
#define FUEL_ID                 0x04
#define TEMP2_ID                0x05
#define VOLTS_ID                0x06
#define GPS_ALT_AP_ID           0x09
#define BARO_ALT_BP_ID          0x10
#define GPS_SPEED_BP_ID         0x11
#define GPS_LONG_BP_ID          0x12
#define GPS_LAT_BP_ID           0x13
#define GPS_COURS_BP_ID         0x14
#define GPS_DAY_MONTH_ID        0x15
#define GPS_YEAR_ID             0x16
#define GPS_HOUR_MIN_ID         0x17
#define GPS_SEC_ID              0x18
#define GPS_SPEED_AP_ID         0x19
#define GPS_LONG_AP_ID          0x1A
#define GPS_LAT_AP_ID           0x1B
#define GPS_COURS_AP_ID         0x1C
#define BARO_ALT_AP_ID          0x21
#define GPS_LONG_EW_ID          0x22
#define GPS_LAT_NS_ID           0x23
#define ACCEL_X_ID              0x24
#define ACCEL_Y_ID              0x25
#define ACCEL_Z_ID              0x26
#define CURRENT_ID              0x28
#define VARIO_ID                0x30
#define VFAS_ID                 0x39
#define VOLTS_BP_ID             0x3A
#define VOLTS_AP_ID             0x3B
#define FRSKY_LAST_ID           0x3F

// FrSky new DATA IDs (2 bytes)
#define ALT_FIRST_ID            0x0100
#define ALT_LAST_ID             0x010f
#define VARIO_FIRST_ID          0x0110
#define VARIO_LAST_ID           0x011f
#define CURR_FIRST_ID           0x0200
#define CURR_LAST_ID            0x020f
#define VFAS_FIRST_ID           0x0210
#define VFAS_LAST_ID            0x021f
#define CELLS_FIRST_ID          0x0300
#define CELLS_LAST_ID           0x030f
#define T1_FIRST_ID             0x0400
#define T1_LAST_ID              0x040f
#define T2_FIRST_ID             0x0410
#define T2_LAST_ID              0x041f
#define RPM_FIRST_ID            0x0500
#define RPM_LAST_ID             0x050f
#define FUEL_FIRST_ID           0x0600
#define FUEL_LAST_ID            0x060f
#define ACCX_FIRST_ID           0x0700
#define ACCX_LAST_ID            0x070f
#define ACCY_FIRST_ID           0x0710
#define ACCY_LAST_ID            0x071f
#define ACCZ_FIRST_ID           0x0720
#define ACCZ_LAST_ID            0x072f
#define GPS_LONG_LATI_FIRST_ID  0x0800
#define GPS_LONG_LATI_LAST_ID   0x080f
#define GPS_ALT_FIRST_ID        0x0820
#define GPS_ALT_LAST_ID         0x082f
#define GPS_SPEED_FIRST_ID      0x0830
#define GPS_SPEED_LAST_ID       0x083f
#define GPS_COURS_FIRST_ID      0x0840
#define GPS_COURS_LAST_ID       0x084f
#define GPS_TIME_DATE_FIRST_ID  0x0850
#define GPS_TIME_DATE_LAST_ID   0x085f
#define A3_FIRST_ID             0x0900
#define A3_LAST_ID              0x090f
#define A4_FIRST_ID             0x0910
#define A4_LAST_ID              0x091f
#define AIR_SPEED_FIRST_ID      0x0a00
#define AIR_SPEED_LAST_ID       0x0a0f
#define RSSI_ID                 0xf101
#define ADC1_ID                 0xf102
#define ADC2_ID                 0xf103
#define BATT_ID                 0xf104
#define SWR_ID                  0xf105
#define XJT_VERSION_ID          0xf106

// Default sensor data IDs (Physical IDs + CRC)
#define DATA_ID_VARIO            0x00 // 0
#define DATA_ID_FLVSS            0xA1 // 1
#define DATA_ID_FAS              0x22 // 2
#define DATA_ID_GPS              0x83 // 3
#define DATA_ID_RPM              0xE4 // 4
#define DATA_ID_SP2UH            0x45 // 5
#define DATA_ID_SP2UR            0xC6 // 6

#endif
