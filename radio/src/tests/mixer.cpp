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

#include "gtests.h"

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

TEST(Trims, throttleTrim)
{
  MODEL_RESET();
  modelDefault(0);
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

TEST(Trims, invertedThrottlePlusThrottleTrim)
{
  MODEL_RESET();
  modelDefault(0);
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

TEST(Trims, throttleTrimWithZeroWeightOnThrottle)
{
  MODEL_RESET();
  modelDefault(0);
  g_model.thrTrim = 1;
#if defined(PCBTARANIS)
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

TEST(Trims, invertedThrottlePlusthrottleTrimWithZeroWeightOnThrottle)
{
  MODEL_RESET();
  modelDefault(0);
  g_model.throttleReversed = 1;
  g_model.thrTrim = 1;
#if defined(PCBTARANIS)
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

#if !defined(PCBTARANIS)
TEST(Trims, greaterTrimLink)
{
  MODEL_RESET();
  setTrimValue(1, RUD_STICK, TRIM_EXTENDED_MAX+3); // link to FP3 trim
  setTrimValue(3, RUD_STICK, 32);
  EXPECT_EQ(getRawTrimValue(getTrimFlightPhase(1, RUD_STICK), RUD_STICK), 32);
}

TEST(Trims, chainedTrims)
{
  MODEL_RESET();
  setTrimValue(0, RUD_STICK, 32);
  setTrimValue(1, RUD_STICK, TRIM_EXTENDED_MAX+1); // link to FP0 trim
  setTrimValue(2, RUD_STICK, TRIM_EXTENDED_MAX+2); // link to FP1 trim
  EXPECT_EQ(getRawTrimValue(getTrimFlightPhase(0, RUD_STICK), RUD_STICK), 32);
}

TEST(Trims, infiniteChainedTrims)
{
  MODEL_RESET();
  setTrimValue(0, RUD_STICK, 32);
  setTrimValue(1, RUD_STICK, TRIM_EXTENDED_MAX+3); // link to FP3 trim
  setTrimValue(2, RUD_STICK, TRIM_EXTENDED_MAX+2); // link to FP1 trim
  setTrimValue(3, RUD_STICK, TRIM_EXTENDED_MAX+3); // link to FP2 trim
  EXPECT_EQ(getRawTrimValue(getTrimFlightPhase(0, RUD_STICK), RUD_STICK), 32);
}
#endif

TEST(Trims, CopyTrimsToOffset)
{
  MODEL_RESET();
  modelDefault(0);
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

TEST(Trims, CopySticksToOffset)
{
  MODEL_RESET();
  modelDefault(0);
  anaInValues[ELE_STICK] = -100;
#if defined(CPUARM)
  doMixerCalculations();
#else
  perMain();
#endif
  copySticksToOffset(1);
  EXPECT_EQ(g_model.limitData[1].offset, -97);
}

TEST(Trims, InstantTrim)
{
  MODEL_RESET();
  modelDefault(0);
  anaInValues[AIL_STICK] = 50;
  instantTrim();
  EXPECT_EQ(25, getTrimValue(0, AIL_STICK));
}

#if defined(PCBTARANIS)
TEST(Trims, InstantTrimNegativeCurve)
{
  MODEL_RESET();
  modelDefault(0);
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
#if defined(CPUARM)
  doMixerCalculations();
#else
  perMain();
#endif
  EXPECT_EQ(chans[0], CHANNEL_MAX/2);
  EXPECT_EQ(chans[1], CHANNEL_MAX);
  simuSetSwitch(3, 0);
#if defined(CPUARM)
  doMixerCalculations();
#else
  perMain();
#endif
  EXPECT_EQ(chans[0], CHANNEL_MAX/2);
  EXPECT_EQ(chans[1], CHANNEL_MAX);
}

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
#if defined(PCBTARANIS)
  g_eeGeneral.switchConfig = 0x03;
#endif

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

TEST(Mixer, DelayOnSwitch)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_MAX;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].swtch = TR(SWSRC_THR, SWSRC_SA2);
  g_model.mixData[0].delayUp = DELAY_STEP*5;
  g_model.mixData[0].delayDown = DELAY_STEP*5;

  evalFlightModeMixes(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);

  simuSetSwitch(0, 1);
  CHECK_DELAY(0, 500);

  evalFlightModeMixes(e_perout_mode_normal, 1);
  EXPECT_EQ(chans[0], CHANNEL_MAX);

  simuSetSwitch(0, 0);
  CHECK_DELAY(0, 500);

  evalFlightModeMixes(e_perout_mode_normal, 1);
  EXPECT_EQ(chans[0], 0);
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


#if defined(HELI) && defined(VIRTUALINPUTS)
TEST(Heli, BasicTest)
{
  MODEL_RESET();
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
}

TEST(Heli, Mode2Test)
{
  MODEL_RESET();
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
}
#endif

#if defined(HELI) && !defined(PCBTARANIS)
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

TEST(Trainer, UnpluggedTest)
{
  MODEL_RESET();
  MIXER_RESET();
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
