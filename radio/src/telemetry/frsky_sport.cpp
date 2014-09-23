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

// FrSky PRIM IDs (1 byte)
#define DATA_FRAME              0x10

// FrSky old DATA IDs (1 byte)
#define GPS_ALT_BP_ID           0x01
#define TEMP1_ID                0x02
#define RPM_ID                  0x03
#define FUEL_ID                 0x04
#define TEMP2_ID                0x05
#define VOLTS_ID                0x06
#define GPS_ALT_AP_ID           0x09
#define BARO_ALT_BP_ID          0x10
#define GPS_SPEED_BP_ID         0x11
#define GPS_LONG_BP_ID          0x12
#define GPS_LAT_BP_ID           0x13
#define GPS_COURS_BP_ID         0x14
#define GPS_DAY_MONTH_ID        0x15
#define GPS_YEAR_ID             0x16
#define GPS_HOUR_MIN_ID         0x17
#define GPS_SEC_ID              0x18
#define GPS_SPEED_AP_ID         0x19
#define GPS_LONG_AP_ID          0x1A
#define GPS_LAT_AP_ID           0x1B
#define GPS_COURS_AP_ID         0x1C
#define BARO_ALT_AP_ID          0x21
#define GPS_LONG_EW_ID          0x22
#define GPS_LAT_NS_ID           0x23
#define ACCEL_X_ID              0x24
#define ACCEL_Y_ID              0x25
#define ACCEL_Z_ID              0x26
#define CURRENT_ID              0x28
#define VARIO_ID                0x30
#define VFAS_ID                 0x39
#define VOLTS_BP_ID             0x3A
#define VOLTS_AP_ID             0x3B
#define FRSKY_LAST_ID           0x3F

// FrSky new DATA IDs (2 bytes)
#define ALT_FIRST_ID            0x0100
#define ALT_LAST_ID             0x010f
#define VARIO_FIRST_ID          0x0110
#define VARIO_LAST_ID           0x011f
#define CURR_FIRST_ID           0x0200
#define CURR_LAST_ID            0x020f
#define VFAS_FIRST_ID           0x0210
#define VFAS_LAST_ID            0x021f
#define CELLS_FIRST_ID          0x0300
#define CELLS_LAST_ID           0x030f
#define T1_FIRST_ID             0x0400
#define T1_LAST_ID              0x040f
#define T2_FIRST_ID             0x0410
#define T2_LAST_ID              0x041f
#define RPM_FIRST_ID            0x0500
#define RPM_LAST_ID             0x050f
#define FUEL_FIRST_ID           0x0600
#define FUEL_LAST_ID            0x060f
#define ACCX_FIRST_ID           0x0700
#define ACCX_LAST_ID            0x070f
#define ACCY_FIRST_ID           0x0710
#define ACCY_LAST_ID            0x071f
#define ACCZ_FIRST_ID           0x0720
#define ACCZ_LAST_ID            0x072f
#define GPS_LONG_LATI_FIRST_ID  0x0800
#define GPS_LONG_LATI_LAST_ID   0x080f
#define GPS_ALT_FIRST_ID        0x0820
#define GPS_ALT_LAST_ID         0x082f
#define GPS_SPEED_FIRST_ID      0x0830
#define GPS_SPEED_LAST_ID       0x083f
#define GPS_COURS_FIRST_ID      0x0840
#define GPS_COURS_LAST_ID       0x084f
#define GPS_TIME_DATE_FIRST_ID  0x0850
#define GPS_TIME_DATE_LAST_ID   0x085f
#define A3_FIRST_ID             0x0900
#define A3_LAST_ID              0x090f
#define A4_FIRST_ID             0x0910
#define A4_LAST_ID              0x091f
#define AIR_SPEED_FIRST_ID      0x0a00
#define AIR_SPEED_LAST_ID       0x0a0f
#define RSSI_ID                 0xf101
#define ADC1_ID                 0xf102
#define ADC2_ID                 0xf103
#define BATT_ID                 0xf104
#define SWR_ID                  0xf105

