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

uint8_t   s_eeDirtyMsk;
tmr10ms_t s_eeDirtyTime10ms;

void eeDirty(uint8_t msk)
{
  s_eeDirtyMsk |= msk;
  s_eeDirtyTime10ms = get_tmr10ms() ;
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
  eeCheck(true); // force writing of current model data before this is changed
  g_eeGeneral.currModel = sub;
  eeDirty(EE_GENERAL);
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

void eeReadAll()
{
  if (!eepromOpen() || !eeLoadGeneral()) {
    eeErase(true);
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

#if !defined(CPUARM)
  eeLoadModel(g_eeGeneral.currModel);
#endif
}
