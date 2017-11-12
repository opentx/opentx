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
extern uint16_t anaInValues[NUM_STICKS+NUM_POTS+NUM_SLIDERS];

void doMixerCalculations();

#if defined(PCBTARANIS) || defined(PCBHORUS)
#define RADIO_RESET() \
  g_eeGeneral.switchConfig = 0x00007bff
#else
  #define RADIO_RESET()
#endif

inline void SYSTEM_RESET()
{
#if defined(CPUARM) && defined(EEPROM)
  memset(modelHeaders, 0, sizeof(modelHeaders));
#endif
  generalDefault();
  g_eeGeneral.templateSetup = 0;
  for (int i=0; i<NUM_SWITCHES; i++) {
    simuSetSwitch(i, -1);
  }
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
#if defined(TELEMETRY_FRSKY)
  memclear(&telemetryData, sizeof(telemetryData));
  TELEMETRY_RSSI() = 100;
#endif
#if defined(CPUARM)
#if defined(TELEMETRY_FRSKY)
  for (int i=0; i<MAX_TELEMETRY_SENSORS; i++) {
    telemetryItems[i].clear();
  }
#endif
  memclear(g_model.telemetrySensors, sizeof(g_model.telemetrySensors));
#endif
}

class OpenTxTest : public testing::Test 
{
  protected:  // You should make the members protected s.t. they can be
              // accessed from sub-classes.

    // virtual void SetUp() will be called before each test is run.  You
    // should define it if you need to initialize the varaibles.
    // Otherwise, this can be skipped.
    virtual void SetUp() 
    {
      SYSTEM_RESET();
      MODEL_RESET();
      MIXER_RESET();
      modelDefault(0);
      RADIO_RESET();
    }
};

#endif // _GTESTS_H_
