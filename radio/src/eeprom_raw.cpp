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

#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include "opentx.h"
#include "timers.h"

#define EEPROM_SIZE           (4*1024*1024/8)
#define EEPROM_BLOCK_SIZE     (4*1024)
#define EEPROM_MARK           0x84697771 /* thanks ;) */
#define EEPROM_ZONE_SIZE      (8*1024)
#define EEPROM_BUFFER_SIZE    256
#define EEPROM_FAT_SIZE       128
#define EEPROM_MAX_ZONES      (EEPROM_SIZE / EEPROM_ZONE_SIZE)
#define EEPROM_MAX_FILES      (EEPROM_MAX_ZONES - 1)
#define FIRST_FILE_AVAILABLE  (1+MAX_MODELS)

PACK(struct EepromHeaderFile
{
  uint8_t zoneIndex:7;
  uint8_t exists:1;
});

PACK(struct EepromHeader
{
  uint32_t         mark;
  uint32_t         index;
  EepromHeaderFile files[EEPROM_MAX_FILES];
});

PACK(struct EepromFileHeader
{
  uint16_t fileIndex;
  uint16_t size;
});

EepromHeader eepromHeader __DMA;
volatile EepromWriteState eepromWriteState = EEPROM_IDLE;
uint8_t eepromWriteZoneIndex = FIRST_FILE_AVAILABLE;
uint8_t eepromWriteFileIndex;
uint16_t eepromWriteSize;
uint8_t * eepromWriteSourceAddr;
uint32_t eepromWriteDestinationAddr;
uint16_t eepromFatAddr = 0;
uint8_t eepromWriteBuffer[EEPROM_BUFFER_SIZE] __DMA;

void eepromWaitSpiComplete()
{
  while (!Spi_complete) {
    SIMU_SLEEP(5/*ms*/);
  }
  Spi_complete = false;
}

void eepromWaitReadStatus()
{
  while ((eepromReadStatus() & 1) != 0) {
    SIMU_SLEEP(5/*ms*/);
  }
}

void eepromEraseBlock(uint32_t address, bool blocking=true)
{
  // TRACE("eepromEraseBlock(%d)", address);

#if defined(SIMU)
  static uint8_t erasedBlock[EEPROM_BLOCK_SIZE]; // can't be on the stack!
  memset(erasedBlock, 0xff, sizeof(erasedBlock));
  eeprom_pointer = address;
  eeprom_buffer_data = erasedBlock;
  eeprom_buffer_size = EEPROM_BLOCK_SIZE;
  eeprom_read_operation = false;
  Spi_complete = false;
  sem_post(eeprom_write_sem);
#else
  eepromWriteEnable();
  eepromBlockErase(address);
#endif

  if (blocking) {
    eepromWaitSpiComplete();
    eepromWaitReadStatus();
  }
}

void eepromRead(uint32_t address, uint8_t * buffer, uint32_t size, bool blocking=true)
{
  // TRACE("eepromRead(%d, %p, %d)", address, buffer, size);

#if defined(SIMU)
  assert(size);
  eeprom_pointer = address;
  eeprom_buffer_data = buffer;
  eeprom_buffer_size = size;
  eeprom_read_operation = true;
  Spi_complete = false;
  sem_post(eeprom_write_sem);
#else
  eepromReadArray(address, buffer, size);
#endif

  if (blocking) {
    eepromWaitSpiComplete();
  }
}

void eepromWrite(uint32_t address, uint8_t * buffer, uint32_t size, bool blocking=true)
{
  // TRACE("eepromWrite(%d, %p, %d)", address, buffer, size);

#if defined(SIMU)
  assert(size);
  eeprom_pointer = address;
  eeprom_buffer_data = buffer;
  eeprom_buffer_size = size+1;
  eeprom_read_operation = false;
  Spi_complete = false;
  sem_post(eeprom_write_sem);
#else
  eepromWriteEnable();
  eepromByteProgram(address, buffer, size);
#endif

  if (blocking) {
    eepromWaitSpiComplete();
    eepromWaitReadStatus();
  }
}

