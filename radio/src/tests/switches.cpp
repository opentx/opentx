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

#include <math.h>
#include <gtest/gtest.h>

#define SWAP_DEFINED
#include "opentx.h"

#define MODEL_RESET() \
  memset(&g_model, 0, sizeof(g_model)); \
  extern uint8_t s_mixer_first_run_done; \
  s_mixer_first_run_done = false; \
  lastFlightMode = 255;

extern void MIXER_RESET();

#if !defined(PCBTARANIS)
TEST(getSwitch, undefCSW)
{
  MODEL_RESET();
  EXPECT_EQ(getSwitch(NUM_PSWITCH), false);
  EXPECT_EQ(getSwitch(-NUM_PSWITCH), true); // no good answer there!
}
#endif

#if !defined(CPUARM)
TEST(getSwitch, circularCSW)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.logicalSw[0] = { SWSRC_SW1, SWSRC_SW1, LS_FUNC_OR };
  g_model.logicalSw[1] = { SWSRC_SW1, SWSRC_SW1, LS_FUNC_AND };

  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);
  EXPECT_EQ(getSwitch(-SWSRC_SW1), true);
  EXPECT_EQ(getSwitch(SWSRC_SW2), false);
  EXPECT_EQ(getSwitch(-SWSRC_SW2), true);
}
#endif

#if defined(PCBTARANIS)
TEST(getSwitch, OldTypeStickyCSW)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.logicalSw[0] = { LS_FUNC_AND, SWSRC_SA0, 0, 0,  };
  g_model.logicalSw[1] = { LS_FUNC_OR, SWSRC_SW1, SWSRC_SW2, 0  };

  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);
  EXPECT_EQ(getSwitch(SWSRC_SW2), false);

  // now trigger SA0, both switches should become true
  simuSetSwitch(0, -1);
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), true);
  EXPECT_EQ(getSwitch(SWSRC_SW2), true);

  // now release SA0 and SW2 should stay true
  simuSetSwitch(0, 0);
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);
  EXPECT_EQ(getSwitch(SWSRC_SW2), true);

  // now reset logical switches
  logicalSwitchesReset();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);
  EXPECT_EQ(getSwitch(SWSRC_SW2), false);
}
#endif // #if defined(PCBTARANIS)

TEST(getSwitch, nullSW)
{
  MODEL_RESET();
  EXPECT_EQ(getSwitch(0), true);
}

#if 0
TEST(getSwitch, DISABLED_VfasWithDelay)
{
  MODEL_RESET();
  MIXER_RESET();
  memclear(&frskyData, sizeof(frskyData));
  /*
  Test for logical switch:
      L1  Vfas < 9.6 Delay (0.5s)

  (gdb) print Open9xX9D::g_model.logicalSw[0] 
  $3 = {v1 = -39 '\331', v2 = 96, v3 = 0, func = 4 '\004', delay = 5 '\005', duration = 0 '\000', andsw = 0 '\000'}
  */
  g_model.logicalSw[0] = {int8_t(MIXSRC_FIRST_TELEM+TELEM_VFAS-1), 96, 0, 4, 5, 0, 0};
  frskyData.hub.vfas = 150;   //unit is 100mV

  //telemetry streaming is FALSE, so L1 should be FALSE no matter what value Vfas has
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  //every logicalSwitchesTimerTick() represents 100ms
  //so now after 5 ticks we should still have a FALSE value
  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  //now turn on telemetry
  EXPECT_EQ(TELEMETRY_STREAMING(), false);
  TELEMETRY_RSSI() = 50;
  EXPECT_EQ(TELEMETRY_STREAMING(), true);

  //vfas is 15.0V so L1 should still be FALSE
  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);
  logicalSwitchesTimerTick();
  logicalSwitchesTimerTick();
  logicalSwitchesTimerTick();
  logicalSwitchesTimerTick();
  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  //now reduce vfas to 9.5V and L1 should become TRUE after 0.5s
  frskyData.hub.vfas = 95;
  evalLogicalSwitches();

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), true);

  logicalSwitchesTimerTick();
  logicalSwitchesTimerTick();
  logicalSwitchesTimerTick();
  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), true);


  //now stop telemetry, L1 should become FALSE immediatelly
  TELEMETRY_RSSI() = 0;
  EXPECT_EQ(TELEMETRY_STREAMING(), false);
  evalLogicalSwitches();

  logicalSwitchesTimerTick();
  logicalSwitchesTimerTick();
  logicalSwitchesTimerTick();
  logicalSwitchesTimerTick();
  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);
}

