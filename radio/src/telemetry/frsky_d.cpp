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

#if (defined(FRSKY_HUB) || defined(WS_HOW_HIGH))
void checkMinMaxAltitude()
{
  if (TELEMETRY_RELATIVE_BARO_ALT_BP > telemetryData.hub.maxAltitude)
    telemetryData.hub.maxAltitude = TELEMETRY_RELATIVE_BARO_ALT_BP;
  if (TELEMETRY_RELATIVE_BARO_ALT_BP < telemetryData.hub.minAltitude)
    telemetryData.hub.minAltitude = TELEMETRY_RELATIVE_BARO_ALT_BP;
}
#endif

#if defined(FRSKY_HUB)
void parseTelemHubByte(uint8_t byte)
{
  static int8_t structPos;
  static uint8_t lowByte;
  static TS_STATE state = TS_IDLE;

  if (byte == 0x5e) {
    state = TS_DATA_ID;
    return;
  }
  if (state == TS_IDLE) {
    return;
  }
  if (state & TS_XOR) {
    byte = byte ^ 0x60;
    state = (TS_STATE)(state - TS_XOR);
  }
  else if (byte == 0x5d) {
    state = (TS_STATE)(state | TS_XOR);
    return;
  }
  if (state == TS_DATA_ID) {
    if (byte > 0x3f) {
      state = TS_IDLE;
    }
    else {
      structPos = byte * 2;
      state = TS_DATA_LOW;
    }
    return;
  }
  if (state == TS_DATA_LOW) {
    lowByte = byte;
    state = TS_DATA_HIGH;
    return;
  }

  state = TS_IDLE;

#if defined(GPS)
  if ((uint8_t)structPos == offsetof(FrskyTelemetryData, gpsLatitude_bp)) {
    if (lowByte || byte)
      telemetryData.hub.gpsFix = 1;
    else if (telemetryData.hub.gpsFix > 0 && telemetryData.hub.gpsLatitude_bp > 1)
      telemetryData.hub.gpsFix = 0;
  }
  else if ((uint8_t)structPos == offsetof(FrskyTelemetryData, gpsLongitude_bp)) {
    if (lowByte || byte)
      telemetryData.hub.gpsFix = 1;
    else if (telemetryData.hub.gpsFix > 0 && telemetryData.hub.gpsLongitude_bp > 1)
      telemetryData.hub.gpsFix = 0;
  }
  
  if ((uint8_t)structPos == offsetof(FrskyTelemetryData, gpsAltitude_bp) ||
      ((uint8_t)structPos >= offsetof(FrskyTelemetryData, gpsAltitude_ap) && (uint8_t)structPos <= offsetof(FrskyTelemetryData, gpsLatitudeNS) && (uint8_t)structPos != offsetof(FrskyTelemetryData, baroAltitude_bp) && (uint8_t)structPos != offsetof(FrskyTelemetryData, baroAltitude_ap))) {
    // If we don't have a fix, we may discard the value
    if (telemetryData.hub.gpsFix <= 0)
      return;
  }
#endif  // #if defined(GPS)

  ((uint8_t*)&telemetryData.hub)[structPos] = lowByte;
  ((uint8_t*)&telemetryData.hub)[structPos+1] = byte;

  switch ((uint8_t)structPos) {

    case offsetof(FrskyTelemetryData, rpm):
      telemetryData.hub.rpm *= (uint8_t)60/(g_model.frsky.blades+2);
      if (telemetryData.hub.rpm > telemetryData.hub.maxRpm)
        telemetryData.hub.maxRpm = telemetryData.hub.rpm;
      break;

    case offsetof(FrskyTelemetryData, temperature1):
      if (telemetryData.hub.temperature1 > telemetryData.hub.maxTemperature1)
        telemetryData.hub.maxTemperature1 = telemetryData.hub.temperature1;
      break;

    case offsetof(FrskyTelemetryData, temperature2):
      if (telemetryData.hub.temperature2 > telemetryData.hub.maxTemperature2)
        telemetryData.hub.maxTemperature2 = telemetryData.hub.temperature2;
      break;

    case offsetof(FrskyTelemetryData, current):
#if defined(FAS_OFFSET) || !defined(CPUM64)
      if ((int16_t)telemetryData.hub.current > 0 && ((int16_t)telemetryData.hub.current + g_model.frsky.fasOffset) > 0)
        telemetryData.hub.current += g_model.frsky.fasOffset;
      else
        telemetryData.hub.current = 0;
#endif
      if (telemetryData.hub.current > telemetryData.hub.maxCurrent)
        telemetryData.hub.maxCurrent = telemetryData.hub.current;
      break;

    case offsetof(FrskyTelemetryData, currentConsumption):
      // we receive data from openXsensor. stops the calculation of consumption and power
      telemetryData.hub.openXsensor = 1;
      break;

    case offsetof(FrskyTelemetryData, volts_ap):
#if defined(FAS_PROTOTYPE)
      telemetryData.hub.vfas = (telemetryData.hub.volts_bp * 10 + telemetryData.hub.volts_ap);
#else
      telemetryData.hub.vfas = ((telemetryData.hub.volts_bp * 10 + telemetryData.hub.volts_ap) * 21) / 11;
#endif
      /* TODO later if (!telemetryData.hub.minVfas || telemetryData.hub.minVfas > telemetryData.hub.vfas)
        telemetryData.hub.minVfas = telemetryData.hub.vfas; */
      break;

    case offsetof(FrskyTelemetryData, baroAltitude_bp):
      // First received barometer altitude => Altitude offset
      if (!telemetryData.hub.baroAltitudeOffset)
        telemetryData.hub.baroAltitudeOffset = -telemetryData.hub.baroAltitude_bp;
      telemetryData.hub.baroAltitude_bp += telemetryData.hub.baroAltitudeOffset;
      checkMinMaxAltitude();
      break;

#if defined(GPS)
    case offsetof(FrskyTelemetryData, gpsAltitude_ap):
      if (!telemetryData.hub.gpsAltitudeOffset)
        telemetryData.hub.gpsAltitudeOffset = -telemetryData.hub.gpsAltitude_bp;
      telemetryData.hub.gpsAltitude_bp += telemetryData.hub.gpsAltitudeOffset;
      if (!telemetryData.hub.baroAltitudeOffset) {
        if (telemetryData.hub.gpsAltitude_bp > telemetryData.hub.maxAltitude)
          telemetryData.hub.maxAltitude = telemetryData.hub.gpsAltitude_bp;
        if (telemetryData.hub.gpsAltitude_bp < telemetryData.hub.minAltitude)
          telemetryData.hub.minAltitude = telemetryData.hub.gpsAltitude_bp;
      }
      if (!telemetryData.hub.pilotLatitude && !telemetryData.hub.pilotLongitude) {
        // First received GPS position => Pilot GPS position
        getGpsPilotPosition();
      }
      else if (telemetryData.hub.gpsDistNeeded || menuHandlers[menuLevel] == menuViewTelemetryFrsky) {
        getGpsDistance();
      }
      break;

    case offsetof(FrskyTelemetryData, gpsSpeed_bp):
      // Speed => Max speed
      if (telemetryData.hub.gpsSpeed_bp > telemetryData.hub.maxGpsSpeed)
        telemetryData.hub.maxGpsSpeed = telemetryData.hub.gpsSpeed_bp;
      break;
#endif

    case offsetof(FrskyTelemetryData, volts):
      frskyUpdateCells();
      break;

    case offsetof(FrskyTelemetryData, accelX):
    case offsetof(FrskyTelemetryData, accelY):
    case offsetof(FrskyTelemetryData, accelZ):
      *(int16_t*)(&((uint8_t*)&telemetryData.hub)[structPos]) /= 10;
      break;

#if 0
    case offsetof(FrskyTelemetryData, gpsAltitude_bp):
    case offsetof(FrskyTelemetryData, fuelLevel):
    case offsetof(FrskyTelemetryData, gpsLongitude_bp):
    case offsetof(FrskyTelemetryData, gpsLatitude_bp):
    case offsetof(FrskyTelemetryData, gpsCourse_bp):
    case offsetof(FrskyTelemetryData, day):
    case offsetof(FrskyTelemetryData, year):
    case offsetof(FrskyTelemetryData, sec):
    case offsetof(FrskyTelemetryData, gpsSpeed_ap):
    case offsetof(FrskyTelemetryData, gpsLongitude_ap):
    case offsetof(FrskyTelemetryData, gpsLatitude_ap):
    case offsetof(FrskyTelemetryData, gpsCourse_ap):
    case offsetof(FrskyTelemetryData, gpsLongitudeEW):
    case offsetof(FrskyTelemetryData, gpsLatitudeNS):
    case offsetof(FrskyTelemetryData, varioSpeed):
    case offsetof(FrskyTelemetryData, power): /* because sent by openXsensor */
    case offsetof(FrskyTelemetryData, vfas):
    case offsetof(FrskyTelemetryData, volts_bp):
      break;

    default:
      *((uint16_t *)(((uint8_t*)&telemetryData.hub) + structPos)) = previousValue;
      break;
#endif
  }
}
#endif  // #if defined(FRSKY_HUB)