bool eepromOpen()
{
  int32_t bestFatAddr = -1;
  uint32_t bestFatIndex = 0;
  eepromFatAddr = 0;
  while (eepromFatAddr < EEPROM_ZONE_SIZE) {
    eepromRead(eepromFatAddr, (uint8_t *)&eepromHeader, sizeof(eepromHeader.mark) + sizeof(eepromHeader.index));
    if (eepromHeader.mark == EEPROM_MARK && eepromHeader.index >= bestFatIndex) {
      bestFatAddr = eepromFatAddr;
      bestFatIndex = eepromHeader.index;
    }
    eepromFatAddr += EEPROM_FAT_SIZE;
  }
  if (bestFatAddr >= 0) {
    eepromFatAddr = bestFatAddr;
    eepromRead(eepromFatAddr, (uint8_t *)&eepromHeader, sizeof(eepromHeader));  
    return true;
  }
  else {
    return false;
  }
}

uint32_t readFile(int index, uint8_t * data, uint32_t size)
{
  if (eepromHeader.files[index].exists) {
    EepromFileHeader header;
    uint32_t address = eepromHeader.files[index].zoneIndex * EEPROM_ZONE_SIZE;
    eepromRead(address, (uint8_t *)&header, sizeof(header));
    if (size < header.size) {
      header.size = size;
    }
    if (header.size > 0) {
      eepromRead(address + sizeof(header), data, header.size);
      size -= header.size;
    }
    if (size > 0) {
      memset(data + header.size, 0, size);
    }
    return header.size;
  }
  else {
    return 0;
  }
}

void eepromIncFatAddr()
{
  eepromHeader.index += 1;
  eepromFatAddr += EEPROM_FAT_SIZE;
  if (eepromFatAddr >= EEPROM_ZONE_SIZE) {
    eepromFatAddr = 0;
  }
}

void writeFile(int index, uint8_t * data, uint32_t size)
{
  uint32_t zoneIndex = eepromHeader.files[eepromWriteZoneIndex].zoneIndex;
  eepromHeader.files[eepromWriteZoneIndex].exists = 0;
  eepromHeader.files[eepromWriteZoneIndex].zoneIndex = eepromHeader.files[index].zoneIndex;
  eepromHeader.files[index].exists = (size > 0);
  eepromHeader.files[index].zoneIndex = zoneIndex;
  eepromWriteFileIndex = index;
  eepromWriteSourceAddr = data;
  eepromWriteSize = size;
  eepromWriteDestinationAddr = zoneIndex * EEPROM_ZONE_SIZE;
  eepromWriteState = EEPROM_START_WRITE;
  eepromWriteZoneIndex += 1;
  if (eepromWriteZoneIndex >= EEPROM_MAX_FILES) {
    eepromWriteZoneIndex = FIRST_FILE_AVAILABLE;
  }
  eepromIncFatAddr();
}

void eeDeleteModel(uint8_t index)
{
  eeCheck(true);
  memclear(&modelHeaders[index], sizeof(ModelHeader));
  writeFile(index+1, (uint8_t *)&g_model, 0);
  eepromWriteWait();
}

bool eeCopyModel(uint8_t dst, uint8_t src)
{
  eeCheck(true);

  uint32_t eepromWriteSourceAddr = eepromHeader.files[src+1].zoneIndex * EEPROM_ZONE_SIZE;
  uint32_t eepromWriteDestinationAddr = eepromHeader.files[dst+1].zoneIndex * EEPROM_ZONE_SIZE;

  // erase blocks
  eepromEraseBlock(eepromWriteDestinationAddr);
  eepromEraseBlock(eepromWriteDestinationAddr+EEPROM_BLOCK_SIZE);

  // write model
  for (int pos=0; pos<EEPROM_ZONE_SIZE; pos+=EEPROM_BUFFER_SIZE) {
    eepromRead(eepromWriteSourceAddr+pos, eepromWriteBuffer, EEPROM_BUFFER_SIZE);
    eepromWrite(eepromWriteDestinationAddr+pos, eepromWriteBuffer, EEPROM_BUFFER_SIZE);
  }

  // write FAT
  eepromHeader.files[dst+1].exists = 1;
  eepromIncFatAddr();
  eepromWriteState = EEPROM_WRITE_NEW_FAT;
  eepromWriteWait();

  modelHeaders[dst] = modelHeaders[src];

  return true;
}