TEST(getSwitch, DISABLED_RssiWithDuration)
{
  MODEL_RESET();
  MIXER_RESET();
  memclear(&frskyData, sizeof(frskyData));
  /*
  Test for logical switch:
      L1  RSSI > 10 Duration (0.5s)

  (gdb) print Open9xX9D::g_model.logicalSw[0] 
  $1 = {v1 = -55 '\311', v2 = 10, v3 = 0, func = 3 '\003', delay = 0 '\000', duration = 5 '\005', andsw = 0 '\000'}
  */

  g_model.logicalSw[0] = {int8_t(MIXSRC_FIRST_TELEM+TELEM_RSSI_RX-1), 10, 0, 3, 0, 5, 0};

  EXPECT_EQ(TELEMETRY_STREAMING(), false);

  evalLogicalSwitches();
  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  //now set RSSI to 5, L1 should still be FALSE
  TELEMETRY_RSSI() = 5;
  evalLogicalSwitches();
  EXPECT_EQ(TELEMETRY_STREAMING(), true);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  //now set RSSI to 100, L1 should become TRUE for 0.5s
  TELEMETRY_RSSI() = 100;
  evalLogicalSwitches();
  EXPECT_EQ(TELEMETRY_STREAMING(), true);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), true);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), true);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), true);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), true);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  //repeat  telemetry streaming OFF and ON to test for duration processing
  TELEMETRY_RSSI() = 0;
  evalLogicalSwitches();
  EXPECT_EQ(TELEMETRY_STREAMING(), false);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  //now set RSSI to 100, L1 should become TRUE for 0.5s
  TELEMETRY_RSSI() = 100;
  evalLogicalSwitches();
  EXPECT_EQ(TELEMETRY_STREAMING(), true);

  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), true);

  logicalSwitchesTimerTick();
  logicalSwitchesTimerTick();
  logicalSwitchesTimerTick();
  logicalSwitchesTimerTick();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);
}
#endif // #if defined(PCBTARANIS) && defined(FRSKY)


#if !defined(CPUARM)
TEST(getSwitch, recursiveSW)
{
  MODEL_RESET();
  MIXER_RESET();

  g_model.logicalSw[0] = { SWSRC_RUD, -SWSRC_SW2, LS_FUNC_OR };
  g_model.logicalSw[1] = { SWSRC_ELE, -SWSRC_SW1, LS_FUNC_OR };

  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);
  EXPECT_EQ(getSwitch(SWSRC_SW2), true);

  LS_RECURSIVE_EVALUATION_RESET();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);
  EXPECT_EQ(getSwitch(SWSRC_SW2), true);

  simuSetSwitch(1, 1);
  LS_RECURSIVE_EVALUATION_RESET();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), true);
  EXPECT_EQ(getSwitch(SWSRC_SW2), true);

  LS_RECURSIVE_EVALUATION_RESET();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), true);
  EXPECT_EQ(getSwitch(SWSRC_SW2), false);
}
#endif // #if !defined(CPUARM)

#if defined(PCBTARANIS)
TEST(getSwitch, inputWithTrim)
{
  MODEL_RESET();
  modelDefault(0);
  MIXER_RESET();

  g_model.logicalSw[0] = { LS_FUNC_VPOS, MIXSRC_FIRST_INPUT, 0, 0 };

  doMixerCalculations();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), false);

  setTrimValue(0, 0, 32);
  doMixerCalculations();
  evalLogicalSwitches();
  EXPECT_EQ(getSwitch(SWSRC_SW1), true);
}
#endif
