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
#include "frsky_sport_update.h"

#define PRIM_REQ_POWERUP    0
#define PRIM_REQ_VERSION    1
#define PRIM_CMD_DOWNLOAD   3
#define PRIM_DATA_WORD      4
#define PRIM_DATA_EOF       5

#define PRIM_ACK_POWERUP    0x80
#define PRIM_ACK_VERSION    0x81
#define PRIM_REQ_DATA_ADDR  0x82
#define PRIM_END_DOWNLOAD   0x83
#define PRIM_DATA_CRC_ERR   0x84

void FrskyFirmwareUpdate::processFrame(const uint8_t * frame)
{
  if (frame[0] == 0x5E && frame[1] == 0x50) {
    switch (frame[2]) {
      case PRIM_ACK_POWERUP :
        if (state == SPORT_POWERUP_REQ) {
          state = SPORT_POWERUP_ACK;
        }
        break;

      case PRIM_ACK_VERSION:
        if (state == SPORT_VERSION_REQ) {
          state = SPORT_VERSION_ACK;
          // here we could display the version
        }
        break;

      case PRIM_REQ_DATA_ADDR:
        if (state == SPORT_DATA_TRANSFER) {
          address = *((uint32_t *)(&frame[3]));
          state = SPORT_DATA_REQ;
        }
        break;

      case PRIM_END_DOWNLOAD :
        state = SPORT_COMPLETE ;
        break;

      case PRIM_DATA_CRC_ERR :
        state = SPORT_FAIL ;
        break;
    }
  }
}

void FrskyFirmwareUpdate::startup()
{
  switch(module) {
#if defined(INTMODULE_USART)
    case INTERNAL_MODULE:
      intmoduleSerialStart(57600, true);
      break;
#endif

    default:
      telemetryInit(PROTOCOL_TELEMETRY_FRSKY_SPORT);
      break;
  }

  if (module == INTERNAL_MODULE)
    INTERNAL_MODULE_ON();
  else if (module == EXTERNAL_MODULE)
    EXTERNAL_MODULE_ON();
  else
    SPORT_UPDATE_POWER_ON();
}

const uint8_t * FrskyFirmwareUpdate::readFullDuplexFrame(ModuleFifo & fifo, uint32_t timeout)
{
  uint8_t len = 0;
  bool bytestuff = false;
  while (len < 10) {
    uint32_t elapsed = 0;
    uint8_t byte;
    while (!fifo.pop(byte)) {
      RTOS_WAIT_MS(1);
      if (elapsed++ >= timeout) {
        return nullptr;
      }
    }
    if (byte == 0x7D) {
      bytestuff = true;
      continue;
    }
    if (bytestuff) {
      frame[len] = 0x20 ^ byte;
      bytestuff = false;
    }
    else {
      frame[len] = byte;
    }
    if (len > 0 || byte == 0x7E) {
      ++len;
    }
  }
  return &frame[1];
}

const uint8_t * FrskyFirmwareUpdate::readHalfDuplexFrame(uint32_t timeout)
{
  for (int i=timeout; i>=0; i--) {
    uint8_t byte ;
    while (telemetryGetByte(&byte)) {
      if (pushFrskyTelemetryData(byte)) {
        return telemetryRxBuffer;
      }
    }
    RTOS_WAIT_MS(1);
  }
  return nullptr;
}

const uint8_t * FrskyFirmwareUpdate::readFrame(uint32_t timeout)
{
  switch(module) {
#if defined(INTMODULE_USART)
    case INTERNAL_MODULE:
      return readFullDuplexFrame(intmoduleFifo, timeout);
#endif

    default:
      return readHalfDuplexFrame(timeout);
  }
}

bool FrskyFirmwareUpdate::waitState(State newState, uint32_t timeout)
{
#if defined(SIMU)
  UNUSED(state);
  UNUSED(timeout);
  static uint8_t pass = 0;
  if (++pass == 10) {
    pass = 0;
    RTOS_WAIT_MS(1);
  }
  return true;
#else
  watchdogSuspend(timeout / 10);

  const uint8_t * frame = readFrame(timeout);
  if (!frame) {
    return false;
  }

  processFrame(frame);
  return state == newState;
#endif
}

