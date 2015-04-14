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
#define EEPROM_FILE_SIZE      (8*1024)
#define EEPROM_BUFFER_SIZE    256
#define EEPROM_FAT_SIZE       128
#define EEPROM_MAX_FILES      (EEPROM_SIZE / EEPROM_FILE_SIZE)
#define FIRST_FILE_AVAILABLE  (1+MAX_MODELS)

PACK(struct EepromHeaderFile
{
  uint8_t exists:1;
  uint8_t index:7;
});

PACK(struct EepromHeader
{
  uint32_t         mark;
  EepromHeaderFile files[EEPROM_MAX_FILES];
});

EepromHeader eepromHeader;
EepromWriteState eepromWriteState = EEPROM_IDLE;
uint8_t eepromWriteFileIndex = FIRST_FILE_AVAILABLE;
uint16_t eepromWriteSize;
uint8_t * eepromWriteSourceAddr;
uint32_t eepromWriteDestinationAddr;
uint16_t eepromFatAddr = 0;
uint16_t eepromPreviousFatAddr;
uint8_t eepromWriteBuffer[EEPROM_BUFFER_SIZE];

#define COMMAND_BLOCK_ERASE 0x20
#define COMMAND_WRITE       0x02
#define COMMAND_READ        0x03

void eepromWaitSpiComplete()
{
  while (!Spi_complete) {
    SIMU_SLEEP(5/*ms*/);
  }
  Spi_complete = false;
}

void eepromWaitReadStatus()
{
  while (eepromReadStatus() & 1) {
    SIMU_SLEEP(5/*ms*/);
  }
}

void eepromEraseBlock(uint32_t address, bool blocking=true)
{
  // TRACE("eepromEraseBlock(%d)", address);

#if defined(SIMU)
  static uint8_t erasedBlock[EEPROM_BLOCK_SIZE] = { 0xff };
  eeprom_pointer = address;
  eeprom_buffer_data = (char *)erasedBlock;
  eeprom_buffer_size = EEPROM_BLOCK_SIZE;
  eeprom_read_operation = false;
  Spi_complete = false;
  sem_post(eeprom_write_sem);
#else
  eeprom_write_enable();
  uint8_t * p = Spi_tx_buf;
  *p = COMMAND_BLOCK_ERASE;
  *(p+1) = address >> 16;
  *(p+2) = address >> 8;
  *(p+3) = address;
  spi_PDC_action(p, 0, 0, 4, 0);
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
  eeprom_buffer_data = (char*)buffer;
  eeprom_buffer_size = size;
  eeprom_read_operation = true;
  Spi_complete = false;
  sem_post(eeprom_write_sem);
#else
  uint8_t * p = Spi_tx_buf;
  *p = COMMAND_READ;
  *(p+1) = address >> 16;
  *(p+2) = address >> 8;
  *(p+3) = address;
  spi_PDC_action(p, 0, buffer, 4, size);
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
  eeprom_buffer_data = (char*)buffer;
  eeprom_buffer_size = size+1;
  eeprom_read_operation = false;
  Spi_complete = false;
  sem_post(eeprom_write_sem);
#else
  eeprom_write_enable();
  uint8_t * p = Spi_tx_buf;
  *p = COMMAND_WRITE;
  *(p+1) = address >> 16;
  *(p+2) = address >> 8;
  *(p+3) = address;
  spi_PDC_action(p, buffer, 0, 4, size);
#endif

  if (blocking) {
    eepromWaitSpiComplete();
    eepromWaitReadStatus();
  }
}

bool eepromOpen()
{
  eepromFatAddr = 0;
  while (eepromFatAddr < EEPROM_FILE_SIZE) {
    eepromRead(eepromFatAddr, (uint8_t *)&eepromHeader, sizeof(eepromHeader.mark));
    if (eepromHeader.mark == EEPROM_MARK) {
      eepromRead(eepromFatAddr, (uint8_t *)&eepromHeader, sizeof(eepromHeader));
      return true;
    }
    eepromFatAddr += EEPROM_FAT_SIZE;
  }
  return false;
}

bool eepromCheck()
{
  return true;
}

uint32_t readFile(int index, uint8_t * data, uint32_t size)
{
  uint16_t fileSize;
  uint32_t address = eepromHeader.files[index].index * EEPROM_FILE_SIZE;
  eepromRead(address, (uint8_t *)&fileSize, sizeof(fileSize));
  if (size < fileSize) {
    fileSize = size;
  }
  if (fileSize > 0) {
    eepromRead(address + sizeof(fileSize), data, fileSize);
    size -= fileSize;
  }
  if (size > 0) {
    memset(data + fileSize, 0, size);
  }
  return fileSize;
}

void eepromIncFatAddr()
{
  eepromPreviousFatAddr = eepromFatAddr;
  eepromFatAddr += EEPROM_FAT_SIZE;
  if (eepromFatAddr >= EEPROM_FILE_SIZE) {
    eepromFatAddr = 0;
  }
}

