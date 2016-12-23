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

#define I2C_HIGH_CURRENT 0x03
#define I2C_TEXTGEN 0x0c
#define I2C_GPS  0x17
#define I2C_GPS2 0x17
#define I2C_CELLS 0x3a
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
  {0x01,             0,  int16,     ZSTR_A1,                UNIT_VOLTS,                  1},

  // Temperature internal sensor
  {0x02,             0,  int16,     ZSTR_TEMP1,             UNIT_CELSIUS,                1},

  // High current internal sensor (0x03), 300A/2048 resolution
  {I2C_HIGH_CURRENT, 0,  int16,     ZSTR_CURR,              UNIT_AMPS,                   1},

  //Powerbox (also mentioned as 0x7D but that is also transmitter frame data)
  {0x0a,             0,  uint16,    ZSTR_BATT1_VOLTAGE,     UNIT_VOLTS,                  2},
  {0x0a,             2,  uint16,    ZSTR_BATT2_VOLTAGE,     UNIT_VOLTS,                  2},
  {0x0a,             4,  uint16,    ZSTR_BATT1_CONSUMPTION, UNIT_MAH,                    0},
  {0x0a,             6,  uint16,    ZSTR_BATT2_CONSUMPTION, UNIT_MAH,                    0},


  // Lap Timer
  {0x0b,             0, uint8,      ZSTR_LAP_NUMBER,        UNIT_RAW,                    0},
  {0x0b,             0, uint8,      ZSTR_GATE_NUMBER,       UNIT_RAW,                    0},
  {0x0b,             0, uint32,     ZSTR_LAP_TIME,          UNIT_SECONDS,                3},
  {0x0b,             0, uint32,     ZSTR_GATE_TIME,         UNIT_SECONDS,                3},

  // Text Generator
  {I2C_TEXTGEN,      0, uint32,     ZSTR_FLIGHT_MODE,       UNIT_TEXT,                   0},

  // AirSpeed, also has max (+2, int16)
  {0x11,             0,  int16,     ZSTR_ASPD,              UNIT_KMH,                    0},

  // Altitude, also has max (+2, int16)
  {0x12,             0,  int16,     ZSTR_ALT,               UNIT_METERS,                 1},


  // {0x38, strain}

  // G-Force (+min, max)
  {0x14,             0,  int16,     ZSTR_ACCX,              UNIT_G,                      2},
  {0x14,             2,  int16,     ZSTR_ACCY,              UNIT_G,                      2},
  {0x14,             4,  int16,     ZSTR_ACCZ,              UNIT_G,                      2},


  // 0x15,  JETCAT/TURBINE, BCD Encoded values
  // TODO: Add decoding of status information
  // {0x15,             0,  uint8,     ZSTR_STATUS,            UNIT_BITFIELD,               0},
  {0x15,             1,  uint8bcd,  ZSTR_THROTTLE,          UNIT_PERCENT,                0},
  {0x15,             2,  uint16bcd, ZSTR_A1,                UNIT_VOLTS,                  2},
  {0x15,             4,  uint16bcd, ZSTR_A2,                UNIT_VOLTS,                  2},
  {0x15,             6,  uint32bcd, ZSTR_RPM,               UNIT_RPMS,                   0},
  {0x15,             10, uint16bcd, ZSTR_TEMP1,             UNIT_CELSIUS,                0},
  // {0x15,             0,  uint8,     ZSTR_STATUS,            UNIT_BITFIELD,               0},

  // 0x16-0x17 GPS
  // GPS is bcd encoded and also uses flags. Hard to get right without an actual GPS Sensor
  // Time/date is also BCD encoded but so this FrSky's, so treat it as uint32
  {I2C_GPS2,         0,  uint16bcd, ZSTR_GSPD,              UNIT_KTS,                    1},
  {I2C_GPS2,         2,  uint32,    ZSTR_GPSDATETIME,       UNIT_DATETIME,               0},


  //{0x17, 2, uint32, ZSTR_GPSDATETIME, UNIT_DATETIME}, utc in bcd HH:MM:SS.S
  {0x17,             6,  uint8bcd,  ZSTR_SATELLITES,        UNIT_RAW,                    0},
  //{0x17, 7, uint8bcd, ZSTR_GPSALT, UNIT_METERS}, altitude high bits

  // 0x19 Jetcat flow rate
  // {0x19,             0,  uint16bcd, ZSTR_FUEL_CONSUMPTION,  UNIT_MILLILITERS_PER_MINUTE, 1}, missing ml/min
  {0x19,             2,  uint32bcd, ZSTR_FUEL,              UNIT_MILLILITERS,            1},

  // 0x1a Gyro
  {0x1a,             0,  int16,     ZSTR_GYROX,             UNIT_DEGREE,                 1},
  {0x1a,             2,  int16,     ZSTR_GYROY,             UNIT_DEGREE,                 1},
  {0x1a,             4,  int16,     ZSTR_GYROZ,             UNIT_DEGREE,                 1},

  // 0x1b Attitude & Mag Compass
  // mag Units are tbd so probably no sensor in existance, ignore them for now
  {0x1b,             0,  int16,     ZSTR_ROLL,              UNIT_DEGREE,                 1},
  {0x1b,             2,  int16,     ZSTR_PITCH,             UNIT_DEGREE,                 1},
  {0x1b,             4,  int16,     ZSTR_YAW,               UNIT_DEGREE,                 1},

  // {0x20, esc}, does not exist in the wild?

  // Dual Cell monitor (0x34)
  {0x34,             0,  int16,     ZSTR_BATT1_CURRENT,     UNIT_AMPS,                   1},
  {0x34,             2,  int16,     ZSTR_BATT1_CONSUMPTION, UNIT_MAH,                    1},
  {0x34,             4,  uint16,    ZSTR_BATT1_TEMP,        UNIT_CELSIUS,                1},
  {0x34,             6,  int16,     ZSTR_BATT2_CURRENT,     UNIT_AMPS,                   1},
  {0x34,             8,  int16,     ZSTR_BATT2_CONSUMPTION, UNIT_MAH,                    1},
  {0x34,             10, uint16,    ZSTR_BATT2_TEMP,        UNIT_CELSIUS,                1},

  // Tank pressure + custom input bits (ignore for now)
  //{0x38,             0,  uint16,    ZSTR_STATUS_BITS,       UNIT_BITFIELD,               0},
  //{0x38,             0,  uint16,    ZSTR_PRESSSURE,         UNIT_PSI,                    1},

  // Cells (0x3a)
  {I2C_CELLS,        0,  uint16,    ZSTR_CELLS,             UNIT_VOLTS,                  2},
  {I2C_CELLS,        2,  uint16,    ZSTR_CELLS,             UNIT_VOLTS,                  2},
  {I2C_CELLS,        4,  uint16,    ZSTR_CELLS,             UNIT_VOLTS,                  2},
  {I2C_CELLS,        6,  uint16,    ZSTR_CELLS,             UNIT_VOLTS,                  2},
  {I2C_CELLS,        8,  uint16,    ZSTR_CELLS,             UNIT_VOLTS,                  2},
  {I2C_CELLS,        10, uint16,    ZSTR_CELLS,             UNIT_VOLTS,                  2},
  {I2C_CELLS,        12, uint16,    ZSTR_TEMP2,             UNIT_CELSIUS,                2},
  
  // Vario-S
  {0x40,             0,  int16,     ZSTR_ALT,               UNIT_METERS,                 1},
  {0x40,             2,  int16,     ZSTR_VSPD,              UNIT_METERS_PER_SECOND,      1},

  // 0x50-0x56 custom 3rd party sensors
  //{0x50, 0, int16, ZSTR_}
  
  // 0x7d are transmitter channels frame data [7], probably only available on the Spektrum
  // telemetry bus on the model itself

  // RPM/Volts/Temperature
  {0x7e,             0,  uint16,    ZSTR_RPM,               UNIT_RPMS,                   0},
  {0x7e,             2,  uint16,    ZSTR_A3,                UNIT_VOLTS,                  2},
  {0x7e,             4,  int16,     ZSTR_TEMP2,             UNIT_FAHRENHEIT,             0},

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

