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

#include "opentx.h"

#define START_STOP         0x7e
#define BYTESTUFF          0x7d
#define STUFF_MASK         0x20

// FrSky PRIM IDs (1 byte)
#define DATA_FRAME         0x10

// FrSky old DATA IDs (1 byte)
#define GPS_ALT_BP_ID      0x01
#define TEMP1_ID           0x02
#define RPM_ID             0x03
#define FUEL_ID            0x04
#define TEMP2_ID           0x05
#define VOLTS_ID           0x06
#define GPS_ALT_AP_ID      0x09
#define BARO_ALT_BP_ID     0x10
#define GPS_SPEED_BP_ID    0x11
#define GPS_LONG_BP_ID     0x12
#define GPS_LAT_BP_ID      0x13
#define GPS_COURS_BP_ID    0x14
#define GPS_DAY_MONTH_ID   0x15
#define GPS_YEAR_ID        0x16
#define GPS_HOUR_MIN_ID    0x17
#define GPS_SEC_ID         0x18
#define GPS_SPEED_AP_ID    0x19
#define GPS_LONG_AP_ID     0x1A
#define GPS_LAT_AP_ID      0x1B
#define GPS_COURS_AP_ID    0x1C
#define BARO_ALT_AP_ID     0x21
#define GPS_LONG_EW_ID     0x22
#define GPS_LAT_NS_ID      0x23
#define ACCEL_X_ID         0x24
#define ACCEL_Y_ID         0x25
#define ACCEL_Z_ID         0x26
#define CURRENT_ID         0x28
#define VARIO_ID           0x30
#define VFAS_ID            0x39
#define VOLTS_BP_ID        0x3A
#define VOLTS_AP_ID        0x3B
#define FRSKY_LAST_ID      0x3F

// FrSky new DATA IDs (2 bytes)
#define RSSI_ID            0xf101
#define ADC1_ID            0xf102
#define ADC2_ID            0xf103
#define BATT_ID            0xf104
#define SWR_ID             0xf105
#define T1_FIRST_ID        0x0400
#define T1_LAST_ID         0x040f
#define T2_FIRST_ID        0x0410
#define T2_LAST_ID         0x041f
#define RPM_FIRST_ID       0x0500
#define RPM_LAST_ID        0x050f
#define FUEL_FIRST_ID      0x0600
#define FUEL_LAST_ID       0x060f
#define ALT_FIRST_ID       0x0100
#define ALT_LAST_ID        0x010f
#define VARIO_FIRST_ID     0x0110
#define VARIO_LAST_ID      0x011f
#define ACCX_FIRST_ID      0x0700
#define ACCX_LAST_ID       0x070f
#define ACCY_FIRST_ID      0x0710
#define ACCY_LAST_ID       0x071f
#define ACCZ_FIRST_ID      0x0720
#define ACCZ_LAST_ID       0x072f
#define CURR_FIRST_ID      0x0200
#define CURR_LAST_ID       0x020f
#define VFAS_FIRST_ID      0x0210
#define VFAS_LAST_ID       0x021f
#define GPS_SPEED_FIRST_ID 0x0830
#define GPS_SPEED_LAST_ID  0x083f
#define CELLS_FIRST_ID     0x0300
#define CELLS_LAST_ID      0x030f

// FrSky wrong IDs ?
#define BETA_VARIO_ID      0x8030
#define BETA_BARO_ALT_ID   0x8010

uint8_t telemetryState = TELEMETRY_INIT;

uint8_t frskyRxBuffer[FRSKY_RX_PACKET_SIZE];   // Receive buffer. 9 bytes (full packet), worst case 18 bytes with byte-stuffing (+1)
uint8_t frskyTxBuffer[FRSKY_TX_PACKET_SIZE];   // Ditto for transmit buffer
#if !defined(CPUARM)
uint8_t frskyTxBufferCount = 0;
#endif
uint8_t link_counter = 0;
FrskyData frskyData;

