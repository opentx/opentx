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

#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <math.h>
#include <gtest/gtest.h>

#define SWAP_DEFINED
#include "opentx.h"

#define CHANNEL_MAX (1024*256)

void doMixerCalculations();

#define MODEL_RESET() \
  memset(&g_model, 0, sizeof(g_model)); \
  extern uint8_t s_mixer_first_run_done; \
  s_mixer_first_run_done = false; \
  lastFlightMode = 255;

int32_t lastAct = 0;
void MIXER_RESET()
{
  memset(channelOutputs, 0, sizeof(channelOutputs));
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

uint16_t anaInValues[NUM_STICKS+NUM_POTS] = { 0 };
uint16_t anaIn(uint8_t chan)
{
  if (chan < NUM_STICKS+NUM_POTS)
    return anaInValues[chan];
  else
    return 0;
}

#if !defined(PCBTARANIS)
TEST(Trims, greaterTrimLink)
{
  MODEL_RESET();
  setTrimValue(1, 0, TRIM_EXTENDED_MAX+3); // link to FP3 trim
  setTrimValue(3, 0, 32);
  EXPECT_EQ(getRawTrimValue(getTrimFlightPhase(1, 0), 0), 32);
}

TEST(Trims, chainedTrims)
{
  MODEL_RESET();
  setTrimValue(0, 0, 32);
  setTrimValue(1, 0, TRIM_EXTENDED_MAX+1); // link to FP0 trim
  setTrimValue(2, 0, TRIM_EXTENDED_MAX+2); // link to FP1 trim
  EXPECT_EQ(getRawTrimValue(getTrimFlightPhase(0, 2), 0), 32);
}

TEST(Trims, infiniteChainedTrims)
{
  MODEL_RESET();
  setTrimValue(0, 0, 32);
  setTrimValue(1, 0, TRIM_EXTENDED_MAX+3); // link to FP3 trim
  setTrimValue(2, 0, TRIM_EXTENDED_MAX+2); // link to FP1 trim
  setTrimValue(3, 0, TRIM_EXTENDED_MAX+3); // link to FP2 trim
  EXPECT_EQ(getRawTrimValue(getTrimFlightPhase(0, 2), 0), 32);
}
#endif

TEST(Trims, CopyTrimsToOffset)
{
  MODEL_RESET();
  modelDefault(0);
  setTrimValue(0, 1, -100); // -100 on elevator
  evalFunctions(); // it disables all safety channels
  copyTrimsToOffset(1);
  EXPECT_EQ(getTrimValue(0, 1), -100); // unchanged
#if defined(CPUARM)
  EXPECT_EQ(g_model.limitData[1].offset, -195);
#else
  EXPECT_EQ(g_model.limitData[1].offset, -200);
#endif
}

TEST(Trims, CopySticksToOffset)
{
  MODEL_RESET();
  modelDefault(0);
  anaInValues[ELE_STICK] = -100;
  perMain();
  copySticksToOffset(1);
  EXPECT_EQ(g_model.limitData[1].offset, -97);
}


TEST(Curves, LinearIntpol)
{
  MODEL_RESET();
  for (int8_t i=-2; i<=2; i++) {
    g_model.points[2+i] = 50*i;
  }
  EXPECT_EQ(applyCustomCurve(-1024, 0), -1024);
  EXPECT_EQ(applyCustomCurve(0, 0), 0);
  EXPECT_EQ(applyCustomCurve(1024, 0), 1024);
  EXPECT_EQ(applyCustomCurve(-192, 0), -192);
}


#if !defined(CPUARM)
TEST(FlightModes, nullFadeOut_posFadeIn)
{
  MODEL_RESET();
  g_model.flightModeData[1].swtch = SWSRC_ID1;
  g_model.flightModeData[1].fadeIn = 15;
  perMain();
  simuSetSwitch(3, 0);
  perMain();
}

TEST(Mixer, R2029Comment)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].srcRaw = MIXSRC_CH2;
  g_model.mixData[0].swtch = -SWSRC_THR;
  g_model.mixData[0].weight = 100;
  g_model.mixData[1].destCh = 1;
  g_model.mixData[1].srcRaw = MIXSRC_Thr;
  g_model.mixData[1].swtch = SWSRC_THR;
  g_model.mixData[1].weight = 100;
  anaInValues[THR_STICK] = 1024;
  simuSetSwitch(0, 1);
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);
  EXPECT_EQ(chans[1], CHANNEL_MAX);
  simuSetSwitch(0, 0);
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);
  EXPECT_EQ(chans[1], 0);
  simuSetSwitch(0, 1);
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);
  EXPECT_EQ(chans[1], CHANNEL_MAX);
}

