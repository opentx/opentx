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

#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include "opentx.h"
#include "timers.h"

void eeLoadModel(uint8_t index)
{
  if (index < MAX_MODELS) {

    preModelLoad();

    uint16_t size = eeLoadModelData(index);

#if defined(SIMU) && defined(EEPROM_ZONE_SIZE)
    if (sizeof(uint16_t) + sizeof(g_model) > EEPROM_ZONE_SIZE) {
      TRACE("Model data size can't exceed %d bytes (%d bytes)", int(EEPROM_ZONE_SIZE-sizeof(uint16_t)), (int)sizeof(g_model));
    }
#endif

#if defined(SIMU)
    if (size > 0 && size != sizeof(g_model)) {
      TRACE("Model data read=%d bytes vs %d bytes\n", size, (int)sizeof(ModelData));
    }
#endif

    bool alarms = true;
    if (size < EEPROM_MIN_MODEL_SIZE) { // if not loaded a fair amount
      modelDefault(index) ;
      storageCheck(true);
      alarms = false;
    }

    postModelLoad(alarms);
  }
}

uint8_t eeFindEmptyModel(uint8_t id, bool down)
{
  uint8_t i = id;
  for (;;) {
    i = (MAX_MODELS + (down ? i+1 : i-1)) % MAX_MODELS;
    if (!eeModelExists(i)) break;
    if (i == id) return 0xff; // no free space in directory left
  }
  return i;
}

void selectModel(uint8_t sub)
{
#if !defined(COLORLCD)
  showMessageBox(STR_LOADINGMODEL);
#endif
  storageFlushCurrentModel();
  storageCheck(true); // force writing of current model data before this is changed
  g_eeGeneral.currModel = sub;
  storageDirty(EE_GENERAL);
  eeLoadModel(sub);
}

#if defined(CPUARM)
ModelHeader modelHeaders[MAX_MODELS];
void eeLoadModelHeaders()
{
  for (uint32_t i=0; i<MAX_MODELS; i++) {
    eeLoadModelHeader(i, &modelHeaders[i]);
  }
}
#endif

void storageReadRadioSettings()
{
  if (!eepromOpen() || !eeLoadGeneral()) {
    storageEraseAll(true);
  }
  else {
    eeLoadModelHeaders();
  }

#if defined(CPUARM)
  for (uint8_t i=0; languagePacks[i]!=NULL; i++) {
    if (!strncmp(g_eeGeneral.ttsLanguage, languagePacks[i]->id, 2)) {
      currentLanguagePackIdx = i;
      currentLanguagePack = languagePacks[i];
    }
  }
#endif
}

void storageReadCurrentModel()
{
  eeLoadModel(g_eeGeneral.currModel);
}

void storageReadAll()
{
  storageReadRadioSettings();
  storageReadCurrentModel();
}

void storageEraseAll(bool warn)
{
  TRACE("storageEraseAll");

  generalDefault();
  modelDefault(0);

  if (warn) {
    ALERT(STR_STORAGE_WARNING, STR_BAD_RADIO_DATA, AU_BAD_RADIODATA);
  }

  RAISE_ALERT(STR_STORAGE_WARNING, STR_STORAGE_FORMAT, NULL, AU_NONE);

  storageFormat();
  storageDirty(EE_GENERAL|EE_MODEL);
  storageCheck(true);
}
