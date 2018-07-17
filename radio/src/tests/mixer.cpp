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

#include "gtests.h"

class TrimsTest : public OpenTxTest {};
class MixerTest : public OpenTxTest {};

#define CHECK_NO_MOVEMENT(channel, value, duration) \
    for (int i=1; i<=(duration); i++) { \
      evalFlightModeMixes(e_perout_mode_normal, 1); \
      GTEST_ASSERT_EQ((value), chans[(channel)]); \
    }

#define CHECK_SLOW_MOVEMENT(channel, sign, duration) \
    do { \
    for (int i=1; i<=(duration); i++) { \
      evalFlightModeMixes(e_perout_mode_normal, 1); \
      lastAct = lastAct + (sign) * (1<<19)/500; /* 100 on ARM */ \
      GTEST_ASSERT_EQ(256 * (lastAct >> 8), chans[(channel)]); \
    } \
    } while (0)

#define CHECK_DELAY(channel, duration) \
    do { \
      int32_t value = chans[(channel)]; \
      for (int i=1; i<=(duration); i++) { \
        evalFlightModeMixes(e_perout_mode_normal, 1); \
        GTEST_ASSERT_EQ(chans[(channel)], value); \
      } \
    } while (0)

TEST_F(TrimsTest, throttleTrim)
{
  g_model.thrTrim = 1;
  // stick max + trim max
  anaInValues[THR_STICK] = +1024;
  setTrimValue(0, THR_STICK, TRIM_MAX);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 1024);
  // stick max + trim min
  anaInValues[THR_STICK] = +1024;
  setTrimValue(0, THR_STICK, TRIM_MIN);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 1024);
  // stick min + trim max
  anaInValues[THR_STICK] = -1024;
  setTrimValue(0, THR_STICK, TRIM_MAX);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], -1024+500);
  // stick min + trim mid
  anaInValues[THR_STICK] = -1024;
  setTrimValue(0, THR_STICK, 0);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], -1024+250);
  // stick min + trim min
  anaInValues[THR_STICK] = -1024;
  setTrimValue(0, THR_STICK, TRIM_MIN);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], -1024);
  // now the same tests with extended Trims
  g_model.extendedTrims = 1;
  // stick max + trim max
  anaInValues[THR_STICK] = +1024;
  setTrimValue(0, THR_STICK, TRIM_EXTENDED_MAX);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 1024);
  // stick max + trim min
  anaInValues[THR_STICK] = +1024;
  setTrimValue(0, THR_STICK, TRIM_EXTENDED_MIN);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 1024);
  // stick min + trim max
  anaInValues[THR_STICK] = -1024;
  setTrimValue(0, THR_STICK, TRIM_EXTENDED_MAX);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], -1024+2000);
  // stick min + trim mid
  anaInValues[THR_STICK] = -1024;
  setTrimValue(0, THR_STICK, 0);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], -1024+1000);
  // stick min + trim min
  anaInValues[THR_STICK] = -1024;
  setTrimValue(0, THR_STICK, TRIM_EXTENDED_MIN);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], -1024);

}

