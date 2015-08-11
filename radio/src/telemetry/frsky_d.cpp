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

// Enumerate FrSky packet codes
#define LINKPKT         0xfe
#define USRPKT          0xfd
#define A11PKT          0xfc
#define A12PKT          0xfb
#define A21PKT          0xfa
#define A22PKT          0xf9
#define ALRM_REQUEST    0xf8
#define RSSI1PKT        0xf7
#define RSSI2PKT        0xf6
#define RSSI_REQUEST    0xf1

#if !defined(CPUARM) && (defined(FRSKY_HUB) || defined(WS_HOW_HIGH))
void checkMinMaxAltitude()
{
  if (TELEMETRY_RELATIVE_BARO_ALT_BP > frskyData.hub.maxAltitude)
    frskyData.hub.maxAltitude = TELEMETRY_RELATIVE_BARO_ALT_BP;
  if (TELEMETRY_RELATIVE_BARO_ALT_BP < frskyData.hub.minAltitude)
    frskyData.hub.minAltitude = TELEMETRY_RELATIVE_BARO_ALT_BP;
}
#endif

#if defined(FRSKY_HUB)
typedef enum {
  TS_IDLE = 0,  // waiting for 0x5e frame marker
  TS_DATA_ID,   // waiting for dataID
  TS_DATA_LOW,  // waiting for data low byte
  TS_DATA_HIGH, // waiting for data high byte
  TS_XOR = 0x80 // decode stuffed byte
} TS_STATE;

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
#if defined(CPUARM)
      structPos = byte;
#else
      structPos = byte * 2;
#endif
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

#if defined(CPUARM)
  processHubPacket(structPos, (byte << 8) + lowByte);
#else

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
#endif
  
#if 0
  uint16_t previousValue = *((uint16_t *)(((uint8_t*)&frskyData.hub) + structPos));
#endif

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
      else if (frskyData.hub.gpsDistNeeded || g_menuStack[g_menuStackPtr] == menuTelemetryFrsky) {
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
#endif
}
#endif

#if defined(WS_HOW_HIGH) && !defined(CPUARM)
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

#if defined(CPUARM)
void frskyDProcessPacket(uint8_t *packet)
{
  // What type of packet?
  switch (packet[0])
  {
    case LINKPKT: // A1/A2/RSSI values
    {
      setTelemetryValue(TELEM_PROTO_FRSKY_D, D_A1_ID, 0, packet[1], UNIT_VOLTS, 0);
      setTelemetryValue(TELEM_PROTO_FRSKY_D, D_A2_ID, 0, packet[2], UNIT_VOLTS, 0);
      setTelemetryValue(TELEM_PROTO_FRSKY_D, D_RSSI_ID, 0, packet[3], UNIT_RAW, 0);
      frskyData.rssi.set(packet[3]);
      frskyStreaming = FRSKY_TIMEOUT10ms; // reset counter only if valid frsky packets are being detected
      break;
    }

    case USRPKT: // User Data packet
      uint8_t numBytes = 3 + (packet[1] & 0x07); // sanitize in case of data corruption leading to buffer overflow
      for (uint8_t i=3; i<numBytes; i++) {
        parseTelemHubByte(packet[i]);
      }
      break;
  }
}
#else
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
#endif
  }
}
#endif

#if !defined(PCBTARANIS)

// Alarms level sent to the FrSky module

uint8_t frskyAlarmsSendState = 0 ;

#if defined(CPUARM)
void frskyPushValue(uint8_t *&ptr, uint8_t value)
{
  // byte stuff the only byte than might need it
  if (value == START_STOP) {
    *ptr++ = BYTESTUFF;
    *ptr++ = 0x5e;
  }
  else if (value == BYTESTUFF) {
    *ptr++ = BYTESTUFF;
    *ptr++ = 0x5d;
  }
  else {
    *ptr++ = value;
  }
}

