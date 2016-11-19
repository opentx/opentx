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

#define PRIM_REQ_POWERUP    (0)
#define PRIM_REQ_VERSION    (1)
#define PRIM_CMD_DOWNLOAD   (3)
#define PRIM_DATA_WORD      (4)
#define PRIM_DATA_EOF       (5)

#define PRIM_ACK_POWERUP    (0x80)
#define PRIM_ACK_VERSION    (0x81)
#define PRIM_REQ_DATA_ADDR  (0x82)
#define PRIM_END_DOWNLOAD   (0x83)
#define PRIM_DATA_CRC_ERR   (0x84)

struct FrSkySportSensor {
  const uint16_t firstId;
  const uint16_t lastId;
  const uint8_t subId;
  const char * name;
  const TelemetryUnit unit;
  const uint8_t prec;
};

const FrSkySportSensor sportSensors[] = {
  { RSSI_ID, RSSI_ID, 0, ZSTR_RSSI, UNIT_DB, 0 },
  { SWR_ID, SWR_ID, 0, ZSTR_SWR, UNIT_RAW, 0 },
  { ADC1_ID, ADC1_ID, 0, ZSTR_A1, UNIT_VOLTS, 1 },
  { ADC2_ID, ADC2_ID, 0, ZSTR_A2, UNIT_VOLTS, 1 },
  { A3_FIRST_ID, A3_LAST_ID, 0, ZSTR_A3, UNIT_VOLTS, 2 },
  { A4_FIRST_ID, A4_LAST_ID, 0, ZSTR_A4, UNIT_VOLTS, 2 },  
  { BATT_ID, BATT_ID, 0, ZSTR_BATT, UNIT_VOLTS, 1 },
  { T1_FIRST_ID, T1_LAST_ID, 0, ZSTR_TEMP1, UNIT_CELSIUS, 0 },
  { T2_FIRST_ID, T2_LAST_ID, 0, ZSTR_TEMP2, UNIT_CELSIUS, 0 },
  { RPM_FIRST_ID, RPM_LAST_ID, 0, ZSTR_RPM, UNIT_RPMS, 0 },
  { FUEL_FIRST_ID, FUEL_LAST_ID, 0, ZSTR_FUEL, UNIT_PERCENT, 0 },
  { ALT_FIRST_ID, ALT_LAST_ID, 0, ZSTR_ALT, UNIT_METERS, 2 },
  { VARIO_FIRST_ID, VARIO_LAST_ID, 0, ZSTR_VSPD, UNIT_METERS_PER_SECOND, 2 },
  { ACCX_FIRST_ID, ACCX_LAST_ID, 0, ZSTR_ACCX, UNIT_G, 2 },
  { ACCY_FIRST_ID, ACCY_LAST_ID, 0, ZSTR_ACCY, UNIT_G, 2 },
  { ACCZ_FIRST_ID, ACCZ_LAST_ID, 0, ZSTR_ACCZ, UNIT_G, 2 },
  { CURR_FIRST_ID, CURR_LAST_ID, 0, ZSTR_CURR, UNIT_AMPS, 1 },
  { VFAS_FIRST_ID, VFAS_LAST_ID, 0, ZSTR_VFAS, UNIT_VOLTS, 2 },
  { AIR_SPEED_FIRST_ID, AIR_SPEED_LAST_ID, 0, ZSTR_ASPD, UNIT_KTS, 1 },
  { GPS_SPEED_FIRST_ID, GPS_SPEED_LAST_ID, 0, ZSTR_GSPD, UNIT_KTS, 3 },
  { CELLS_FIRST_ID, CELLS_LAST_ID, 0, ZSTR_CELLS, UNIT_CELLS, 2 },
  { GPS_ALT_FIRST_ID, GPS_ALT_LAST_ID, 0, ZSTR_GPSALT, UNIT_METERS, 2 },
  { GPS_TIME_DATE_FIRST_ID, GPS_TIME_DATE_LAST_ID, 0, ZSTR_GPSDATETIME, UNIT_DATETIME, 0 },
  { GPS_LONG_LATI_FIRST_ID, GPS_LONG_LATI_LAST_ID, 0, ZSTR_GPS, UNIT_GPS, 0 },
  { FUEL_QTY_FIRST_ID, FUEL_QTY_LAST_ID, 0, ZSTR_FUEL, UNIT_MILLILITERS, 2 },
  { GPS_COURS_FIRST_ID, GPS_COURS_LAST_ID, 0, ZSTR_HDG, UNIT_DEGREE, 2 },
  { POWERBOX_BATT1_FIRST_ID, POWERBOX_BATT1_LAST_ID, 0, ZSTR_BATT1_VOLTAGE, UNIT_VOLTS, 3 },
  { POWERBOX_BATT2_FIRST_ID, POWERBOX_BATT2_LAST_ID, 0, ZSTR_BATT2_VOLTAGE, UNIT_VOLTS, 3 },
  { POWERBOX_BATT1_FIRST_ID, POWERBOX_BATT1_LAST_ID, 1, ZSTR_BATT1_CURRENT, UNIT_AMPS, 2 },
  { POWERBOX_BATT2_FIRST_ID, POWERBOX_BATT2_LAST_ID, 1, ZSTR_BATT2_CURRENT, UNIT_AMPS, 2 },
  { POWERBOX_CNSP_FIRST_ID, POWERBOX_CNSP_LAST_ID, 0, ZSTR_BATT1_CONSUMPTION, UNIT_MAH, 0 },
  { POWERBOX_CNSP_FIRST_ID, POWERBOX_CNSP_LAST_ID, 1, ZSTR_BATT2_CONSUMPTION, UNIT_MAH, 0 },
  { POWERBOX_STATE_FIRST_ID, POWERBOX_STATE_LAST_ID, 0, ZSTR_RX1_FAILSAFE, UNIT_RAW, 0 },
  { POWERBOX_STATE_FIRST_ID, POWERBOX_STATE_LAST_ID, 1, ZSTR_RX1_LOSTFRAME, UNIT_RAW, 0 },
  { POWERBOX_STATE_FIRST_ID, POWERBOX_STATE_LAST_ID, 2, ZSTR_RX2_FAILSAFE, UNIT_RAW, 0 },
  { POWERBOX_STATE_FIRST_ID, POWERBOX_STATE_LAST_ID, 3, ZSTR_RX2_LOSTFRAME, UNIT_RAW, 0 },
  { POWERBOX_STATE_FIRST_ID, POWERBOX_STATE_LAST_ID, 4, ZSTR_RX1_CONN_LOST, UNIT_RAW, 0 },
  { POWERBOX_STATE_FIRST_ID, POWERBOX_STATE_LAST_ID, 5, ZSTR_RX2_CONN_LOST, UNIT_RAW, 0 },
  { POWERBOX_STATE_FIRST_ID, POWERBOX_STATE_LAST_ID, 6, ZSTR_RX1_NO_SIGNAL, UNIT_RAW, 0 },
  { POWERBOX_STATE_FIRST_ID, POWERBOX_STATE_LAST_ID, 7, ZSTR_RX2_NO_SIGNAL, UNIT_RAW, 0 },
  { 0, 0, 0, NULL, UNIT_RAW, 0 } // sentinel
};

