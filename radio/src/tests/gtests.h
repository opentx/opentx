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

#ifndef _GTESTS_H_
#define _GTESTS_H_

#include <QtCore/QString>
#include <math.h>
#include <gtest/gtest.h>

#define SWAP_DEFINED
#include "opentx.h"

#define CHANNEL_MAX (1024*256)

extern int32_t lastAct;
extern uint16_t anaInValues[NUM_STICKS+NUM_POTS];

void doMixerCalculations();

#if defined(PCBTARANIS)
#define RADIO_RESET() \
  g_eeGeneral.switchConfig = 0x00007bff
#else
  #define RADIO_RESET()
#endif

inline void SYSTEM_RESET()
{
#if defined(CPUARM)
  memset(modelHeaders, 0, sizeof(modelHeaders));
#endif
  generalDefault();
  g_eeGeneral.templateSetup = 0;
}

inline void MODEL_RESET()
{
  memset(&g_model, 0, sizeof(g_model));
  memset(&anaInValues, 0, sizeof(anaInValues));
  extern uint8_t s_mixer_first_run_done;
  s_mixer_first_run_done = false;
  lastFlightMode = 255;
}

inline void MIXER_RESET()
{
  memset(channelOutputs, 0, sizeof(channelOutputs));
  memset(chans, 0, sizeof(chans));
  memset(ex_chans, 0, sizeof(ex_chans));
  memset(act, 0, sizeof(act));
  memset(swOn, 0, sizeof(swOn));
#if !defined(CPUARM)
  s_last_switch_used = 0;
  s_last_switch_value = 0;
#endif
  mixerCurrentFlightMode = lastFlightMode = 0;
  lastAct = 0;
  logicalSwitchesReset();
}

inline void TELEMETRY_RESET()
{
#if defined(FRSKY)
  memclear(&frskyData, sizeof(frskyData));
  TELEMETRY_RSSI() = 100;
#endif
#if defined(CPUARM) && defined(FRSKY)
  for (int i=0; i<MAX_SENSORS; i++) {
    telemetryItems[i].clear();
  }
#endif
}

bool checkScreenshot(QString test);

#endif // _GTESTS_H_