// The bcd int parameter has wrong endian
static int32_t bcdToInt16(uint16_t bcd)
{
  return (bcd & 0x0f00) + 10 * (bcd & 0xf000) + 100 * (bcd & 0x000f) + 1000 * (bcd & 0x00f0);
}

// The bcd int parameter has wrong endian
static int32_t bcdToInt8(uint8_t bcd)
{
  return (bcd & 0xf) + 10 * (bcd & 0xf0);
}

static int32_t bcdToInt32(uint32_t bcd)
{
  return bcdToInt16(bcd >> 16) + 10000 * bcdToInt16(bcd);
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
  setTelemetryValue(TELEM_PROTO_SPEKTRUM, (I2C_PSEUDO_TX << 8) + 0, 0, 0, packet[1], UNIT_RAW, 0);
  // highest bit indicates that TM1100 is in use, ignore it
  uint8_t i2cAddress = (packet[2] & 0x7f);
  uint8_t instance = packet[3];

  if (i2cAddress == I2C_TEXTGEN) {
    uint8_t lineNumber = packet[4];

    uint16_t pseudoId = (i2cAddress << 8 | lineNumber);

    for (int i=5; i<SPEKTRUM_TELEMETRY_LENGTH; i++)
    {
      setTelemetryValue(TELEM_PROTO_SPEKTRUM, pseudoId, 0, instance, packet[i], UNIT_TEXT, i-5);
    }
    // Set a sential \0 just for safety since we have the space there
    setTelemetryValue(TELEM_PROTO_SPEKTRUM, pseudoId, 0, instance, '\0', UNIT_TEXT, 13);


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

      if (i2cAddress == I2C_CELLS && sensor->unit == UNIT_VOLTS) {
        // Map to FrSky style cell values
        int cellIndex = (sensor->startByte / 2) << 16;
        value = value | cellIndex;
      }

      if (sensor->i2caddress == I2C_HIGH_CURRENT && sensor->unit == UNIT_AMPS)
        // Spektrum's documents talks says: Resolution: 300A/2048 = 0.196791 A/tick
        // Note that 300/2048 = 0,1464. DeviationTX also uses the 0.196791 figure
        value = value * 196791 / 100000;
      else if (sensor->i2caddress == I2C_GPS2 && sensor->unit == UNIT_DATETIME) {
        // Frsky time is HH:MM:SS:00 bcd encodes while spektrum uses 0HH:MM:SS.S
        value = (value & 0xfffffff0) << 4;
      }

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
      
      uint16_t pseudoId = (sensor->i2caddress << 8 | sensor->startByte);
      setTelemetryValue(TELEM_PROTO_SPEKTRUM, pseudoId, 0, instance, value, sensor->unit, sensor->precision);
    }
  }
  if (!handled) {
    // If we see a sensor that is not handled at all, add the raw values of this sensor to show its existance to
    // the user and help debugging/implementing these sensors
    for (int startByte=0; startByte<14; startByte+=2) {
      int32_t value = spektrumGetValue(packet + 4, startByte, uint16);
      uint16_t pseudoId = i2cAddress << 8 | startByte;
      setTelemetryValue(TELEM_PROTO_SPEKTRUM, pseudoId, 0, instance, value, UNIT_RAW, 0);
    }
  }
}

