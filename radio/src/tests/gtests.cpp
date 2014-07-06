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
bool checkScreenshot(QString test);


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

TEST(outdezNAtt, test_unsigned)
{
  lcd_clear();
  lcd_outdezNAtt(0, 0, 65530, LEFT|UNSIGN);
  EXPECT_TRUE(checkScreenshot("unsigned")) << "Unsigned numbers will be bad displayed";
}

#if !defined(PCBSKY9X)
TEST(EEPROM, 100_random_writes)
{
  eepromFile = NULL; // in memory
  RlcFile f;
  uint8_t buf[1000];
  uint8_t buf2[1000];

  EeFsFormat();

  for(int i=0; i<100; i++) {
    int size = rand()%800;
    for(int j=0; j<size; j++) {
      buf[j] = rand() < (RAND_MAX/10000*i) ? 0 : (j&0xff);
    }
    f.writeRlc(5, 5, buf, size, 100);
    // printf("size=%4d red=%4d\n\n\n", size, f.size());
    f.openRd(5);
    uint16_t n = f.readRlc(buf2,size+1);
    EXPECT_EQ(n, size);
    EXPECT_EQ(memcmp(buf, buf2, size), 0);
  }
}

TEST(EEPROM, test2)
{
  eepromFile = NULL; // in memory
  RlcFile f;
  uint8_t buf[1000];

  EeFsFormat();

  for(int i=0; i<1000; i++) buf[i]='6'+i%4;

  f.writeRlc(6, 6, buf, 300, 100);

  f.openRd(6);
  uint16_t sz=0;
  for(int i=0; i<500; i++){
    uint8_t b;
    uint16_t n=f.readRlc(&b,1);
    if(n) EXPECT_EQ(b, ('6'+sz%4));
    sz+=n;
  }
  EXPECT_EQ(sz, 300);
}

TEST(EEPROM, eeCheckImmediately)
{
  eepromFile = NULL; // in memory
  // RlcFile f;
  uint8_t buf[1000];

  EeFsFormat();

  for(int i=0; i<1000; i++) buf[i]='6'+i%4;

  theFile.writeRlc(6, 6, buf, 300, false);

  eeCheck(true);

  theFile.openRd(6);
  uint16_t sz=0;
  for(int i=0; i<500; i++){
    uint8_t b;
    uint16_t n=theFile.readRlc(&b,1);
    if(n) EXPECT_EQ(b, ('6'+sz%4));
    sz+=n;
  }
  EXPECT_EQ(sz, 300);
}

TEST(EEPROM, copy)
{
  eepromFile = NULL; // in memory

  uint8_t buf[1000];

  EeFsFormat();

  for(int i=0; i<1000; i++) buf[i]='6'+i%4;

  theFile.writeRlc(5, 6, buf, 300, true);

  theFile.copy(6, 5);

  theFile.openRd(6);
  uint16_t sz=0;
  for(int i=0; i<500; i++){
    uint8_t b;
    uint16_t n=theFile.readRlc(&b,1);
    if(n) EXPECT_EQ(b, ('6'+sz%4));
    sz+=n;
  }
  EXPECT_EQ(sz, 300);
}

TEST(EEPROM, rm)
{
  eepromFile = NULL; // in memory

  uint8_t buf[1000];

  EeFsFormat();

  for(int i=0; i<1000; i++) buf[i]='6'+i%4;

  theFile.writeRlc(5, 6, buf, 300, true);

  EXPECT_EQ(EFile::exists(5), true);

  EFile::rm(5);

  EXPECT_EQ(EFile::exists(5), false);

  theFile.openRd(5);
  uint16_t sz=0;
  for(int i=0; i<500; i++){
    uint8_t b;
    uint16_t n=theFile.readRlc(&b,1);
    if(n) EXPECT_EQ(b, ('6'+sz%4));
    sz+=n;
  }
  EXPECT_EQ(sz, 0);
}
#endif