void writeFile(int index, uint8_t * data, uint32_t size)
{
  uint8_t blockIndex = eepromHeader.files[eepromWriteFileIndex].index;
  eepromHeader.files[eepromWriteFileIndex].exists = 0;
  eepromHeader.files[eepromWriteFileIndex].index = eepromHeader.files[index].index;
  eepromHeader.files[index].exists = (size > 0);
  eepromHeader.files[index].index = blockIndex;
  eepromWriteSourceAddr = data;
  eepromWriteSize = size;
  eepromWriteDestinationAddr = blockIndex * EEPROM_FILE_SIZE;
  eepromWriteState = EEPROM_START_WRITE;
  eepromWriteFileIndex += 1;
  if (eepromWriteFileIndex >= EEPROM_MAX_FILES) {
    eepromWriteFileIndex = FIRST_FILE_AVAILABLE;
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

  uint32_t eepromWriteSourceAddr = eepromHeader.files[src+1].index * EEPROM_FILE_SIZE;
  uint32_t eepromWriteDestinationAddr = eepromHeader.files[dst+1].index * EEPROM_FILE_SIZE;

  // erase blocks
  eepromEraseBlock(eepromWriteDestinationAddr);
  eepromEraseBlock(eepromWriteDestinationAddr+EEPROM_BLOCK_SIZE);

  // write model
  for (int pos=0; pos<EEPROM_FILE_SIZE; pos+=EEPROM_BUFFER_SIZE) {
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
    if (!eeConvert())
      return false;
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
    if (sizeof(uint16_t) + sizeof(g_model) > EEPROM_FILE_SIZE)
      TRACE("Model data size can't exceed %d bytes (%d bytes)", int(EEPROM_FILE_SIZE-sizeof(uint16_t)), (int)sizeof(g_model));
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
  for (int i=0; i<EEPROM_MAX_FILES; i++) {
    eepromHeader.files[i].exists = 0;
    eepromHeader.files[i].index = i+1;
  }
  eepromEraseBlock(0);
  eepromWrite(0, (uint8_t *)&eepromHeader, sizeof(eepromHeader));
}

void eeErase(bool warn)
{
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

  if (s_eeDirtyMsk & EE_GENERAL) {
    s_eeDirtyMsk -= EE_GENERAL;
    writeGeneralSettings();
    if (immediately)
      eepromWriteWait();
    else
      return;
  }

  if (s_eeDirtyMsk & EE_MODEL) {
    s_eeDirtyMsk -= EE_MODEL;
    writeModel(g_eeGeneral.currModel);
    if (immediately)
      eepromWriteWait();
  }
}

void eepromWriteProcess()
{
  if (eepromWriteState > EEPROM_START_WRITE && Spi_complete) {
    eepromWriteState = EepromWriteState(eepromWriteState + 1);
    Spi_complete = false;
  }

  // TRACE("eepromWriteProcess(state=%d)", eepromWriteState);

  switch (eepromWriteState) {
    case EEPROM_ERASING_FILE_BLOCK1_WAIT:
    case EEPROM_ERASING_FILE_BLOCK2_WAIT:
    case EEPROM_WRITING_BUFFER_WAIT:
    case EEPROM_ERASING_FAT_BLOCK_WAIT:
    case EEPROM_WRITING_NEW_FAT_WAIT:
    case EEPROM_OVERWRITING_OLD_FAT_WAIT:
      if ((eepromReadStatus() & 1) == 0)
        eepromWriteState = EepromWriteState(eepromWriteState + 1);
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
      int size = min<int>(EEPROM_BUFFER_SIZE-2, eepromWriteSize);
      *((uint16_t *)eepromWriteBuffer) = eepromWriteSize;
      memcpy(eepromWriteBuffer+2, eepromWriteSourceAddr, size);
      eepromWriteState = EEPROM_WRITING_BUFFER;
      eepromWrite(eepromWriteDestinationAddr, eepromWriteBuffer, 2+size, false);
      eepromWriteSourceAddr += size;
      eepromWriteDestinationAddr += 2+size;
      eepromWriteSize -= size;
      break;
    }

    case EEPROM_WRITE_NEXT_BUFFER:
    {
      int size = min<int>(EEPROM_BUFFER_SIZE, eepromWriteSize);
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

    case EEPROM_OVERWRITE_OLD_FAT:
      memset(eepromWriteBuffer, 0, EEPROM_FAT_SIZE);
      eepromWriteState = EEPROM_OVERWRITING_OLD_FAT;
      eepromWrite(eepromPreviousFatAddr, eepromWriteBuffer, EEPROM_FAT_SIZE, false);
      break;

    case EEPROM_END_WRITE:
      eepromWriteState = EEPROM_IDLE;
      break;

    default:
      break;
  }
}

uint16_t eeModelSize(uint8_t i_fileSrc)
{
  uint16_t result = 0;

  if (eepromHeader.files[i_fileSrc+1].exists) {
    uint32_t address = eepromHeader.files[i_fileSrc+1].index * EEPROM_FILE_SIZE;
    eepromRead(address, (uint8_t *)&result, sizeof(uint16_t));
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

  strcpy(statusLineMsg, PSTR("File "));
  strcpy(statusLineMsg+5, &buf[sizeof(MODELS_PATH)]);

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

  uint32_t address = eepromHeader.files[i_fileSrc+1].index * EEPROM_FILE_SIZE + 2;
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
  showStatusLine();
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
  uint32_t address = eepromHeader.files[i_fileDst+1].index * EEPROM_FILE_SIZE;

  // erase blocks
  eepromEraseBlock(address);
  eepromEraseBlock(address+EEPROM_BLOCK_SIZE);

  // write size
  eepromWrite(address, (uint8_t *)&size, sizeof(size));
  address += sizeof(size);

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

#if defined(CPUARM)
  if (version < EEPROM_VER) {
    ConvertModel(i_fileDst, version);
    loadModel(g_eeGeneral.currModel);
  }
#endif

  return NULL;
}
#endif
