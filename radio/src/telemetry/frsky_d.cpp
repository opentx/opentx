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

#include "../opentx.h"

#if (defined(FRSKY_HUB) || defined(WS_HOW_HIGH))
void checkMinMaxAltitude()
{
  if (TELEMETRY_RELATIVE_BARO_ALT_BP > frskyData.hub.maxAltitude)
    frskyData.hub.maxAltitude = TELEMETRY_RELATIVE_BARO_ALT_BP;
  if (TELEMETRY_RELATIVE_BARO_ALT_BP < frskyData.hub.minAltitude)
    frskyData.hub.minAltitude = TELEMETRY_RELATIVE_BARO_ALT_BP;
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
  if ((uint8_t)structPos == offsetof(FrskySerialData, gpsLatitude_bp)) {
    if (lowByte || byte)
      frskyData.hub.gpsFix = 1;
    else if (frskyData.hub.gpsFix > 0 && frskyData.hub.gpsLatitude_bp > 1)
      frskyData.hub.gpsFix = 0;
  }
  else if ((uint8_t)structPos == offsetof(FrskySerialData, gpsLongitude_bp)) {
    if (lowByte || byte)
      frskyData.hub.gpsFix = 1;
    else if (frskyData.hub.gpsFix > 0 && frskyData.hub.gpsLongitude_bp > 1)
      frskyData.hub.gpsFix = 0;
  }
  
  if ((uint8_t)structPos == offsetof(FrskySerialData, gpsAltitude_bp) ||
      ((uint8_t)structPos >= offsetof(FrskySerialData, gpsAltitude_ap) && (uint8_t)structPos <= offsetof(FrskySerialData, gpsLatitudeNS) && (uint8_t)structPos != offsetof(FrskySerialData, baroAltitude_bp) && (uint8_t)structPos != offsetof(FrskySerialData, baroAltitude_ap))) {
    // If we don't have a fix, we may discard the value
    if (frskyData.hub.gpsFix <= 0)
      return;
  }
#endif  // #if defined(GPS)

  ((uint8_t*)&frskyData.hub)[structPos] = lowByte;
  ((uint8_t*)&frskyData.hub)[structPos+1] = byte;

  switch ((uint8_t)structPos) {

    case offsetof(FrskySerialData, rpm):
      frskyData.hub.rpm *= (uint8_t)60/(g_model.frsky.blades+2);
      if (frskyData.hub.rpm > frskyData.hub.maxRpm)
        frskyData.hub.maxRpm = frskyData.hub.rpm;
      break;

    case offsetof(FrskySerialData, temperature1):
      if (frskyData.hub.temperature1 > frskyData.hub.maxTemperature1)
        frskyData.hub.maxTemperature1 = frskyData.hub.temperature1;
      break;

    case offsetof(FrskySerialData, temperature2):
      if (frskyData.hub.temperature2 > frskyData.hub.maxTemperature2)
        frskyData.hub.maxTemperature2 = frskyData.hub.temperature2;
      break;

    case offsetof(FrskySerialData, current):
#if defined(FAS_OFFSET) || !defined(CPUM64)
      if ((int16_t)frskyData.hub.current > 0 && ((int16_t)frskyData.hub.current + g_model.frsky.fasOffset) > 0)
        frskyData.hub.current += g_model.frsky.fasOffset;
      else
        frskyData.hub.current = 0;
#endif
      if (frskyData.hub.current > frskyData.hub.maxCurrent)
        frskyData.hub.maxCurrent = frskyData.hub.current;
      break;

    case offsetof(FrskySerialData, currentConsumption):
      // we receive data from openXsensor. stops the calculation of consumption and power
      frskyData.hub.openXsensor = 1;
      break;

    case offsetof(FrskySerialData, volts_ap):
#if defined(FAS_BSS)
      frskyData.hub.vfas = (frskyData.hub.volts_bp * 10 + frskyData.hub.volts_ap);
#else
      frskyData.hub.vfas = ((frskyData.hub.volts_bp * 100 + frskyData.hub.volts_ap * 10) * 21) / 110;
#endif
      /* TODO later if (!frskyData.hub.minVfas || frskyData.hub.minVfas > frskyData.hub.vfas)
        frskyData.hub.minVfas = frskyData.hub.vfas; */
      break;

    case offsetof(FrskySerialData, baroAltitude_bp):
      // First received barometer altitude => Altitude offset
      if (!frskyData.hub.baroAltitudeOffset)
        frskyData.hub.baroAltitudeOffset = -frskyData.hub.baroAltitude_bp;
      frskyData.hub.baroAltitude_bp += frskyData.hub.baroAltitudeOffset;
      checkMinMaxAltitude();
      break;

#if defined(GPS)
    case offsetof(FrskySerialData, gpsAltitude_ap):
      if (!frskyData.hub.gpsAltitudeOffset)
        frskyData.hub.gpsAltitudeOffset = -frskyData.hub.gpsAltitude_bp;
      frskyData.hub.gpsAltitude_bp += frskyData.hub.gpsAltitudeOffset;
      if (!frskyData.hub.baroAltitudeOffset) {
        if (frskyData.hub.gpsAltitude_bp > frskyData.hub.maxAltitude)
          frskyData.hub.maxAltitude = frskyData.hub.gpsAltitude_bp;
        if (frskyData.hub.gpsAltitude_bp < frskyData.hub.minAltitude)
          frskyData.hub.minAltitude = frskyData.hub.gpsAltitude_bp;
      }
      if (!frskyData.hub.pilotLatitude && !frskyData.hub.pilotLongitude) {
        // First received GPS position => Pilot GPS position
        getGpsPilotPosition();
      }
      else if (frskyData.hub.gpsDistNeeded || menuHandlers[menuLevel] == menuTelemetryFrsky) {
        getGpsDistance();
      }
      break;

    case offsetof(FrskySerialData, gpsSpeed_bp):
      // Speed => Max speed
      if (frskyData.hub.gpsSpeed_bp > frskyData.hub.maxGpsSpeed)
        frskyData.hub.maxGpsSpeed = frskyData.hub.gpsSpeed_bp;
      break;
#endif

    case offsetof(FrskySerialData, volts):
      frskyUpdateCells();
      break;

#if defined(GPS)
    case offsetof(FrskySerialData, hour):
      frskyData.hub.hour = ((uint8_t)(frskyData.hub.hour + g_eeGeneral.timezone + 24)) % 24;
      break;
#endif

    case offsetof(FrskySerialData, accelX):
    case offsetof(FrskySerialData, accelY):
    case offsetof(FrskySerialData, accelZ):
      *(int16_t*)(&((uint8_t*)&frskyData.hub)[structPos]) /= 10;
      break;

#if 0
    case offsetof(FrskySerialData, gpsAltitude_bp):
    case offsetof(FrskySerialData, fuelLevel):
    case offsetof(FrskySerialData, gpsLongitude_bp):
    case offsetof(FrskySerialData, gpsLatitude_bp):
    case offsetof(FrskySerialData, gpsCourse_bp):
    case offsetof(FrskySerialData, day):
    case offsetof(FrskySerialData, year):
    case offsetof(FrskySerialData, sec):
    case offsetof(FrskySerialData, gpsSpeed_ap):
    case offsetof(FrskySerialData, gpsLongitude_ap):
    case offsetof(FrskySerialData, gpsLatitude_ap):
    case offsetof(FrskySerialData, gpsCourse_ap):
    case offsetof(FrskySerialData, gpsLongitudeEW):
    case offsetof(FrskySerialData, gpsLatitudeNS):
    case offsetof(FrskySerialData, varioSpeed):
    case offsetof(FrskySerialData, power): /* because sent by openXsensor */
    case offsetof(FrskySerialData, vfas):
    case offsetof(FrskySerialData, volts_bp):
      break;

    default:
      *((uint16_t *)(((uint8_t*)&frskyData.hub) + structPos)) = previousValue;
      break;
#endif
  }
}
#endif  // #if defined(FRSKY_HUB)