const FrSkySportSensor * getFrSkySportSensor(uint16_t id, uint8_t subId=0)
{
  const FrSkySportSensor * result = NULL;
  for (const FrSkySportSensor * sensor = sportSensors; sensor->firstId; sensor++) {
    if (id >= sensor->firstId && id <= sensor->lastId && subId == sensor->subId) {
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
  // TRACE("crc: 0x%02x", crc);
  return (crc == 0x00ff);
}

#define SPORT_DATA_U8(packet)   (packet[4])
#define SPORT_DATA_S32(packet)  (*((int32_t *)(packet+4)))
#define SPORT_DATA_U32(packet)  (*((uint32_t *)(packet+4)))
#define HUB_DATA_U16(packet)    (*((uint16_t *)(packet+4)))

enum SportUpdateState {
  SPORT_IDLE,
  SPORT_POWERUP_REQ,
  SPORT_POWERUP_ACK,
  SPORT_VERSION_REQ,
  SPORT_VERSION_ACK,
  SPORT_DATA_TRANSFER,
  SPORT_DATA_REQ,
  SPORT_COMPLETE,
  SPORT_FAIL
};

uint8_t  sportUpdateState = SPORT_IDLE;
uint32_t sportUpdateAddr = 0;
bool intPwr, extPwr;

void processSportUpdatePacket(uint8_t *packet)
{
  if (packet[0]==0x5E && packet[1]==0x50) {
    switch (packet[2]) {
      case PRIM_ACK_POWERUP :
        if (sportUpdateState == SPORT_POWERUP_REQ) {
          sportUpdateState = SPORT_POWERUP_ACK;
        }
        break;

      case PRIM_ACK_VERSION:
        if (sportUpdateState == SPORT_VERSION_REQ) {
          sportUpdateState = SPORT_VERSION_ACK;
          // SportVersion[0] = packet[3] ;
          // SportVersion[1] = packet[4] ;
          // SportVersion[2] = packet[5] ;
          // SportVersion[3] = packet[6] ;
          // SportVerValid = 1 ;
        }
        break;

      case PRIM_REQ_DATA_ADDR :
        if (sportUpdateState == SPORT_DATA_TRANSFER) {
          sportUpdateAddr = *((uint32_t *)(&packet[3]));
          sportUpdateState = SPORT_DATA_REQ;
        }
        break;

      case PRIM_END_DOWNLOAD :
        sportUpdateState = SPORT_COMPLETE ;
        break;
				
      case PRIM_DATA_CRC_ERR :
        sportUpdateState = SPORT_FAIL ;
        break;
    }
  }
}

void processSportPacket(uint16_t id, uint8_t subId, uint8_t instance, uint32_t data)
{
  const FrSkySportSensor * sensor = getFrSkySportSensor(id, subId);
  TelemetryUnit unit = UNIT_RAW;
  uint8_t precision = 0;
  if (sensor) {
    unit = sensor->unit;
    precision = sensor->prec;
  }
  if (unit == UNIT_CELLS) {
    uint8_t cellsCount = (data & 0xF0) >> 4;
    uint8_t cellIndex = (data & 0x0F);
    uint32_t mask = (cellsCount << 24) + (cellIndex << 16);
    setTelemetryValue(TELEM_PROTO_FRSKY_SPORT, id, subId, instance, mask + (((data & 0x000FFF00) >> 8) / 5), unit, precision);
    if (cellIndex+1 < cellsCount) {
      mask += (1 << 16);
      setTelemetryValue(TELEM_PROTO_FRSKY_SPORT, id, subId, instance, mask + (((data & 0xFFF00000) >> 20) / 5), unit, precision);
    }
  }
  else {
    setTelemetryValue(TELEM_PROTO_FRSKY_SPORT, id, subId, instance, data, unit, precision);
  }
}

void processSportPacket(uint8_t * packet)
{
  uint8_t  instance = (packet[0] & 0x1F) + 1;
  uint8_t  prim   = packet[1];
  uint16_t id  = *((uint16_t *)(packet+2));

#if defined(PCBTARANIS) && !defined(SIMU)
  if (sportUpdateState != SPORT_IDLE) {
    processSportUpdatePacket(packet);	// Uses different chksum
    return;
  }
#endif

  if (!checkSportPacket(packet)) {
    TRACE("processSportPacket(): checksum error ");
    DUMP(packet, FRSKY_SPORT_PACKET_SIZE);
    return;
  }

  if (prim == DATA_FRAME)  {
    uint32_t data = SPORT_DATA_S32(packet);

    if (id == RSSI_ID) {
      frskyStreaming = FRSKY_TIMEOUT10ms; // reset counter only if valid frsky packets are being detected
      data = SPORT_DATA_U8(packet);
      if (data == 0)
        frskyData.rssi.reset();
      else
        frskyData.rssi.set(data);
    }
#if defined(PCBTARANIS) && defined(REVPLUS)
    else if (id == XJT_VERSION_ID) {
      frskyData.xjtVersion = HUB_DATA_U16(packet);
      if (!IS_VALID_XJT_VERSION()) {
        frskyData.swr.set(0x00);
      }
    }
    else if (id == SWR_ID) {
      if (IS_VALID_XJT_VERSION())
        frskyData.swr.set(SPORT_DATA_U8(packet));
      else
        frskyData.swr.set(0x00);
    }
#else
    else if (id == XJT_VERSION_ID) {
      frskyData.xjtVersion = HUB_DATA_U16(packet);
    }
    else if (id == SWR_ID) {
      frskyData.swr.set(SPORT_DATA_U8(packet));
    }
#endif

    if (TELEMETRY_STREAMING()/* because when Rx is OFF it happens that some old A1/A2 values are sent from the XJT module*/) {
      if ((id >> 8) == 0) {
        // The old FrSky IDs
        processHubPacket(id, HUB_DATA_U16(packet));
      }
      else if (!IS_HIDDEN_TELEMETRY_VALUE(id)) {
        if (id == ADC1_ID || id == ADC2_ID || id == BATT_ID || id == SWR_ID) {
          data = SPORT_DATA_U8(packet);
        }
        if (id >= POWERBOX_BATT1_FIRST_ID && id <= POWERBOX_BATT2_LAST_ID) {
          processSportPacket(id, 0, instance, data & 0xffff);
          processSportPacket(id, 1, instance, data >> 16);
        }
        else if (id >= POWERBOX_CNSP_FIRST_ID && id <= POWERBOX_CNSP_LAST_ID) {
          processSportPacket(id, 0, instance, data & 0xffff);
          processSportPacket(id, 1, instance, data >> 16);
        }
        else if (id >= POWERBOX_STATE_FIRST_ID && id <= POWERBOX_STATE_LAST_ID) {
          processSportPacket(id, 0, instance, bool(data & 0x0080000));
          processSportPacket(id, 1, instance, bool(data & 0x0100000));
          processSportPacket(id, 2, instance, bool(data & 0x0200000));
          processSportPacket(id, 3, instance, bool(data & 0x0400000));
          processSportPacket(id, 4, instance, bool(data & 0x0800000));
          processSportPacket(id, 5, instance, bool(data & 0x1000000));
          processSportPacket(id, 6, instance, bool(data & 0x2000000));
          processSportPacket(id, 7, instance, bool(data & 0x4000000));
        }
        else {
          processSportPacket(id, 0, instance, data);
        }
      }
    }
  }
}

void frskySportSetDefault(int index, uint16_t id, uint8_t subId, uint8_t instance)
{
  TelemetrySensor & telemetrySensor = g_model.telemetrySensors[index];

  telemetrySensor.id = id;
  telemetrySensor.subId = subId;
  telemetrySensor.instance = instance;

  const FrSkySportSensor * sensor = getFrSkySportSensor(id, subId);
  if (sensor) {
    TelemetryUnit unit = sensor->unit;
    uint8_t prec = min<uint8_t>(2, sensor->prec);
    telemetrySensor.init(sensor->name, unit, prec);
    if (id == RSSI_ID) {
      telemetrySensor.logs = true;
    }
    else if (id >= ADC1_ID && id <= BATT_ID) {
      telemetrySensor.custom.ratio = 132;
      telemetrySensor.filter = 1;
    }
    else if (id >= CURR_FIRST_ID && id <= CURR_LAST_ID) {
      telemetrySensor.onlyPositive = 1;
    }
    else if (id >= ALT_FIRST_ID && id <= ALT_LAST_ID) {
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

#if defined(PCBTARANIS)
bool sportWaitState(SportUpdateState state, int timeout)
{
#if defined(SIMU)
    SIMU_SLEEP_NORET(1);
    return true;
#else
  for (int i=timeout/2; i>=0; i--) {
    uint8_t data ;
    while (telemetryFifo.pop(data)) {
      processSerialData(data);
    }
    if (sportUpdateState == state) {
      return true;
    }
    else if (sportUpdateState == SPORT_FAIL) {
      return false;
    }
    CoTickDelay(1);
  }
  return false;
#endif
}

void blankPacket(uint8_t *packet)
{
  memset(packet+2, 0, 6);
}

uint8_t sportUpdatePacket[16];

void writePacket(uint8_t * packet)
{
  uint8_t *ptr = sportUpdatePacket;
  *ptr++ = 0x7E;
  *ptr++ = 0xFF;
  packet[7] = crc16(packet, 7);
  for (int i=0; i<8; i++) {
    if (packet[i] == 0x7E || packet[i] == 0x7D) {
      *ptr++ = 0x7D;
      *ptr++ = 0x20 ^ packet[i];
    }
    else {
      *ptr++ = packet[i];
    }
  }
  sportSendBuffer(sportUpdatePacket, ptr-sportUpdatePacket);
}

const char * sportUpdatePowerOn(ModuleIndex module)
{
  uint8_t packet[8];

  sportUpdateState = SPORT_POWERUP_REQ;

  sportWaitState(SPORT_IDLE, 500); // Clear the fifo

  telemetryPortInit(FRSKY_SPORT_BAUDRATE);

#if defined(PCBTARANIS)
  if (module == INTERNAL_MODULE)
    INTERNAL_MODULE_ON();
  else
    EXTERNAL_MODULE_ON();
#endif

  sportWaitState(SPORT_IDLE, 50); // Clear the fifo

  for (int i=0; i<10; i++) {
    // max 10 attempts
    blankPacket(packet);
    packet[0] = 0x50 ;
    packet[1] = PRIM_REQ_POWERUP;
    writePacket(packet);
    if (sportWaitState(SPORT_POWERUP_ACK, 100))
      return NULL;
  }
  return "Module not responding";
}

const char * sportUpdateReqVersion()
{
  uint8_t packet[8];
  sportWaitState(SPORT_IDLE, 20); // Clear the fifo
  sportUpdateState = SPORT_VERSION_REQ;
  for (int i=0; i<10; i++) {
    // max 10 attempts
    blankPacket(packet) ;
    packet[0] = 0x50 ;
    packet[1] = PRIM_REQ_VERSION ;
    writePacket(packet);
    if (sportWaitState(SPORT_VERSION_ACK, 200))
      return NULL;
  }
  return "Version request failed";
}

const char * sportUpdateUploadFile(const char *filename)
{
  FIL file;
  uint32_t buffer[1024/4];
  UINT count;
  uint8_t packet[8];

  if (f_open(&file, filename, FA_READ) != FR_OK) {
    return "Error opening file";
  }

  sportWaitState(SPORT_IDLE, 200); // Clear the fifo
  sportUpdateState = SPORT_DATA_TRANSFER;
  blankPacket(packet) ;
  packet[0] = 0x50 ;
  packet[1] = PRIM_CMD_DOWNLOAD ;
  // Stop here for testing
  writePacket(packet);

  while(1) {
    if (f_read(&file, buffer, 1024, &count) != FR_OK) {
      f_close(&file);
      return "Error reading file";
    }

    count >>= 2;

    for (UINT i=0; i<count; i++) {
      if (!sportWaitState(SPORT_DATA_REQ, 2000)) {
        return "Module refused data";
      }
      packet[0] = 0x50 ;
      packet[1] = PRIM_DATA_WORD ;
      packet[6] = sportUpdateAddr & 0x000000FF;
      uint32_t offset = ( sportUpdateAddr & 1023 ) >> 2;           // 32 bit word offset into buffer
      uint32_t *data = (uint32_t *)(&packet[2]);
      *data = buffer[offset];
      sportUpdateState = SPORT_DATA_TRANSFER,
      writePacket(packet);
      if (i==0) {
        updateProgressBar(file.fptr, file.fsize);
      }
    }

    if (count < 256) {
      f_close(&file);
      return NULL;
    }
  }
}

const char * sportUpdateEnd()
{
  uint8_t packet[8];
  if (!sportWaitState(SPORT_DATA_REQ, 2000))
    return "Module refused data";
  blankPacket(packet);
  packet[0] = 0x50 ;
  packet[1] = PRIM_DATA_EOF;
  writePacket(packet);
  if (!sportWaitState(SPORT_COMPLETE, 2000)) {
    return "Module rejected firmware";
  }
  return NULL;
}

void sportFirmwareUpdate(ModuleIndex module, const char *filename)
{
  pausePulses();

  lcd_clear();
  displayProgressBar(STR_WRITING);

#if defined(PCBTARANIS)
  uint8_t intPwr = IS_INTERNAL_MODULE_ON();
  uint8_t extPwr = IS_EXTERNAL_MODULE_ON();
  INTERNAL_MODULE_OFF();
  EXTERNAL_MODULE_OFF();
#endif

  const char * result = sportUpdatePowerOn(module);
  if (!result) result = sportUpdateReqVersion();
  if (!result) result = sportUpdateUploadFile(filename);
  if (!result) result = sportUpdateEnd();

  if (result) {
    POPUP_WARNING("Firmware Update Error");
    SET_WARNING_INFO(result, strlen(result), 0);
  }

#if defined(PCBTARANIS)
  INTERNAL_MODULE_OFF();
  EXTERNAL_MODULE_OFF();
#endif

  sportWaitState(SPORT_IDLE, 500); // Clear the fifo

#if defined(PCBTARANIS)
  if (intPwr)
    INTERNAL_MODULE_ON();
  if (extPwr)
    EXTERNAL_MODULE_ON();
#endif

  sportUpdateState = SPORT_IDLE;

  resumePulses();
}
#endif
