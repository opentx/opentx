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

#if !defined(DISABLE_MULTI_UPDATE)
#include "opentx.h"
#include "multi_firmware_update.h"
#include "stk500.h"
#include "debug.h"

#define UPDATE_MULTI_EXT_BIN ".bin"

//#define DEBUG_EXT_MODULE_FLASH

class MultiFirmwareUpdateDriver
{
  public:
    MultiFirmwareUpdateDriver() = default;
    const char* flashFirmware(FIL* file, const char* label) const;

  protected:
    virtual void init() const = 0;
    virtual bool getByte(uint8_t& byte) const = 0;
    virtual void sendByte(uint8_t byte) const = 0;
    virtual void clear() const = 0;

  private:
    bool getRxByte(uint8_t& byte) const;
    bool checkRxByte(uint8_t byte) const;
    const char * waitForInitialSync() const;
    const char * getDeviceSignature(uint8_t* signature) const;
    const char * loadAddress(uint32_t offset) const;
    const char * progPage(uint8_t* buffer, uint16_t size) const;
};

#if defined(INTERNAL_MODULE_MULTI)

class MultiInternalUpdateDriver: public MultiFirmwareUpdateDriver
{
  public:
    MultiInternalUpdateDriver() = default;

  protected:
    void init() const override
    {
      INTERNAL_MODULE_ON();
      intmoduleSerialStart(57600, true, USART_Parity_No, USART_StopBits_1, USART_WordLength_8b);
    }

    bool getByte(uint8_t& byte) const override
    {
      return intmoduleFifo.pop(byte);
    }

    void sendByte(uint8_t byte) const override
    {
      intmoduleSendByte(byte);
    }

    void clear() const override
    {
      intmoduleFifo.clear();
    }
};

static const MultiInternalUpdateDriver multiInternalUpdateDriver;

#endif

class MultiExternalUpdateDriver: public MultiFirmwareUpdateDriver
{
  public:
    MultiExternalUpdateDriver() = default;

  protected:
    void init() const override
    {
#if !defined(EXTMODULE_USART)
      GPIO_InitTypeDef GPIO_InitStructure;
      GPIO_InitStructure.GPIO_Pin = EXTMODULE_TX_GPIO_PIN;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
      GPIO_Init(EXTMODULE_TX_GPIO, &GPIO_InitStructure);
#endif

      EXTERNAL_MODULE_ON();
      telemetryPortInit(57600, TELEMETRY_SERIAL_DEFAULT);
    }

    bool getByte(uint8_t& byte) const override
    {
      return telemetryGetByte(&byte);
    }

    void sendByte(uint8_t byte) const override
    {
      extmoduleSendInvertedByte(byte);
    }

    void clear() const override
    {
      telemetryClearFifo();
    }
};

static const MultiExternalUpdateDriver multiExternalUpdateDriver;

bool MultiFirmwareUpdateDriver::getRxByte(uint8_t& byte) const
{
  uint16_t time;

  time = getTmr2MHz() ;
  while ( (uint16_t) (getTmr2MHz() - time) < 25000 ) {  // 12.5mS

    if (getByte(byte)) {
#if defined(DEBUG_EXT_MODULE_FLASH)
      TRACE("[RX] 0x%X", byte);
#endif
      return true;
    }
  }

  byte = 0;
  return false;
}

bool MultiFirmwareUpdateDriver::checkRxByte(uint8_t byte) const
{
  uint8_t rxchar;
  return getRxByte(rxchar) ? rxchar == byte : false;
}

const char * MultiFirmwareUpdateDriver::waitForInitialSync() const
{
  clear();

  uint8_t byte;
  int retries = 1000;
  do {
    // Send sync request
    sendByte(STK_GET_SYNC);
    sendByte(CRC_EOP);

    getRxByte(byte);
    wdt_reset();

  } while((byte != STK_INSYNC) && --retries);

  if (!retries) {
    return "NoSync";
  }

  if (byte != STK_INSYNC) {
    return "NoSync";
  }

  if (!checkRxByte(STK_OK)) {
    return "NoSync";
  }

  return nullptr;
}