void frskyDSendNextAlarm(void)
{
#if defined(PCBSKY9X)
  if (telemetryTransmitPending()) {
    return; // we only have one buffer. If it's in use, then we can't send yet.
  }
#endif

#if 0
  uint8_t *ptr = &frskyTxBuffer[0];

  *ptr++ = START_STOP;        // Start of packet

  // Now send a packet
  frskyAlarmsSendState -= 1;
  uint8_t alarm = 1 - (frskyAlarmsSendState % 2);
  if (frskyAlarmsSendState < SEND_MODEL_ALARMS) {
    uint8_t channel = 1 - (frskyAlarmsSendState / 2);
    *ptr++ = (A22PKT + frskyAlarmsSendState); // fc - fb - fa - f9
    frskyPushValue(ptr, g_model.frsky.channels[channel].alarms_value[alarm]);
    *ptr++ = ALARM_GREATER(channel, alarm);
    *ptr++ = (IS_SOUND_OFF() ? alarm_off : ALARM_LEVEL(channel, alarm));
  }
  else {
    *ptr++ = (RSSI1PKT-alarm);  // f7 - f6
    frskyPushValue(ptr, getRssiAlarmValue(alarm));
    *ptr++ = 0x00;
    *ptr++ = (IS_SOUND_OFF() ? alarm_off : ((2+alarm+g_model.frsky.rssiAlarms[alarm].level) % 4));
  }

  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = 0x00;
  *ptr++ = START_STOP; // End of packet

  telemetryTransmitBuffer(frskyTxBuffer, ptr - &frskyTxBuffer[0]);
#elif !defined(WIN32)
#warning "FrSky module alarms removed!"
#endif
}

#else

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
#endif
#endif

// checks if an alarm is raised, not used any more
#if 0
bool isFrskyAlarmRaised(uint8_t idx)
{
  for (int i=0; i<2; i++) {
    if (ALARM_LEVEL(idx, i) != alarm_off) {
      if (ALARM_GREATER(idx, i)) {
        if (frskyData.analog[idx].value > g_model.frsky.channels[idx].alarms_value[i])
          return true;
      }
      else {
        if (frskyData.analog[idx].value < g_model.frsky.channels[idx].alarms_value[i])
          return true;
      }
    }
  }
  return false;
}
#endif

#if defined(CPUARM)
struct FrSkyDSensor {
  const uint8_t id;
  const char * name;
  const TelemetryUnit unit;
  const uint8_t prec;
};

const FrSkyDSensor frskyDSensors[] = {
  { D_RSSI_ID, ZSTR_RSSI, UNIT_RAW, 0 },
  { D_A1_ID, ZSTR_A1, UNIT_VOLTS, 1 },
  { D_A2_ID, ZSTR_A2, UNIT_VOLTS, 1 },
  { RPM_ID, ZSTR_RPM, UNIT_RPMS, 0 },
  { FUEL_ID, ZSTR_FUEL, UNIT_PERCENT, 0 },
  { TEMP1_ID, ZSTR_TEMP, UNIT_CELSIUS, 0 },
  { TEMP2_ID, ZSTR_TEMP, UNIT_CELSIUS, 0 },
  { CURRENT_ID, ZSTR_CURR, UNIT_AMPS, 1 },
  { ACCEL_X_ID, ZSTR_ACCX, UNIT_G, 3 },
  { ACCEL_Y_ID, ZSTR_ACCY, UNIT_G, 3 },
  { ACCEL_Z_ID, ZSTR_ACCZ, UNIT_G, 3 },
  { VARIO_ID, ZSTR_VSPD, UNIT_METERS_PER_SECOND, 2 },
  { VFAS_ID, ZSTR_VFAS, UNIT_VOLTS, 2 },
  { BARO_ALT_AP_ID, ZSTR_ALT, UNIT_METERS, 1 },   // we map hi precision vario into PREC1!
  { VOLTS_AP_ID, ZSTR_VFAS, UNIT_VOLTS, 2 },
  { GPS_SPEED_BP_ID, ZSTR_GSPD, UNIT_KTS, 0 },
  { GPS_COURS_BP_ID, ZSTR_HDG, UNIT_DEGREE, 0 },
  { VOLTS_ID, ZSTR_CELLS, UNIT_CELLS, 2 },
  { GPS_ALT_BP_ID, ZSTR_GPSALT, UNIT_METERS, 0 },
  { GPS_HOUR_MIN_ID, ZSTR_GPSDATETIME, UNIT_DATETIME, 0 },
  { GPS_LAT_AP_ID, ZSTR_GPS, UNIT_GPS, 0 },
  { 0, NULL, UNIT_RAW, 0 } // sentinel
};

const FrSkyDSensor * getFrSkyDSensor(uint8_t id)
{
  const FrSkyDSensor * result = NULL;
  for (const FrSkyDSensor * sensor = frskyDSensors; sensor->id; sensor++) {
    if (id == sensor->id) {
      result = sensor;
      break;
    }
  }
  return result;
}

