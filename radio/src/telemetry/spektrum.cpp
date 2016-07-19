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
 * http://www.rcgroups.com/forums/showthread.php?t=1726960&page=22#post34091390
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



#define I2C_PSEUDO_TX 0x01
#define SPEKTRUM_TELEMETRY_LENGTH 18

#define I2C_CELLS 0x3a
#define I2C_QOS 0x7f


enum SpektrumDataType : uint8_t {
  int8,
  uint8,
  uint16,
  int16,
  uint32,
  int32,
  uint16bcd,
  uint8bcd,
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
  {0x01,      0,  int16,     ZSTR_A1,                UNIT_VOLTS,             1},

  // Temperature internal sensor
  {0x02,      0,  int16,     ZSTR_TEMP1,             UNIT_CELSIUS,           1},

  // High current internal sensor, 300A/2048 resolution
  {0x03,      0,  int16,     ZSTR_CURR,              UNIT_AMPS,              1},

  //Powerbox (also mentioned as 0x7D but that is also transmitter frame data)
  {0x0a,      0,  uint16,    ZSTR_BATT1_VOLTAGE,     UNIT_VOLTS,             2},
  {0x0a,      2,  uint16,    ZSTR_BATT2_VOLTAGE,     UNIT_VOLTS,             2},
  {0x0a,      4,  uint16,    ZSTR_BATT1_CONSUMPTION, UNIT_MAH,               0},
  {0x0a,      6,  uint16,    ZSTR_BATT2_CONSUMPTION, UNIT_MAH,               0},

  // AirSpeed, also has max (+2, int16)
  {0x11,      0,  int16,     ZSTR_ASPD,              UNIT_KMH,               0},

  // Altitude, also has max (+2, int16)
  {0x12,      0,  int16,     ZSTR_ALT,               UNIT_METERS,            1},

  // {0x20, esc}
  // {0x22, fuel/tank}

  // {0x34, battery current/capacity}
  // {0x38, strain}

  // G-Force (+min, max)
  {0x14,      0,  int16,     ZSTR_ACCX,              UNIT_G,                 2},
  {0x14,      2,  int16,     ZSTR_ACCY,              UNIT_G,                 2},
  {0x14,      4,  int16,     ZSTR_ACCZ,              UNIT_G,                 2},
  // 0x15,  JETCAT/TURBINE, BCD Encoded values

  // 0x16-0x17 GPS
  {0x17,      0,  uint16bcd, ZSTR_GSPD,              UNIT_KTS,               1},
  //{0x17, 2, uint32, ZSTR_GPSDATETIME, UNIT_DATETIME}, utc in bcd HH:MM:SS.S
  {0x17,      6,  uint8bcd,  ZSTR_SATELLITES,        UNIT_RAW,               0},
  //{0x17, 7, uint8bcd, ZSTR_GPSALT, UNIT_METERS}, altitude high bits
  // 0x19 Jetcat flow rate

  // 0x1a Gyro
  {0x1a,      0,  int16,     ZSTR_GYROX,             UNIT_DEGREE,            1},
  {0x1a,      2,  int16,     ZSTR_GYROY,             UNIT_DEGREE,            1},
  {0x1a,      4,  int16,     ZSTR_GYROZ,             UNIT_DEGREE,            1},

  // 0x1b Attitude & Mag Compass
  // mag Units are tbd so probably no sensor in existance, ignore them for now
  {0x1b,      0,  int16,     ZSTR_ROLL,              UNIT_DEGREE,            1},
  {0x1b,      2,  int16,     ZSTR_PITCH,             UNIT_DEGREE,            1},
  {0x1b,      4,  int16,     ZSTR_YAW,               UNIT_DEGREE,            1},

  // Cells (0x3a)
  {I2C_CELLS, 0,  uint16,    ZSTR_CELLS,             UNIT_VOLTS,             2},
  {I2C_CELLS, 2,  uint16,    ZSTR_CELLS,             UNIT_VOLTS,             2},
  {I2C_CELLS, 4,  uint16,    ZSTR_CELLS,             UNIT_VOLTS,             2},
  {I2C_CELLS, 6,  uint16,    ZSTR_CELLS,             UNIT_VOLTS,             2},
  {I2C_CELLS, 8,  uint16,    ZSTR_CELLS,             UNIT_VOLTS,             2},
  {I2C_CELLS, 10, uint16,    ZSTR_CELLS,             UNIT_VOLTS,             2},
  {I2C_CELLS, 12, uint16,    ZSTR_TEMP2,             UNIT_CELSIUS,           2},


  // Vario-S
  {0x40,      0,  int16,     ZSTR_ALT,               UNIT_METERS,            1},
  {0x40,      2,  int16,     ZSTR_VSPD,              UNIT_METERS_PER_SECOND, 1},

  // 0x50-0x56 custom 3rd party sensors
  //{0x50, 0, int16, ZSTR_}

  // RPM/Volts/Temperature
  {0x7e,      0,  uint16,    ZSTR_RPM,               UNIT_RPMS,              0},
  {0x7e,      2,  uint16,    ZSTR_A3,                UNIT_VOLTS,             2},
  {0x7e,      4,  int16,     ZSTR_TEMP2,             UNIT_FAHRENHEIT,        1},