TEST(Mixer, Cascaded3Channels)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].srcRaw = MIXSRC_CH2;
  g_model.mixData[0].weight = 100;
  g_model.mixData[1].destCh = 1;
  g_model.mixData[1].srcRaw = MIXSRC_CH3;
  g_model.mixData[1].weight = 100;
  g_model.mixData[2].destCh = 2;
  g_model.mixData[2].srcRaw = MIXSRC_THR;
  g_model.mixData[2].weight = 100;
  simuSetSwitch(0, 1);
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], CHANNEL_MAX);
  EXPECT_EQ(chans[1], CHANNEL_MAX);
  EXPECT_EQ(chans[2], CHANNEL_MAX);
}

TEST(Mixer, CascadedOrderedChannels)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].srcRaw = MIXSRC_THR;
  g_model.mixData[0].weight = 100;
  g_model.mixData[1].destCh = 1;
  g_model.mixData[1].srcRaw = MIXSRC_CH1;
  g_model.mixData[1].weight = 100;
  simuSetSwitch(0, 1);
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], CHANNEL_MAX);
  EXPECT_EQ(chans[1], CHANNEL_MAX);
}

TEST(Mixer, Cascaded5Channels)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].srcRaw = MIXSRC_CH2;
  g_model.mixData[0].weight = 100;
  g_model.mixData[1].destCh = 1;
  g_model.mixData[1].srcRaw = MIXSRC_CH3;
  g_model.mixData[1].weight = 100;
  g_model.mixData[2].destCh = 2;
  g_model.mixData[2].srcRaw = MIXSRC_CH4;
  g_model.mixData[2].weight = 100;
  g_model.mixData[3].destCh = 3;
  g_model.mixData[3].srcRaw = MIXSRC_CH5;
  g_model.mixData[3].weight = 100;
  g_model.mixData[4].destCh = 4;
  g_model.mixData[4].srcRaw = MIXSRC_THR;
  g_model.mixData[4].weight = 100;
  for (uint8_t i=0; i<10; i++) {
    simuSetSwitch(0, 1);
    doMixerCalculations();
    EXPECT_EQ(chans[0], CHANNEL_MAX);
    EXPECT_EQ(chans[1], CHANNEL_MAX);
    EXPECT_EQ(chans[2], CHANNEL_MAX);
    EXPECT_EQ(chans[3], CHANNEL_MAX);
    EXPECT_EQ(chans[4], CHANNEL_MAX);
    simuSetSwitch(0, 0);
    doMixerCalculations();
    EXPECT_EQ(chans[0], -CHANNEL_MAX);
    EXPECT_EQ(chans[1], -CHANNEL_MAX);
    EXPECT_EQ(chans[2], -CHANNEL_MAX);
    EXPECT_EQ(chans[3], -CHANNEL_MAX);
    EXPECT_EQ(chans[4], -CHANNEL_MAX);
  }
}
#endif

TEST(Mixer, InfiniteRecursiveChannels)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].srcRaw = MIXSRC_CH2;
  g_model.mixData[0].weight = 100;
  g_model.mixData[1].destCh = 1;
  g_model.mixData[1].srcRaw = MIXSRC_CH3;
  g_model.mixData[1].weight = 100;
  g_model.mixData[2].destCh = 2;
  g_model.mixData[2].srcRaw = MIXSRC_CH1;
  g_model.mixData[2].weight = 100;
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[2], 0);
  EXPECT_EQ(chans[1], 0);
  EXPECT_EQ(chans[0], 0);
}

TEST(Mixer, BlockingChannel)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].srcRaw = MIXSRC_CH1;
  g_model.mixData[0].weight = 100;
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);
}

