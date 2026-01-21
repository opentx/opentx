/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"
#include "spektrum.h"

/*
 * Documentation of the Spektrum protocol is available under
 * https://www.spektrumrc.com/ProdInfo/Files/SPM_Telemetry_Developers_Specs.pdf
 * https://github.com/SpektrumRC/SpektrumDocumentation/blob/master/Telemetry/spektrumTelemetrySensors.h
 *
 * Multi module adds two byte header of 0xAA [RSSI of telemetry packet] [16 byte message]
 */

/*
 * The 16 byte message is formated as follows
 * [ic2 address] [secondary id] [14 byte ic2 address (type) specific data]
 * we translate the secondary to the instance of the sensor
 *
 * OpenTX Mapping
 *
 *  instance = secondary id  (almost always 0)
 *  subid = 0 (always 0)
 *  id = i2c address <<8 | start byte
 */

#define I2C_PSEUDO_TX 0xf0

#define SPEKTRUM_TELEMETRY_LENGTH 18
#define DSM_BIND_PACKET_LENGTH 12

#define I2C_VOLT_INTERNAL 0x01
#define I2C_TEMP_INTERNAL 0x02
#define I2C_HIGH_CURRENT 0x03
#define I2C_FWD_PGM 0x09
#define I2C_POWERBOX 0X0a
#define I2C_LAPTIMER 0X0b
#define I2C_TEXTGEN 0x0c
#define IC2_AIRSPEED 0x11
#define IC2_ALTITUDE 0x12
#define I2C_GFORCE 0x14
#define I2C_JETCAT 0x15
#define I2C_GPS_LOC 0x16
#define I2C_GPS_STAT 0x17
#define I2C_DUALENERGY 0x18
#define I2C_JETCAT2 0x19
#define I2C_GYRO 0x1a
#define I2C_ATTMAG 0x1b
#define I2C_ESC 0x20
#define IC2_BATTERY 0x34
#define I2C_CELLS 0x3a
#define I2C_VARIO 0x40
#define I2C_RTC 0x7c
#define I2C_RPMVOLTTEMP 0x7e

// SMART_BAT is using fake I2C adresses compared to official Spektrum address because of subtype used only for this I2C address
#define I2C_SMART_BAT_BASE_ADDRESS    0x42
#define I2C_SMART_BAT_REALTIME        0x42
#define I2C_SMART_BAT_CELLS_1_6       0x43
#define I2C_SMART_BAT_CELLS_7_12      0x44
#define I2C_SMART_BAT_CELLS_13_18     0x45
#define I2C_SMART_BAT_ID              0x4A
#define I2C_SMART_BAT_LIMITS          0x4B

#define I2C_QOS 0x7f

enum SpektrumDataType : uint8_t {
  int8,
  int16,
  int32,
  uint8,
  uint16,
  uint32,
  uint8bcd,
  uint16bcd,
  uint32bcd,
  uint16le,
  uint32le,
  custom
};

struct SpektrumSensor {
  const uint8_t i2caddress;
  const uint8_t startByte;
  const SpektrumDataType dataType;
  const char *name;
  const TelemetryUnit unit;
  const uint8_t precision;
};