void processHubPacket(uint8_t id, int16_t value)
{
  static uint8_t lastId = 0;
  static uint16_t lastValue = 0;
  TelemetryUnit unit = UNIT_RAW;
  uint8_t precision = 0;
  int32_t data = value;

  if (id > FRSKY_LAST_ID || id == GPS_SPEED_AP_ID || id == GPS_ALT_AP_ID || id == GPS_COURS_AP_ID) {
    return;
  }

  if (id == GPS_LAT_BP_ID || id == GPS_LONG_BP_ID || id == BARO_ALT_BP_ID || id == VOLTS_BP_ID) {
    lastId = id;
    lastValue = value;
    return;
  }

  if (id == GPS_LAT_AP_ID) {
    if (lastId == GPS_LAT_BP_ID) {
      data += lastValue << 16;
      unit = UNIT_GPS_LATITUDE;
    }
    else {
      return;
    }
  }
  else if (id == GPS_LONG_AP_ID) {
    if (lastId == GPS_LONG_BP_ID) {
      data += lastValue << 16;
      id = GPS_LAT_AP_ID;
      unit = UNIT_GPS_LONGITUDE;
    }
    else {
      return;
    }
  }
  else if (id == GPS_LAT_NS_ID) {
    id = GPS_LAT_AP_ID;
    unit = UNIT_GPS_LATITUDE_NS;
  }
  else if (id == GPS_LONG_EW_ID) {
    id = GPS_LAT_AP_ID;
    unit = UNIT_GPS_LONGITUDE_EW;
  }
  else if (id == BARO_ALT_AP_ID) {
    if (lastId == BARO_ALT_BP_ID) {
      if (data > 9 || frskyData.varioHighPrecision) {
        frskyData.varioHighPrecision = true;
        data /= 10;    // map hi precision vario into low precision. Altitude is stored in 0.1m anyways
      }
      data = (int16_t)lastValue * 10 + (((int16_t)lastValue < 0) ? -data : data);
      unit = UNIT_METERS;
      precision = 1;
    }
    else {
      return;
    }
  }
  else if (id == VOLTS_AP_ID) {
    if (lastId == VOLTS_BP_ID) {
      data = ((lastValue * 100 + value * 10) * 210) / 110;
      unit = UNIT_VOLTS;
      precision = 2;
    }
    else {
      return;
    }
  }
  else if (id == VOLTS_ID) {
    unit = UNIT_CELLS;
    uint32_t cellData = (uint32_t)data;
    data = ((cellData & 0x00F0) << 12) + (((((cellData & 0xFF00) >> 8) + ((cellData & 0x000F) << 8))) / 5);
  }
  else if (id == GPS_DAY_MONTH_ID) {
    id = GPS_HOUR_MIN_ID;
    unit = UNIT_DATETIME_DAY_MONTH;
  }
  else if (id == GPS_HOUR_MIN_ID) {
    unit = UNIT_DATETIME_HOUR_MIN;
  }
  else if (id == GPS_SEC_ID) {
    id = GPS_HOUR_MIN_ID;
    unit = UNIT_DATETIME_SEC;
  }
  else if (id == GPS_YEAR_ID) {
    id = GPS_HOUR_MIN_ID;
    unit = UNIT_DATETIME_YEAR;
  }
  else {
    const FrSkyDSensor * sensor = getFrSkyDSensor(id);
    if (sensor) {
      unit = sensor->unit;
      precision = sensor->prec;
    }
  }
  if (id == RPM_ID) {
    data = data * 60;
  }
  else if (id == VFAS_ID) {
    if (data >= VFAS_D_HIPREC_OFFSET) {
      // incoming value has a resolution of 0.01V and added offset of VFAS_D_HIPREC_OFFSET
      data -= VFAS_D_HIPREC_OFFSET;
    }
    else {
      // incoming value has a resolution of 0.1V
      data *= 10;
    }
  }
  
  setTelemetryValue(TELEM_PROTO_FRSKY_D, id, 0, data, unit, precision);
}

void frskyDSetDefault(int index, uint16_t id)
{
  TelemetrySensor & telemetrySensor = g_model.telemetrySensors[index];

  telemetrySensor.id = id;
  telemetrySensor.instance = 0;

  const FrSkyDSensor * sensor = getFrSkyDSensor(id);
  if (sensor) {
    TelemetryUnit unit = sensor->unit;
    uint8_t prec = min<uint8_t>(2, sensor->prec);
    telemetrySensor.init(sensor->name, unit, prec);
    if (id == D_RSSI_ID) {
      telemetrySensor.filter = 1;
      telemetrySensor.logs = true;
    }
    else if (id >= D_A1_ID && id <= D_A2_ID) {
      telemetrySensor.custom.ratio = 132;
      telemetrySensor.filter = 1;
    }
    else if (id == CURRENT_ID) {
      telemetrySensor.onlyPositive = 1;
    }
    else if (id == BARO_ALT_AP_ID) {
      telemetrySensor.autoOffset = 1;
    }
    if (unit == UNIT_RPMS) {
      telemetrySensor.custom.ratio = 1;
      telemetrySensor.custom.offset = 1;
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

  eeDirty(EE_MODEL);
}
#endif