#if defined(FRSKY) && !defined(FRSKY_SPORT)
extern void frskyDProcessPacket(uint8_t *packet);
TEST(FrSky, gpsNfuel)
{
  g_model.frsky.usrProto = 1;
  frskyData.hub.gpsFix = 1;

  uint8_t pkt1[] = { 0xfd, 0x07, 0x00, 0x5e, 0x14, 0x2c, 0x00, 0x5e, 0x1c, 0x03 };
  uint8_t pkt2[] = { 0xfd, 0x07, 0x00, 0x00, 0x5e, 0x13, 0x38, 0x0c, 0x5e, 0x1b };
  uint8_t pkt3[] = { 0xfd, 0x07, 0x00, 0xc9, 0x06, 0x5e, 0x23, 0x4e, 0x00, 0x5e };
  uint8_t pkt4[] = { 0xfd, 0x07, 0x00, 0x12, 0xef, 0x2e, 0x5e, 0x1a, 0x98, 0x26 };
  uint8_t pkt5[] = { 0xfd, 0x07, 0x00, 0x5e, 0x22, 0x45, 0x00, 0x5e, 0x11, 0x02 };
  uint8_t pkt6[] = { 0xfd, 0x07, 0x00, 0x00, 0x5e, 0x19, 0x93, 0x00, 0x5e, 0x04 };
  uint8_t pkt7[] = { 0xfd, 0x03, 0x00, 0x64, 0x00, 0x5e };
  frskyDProcessPacket(pkt1);
  frskyDProcessPacket(pkt2);
  frskyDProcessPacket(pkt3);
  frskyDProcessPacket(pkt4);
  frskyDProcessPacket(pkt5);
  frskyDProcessPacket(pkt6);
  frskyDProcessPacket(pkt7);
  EXPECT_EQ(frskyData.hub.gpsCourse_bp, 44);
  EXPECT_EQ(frskyData.hub.gpsCourse_ap, 03);
  EXPECT_EQ(frskyData.hub.gpsLongitude_bp / 100, 120);
  EXPECT_EQ(frskyData.hub.gpsLongitude_bp % 100, 15);
  EXPECT_EQ(frskyData.hub.gpsLongitude_ap, 0x2698);
  EXPECT_EQ(frskyData.hub.gpsLatitudeNS, 'N');
  EXPECT_EQ(frskyData.hub.gpsLongitudeEW, 'E');
  EXPECT_EQ(frskyData.hub.fuelLevel, 100);
}

TEST(FrSky, dateNtime)
{
  uint8_t pkt1[] = { 0xfd, 0x07, 0x00, 0x5e, 0x15, 0x0f, 0x07, 0x5e, 0x16, 0x0b };
  uint8_t pkt2[] = { 0xfd, 0x07, 0x00, 0x00, 0x5e, 0x17, 0x06, 0x12, 0x5e, 0x18 };
  uint8_t pkt3[] = { 0xfd, 0x03, 0x00, 0x32, 0x00, 0x5e };
  frskyDProcessPacket(pkt1);
  frskyDProcessPacket(pkt2);
  frskyDProcessPacket(pkt3);
  EXPECT_EQ(frskyData.hub.day, 15);
  EXPECT_EQ(frskyData.hub.month, 07);
  EXPECT_EQ(frskyData.hub.year, 11);
  EXPECT_EQ(frskyData.hub.hour, 06);
  EXPECT_EQ(frskyData.hub.min, 18);
  EXPECT_EQ(frskyData.hub.sec, 50);
}
#endif

#if defined(FRSKY_SPORT)
extern bool checkSportPacket(uint8_t *packet);
TEST(FrSkySPORT, checkCrc)
{
  // uint8_t pkt[] = { 0x7E, 0x98, 0x10, 0x10, 0x00, 0x7D, 0x5E, 0x02, 0x00, 0x00, 0x5F };
  uint8_t pkt[] = { 0x7E, 0x98, 0x10, 0x10, 0x00, 0x7E, 0x02, 0x00, 0x00, 0x5F };
  EXPECT_EQ(checkSportPacket(pkt+1), true);
}
#endif


#if defined(FRSKY) && defined(CPUARM)
TEST(FrSky, FrskyValueWithMinAveraging)
{
  /*
    The following expected[] array is filled
    with values that correspond to 4 elements 
    long averaging buffer.
    If length of averaging buffer is changed, this
    values must be adjusted
  */
  uint8_t expected[] = { 10, 12, 17, 25, 35, 45, 55, 65, 75, 85, 92, 97, 100, 100, 100, 100, 100};
  int testPos = 0;
  //test of averaging
  FrskyValueWithMin testVal;
  testVal.value = 0;  
  testVal.set(10);
  EXPECT_EQ(testVal.value, expected[testPos++]);
  for(int n=2; n<10; ++n) {
    testVal.set(n*10);
    EXPECT_EQ(testVal.value, expected[testPos++]);
  }
  for(int n=2; n<10; ++n) {
    testVal.set(100);
    EXPECT_EQ(testVal.value, expected[testPos++]);
  }
}
#endif


#if !defined(PCBTARANIS)
TEST(getSwitch, undefCSW)
{
  MODEL_RESET();
  EXPECT_EQ(getSwitch(NUM_PSWITCH), false);
  EXPECT_EQ(getSwitch(-NUM_PSWITCH), true); // no good answer there!
}
#endif

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