TEST_F(TrimsTest, invertedThrottlePlusThrottleTrim)
{
  g_model.throttleReversed = 1;
  g_model.thrTrim = 1;
  // stick max + trim max
  anaInValues[THR_STICK] = +1024;
  setTrimValue(0, THR_STICK, TRIM_MAX);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], -1024);
  // stick max + trim mid
  anaInValues[THR_STICK] = +1024;
  setTrimValue(0, THR_STICK, 0);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], -1024+250);
  // stick max + trim min
  anaInValues[THR_STICK] = +1024;
  setTrimValue(0, THR_STICK, TRIM_MIN);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], -1024+500);
  // stick min + trim max
  anaInValues[THR_STICK] = -1024;
  setTrimValue(0, THR_STICK, TRIM_MAX);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], +1024);
  // stick min + trim min
  anaInValues[THR_STICK] = -1024;
  setTrimValue(0, THR_STICK, TRIM_MIN);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], +1024);

  // now the same tests with extended Trims
  g_model.extendedTrims = 1;
  // stick max + trim max
  anaInValues[THR_STICK] = +1024;
  setTrimValue(0, THR_STICK, TRIM_EXTENDED_MAX);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], -1024);
  // stick max + trim mid
  anaInValues[THR_STICK] = +1024;
  setTrimValue(0, THR_STICK, 0);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], -1024+1000);
  // stick max + trim min
  anaInValues[THR_STICK] = +1024;
  setTrimValue(0, THR_STICK, TRIM_EXTENDED_MIN);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], -1024+2000);
  // stick min + trim max
  anaInValues[THR_STICK] = -1024;
  setTrimValue(0, THR_STICK, TRIM_EXTENDED_MAX);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], +1024);
  // stick min + trim min
  anaInValues[THR_STICK] = -1024;
  setTrimValue(0, THR_STICK, TRIM_EXTENDED_MIN);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], +1024);
}

TEST_F(TrimsTest, throttleTrimWithZeroWeightOnThrottle)
{
  g_model.thrTrim = 1;
#if defined(VIRTUAL_INPUTS)
  // the input already exists
  ExpoData *expo = expoAddress(THR_STICK);
#else
  ExpoData *expo = expoAddress(0);
  expo->mode = 3;
  expo->chn = THR_STICK;
#endif
  expo->weight = 0;
  // stick max + trim max
  anaInValues[THR_STICK] = +1024;
  setTrimValue(0, THR_STICK, TRIM_MAX);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 250);
  // stick max + trim mid
  anaInValues[THR_STICK] = +1024;
  setTrimValue(0, THR_STICK, 0);
  evalMixes(1);
  EXPECT_LE(abs(channelOutputs[2] - 125), 1);  //can't use precise comparison here because of lower precision math on 9X
  // stick max + trim min
  anaInValues[THR_STICK] = +1024;
  setTrimValue(0, THR_STICK, TRIM_MIN);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 0);
  // stick min + trim max
  anaInValues[THR_STICK] = -1024;
  setTrimValue(0, THR_STICK, TRIM_MAX);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 250);
  // stick min + trim mid
  anaInValues[THR_STICK] = -1024;
  setTrimValue(0, THR_STICK, 0);
  evalMixes(1);
  EXPECT_LE(abs(channelOutputs[2] - 125), 1);
  // stick min + trim min
  anaInValues[THR_STICK] = -1024;
  setTrimValue(0, THR_STICK, TRIM_MIN);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 0);

  // now some tests with extended Trims
  g_model.extendedTrims = 1;
  // trim min + various stick positions = should always be same value
  setTrimValue(0, THR_STICK, TRIM_EXTENDED_MIN);
  anaInValues[THR_STICK] = -1024;
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 0);
  anaInValues[THR_STICK] = -300;
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 0);
  anaInValues[THR_STICK] = +300;
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 0);
  anaInValues[THR_STICK] = +1024;
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 0);

  // trim max + various stick positions = should always be same value
  setTrimValue(0, THR_STICK, TRIM_EXTENDED_MAX);
  anaInValues[THR_STICK] = -1024;
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 1000);
  anaInValues[THR_STICK] = -300;
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 1000);
  anaInValues[THR_STICK] = +300;
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 1000);
  anaInValues[THR_STICK] = +1024;
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 1000);
}

