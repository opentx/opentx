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

#include "../opentx.h"

uint8_t   storageDirtyMsk;
tmr10ms_t storageDirtyTime10ms;

#if defined(RAMBACKUP)
uint8_t   rambackupDirtyMsk;
tmr10ms_t rambackupDirtyTime10ms;
#endif

void storageDirty(uint8_t msk)
{
  storageDirtyMsk |= msk;
  storageDirtyTime10ms = get_tmr10ms();

#if defined(RAMBACKUP)
  rambackupDirtyMsk = storageDirtyMsk;
  rambackupDirtyTime10ms = storageDirtyTime10ms;
#endif
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

#if defined(COLORLCD)
  loadCustomScreens();
#endif

  LOAD_MODEL_BITMAP();
  LUA_LOAD_MODEL_SCRIPTS();
  SEND_FAILSAFE_1S();
  PLAY_MODEL_NAME();
}
