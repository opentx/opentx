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

const char * writeFile(const char * filename, const uint8_t * data, uint16_t size)
{
  FIL file;
  char buf[8];
  UINT written;

  FRESULT result = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  *(uint32_t*)&buf[0] = O9X_FOURCC;
  buf[4] = EEPROM_VER;
  buf[5] = 'M';
  *(uint16_t*)&buf[6] = size;

  result = f_write(&file, buf, 8, &written);
  if (result != FR_OK || written != 8) {
    f_close(&file);
    return SDCARD_ERROR(result);
  }

  result = f_write(&file, data, size, &written);
  if (result != FR_OK || written != size) {
    f_close(&file);
    return SDCARD_ERROR(result);
  }

  f_close(&file);
  return NULL;
}

const char * writeModel()
{
  char path[256];
  strcpy(path, STR_MODELS_PATH);
  path[sizeof(MODELS_PATH)-1] = '/';
  strAppend(&path[sizeof(MODELS_PATH)], g_eeGeneral.currModelFilename, sizeof(g_eeGeneral.currModelFilename));
  return writeFile(path, (uint8_t *)&g_model, sizeof(g_model));
}

const char * loadFile(const char * filename, uint8_t * data, uint16_t maxsize)
{
  FIL file;
  char buf[8];
  UINT read;

  FRESULT result = f_open(&file, filename, FA_OPEN_EXISTING | FA_READ);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  if (f_size(&file) < 8) {
    f_close(&file);
    return STR_INCOMPATIBLE;
  }

  result = f_read(&file, (uint8_t *)buf, 8, &read);
  if (result != FR_OK || read != 8) {
    f_close(&file);
    return SDCARD_ERROR(result);
  }

  uint8_t version = (uint8_t)buf[4];
  if (*(uint32_t*)&buf[0] != O9X_FOURCC || version < FIRST_CONV_EEPROM_VER || version > EEPROM_VER || buf[5] != 'M') {
    f_close(&file);
    return STR_INCOMPATIBLE;
  }

  uint16_t size = min<uint16_t>(maxsize, *(uint16_t*)&buf[6]);
  result = f_read(&file, data, size, &read);
  if (result != FR_OK || read != size) {
    f_close(&file);
    return SDCARD_ERROR(result);
  }

  f_close(&file);
  return NULL;
}

const char * loadModel(const char * filename)
{
  char path[256];
  strcpy(path, STR_MODELS_PATH);
  path[sizeof(MODELS_PATH)-1] = '/';
  strcpy(&path[sizeof(MODELS_PATH)], filename);

  preModelLoad();

  const char * error = loadFile(path, (uint8_t *)&g_model, sizeof(g_model));
  if (error) {
    TRACE("loadModel error=%s", error);
  }

  bool newModel = false;
  if (error) {
    modelDefault(0) ;
    storageCheck(true);
    newModel = true;
  }

  postModelLoad(newModel);

  return error;
}

const char RADIO_SETTINGS_PATH[] = RADIO_PATH "/radio.bin";

const char * loadGeneralSettings()
{
  const char * error = loadFile(RADIO_SETTINGS_PATH, (uint8_t *)&g_eeGeneral, sizeof(g_eeGeneral));
  if (error) {
    TRACE("loadGeneralSettings error=%s", error);
  }
  // TODO this is temporary, we only have one model for now
  return error;
}

const char * writeGeneralSettings()
{
  return writeFile(RADIO_SETTINGS_PATH, (uint8_t *)&g_eeGeneral, sizeof(g_eeGeneral));
}

void storageCheck(bool immediately)
{
  // if (immediately) {
  //  eepromWriteWait();
  // }

  if (storageDirtyMsk & EE_GENERAL) {
    TRACE("eeprom write general");
    storageDirtyMsk -= EE_GENERAL;
    writeGeneralSettings();
    // if (immediately)
    //   eepromWriteWait();
    // else
      return;
  }

  if (storageDirtyMsk & EE_MODEL) {
    TRACE("eeprom write model");
    storageDirtyMsk -= EE_MODEL;
    const char * error = writeModel();
    if (error) {
      TRACE("writeModel error=%s", error);
    }
    // if (immediately)
    //  eepromWriteWait();
  }
}

void storageReadAll()
{
  sdInit();

  if (loadGeneralSettings() != NULL) {
    storageEraseAll(true);
    strcpy(g_eeGeneral.currModelFilename, "model.bin");
  }

  stickMode = g_eeGeneral.stickMode;

#if defined(CPUARM)
  for (uint8_t i=0; languagePacks[i]!=NULL; i++) {
    if (!strncmp(g_eeGeneral.ttsLanguage, languagePacks[i]->id, 2)) {
      currentLanguagePackIdx = i;
      currentLanguagePack = languagePacks[i];
    }
  }
#endif

  loadModel(g_eeGeneral.currModelFilename);
}

void storageFormat()
{
  sdCheckAndCreateDirectory(RADIO_PATH);
  sdCheckAndCreateDirectory(MODELS_PATH);
}