void FrskyFirmwareUpdate::startFrame(uint8_t command)
{
  frame[0] = 0x50;
  frame[1] = command;
  memset(&frame[2], 0, 6);
}

// TODO merge this function
void FrskyFirmwareUpdate::sendFrame()
{
  uint8_t * ptr = outputTelemetryBuffer.data;
  *ptr++ = 0x7E;
  *ptr++ = 0xFF;
  frame[7] = crc16(frame, 7);
  for (int i=0; i<8; i++) {
    if (frame[i] == 0x7E || frame[i] == 0x7D) {
      *ptr++ = 0x7D;
      *ptr++ = 0x20 ^ frame[i];
    }
    else {
      *ptr++ = frame[i];
    }
  }

  switch(module) {
#if defined(INTMODULE_USART)
    case INTERNAL_MODULE:
      return intmoduleSendBuffer(outputTelemetryBuffer.data, ptr - outputTelemetryBuffer.data);
#endif

    default:
      return sportSendBuffer(outputTelemetryBuffer.data, ptr - outputTelemetryBuffer.data);
  }
}

const char * FrskyFirmwareUpdate::sendPowerOn()
{
  state = SPORT_POWERUP_REQ;
  waitState(SPORT_IDLE, 50); // Wait 50ms and clear the fifo
  for (int i=0; i<10; i++) {
    // max 10 attempts
    startFrame(PRIM_REQ_POWERUP);
    sendFrame();
    if (waitState(SPORT_POWERUP_ACK, 100))
      return nullptr;
  }

  if (telemetryProtocol != PROTOCOL_TELEMETRY_FRSKY_SPORT) {
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
    return TR("Device pin no resp", "Device pin not responding");
  }
#else
  return TR("Not responding", "Device not responding");
#endif
}

const char * FrskyFirmwareUpdate::sendReqVersion()
{
  waitState(SPORT_IDLE, 20); // Clear the fifo
  state = SPORT_VERSION_REQ;
  for (int i=0; i<10; i++) {
    // max 10 attempts
    startFrame(PRIM_REQ_VERSION) ;
    sendFrame();
    if (waitState(SPORT_VERSION_ACK, 100))
      return nullptr;
  }
  return "Version request failed";
}

const char * FrskyFirmwareUpdate::uploadFile(const char * filename)
{
  FIL file;
  uint32_t buffer[1024 / sizeof(uint32_t)];
  UINT count;

  if (f_open(&file, filename, FA_READ) != FR_OK) {
    return "Error opening file";
  }

  waitState(SPORT_IDLE, 200); // Clear the fifo
  state = SPORT_DATA_TRANSFER;
  startFrame(PRIM_CMD_DOWNLOAD);
  sendFrame();

  while (1) {
    if (f_read(&file, buffer, 1024, &count) != FR_OK) {
      f_close(&file);
      return "Error reading file";
    }

    count >>= 2;

    for (uint32_t i=0; i<count; i++) {
      if (!waitState(SPORT_DATA_REQ, 2000)) {
        return "Device refused data";
      }
      startFrame(PRIM_DATA_WORD);
      uint32_t offset = (address & 1023) >> 2; // 32 bit word offset into buffer
      *((uint32_t *)(frame + 2)) = buffer[offset];
      frame[6] = address & 0x000000FF;
      state = SPORT_DATA_TRANSFER,
      sendFrame();
      if (i == 0) {
        drawProgressScreen(getBasename(filename), STR_WRITING, file.fptr, file.obj.objsize);
      }
    }

    if (count < 256) {
      f_close(&file);
      return nullptr;
    }
  }
}

const char * FrskyFirmwareUpdate::endTransfer()
{
  if (!waitState(SPORT_DATA_REQ, 2000))
    return "Device refused data";
  startFrame(PRIM_DATA_EOF);
  sendFrame();
  if (!waitState(SPORT_COMPLETE, 2000)) {
    return "Device rejected firmware";
  }
  return nullptr;
}