#if defined(DEBUG) && !defined(SIMU)
#include "../fifo.h"
extern FIL g_telemetryFile;
Fifo<512> debugTelemetryFifo;
#endif

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
      if (frskyData.hub.current > frskyData.hub.maxCurrent)
        frskyData.hub.maxCurrent = frskyData.hub.current;
      break;

    case VOLTS_AP_ID:
#if defined(FAS_BSS)
      frskyData.hub.vfas = (frskyData.hub.volts_bp * 10 + frskyData.hub.volts_ap);
#else
      frskyData.hub.vfas = ((frskyData.hub.volts_bp * 100 + frskyData.hub.volts_ap * 10) * 21) / 110;
#endif
      /* TODO later if (!frskyData.hub.minVfas || frskyData.hub.minVfas > frskyData.hub.vfas)
        frskyData.hub.minVfas = frskyData.hub.vfas; */
      break;

    case BARO_ALT_AP_ID:
      setBaroAltitude((int32_t)100 * frskyData.hub.baroAltitude_bp + frskyData.hub.baroAltitude_ap);
      break;

    case GPS_ALT_AP_ID:
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

    case GPS_SPEED_BP_ID:
      // Speed => Max speed
      if (frskyData.hub.gpsSpeed_bp > frskyData.hub.maxGpsSpeed)
        frskyData.hub.maxGpsSpeed = frskyData.hub.gpsSpeed_bp;
      break;

    case VOLTS_ID:
      // Voltage => Cell number + Cell voltage
    {
      uint8_t battnumber = ((frskyData.hub.volts & 0x00F0) >> 4);
      if (battnumber < 12) {
        if (frskyData.hub.cellsCount < battnumber+1) {
          frskyData.hub.cellsCount = battnumber+1;
        }
        uint8_t cellVolts = (uint8_t)(((((frskyData.hub.volts & 0xFF00) >> 8) + ((frskyData.hub.volts & 0x000F) << 8)))/10);
        frskyData.hub.cellVolts[battnumber] = cellVolts;
        if (!frskyData.hub.minCellVolts || cellVolts < frskyData.hub.minCellVolts || battnumber==frskyData.hub.minCellIdx) {
          frskyData.hub.minCellIdx = battnumber;
          frskyData.hub.minCellVolts = cellVolts;
        }
      }
      break;
    }

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

#define SPORT_DATA_U8(packet)   (packet[4])
#define SPORT_DATA_S32(packet)  (*((int32_t *)(packet+4)))
#define SPORT_DATA_U32(packet)  (*((uint32_t *)(packet+4)))
#define HUB_DATA_U16(packet)    (*((uint16_t *)(packet+4)))