const SpektrumSensor spektrumSensors[] = {
  // High voltage internal sensor
  {I2C_VOLT_INTERNAL,        0,  int16,     ZSTR_A1,                UNIT_VOLTS,                  1},

  // Temperature internal sensor
  {I2C_TEMP_INTERNAL,        0,  int16,     ZSTR_TEMP1,             UNIT_CELSIUS,                1},

  // High current internal sensor (0x03), 300A/2048 resolution
  {I2C_HIGH_CURRENT, 0,  int16,     ZSTR_CURR,              UNIT_AMPS,                   1},

  // Powerbox (also mentioned as 0x7D but that is also transmitter frame data)
  {I2C_POWERBOX,             0,  uint16,    ZSTR_BATT1_VOLTAGE,     UNIT_VOLTS,                  2},
  {I2C_POWERBOX,             2,  uint16,    ZSTR_BATT2_VOLTAGE,     UNIT_VOLTS,                  2},
  {I2C_POWERBOX,             4,  uint16,    ZSTR_BATT1_CONSUMPTION, UNIT_MAH,                    0},
  {I2C_POWERBOX,             6,  uint16,    ZSTR_BATT2_CONSUMPTION, UNIT_MAH,                    0},


  // Lap Timer
  {I2C_LAPTIMER,             0, uint8,      ZSTR_LAP_NUMBER,        UNIT_RAW,                    0},
  {I2C_LAPTIMER,             0, uint8,      ZSTR_GATE_NUMBER,       UNIT_RAW,                    0},
  {I2C_LAPTIMER,             0, uint32,     ZSTR_LAP_TIME,          UNIT_SECONDS,                3},
  {I2C_LAPTIMER,             0, uint32,     ZSTR_GATE_TIME,         UNIT_SECONDS,                3},

  // Text Generator
  {I2C_TEXTGEN,      0, uint32,     ZSTR_FLIGHT_MODE,       UNIT_TEXT,                   0},

  // AirSpeed, also has max (+2, int16)
  {IC2_AIRSPEED,             0,  int16,     ZSTR_ASPD,              UNIT_KMH,                    0},

  // Altitude, also has max (+2, int16)
  {IC2_ALTITUDE,             0,  int16,     ZSTR_ALT,               UNIT_METERS,                 1},


  // {0x38, strain}

  // G-Force (+min, max)
  {I2C_GFORCE,             0,  int16,     ZSTR_ACCX,              UNIT_G,                      2},
  {I2C_GFORCE,             2,  int16,     ZSTR_ACCY,              UNIT_G,                      2},
  {I2C_GFORCE,             4,  int16,     ZSTR_ACCZ,              UNIT_G,                      2},


  // JETCAT/TURBINE, BCD Encoded values
  // TODO: Add decoding of status information
  // {I2C_JETCAT,             0,  uint8,     ZSTR_STATUS,            UNIT_BITFIELD,               0},
  {I2C_JETCAT,             1,  uint8bcd,  ZSTR_THROTTLE,          UNIT_PERCENT,                0},
  {I2C_JETCAT,             2,  uint16bcd, ZSTR_A1,                UNIT_VOLTS,                  2},
  {I2C_JETCAT,             4,  uint16bcd, ZSTR_A2,                UNIT_VOLTS,                  2},
  {I2C_JETCAT,             6,  uint32bcd, ZSTR_RPM,               UNIT_RPMS,                   0},
  {I2C_JETCAT,             10, uint16bcd, ZSTR_TEMP1,             UNIT_CELSIUS,                0},
  // {I2C_JETCAT,             0,  uint8,     ZSTR_STATUS,            UNIT_BITFIELD,               0},

  // GPS
  // GPS is bcd encoded and also uses flags. Hard to get right without an actual GPS Sensor
  // Time/date is also BCD encoded but so this FrSky's, so treat it as uint32
  {I2C_GPS_LOC,      0,  uint16bcd, ZSTR_GPSALT,            UNIT_METERS,                 1}, // altitude low bits bcd 3.1
  {I2C_GPS_LOC,      2,  uint32le,  ZSTR_GPS,               UNIT_GPS,                    0}, // lat bcd DDMM.MMMM
  {I2C_GPS_LOC,      6,  uint32le,  ZSTR_GPS,               UNIT_GPS,                    0}, // lon bcd DDMM.MMMM
  {I2C_GPS_LOC,      10, uint16bcd, ZSTR_HDG,               UNIT_DEGREE,                 1}, // bcd 3.1
  {I2C_GPS_LOC,      13, uint8,     ZSTR_GPS,               UNIT_BITFIELD,               0}, // gps flags
  {I2C_GPS_STAT,     0,  uint16bcd, ZSTR_GSPD,              UNIT_KTS,                    1}, // bcd 3.1
  {I2C_GPS_STAT,     2,  uint32le,  ZSTR_GPSDATETIME,       UNIT_DATETIME,               0}, // bcd 0HH:MM:SS.S
  {I2C_GPS_STAT,     6,  uint8bcd,  ZSTR_SATELLITES,        UNIT_RAW,                    0}, // bcd 2.0
  {I2C_GPS_STAT,     7,  uint8bcd,  ZSTR_GPS,               UNIT_BITFIELD,               0}, // altitude high bits bcd 2.0

  // Dual energy
  {I2C_DUALENERGY,   0,  int16,     ZSTR_BATT1_CURRENT,     UNIT_AMPS,                   2},
  {I2C_DUALENERGY,   2,  int16,     ZSTR_BATT1_CONSUMPTION, UNIT_MAH,                    1},
  {I2C_DUALENERGY,   4,  uint16,    ZSTR_BATT1_VOLTAGE,     UNIT_VOLTS,                  2},
  {I2C_DUALENERGY,   6,  int16,     ZSTR_BATT2_CURRENT,     UNIT_AMPS,                   1},
  {I2C_DUALENERGY,   8,  int16,     ZSTR_BATT2_CONSUMPTION, UNIT_MAH,                    0},
  {I2C_DUALENERGY,   10, uint16,    ZSTR_BATT2_VOLTAGE,     UNIT_VOLTS,                  2},
  
  // Jetcat flow rate
  // {I2C_JETCAT2,             0,  uint16bcd, ZSTR_FUEL_CONSUMPTION,  UNIT_MILLILITERS_PER_MINUTE, 1}, missing ml/min
  {I2C_JETCAT2,      2,  uint32bcd, ZSTR_FUEL,              UNIT_MILLILITERS,            1},

  // Gyro
  {I2C_GYRO,         0,  int16,     ZSTR_GYROX,             UNIT_DEGREE,                 1},
  {I2C_GYRO,         2,  int16,     ZSTR_GYROY,             UNIT_DEGREE,                 1},
  {I2C_GYRO,         4,  int16,     ZSTR_GYROZ,             UNIT_DEGREE,                 1},

  // Attitude & Mag Compass
  // mag Units are tbd so probably no sensor in existance, ignore them for now
  {I2C_ATTMAG,       0,  int16,     ZSTR_ROLL,              UNIT_DEGREE,                 1},
  {I2C_ATTMAG,       2,  int16,     ZSTR_PITCH,             UNIT_DEGREE,                 1},
  {I2C_ATTMAG,       4,  int16,     ZSTR_YAW,               UNIT_DEGREE,                 1},

  // Smart ESC telemetry
  {I2C_ESC,          0,  uint16,    ZSTR_ESC_RPM,           UNIT_RPMS,                   0},
  {I2C_ESC,          2,  uint16,    ZSTR_ESC_VIN,           UNIT_VOLTS,                  2},
  {I2C_ESC,          4,  uint16,    ZSTR_ESC_TFET,          UNIT_CELSIUS,                1},
  {I2C_ESC,          6,  uint16,    ZSTR_ESC_CUR,           UNIT_AMPS,                   2},
  {I2C_ESC,          8,  uint16,    ZSTR_ESC_TBEC,          UNIT_CELSIUS,                1},
  {I2C_ESC,          10, uint8,     ZSTR_ESC_BCUR,          UNIT_AMPS,                   1},
  {I2C_ESC,          11, uint8,     ZSTR_ESC_VBEC,          UNIT_VOLTS,                  1},
  {I2C_ESC,          12, uint8,     ZSTR_ESC_THR,           UNIT_PERCENT,                1},
  {I2C_ESC,          13, uint8,     ZSTR_ESC_POUT,          UNIT_PERCENT,                1},

  // Dual Batteries
  {IC2_BATTERY,      0,  int16,     ZSTR_BATT1_CURRENT,     UNIT_AMPS,                   1},
  {IC2_BATTERY,      2,  int16,     ZSTR_BATT1_CONSUMPTION, UNIT_MAH,                    0},
  {IC2_BATTERY,      4,  uint16,    ZSTR_BATT1_TEMP,        UNIT_CELSIUS,                1},
  {IC2_BATTERY,      6,  int16,     ZSTR_BATT2_CURRENT,     UNIT_AMPS,                   1},
  {IC2_BATTERY,      8,  int16,     ZSTR_BATT2_CONSUMPTION, UNIT_MAH,                    0},
  {IC2_BATTERY,      10, uint16,    ZSTR_BATT2_TEMP,        UNIT_CELSIUS,                1},

  // Tank pressure + custom input bits (ignore for now)
  //{0x38,             0,  uint16,    ZSTR_STATUS_BITS,       UNIT_BITFIELD,               0},
  //{0x38,             0,  uint16,    ZSTR_PRESSSURE,         UNIT_PSI,                    1},

  // Cells
  {I2C_CELLS,        0,  uint16,    ZSTR_CELLS,             UNIT_VOLTS,                  2},
  {I2C_CELLS,        2,  uint16,    ZSTR_CELLS,             UNIT_VOLTS,                  2},
  {I2C_CELLS,        4,  uint16,    ZSTR_CELLS,             UNIT_VOLTS,                  2},
  {I2C_CELLS,        6,  uint16,    ZSTR_CELLS,             UNIT_VOLTS,                  2},
  {I2C_CELLS,        8,  uint16,    ZSTR_CELLS,             UNIT_VOLTS,                  2},
  {I2C_CELLS,        10, uint16,    ZSTR_CELLS,             UNIT_VOLTS,                  2},
  {I2C_CELLS,        12, uint16,    ZSTR_TEMP2,             UNIT_CELSIUS,                2},

  // Vario-S
  {I2C_VARIO,        0,  int16,     ZSTR_ALT,               UNIT_METERS,                 1},
  {I2C_VARIO,        2,  int16,     ZSTR_VSPD,              UNIT_METERS_PER_SECOND,      1},
  {I2C_VARIO,        4,  int16,     ZSTR_VSPD,              UNIT_METERS_PER_SECOND,      1},
  {I2C_VARIO,        6,  int16,     ZSTR_VSPD,              UNIT_METERS_PER_SECOND,      0},
  {I2C_VARIO,        8,  int16,     ZSTR_VSPD,              UNIT_METERS_PER_SECOND,      0},
  {I2C_VARIO,        10, int16,     ZSTR_VSPD,              UNIT_METERS_PER_SECOND,      0},
  {I2C_VARIO,        12, int16,     ZSTR_VSPD,              UNIT_METERS_PER_SECOND,      0},

  // Smartbat
  //{I2C_SMART_BAT_REALTIME,        1,  int8,      ZSTR_SMART_BAT_BTMP,    UNIT_CELSIUS,             0},  // disabled because sensor is a duplicate of cells sensors ones
  {I2C_SMART_BAT_REALTIME,       2,  uint32le,  ZSTR_SMART_BAT_BCUR,    UNIT_MAH,                 0},
  {I2C_SMART_BAT_REALTIME,       6,  uint16le,  ZSTR_SMART_BAT_BCAP,    UNIT_MAH,                 0},
  {I2C_SMART_BAT_REALTIME,       8,  uint16le,  ZSTR_SMART_BAT_MIN_CEL, UNIT_VOLTS,               2},
  {I2C_SMART_BAT_REALTIME,       10, uint16le,  ZSTR_SMART_BAT_MAX_CEL, UNIT_VOLTS,               2},
  //{I2C_SMART_BAT_REALTIME,          12,  uint16le,  "RFU[2]", UNIT_RAW,                 0},   // disabled to save sensors slots

  {I2C_SMART_BAT_CELLS_1_6,       1,  int8,      ZSTR_SMART_BAT_BTMP,   UNIT_CELSIUS,             0},
  {I2C_SMART_BAT_CELLS_1_6,       2,  uint16le,  ZSTR_CL01,             UNIT_VOLTS,               2},
  {I2C_SMART_BAT_CELLS_1_6,       4,  uint16le,  ZSTR_CL02,             UNIT_VOLTS,               2},
  {I2C_SMART_BAT_CELLS_1_6,       6,  uint16le,  ZSTR_CL03,             UNIT_VOLTS,               2},
  {I2C_SMART_BAT_CELLS_1_6,       8,  uint16le,  ZSTR_CL04,             UNIT_VOLTS,               2},
  {I2C_SMART_BAT_CELLS_1_6,       10,  uint16le,  ZSTR_CL05,            UNIT_VOLTS,               2},
  {I2C_SMART_BAT_CELLS_1_6,       12, uint16le,  ZSTR_CL06,             UNIT_VOLTS,               2},

  {I2C_SMART_BAT_CELLS_7_12,      1,  int8,      ZSTR_SMART_BAT_BTMP,   UNIT_CELSIUS,             0},
  {I2C_SMART_BAT_CELLS_7_12,      2,  uint16le,  ZSTR_CL07,             UNIT_VOLTS,               2},
  {I2C_SMART_BAT_CELLS_7_12,      4,  uint16le,  ZSTR_CL08,             UNIT_VOLTS,               2},
  {I2C_SMART_BAT_CELLS_7_12,      6,  uint16le,  ZSTR_CL09,             UNIT_VOLTS,               2},
  {I2C_SMART_BAT_CELLS_7_12,      8,  uint16le,  ZSTR_CL10,             UNIT_VOLTS,               2},
  {I2C_SMART_BAT_CELLS_7_12,      10,  uint16le,  ZSTR_CL11,            UNIT_VOLTS,               2},
  {I2C_SMART_BAT_CELLS_7_12,      12, uint16le,  ZSTR_CL12,             UNIT_VOLTS,               2},

  {I2C_SMART_BAT_CELLS_13_18,     1,  int8,      ZSTR_SMART_BAT_BTMP,   UNIT_CELSIUS,             0},
  {I2C_SMART_BAT_CELLS_13_18,     2,  uint16le,  ZSTR_CL13,             UNIT_VOLTS,               2},
  {I2C_SMART_BAT_CELLS_13_18,     4,  uint16le,  ZSTR_CL14,             UNIT_VOLTS,               2},
  {I2C_SMART_BAT_CELLS_13_18,     6,  uint16le,  ZSTR_CL15,             UNIT_VOLTS,               2},
  {I2C_SMART_BAT_CELLS_13_18,     8,  uint16le,  ZSTR_CL16,             UNIT_VOLTS,               2},
  {I2C_SMART_BAT_CELLS_13_18,     10, uint16le,  ZSTR_CL17,             UNIT_VOLTS,               2},
  {I2C_SMART_BAT_CELLS_13_18,     12, uint16le,  ZSTR_CL18,             UNIT_VOLTS,               2},

  //{I2C_SMART_BAT_ID,              1,  uint8,  "chemistery",  UNIT_RAW, 0},   // disabled to save sensors slots
  //{I2C_SMART_BAT_ID,              2,  uint8,  "number of cells",  UNIT_RAW, 0},   // disabled to save sensors slots
  //{I2C_SMART_BAT_ID,              3,  uint8,  "manufacturer code",  UNIT_RAW, 0},   // disabled to save sensors slots
  {I2C_SMART_BAT_ID,              4,  uint16le,  ZSTR_SMART_BAT_CYCLES,  UNIT_RAW,                 0},
  //{I2C_SMART_BAT_ID,              6,  uint8,  "uniqueID[8]",  UNIT_RAW, 0},   // disabled to save sensors slots

  //{I2C_SMART_BAT_LIMITS,          1,  uint8,  "rfu",  UNIT_RAW, 0},   // disabled to save sensors slots
  {I2C_SMART_BAT_LIMITS,          2,  uint16le,  ZSTR_SMART_BAT_CAPACITY,UNIT_MAH,                 0},
  //{I2C_SMART_BAT_LIMITS,          4,  uint16le,  "dischargeCurrentRating",  UNIT_RAW, 0},   // disabled to save sensors slots
  //{I2C_SMART_BAT_LIMITS,          6,  uint16le,  "overDischarge_mV",  UNIT_RAW, 0},   // disabled to save sensors slots
  //{I2C_SMART_BAT_LIMITS,          8,  uint16le,  "zeroCapacity_mV",  UNIT_RAW, 0},   // disabled to save sensors slots
  //{I2C_SMART_BAT_LIMITS,          10,  uint16le,  "fullyCharged_mV",  UNIT_RAW, 0},   // disabled to save sensors slots
  //{I2C_SMART_BAT_LIMITS,          12,  uint8,  "minWorkingTemp",  UNIT_RAW, 0},   // disabled to save sensors slots
  //{I2C_SMART_BAT_LIMITS,          13,  uint8,  "maxWorkingTemp",  UNIT_RAW, 0},   // disabled to save sensors slots

  // 0x50-0x56 custom 3rd party sensors
  //{0x50, 0, int16, ZSTR_}

  // 0x7d are transmitter channels frame data [7], probably only available on the Spektrum
  // telemetry bus on the model itself

  // RPM/Volts/Temperature
  {I2C_RPMVOLTTEMP,             0,  uint16,    ZSTR_RPM,               UNIT_RPMS,                   0},
  {I2C_RPMVOLTTEMP,             2,  uint16,    ZSTR_A3,                UNIT_VOLTS,                  2},
  {I2C_RPMVOLTTEMP,             4,  int16,     ZSTR_TEMP2,             UNIT_FAHRENHEIT,             0},

  // 0x7f, QoS DATA, also called Flight Log,, with A, B, L, R, F, H?
  // A - Antenna Fades on Receiver A
  // B - Antenna Fades on Receiver B
  // L - Antenna Fades on left Receiver
  // R - Antenna Fades on right Receiver
  // F - Frame losses
  {I2C_QOS,          0,  uint16,    ZSTR_QOS_A,             UNIT_RAW,                    0},
  {I2C_QOS,          2,  uint16,    ZSTR_QOS_B,             UNIT_RAW,                    0},
  {I2C_QOS,          4,  uint16,    ZSTR_QOS_L,             UNIT_RAW,                    0},
  {I2C_QOS,          6,  uint16,    ZSTR_QOS_R,             UNIT_RAW,                    0},
  {I2C_QOS,          8,  uint16,    ZSTR_QOS_F,             UNIT_RAW,                    0},
  {I2C_QOS,          10, uint16,    ZSTR_QOS_H,             UNIT_RAW,                    0},
  {I2C_QOS,          12, uint16,    ZSTR_A2,                UNIT_VOLTS,                  2},

  {I2C_PSEUDO_TX,    0,  uint8,     ZSTR_TX_RSSI,           UNIT_RAW,                    0},
  {I2C_PSEUDO_TX,    4,  uint32,    ZSTR_BIND,              UNIT_RAW,                    0},
  {0,                0,  int16,     NULL,                   UNIT_RAW,                    0} //sentinel
};