void FrskyFirmwareUpdate::flashDevice(const char * filename)
{
  pausePulses();

  uint8_t intPwr = IS_INTERNAL_MODULE_ON();
  uint8_t extPwr = IS_EXTERNAL_MODULE_ON();

  drawProgressScreen(getBasename(filename), STR_DEVICE_RESET, 0, 0);

  INTERNAL_MODULE_OFF();
  EXTERNAL_MODULE_OFF();
  SPORT_UPDATE_POWER_OFF();

  /* wait 2s off */
  watchdogSuspend(2000);
  RTOS_WAIT_MS(2000);

  startup();

  const char * result = sendPowerOn();
  if (!result) result = sendReqVersion();
  if (!result) result = uploadFile(filename);
  if (!result) result = endTransfer();

  AUDIO_PLAY(AU_SPECIAL_SOUND_BEEP1 );
  BACKLIGHT_ENABLE();

  if (result) {
    POPUP_WARNING(STR_FIRMWARE_UPDATE_ERROR);
    SET_WARNING_INFO(result, strlen(result), 0);
  }
  else {
    POPUP_INFORMATION(STR_FIRMWARE_UPDATE_SUCCESS);
  }

  INTERNAL_MODULE_OFF();
  EXTERNAL_MODULE_OFF();
  SPORT_UPDATE_POWER_OFF();

  waitState(SPORT_IDLE, 500); // Clear the fifo

  /* wait 2s off */
  watchdogSuspend(2000);
  RTOS_WAIT_MS(2000);

  if (intPwr) {
    INTERNAL_MODULE_ON();
    setupPulses(INTERNAL_MODULE);
  }
  if (extPwr) {
    EXTERNAL_MODULE_ON();
    setupPulses(EXTERNAL_MODULE);
  }

  state = SPORT_IDLE;
  resumePulses();
}

const char * FrskyFirmwareUpdate::waitChipAnswer(uint8_t & status)
{
  telemetryPortSetDirectionInput();

  uint8_t buffer[12];
  for (uint8_t i = 0; i < sizeof(buffer); i++) {
    uint32_t retry = 0;
    while(1) {
      if (telemetryGetByte(&buffer[i])) {
        if ((i == 0 && buffer[0] != 0x7F) ||
            (i == 1 && buffer[1] != 0xFE) ||
            (i == 10 && buffer[10] != 0x0D) ||
            (i == 11 && buffer[11] != 0x0A)) {
          i = 0;
          continue;
        }
        break;
      }
      if (++retry == 20000) {
        return "No answer";
      }
      RTOS_WAIT_MS(1);
    }
  }
  status = buffer[8];
  return nullptr;
}

const char * FrskyFirmwareUpdate::startChipBootloader()
{
  telemetryPortSetDirectionOutput();

  sportSendByte(0x01);

  for (uint8_t i = 0; i < 30; i++)
    sportSendByte(0x7E);

  for (uint32_t i = 0; i < 100; i++) {
    RTOS_WAIT_MS(20);
    sportSendByte(0x7F);
  }

  sportSendByte(0xFA);

  /*for (uint8_t i=0; i < 30; i++)
    sportSendByte(0x7E);
  for (uint8_t i=0; i < 50; i++)
    sportSendByte(0x7F);*/

  uint8_t status;
  const char * result = waitChipAnswer(status);
  if (result)
    return result;

  return status == 0x08 ? nullptr : "Bootloader failed";
}

void FrskyFirmwareUpdate::sendChipByte(uint8_t byte, bool crc)
{
  sportSendByte(byte);
  if (crc) {
    chipCrc ^= byte;
  }
}

const char * FrskyFirmwareUpdate::sendChipUpgradeCommand(char command, uint32_t packetsCount)
{
  telemetryPortSetDirectionOutput();

  chipCrc = 0;

  // Head
  sendChipByte(0x7F, false);
  sendChipByte(0xFE, false);

  // Addr
  sendChipByte(0xFA);

  // Cmd
  sendChipByte(command);

  // Packets count
  sendChipByte(packetsCount >> 8);
  sendChipByte(packetsCount);

  // Len
  sendChipByte('E' == command ? 0x00 : 0x0C);
  sendChipByte(0x40);

  // Data
  for (uint8_t i=0; i < 0x40; i++)
    sendChipByte('E' == command ? 0xF7 : 0x7F);

  // Checksum
  sendChipByte(chipCrc, false);

  // Tail
  sendChipByte(0x0D, false);
  sendChipByte(0x0A, false);

  uint8_t status;
  const char * result = waitChipAnswer(status);
  if (result)
    return result;

  return status == 0x00 ? nullptr : "Upgrade failed";
}