void eeSwapModels(uint8_t id1, uint8_t id2)
{
  eeCheck(true);
  {
    EepromHeaderFile tmp = eepromHeader.files[id1+1];
    eepromHeader.files[id1+1] = eepromHeader.files[id2+1];
    eepromHeader.files[id2+1] = tmp;
  }
  eepromIncFatAddr();
  eepromWriteState = EEPROM_WRITE_NEW_FAT;
  eepromWriteWait();

  {
    ModelHeader tmp = modelHeaders[id1];
    modelHeaders[id1] = modelHeaders[id2];
    modelHeaders[id2] = tmp;
  }
}

// For conversions ...
uint32_t loadGeneralSettings()
{
  return readFile(0, (uint8_t *)&g_eeGeneral, sizeof(g_eeGeneral));
}

uint32_t loadModel(uint32_t index)
{
  return readFile(index+1, (uint8_t *)&g_model, sizeof(g_model));
}

void writeGeneralSettings()
{
  writeFile(0, (uint8_t *)&g_eeGeneral, sizeof(g_eeGeneral));
}

void writeModel(int index)
{
  writeFile(index+1, (uint8_t *)&g_model, sizeof(g_model));
}

bool eeLoadGeneral()
{
  loadGeneralSettings();

  if (g_eeGeneral.version != EEPROM_VER) {
    TRACE("EEPROM version %d instead of %d", g_eeGeneral.version, EEPROM_VER);
#if defined(PCBSKY9X)
    if (!eeConvert()) {
      return false;
    }
#else
    return false;
#endif
  }
  return true;
}

void eeLoadModel(uint8_t id)
{
  if (id<MAX_MODELS) {

#if defined(SDCARD)
    closeLogs();
#endif

    if (pulsesStarted()) {
      pausePulses();
    }

    pauseMixerCalculations();

    uint32_t size = loadModel(id);

#if defined(SIMU)
    if (sizeof(uint16_t) + sizeof(g_model) > EEPROM_ZONE_SIZE)
      TRACE("Model data size can't exceed %d bytes (%d bytes)", int(EEPROM_ZONE_SIZE-sizeof(uint16_t)), (int)sizeof(g_model));
    if (size > 0 && size != sizeof(g_model))
      TRACE("Model data read=%d bytes vs %d bytes\n", size, (int)sizeof(ModelData));
#endif

    if (size < EEPROM_BUFFER_SIZE) { // if not loaded a fair amount
      modelDefault(id) ;
      eeCheck(true);
    }

    AUDIO_FLUSH();
    flightReset();
    logicalSwitchesReset();

    if (pulsesStarted()) {
      checkAll();
      resumePulses();
    }

    customFunctionsReset();

    restoreTimers();

#if defined(CPUARM)
    for (int i=0; i<MAX_SENSORS; i++) {
      TelemetrySensor & sensor = g_model.telemetrySensors[i];
      if (sensor.type == TELEM_TYPE_CALCULATED && sensor.persistent) {
        telemetryItems[i].value = sensor.persistentValue;
      }
    }
#endif

    LOAD_MODEL_CURVES();

    resumeMixerCalculations();
    // TODO pulses should be started after mixer calculations ...

#if defined(FRSKY)
    frskySendAlarms();
#endif

#if defined(SDCARD)
    referenceModelAudioFiles();
#endif

    LOAD_MODEL_BITMAP();
    SEND_FAILSAFE_1S();
    PLAY_MODEL_NAME();
  }
}

