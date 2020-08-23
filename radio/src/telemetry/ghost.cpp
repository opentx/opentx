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

const char * ghstRfProfileValue[GHST_RF_PROFILE_COUNT] = {"Auto", "Norm", "Race", "Pure", "Long"};
const uint32_t ghstPwrValueuW[GHST_PWR_COUNT] = {16, 100, 1000, 25000, 100000, 200000,
                                                 350000, 500000, 600000, 1000000, 15000000,
                                                 2000000, 3000000, 4000000};

struct GhostSensor
{
  const uint16_t id;
  const char * name;
  const TelemetryUnit unit;
  const uint8_t precision;
};

// telemetry sensors ID
enum
{
  GHOST_ID_RX_RSSI = 0x0001,            // Rx-side RSSI
  GHOST_ID_RX_LQ = 0x0002,              // Rx-side link quality
  GHOST_ID_RX_SNR = 0x0003,             // Rx-side signal to noise
  GHOST_ID_FRAME_RATE = 0x0004,         // Tx-side frame rate
  GHOST_ID_TX_POWER = 0x0005,           // Tx-side power output
  GHOST_ID_RF_MODE = 0x0006,            // Tx-side frame rate
  GHOST_ID_TOTAL_LATENCY = 0x0007,      // Tx-side total latency
};

const GhostSensor ghostSensors[] = {
  {GHOST_ID_RX_RSSI,       ZSTR_RSSI,          UNIT_DB,         0},
  {GHOST_ID_RX_LQ,         ZSTR_RX_QUALITY,    UNIT_PERCENT,    0},
  {GHOST_ID_RX_SNR,        ZSTR_RX_SNR,        UNIT_DB,         0},

  {GHOST_ID_FRAME_RATE,    ZSTR_FRAME_RATE,    UNIT_HERTZ,      0},
  {GHOST_ID_TX_POWER,      ZSTR_TX_POWER,      UNIT_MILLIWATTS, 0},
  {GHOST_ID_RF_MODE,       ZSTR_RF_MODE,       UNIT_TEXT,       0},
  {GHOST_ID_TOTAL_LATENCY, ZSTR_TOTAL_LATENCY, UNIT_US,         0},

  {0x00,                   NULL,               UNIT_RAW,        0},
};

const GhostSensor * getGhostSensor(uint8_t id)
{
  for (const GhostSensor * sensor = ghostSensors; sensor->id; sensor++) {
    if (id == sensor->id)
      return sensor;
  }
  return nullptr;
}

void processGhostTelemetryValue(uint8_t index, int32_t value)
{
  if (!TELEMETRY_STREAMING())
    return;

  const GhostSensor * sensor = getGhostSensor(index);
  setTelemetryValue(PROTOCOL_TELEMETRY_GHOST, sensor->id, 0, 0, value, sensor->unit, sensor->precision);
}

bool checkGhostTelemetryFrameCRC()
{
  uint8_t len = telemetryRxBuffer[1];
  uint8_t crc = crc8(&telemetryRxBuffer[2], len - 1);
  return (crc == telemetryRxBuffer[len + 1]);
}

uint16_t getTelemetryValue_u16(uint8_t index)
{
  return (telemetryRxBuffer[index] << 8) | telemetryRxBuffer[index + 1];
}

uint32_t getTelemetryValue_s32(uint8_t index)
{
  uint32_t val = 0;
  for (int i = 0; i < 4; ++i)
    val <<= 8, val |= telemetryRxBuffer[index + i];
  return val;
}

void processGhostTelemetryFrame()
{
  if (!checkGhostTelemetryFrameCRC()) {
    TRACE("[GS] CRC error");
    return;
  }

  uint8_t id = telemetryRxBuffer[2];
  switch (id) {
    case GHST_DL_LINK_STAT:
      uint8_t rssiVal = min<uint8_t>(telemetryRxBuffer[3], 100);
      uint8_t lqVal = min<uint8_t>(telemetryRxBuffer[4], 100);
      uint8_t snrVal = min<uint8_t>(telemetryRxBuffer[5], 100);

      processGhostTelemetryValue(GHOST_ID_RX_RSSI, rssiVal);
      processGhostTelemetryValue(GHOST_ID_RX_LQ, lqVal);
      processGhostTelemetryValue(GHOST_ID_RX_SNR, snrVal);

      // give OpenTx the LQ value, not RSSI
      TRACE("LQ %d", lqVal);
      if (lqVal) {
        telemetryData.rssi.set(lqVal);
        telemetryStreaming = TELEMETRY_TIMEOUT10ms;
      }
      else {
        telemetryData.rssi.reset();
        telemetryStreaming = 0;
      }

      uint8_t txPwrEnum = min<uint8_t>(telemetryRxBuffer[6], GHST_PWR_4W);
      processGhostTelemetryValue(GHOST_ID_TX_POWER, ghstPwrValueuW[txPwrEnum] / 1000);
      processGhostTelemetryValue(GHOST_ID_FRAME_RATE, getTelemetryValue_u16(7));
      processGhostTelemetryValue(GHOST_ID_TOTAL_LATENCY, getTelemetryValue_u16(9));
      uint8_t rfModeEnum = min<uint8_t>(telemetryRxBuffer[11], GHST_RF_PROFILE_MAX);

      // RF mode string, one char at a time
      const GhostSensor * sensor = getGhostSensor(GHOST_ID_RF_MODE);
      const char * rfModeString = ghstRfProfileValue[rfModeEnum];
      int i = 0;
      if (TELEMETRY_STREAMING()) {
        do {
          setTelemetryValue(PROTOCOL_TELEMETRY_GHOST, sensor->id, 0, 0, rfModeString[i], UNIT_TEXT, i);
        } while (rfModeString[i++] != 0);
      }
      break;
  }
}

void processGhostTelemetryData(uint8_t data)
{
  if (telemetryRxBufferCount == 0 && data != GHST_ADDR_RADIO) {
    TRACE("[GH] address 0x%02X error", data);
    return;
  }

  if (telemetryRxBufferCount < TELEMETRY_RX_PACKET_SIZE) {
    telemetryRxBuffer[telemetryRxBufferCount++] = data;
  }
  else {
    TRACE("[GH] array size %d error", telemetryRxBufferCount);
    telemetryRxBufferCount = 0;
  }

  if (telemetryRxBufferCount > 4) {
    uint8_t length = telemetryRxBuffer[1];
    if (length + 2 == telemetryRxBufferCount) {
      processGhostTelemetryFrame();
      telemetryRxBufferCount = 0;
    }
  }
}


void ghostSetDefault(int index, uint8_t id, uint8_t subId)
{
  TelemetrySensor &telemetrySensor = g_model.telemetrySensors[index];

  telemetrySensor.id = id;
  telemetrySensor.instance = subId;

  const GhostSensor * sensor = getGhostSensor(id);
  if (sensor) {
    TelemetryUnit unit = sensor->unit;
    uint8_t prec = min<uint8_t>(2, sensor->precision);
    telemetrySensor.init(sensor->name, unit, prec);
  }
  else
    telemetrySensor.init(id);

  storageDirty(EE_MODEL);
}