#if defined(PCBTARANIS)
TEST(getSwitch, OldTypeStickyCSW)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.logicalSw[0] = { SWSRC_SA0, 0, 0, LS_FUNC_AND };
  g_model.logicalSw[1] = { SWSRC_SW1, SWSRC_SW2, 0, LS_FUNC_OR };

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

void doPaint(QPainter & p)
{
  QRgb rgb = qRgb(161, 161, 161);

  p.setBackground(QBrush(rgb));
  p.eraseRect(0, 0, LCD_W, LCD_H);

  if (1) {
#if !defined(PCBTARANIS)
    rgb = qRgb(0, 0, 0);
    p.setPen(rgb);
    p.setBrush(QBrush(rgb));
#endif

#if defined(PCBTARANIS)
    unsigned int previousDepth = 0xFF;
#endif

    for (int y=0; y<LCD_H; y++) {
#if defined(PCBTARANIS)
      unsigned int idx = (y/2) * LCD_W;
#else
      unsigned int idx = (y/8) * LCD_W;
      unsigned int mask = (1 << (y%8));
#endif
      for (int x=0; x<LCD_W; x++, idx++) {
#if !defined(PCBTARANIS)
        if (lcd_buf[idx] & mask) {
          p.drawPoint(x, y);
        }
#else
        unsigned int z = (y & 1) ? (lcd_buf[idx] >> 4) : (lcd_buf[idx] & 0x0F);
        if (z) {
          if (z != previousDepth) {
            previousDepth = z;
            rgb = qRgb(161-(z*161)/15, 161-(z*161)/15, 161-(z*161)/15);
            p.setPen(rgb);
            p.setBrush(QBrush(rgb));
          }
          p.drawPoint(x, y);
        }
#endif
      }
    }
  }
}

bool checkScreenshot(QString test)
{
  lcdRefresh();
  QImage buffer(LCD_W, LCD_H, QImage::Format_RGB32);
  QPainter p(&buffer);
  doPaint(p);
  QString filename(QString("%1_%2x%3.png").arg(test).arg(LCD_W).arg(LCD_H));
  buffer.save("/tmp/" + filename);
  QFile screenshot("/tmp/" + filename);
  if (!screenshot.open(QIODevice::ReadOnly))
    return false;
  QFile reference("./tests/" + filename);
  if (!reference.open(QIODevice::ReadOnly))
    return false;
  if (reference.readAll() != screenshot.readAll())
    return false;
  screenshot.remove();
  return true;
}

TEST(Lcd, Invers_0_0)
{
  lcd_clear();
  lcd_putsAtt(0, 0, "Test", INVERS);
  EXPECT_TRUE(checkScreenshot("invers_0_0"));
}

TEST(Lcd, Invers_0_1)
{
  lcd_clear();
  lcd_putsAtt(0, 1, "Test", INVERS);
  EXPECT_TRUE(checkScreenshot("invers_0_1"));
}

TEST(Lcd, Prec2_Left)
{
  lcd_clear();
  lcd_outdezAtt(0, 0, 2, PREC2|LEFT);
  EXPECT_TRUE(checkScreenshot("prec2_left"));
}

TEST(Lcd, Prec2_Right)
{
  lcd_clear();
  lcd_outdezAtt(LCD_W, LCD_H-FH, 2, PREC2);
  EXPECT_TRUE(checkScreenshot("prec2_right"));
}

#if defined(CPUARM)
TEST(Lcd, Prec1_Dblsize_Invers)
{
  lcd_clear();
  lcd_outdezAtt(LCD_W, 10, 51, PREC1|DBLSIZE|INVERS);
  EXPECT_TRUE(checkScreenshot("prec1_dblsize_invers"));
}
#endif

TEST(Lcd, Line_Wrap)
{
  lcd_clear();
  lcd_puts(LCD_W-10, 0, "TEST");
  EXPECT_TRUE(checkScreenshot("line_wrap"));
}

#if defined(CPUARM)
TEST(Lcd, Smlsize_putsStrIdx)
{
  lcd_clear();
  putsStrIdx(0, 0, "FM", 0, SMLSIZE);
  EXPECT_TRUE(checkScreenshot("smlsize_putsstridx"));
}
#endif

TEST(Lcd, vline)
{
  lcd_clear();
  for (int x=0; x<100; x+=2) {
    lcd_vline(x, x/2, 12);
  }
  EXPECT_TRUE(checkScreenshot("vline"));
}

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);
  StartEepromThread(NULL);
  g_menuStackPtr = 0;
  g_menuStack[0] = menuMainView;
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