TEST_F(TrimsTest, invertedThrottlePlusthrottleTrimWithZeroWeightOnThrottle)
{
  g_model.throttleReversed = 1;
  g_model.thrTrim = 1;
#if defined(VIRTUAL_INPUTS)
  // the input already exists
  ExpoData *expo = expoAddress(THR_STICK);
#else
  ExpoData *expo = expoAddress(0);
  expo->mode = 3;
  expo->chn = THR_STICK;
#endif
  expo->weight = 0;
  // stick max + trim max
  anaInValues[THR_STICK] = +1024;
  setTrimValue(0, THR_STICK, TRIM_MAX);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 0);
  // stick max + trim mid
  anaInValues[THR_STICK] = +1024;
  setTrimValue(0, THR_STICK, 0);
  evalMixes(1);
  EXPECT_LE(abs(channelOutputs[2] - 125), 1);
  // stick max + trim min
  anaInValues[THR_STICK] = +1024;
  setTrimValue(0, THR_STICK, TRIM_MIN);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 250);
  // stick min + trim max
  anaInValues[THR_STICK] = -1024;
  setTrimValue(0, THR_STICK, TRIM_MAX);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 0);
  // stick min + trim mid
  anaInValues[THR_STICK] = -1024;
  setTrimValue(0, THR_STICK, 0);
  evalMixes(1);
  EXPECT_LE(abs(channelOutputs[2] - 125), 1);
  // stick min + trim min
  anaInValues[THR_STICK] = -1024;
  setTrimValue(0, THR_STICK, TRIM_MIN);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 250);

  // now some tests with extended Trims
  g_model.extendedTrims = 1;
  // trim min + various stick positions = should always be same value
  setTrimValue(0, THR_STICK, TRIM_EXTENDED_MIN);
  anaInValues[THR_STICK] = -1024;
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 1000);
  anaInValues[THR_STICK] = -300;
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 1000);
  anaInValues[THR_STICK] = +300;
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 1000);
  anaInValues[THR_STICK] = +1024;
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 1000);

  // trim max + various stick positions = should always be same value
  setTrimValue(0, THR_STICK, TRIM_EXTENDED_MAX);
  anaInValues[THR_STICK] = -1024;
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 0);
  anaInValues[THR_STICK] = -300;
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 0);
  anaInValues[THR_STICK] = +300;
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 0);
  anaInValues[THR_STICK] = +1024;
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 0);
}

#if !defined(VIRTUAL_INPUTS)
TEST_F(TrimsTest, greaterTrimLink)
{
  setTrimValue(1, RUD_STICK, TRIM_EXTENDED_MAX+3); // link to FP3 trim
  setTrimValue(3, RUD_STICK, 32);
  EXPECT_EQ(getRawTrimValue(getTrimFlightMode(1, RUD_STICK), RUD_STICK), 32);
}

TEST_F(TrimsTest, chainedTrims)
{
  setTrimValue(0, RUD_STICK, 32);
  setTrimValue(1, RUD_STICK, TRIM_EXTENDED_MAX+1); // link to FP0 trim
  setTrimValue(2, RUD_STICK, TRIM_EXTENDED_MAX+2); // link to FP1 trim
  EXPECT_EQ(getRawTrimValue(getTrimFlightMode(0, RUD_STICK), RUD_STICK), 32);
}

TEST_F(TrimsTest, infiniteChainedTrims)
{
  setTrimValue(0, RUD_STICK, 32);
  setTrimValue(1, RUD_STICK, TRIM_EXTENDED_MAX+3); // link to FP3 trim
  setTrimValue(2, RUD_STICK, TRIM_EXTENDED_MAX+2); // link to FP1 trim
  setTrimValue(3, RUD_STICK, TRIM_EXTENDED_MAX+3); // link to FP2 trim
  EXPECT_EQ(getRawTrimValue(getTrimFlightMode(0, RUD_STICK), RUD_STICK), 32);
}
#endif