void processSportPacket(uint8_t *packet)
{
  /* uint8_t  dataId = packet[0]; */
  uint8_t  prim   = packet[1];
  uint16_t appId  = *((uint16_t *)(packet+2));

  switch (prim)
  {
    case DATA_FRAME:

      if (appId == RSSI_ID) {
        frskyData.rssi[0].set(SPORT_DATA_U8(packet));
      }
      if (appId == SWR_ID) {
        frskyData.rssi[1].set(SPORT_DATA_U8(packet));
      }
      else if (appId == ADC1_ID || appId == ADC2_ID) {
        // A1/A2 of DxR receivers
        frskyData.analog[appId-ADC1_ID].set(SPORT_DATA_U8(packet));
      }
      else if (appId == BATT_ID) {
        frskyData.analog[0].set(SPORT_DATA_U8(packet));
      }
      else if ((appId >> 8) == 0) {
        // The old FrSky IDs
        uint8_t  id = (uint8_t)appId;
        uint16_t value = HUB_DATA_U16(packet);
        processHubPacket(id, value);
      }
      else if (appId == BETA_BARO_ALT_ID) {
        int32_t baroAltitude = SPORT_DATA_S32(packet);
        setBaroAltitude(10 * (baroAltitude >> 8));
      }
      else if (appId == BETA_VARIO_ID) {
        int32_t varioSpeed = SPORT_DATA_S32(packet);
        frskyData.hub.varioSpeed = 10 * (varioSpeed >> 8);
      }
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
        if (frskyData.hub.current > frskyData.hub.maxCurrent)
          frskyData.hub.maxCurrent = frskyData.hub.current;
      }
      else if (appId >= VFAS_FIRST_ID && appId <= VFAS_LAST_ID) {
        frskyData.hub.vfas = SPORT_DATA_U32(packet)/10;   //TODO: remove /10 and display with PREC2 when using SPORT
      }
      else if (appId >= GPS_SPEED_FIRST_ID && appId <= GPS_SPEED_LAST_ID) {
        frskyData.hub.gpsSpeed_bp = SPORT_DATA_U32(packet);
        if (frskyData.hub.gpsSpeed_bp > frskyData.hub.maxGpsSpeed)
          frskyData.hub.maxGpsSpeed = frskyData.hub.gpsSpeed_bp;
      }
      else if (appId >= CELLS_FIRST_ID && appId <= CELLS_LAST_ID) {
        uint32_t cells = SPORT_DATA_U32(packet);
        uint8_t battnumber = cells & 0xF;
        uint32_t minCell, minCellNum;
        
        //TODO: Use reported total voltages (bits 4-7)?
        frskyData.hub.cellVolts[battnumber] = ((cells & 0x000FFF00) >> 8) / 10;
        frskyData.hub.cellVolts[battnumber+1] = ((cells & 0xFFF00000) >> 20) / 10;
        
        if (frskyData.hub.cellsCount < battnumber+2)
          frskyData.hub.cellsCount = battnumber+2;
        if (frskyData.hub.cellVolts[battnumber+1] == 0)
          frskyData.hub.cellsCount--;
        
        if((frskyData.hub.cellVolts[battnumber] < frskyData.hub.cellVolts[battnumber+1]) || (frskyData.hub.cellVolts[battnumber+1] == 0)) {
          minCell = frskyData.hub.cellVolts[battnumber];
          minCellNum = battnumber;
        }
        else {
          minCell = frskyData.hub.cellVolts[battnumber+1];
          minCellNum = battnumber+1;
        }
        	
        if (!frskyData.hub.minCellVolts || minCell < frskyData.hub.minCellVolts || minCellNum==frskyData.hub.minCellIdx) {
          frskyData.hub.minCellIdx = minCellNum;
          frskyData.hub.minCellVolts = minCell;
        }
      }
      break;

  }
}

// Receive buffer state machine state enum
enum FrSkyDataState {
  STATE_DATA_IDLE,
  STATE_DATA_IN_FRAME,
  STATE_DATA_XOR,
};

void processSerialData(uint8_t data)
{
  static uint8_t numPktBytes = 0;
  static uint8_t dataState = STATE_DATA_IDLE;

#if defined(BLUETOOTH)
  // TODO if (g_model.bt_telemetry)
  btPushByte(data);
#endif

#if defined(DEBUG) && !defined(SIMU)
  debugTelemetryFifo.push(data);
#endif

  if (data == START_STOP) {
    dataState = STATE_DATA_IN_FRAME;
    numPktBytes = 0;
  }
  else {
    switch (dataState) {
      case STATE_DATA_XOR:
        frskyRxBuffer[numPktBytes++] = data ^ STUFF_MASK;
        dataState = STATE_DATA_IN_FRAME;
        break;

      case STATE_DATA_IN_FRAME:
        if (data == BYTESTUFF)
          dataState = STATE_DATA_XOR; // XOR next byte
        else
          frskyRxBuffer[numPktBytes++] = data;
        break;
    }
  }

  if (numPktBytes == FRSKY_RX_PACKET_SIZE) {
    processSportPacket(frskyRxBuffer);
    dataState = STATE_DATA_IDLE;
  }
}

uint8_t frskyAlarmsSendState = 0 ;