TEST(Mixer, RecursiveAddChannel)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_MAX;
  g_model.mixData[0].weight = 50;
  g_model.mixData[1].destCh = 0;
  g_model.mixData[1].mltpx = MLTPX_ADD;
  g_model.mixData[1].srcRaw = MIXSRC_CH2;
  g_model.mixData[1].weight = 100;
  g_model.mixData[2].destCh = 1;
  g_model.mixData[2].srcRaw = MIXSRC_Rud;
  g_model.mixData[2].weight = 100;
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], CHANNEL_MAX/2);
  EXPECT_EQ(chans[1], 0);
}

TEST(Mixer, RecursiveAddChannelAfterInactivePhase)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.flightModeData[1].swtch = SWSRC_ID1;
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_CH2;
  g_model.mixData[0].flightModes = 0b11110;
  g_model.mixData[0].weight = 50;
  g_model.mixData[1].destCh = 0;
  g_model.mixData[1].mltpx = MLTPX_ADD;
  g_model.mixData[1].srcRaw = MIXSRC_MAX;
  g_model.mixData[1].flightModes = 0b11101;
  g_model.mixData[1].weight = 50;
  g_model.mixData[2].destCh = 1;
  g_model.mixData[2].srcRaw = MIXSRC_MAX;
  g_model.mixData[2].weight = 100;
  simuSetSwitch(3, -1);
  perMain();
  EXPECT_EQ(chans[0], CHANNEL_MAX/2);
  EXPECT_EQ(chans[1], CHANNEL_MAX);
  simuSetSwitch(3, 0);
  perMain();
  EXPECT_EQ(chans[0], CHANNEL_MAX/2);
  EXPECT_EQ(chans[1], CHANNEL_MAX);
}

#define CHECK_NO_MOVEMENT(channel, value, duration) \
    for (int i=1; i<=(duration); i++) { \
      evalFlightModeMixes(e_perout_mode_normal, 1); \
      EXPECT_EQ(chans[(channel)], (value)); \
    }

#define CHECK_SLOW_MOVEMENT(channel, sign, duration) \
    do { \
    for (int i=1; i<=(duration); i++) { \
      evalFlightModeMixes(e_perout_mode_normal, 1); \
      lastAct = lastAct + (sign) * (1<<19)/500; /* 100 on ARM */ \
      EXPECT_EQ(chans[(channel)], 256 * (lastAct >> 8)); \
    } \
    } while (0)

#define CHECK_DELAY(channel, duration) \
    do { \
      int32_t value = chans[(channel)]; \
      for (int i=1; i<=(duration); i++) { \
        evalFlightModeMixes(e_perout_mode_normal, 1); \
        EXPECT_EQ(chans[(channel)], value); \
      } \
    } while (0)

#if !defined(CPUARM)
TEST(Mixer, SlowOnSwitch)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_MAX;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].swtch = SWSRC_THR;
  g_model.mixData[0].speedUp = SLOW_STEP*5;
  g_model.mixData[0].speedDown = SLOW_STEP*5;

  s_mixer_first_run_done = true;
  
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);

  simuSetSwitch(0, 1);
  CHECK_SLOW_MOVEMENT(0, +1, 250);

  simuSetSwitch(0, -1);
  CHECK_SLOW_MOVEMENT(0, -1, 250);
}

TEST(Mixer, SlowUpOnSwitch)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_MAX;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].swtch = SWSRC_THR;
  g_model.mixData[0].speedUp = SLOW_STEP*5;
  g_model.mixData[0].speedDown = 0;

  simuSetSwitch(0, 0);
  evalFlightModeMixes(e_perout_mode_normal, 0);
  s_mixer_first_run_done = true;
  EXPECT_EQ(chans[0], 0);

  simuSetSwitch(0, 1);
  CHECK_SLOW_MOVEMENT(0, +1, 250);

  simuSetSwitch(0, 0);
  evalFlightModeMixes(e_perout_mode_normal, 1);
  EXPECT_EQ(chans[0], 0);

  lastAct = 0;
  simuSetSwitch(0, 1);
  CHECK_SLOW_MOVEMENT(0, +1, 100);
}
#endif

TEST(Mixer, SlowOnPhase)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.flightModeData[1].swtch = TR(SWSRC_THR, SWSRC_SA0);
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_MAX;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].flightModes = 0x2 + 0x4 + 0x8 + 0x10 /*only enabled in phase 0*/;
  g_model.mixData[0].speedUp = SLOW_STEP*5;
  g_model.mixData[0].speedDown = SLOW_STEP*5;

  s_mixer_first_run_done = true;
  mixerCurrentFlightMode = 0;
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);

  CHECK_SLOW_MOVEMENT(0, +1, 250);

  mixerCurrentFlightMode = 1;
  CHECK_SLOW_MOVEMENT(0, -1, 250);
}

