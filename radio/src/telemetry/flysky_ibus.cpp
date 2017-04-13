/*
 * Copyright (C) OpenTX
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

/*
 * Telemetry format from goebish/Deviation/flysky_afhds2a_a7105.c
 *
 * format from RX:
 * AA | TXID | RXID | sensor id | sensor # | value 16 bit big endian | sensor id ......
 *  max 7 sensors per packet
 *
 *  TXID + RXID are already skipped in MULTI module to save memory+transmission time, format from Multi is:
 *  AA | TX_RSSI | sensor ...
 *
 * OpenTX Mapping
 *
 *  instance = sensor id
 *
 *  Additional sensors from https://github.com/cleanflight/cleanflight/blob/master/src/main/telemetry/ibus.c
 *
 */


#define FLYSKY_TELEMETRY_LENGTH (2+7*4)

struct FlySkySensor {
  const uint16_t id;
  const char *name;
  const TelemetryUnit unit;
  const uint8_t precision;
};

#define TX_RSSI_ID              300      // Pseudo id outside 1 byte range of FlySky sensors
#define FS_ID_TEMP              0x01
#define FS_ID_SNR               0xfa
#define FS_ID_NOISE             0xfb
#define FS_ID_RSSI              0xfc

const FlySkySensor flySkySensors[] = {

  // RX Voltage (remapped, really 0x0)
  {0x100,           ZSTR_A1,                UNIT_VOLTS,                  2},
  // Temperature
  {FS_ID_TEMP,      ZSTR_TEMP1,             UNIT_CELSIUS,                1},
  // RPM
  {0x02,            ZSTR_RPM,               UNIT_RAW,                    0},
  // External voltage
  {0x03,            ZSTR_A3,                UNIT_VOLTS,                  2},
  // RX SNR
  {FS_ID_SNR,       ZSTR_RX_SNR,            UNIT_DB,                     0},
  // RX Noise
  {FS_ID_NOISE,     ZSTR_RX_NOISE,          UNIT_DB,                     0},
  // RX RSSI (0xfc)
  {FS_ID_RSSI,      ZSTR_RSSI,              UNIT_DB,                     0},
  // RX error rate
  {0xfe,            ZSTR_RX_QUALITY,        UNIT_RAW,                    0},
  // 0xff is an unused sensor slot
  // Pseudo sensor for TRSSI
  {TX_RSSI_ID,      ZSTR_TX_RSSI,           UNIT_RAW,                    0},
  // sentinel
  {0x00,            NULL,                   UNIT_RAW,                    0},
};

static void processFlySkySensor(const uint8_t *packet)
{
  uint16_t id = packet[0];
  const uint8_t instance = packet[1];
  int32_t value = (packet[3] << 8)  + packet[2];

  if (id == 0xff) {
    // No sensor
    return;
  }

  if (id == 0) {
    // Some part of OpenTX does not like sensor with id and instance 0, remap to 0x100
    id = 0x100;
  }

  if (id == FS_ID_SNR) {
    telemetryData.rssi.set(value);
  }

  for (const FlySkySensor * sensor = flySkySensors; sensor->id; sensor++) {
    // Extract value, skip header
    if (sensor->id == id) {
      // The Noise and Signal sensors that are specified in dB send the absolute value
      if (id == FS_ID_NOISE || id == FS_ID_RSSI)
        value = -value;
      else if (id == FS_ID_TEMP)
        // Temperature sensors have 40 degree offset
        value -= 400;
      setTelemetryValue(TELEM_PROTO_FLYSKY_IBUS, id, 0, instance, value, sensor->unit, sensor->precision);
      return;
    }
  }
  setTelemetryValue(TELEM_PROTO_FLYSKY_IBUS, id, 0, instance, value, UNIT_RAW, 0);
}

void processFlySkyPacket(const uint8_t *packet)
{
  // Set TX RSSI Value, reverse MULTIs scaling
  setTelemetryValue(TELEM_PROTO_FLYSKY_IBUS, TX_RSSI_ID, 0, 0, packet[0], UNIT_RAW, 0);

  for (int sensor = 0; sensor < 7; sensor++) {
    int index = 1 + (4 * sensor);
    processFlySkySensor(packet+index);
  }
  telemetryStreaming = TELEMETRY_TIMEOUT10ms;
}

void processFlySkyTelemetryData(uint8_t data)
{
  if (telemetryRxBufferCount == 0 && data != 0xAA) {
    TRACE("[IBUS] invalid start byte 0x%02X", data);
    return;
  }

  if (telemetryRxBufferCount < TELEMETRY_RX_PACKET_SIZE) {
    telemetryRxBuffer[telemetryRxBufferCount++] = data;
  }
  else {
    TRACE("[IBUS] array size %d error", telemetryRxBufferCount);
    telemetryRxBufferCount = 0;
  }


  if (telemetryRxBufferCount >= FLYSKY_TELEMETRY_LENGTH) {
    // debug print the content of the packets
#if 0
    debugPrintf("[IBUS] Packet 0x%02X rssi 0x%02X: ",
                telemetryRxBuffer[0], telemetryRxBuffer[1]);
    for (int i=0; i<7; i++) {
      debugPrintf("[%02X %02X %02X%02X] ", telemetryRxBuffer[i*4+2], telemetryRxBuffer[i*4 + 3],
                  telemetryRxBuffer[i*4 + 4], telemetryRxBuffer[i*4 + 5]);
    }
    debugPrintf("\r\n");
#endif
    processFlySkyPacket(telemetryRxBuffer+1);
    telemetryRxBufferCount = 0;
  }
}

const FlySkySensor *getFlySkySensor(uint16_t id)
{
  for (const FlySkySensor * sensor = flySkySensors; sensor->id; sensor++) {
    if (id == sensor->id)
      return sensor;
  }
  return nullptr;
}

void flySkySetDefault(int index, uint16_t id, uint8_t subId, uint8_t instance)
{
  TelemetrySensor &telemetrySensor = g_model.telemetrySensors[index];
  telemetrySensor.id = id;
  telemetrySensor.subId = subId;
  telemetrySensor.instance = instance;

  const FlySkySensor *sensor = getFlySkySensor(id);
  if (sensor) {
    TelemetryUnit unit = sensor->unit;
    uint8_t prec = min<uint8_t>(2, sensor->precision);
    telemetrySensor.init(sensor->name, unit, prec);

    if (unit == UNIT_RPMS) {
      telemetrySensor.custom.ratio = 1;
      telemetrySensor.custom.offset = 1;
    }
  }
  else {
    telemetrySensor.init(id);
  }

  storageDirty(EE_MODEL);
}