void telemetryInterrupt10ms()
{
  uint16_t voltage = 0; /* unit: 1/10 volts */

#if defined(FRSKY_HUB)
  for (uint8_t i=0; i<frskyData.hub.cellsCount; i++)
    voltage += frskyData.hub.cellVolts[i];
  voltage /= 5;
  frskyData.hub.cellsSum = voltage;
#endif

  if (TELEMETRY_STREAMING()) {
    uint8_t channel = g_model.frsky.voltsSource;
    if (channel <= 1) {
      voltage = applyChannelRatio(channel, frskyData.analog[channel].value) / 10;
    }
#if defined(FRSKY_HUB)
    else if (channel == 2) {
      voltage = frskyData.hub.vfas;
    }
#endif

#if defined(FRSKY_HUB)
    uint16_t current = frskyData.hub.current; /* unit: 1/10 amps */
#else
    uint16_t current = 0;
#endif

    channel = g_model.frsky.currentSource - FRSKY_SOURCE_A1;
    if (channel <= 1) {
      current = applyChannelRatio(channel, frskyData.analog[channel].value) / 10;
    }

    frskyData.hub.power = (current * voltage) / 100;
    if (frskyData.hub.power > frskyData.hub.maxPower)
      frskyData.hub.maxPower = frskyData.hub.power;

    frskyData.hub.currentPrescale += current;
    if (frskyData.hub.currentPrescale >= 3600) {
      frskyData.hub.currentConsumption += 1;
      frskyData.hub.currentPrescale -= 3600;
    }
  }
}

inline bool alarmRaised(uint8_t channel, uint8_t idx)
{
  return g_model.frsky.channels[channel].ratio > 0 && g_model.frsky.channels[channel].alarms_value[idx] > 0 && frskyData.analog[channel].value < g_model.frsky.channels[channel].alarms_value[idx];
}

void telemetryWakeup()
{
#if defined(PCBSKY9X)
  rxPdcUsart(processSerialData);              // Receive serial data here
#endif

#if 0
  // Attempt to transmit any waiting Fr-Sky alarm set packets every 50ms (subject to packet buffer availability)
  static uint8_t frskyTxDelay = 5;
  if (frskyAlarmsSendState && (--frskyTxDelay == 0)) {
    frskyTxDelay = 5; // 50ms
    // frskySendNextAlarm();
  }
#endif

#if defined(VARIO)
  if (TELEMETRY_STREAMING() && !IS_FAI_ENABLED())
    varioWakeup();
#endif

#if defined(DEBUG) && !defined(SIMU)
  static tmr10ms_t lastTime = 0;
  tmr10ms_t newTime = get_tmr10ms();
  uint8_t data;
  struct gtm utm;
  gettime(&utm);
      
  while (debugTelemetryFifo.pop(data)) {
    if (lastTime != newTime) {
      f_printf(&g_telemetryFile, "\r\n%4d-%02d-%02d,%02d:%02d:%02d.%02d0: %02X", utm.tm_year+1900, utm.tm_mon+1, utm.tm_mday, utm.tm_hour, utm.tm_min, utm.tm_sec, g_ms100, data);
      lastTime = newTime;
    }
    else {
      f_printf(&g_telemetryFile, " %02X", data);
    }
  }
#endif

  static tmr10ms_t alarmsCheckTime = 0;
  static uint8_t alarmsCheckStep = 0;

  if (int32_t(get_tmr10ms() - alarmsCheckTime) > 0) {

    alarmsCheckTime = get_tmr10ms() + 100; /* next check in 1second */

    if (alarmsCheckStep == 0) {
      if (frskyData.rssi[1].value > 0x33) {
        AUDIO_SWR_RED();
        s_global_warning = PSTR(CENTER "Antenna problem!");
        alarmsCheckTime = get_tmr10ms() + 300; /* next check in 3seconds */
      }
    }
    else if (TELEMETRY_STREAMING()) {
      if (alarmsCheckStep == 1) {
        if (getRssiAlarmValue(1) && frskyData.rssi[0].value < getRssiAlarmValue(1)) {
          AUDIO_RSSI_RED();
          alarmsCheckTime = get_tmr10ms() + 300; /* next check in 3seconds */
        }
        else if (getRssiAlarmValue(0) && frskyData.rssi[0].value < getRssiAlarmValue(0)) {
          AUDIO_RSSI_ORANGE();
          alarmsCheckTime = get_tmr10ms() + 300; /* next check in 3seconds */
        }
      }
      else if (alarmsCheckStep == 2) {
        if (alarmRaised(1, 1) && g_model.moduleData[INTERNAL_MODULE].rfProtocol == RF_PROTO_D8) {
          AUDIO_A2_RED();
          alarmsCheckTime = get_tmr10ms() + 300; /* next check in 3seconds */
        }
        else if (alarmRaised(1, 0) && g_model.moduleData[INTERNAL_MODULE].rfProtocol == RF_PROTO_D8) {
          AUDIO_A2_ORANGE();
          alarmsCheckTime = get_tmr10ms() + 300; /* next check in 3seconds */
        }
      }
      else if (alarmsCheckStep == 3) {
        if (alarmRaised(0, 1)) {
          AUDIO_A1_RED();
          alarmsCheckTime = get_tmr10ms() + 300; /* next check in 3seconds */
        }
        else if (alarmRaised(0, 0)) {
          AUDIO_A1_ORANGE();
          alarmsCheckTime = get_tmr10ms() + 300; /* next check in 3seconds */
        }
      }
    }

    if (++alarmsCheckStep == 4) {
      alarmsCheckStep = 0;
    }
  }

  if (TELEMETRY_STREAMING()) {
    if (telemetryState == TELEMETRY_KO) {
      AUDIO_TELEMETRY_BACK();
    }
    telemetryState = TELEMETRY_OK;
  }
  else if (telemetryState == TELEMETRY_OK) {
    telemetryState = TELEMETRY_KO;
    AUDIO_TELEMETRY_LOST();
  }
}