#if !defined(CPUARM)
TEST(Mixer, SlowOnSwitchAndPhase)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.flightModeData[1].swtch = TR(SWSRC_THR, SWSRC_SA0);
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_MAX;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].swtch = TR(SWSRC_THR, SWSRC_SA0);
#if defined(CPUARM)
  g_model.mixData[0].flightModes = 0x2 + 0x4 + 0x8 + 0x10 + 0x20 + 0x40 + 0x80 + 0x100 /*only enabled in phase 0*/;
#else
  g_model.mixData[0].flightModes = 0x2 + 0x4 + 0x8 + 0x10 /*only enabled in phase 0*/;
#endif
  g_model.mixData[0].speedUp = SLOW_STEP*5;
  g_model.mixData[0].speedDown = SLOW_STEP*5;

  s_mixer_first_run_done = true;
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);

  simuSetSwitch(0, 1);
  mixerCurrentFlightMode = 0;
  CHECK_SLOW_MOVEMENT(0, +1, 250);

  simuSetSwitch(0, -1);
  mixerCurrentFlightMode = 1;
  CHECK_SLOW_MOVEMENT(0, -1, 250);
}
#endif

TEST(Mixer, SlowOnSwitchSource)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = TR(MIXSRC_THR, MIXSRC_SA);
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].speedUp = SLOW_STEP*5;
  g_model.mixData[0].speedDown = SLOW_STEP*5;

  s_mixer_first_run_done = true;

  simuSetSwitch(0, -1);
  CHECK_SLOW_MOVEMENT(0, -1, 250);
  EXPECT_EQ(chans[0], -CHANNEL_MAX);

  simuSetSwitch(0, 1);
  CHECK_SLOW_MOVEMENT(0, +1, 500);
}

TEST(Mixer, SlowDisabledOnStartup)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_MAX;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].speedUp = SLOW_STEP*5;
  g_model.mixData[0].speedDown = SLOW_STEP*5;

  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], CHANNEL_MAX);
}

#if !defined(CPUARM)
TEST(Mixer, SlowAndDelayOnReplace3POSSource)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_REP;
  g_model.mixData[0].srcRaw = MIXSRC_3POS;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].delayUp = 10;
  g_model.mixData[0].speedUp = SLOW_STEP*5;
  g_model.mixData[0].speedDown = SLOW_STEP*5;

  s_mixer_first_run_done = true;
  
  simuSetSwitch(3, -1);
  CHECK_SLOW_MOVEMENT(0, -1, 250);
  EXPECT_EQ(chans[0], -CHANNEL_MAX);

  simuSetSwitch(3, 0);
  CHECK_DELAY(0, 500);
  CHECK_SLOW_MOVEMENT(0, +1, 250/*half course*/);
  EXPECT_EQ(chans[0], 0);

  simuSetSwitch(3, 1);
  CHECK_DELAY(0, 500);
  CHECK_SLOW_MOVEMENT(0, +1, 250);
}
#endif

#if !defined(CPUARM)
TEST(Mixer, SlowOnSwitchReplace)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_MAX;
  g_model.mixData[0].weight = 50;
  g_model.mixData[1].destCh = 0;
  g_model.mixData[1].mltpx = MLTPX_REP;
  g_model.mixData[1].srcRaw = MIXSRC_MAX;
  g_model.mixData[1].weight = 100;
  g_model.mixData[1].swtch = SWSRC_THR;
  g_model.mixData[1].speedDown = SLOW_STEP*5;

  simuSetSwitch(0, 0);
  evalFlightModeMixes(e_perout_mode_normal, 1);
  EXPECT_EQ(chans[0], CHANNEL_MAX/2);

  simuSetSwitch(0, 1);
  evalFlightModeMixes(e_perout_mode_normal, 1);
  // slow is not applied, but it's better than the first mix not applied at all!
  EXPECT_EQ(chans[0], CHANNEL_MAX);

  simuSetSwitch(0, 0);
  evalFlightModeMixes(e_perout_mode_normal, 1);
  // slow is not applied, but it's better than the first mix not applied at all!
  EXPECT_EQ(chans[0], CHANNEL_MAX/2);
}
#endif

