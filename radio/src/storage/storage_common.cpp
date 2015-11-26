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

uint8_t   storageDirtyMsk;
tmr10ms_t storageDirtyTime10ms;

void storageDirty(uint8_t msk)
{
  storageDirtyMsk |= msk;
  storageDirtyTime10ms = get_tmr10ms() ;
}

void preModelLoad()
{
#if defined(CPUARM)
  watchdogSetTimeout(500/*5s*/);
#endif

#if defined(SDCARD)
  closeLogs();
#endif

  if (pulsesStarted()) {
    pausePulses();
  }

  pauseMixerCalculations();
}

void postModelLoad(bool newModel)
{
  AUDIO_FLUSH();
  flightReset();
  logicalSwitchesReset();

  if (pulsesStarted()) {
#if defined(GUI)
    if (!newModel) {
      checkAll();
    }
#endif
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

#if defined(CPUARM) && defined(SDCARD)
  referenceModelAudioFiles();
#endif

  LOAD_MODEL_BITMAP();
  LUA_LOAD_MODEL_SCRIPTS();
  SEND_FAILSAFE_1S();
  PLAY_MODEL_NAME();
}

void storageEraseAll(bool warn)
{
  TRACE("storageEraseAll()");

  generalDefault();
  modelDefault(0);

  if (warn) {
    ALERT(STR_STORAGE_WARNING, STR_BAD_RADIO_DATA, AU_BAD_RADIODATA);
  }

  MESSAGE(STR_STORAGE_WARNING, STR_STORAGE_FORMAT, NULL, AU_STORAGE_FORMAT);

  storageFormat();
  storageDirty(EE_GENERAL|EE_MODEL);
  storageCheck(true);
}