bool eeModelExists(uint8_t id)
{
  return (eepromHeader.files[id+1].exists);
}

void eeLoadModelHeader(uint8_t id, ModelHeader * header)
{
  readFile(id+1, (uint8_t *)header, sizeof(ModelHeader));
}

void eepromFormat()
{
  eepromFatAddr = 0;
  eepromHeader.mark = EEPROM_MARK;
  eepromHeader.index = 0;
  for (int i=0; i<EEPROM_MAX_FILES; i++) {
    eepromHeader.files[i].exists = 0;
    eepromHeader.files[i].zoneIndex = i+1;
  }
  eepromEraseBlock(0);
  eepromEraseBlock(EEPROM_BLOCK_SIZE);
  eepromWrite(0, (uint8_t *)&eepromHeader, sizeof(eepromHeader));
}

void eeErase(bool warn)
{
  TRACE("eeErase()");

  generalDefault();
  modelDefault(0);

  if (warn) {
    ALERT(STR_EEPROMWARN, STR_BADEEPROMDATA, AU_BAD_EEPROM);
  }

  MESSAGE(STR_EEPROMWARN, STR_EEPROMFORMATTING, NULL, AU_EEPROM_FORMATTING);

  eepromFormat();
  eeDirty(EE_GENERAL);
  eeDirty(EE_MODEL);
  eeCheck(true);
}

void eepromWriteWait(EepromWriteState state/* = EEPROM_IDLE*/)
{
  while (eepromWriteState != state) {
#if defined(CPUSTM32)
    // Waits a little bit for CS transitions
    CoTickDelay(1/*2ms*/);
#endif
    eepromWriteProcess();
#ifdef SIMU
    sleep(5/*ms*/);
#endif
  }
}

void eeCheck(bool immediately)
{
  if (immediately) {
    eepromWriteWait();
  }

  assert(eepromWriteState == EEPROM_IDLE);

  if (s_eeDirtyMsk & EE_GENERAL) {
    TRACE("eeprom write general");
    s_eeDirtyMsk -= EE_GENERAL;
    writeGeneralSettings();
    if (immediately)
      eepromWriteWait();
    else
      return;
  }

  if (s_eeDirtyMsk & EE_MODEL) {
    TRACE("eeprom write model");
    s_eeDirtyMsk -= EE_MODEL;
    writeModel(g_eeGeneral.currModel);
    if (immediately)
      eepromWriteWait();
  }
}

