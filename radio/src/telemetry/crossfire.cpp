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

struct CrossfireSensor {
  const uint8_t id;
  const uint8_t subId;
  const char * name;
  const TelemetryUnit unit;
  const uint8_t prec;
};

const CrossfireSensor crossfireSensors[] = {
  {LINK_STATS_ID, 0, ZSTR_RX_RSSI1,   UNIT_DB,         0},
  {LINK_STATS_ID, 1, ZSTR_RX_RSSI2,   UNIT_DB,         0},
  {LINK_STATS_ID, 2, ZSTR_RX_QUALITY, UNIT_PERCENT,    0},
  {LINK_STATS_ID, 3, ZSTR_RX_SNR,     UNIT_DB,         0},
  {LINK_STATS_ID, 4, ZSTR_ANTENNA,    UNIT_RAW,        0},
  {LINK_STATS_ID, 5, ZSTR_RF_MODE,    UNIT_RAW,        0},
  {LINK_STATS_ID, 6, ZSTR_TX_POWER,   UNIT_MILLIWATTS, 0},
  {LINK_STATS_ID, 7, ZSTR_TX_RSSI,    UNIT_DB,         0},
  {LINK_STATS_ID, 8, ZSTR_TX_QUALITY, UNIT_PERCENT,    0},
  {LINK_STATS_ID, 9, ZSTR_TX_SNR,     UNIT_DB,         0},
  { 0,            0, NULL,            UNIT_RAW,        0} // sentinel
};

const CrossfireSensor * getCrossfireSensor(uint8_t id, uint8_t subId=0)
{
  const CrossfireSensor * result = NULL;
  for (const CrossfireSensor * sensor = crossfireSensors; sensor->id; sensor++) {
    if (id == sensor->id && subId == sensor->subId) {
      result = sensor;
      break;
    }
  }
  return result;
}

void processCrossfireTelemetryFrame(uint8_t id, uint8_t subId, uint32_t value)
{
  const CrossfireSensor * sensor = getCrossfireSensor(id, subId);
  TelemetryUnit unit = UNIT_RAW;
  uint8_t precision = 0;
  if (sensor) {
    unit = sensor->unit;
    precision = sensor->prec;
  }
  setTelemetryValue(TELEM_PROTO_CROSSFIRE, id, 0, subId, value, unit, precision);
}

bool checkCrossfireTelemetryFrameCRC()
{
  // uint8_t length = telemetryRxBuffer[1];
  return true;
}

void processCrossfireTelemetryFrame()
{
  if (!checkCrossfireTelemetryFrameCRC()) {
    return;
  }

  uint8_t id = telemetryRxBuffer[2];
  switch(id) {
    case LINK_STATS_ID:
      for (int i=0; i<10; i++) {
        processCrossfireTelemetryFrame(id, i, telemetryRxBuffer[3+i]);
      }
      break;
  }
}

void processCrossfireTelemetryData(uint8_t data)
{
  if (telemetryRxBufferCount == 0 && data != 0x00) {
    return;
  }

  if (telemetryRxBufferCount == 1 && (data <  || data > TELEMETRY_RX_PACKET_SIZE-2)) {
    telemetryRxBufferCount = 0;
    return;
  }

  if (telemetryRxBufferCount < TELEMETRY_RX_PACKET_SIZE) {
    telemetryRxBuffer[telemetryRxBufferCount++] = data;
  }
  else {
    TRACE("processCrossfirePacket(): length error ");
    DUMP(telemetryRxBuffer, TELEMETRY_RX_PACKET_SIZE);
    telemetryRxBufferCount = 0;
  }

  if (telemetryRxBufferCount > 4) {
    uint8_t length = telemetryRxBuffer[1];
    if (length + 2 == telemetryRxBufferCount) {
      processCrossfireTelemetryFrame();
      telemetryRxBufferCount = 0;
    }
  }
}

void crossfireSetDefault(int index, uint8_t id, uint8_t subId)
{
  TelemetrySensor & telemetrySensor = g_model.telemetrySensors[index];

  telemetrySensor.id = id;
  telemetrySensor.instance = subId;

  const CrossfireSensor * sensor = getCrossfireSensor(id, subId);
  if (sensor) {
    TelemetryUnit unit = sensor->unit;
    uint8_t prec = min<uint8_t>(2, sensor->prec);
    telemetrySensor.init(sensor->name, unit, prec);
    if (id == LINK_STATS_ID) {
      telemetrySensor.logs = true;
    }
  }
  else {
    telemetrySensor.init(id);
  }

  storageDirty(EE_MODEL);
}