static int32_t bcdToInt8(uint8_t bcd)
{
  return 10 * ((bcd & 0xf0) >> 4) + (bcd & 0xf);
}

static int32_t bcdToInt16(uint16_t bcd)
{
  return 100 * bcdToInt8(bcd >> 8) + bcdToInt8(bcd);
}

static int32_t bcdToInt32(uint32_t bcd)
{
  return 10000 * bcdToInt16(bcd >> 16) + bcdToInt16(bcd);
}

// Spektrum uses Big Endian data types
static int32_t spektrumGetValue(const uint8_t *packet, int startByte, SpektrumDataType type)
{
  const uint8_t * data = packet + startByte;
  switch (type) {
    case uint8:
      return *((uint8_t *) (data));
    case int8:
      return *((int8_t *) (data));
    case int16:
      return (int16_t) ((uint16_t) (data[1] + (data[0] << 8)));
    case uint16:
      return ((uint16_t) (data[1] + (data[0] << 8)));
    case int32:
      return ((int32_t) (data[3] + (data[2] << 8) + (data[1] << 16) + (data[0] << 24)));
    case uint32:
      return ((uint32_t) (data[3] + (data[2] << 8) + (data[1] << 16) + (data[0] << 24)));
    case uint16bcd:
      return bcdToInt16(*(uint16_t *)data);
    case uint8bcd:
      return bcdToInt8(*(uint8_t *)data);
    case uint32bcd:
      return bcdToInt32(*(uint32_t *)data);
    case uint16le:
      return (int16_t) ((uint16_t) (data[0] + (data[1] << 8)));
    case uint32le:
      return ((uint32_t) (data[0] + (data[1] << 8) + (data[2] << 16) + (data[3] << 24)));
    default:
      return -1;
  }
}

