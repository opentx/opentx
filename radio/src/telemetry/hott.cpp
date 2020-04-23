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
packet[0]    = TX RSSI value
packet[1]    = TX LQI value
packet[2]    = Telemetry type
packet[3]    = Telemetry page
packet[4-13] = Telemetry data

Telemetry type: 0x00 RX, 0x09 Vario, 0x0A GPS, 0x0B not seen, 0x0C ESC, 0x0D GAM, 0x0E EAM, 0x80 Text config mode
Telemetry page: 0x00..0x04 for all telemetry except Text config mode which is 0x00..0x12. The pages can be present or not, they also do not have to follow each others.

Here is a description of the RX telemetry:
 [2] = 0x00 -> Telemetry type RX
 [3] = 0x00 -> Telemetry page 0
 [4] = ?? looks to be a copy of [7]
 [5] = RX_Voltage*10 in V
 [6] = Temperature-20 in °C
 [7] = RX_RSSI CC2500 formated (a<128:a/2-71dBm, a>=128:(a-256)/2-71dBm)
 [8] = RX_LQI in %
 [9] = RX_MIN_Voltage*10 in V
 [10,11] = [11]*256+[10]=max lost packet time in ms, max value seems 2s=0x7D0
 [12] = 0x00 ??
 [13] = ?? looks to be a copy of [7]
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
  HOTT_PAGE_00 = 0x00,
  HOTT_PAGE_01 = 0x01,
  HOTT_PAGE_02 = 0x02,
  HOTT_PAGE_03 = 0x03,
  HOTT_PAGE_04 = 0x04,
};

enum
{
  HOTT_TELEM_RX    = 0x00,
  HOTT_TELEM_VARIO = 0x09,
  HOTT_TELEM_GPS   = 0x0A,
  HOTT_TELEM_ESC   = 0x0C,
  HOTT_TELEM_GAM   = 0x0D,
  HOTT_TELEM_EAM   = 0x0E,
  HOTT_TELEM_TEXT  = 0x80,
};

// telemetry sensors ID
enum
{
  HOTT_ID_RX_VOLTAGE = 0x0004,  // RX_Batt Voltage
  HOTT_ID_TEMP1      = 0x0005,  // RX Temperature sensor
  HOTT_TX_RSSI_ID    = 0xFF00,  // Pseudo id outside 1 byte range of Hott sensors
  HOTT_TX_LQI_ID     = 0xFF01,  // Pseudo id outside 1 byte range of Hott sensors
  HOTT_RX_RSSI_ID    = 0xFF02,  // Pseudo id outside 1 byte range of Hott sensors
  HOTT_RX_LQI_ID     = 0xFF03,  // Pseudo id outside 1 byte range of Hott sensors
};

const HottSensor hottSensors[] = {
  //HOTT_TELEM_RX
  {HOTT_ID_RX_VOLTAGE,   ZSTR_BATT,       UNIT_VOLTS,             1},  // RX_Batt Voltage
  {HOTT_ID_TEMP1,        ZSTR_TEMP1,      UNIT_CELSIUS,           0},  // RX Temperature sensor

  {HOTT_TX_RSSI_ID,      ZSTR_TX_RSSI,    UNIT_DB,                0},  // Pseudo id outside 1 byte range of Hott sensors
  {HOTT_TX_LQI_ID,       ZSTR_TX_QUALITY, UNIT_RAW,               0},  // Pseudo id outside 1 byte range of Hott sensors
  {HOTT_RX_RSSI_ID,      ZSTR_RSSI,       UNIT_DB,                0},  // RX RSSI
  {HOTT_RX_LQI_ID,       ZSTR_RX_QUALITY, UNIT_RAW,               0},  // RX LQI
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
  int16_t dBm=packet[0];
  if(dBm>=128) dBm=256-dBm;
  dBm=dBm/2-71;
  setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, HOTT_TX_RSSI_ID, 0, 0, dBm, UNIT_RAW, 0);
  // Set TX LQI  Value
  setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, HOTT_TX_LQI_ID, 0, 0, packet[1], UNIT_RAW, 0);

#if defined(LUA)
  #define HOTT_MENU_NBR_PAGE 0x13
  // Config menu consists of the different telem pages put all together
  //   [3] = config mennu page 0x00 to 0x12.
  //   Page X [4] = seems like all the telem pages with the same value are going together to make the full config menu text. Seen so far 'a', 'b', 'c', 'd'
  //   Page X [5..13] = 9 ascii chars to be displayed, char is highlighted when ascii|0x80
  //   Screen display is 21 characters large which means that once the first 21 chars are filled go to the begining of the next line
  //   Menu commands are sent through TX packets:
  //     packet[28]= 0xXF=>no key press, 0xXD=>down, 0xXB=>up, 0xX9=>enter, 0xXE=>right, 0xX7=>left with X=0, 9=Vario, A=GPS, B=Unknown, C=ESC, D=GAM, or E=EAM
  //     packet[29]= 0xX1/0xX9 with X=0 or X counting 0,1,1,2,2,..,9,9
  if (Multi_Buffer && memcmp(Multi_Buffer, "HoTT", 4) == 0) {
    // HoTT Lua script is running
    if (Multi_Buffer[4] == 0xFF) {
      // Init
      memset(&Multi_Buffer[6], ' ', HOTT_MENU_NBR_PAGE * 9); // Clear text buffer
    }
    
    if (packet[2]==HOTT_TELEM_TEXT && packet[3] < HOTT_MENU_NBR_PAGE && (Multi_Buffer[5]&0xF0) >= 0x90 && (Multi_Buffer[5]&0x0F) >= 0x07) {
      Multi_Buffer[4] = packet[4];                             // Store current menu being received
      memcpy(&Multi_Buffer[6 + packet[3] * 9], &packet[5], 9); // Store the received page in the buffer
    }
    return;
  }