const char * FrskyFirmwareUpdate::sendChipUpgradeData(uint8_t index, uint8_t * data)
{
  telemetryPortSetDirectionOutput();

  chipCrc = 0;

  // Head
  sendChipByte(0x7F, false);
  sendChipByte(0xFE, false);

  // Addr
  sendChipByte(0xFA);

  // Cmd
  sendChipByte('W');

  // Packets count
  sendChipByte(index >> 8);
  sendChipByte(index);

  // Len
  sendChipByte(0x00);
  sendChipByte(0x40);

  // Data
  for (uint8_t i = 0; i < 0x40; i++)
    sendChipByte(*data++);

  // Checksum
  sendChipByte(chipCrc, false);

  // Tail
  sendChipByte(0x0D, false);
  sendChipByte(0x0A, false);

  uint8_t status;
  const char * result = waitChipAnswer(status);
  if (result)
    return result;

  return status == 0x00 ? nullptr : "Upgrade failed";
}

const char * FrskyFirmwareUpdate::doFlashChip(const char * filename)
{
  const char * result;
  FIL file;
  uint8_t buffer[64];
  UINT count;

  result = startChipBootloader();
  if (result)
    return result;

  if (f_open(&file, filename, FA_READ) != FR_OK) {
    return "Error opening file";
  }

  uint32_t packetsCount = (f_size(&file) + sizeof(buffer) - 1) / sizeof(buffer);
  drawProgressScreen(getBasename(filename), STR_FLASH_WRITE, 0, packetsCount);

  result = sendChipUpgradeCommand('A', packetsCount);
  if (result)
    return result;

  uint32_t index = 0;
  while (1) {
    drawProgressScreen(getBasename(filename), STR_FLASH_WRITE, index, packetsCount);
    if (f_read(&file, buffer, sizeof(buffer), &count) != FR_OK) {
      f_close(&file);
      return "Error reading file";
    }
    result = sendChipUpgradeData(index + 1, buffer);
    if (result)
      return result;
    if (++index == packetsCount)
      break;
  }

  f_close(&file);

  return sendChipUpgradeCommand('E', packetsCount);
}

void FrskyFirmwareUpdate::flashChip(const char * filename)
{
  drawProgressScreen(getBasename(filename), STR_DEVICE_RESET, 0, 0);

  pausePulses();

  uint8_t intPwr = IS_INTERNAL_MODULE_ON();
  uint8_t extPwr = IS_EXTERNAL_MODULE_ON();

  INTERNAL_MODULE_OFF();
  EXTERNAL_MODULE_OFF();
  SPORT_UPDATE_POWER_OFF();

  /* wait 2s off */
  watchdogSuspend(2000);
  RTOS_WAIT_MS(2000);

  telemetryInit(PROTOCOL_TELEMETRY_FRSKY_SPORT);

  const char * result = doFlashChip(filename);

  AUDIO_PLAY(AU_SPECIAL_SOUND_BEEP1 );
  BACKLIGHT_ENABLE();

  if (result) {
    POPUP_WARNING(STR_FIRMWARE_UPDATE_ERROR);
    SET_WARNING_INFO(result, strlen(result), 0);
  }
  else {
    POPUP_INFORMATION(STR_FIRMWARE_UPDATE_SUCCESS);
  }

  INTERNAL_MODULE_OFF();
  EXTERNAL_MODULE_OFF();
  SPORT_UPDATE_POWER_OFF();

  waitState(SPORT_IDLE, 500); // Clear the fifo

  /* wait 2s off */
  watchdogSuspend(2000);
  RTOS_WAIT_MS(2000);

  if (intPwr) {
    INTERNAL_MODULE_ON();
    setupPulses(INTERNAL_MODULE);
  }
  if (extPwr) {
    EXTERNAL_MODULE_ON();
    setupPulses(EXTERNAL_MODULE);
  }

  state = SPORT_IDLE;
  resumePulses();
}