bool isSpektrumValidValue(int32_t value, const SpektrumDataType type)
{
  switch (type) {
    case uint16:
      return value != 0xffff;
    case int16:
      return value != 0x7fff;
    case int32:
      return value != 0x7fffffff;
    case uint32:
      return ((uint32_t) value) != 0xffffffff;
    default:
      return true;
  }
}

void processSpektrumPacket(const uint8_t *packet)
{
  setTelemetryValue(PROTOCOL_TELEMETRY_SPEKTRUM, (I2C_PSEUDO_TX << 8) + 0, 0, 0, packet[1], UNIT_RAW, 0);
  // highest bit indicates that TM1100 is in use, ignore it
  uint8_t i2cAddress = (packet[2] & 0x7f);

  if (i2cAddress == I2C_FWD_PGM) {
#if defined(LUA)
    // Forward Programming
    if (Multi_Buffer && memcmp(Multi_Buffer, "DSM", 3) == 0) {
      // Multi_Buffer[0..2]=="DSM" -> Lua script is running
      // Multi_Buffer[3]==0x70 -> TX to RX data ready to be sent
      // Multi_Buffer[4..9]=6 bytes of TX to RX data
      // Multi_Buffer[10..25]=16 bytes of RX to TX data
      Multi_Buffer[10] = i2cAddress;
      memcpy(&Multi_Buffer[11], &packet[3], 15); // Store the received RX answer in the buffer
    }
#endif
    return; // Not a sensor
  }
  //SmartBat Hack
  if (i2cAddress == I2C_SMART_BAT_BASE_ADDRESS) {
    i2cAddress = i2cAddress + (packet[4] >> 4); // use type to create virtual I2CAddresses
  }

  uint8_t instance = packet[3];

  if (i2cAddress == I2C_TEXTGEN) {
    uint8_t lineNumber = packet[4];

    uint16_t pseudoId = (i2cAddress << 8 | lineNumber);

    for (int i=5; i<SPEKTRUM_TELEMETRY_LENGTH; i++)
    {
      setTelemetryValue(PROTOCOL_TELEMETRY_SPEKTRUM, pseudoId, 0, instance, packet[i], UNIT_TEXT, i-5);
    }
    // Set a sential \0 just for safety since we have the space there
    setTelemetryValue(PROTOCOL_TELEMETRY_SPEKTRUM, pseudoId, 0, instance, '\0', UNIT_TEXT, 13);


    return;
  }

  bool handled = false;
  for (const SpektrumSensor * sensor = spektrumSensors; sensor->i2caddress; sensor++) {
    if (i2cAddress == sensor->i2caddress) {
      handled = true;

      // Extract value, skip header
      int32_t value = spektrumGetValue(packet + 4, sensor->startByte, sensor->dataType);

      if (!isSpektrumValidValue(value, sensor->dataType))
        continue;

      // mV to VOLT PREC2 for Smart Batteries
      if ((i2cAddress >= I2C_SMART_BAT_REALTIME  && i2cAddress <= I2C_SMART_BAT_LIMITS) && sensor->unit == UNIT_VOLTS) {
        if (value == -1) {
          continue;  // discard unavailable sensors
        }
        else {
          value = value / 10;
        }
      }

      if (i2cAddress == I2C_ESC) {
        // RPM, 10RPM (0-655340 RPM)
        if (sensor->startByte == 0)
          value = value * 10;
        // BEC Voltage 50mV (0-12.70A). Throttle 0.5% (0-127%). Power 0.5% (0-127%)
        else if (sensor->startByte == 11 || sensor->startByte == 12 || sensor->startByte == 13)
          value = value / 2;
      }

      if (i2cAddress == I2C_CELLS && sensor->unit == UNIT_VOLTS) {
        // Map to FrSky style cell values
        int cellIndex = (sensor->startByte / 2) << 16;
        value = value | cellIndex;
      }

      if (sensor->i2caddress == I2C_HIGH_CURRENT && sensor->unit == UNIT_AMPS)
        // Spektrum's documents talks says: Resolution: 300A/2048 = 0.196791 A/tick
        // Note that 300/2048 = 0,1464. DeviationTX also uses the 0.196791 figure
        value = value * 196791 / 100000;

      // Check if this looks like a LemonRX Transceiver, they use QoS Frame loss A as RSSI indicator(0-100)
      if (i2cAddress == I2C_QOS && sensor->startByte == 0) {
        if (spektrumGetValue(packet + 4, 2, uint16) == 0x8000 &&
            spektrumGetValue(packet + 4, 4, uint16) == 0x8000 &&
            spektrumGetValue(packet + 4, 6, uint16) == 0x8000 &&
            spektrumGetValue(packet + 4, 8, uint16) == 0x8000) {
          telemetryData.rssi.set(value);
        }
        else {
          // Otherwise use the received signal strength of the telemetry packet as indicator
          // Range is 0-31, multiply by 3 to get an almost full reading for 0x1f, the maximum the cyrf chip reports
          telemetryData.rssi.set(packet[1] * 3);
        }
        telemetryStreaming = TELEMETRY_TIMEOUT10ms;
      }

      // GPS loc
      static bool is_north, is_east, is_lon_bigger_than_100, is_alt_neg;
      static uint32_t altitude_high;
      static int32_t latitude, longitude;
      if (i2cAddress == I2C_GPS_LOC) {
        if (sensor->startByte == 0) {
          value += altitude_high;
          if (is_alt_neg)
              value *= -1;
        }
        else if (sensor->startByte == 2) {
          uint32_t deg, min;
          deg = bcdToInt8(value >> 24);
          min = bcdToInt32(value & 0x00ffffff);
          latitude = deg * 600000L + min;
          if (!is_north)
            latitude *= -1;
          latitude = (latitude * 5) / 3; // min/10000 => deg/1000000
          setTelemetryValue(PROTOCOL_TELEMETRY_SPEKTRUM, 0x1602, 0, 0, latitude, UNIT_GPS_LATITUDE, 0);
          continue;
        }
        else if (sensor->startByte == 6) {
          uint32_t deg, min;
          deg = bcdToInt8(value >> 24);
          min = bcdToInt32(value & 0x00ffffff);
          if (is_lon_bigger_than_100)
            deg += 100;
          longitude = deg * 600000L + min;
          if (!is_east)
            longitude *= -1;
          longitude = (longitude * 5) / 3; // min/10000 => deg/1000000
          setTelemetryValue(PROTOCOL_TELEMETRY_SPEKTRUM, 0x1602, 0, 0, longitude, UNIT_GPS_LONGITUDE, 0);
          continue;
        }
        else if (sensor->startByte == 13) {
          is_north = value & 0x01;
          is_east = value & 0x02;
          is_lon_bigger_than_100 = value & 0x04;
          is_alt_neg = value & 0x80;
          continue;
        }
      }

      //GPS stat
      if (i2cAddress == I2C_GPS_STAT) {
        if (sensor->startByte == 2) {
          // Frsky time is HH:MM:SS:00 while spektrum is bcd 0HH:MM:SS.S
          uint8_t hour = bcdToInt8(value >> 20);
          uint8_t minute = bcdToInt8(value >> 12);
          uint8_t second = bcdToInt8(value  >> 4);
          value = ((uint32_t)hour << 24) | ((uint32_t)minute << 16) | ((uint32_t)second << 8);
        }
        else if (sensor->startByte == 7) {
          altitude_high = value * 10000;
          continue;
        }
      }

      // Vario
      if (sensor->i2caddress == I2C_VARIO) {
        if (sensor->startByte == 2)
          value = value * 4;
        else if (sensor->startByte == 4)
          value = value * 2;
        else if (sensor->startByte == 6)
          value = value;
        else if (sensor->startByte == 8)
          value = value * 2 / 3;
        else if (sensor->startByte == 10)
          value = value / 2;
        else if (sensor->startByte == 12)
          value = value / 3;
      }

      uint16_t pseudoId = (sensor->i2caddress << 8 | sensor->startByte);
      setTelemetryValue(PROTOCOL_TELEMETRY_SPEKTRUM, pseudoId, 0, instance, value, sensor->unit, sensor->precision);
    }
  }
  if (!handled) {
    // If we see a sensor that is not handled at all, add the raw values of this sensor to show its existance to
    // the user and help debugging/implementing these sensors
    for (int startByte=0; startByte<14; startByte+=2) {
      int32_t value = spektrumGetValue(packet + 4, startByte, uint16);
      uint16_t pseudoId = i2cAddress << 8 | startByte;
      setTelemetryValue(PROTOCOL_TELEMETRY_SPEKTRUM, pseudoId, 0, instance, value, UNIT_RAW, 0);
    }
  }
}