// Parse the DSM2 bind response, Fields are as per http://www.rcgroups.com/forums/showpost.php?p=35692146&postcount=5191
// "I"  here means the multi module

/*
0-3   4 bytes -> Cyrf ID of the TX xor 0xFF but you don't care as I've checked it already...
4     1 byte -> RX version but you don't care...
5     1 byte -> number of channels, example 0x06=6 channels
6     1 byte -> max DSM type allowed:
        0x01 => 22ms 1024 DSM2 1 packet => number of channels is <8 and no telemetry
        0x02 => 22ms 1024 DSM2 2 packets => either a number of channel >7 or telemetry enable RX
        0x12 => 11ms 2048 DSM2 2 packets => can be any number of channels with/without telemetry -> this mode might be supported following Mike's trials, note the channels should be duplicated between the packets which is not the case today
        0xa2 => 22ms 2048 DSMX 1 packet => number of channels is <8 and no telemetry
        0xb2 => 11ms 2048 DSMX => can be any number of channels with/without telemetry -> this mode is only half supported since the channels should be duplicated between the packets which is not the case but might be supported following Mike's trials
7     0x00: not sure of the use of this byte since I've always seen it at 0...
8-9   2 bytes CRC but you don't care as I've checked it already...

 Examples:           DSM   #Chan  RXver
 Inductrix           0xa2   07     1
 LemonRX+Sat+tele    0xb2   07     1

 */
