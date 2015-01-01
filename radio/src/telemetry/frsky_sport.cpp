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

struct FrSkySportSensor {
  const uint16_t firstId;
  const uint16_t lastId;
  const char * name;
  const TelemetryUnit unit;
  const uint8_t prec;
};

const FrSkySportSensor sportSensors[] = {
  { RSSI_ID, RSSI_ID, ZSTR_RSSI, UNIT_RAW, 0 },
  { ADC1_ID, ADC1_ID, ZSTR_A1, UNIT_VOLTS, 0 },
  { ADC2_ID, ADC2_ID, ZSTR_A2, UNIT_VOLTS, 0 },
  { BATT_ID, BATT_ID, ZSTR_BATT, UNIT_VOLTS, 0 },
  { T1_FIRST_ID, T2_LAST_ID, ZSTR_TEMP, UNIT_CELSIUS, 0 },
  { RPM_FIRST_ID, RPM_LAST_ID, ZSTR_RPM, UNIT_RPMS, 0 },
  { FUEL_FIRST_ID, FUEL_LAST_ID, ZSTR_FUEL, UNIT_PERCENT, 0 },
  { ALT_FIRST_ID, ALT_LAST_ID, ZSTR_ALT, UNIT_METERS, 2 },
  { VARIO_FIRST_ID, VARIO_LAST_ID, ZSTR_VSPD, UNIT_METERS_PER_SECOND, 2 },
  { ACCX_FIRST_ID, ACCX_LAST_ID, ZSTR_ACCX, UNIT_G, 2 },
  { ACCY_FIRST_ID, ACCY_LAST_ID, ZSTR_ACCY, UNIT_G, 2 },
  { ACCZ_FIRST_ID, ACCZ_LAST_ID, ZSTR_ACCZ, UNIT_G, 2 },
  { CURR_FIRST_ID, CURR_LAST_ID, ZSTR_CURR, UNIT_AMPS, 1 },
  { VFAS_FIRST_ID, VFAS_LAST_ID, ZSTR_VFAS, UNIT_VOLTS, 2 },
  { AIR_SPEED_FIRST_ID, AIR_SPEED_LAST_ID, ZSTR_ASPD, UNIT_METERS_PER_SECOND, 1 },
  { GPS_SPEED_FIRST_ID, GPS_SPEED_LAST_ID, ZSTR_GSPD, UNIT_KTS, 3 },
  { CELLS_FIRST_ID, CELLS_LAST_ID, ZSTR_CELLS, UNIT_CELLS, 2 },
  { GPS_ALT_FIRST_ID, GPS_ALT_LAST_ID, ZSTR_GPSALT, UNIT_METERS, 2 },
  { GPS_TIME_DATE_FIRST_ID, GPS_TIME_DATE_LAST_ID, ZSTR_GPSDATETIME, UNIT_DATETIME, 0 },
  { GPS_LONG_LATI_FIRST_ID, GPS_LONG_LATI_LAST_ID, ZSTR_GPS, UNIT_GPS, 0 },
  { 0, 0, NULL, UNIT_RAW, 0 } // sentinel
};

const FrSkySportSensor * getFrSkySportSensor(uint16_t id)
{
  const FrSkySportSensor * result = NULL;
  for (const FrSkySportSensor * sensor = sportSensors; sensor->firstId; sensor++) {
    if (id >= sensor->firstId && id <= sensor->lastId) {
      result = sensor;
      break;
    }
  }
  return result;
}

bool checkSportPacket(uint8_t *packet)
{
  short crc = 0;
  for (int i=1; i<FRSKY_SPORT_PACKET_SIZE; ++i) {
    crc += packet[i]; // 0-1FE
    crc += crc >> 8;  // 0-1FF
    crc &= 0x00ff;    // 0-FF
  }
  return (crc == 0x00ff);
}

#define SPORT_DATA_U8(packet)   (packet[4])
#define SPORT_DATA_S32(packet)  (*((int32_t *)(packet+4)))
#define SPORT_DATA_U32(packet)  (*((uint32_t *)(packet+4)))
#define HUB_DATA_U16(packet)    (*((uint16_t *)(packet+4)))

void processSportPacket(uint8_t *packet)
{
  uint8_t  dataId = packet[0];
  uint8_t  prim   = packet[1];
  uint16_t appId  = *((uint16_t *)(packet+2));

  if (!checkSportPacket(packet)) {
    return;
  }

  if (prim == DATA_FRAME)  {
    uint32_t data = SPORT_DATA_S32(packet);

    if (appId == RSSI_ID) {
      frskyStreaming = FRSKY_TIMEOUT10ms; // reset counter only if valid frsky packets are being detected
      data = SPORT_DATA_U8(packet);
      frskyData.rssi.set(data);
    }

    else if (appId == SWR_ID) {
#if defined(SWR)
      data = SPORT_DATA_U8(packet);
      frskyData.swr.set(data);
#else
      // SWR skipped (not reliable on Taranis+)
      return;
#endif
    }

    if (TELEMETRY_STREAMING()/* because when Rx is OFF it happens that some old A1/A2 values are sent from the XJT module*/) {
      if ((appId >> 8) == 0) {
        // The old FrSky IDs
        uint8_t  id = (uint8_t)appId;
        uint16_t value = HUB_DATA_U16(packet);
        processHubPacket(id, value);
      }
      else {
        if (appId == ADC1_ID || appId == ADC2_ID || appId == BATT_ID) {
          data = SPORT_DATA_U8(packet);
        }
        const FrSkySportSensor * sensor = getFrSkySportSensor(appId);
        TelemetryUnit unit = UNIT_RAW;
        uint8_t precision = 0;
        if (sensor) {
          unit = sensor->unit;
          precision = sensor->prec;
        }
        setTelemetryValue(TELEM_PROTO_FRSKY_SPORT, appId, dataId, data, unit, precision);
      }
    }
  }
}

void frskySportSetDefault(int index, uint16_t id, uint8_t instance)
{
  TelemetrySensor & telemetrySensor = g_model.telemetrySensors[index];

  telemetrySensor.id = id;
  telemetrySensor.instance = instance;

  const FrSkySportSensor * sensor = getFrSkySportSensor(id);
  if (sensor) {
    TelemetryUnit unit = sensor->unit;
    uint8_t prec = min<uint8_t>(2, sensor->prec);
    telemetrySensor.init(sensor->name, unit, prec);
    if (id >= ADC1_ID && id <= BATT_ID) {
      telemetrySensor.prec = 1;
      telemetrySensor.custom.ratio = 132;
      telemetrySensor.inputFlags = TELEM_INPUT_FLAGS_FILTERING;
    }
  }
  else {
    telemetrySensor.init(id);
  }

  eeDirty(EE_MODEL);
}