// Parse the DSM2 bind response, Fields are as per http://www.rcgroups.com/forums/showpost.php?p=35692146&postcount=5191
// "I"  here means the multi module

/*
0-3   4 bytes -> Cyrf ID of the TX xor 0xFF but don't care...
4     1 byte -> RX version but don't care...
5     1 byte -> number of channels, example 0x06=6 channels
6     1 byte -> max DSM type allowed:
        0x01 => 1024 DSM2 1 packet => number of channels is <8 and no telemetry
        0x02 => 1024 DSM2 2 packets => either a number of channel >7 or telemetry enable RX
        0x12 => 2048 DSM2 2 packets => can be any number of channels with/without telemetry -> this mode might be supported following Mike's trials, note the channels should be duplicated between the packets which is not the case today
        0xa2 => 2048 DSMX 1 packet => number of channels is <8 and no telemetry
        0xb2 => 2048 DSMX 2 packets => can be any number of channels with/without telemetry -> this mode is only half supported since the channels should be duplicated between the packets which is not the case but might be supported following Mike's trials
7     0x00: not sure of the use of this byte since I've always seen it at 0...
8-9   2 bytes CRC but don't care...

 Examples:           DSM   #Chan  RXver
 Inductrix           0xa2   07     1
 LemonRX+Sat+tele    0xb2   07     1

 */
