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

#define HOTT_TELEM_NORMAL 0x00
#define HOTT_TELEM_BIND 0x40
#define HOTT_TELEM_MENU 0x80

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
  //frame 00
  {HOTT_ID_RX_VOLTAGE,   ZSTR_BATT,       UNIT_VOLTS,             1},  // RX_Batt Voltage
  {HOTT_ID_TEMP1,        ZSTR_TEMP1,      UNIT_CELSIUS,           0},  // RX Temperature sensor
  //frame 01
  //frame 02
  //frame 03
  //frame 04
  {HOTT_TX_RSSI_ID,      ZSTR_TX_RSSI,    UNIT_RAW,               0},  // Pseudo id outside 1 byte range of Hott sensors
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
  setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, HOTT_TX_RSSI_ID, 0, 0, packet[0]>>1, UNIT_RAW, 0);
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
  //     packet[28]= 0xXF=>no key press, 0xXD=>down, 0xXB=>up, 0xX9=>enter, 0xXE=>right, 0xX7=>left with X=0 or D
  //     packet[29]= 0xX1/0xX9 with X=0 or X counting 0,1,1,2,2,..,9,9
  if (Multi_Buffer && memcmp(Multi_Buffer, "HoTT", 4) == 0) {
    // HoTT Lua script is running
    if (Multi_Buffer[4] == 0xFF) {
      // Init
      memset(&Multi_Buffer[6], ' ', HOTT_MENU_NBR_PAGE * 9); // Clear text buffer
    }
    
    if (packet[3] < HOTT_MENU_NBR_PAGE && Multi_Buffer[5] >= 0xD7 && Multi_Buffer[5] <= 0xDF) {
      Multi_Buffer[4] = packet[4];                             // Store current menu being received
      memcpy(&Multi_Buffer[6 + packet[3] * 9], &packet[5], 9); // Store the received page in the buffer
    }
    return;
  }
#endif

  const HottSensor * sensor;
  int32_t value;

  if (packet[2] == HOTT_TELEM_NORMAL) {
    switch (packet[3]) {
      case HOTT_FRAME_00:
        // RX_VOLT
        value = packet[5];
        sensor = getHottSensor(HOTT_ID_RX_VOLTAGE);
        setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, HOTT_ID_RX_VOLTAGE, 0, 0, value, sensor->unit, sensor->precision);
        // RX_TEMP
        value = packet[6] - 20;
        sensor = getHottSensor(HOTT_ID_TEMP1);
        setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, HOTT_ID_TEMP1, 0, 0, value, sensor->unit, sensor->precision);
        // RX_RSSI
        setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, HOTT_RX_RSSI_ID, 0, 0, packet[7], UNIT_DB, 0);
        // RX_LQI
        telemetryData.rssi.set(packet[8]);
        if (packet[8] > 0)
          telemetryStreaming = TELEMETRY_TIMEOUT10ms;
        setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, HOTT_RX_LQI_ID, 0, 0, packet[8], UNIT_RAW, 0);
        break;

      default:
        // unknown
        value = (packet[7] << 24) | (packet[6] << 16) | (packet[5] << 8) | packet[4];
        setTelemetryValue(PROTOCOL_TELEMETRY_HOTT, 0xFE00 | packet[3], 0, 0, value, UNIT_RAW, 0);
        break;
    }
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