#if defined(WS_HOW_HIGH)
void parseTelemWSHowHighByte(uint8_t byte)
{
  if (frskyUsrStreaming < (WSHH_TIMEOUT10ms - 10)) {
    ((uint8_t*)&frskyData.hub)[offsetof(FrskySerialData, baroAltitude_bp)] = byte;
    checkMinMaxAltitude();
  }
  else {
    // At least 100mS passed since last data received
    ((uint8_t*)&frskyData.hub)[offsetof(FrskySerialData, baroAltitude_bp)+1] = byte;
  }
  // baroAltitude_bp unit here is feet!
  frskyUsrStreaming = WSHH_TIMEOUT10ms; // reset counter
}
#endif

void frskyDProcessPacket(uint8_t *packet)
{
  // What type of packet?
  switch (packet[0])
  {
    case LINKPKT: // A1/A2/RSSI values
    {
      frskyData.analog[TELEM_ANA_A1].set(packet[1], g_model.frsky.channels[TELEM_ANA_A1].type);
      frskyData.analog[TELEM_ANA_A2].set(packet[2], g_model.frsky.channels[TELEM_ANA_A2].type);
      frskyData.rssi[0].set(packet[3]);
      frskyData.rssi[1].set(packet[4] / 2);
      frskyStreaming = FRSKY_TIMEOUT10ms; // reset counter only if valid frsky packets are being detected
      link_counter += 256 / FRSKY_D_AVERAGING;
#if defined(VARIO)
      uint8_t varioSource = g_model.frsky.varioSource - VARIO_SOURCE_A1;
      if (varioSource < 2) {
        frskyData.hub.varioSpeed = applyChannelRatio(varioSource, frskyData.analog[varioSource].value);
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

inline void frskyDSendNextAlarm(void)
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