void processDSMBindPacket(uint8_t module, const uint8_t *packet)
{
  uint32_t debugval;
  if (g_model.moduleData[module].type == MODULE_TYPE_MULTIMODULE && g_model.moduleData[module].getMultiProtocol() == MODULE_SUBTYPE_MULTI_DSM2 && g_model.moduleData[module].subType == MM_RF_DSM2_SUBTYPE_AUTO) {
    // Only sets channel etc when in DSM/AUTO mode
    int channels = packet[5];
    if (channels > 12) {
      channels = 12;
    }
    else if (channels < 3) {
      channels = 3;
    }

    switch(packet[6]) {
      case 0xa2:
        g_model.moduleData[module].subType = MM_RF_DSM2_SUBTYPE_DSMX_22;
        break;
      case 0x12:
        g_model.moduleData[module].subType = MM_RF_DSM2_SUBTYPE_DSM2_11;
        if (channels == 7) {
          channels = 12;    // change the number of channels if 7
        }
        break;
      case 0x01:
      case 0x02:
        g_model.moduleData[module].subType = MM_RF_DSM2_SUBTYPE_DSM2_22;
        break;
      default: // 0xb2 or unknown
        g_model.moduleData[module].subType = MM_RF_DSM2_SUBTYPE_DSMX_11;
        if (channels == 7) {
          channels = 12;    // change the number of channels if 7
        }
        break;
    }

    g_model.moduleData[module].channelsCount = channels - 8;
    g_model.moduleData[module].multi.optionValue &= 0xFD;    // clear the 11ms servo refresh rate flag

    storageDirty(EE_MODEL);
  }

  debugval = packet[7] << 24 | packet[6] << 16 | packet[5] << 8 | packet[4];

  /* log the bind packet as telemetry for quick debugging */
  setTelemetryValue(PROTOCOL_TELEMETRY_SPEKTRUM, (I2C_PSEUDO_TX << 8) + 4, 0, 0, debugval, UNIT_RAW, 0);

  /* Finally stop binding as the rx just told us that it is bound */
  if (g_model.moduleData[module].type == MODULE_TYPE_MULTIMODULE && g_model.moduleData[module].getMultiProtocol() == MODULE_SUBTYPE_MULTI_DSM2 && moduleState[module].mode == MODULE_MODE_BIND) {
    setMultiBindStatus(module, MULTI_BIND_FINISHED);
  }
}