#if !defined(PCBTARANIS)
TEST(Mixer, NoTrimOnInactiveMix)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_Thr;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].swtch = SWSRC_THR;
  g_model.mixData[0].speedUp = SLOW_STEP*5;
  g_model.mixData[0].speedDown = SLOW_STEP*5;
  setTrimValue(0, 2, 256);

  s_mixer_first_run_done = true;

  simuSetSwitch(0, 1);
  CHECK_SLOW_MOVEMENT(0, 1, 100);

  simuSetSwitch(0, -1);
  CHECK_SLOW_MOVEMENT(0, -1, 100);
}
#endif

TEST(Mixer, SlowOnMultiply)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_MAX;
  g_model.mixData[0].weight = 100;
  g_model.mixData[1].destCh = 0;
  g_model.mixData[1].mltpx = MLTPX_MUL;
  g_model.mixData[1].srcRaw = MIXSRC_MAX;
  g_model.mixData[1].weight = 100;
  g_model.mixData[1].swtch = TR(SWSRC_THR, SWSRC_SA0);
  g_model.mixData[1].speedUp = SLOW_STEP*5;
  g_model.mixData[1].speedDown = SLOW_STEP*5;

  s_mixer_first_run_done = true;

  simuSetSwitch(0, 1);
  CHECK_SLOW_MOVEMENT(0, 1, 250);

  simuSetSwitch(0, -1);
  CHECK_NO_MOVEMENT(0, CHANNEL_MAX, 250);

  simuSetSwitch(0, 1);
  CHECK_NO_MOVEMENT(0, CHANNEL_MAX, 250);
}


#if defined(HELI) && defined(PCBTARANIS)
TEST(Heli, BasicTest)
{
  MODEL_RESET();
  applyDefaultTemplate();
  g_model.swashR.collectiveSource = MIXSRC_Thr;
  g_model.swashR.type = SWASH_TYPE_120;
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_CYC1;
  g_model.mixData[0].weight = 100;
  g_model.mixData[1].destCh = 1;
  g_model.mixData[1].mltpx = MLTPX_ADD;
  g_model.mixData[1].srcRaw = MIXSRC_CYC2;
  g_model.mixData[1].weight = 100;
  g_model.mixData[2].destCh = 2;
  g_model.mixData[2].mltpx = MLTPX_ADD;
  g_model.mixData[2].srcRaw = MIXSRC_CYC3;
  g_model.mixData[2].weight = 100;
  anaInValues[ELE_STICK] = 1024;
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], -CHANNEL_MAX);
  EXPECT_EQ(chans[1], CHANNEL_MAX/2);
  EXPECT_EQ(chans[2], CHANNEL_MAX/2);
}

TEST(Heli, Mode2Test)
{
  MODEL_RESET();
  g_eeGeneral.templateSetup = 2;
  applyDefaultTemplate();
  g_model.swashR.collectiveSource = MIXSRC_Thr;
  g_model.swashR.type = SWASH_TYPE_120;
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_CYC1;
  g_model.mixData[0].weight = 100;
  g_model.mixData[1].destCh = 1;
  g_model.mixData[1].mltpx = MLTPX_ADD;
  g_model.mixData[1].srcRaw = MIXSRC_CYC2;
  g_model.mixData[1].weight = 100;
  g_model.mixData[2].destCh = 2;
  g_model.mixData[2].mltpx = MLTPX_ADD;
  g_model.mixData[2].srcRaw = MIXSRC_CYC3;
  g_model.mixData[2].weight = 100;
  anaInValues[ELE_STICK] = 1024;
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], -CHANNEL_MAX);
  EXPECT_EQ(chans[1], CHANNEL_MAX/2);
  EXPECT_EQ(chans[2], CHANNEL_MAX/2);
}
#elif defined(HELI)
TEST(Heli, SimpleTest)
{
  MODEL_RESET();
  applyTemplate(TMPL_HELI_SETUP);
  anaInValues[ELE_STICK] = 1024;
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], -CHANNEL_MAX);
  EXPECT_EQ(chans[1], CHANNEL_MAX/2);
  EXPECT_EQ(chans[1], CHANNEL_MAX/2);
}
#endif