// Default sensor data IDs (Physical IDs + CRC)
#define DATA_ID_VARIO            0x00 // 0
#define DATA_ID_FLVSS            0xA1 // 1
#define DATA_ID_FAS              0x22 // 2
#define DATA_ID_GPS              0x83 // 3
#define DATA_ID_RPM              0xE4 // 4
#define DATA_ID_SP2UH            0x45 // 5
#define DATA_ID_SP2UR            0xC6 // 6

void setBaroAltitude(int32_t baroAltitude)
{
  // First received barometer altitude => Altitude offset
  if (!frskyData.hub.baroAltitudeOffset)
    frskyData.hub.baroAltitudeOffset = -baroAltitude;

  baroAltitude += frskyData.hub.baroAltitudeOffset;
  frskyData.hub.baroAltitude = baroAltitude;

  baroAltitude /= 100;
  if (baroAltitude > frskyData.hub.maxAltitude)
    frskyData.hub.maxAltitude = baroAltitude;
  if (baroAltitude < frskyData.hub.minAltitude)
    frskyData.hub.minAltitude = baroAltitude;
}

void processHubPacket(uint8_t id, uint16_t value)
{
  if (id > FRSKY_LAST_ID)
    return;

  if (id == GPS_LAT_BP_ID) {
    if (value)
      frskyData.hub.gpsFix = 1;
    else if (frskyData.hub.gpsFix > 0 && frskyData.hub.gpsLatitude_bp > 1)
      frskyData.hub.gpsFix = 0;
  }
  else if (id == GPS_LONG_BP_ID) {
    if (value)
      frskyData.hub.gpsFix = 1;
    else if (frskyData.hub.gpsFix > 0 && frskyData.hub.gpsLongitude_bp > 1)
      frskyData.hub.gpsFix = 0;
  }

  if (id == GPS_ALT_BP_ID || (id >= GPS_ALT_AP_ID && id <= GPS_LAT_NS_ID && id != BARO_ALT_BP_ID && id != BARO_ALT_AP_ID)) {
    // if we don't have a fix, we may discard the value
    if (frskyData.hub.gpsFix <= 0)
      return;
  }

  ((uint16_t*)&frskyData.hub)[id] = value;

  switch (id) {

    case RPM_ID:
      frskyData.hub.rpm *= (uint8_t)60/(g_model.frsky.blades+2);
      if (frskyData.hub.rpm > frskyData.hub.maxRpm)
        frskyData.hub.maxRpm = frskyData.hub.rpm;
      break;
      
    case TEMP1_ID:
      if (frskyData.hub.temperature1 > frskyData.hub.maxTemperature1)
        frskyData.hub.maxTemperature1 = frskyData.hub.temperature1;
      break;

    case TEMP2_ID:
      if (frskyData.hub.temperature2 > frskyData.hub.maxTemperature2)
        frskyData.hub.maxTemperature2 = frskyData.hub.temperature2;
      break;

    case CURRENT_ID:
      if ((int16_t)frskyData.hub.current > 0 && ((int16_t)frskyData.hub.current + g_model.frsky.fasOffset) > 0)
        frskyData.hub.current += g_model.frsky.fasOffset;
      else
        frskyData.hub.current = 0;
      if (frskyData.hub.current > frskyData.hub.maxCurrent)
        frskyData.hub.maxCurrent = frskyData.hub.current;
      break;

    case VOLTS_AP_ID:
#if defined(FAS_BSS)
      frskyData.hub.vfas = (frskyData.hub.volts_bp * 10 + frskyData.hub.volts_ap);
#else
      frskyData.hub.vfas = ((frskyData.hub.volts_bp * 100 + frskyData.hub.volts_ap * 10) * 21) / 110;
#endif
      if (!frskyData.hub.minVfas || frskyData.hub.vfas < frskyData.hub.minVfas)
        frskyData.hub.minVfas = frskyData.hub.vfas;
      break;

    case BARO_ALT_AP_ID:
      if (frskyData.hub.baroAltitude_ap > 9)
        frskyData.hub.varioHighPrecision = true;
      if (!frskyData.hub.varioHighPrecision)
        frskyData.hub.baroAltitude_ap *= 10;
      setBaroAltitude((int32_t)100 * frskyData.hub.baroAltitude_bp + (frskyData.hub.baroAltitude_bp >= 0 ? frskyData.hub.baroAltitude_ap : -frskyData.hub.baroAltitude_ap));
      break;

    case GPS_ALT_AP_ID:
    {
      frskyData.hub.gpsAltitude = (frskyData.hub.gpsAltitude_bp * 100) + frskyData.hub.gpsAltitude_ap;
      if (!frskyData.hub.gpsAltitudeOffset)
        frskyData.hub.gpsAltitudeOffset = -frskyData.hub.gpsAltitude;
      if (!frskyData.hub.baroAltitudeOffset) {
        int altitude = TELEMETRY_RELATIVE_GPS_ALT_BP;
        if (altitude > frskyData.hub.maxAltitude)
          frskyData.hub.maxAltitude = altitude;
        if (altitude < frskyData.hub.minAltitude)
          frskyData.hub.minAltitude = altitude;
      }
      if (!frskyData.hub.pilotLatitude && !frskyData.hub.pilotLongitude) {
        // First received GPS position => Pilot GPS position
        getGpsPilotPosition();
      }
      else if (frskyData.hub.gpsDistNeeded || g_menuStack[g_menuStackPtr] == menuTelemetryFrsky) {
        getGpsDistance();
      }
      break;
    }

    case GPS_SPEED_BP_ID:
      // Speed => Max speed
      if (frskyData.hub.gpsSpeed_bp > frskyData.hub.maxGpsSpeed)
        frskyData.hub.maxGpsSpeed = frskyData.hub.gpsSpeed_bp;
      break;

    case VOLTS_ID:
      frskyUpdateCells();
      break;

    case GPS_HOUR_MIN_ID:
      frskyData.hub.hour = ((uint8_t)(frskyData.hub.hour + g_eeGeneral.timezone + 24)) % 24;
      break;

    case ACCEL_X_ID:
    case ACCEL_Y_ID:
    case ACCEL_Z_ID:
      ((int16_t*)(&frskyData.hub))[id] /= 10;
      break;

  }
}