void processSpektrumTelemetryData(uint8_t module, uint8_t data, uint8_t* rxBuffer, uint8_t& rxBufferCount)
{
  if (rxBufferCount == 0 && data != 0xAA) {
    TRACE("[SPK] invalid start byte 0x%02X", data);
    return;
  }

  if (rxBufferCount < TELEMETRY_RX_PACKET_SIZE) {
    rxBuffer[rxBufferCount++] = data;
  }
  else {
    TRACE("[SPK] array size %d error", rxBufferCount);
    rxBufferCount = 0;
  }

  if (rxBuffer[1] == 0x80 && rxBufferCount >= DSM_BIND_PACKET_LENGTH) {
    processDSMBindPacket(module, rxBuffer+2);
    rxBufferCount = 0;
    return;
  }

  if (rxBufferCount >= SPEKTRUM_TELEMETRY_LENGTH) {
    // Debug print content of Telemetry to console
#if 0
    debugPrintf("[SPK] Packet 0x%02X rssi 0x%02X: ic2 0x%02x, %02x: ",
                rxBuffer[0], rxBuffer[1], rxBuffer[2], rxBuffer[3]);
    for (int i=4; i<SPEKTRUM_TELEMETRY_LENGTH; i+=4) {
      debugPrintf("%02X%02X %02X%02X  ", rxBuffer[i], rxBuffer[i + 1],
                  rxBuffer[i + 2], rxBuffer[i + 3]);
    }
    debugPrintf(CRLF);
#endif
    processSpektrumPacket(rxBuffer);
    rxBufferCount = 0;
  }
}