#if defined(WS_HOW_HIGH)
void parseTelemWSHowHighByte(uint8_t byte)
{
  if (wshhStreaming < (WSHH_TIMEOUT10ms - 10)) {
    ((uint8_t*)&telemetryData.hub)[offsetof(FrskyTelemetryData, baroAltitude_bp)] = byte;
    checkMinMaxAltitude();
  }
  else {
    // At least 100mS passed since last data received
    ((uint8_t*)&telemetryData.hub)[offsetof(FrskyTelemetryData, baroAltitude_bp)+1] = byte;
  }
  // baroAltitude_bp unit here is feet!
  wshhStreaming = WSHH_TIMEOUT10ms; // reset counter
}
#endif

void frskyDProcessPacket(const uint8_t *packet)
{
  // What type of packet?
  switch (packet[0])
  {
    case LINKPKT: // A1/A2/RSSI values
    {
      telemetryData.analog[TELEM_ANA_A1].set(packet[1], g_model.frsky.channels[TELEM_ANA_A1].type);
      telemetryData.analog[TELEM_ANA_A2].set(packet[2], g_model.frsky.channels[TELEM_ANA_A2].type);
      telemetryData.rssi[0].set(packet[3]);
      telemetryData.rssi[1].set(packet[4] / 2);
      telemetryStreaming = TELEMETRY_TIMEOUT10ms; // reset counter only if valid packets are being detected
      link_counter += 256 / FRSKY_D_AVERAGING;
#if defined(VARIO)
      uint8_t varioSource = g_model.frsky.varioSource - VARIO_SOURCE_A1;
      if (varioSource < 2) {
        telemetryData.hub.varioSpeed = applyChannelRatio(varioSource, telemetryData.analog[varioSource].value);
      }
#endif
      break;
    }
#if defined(FRSKY_HUB) || defined (WS_HOW_HIGH)
    case USRPKT: // User Data packet
      uint8_t numBytes = 3 + (packet[1] & 0x07); // sanitize in case of data corruption leading to buffer overflow
      for (uint8_t i=3; i<numBytes; i++) {
#if defined(FRSKY_HUB)
        if (IS_USR_PROTO_FRSKY_HUB()) {
          parseTelemHubByte(packet[i]);
        }
#endif
#if defined(WS_HOW_HIGH)
        if (IS_USR_PROTO_WS_HOW_HIGH()) {
          parseTelemWSHowHighByte(packet[i]);
        }
#endif
      }
      break;
#endif  // #if defined(FRSKY_HUB) || defined (WS_HOW_HIGH)
  }
}