TEST_F(TrimsTest, CopyTrimsToOffset)
{
  setTrimValue(0, ELE_STICK, -100); // -100 on elevator
#if defined(CPUARM)
  evalFunctions(g_model.customFn, modelFunctionsContext); // it disables all safety channels
  copyTrimsToOffset(1);
  EXPECT_EQ(getTrimValue(0, ELE_STICK), -100); // unchanged
  EXPECT_EQ(g_model.limitData[1].offset, -195);
#else
  evalFunctions(); // it disables all safety channels
  copyTrimsToOffset(1);
  EXPECT_EQ(getTrimValue(0, ELE_STICK), -100); // unchanged
  EXPECT_EQ(g_model.limitData[1].offset, -200);
#endif
}

TEST_F(TrimsTest, CopySticksToOffset)
{
  anaInValues[ELE_STICK] = -100;
  evalMixes(1);
  copySticksToOffset(1);
  EXPECT_EQ(g_model.limitData[1].offset, -97);
}

TEST_F(TrimsTest, InstantTrim)
{
  anaInValues[AIL_STICK] = 50;
  instantTrim();
  EXPECT_EQ(25, getTrimValue(0, AIL_STICK));
}

#if defined(VIRTUAL_INPUTS)
TEST_F(TrimsTest, InstantTrimNegativeCurve)
{
  ExpoData *expo = expoAddress(AIL_STICK);
  expo->curve.type = CURVE_REF_CUSTOM;
  expo->curve.value = 1;
  g_model.points[0] = -100;
  g_model.points[1] = -75;
  g_model.points[2] = -50;
  g_model.points[3] = -25;
  g_model.points[4] = 0;
  anaInValues[AIL_STICK] = 512;
  instantTrim();
  EXPECT_EQ(128, getTrimValue(0, AIL_STICK));
}
#endif

TEST_F(TrimsTest, throttleTrimEle) {
  SYSTEM_RESET();
  MODEL_RESET();
  MIXER_RESET();
  modelDefault(0);
  g_eeGeneral.templateSetup = 17;
  applyDefaultTemplate();
  g_model.thrTrim = 1;
// checks ELE sticks are not affected by throttleTrim
// stick max + trim min
  anaInValues[ELE_STICK] = +1024;
  setTrimValue(0, ELE_STICK, TRIM_MIN);
  evalMixes(1);
  EXPECT_EQ(channelOutputs[2], 1024 - 250);
  SYSTEM_RESET();
  MODEL_RESET();
  MIXER_RESET();
  modelDefault(0);
  g_eeGeneral.templateSetup = 0;
  applyDefaultTemplate();
}

TEST(Curves, LinearIntpol)
{
  SYSTEM_RESET();
  MODEL_RESET();
  MIXER_RESET();
  modelDefault(0);
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
  SYSTEM_RESET();
  MODEL_RESET();
  MIXER_RESET();
  modelDefault(0);
  lastFlightMode = 255;
  simuSetSwitch(3, 1);
  g_model.flightModeData[1].swtch = SWSRC_ID1;
  g_model.flightModeData[1].fadeIn = 15;
  evalMixes(1);
  simuSetSwitch(3, 0);
  evalMixes(1);
  // run mixes enough time to fade out flight modes (otherwise the mixer internal state flightModesFade  could affect other tests)
  simuSetSwitch(3, 1);
  for(int n=0; n<200; n++) {
    evalMixes(1);  
  }
}

TEST_F(MixerTest, R2029Comment)
{
  SYSTEM_RESET();
  MODEL_RESET();
  MIXER_RESET();
  modelDefault(0);
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].srcRaw = MIXSRC_CH2;
  g_model.mixData[0].swtch = -SWSRC_THR;
  g_model.mixData[0].weight = 100;
  g_model.mixData[1].destCh = 1;
  g_model.mixData[1].srcRaw = MIXSRC_Thr;
  g_model.mixData[1].swtch = SWSRC_THR;
  g_model.mixData[1].weight = 100;
  anaInValues[THR_STICK] = 1024;
  simuSetSwitch(1, 1);
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);
  EXPECT_EQ(chans[1], CHANNEL_MAX);
  simuSetSwitch(1, 0);
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);
  EXPECT_EQ(chans[1], 0);
  simuSetSwitch(1, 1);
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);
  EXPECT_EQ(chans[1], CHANNEL_MAX);
}

