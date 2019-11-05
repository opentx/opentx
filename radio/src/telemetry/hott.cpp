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

/* Telemetry 
packet[0] = TX RSSI value
packet[1] = TX LQI value
packet[2] = frame number
packet[3-12] telemetry data

The frame number takes the following values: 0x00, 0x01, 0x02, 0x03 and 0x04. The frames can be present or not, they also do not have to follow each others.
Here is a description of the telemetry data for each frame number:
- frame 0x00
[3] = [12] = ??
[4] = RX_Voltage*10 in V
[5] = Temperature-20 in °C
[6] = RX_RSSI
[7] = RX_LQI ??
[8] = RX_STR ??
[9,10] = [10]*256+[9]=max lost packet time in ms, max value seems 2s=0x7D0
[11] = 0x00 ??
- frame 0x01					Unknown
- frame 0x02					Unknown
- frame 0x03					Unknown
- frame 0x04					Unknown
*/

struct HottSensor
{
  const uint16_t id;
  const char * name;
  const TelemetryUnit unit;
  const uint8_t precision;
};

// telemetry frames
enum
{
  HOTT_FRAME_00 = 0x00,
  HOTT_FRAME_01 = 0x01,
  HOTT_FRAME_02 = 0x02,
  HOTT_FRAME_03 = 0x03,
  HOTT_FRAME_04 = 0x04,
};

// telemetry sensors ID
enum
{
  HOTT_ID_RX_VOLTAGE = 0x0004,// RX_Batt Voltage
  HOTT_ID_TEMP1 = 0x0005,     // RX Temperature sensor
  TX_RSSI_ID = 0xFF00,        // Pseudo id outside 1 byte range of Hott sensors
  TX_LQI_ID = 0xFF01,         // Pseudo id outside 1 byte range of Hott sensors
  RX_RSSI_ID = 0xFF02,        // Pseudo id outside 1 byte range of Hott sensors
  RX_LQI_ID = 0xFF03,         // Pseudo id outside 1 byte range of Hott sensors
};

const HottSensor hottSensors[] = {
  //frame 00
  {HOTT_ID_RX_VOLTAGE,   ZSTR_BATT,       UNIT_VOLTS,             1},  // RX_Batt Voltage
  {HOTT_ID_TEMP1,        ZSTR_TEMP1,      UNIT_CELSIUS,           0},  // RX Temperature sensor
  //frame 01
  //frame 02
  //frame 03
  //frame 04
  {TX_RSSI_ID,           ZSTR_TX_RSSI,    UNIT_RAW,               0},  // Pseudo id outside 1 byte range of Hott sensors
  {TX_LQI_ID,            ZSTR_TX_QUALITY, UNIT_RAW,               0},  // Pseudo id outside 1 byte range of Hott sensors
  {RX_RSSI_ID,           ZSTR_RSSI,       UNIT_DB,                0},  // RX RSSI
  {RX_LQI_ID,            ZSTR_RX_QUALITY, UNIT_RAW,               0},  // RX LQI
  {0x00,                 NULL,            UNIT_RAW,               0},  // sentinel
};

const HottSensor * getHottSensor(uint16_t id)
{
  for (const HottSensor * sensor = hottSensors; sensor->id; sensor++) {
    if (id == sensor->id)
      return sensor;
  }
  return nullptr;
}

void processHottPacket(const uint8_t * packet)
{
  // Set TX RSSI Value
  setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, TX_RSSI_ID, 0, 0, packet[0]>>1, UNIT_RAW, 0);
  // Set TX LQI  Value
  setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, TX_LQI_ID, 0, 0, packet[1], UNIT_RAW, 0);

  const HottSensor * sensor;
  int32_t value;

  switch (packet[2]) {
    case HOTT_FRAME_00:
      //RX_VOLT
      value = packet[4];
      sensor = getHottSensor(HOTT_ID_RX_VOLTAGE);
      setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, HOTT_ID_RX_VOLTAGE, 0, 0, value, sensor->unit, sensor->precision);
      //RX_TEMP
      value = packet[5]-20;
      sensor = getHottSensor(HOTT_ID_TEMP1);
      setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, HOTT_ID_TEMP1, 0, 0, value, sensor->unit, sensor->precision);
      // RX_RSSI
      setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, RX_RSSI_ID, 0, 0, packet[6], UNIT_DB, 0);
      // RX_LQI
      telemetryData.rssi.set(packet[7]);
      if (packet[7] > 0) telemetryStreaming = TELEMETRY_TIMEOUT10ms;
      setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, RX_LQI_ID, 0, 0, packet[7], UNIT_RAW, 0);
      return;
  }
  //unknown
  value = (packet[6] << 24) | (packet[5] << 16) | (packet[4] << 8) | packet[3];
  setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, 0xFE00 | packet[2], 0, 0, value, UNIT_RAW, 0);
}

void hottSetDefault(int index, uint16_t id, uint8_t subId, uint8_t instance)
{
  TelemetrySensor &telemetrySensor = g_model.telemetrySensors[index];
  telemetrySensor.id = id;
  telemetrySensor.subId = subId;
  telemetrySensor.instance = instance;

  const HottSensor * sensor = getHottSensor(id);
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