void eepromWriteProcess()
{
  // TRACE("eepromWriteProcess(state=%d)", eepromWriteState);

  switch (eepromWriteState) {
    case EEPROM_ERASING_FILE_BLOCK1:
    case EEPROM_ERASING_FILE_BLOCK2:
    case EEPROM_WRITING_BUFFER:
    case EEPROM_ERASING_FAT_BLOCK:
    case EEPROM_WRITING_NEW_FAT:
      if (Spi_complete) {
        eepromWriteState = EepromWriteState(eepromWriteState + 1);
      }
      break;

    case EEPROM_ERASING_FILE_BLOCK1_WAIT:
    case EEPROM_ERASING_FILE_BLOCK2_WAIT:
    case EEPROM_WRITING_BUFFER_WAIT:
    case EEPROM_ERASING_FAT_BLOCK_WAIT:
    case EEPROM_WRITING_NEW_FAT_WAIT:
      if ((eepromReadStatus() & 1) == 0) {
        eepromWriteState = EepromWriteState(eepromWriteState + 1);
      }
      break;

    case EEPROM_START_WRITE:
      eepromWriteState = EEPROM_ERASING_FILE_BLOCK1;
      eepromEraseBlock(eepromWriteDestinationAddr, false);
      break;

    case EEPROM_ERASE_FILE_BLOCK2:
      eepromWriteState = EEPROM_ERASING_FILE_BLOCK2;
      eepromEraseBlock(eepromWriteDestinationAddr + EEPROM_BLOCK_SIZE, false);
      break;

    case EEPROM_WRITE_BUFFER:
    {
      EepromFileHeader * header = (EepromFileHeader *)eepromWriteBuffer;
      header->fileIndex = eepromWriteFileIndex;
      header->size = eepromWriteSize;
      uint32_t size = min<uint32_t>(EEPROM_BUFFER_SIZE-sizeof(EepromFileHeader), eepromWriteSize);
      memcpy(eepromWriteBuffer+sizeof(EepromFileHeader), eepromWriteSourceAddr, size);
      eepromWriteState = EEPROM_WRITING_BUFFER;
      eepromWrite(eepromWriteDestinationAddr, eepromWriteBuffer, sizeof(EepromFileHeader)+size, false);
      eepromWriteSourceAddr += size;
      eepromWriteDestinationAddr += sizeof(EepromFileHeader)+size;
      eepromWriteSize -= size;
      break;
    }

    case EEPROM_WRITE_NEXT_BUFFER:
    {
      uint32_t size = min<uint32_t>(EEPROM_BUFFER_SIZE, eepromWriteSize);
      if (size > 0) {
        memcpy(eepromWriteBuffer, eepromWriteSourceAddr, size);
        eepromWriteState = EEPROM_WRITING_BUFFER;
        eepromWrite(eepromWriteDestinationAddr, eepromWriteBuffer, size, false);
        eepromWriteSourceAddr += size;
        eepromWriteDestinationAddr += size;
        eepromWriteSize -= size;
        break;
      }
      else if (eepromFatAddr == 0 || eepromFatAddr == EEPROM_BLOCK_SIZE) {
        eepromWriteState = EEPROM_ERASING_FAT_BLOCK;
        eepromEraseBlock(eepromFatAddr, false);
        break;
      }
    }
    /* no break */

    case EEPROM_WRITE_NEW_FAT:
      eepromWriteState = EEPROM_WRITING_NEW_FAT;
      eepromWrite(eepromFatAddr, (uint8_t *)&eepromHeader, sizeof(eepromHeader), false);
      break;

    case EEPROM_END_WRITE:
      eepromWriteState = EEPROM_IDLE;
      break;

    default:
      break;
  }
}

uint16_t eeModelSize(uint8_t index)
{
  uint16_t result = 0;

  if (eepromHeader.files[index+1].exists) {
    uint32_t address = eepromHeader.files[index+1].zoneIndex * EEPROM_ZONE_SIZE;
    EepromFileHeader header;
    eepromRead(address, (uint8_t *)&header, sizeof(header));
    result = header.size;
  }

  return result;
}

#if defined(SDCARD)
const pm_char * eeBackupModel(uint8_t i_fileSrc)
{
  char *buf = reusableBuffer.modelsel.mainname;
  FIL archiveFile;
  DIR archiveFolder;
  UINT written;

  eeCheck(true);

  if (!sdMounted()) {
    return STR_NO_SDCARD;
  }

  // check and create folder here
  strcpy(buf, STR_MODELS_PATH);
  FRESULT result = f_opendir(&archiveFolder, buf);
  if (result != FR_OK) {
    if (result == FR_NO_PATH)
      result = f_mkdir(buf);
    if (result != FR_OK)
      return SDCARD_ERROR(result);
  }
  else {
    f_closedir(&archiveFolder);
  }

  buf[sizeof(MODELS_PATH)-1] = '/';
  strcpy(strcat_modelname(&buf[sizeof(MODELS_PATH)], i_fileSrc), STR_MODELS_EXT);

  result = f_open(&archiveFile, buf, FA_CREATE_ALWAYS | FA_WRITE);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

#if defined(PCBSKY9X)
  strcpy(statusLineMsg, PSTR("File "));
  strcpy(statusLineMsg+5, &buf[sizeof(MODELS_PATH)]);
#endif

  uint16_t size = eeModelSize(i_fileSrc);

  *(uint32_t*)&buf[0] = O9X_FOURCC;
  buf[4] = g_eeGeneral.version;
  buf[5] = 'M';
  *(uint16_t*)&buf[6] = size;

  result = f_write(&archiveFile, buf, 8, &written);
  if (result != FR_OK || written != 8) {
    f_close(&archiveFile);
    return SDCARD_ERROR(result);
  }

  uint32_t address = eepromHeader.files[i_fileSrc+1].zoneIndex * EEPROM_ZONE_SIZE + sizeof(EepromFileHeader);
  while (size > 0) {
    uint16_t blockSize = min<uint16_t>(size, EEPROM_BUFFER_SIZE);
    eepromRead(address, eepromWriteBuffer, blockSize);
    result = f_write(&archiveFile, eepromWriteBuffer, blockSize, &written);
    if (result != FR_OK || written != blockSize) {
      f_close(&archiveFile);
      return SDCARD_ERROR(result);
    }
    size -= blockSize;
    address += blockSize;
  }

  f_close(&archiveFile);

#if defined(PCBSKY9X)
  showStatusLine();
#endif

  return NULL;
}