TEST_F(MixerTest, Cascaded3Channels)
{
  SYSTEM_RESET();
  MODEL_RESET();
  MIXER_RESET();
  modelDefault(0);
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].srcRaw = MIXSRC_CH2;
  g_model.mixData[0].weight = 100;
  g_model.mixData[1].destCh = 1;
  g_model.mixData[1].srcRaw = MIXSRC_CH3;
  g_model.mixData[1].weight = 100;
  g_model.mixData[2].destCh = 2;
  g_model.mixData[2].srcRaw = MIXSRC_THR;
  g_model.mixData[2].weight = 100;
  simuSetSwitch(1, 1);
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], CHANNEL_MAX);
  EXPECT_EQ(chans[1], CHANNEL_MAX);
  EXPECT_EQ(chans[2], CHANNEL_MAX);
}

TEST_F(MixerTest, CascadedOrderedChannels)
{
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].srcRaw = MIXSRC_THR;
  g_model.mixData[0].weight = 100;
  g_model.mixData[1].destCh = 1;
  g_model.mixData[1].srcRaw = MIXSRC_CH1;
  g_model.mixData[1].weight = 100;
  simuSetSwitch(1, 1);
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], CHANNEL_MAX);
  EXPECT_EQ(chans[1], CHANNEL_MAX);
}

TEST_F(MixerTest, Cascaded5Channels)
{
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
    simuSetSwitch(1, 1);
    evalMixes(1);
    EXPECT_EQ(chans[0], CHANNEL_MAX);
    EXPECT_EQ(chans[1], CHANNEL_MAX);
    EXPECT_EQ(chans[2], CHANNEL_MAX);
    EXPECT_EQ(chans[3], CHANNEL_MAX);
    EXPECT_EQ(chans[4], CHANNEL_MAX);
    simuSetSwitch(1, 0);
    evalMixes(1);
    EXPECT_EQ(chans[0], -CHANNEL_MAX);
    EXPECT_EQ(chans[1], -CHANNEL_MAX);
    EXPECT_EQ(chans[2], -CHANNEL_MAX);
    EXPECT_EQ(chans[3], -CHANNEL_MAX);
    EXPECT_EQ(chans[4], -CHANNEL_MAX);
  }
}
#endif

TEST_F(MixerTest, InfiniteRecursiveChannels)
{
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

TEST_F(MixerTest, BlockingChannel)
{
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].srcRaw = MIXSRC_CH1;
  g_model.mixData[0].weight = 100;
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);
}

TEST_F(MixerTest, RecursiveAddChannel)
{
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

TEST_F(MixerTest, RecursiveAddChannelAfterInactivePhase)
{
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
  evalMixes(1);
  EXPECT_EQ(chans[0], CHANNEL_MAX/2);
  EXPECT_EQ(chans[1], CHANNEL_MAX);
  simuSetSwitch(3, 0);
  evalMixes(1);
  EXPECT_EQ(chans[0], CHANNEL_MAX/2);
  EXPECT_EQ(chans[1], CHANNEL_MAX);
}

#if !defined(CPUARM)
TEST_F(MixerTest, SlowOnSwitch)
{
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

  simuSetSwitch(1, 1);
  CHECK_SLOW_MOVEMENT(0, +1, 250);

  simuSetSwitch(1, -1);
  CHECK_SLOW_MOVEMENT(0, -1, 250);
}

TEST_F(MixerTest, SlowUpOnSwitch)
{
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_MAX;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].swtch = SWSRC_THR;
  g_model.mixData[0].speedUp = SLOW_STEP*5;
  g_model.mixData[0].speedDown = 0;

  simuSetSwitch(1, 0);
  evalFlightModeMixes(e_perout_mode_normal, 0);
  s_mixer_first_run_done = true;
  EXPECT_EQ(chans[0], 0);

  simuSetSwitch(1, 1);
  CHECK_SLOW_MOVEMENT(0, +1, 250);

  simuSetSwitch(1, 0);
  evalFlightModeMixes(e_perout_mode_normal, 1);
  EXPECT_EQ(chans[0], 0);

  lastAct = 0;
  simuSetSwitch(1, 1);
  CHECK_SLOW_MOVEMENT(0, +1, 100);
}
#endif