void FRSKY_Init(void)
{
  // clear frsky variables
  resetTelemetry();
}

void FRSKY_End(void)
{
}

void FrskyValueWithMinMax::set(uint8_t value)
{
  this->value = value;

  if (value) {
    if (!max || value > max)
      max = value;
    if (!min || value < min)
      min = value;
  }
}

void resetTelemetry()
{
  memclear(&frskyData, sizeof(frskyData));

#if defined(FRSKY_HUB)
  frskyData.hub.gpsLatitude_bp = 2;
  frskyData.hub.gpsLongitude_bp = 2;
  frskyData.hub.gpsFix = -1;
#endif

#ifdef SIMU
  frskyData.analog[0].set(120);
  frskyData.analog[1].set(240);
  frskyData.rssi[0].value = 75;
  frskyData.rssi[1].value = 30;
  frskyData.hub.fuelLevel = 75;
  frskyData.hub.rpm = 12000;

  frskyData.hub.gpsFix = 1;
  frskyData.hub.gpsLatitude_bp = 4401;
  frskyData.hub.gpsLatitude_ap = 7710;
  frskyData.hub.gpsLongitude_bp = 1006;
  frskyData.hub.gpsLongitude_ap = 8872;
  getGpsPilotPosition();

  frskyData.hub.gpsLatitude_bp = 4401;
  frskyData.hub.gpsLatitude_ap = 7455;
  frskyData.hub.gpsLongitude_bp = 1006;
  frskyData.hub.gpsLongitude_ap = 9533;
  getGpsDistance();

  frskyData.hub.cellsCount = 6;

#if 0 // defined(FRSKY_SPORT)
  frskyData.hub.baroAltitude = 50 * 100;
#else
  frskyData.hub.baroAltitude_bp = 50;
#endif

  frskyData.hub.gpsAltitude_bp = 50;

  frskyData.hub.minAltitude = 10;
  frskyData.hub.maxAltitude = 500;

  frskyData.hub.accelY = 100;
  frskyData.hub.temperature1 = -30;
  frskyData.hub.maxTemperature1 = 100;
  
  frskyData.hub.current = 5;
  frskyData.hub.maxCurrent = 56;
#endif
}