const pm_char * eeRestoreModel(uint8_t i_fileDst, char *model_name)
{
  char *buf = reusableBuffer.modelsel.mainname;
  FIL restoreFile;
  UINT read;

  eeCheck(true);

  if (!sdMounted()) {
    return STR_NO_SDCARD;
  }

  strcpy(buf, STR_MODELS_PATH);
  buf[sizeof(MODELS_PATH)-1] = '/';
  strcpy(&buf[sizeof(MODELS_PATH)], model_name);
  strcpy(&buf[strlen(buf)], STR_MODELS_EXT);

  FRESULT result = f_open(&restoreFile, buf, FA_OPEN_EXISTING | FA_READ);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  if (f_size(&restoreFile) < 8) {
    f_close(&restoreFile);
    return STR_INCOMPATIBLE;
  }

  result = f_read(&restoreFile, (uint8_t *)buf, 8, &read);
  if (result != FR_OK || read != 8) {
    f_close(&restoreFile);
    return SDCARD_ERROR(result);
  }

  uint8_t version = (uint8_t)buf[4];
  if (*(uint32_t*)&buf[0] != O9X_FOURCC || version < FIRST_CONV_EEPROM_VER || version > EEPROM_VER || buf[5] != 'M') {
    f_close(&restoreFile);
    return STR_INCOMPATIBLE;
  }

  if (eeModelExists(i_fileDst)) {
    eeDeleteModel(i_fileDst);
  }

  uint16_t size = min<uint16_t>(sizeof(g_model), *(uint16_t*)&buf[6]);
  uint32_t address = eepromHeader.files[i_fileDst+1].zoneIndex * EEPROM_ZONE_SIZE;

  // erase blocks
  eepromEraseBlock(address);
  eepromEraseBlock(address+EEPROM_BLOCK_SIZE);

  // write header
  EepromFileHeader header = { uint16_t(i_fileDst+1), size };
  eepromWrite(address, (uint8_t *)&header, sizeof(header));
  address += sizeof(header);

  // write model
  while (size > 0) {
    uint16_t blockSize = min<uint16_t>(size, EEPROM_BUFFER_SIZE);
    result = f_read(&restoreFile, eepromWriteBuffer, blockSize, &read);
    if (result != FR_OK || read != blockSize) {
      f_close(&g_oLogFile);
      return SDCARD_ERROR(result);
    }
    eepromWrite(address, eepromWriteBuffer, blockSize);
    size -= blockSize;
    address += blockSize;
  }

  // write FAT
  eepromHeader.files[i_fileDst+1].exists = 1;
  eepromIncFatAddr();
  eepromWriteState = EEPROM_WRITE_NEW_FAT;
  eepromWriteWait();

  eeLoadModelHeader(i_fileDst, &modelHeaders[i_fileDst]);

#if defined(PCBSKY9X)
  if (version < EEPROM_VER) {
    ConvertModel(i_fileDst, version);
    loadModel(g_eeGeneral.currModel);
  }
#endif

  return NULL;
}
#endif