const SpektrumSensor *getSpektrumSensor(uint16_t pseudoId)
{
  uint8_t startByte = (uint8_t) (pseudoId & 0xff);
  uint8_t i2cadd = (uint8_t) (pseudoId >> 8);
  for (const SpektrumSensor * sensor = spektrumSensors; sensor->i2caddress; sensor++) {
    if (i2cadd == sensor->i2caddress && startByte == sensor->startByte) {
      return sensor;
    }
  }
  return nullptr;
}

void spektrumSetDefault(int index, uint16_t id, uint8_t subId, uint8_t instance)
{
  TelemetrySensor &telemetrySensor = g_model.telemetrySensors[index];
  telemetrySensor.id = id;
  telemetrySensor.subId = subId;
  telemetrySensor.instance = instance;

  const SpektrumSensor *sensor = getSpektrumSensor(id);
  if (sensor) {
    TelemetryUnit unit = sensor->unit;
    uint8_t prec = min<uint8_t>(2, sensor->precision);
    telemetrySensor.init(sensor->name, unit, prec);

    if (unit == UNIT_RPMS) {
      telemetrySensor.custom.ratio = 1;
      telemetrySensor.custom.offset = 1;
    }
    else if (unit == UNIT_FAHRENHEIT) {
      if (!IS_IMPERIAL_ENABLE()) {
        telemetrySensor.unit = UNIT_CELSIUS;
      }

    }
    else if (unit == UNIT_METERS) {
      if (IS_IMPERIAL_ENABLE()) {
        telemetrySensor.unit = UNIT_FEET;
      }
    }
  }
  else {
    telemetrySensor.init(id);
  }

  storageDirty(EE_MODEL);
}