bool checkSportPacket(uint8_t *packet)
{
  short crc = 0;
  for (int i=1; i<FRSKY_SPORT_PACKET_SIZE; i++) {
    crc += packet[i]; //0-1FF
    crc += crc >> 8; //0-100
    crc &= 0x00ff;
    crc += crc >> 8; //0-0FF
    crc &= 0x00ff;
  }
  return (crc == 0x00ff);
}

#define SPORT_DATA_U8(packet)   (packet[4])
#define SPORT_DATA_S32(packet)  (*((int32_t *)(packet+4)))
#define SPORT_DATA_U32(packet)  (*((uint32_t *)(packet+4)))
#define HUB_DATA_U16(packet)    (*((uint16_t *)(packet+4)))

void frskySportProcessPacket(uint8_t *packet)
{
  uint8_t  dataId = packet[0];
  uint8_t  prim   = packet[1];
  uint16_t appId  = *((uint16_t *)(packet+2));

  if (!checkSportPacket(packet)) {
    return;
  }

  switch (prim)
  {
    case DATA_FRAME:

      if (appId == RSSI_ID) {
        frskyStreaming = FRSKY_TIMEOUT10ms; // reset counter only if valid frsky packets are being detected
        link_counter += 256 / FRSKY_SPORT_AVERAGING;
        frskyData.rssi[0].set(SPORT_DATA_U8(packet));
      }
      if (appId == SWR_ID) {
        frskyData.swr.set(SPORT_DATA_U8(packet));
      }
      else if (frskyData.rssi[0].value > 0) {
        if (appId == ADC1_ID || appId == ADC2_ID) {
          // A1/A2 of DxR receivers
          uint8_t idx = appId - ADC1_ID;
          frskyData.analog[idx].set(SPORT_DATA_U8(packet), g_model.frsky.channels[idx].type);
  #if defined(VARIO)
          uint8_t varioSource = g_model.frsky.varioSource - VARIO_SOURCE_A1;
          if (varioSource == appId-ADC1_ID) {
            frskyData.hub.varioSpeed = applyChannelRatio(varioSource, frskyData.analog[varioSource].value);
          }
  #endif
        }
        else if (appId == BATT_ID) {
          frskyData.analog[TELEM_ANA_A1].set(SPORT_DATA_U8(packet), UNIT_VOLTS);
        }
        else if ((appId >> 8) == 0) {
          // The old FrSky IDs
          uint8_t  id = (uint8_t)appId;
          uint16_t value = HUB_DATA_U16(packet);
          processHubPacket(id, value);
        }
#if 1
        else {
          setTelemetryValue(TELEM_PROTO_FRSKY_SPORT, appId, dataId, SPORT_DATA_S32(packet));
        }
#else
        else if (appId >= T1_FIRST_ID && appId <= T1_LAST_ID) {
          frskyData.hub.temperature1 = SPORT_DATA_S32(packet);
          if (frskyData.hub.temperature1 > frskyData.hub.maxTemperature1)
            frskyData.hub.maxTemperature1 = frskyData.hub.temperature1;
        }
        else if (appId >= T2_FIRST_ID && appId <= T2_LAST_ID) {
          frskyData.hub.temperature2 = SPORT_DATA_S32(packet);
          if (frskyData.hub.temperature2 > frskyData.hub.maxTemperature2)
            frskyData.hub.maxTemperature2 = frskyData.hub.temperature2;
        }
        else if (appId >= RPM_FIRST_ID && appId <= RPM_LAST_ID) {
          frskyData.hub.rpm = SPORT_DATA_U32(packet) / (g_model.frsky.blades+2);
          if (frskyData.hub.rpm > frskyData.hub.maxRpm)
            frskyData.hub.maxRpm = frskyData.hub.rpm;
        }
        else if (appId >= FUEL_FIRST_ID && appId <= FUEL_LAST_ID) {
          frskyData.hub.fuelLevel = SPORT_DATA_U32(packet);
        }
        else if (appId >= ALT_FIRST_ID && appId <= ALT_LAST_ID) {
          setBaroAltitude(SPORT_DATA_S32(packet));
        }
        else if (appId >= VARIO_FIRST_ID && appId <= VARIO_LAST_ID) {
          frskyData.hub.varioSpeed = SPORT_DATA_S32(packet);
        }
        else if (appId >= ACCX_FIRST_ID && appId <= ACCX_LAST_ID) {
          frskyData.hub.accelX = SPORT_DATA_S32(packet);
        }
        else if (appId >= ACCY_FIRST_ID && appId <= ACCY_LAST_ID) {
          frskyData.hub.accelY = SPORT_DATA_S32(packet);
        }
        else if (appId >= ACCZ_FIRST_ID && appId <= ACCZ_LAST_ID) {
          frskyData.hub.accelZ = SPORT_DATA_S32(packet);
        }
        else if (appId >= CURR_FIRST_ID && appId <= CURR_LAST_ID) {
          frskyData.hub.current = SPORT_DATA_U32(packet);
          if (((int16_t)frskyData.hub.current + g_model.frsky.fasOffset)>0)
            frskyData.hub.current += g_model.frsky.fasOffset;
          else
            frskyData.hub.current = 0;
          if (frskyData.hub.current > frskyData.hub.maxCurrent)
            frskyData.hub.maxCurrent = frskyData.hub.current;
        }
        else if (appId >= VFAS_FIRST_ID && appId <= VFAS_LAST_ID) {
          frskyData.hub.vfas = SPORT_DATA_U32(packet)/10;   //TODO: remove /10 and display with PREC2 when using SPORT
          if (!frskyData.hub.minVfas || frskyData.hub.vfas < frskyData.hub.minVfas)
            frskyData.hub.minVfas = frskyData.hub.vfas;
        }
        else if (appId >= AIR_SPEED_FIRST_ID && appId <= AIR_SPEED_LAST_ID) {
          frskyData.hub.airSpeed = SPORT_DATA_U32(packet);
          if (frskyData.hub.airSpeed > frskyData.hub.maxAirSpeed)
            frskyData.hub.maxAirSpeed = frskyData.hub.airSpeed;
        }
        else if (appId >= GPS_SPEED_FIRST_ID && appId <= GPS_SPEED_LAST_ID) {
          frskyData.hub.gpsSpeed_bp = (uint16_t) (SPORT_DATA_U32(packet)/1000);
          if (frskyData.hub.gpsSpeed_bp > frskyData.hub.maxGpsSpeed)
            frskyData.hub.maxGpsSpeed = frskyData.hub.gpsSpeed_bp;
        }
        else if (appId >= GPS_TIME_DATE_FIRST_ID && appId <= GPS_TIME_DATE_LAST_ID) {
          uint32_t gps_time_date = SPORT_DATA_U32(packet);
          if (gps_time_date & 0x000000ff) {
            frskyData.hub.year = (uint16_t) ((gps_time_date & 0xff000000) >> 24);
            frskyData.hub.month = (uint8_t) ((gps_time_date & 0x00ff0000) >> 16);
            frskyData.hub.day = (uint8_t) ((gps_time_date & 0x0000ff00) >> 8);
          }
          else {
            frskyData.hub.hour = (uint8_t) ((gps_time_date & 0xff000000) >> 24);
            frskyData.hub.min = (uint8_t) ((gps_time_date & 0x00ff0000) >> 16);
            frskyData.hub.sec = (uint16_t) ((gps_time_date & 0x0000ff00) >> 8);
            frskyData.hub.hour = ((uint8_t) (frskyData.hub.hour + g_eeGeneral.timezone + 24)) % 24;
          }
        }
        else if (appId >= GPS_COURS_FIRST_ID && appId <= GPS_COURS_LAST_ID) {
          uint32_t course = SPORT_DATA_U32(packet);
          frskyData.hub.gpsCourse_bp = course / 100;
          frskyData.hub.gpsCourse_ap = course % 100;
        }
        else if (appId >= GPS_ALT_FIRST_ID && appId <= GPS_ALT_LAST_ID) {
          frskyData.hub.gpsAltitude = SPORT_DATA_S32(packet);

          if (!frskyData.hub.gpsAltitudeOffset)
            frskyData.hub.gpsAltitudeOffset = -frskyData.hub.gpsAltitude;

          if (!frskyData.hub.baroAltitudeOffset) {
            int altitude = TELEMETRY_RELATIVE_GPS_ALT_BP;
            if (altitude > frskyData.hub.maxAltitude)
              frskyData.hub.maxAltitude = altitude;
            if (altitude < frskyData.hub.minAltitude)
              frskyData.hub.minAltitude = altitude;
          }

          if (frskyData.hub.gpsFix > 0) {
            if (!frskyData.hub.pilotLatitude && !frskyData.hub.pilotLongitude) {
              // First received GPS position => Pilot GPS position
              getGpsPilotPosition();
            }
            else if (frskyData.hub.gpsDistNeeded || g_menuStack[g_menuStackPtr] == menuTelemetryFrsky) {
              getGpsDistance();
            }
          }
        }
        else if (appId >= GPS_LONG_LATI_FIRST_ID && appId <= GPS_LONG_LATI_LAST_ID) {
          uint32_t gps_long_lati_data = SPORT_DATA_U32(packet);
          uint32_t gps_long_lati_b1w, gps_long_lati_a1w;
          gps_long_lati_b1w = (gps_long_lati_data & 0x3fffffff) / 10000;
          gps_long_lati_a1w = (gps_long_lati_data & 0x3fffffff) % 10000;
          switch ((gps_long_lati_data & 0xc0000000) >> 30) {
            case 0:
              frskyData.hub.gpsLatitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
              frskyData.hub.gpsLatitude_ap = gps_long_lati_a1w;
              frskyData.hub.gpsLatitudeNS = 'N';
              break;
            case 1:
              frskyData.hub.gpsLatitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
              frskyData.hub.gpsLatitude_ap = gps_long_lati_a1w;
              frskyData.hub.gpsLatitudeNS = 'S';
              break;
            case 2:
              frskyData.hub.gpsLongitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
              frskyData.hub.gpsLongitude_ap = gps_long_lati_a1w;
              frskyData.hub.gpsLongitudeEW = 'E';
              break;
            case 3:
              frskyData.hub.gpsLongitude_bp = (gps_long_lati_b1w / 60 * 100) + (gps_long_lati_b1w % 60);
              frskyData.hub.gpsLongitude_ap = gps_long_lati_a1w;
              frskyData.hub.gpsLongitudeEW = 'W';
              break;
          }
          if (frskyData.hub.gpsLongitudeEW && frskyData.hub.gpsLatitudeNS) {
            frskyData.hub.gpsFix = 1;
          }
          else if (frskyData.hub.gpsFix > 0) {
            frskyData.hub.gpsFix = 0;
          }
        }
        else if (appId >= A3_FIRST_ID && appId <= A3_LAST_ID) {
          frskyData.analog[TELEM_ANA_A3].set((SPORT_DATA_U32(packet)*255+165)/330, UNIT_VOLTS);
        }
        else if (appId >= A4_FIRST_ID && appId <= A4_LAST_ID) {
          frskyData.analog[TELEM_ANA_A4].set((SPORT_DATA_U32(packet)*255+165)/330, UNIT_VOLTS);
        }
        else if (appId >= CELLS_FIRST_ID && appId <= CELLS_LAST_ID) {
          uint32_t data = SPORT_DATA_U32(packet);
          uint8_t battnumber = data & 0xF;
          uint8_t cells = (data & 0xF0) >> 4;
          bool useSecondCell = (battnumber+1 < cells);

          if (dataId == DATA_ID_FLVSS) {
            // first sensor, remember its cell count
            frskyData.hub.sensorCellsCount[0] = cells;
            cells += frskyData.hub.sensorCellsCount[1];
          }
          else {
            // second sensor connected
            frskyData.hub.sensorCellsCount[1] = cells;
            cells += frskyData.hub.sensorCellsCount[0];
            battnumber += frskyData.hub.sensorCellsCount[0];
          }

          if (cells != frskyData.hub.cellsCount) {
            frskySetCellsCount(cells);
          }

          frskySetCellVoltage(battnumber, (frskyCellVoltage_t) ((data & 0x000FFF00) >>  8) / 5);
          if (useSecondCell) {
            frskySetCellVoltage(battnumber+1, (frskyCellVoltage_t) ((data & 0xFFF00000) >> 20) / 5);
          }
        }
#endif
      }
      break;
  }
}

