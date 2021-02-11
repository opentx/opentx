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

struct MLinkSensor
{
  const uint16_t id;
  const char * name;
  const TelemetryUnit unit;
  const uint8_t precision;
};

const MLinkSensor mlinkSensors[] = {
  {MLINK_RX_VOLTAGE,      ZSTR_BATT,              UNIT_VOLTS,             1},
  {MLINK_VOLTAGE,         ZSTR_VFAS,              UNIT_VOLTS,             1},
  {MLINK_CURRENT,         ZSTR_CURR,              UNIT_AMPS,              1},
  {MLINK_VARIO,           ZSTR_VSPD,              UNIT_METERS_PER_SECOND, 1},
  {MLINK_RPM,             ZSTR_RPM,               UNIT_RPMS,              0},
  {MLINK_TEMP,            ZSTR_TEMP1,             UNIT_CELSIUS,           1},
  {MLINK_HEADING,         ZSTR_HDG,               UNIT_DEGREE,            1},
  {MLINK_ALT,             ZSTR_ALT ,              UNIT_METERS,            0},
  {MLINK_FUEL,            ZSTR_FUEL,              UNIT_PERCENT,           0},
  {MLINK_CAPACITY,        ZSTR_CAPACITY,          UNIT_MAH,               0},
  {MLINK_LIQUIDS,         ZSTR_GASSUIT_RES_VOL,   UNIT_MILLILITERS,       0},
  {MLINK_DISTANCE,        ZSTR_DIST,              UNIT_METERS,            0},
  {MLINK_LQI,             ZSTR_RSSI,              UNIT_RAW,               0},
};

const MLinkSensor * getMLinkSensor(uint16_t id)
{
  for (const MLinkSensor * sensor = mlinkSensors; sensor->id; sensor++) {
    if (id == sensor->id)
      return sensor;
  }
  return nullptr;
}


void processMLinkPacket(const uint8_t * packet)
{
  const uint8_t * data = packet + 2;
  if (data[0] == 0x13) {  // RX-9 DR
    for (uint8_t i = 1; i < 5; i += 3) {  //2 sensors per packet
      int32_t val = (data[i + 2] << 8 | data[i + 1]);
      val = val >> 1; // remove alarm flag
      uint8_t adress = (data[i] & 0xF0) >> 4;
      switch (data[i] & 0x0F) {
        case MLINK_VOLTAGE:
          if ((data[i] & 0xF0) == 0x00){
            setTelemetryValue(PROTOCOL_TELEMETRY_MLINK, MLINK_RX_VOLTAGE, 0, adress, val, UNIT_VOLTS, 1);
          }
          else {
            setTelemetryValue(PROTOCOL_TELEMETRY_MLINK, MLINK_VOLTAGE, 0, adress, val, UNIT_VOLTS, 1);
          }
          break;
        case MLINK_CURRENT:
          setTelemetryValue(PROTOCOL_TELEMETRY_MLINK, MLINK_CURRENT, 0, adress, val, UNIT_AMPS, 1);
          break;
        case MLINK_VARIO:
          setTelemetryValue(PROTOCOL_TELEMETRY_MLINK, MLINK_VARIO, 0, adress, val, UNIT_METERS_PER_SECOND, 1);
          break;
        case MLINK_SPEED:
          setTelemetryValue(PROTOCOL_TELEMETRY_MLINK, MLINK_SPEED, 0, adress, val, UNIT_KMH, 1);
          break;
        case MLINK_RPM:
          if (val < 0)
            val = abs(val) * 10;
          else
            val = val * 100;
          setTelemetryValue(PROTOCOL_TELEMETRY_MLINK, MLINK_RPM, 0, adress, val, UNIT_RPMS, 0);
          break;
        case MLINK_TEMP:
          setTelemetryValue(PROTOCOL_TELEMETRY_MLINK, MLINK_TEMP, 0, adress, val, UNIT_CELSIUS, 1);
          break;
        case MLINK_HEADING:
          setTelemetryValue(PROTOCOL_TELEMETRY_MLINK, MLINK_HEADING, 0, adress, val, UNIT_DEGREE, 1);
          break;
        case MLINK_ALT:
          setTelemetryValue(PROTOCOL_TELEMETRY_MLINK, MLINK_ALT, 0, adress, val, UNIT_METERS, 0);
          break;
        case MLINK_FUEL:
          setTelemetryValue(PROTOCOL_TELEMETRY_MLINK, MLINK_FUEL, 0, adress, val, UNIT_PERCENT, 0);
          break;
        case MLINK_CAPACITY:
          setTelemetryValue(PROTOCOL_TELEMETRY_MLINK, MLINK_CAPACITY, 0, adress, val, UNIT_MAH, 0);
          break;
        case MLINK_LIQUIDS:
          setTelemetryValue(PROTOCOL_TELEMETRY_MLINK, MLINK_LIQUIDS, 0, adress, val, UNIT_MILLILITERS, 0);
          break;
        case MLINK_DISTANCE:
          setTelemetryValue(PROTOCOL_TELEMETRY_MLINK, MLINK_DISTANCE, 0, adress, val * 1000, UNIT_METERS, 0);
          break;
        case MLINK_LQI:
          uint8_t mlinkRssi = data[i + 1] >> 1;
          setTelemetryValue(PROTOCOL_TELEMETRY_MLINK, MLINK_LQI, 0, 0, mlinkRssi, UNIT_RAW, 0);
          telemetryData.rssi.set(mlinkRssi);
          if (mlinkRssi > 0)
            telemetryStreaming = TELEMETRY_TIMEOUT10ms;
          break;
      }
    }
  }
  else if (packet[2] == 0x03) {
    TRACE("RX5 type telemetry");
  }
}
void mlinkSetDefault(int index, uint16_t id, uint8_t subId, uint8_t instance)
{
  TelemetrySensor &telemetrySensor = g_model.telemetrySensors[index];
  telemetrySensor.id = id;
  telemetrySensor.subId = subId;
  telemetrySensor.instance = instance;

  const MLinkSensor * sensor = getMLinkSensor(id);
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