void processDSMBindPacket(const uint8_t *packet)
{
  uint32_t debugval;
  if (g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_MULTIMODULE && g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true) == MM_RF_PROTO_DSM2
    && g_model.moduleData[EXTERNAL_MODULE].multi.autoBindMode) {

    int channels = packet[5];
    // Only sets channel etc when in DSM multi mode
    g_model.moduleData[EXTERNAL_MODULE].channelsCount = channels - 8;

    // bool use11ms = (packet[8] & 0x10) ;
    if (packet[6] >= 0xb2)
      g_model.moduleData[EXTERNAL_MODULE].subType = MM_RF_DSM2_SUBTYPE_DSMX_11;
    else if (packet[6] >= 0xa2)
      g_model.moduleData[EXTERNAL_MODULE].subType = MM_RF_DSM2_SUBTYPE_DSMX_22;
    else if (packet[6] >= 0x12)
      g_model.moduleData[EXTERNAL_MODULE].subType = MM_RF_DSM2_SUBTYPE_DSM2_11;
    else
      g_model.moduleData[EXTERNAL_MODULE].subType = MM_RF_DSM2_SUBTYPE_DSM2_22;

    storageDirty(EE_MODEL);

  }

  debugval = packet[7] << 24 | packet[6] << 16 | packet[5] << 8 | packet[4];

  /* log the bind packet as telemetry for quick debugging */
  setTelemetryValue(TELEM_PROTO_SPEKTRUM, (I2C_PSEUDO_TX << 8) + 4, 0, 0, debugval, UNIT_RAW, 0);

  /* Finally stop binding as the rx just told us that it is bound */
  if (g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_MULTIMODULE && g_model.moduleData[EXTERNAL_MODULE].getMultiProtocol(true) == MM_RF_PROTO_DSM2
    && moduleFlag[EXTERNAL_MODULE] == MODULE_BIND) {
    multiBindStatus=MULTI_BIND_FINISHED;
  }
}

void processSpektrumTelemetryData(uint8_t data)
{
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

  if (telemetryRxBuffer[1] == 0x80 && telemetryRxBufferCount >= DSM_BIND_PACKET_LENGTH) {
    processDSMBindPacket(telemetryRxBuffer+2);
    telemetryRxBufferCount = 0;
    return;
  }

  if (telemetryRxBufferCount >= SPEKTRUM_TELEMETRY_LENGTH) {
    // Debug print content of Telemetry to console
#if 0
    debugPrintf("[SPK] Packet 0x%02X rssi 0x%02X: ic2 0x%02x, %02x: ",
                telemetryRxBuffer[0], telemetryRxBuffer[1], telemetryRxBuffer[2], telemetryRxBuffer[3]);
    for (int i=4; i<SPEKTRUM_TELEMETRY_LENGTH; i+=4) {
      debugPrintf("%02X%02X %02X%02X  ", telemetryRxBuffer[i], telemetryRxBuffer[i + 1],
                  telemetryRxBuffer[i + 2], telemetryRxBuffer[i + 3]);
    }
    debugPrintf("\r\n");
#endif
    processSpektrumPacket(telemetryRxBuffer);
    telemetryRxBufferCount = 0;
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