void frskySportSetDefault(int index, uint16_t id, uint8_t instance)
{
  TelemetryValue & telemetryValue = g_model.telemetryValues[index];

  telemetryValue.id = id;
  telemetryValue.instance = instance;

  if (id == RSSI_ID) {
    setTelemetryLabel(telemetryValue, ZSTR_RSSI);
    telemetryValue.unit = UNIT_RAW;
    telemetryValue.flags = TELEM_FLAG_LOG;
  }
  else if (id >= T1_FIRST_ID && id <= T2_LAST_ID) {
    setTelemetryLabel(telemetryValue, ZSTR_TEMP);
    telemetryValue.unit = UNIT_TEMPERATURE;
    telemetryValue.flags = TELEM_FLAG_LOG|TELEM_FLAG_LOSS_ALARM;
  }
  else if (id >= RPM_FIRST_ID && id <= RPM_LAST_ID) {
    setTelemetryLabel(telemetryValue, ZSTR_RPM);
    telemetryValue.unit = UNIT_RPMS;
    telemetryValue.flags = TELEM_FLAG_LOG;
  }
  else if (id >= FUEL_FIRST_ID && id <= FUEL_LAST_ID) {
    setTelemetryLabel(telemetryValue, ZSTR_FUEL);
    telemetryValue.unit = UNIT_PERCENT;
    telemetryValue.flags = TELEM_FLAG_LOG|TELEM_FLAG_LOSS_ALARM;
  }
  else if (id >= ALT_FIRST_ID && id <= ALT_LAST_ID) {
    setTelemetryLabel(telemetryValue, ZSTR_ALT);
    telemetryValue.unit = UNIT_DIST;
    telemetryValue.flags = TELEM_FLAG_LOG;
    telemetryValue.prec = 2;
    // TODO altitude offset
  }
  else if (id >= VARIO_FIRST_ID && id <= VARIO_LAST_ID) {
    setTelemetryLabel(telemetryValue, ZSTR_VSPD);
    telemetryValue.unit = UNIT_METERS_PER_SECOND;
    telemetryValue.flags = TELEM_FLAG_LOG;
  }
  else if (id >= ACCX_FIRST_ID && id <= ACCX_LAST_ID) {
    setTelemetryLabel(telemetryValue, ZSTR_ACCX);
    telemetryValue.unit = UNIT_G;
    telemetryValue.flags = TELEM_FLAG_LOG;
  }
  else if (id >= ACCY_FIRST_ID && id <= ACCY_LAST_ID) {
    setTelemetryLabel(telemetryValue, ZSTR_ACCY);
    telemetryValue.unit = UNIT_G;
    telemetryValue.flags = TELEM_FLAG_LOG;
  }
  else if (id >= ACCZ_FIRST_ID && id <= ACCZ_LAST_ID) {
    setTelemetryLabel(telemetryValue, ZSTR_ACCZ);
    telemetryValue.unit = UNIT_G;
    telemetryValue.flags = TELEM_FLAG_LOG;
  }
  else if (id >= CURR_FIRST_ID && id <= CURR_LAST_ID) {
    setTelemetryLabel(telemetryValue, ZSTR_CURR);
    telemetryValue.unit = UNIT_AMPS;
    telemetryValue.flags = TELEM_FLAG_LOG;
    telemetryValue.prec = 1;
  }
  else if (id >= VFAS_FIRST_ID && id <= VFAS_LAST_ID) {
    setTelemetryLabel(telemetryValue, ZSTR_VFAS);
    telemetryValue.unit = UNIT_VOLTS;
    telemetryValue.flags = TELEM_FLAG_LOG;
    telemetryValue.prec = 2;
  }
  else if (id >= AIR_SPEED_FIRST_ID && id <= AIR_SPEED_LAST_ID) {
    setTelemetryLabel(telemetryValue, ZSTR_ASPD);
    telemetryValue.unit = UNIT_METERS_PER_SECOND;
    telemetryValue.flags = TELEM_FLAG_LOG;
    telemetryValue.prec = 1;
  }
  else if (id >= GPS_SPEED_FIRST_ID && id <= GPS_SPEED_LAST_ID) {
    setTelemetryLabel(telemetryValue, ZSTR_GSPD);
    telemetryValue.unit = UNIT_METERS_PER_SECOND;
    telemetryValue.flags = TELEM_FLAG_LOG;
  }
  else {
    char label[4];
    label[0] = hex2zchar((id & 0xf000) >> 12);
    label[1] = hex2zchar((id & 0x0f00) >> 8);
    label[2] = hex2zchar((id & 0x00f0) >> 4);
    label[3] = hex2zchar((id & 0x000f) >> 0);
    setTelemetryLabel(telemetryValue, label);
    telemetryValue.unit = UNIT_RAW;
  }

  eeDirty(EE_MODEL);

}
