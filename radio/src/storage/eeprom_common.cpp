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

    bool newModel = false;
    if (size < EEPROM_MIN_MODEL_SIZE) { // if not loaded a fair amount
      modelDefault(index) ;
      storageCheck(true);
      newModel = true;
    }

    postModelLoad(newModel);
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
  displayPopup(STR_LOADINGMODEL);
#endif
  saveTimers();
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

void storageReadAll()
{
  if (!eepromOpen() || !eeLoadGeneral()) {
    storageEraseAll(true);
  }
  else {
    eeLoadModelHeaders();
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

  eeLoadModel(g_eeGeneral.currModel);
}
