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

#if defined(STM32)
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

void sportOutputPushByte(uint8_t byte)
{
  if (byte == 0x7E || byte == 0x7D) {
    telemetryOutputPushByte(0x7D);
    telemetryOutputPushByte(0x20 ^ byte);
  }
  else {
    telemetryOutputPushByte(byte);
  }
}

bool isSportOutputBufferAvailable()
{
  return (outputTelemetryBufferSize == 0 && outputTelemetryBufferTrigger == 0x7E);
}

// TODO merge it with S.PORT update function when finished
void sportOutputPushPacket(SportTelemetryPacket * packet)
{
  uint16_t crc = 0;

  for (uint8_t i=1; i<sizeof(SportTelemetryPacket); i++) {
    uint8_t byte = packet->raw[i];
    sportOutputPushByte(byte);
    crc += byte; // 0-1FF
    crc += crc >> 8; // 0-100
    crc &= 0x00ff;
  }

  telemetryOutputPushByte(0xFF-crc);
  telemetryOutputSetTrigger(packet->raw[0]); // physicalId
}

void sportProcessUpdatePacket(uint8_t * packet)
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

bool sportWaitState(SportUpdateState state, int timeout)
{
#if defined(SIMU)
  SIMU_SLEEP_NORET(1);
  return true;
#else
  watchdogSuspend(timeout / 10);
  for (int i=timeout/2; i>=0; i--) {
    uint8_t byte ;
    while (telemetryGetByte(&byte)) {
      processFrskyTelemetryData(byte);
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

void sportClearPacket(uint8_t * packet)
{
  memset(packet+2, 0, 6);
}

// TODO merge this function
void sportWritePacket(uint8_t * packet)
{
  uint8_t * ptr = outputTelemetryBuffer;
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
  sportSendBuffer(outputTelemetryBuffer, ptr-outputTelemetryBuffer);
}

const char * sportUpdatePowerOn(ModuleIndex module)
{
  uint8_t packet[8];

  sportUpdateState = SPORT_POWERUP_REQ;
  sportWaitState(SPORT_IDLE, 500); // Clear the fifo

  telemetryInit(PROTOCOL_FRSKY_SPORT);

#if defined(PCBTARANIS) || defined(PCBHORUS)
  if (module == INTERNAL_MODULE)
    INTERNAL_MODULE_ON();
  else if (module == EXTERNAL_MODULE)
    EXTERNAL_MODULE_ON();
  else
    SPORT_UPDATE_POWER_ON();
#endif

  sportWaitState(SPORT_IDLE, 50); // Clear the fifo

  for (int i=0; i<10; i++) {
    // max 10 attempts
    sportClearPacket(packet);
    packet[0] = 0x50 ;
    packet[1] = PRIM_REQ_POWERUP;
    sportWritePacket(packet);
    if (sportWaitState(SPORT_POWERUP_ACK, 100))
      return NULL;
  }

  if (telemetryProtocol != PROTOCOL_FRSKY_SPORT) {
    return TR("Not responding", "Not S.Port 1");
  }

  if (!IS_FRSKY_SPORT_PROTOCOL()) {
    return TR("Not responding", "Not S.Port 2");
  }
#if defined(PCBX7)
  if (IS_PCBREV_40()) {
    return TR("Bottom pin no resp", "Bottom pin not responding");
  }
  else {
    return TR("Module pin no resp", "Module pin not responding");
  }
#else
  return TR("Not responding", "Module not responding");
#endif
}

const char * sportUpdateReqVersion()
{
  uint8_t packet[8];
  sportWaitState(SPORT_IDLE, 20); // Clear the fifo
  sportUpdateState = SPORT_VERSION_REQ;
  for (int i=0; i<10; i++) {
    // max 10 attempts
    sportClearPacket(packet) ;
    packet[0] = 0x50 ;
    packet[1] = PRIM_REQ_VERSION ;
    sportWritePacket(packet);
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
  sportClearPacket(packet) ;
  packet[0] = 0x50 ;
  packet[1] = PRIM_CMD_DOWNLOAD ;
  // Stop here for testing
  sportWritePacket(packet);

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
      sportWritePacket(packet);
      if (i==0) {
        drawProgressBar(STR_WRITING, file.fptr, file.obj.objsize);
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
  sportClearPacket(packet);
  packet[0] = 0x50 ;
  packet[1] = PRIM_DATA_EOF;
  sportWritePacket(packet);
  if (!sportWaitState(SPORT_COMPLETE, 2000)) {
    return "Module rejected firmware";
  }
  return NULL;
}

void sportFlashDevice(ModuleIndex module, const char * filename)
{
  pausePulses();

#if defined(PCBTARANIS) || defined(PCBHORUS)
  uint8_t intPwr = IS_INTERNAL_MODULE_ON();
  uint8_t extPwr = IS_EXTERNAL_MODULE_ON();
  INTERNAL_MODULE_OFF();
  EXTERNAL_MODULE_OFF();
  SPORT_UPDATE_POWER_OFF();

  /* wait 2s off */
  watchdogSuspend(2000);
  CoTickDelay(1000);
#endif

  const char * result = sportUpdatePowerOn(module);
  if (!result) result = sportUpdateReqVersion();
  if (!result) result = sportUpdateUploadFile(filename);
  if (!result) result = sportUpdateEnd();

  if (result) {
    POPUP_WARNING(STR_FIRMWARE_UPDATE_ERROR);
    SET_WARNING_INFO(result, strlen(result), 0);
  }

#if defined(PCBTARANIS) || defined(PCBHORUS)
  INTERNAL_MODULE_OFF();
  EXTERNAL_MODULE_OFF();
  SPORT_UPDATE_POWER_OFF();
#endif

  sportWaitState(SPORT_IDLE, 500); // Clear the fifo

#if defined(PCBTARANIS) || defined(PCBHORUS)
  if (intPwr)
    INTERNAL_MODULE_ON();
  if (extPwr)
    EXTERNAL_MODULE_ON();
#endif

  sportUpdateState = SPORT_IDLE;

  resumePulses();
}
#endif

void sportProcessPacket(uint8_t * packet)
{
#if defined(STM32)
  if (sportUpdateState != SPORT_IDLE) {
    sportProcessUpdatePacket(packet);	// Uses different chksum
    return;
  }
#endif

  sportProcessTelemetryPacket(packet);
}