TEST_F(MixerTest, SlowOnPhase)
{
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
TEST_F(MixerTest, SlowOnSwitchAndPhase)
{
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

  simuSetSwitch(1, 1);
  mixerCurrentFlightMode = 0;
  CHECK_SLOW_MOVEMENT(0, +1, 250);

  simuSetSwitch(1, -1);
  mixerCurrentFlightMode = 1;
  CHECK_SLOW_MOVEMENT(0, -1, 250);
}
#endif

TEST_F(MixerTest, SlowOnSwitchSource)
{
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
#if defined(PCBTARANIS) || defined(PCBHORUS)
  g_eeGeneral.switchConfig = 0x03;
  g_model.mixData[0].srcRaw = MIXSRC_SA;
  int switch_index = 0;
#else
  g_model.mixData[0].srcRaw = MIXSRC_THR;
  int switch_index = 1;
#endif
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].speedUp = SLOW_STEP*5;
  g_model.mixData[0].speedDown = SLOW_STEP*5;

  s_mixer_first_run_done = true;

  simuSetSwitch(switch_index, -1);
  CHECK_SLOW_MOVEMENT(0, -1, 250);
  EXPECT_EQ(chans[0], -CHANNEL_MAX);

  simuSetSwitch(switch_index, 1);
  CHECK_SLOW_MOVEMENT(0, +1, 500);
}

TEST_F(MixerTest, SlowDisabledOnStartup)
{
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_MAX;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].speedUp = SLOW_STEP*5;
  g_model.mixData[0].speedDown = SLOW_STEP*5;

  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], CHANNEL_MAX);
}

TEST_F(MixerTest, DelayOnSwitch)
{
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_MAX;
  g_model.mixData[0].weight = 100;
#if defined(PCBTARANIS) || defined(PCBHORUS)
  g_model.mixData[0].swtch = SWSRC_SA2;
  int switch_index = 0;
#else
  g_model.mixData[0].swtch = SWSRC_THR;
  int switch_index = 1;
#endif
  g_model.mixData[0].delayUp = DELAY_STEP*5;
  g_model.mixData[0].delayDown = DELAY_STEP*5;

  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);

  simuSetSwitch(switch_index, 1);
  CHECK_DELAY(0, 500);

  evalFlightModeMixes(e_perout_mode_normal, 1);
  EXPECT_EQ(chans[0], CHANNEL_MAX);

  simuSetSwitch(switch_index, 0);
  CHECK_DELAY(0, 500);

  evalFlightModeMixes(e_perout_mode_normal, 1);
  EXPECT_EQ(chans[0], 0);
}

#if !defined(CPUARM)
TEST_F(MixerTest, SlowAndDelayOnReplace3POSSource)
{
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_REP;
  g_model.mixData[0].srcRaw = MIXSRC_3POS;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].delayUp = 10;
  g_model.mixData[0].speedUp = SLOW_STEP*5;
  g_model.mixData[0].speedDown = SLOW_STEP*5;

  s_mixer_first_run_done = true;

  simuSetSwitch(0, -1);
  CHECK_SLOW_MOVEMENT(0, -1, 250);
  EXPECT_EQ(chans[0], -CHANNEL_MAX);

  simuSetSwitch(0, 0);
  CHECK_DELAY(0, 500);
  CHECK_SLOW_MOVEMENT(0, +1, 250/*half course*/);
  EXPECT_EQ(chans[0], 0);

  simuSetSwitch(0, 1);
  CHECK_DELAY(0, 500);
  CHECK_SLOW_MOVEMENT(0, +1, 250);
}
#endif