// Alarms level sent to the FrSky module

uint8_t frskyAlarmsSendState = 0 ;

void frskyPushValue(uint8_t *&ptr, uint8_t value)
{
  // byte stuff the only byte than might need it
  bool bytestuff = false;

  if (value == START_STOP) {
    bytestuff = true;
    value = 0x5e;
  }
  else if (value == BYTESTUFF) {
    bytestuff = true;
    value = 0x5d;
  }

  *ptr++ = value;
  if (bytestuff)
    *ptr = BYTESTUFF;
}

void frskySendPacket(uint8_t type, uint8_t value, uint8_t p1, uint8_t p2)
{
  uint8_t *ptr = &frskyTxBuffer[0];

  *ptr++ = START_STOP;        // End of packet
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = (IS_SOUND_OFF() ? alarm_off : p2);
  *ptr++ = p1;
  frskyPushValue(ptr, value);
  *ptr++ = type;
  *ptr++ = START_STOP; // Start of packet

  frskyTxBufferCount = ptr - &frskyTxBuffer[0];
#if !defined(SIMU)
  telemetryTransmitBuffer();
#endif
}

void frskyDSendNextAlarm(void)
{
  if (frskyTxBufferCount)
    return; // we only have one buffer. If it's in use, then we can't send yet.

  // Now send a packet
  frskyAlarmsSendState -= 1;
  uint8_t alarm = 1 - (frskyAlarmsSendState % 2);
  if (frskyAlarmsSendState < SEND_MODEL_ALARMS) {
    uint8_t channel = 1 - (frskyAlarmsSendState / 2);
    frskySendPacket(A22PKT + frskyAlarmsSendState, g_model.frsky.channels[channel].alarms_value[alarm], ALARM_GREATER(channel, alarm), ALARM_LEVEL(channel, alarm));
  }
  else {
    frskySendPacket(RSSI1PKT-alarm, getRssiAlarmValue(alarm), 0, (2+alarm+g_model.frsky.rssiAlarms[alarm].level) % 4);
  }
}