#endif

  const HottSensor * sensor;
  int32_t value;

  switch (packet[2]) { // Telemetry type
    case HOTT_TELEM_RX:
      if(packet[3]==0)
      { // Telemetry page: only page 0 is for RX
          // RX_VOLT
        value = packet[5];
        sensor = getHottSensor(HOTT_ID_RX_VOLTAGE);
        setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, HOTT_ID_RX_VOLTAGE, 0, 0, value, sensor->unit, sensor->precision);
        // RX_TEMP
        value = packet[6] - 20;
        sensor = getHottSensor(HOTT_ID_TEMP1);
        setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, HOTT_ID_TEMP1, 0, 0, value, sensor->unit, sensor->precision);
        // RX_RSSI
        dBm=packet[7];
        if(dBm>=128) dBm=256-dBm;
        dBm=dBm/2-71;
        setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, HOTT_RX_RSSI_ID, 0, 0, dBm, UNIT_DB, 0);
        // RX_LQI
        telemetryData.rssi.set(packet[8]);
        if (packet[8] > 0)
          telemetryStreaming = TELEMETRY_TIMEOUT10ms;
        setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, HOTT_RX_LQI_ID, 0, 0, packet[8], UNIT_RAW, 0);
      }
      break;
    case HOTT_TELEM_VARIO:
      // https://github.com/betaflight/betaflight/blob/1d8a0e9fd61cf01df7b34805e84365df72d9d68d/src/main/telemetry/hott.h#L240
      switch (packet[3]) { // Telemetry page 1,2,3,4
        case HOTT_PAGE_01:
          break;
        case HOTT_PAGE_02:
          break;
        case HOTT_PAGE_03:
          break;
        case HOTT_PAGE_04:
          break;
      }
      break;
    case HOTT_TELEM_GPS:
      // https://github.com/betaflight/betaflight/blob/1d8a0e9fd61cf01df7b34805e84365df72d9d68d/src/main/telemetry/hott.h#L378
      switch (packet[3]) { // Telemetry page 1,2,3,4
        case HOTT_PAGE_01:
          break;
        case HOTT_PAGE_02:
          break;
        case HOTT_PAGE_03:
          break;
        case HOTT_PAGE_04:
          break;
      }
      break;
    case HOTT_TELEM_ESC:
      // https://github.com/betaflight/betaflight/blob/1d8a0e9fd61cf01df7b34805e84365df72d9d68d/src/main/telemetry/hott.h#L454
      switch (packet[3]) { // Telemetry page 1,2,3,4
        case HOTT_PAGE_01:
          // packet[4 ] uint8_t sensor_id;      //#04 constant value 0xc0
          // packet[5 ] uint8_t alarm_invers1;  //#05 TODO: more info
          // packet[6 ] uint8_t alarm_invers2;  //#06 TODO: more info
          // packet[7 ] uint8_t input_v_L;      //#07 Input voltage low byte
          // packet[8 ] uint8_t input_v_H;      //#08
          // packet[9 ] uint8_t input_v_min_L;  //#09 Input min. voltage low byte
          // packet[10] uint8_t input_v_min_H;  //#10
          // packet[11] uint8_t batt_cap_L;     //#11 battery capacity in 10mAh steps
          // packet[12] uint8_t batt_cap_H;     //#12
          // packet[13] uint8_t esc_temp;       //#13 ESC temperature
          break;
        case HOTT_PAGE_02:
          // packet[4 ] uint8_t esc_max_temp;   //#14 ESC max. temperature
          // packet[5 ] uint8_t current_L;      //#15 Current in 0.1 steps
          // packet[6 ] uint8_t current_H;      //#16
          // packet[7 ] uint8_t current_max_L;  //#17 Current max. in 0.1 steps
          // packet[8 ] uint8_t current_max_H;  //#18
          // packet[9 ] uint8_t rpm_L;          //#19 RPM in 10U/min steps
          // packet[10] uint8_t rpm_H;          //#20
          // packet[11] uint8_t rpm_max_L;      //#21 RPM max
          // packet[12] uint8_t rpm_max_H;      //#22
          // packet[13] uint8_t throttle;       //#23 throttle in %
          break;
        case HOTT_PAGE_03:
          break;
        case HOTT_PAGE_04:
          break;
      }
      break;
    case HOTT_TELEM_GAM:
      // https://github.com/betaflight/betaflight/blob/1d8a0e9fd61cf01df7b34805e84365df72d9d68d/src/main/telemetry/hott.h#L151
      switch (packet[3]) { // Telemetry page 1,2,3,4
        case HOTT_PAGE_01:
          break;
        case HOTT_PAGE_02:
          break;
        case HOTT_PAGE_03:
          break;
        case HOTT_PAGE_04:
          break;
      }
      break;
    case HOTT_TELEM_EAM:
      // https://github.com/betaflight/betaflight/blob/1d8a0e9fd61cf01df7b34805e84365df72d9d68d/src/main/telemetry/hott.h#L288
      switch (packet[3]) { // Telemetry page 1,2,3,4
        case HOTT_PAGE_01:
          break;
        case HOTT_PAGE_02:
          break;
        case HOTT_PAGE_03:
          break;
        case HOTT_PAGE_04:
          break;
      }
      break;
  }
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