const char * MultiFirmwareUpdateDriver::getDeviceSignature(uint8_t* signature) const
{
  // Read signature
  sendByte(STK_READ_SIGN);
  sendByte(CRC_EOP);

  if (!checkRxByte(STK_INSYNC))
    return "NoSync";

  for (uint8_t i=0; i<4; i++) {
    if (!getRxByte(signature[i])) {
      return "NoSignature";
    }
  }

  return nullptr;
}

const char * MultiFirmwareUpdateDriver::loadAddress(uint32_t offset) const
{
  sendByte(STK_LOAD_ADDRESS);
  sendByte(offset & 0xFF); // low  byte
  sendByte(offset >> 8);   // high byte
  sendByte(CRC_EOP);

  if (!checkRxByte(STK_INSYNC) || !checkRxByte(STK_OK)) {
    return "NoSync";
  }

  return nullptr;
}

const char * MultiFirmwareUpdateDriver::progPage(uint8_t* buffer, uint16_t size) const
{
  sendByte(STK_PROG_PAGE);
  // page size 256
  sendByte(1);
  sendByte(0);
  // flash/eeprom flag
  sendByte(0);

  // #if defined(DEBUG_EXT_MODULE_FLASH)
  //   TRACE("writing at 0x%X", writeOffset << 1);
  // #endif

  for (uint16_t i=0; i < size; i++) {
    sendByte(buffer[i]);
  }
  sendByte(CRC_EOP);

  if (!checkRxByte(STK_INSYNC))
    return "NoSync";

  uint8_t byte;
  uint8_t retries = 4;
  do {
    getRxByte(byte);
    wdt_reset();
  } while(!byte && --retries);

  if (!retries || (byte != STK_OK))
    return "NoPageSync";

  return nullptr;
}

const char * MultiFirmwareUpdateDriver::flashFirmware(FIL* file, const char* label) const
{
  const char* result = nullptr;
  init();

  /* wait 500ms for power on */
  watchdogSuspend(500);
  RTOS_WAIT_MS(500);

  result = waitForInitialSync();
  if (result != nullptr)
    return result;

  unsigned char signature[4]; // 3 bytes signature + STK_OK
  result = getDeviceSignature(signature);
  if (result != nullptr)
    return result;

  uint8_t  buffer[256]; // page size = 256
  uint32_t writeOffset = 0x1000; // start offset (word address)

  while (!f_eof(file)) {

    drawProgressScreen(label, STR_WRITING, file->fptr, file->obj.objsize);

    UINT count=0;
    memclear(buffer, 256);
    if (f_read(file, buffer, 256, &count) != FR_OK) {
      result = "Error reading file";
      break;
    }

    if (!count)
      break;

    clear();

    result = loadAddress(writeOffset);
    if (result != nullptr) {
      break;
    }

    result = progPage(buffer, 256);
    if (result != nullptr) {
      break;
    }

    writeOffset += 128; // page / 2
  }

  if (f_eof(file)) {
    drawProgressScreen(label, STR_WRITING, file->fptr, file->obj.objsize);
  }

  sendByte(STK_LEAVE_PROGMODE);
  sendByte(CRC_EOP);

  // eat last sync byte
  checkRxByte(STK_INSYNC);

  return result;
}

// example :  multi-stm-bcsid-01020176
#define MULTI_SIGN_SIZE                             24
#define MULTI_SIGN_BOOTLOADER_SUPPORT_OFFSET        10
#define MULTI_SIGN_BOOTLOADER_CHECK_OFFSET          11
#define MULTI_SIGN_TELEM_TYPE_OFFSET                12
#define MULTI_SIGN_TELEM_INVERSION_OFFSET           13
#define MULTI_SIGN_VERSION_OFFSET                   15