#if !defined(CPUARM)
TEST_F(MixerTest, SlowOnSwitchReplace)
{
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

  simuSetSwitch(1, 0);
  evalFlightModeMixes(e_perout_mode_normal, 1);
  EXPECT_EQ(chans[0], CHANNEL_MAX/2);

  simuSetSwitch(1, 1);
  evalFlightModeMixes(e_perout_mode_normal, 1);
  // slow is not applied, but it's better than the first mix not applied at all!
  EXPECT_EQ(chans[0], CHANNEL_MAX);

  simuSetSwitch(1, 0);
  evalFlightModeMixes(e_perout_mode_normal, 1);
  // slow is not applied, but it's better than the first mix not applied at all!
  EXPECT_EQ(chans[0], CHANNEL_MAX/2);
}
#endif

#if !defined(VIRTUAL_INPUTS)
TEST_F(MixerTest, NoTrimOnInactiveMix)
{
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_Thr;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].swtch = SWSRC_THR;
  g_model.mixData[0].speedUp = SLOW_STEP*5;
  g_model.mixData[0].speedDown = SLOW_STEP*5;
  setTrimValue(0, 2, 256);

  s_mixer_first_run_done = true;

  simuSetSwitch(1, 1);
  CHECK_SLOW_MOVEMENT(0, 1, 100);

  simuSetSwitch(1, -1);
  CHECK_SLOW_MOVEMENT(0, -1, 100);
}
#endif

TEST_F(MixerTest, SlowOnMultiply)
{
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


#if defined(HELI) && defined(VIRTUAL_INPUTS)
TEST(Heli, BasicTest)
{
  SYSTEM_RESET();
  MODEL_RESET();
  MIXER_RESET();
  modelDefault(0);
  g_model.swashR.collectiveSource = MIXSRC_Thr;
  g_model.swashR.elevatorSource = MIXSRC_Ele;
  g_model.swashR.aileronSource = MIXSRC_Ail;
  g_model.swashR.collectiveWeight = 100;
  g_model.swashR.elevatorWeight = 100;
  g_model.swashR.aileronWeight = 100;
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
  SYSTEM_RESET();
  MODEL_RESET();
  MIXER_RESET();
  modelDefault(0);
  g_eeGeneral.templateSetup = 2;
  applyDefaultTemplate();
  g_model.swashR.collectiveSource = MIXSRC_Thr;
  g_model.swashR.elevatorSource = MIXSRC_Ele;
  g_model.swashR.aileronSource = MIXSRC_Ail;
  g_model.swashR.collectiveWeight = 100;
  g_model.swashR.elevatorWeight = 100;
  g_model.swashR.aileronWeight = 100;
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
  SYSTEM_RESET();
}
#endif

#if defined(HELI) && !defined(VIRTUAL_INPUTS)
TEST(Heli, SimpleTest)
{
  SYSTEM_RESET();
  MODEL_RESET();
  MIXER_RESET();
  modelDefault(0);
  applyTemplate(TMPL_HELI_SETUP);
  anaInValues[ELE_STICK] = 1024;
  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], -CHANNEL_MAX);
  EXPECT_EQ(chans[1], CHANNEL_MAX/2);
  EXPECT_EQ(chans[1], CHANNEL_MAX/2);
}
#endif

TEST(Trainer, UnpluggedTest)
{
  SYSTEM_RESET();
  MODEL_RESET();
  MIXER_RESET();
  modelDefault(0);
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_FIRST_TRAINER;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].delayUp = DELAY_STEP*5;
  g_model.mixData[0].delayDown = DELAY_STEP*5;
  ppmInputValidityTimer = 0;
  ppmInput[0] = 1024;
  CHECK_DELAY(0, 5000);
}