  // 0x7f, QoS DATA, also called Flight Log,, with A, B, L, R, F, H?
  // A - Antenna Fades on Receiver A
  // B - Antenna Fades on Receiver B
  // L - Antenna Fades on left Receiver
  // R - Antenna Fades on right Receiver
  // F - Frame losses
  {I2C_QOS,      0,  uint16,    ZSTR_QOS_A,             UNIT_RAW,               0},
  {I2C_QOS,      2,  uint16,    ZSTR_QOS_B,             UNIT_RAW,               0},
  {I2C_QOS,      4,  uint16,    ZSTR_QOS_L,             UNIT_RAW,               0},
  {I2C_QOS,      6,  uint16,    ZSTR_QOS_R,             UNIT_RAW,               0},
  {I2C_QOS,      8,  uint16,    ZSTR_QOS_F,             UNIT_RAW,               0},
  {I2C_QOS,      10, uint16,    ZSTR_QOS_H,             UNIT_RAW,               0},
  {I2C_QOS,      12, uint16,    ZSTR_A2,                UNIT_VOLTS,             2},


  {0,         0,  int16, NULL,                       UNIT_RAW,               0} //sentinel
};

// The bcd int parameter has wrong endian
static int32_t bcdToInt16(unsigned int bcd) {
  return (bcd & 0x0f00) + 10 * (bcd & 0xf000) + 100 * (bcd & 0x000f) + 1000 * (bcd & 0x00f0);
}

// The bcd int parameter has wrong endian
static int32_t bcdToInt8(unsigned int bcd) {
  return (bcd & 0xf) + 10 * (bcd & 0xf0);
}


// Spektrum uses Big Endian data types
static int32_t spektrumGetValue(uint8_t *packet, int startByte, SpektrumDataType type) {
  uint8_t *data = packet + startByte;
  int32_t val;
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
      return bcdToInt16(*(uint16_t *) (data));
    case uint8bcd:
      return bcdToInt8(*(uint8_t *) (data));
    default:
      return -1;
  }
}

bool isSpektrumValidValue(int32_t value, const SpektrumDataType type) {
  switch (type) {
    case uint16:
      return value != 0xffff;
    case int16:
      return value != 0x7fff;
    case int32:
      return value != 0x7fffffff;
    case uint32:
      return ((uint32_t )value) != 0xffffffff;
    default:
      return true;
  }
}

void processSpektrumPacket(uint8_t *packet) {

  setTelemetryValue(TELEM_PROTO_SPEKTRUM, 0xf100, 0, 0, packet[1], UNIT_DB, 1);
  uint8_t ic2address = packet[2];
  //uint8_t instance = packet[3];

  // highest bit indicates that TM1100 is in use, ignore it
  uint8_t instance = (uint8_t) (ic2address & 0x7f);


  for (const SpektrumSensor *sensor = spektrumSensors; sensor->i2caddress; sensor++) {
    if (ic2address == sensor->i2caddress) {

      // Extract value, skip header
      int32_t value = spektrumGetValue(packet + 4, sensor->startByte, sensor->dataType);

      if (!isSpektrumValidValue(value, sensor->dataType))
        continue;

      if (ic2address == I2C_CELLS && sensor->unit == UNIT_VOLTS) {
        // Map to FrSky style cell values

        int cellIndex = (sensor->startByte / 2) << 16;
        value = value | cellIndex;
      }
      
      if (ic2address == I2C_QOS && sensor->startByte == 0) {
        telemetryData.rssi.set(value);
        telemetryStreaming = TELEMETRY_TIMEOUT10ms;
      }


      uint16_t pseudoId = (sensor->i2caddress << 8 | sensor->startByte);
      setTelemetryValue(TELEM_PROTO_SPEKTRUM, pseudoId, 0, instance, value, sensor->unit, sensor->precision);
    }
  }
}

void processSpektrumTelemetryData(uint8_t data) {
  if (telemetryRxBufferCount == 0 && data != 0xAA) {
    TRACE("[SPK] invalid start byte 0x%02X", data);
    return;
  }

  if (telemetryRxBufferCount < TELEMETRY_RX_PACKET_SIZE) {
    telemetryRxBuffer[telemetryRxBufferCount++] = data;
  }
  else {
    TRACE("[SPK] array size %d error", telemetryRxBufferCount);
    telemetryRxBufferCount = 0;
  }

  if (telemetryRxBufferCount >= SPEKTRUM_TELEMETRY_LENGTH) {

    debugPrintf("[SPK] Packet 0x%02X rssi 0x%02X: ic2 0x%02x, %02x: ",
                telemetryRxBuffer[0], telemetryRxBuffer[1], telemetryRxBuffer[2], telemetryRxBuffer[3]);
    for (int i = 4; i < SPEKTRUM_TELEMETRY_LENGTH; i += 4) {
      debugPrintf("%02X%02X %02X%02X  ", telemetryRxBuffer[i], telemetryRxBuffer[i + 1],
                  telemetryRxBuffer[i + 2], telemetryRxBuffer[i + 3]);
    }
    debugPrintf("\r\n");
    processSpektrumPacket(telemetryRxBuffer);
    telemetryRxBufferCount = 0;
  }
}

const SpektrumSensor *getSpektrumSensor(uint16_t pseudoId) {
  uint8_t startByte = (uint8_t) (pseudoId & 0xff);
  uint8_t i2cadd = (uint8_t) (pseudoId >> 8);
  for (const SpektrumSensor *sensor = spektrumSensors; sensor->i2caddress; sensor++) {
    if (i2cadd == sensor->i2caddress && startByte == sensor->startByte)
      return sensor;
  }
  return nullptr;
}

void spektrumSetDefault(int index, uint16_t id, uint8_t subId, uint8_t instance) {
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
  } else {
    telemetrySensor.init(id);
  }

  storageDirty(EE_MODEL);
}