const char * MultiFirmwareInformation::readMultiFirmwareInformation(const char * filename)
{
  FIL file;
  f_open(&file, filename, FA_READ);
  char buffer[MULTI_SIGN_SIZE];
  UINT count;

  f_lseek(&file, f_size(&file) - MULTI_SIGN_SIZE);
  if (f_read(&file, buffer, MULTI_SIGN_SIZE, &count) != FR_OK || count != MULTI_SIGN_SIZE) {
    return "Error reading file";
  }

  if(!memcmp(buffer, "multi-stm", 9))
    boardType = FIRMWARE_MULTI_STM;
  else if (!memcmp(buffer, "multi-avr", 9))
    boardType = FIRMWARE_MULTI_AVR;
  else if (!memcmp(buffer, "multi-orx", 9))
    boardType = FIRMWARE_MULTI_ORX;
  else
    return "Wrong format";

  if(buffer[MULTI_SIGN_BOOTLOADER_SUPPORT_OFFSET] == 'b')
    optibootSupport = true;
  else
    optibootSupport = false;

  if(buffer[MULTI_SIGN_BOOTLOADER_CHECK_OFFSET] == 'c')
    bootloaderCheck = true;
  else
    bootloaderCheck = false;

  if(buffer[MULTI_SIGN_TELEM_TYPE_OFFSET] == 't')
    telemetryType = FIRMWARE_MULTI_TELEM_MULTI_STATUS;
  else if (buffer[MULTI_SIGN_TELEM_TYPE_OFFSET] == 's')
    telemetryType = FIRMWARE_MULTI_TELEM_MULTI_TELEMETRY;
  else
    telemetryType = FIRMWARE_MULTI_TELEM_NONE;

  if(buffer[MULTI_SIGN_TELEM_INVERSION_OFFSET] == 'i')
    telemetryInversion = true;
  else
    telemetryInversion = false;

  return nullptr;
}

bool MultiFlashFirmware(uint8_t moduleIdx, const char * filename)
{
  FIL file;
  const char * result = nullptr;

  MultiFirmwareInformation firmwareFile;
  if (firmwareFile.readMultiFirmwareInformation(filename) != nullptr) {
    POPUP_WARNING("Not a valid file");
    return false;
  }

  if (moduleIdx == EXTERNAL_MODULE) {
    if (!firmwareFile.isMultiExternalFirmware()) {
      POPUP_WARNING(STR_NEEDS_FILE);
      SET_WARNING_INFO(STR_EXT_MULTI_SPEC, strlen(STR_EXT_MULTI_SPEC), 0);
      return false;
    }
  }
  else {
    if (!firmwareFile.isMultiInternalFirmware()) {
      POPUP_WARNING(STR_NEEDS_FILE);
      SET_WARNING_INFO(STR_INT_MULTI_SPEC, strlen(STR_INT_MULTI_SPEC), 0);
      return false;
    }
  }

  if (f_open(&file, filename, FA_READ) != FR_OK) {
    return false;
  }

  pausePulses();

#if defined(INTERNAL_MODULE_MULTI)
  uint8_t intPwr = IS_INTERNAL_MODULE_ON();
  INTERNAL_MODULE_OFF();
#endif

  uint8_t extPwr = IS_EXTERNAL_MODULE_ON();
  EXTERNAL_MODULE_OFF();

  SPORT_UPDATE_POWER_OFF();

  drawProgressScreen(getBasename(filename), STR_DEVICE_RESET, 0, 0);

  /* wait 2s off */
  watchdogSuspend(2000);
  RTOS_WAIT_MS(2000);

  // TODO: real update here
  const MultiFirmwareUpdateDriver* driver = &multiExternalUpdateDriver;
#if defined(INTERNAL_MODULE_MULTI)
  if (moduleIdx == INTERNAL_MODULE)
    driver = &multiInternalUpdateDriver;
#endif

  result = driver->flashFirmware(&file, getBasename(filename));
  f_close(&file);

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

  /* wait 2s off */
  watchdogSuspend(2000);
  RTOS_WAIT_MS(2000);
  telemetryClearFifo();

#if defined(INTERNAL_MODULE_MULTI)
  if (intPwr) {
    INTERNAL_MODULE_ON();
    setupPulsesInternalModule();
  }
#endif

  if (extPwr) {
    EXTERNAL_MODULE_ON();
    setupPulsesExternalModule();
  }

  //state = SPORT_IDLE;
  resumePulses();

  return result;
}
#endif