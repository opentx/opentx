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
#include "opentx.h"

#define CHANNEL_MAX (1024*256)

void doMixerCalculations();

#define MODEL_RESET() memset(&g_model, 0, sizeof(g_model))

#define MIXER_RESET() \
  memset(channelOutputs, 0, sizeof(channelOutputs)); \
  memset(ex_chans, 0, sizeof(ex_chans)); \
  memset(act, 0, sizeof(act)); \
  memset(swOn, 0, sizeof(swOn)); \
  int32_t lastAct = 0; lastAct = lastAct; /* to avoid a warning */

uint16_t anaInValues[NUM_STICKS+NUM_POTS] = { 0 };
uint16_t anaIn(uint8_t chan)
{
  if (chan < NUM_STICKS+NUM_POTS)
    return anaInValues[chan];
  else
    return 0;
}

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

TEST(outdezNAtt, test_unsigned)
{
  uint16_t altitude = 65530;

  uint8_t refBuf[sizeof(displayBuf)];
  memset(displayBuf, 0, sizeof(displayBuf));
  lcd_putc(0*FWNUM, 0, '6');
  lcd_putc(1*FWNUM, 0, '5');
  lcd_putc(2*FWNUM, 0, '5');
  lcd_putc(3*FWNUM, 0, '3');
  lcd_putc(4*FWNUM, 0, '0');
  memcpy(refBuf, displayBuf, sizeof(displayBuf));

  memset(displayBuf, 0, sizeof(displayBuf));
  lcd_outdezNAtt(1, 0, altitude, LEFT|UNSIGN);

  EXPECT_EQ(memcmp(refBuf, displayBuf, sizeof(displayBuf)), 0) << "Unsigned numbers will be bad displayed";
}

TEST(EEPROM, 1000_random_writes)
{
  eepromFile = NULL; // in memory
  RlcFile f;
  uint8_t buf[1000];
  uint8_t buf2[1000];

  EeFsFormat();

  for(int i=0; i<1000; i++) {
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

#if defined(FRSKY)

extern void processFrskyPacket(uint8_t *packet);

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
  processFrskyPacket(pkt1);
  processFrskyPacket(pkt2);
  processFrskyPacket(pkt3);
  processFrskyPacket(pkt4);
  processFrskyPacket(pkt5);
  processFrskyPacket(pkt6);
  processFrskyPacket(pkt7);
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
  processFrskyPacket(pkt1);
  processFrskyPacket(pkt2);
  processFrskyPacket(pkt3);
  EXPECT_EQ(frskyData.hub.day, 15);
  EXPECT_EQ(frskyData.hub.month, 07);
  EXPECT_EQ(frskyData.hub.year, 11);
  EXPECT_EQ(frskyData.hub.hour, 06);
  EXPECT_EQ(frskyData.hub.min, 18);
  EXPECT_EQ(frskyData.hub.sec, 50);
}

#endif

TEST(getSwitch, undefCSW)
{
  MODEL_RESET();
  EXPECT_EQ(getSwitch(MAX_PSWITCH), false);
  EXPECT_EQ(getSwitch(-MAX_PSWITCH), true); // no good answer there!
}

TEST(getSwitch, circularCSW)
{
  MODEL_RESET();
  g_model.customSw[0] = { MAX_SWITCH-NUM_CSW, MAX_SWITCH-NUM_CSW, CS_OR };
  g_model.customSw[1] = { MAX_SWITCH-NUM_CSW, MAX_SWITCH-NUM_CSW, CS_AND };
  EXPECT_EQ(getSwitch(MAX_SWITCH-NUM_CSW), false);
  EXPECT_EQ(getSwitch(-(MAX_SWITCH-NUM_CSW)), true);
  EXPECT_EQ(getSwitch(1+MAX_SWITCH-NUM_CSW), false);
  EXPECT_EQ(getSwitch(-(1+MAX_SWITCH-NUM_CSW)), true);
}

TEST(getSwitch, nullSW)
{
  MODEL_RESET();
  EXPECT_EQ(getSwitch(0), true);
}

TEST(FlightModes, nullFadeOut_posFadeIn)
{
  MODEL_RESET();
  g_model.phaseData[1].swtch = SWSRC_ID1;
  g_model.phaseData[1].fadeIn = 15;
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
  perOut(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);
  EXPECT_EQ(chans[1], CHANNEL_MAX);
  simuSetSwitch(0, 0);
  perOut(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);
  EXPECT_EQ(chans[1], 0);
  simuSetSwitch(0, 1);
  perOut(e_perout_mode_normal, 0);
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
  perOut(e_perout_mode_normal, 0);
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
  perOut(e_perout_mode_normal, 0);
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
  perOut(e_perout_mode_normal, 0);
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
  perOut(e_perout_mode_normal, 0);
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
  perOut(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], CHANNEL_MAX/2);
  EXPECT_EQ(chans[1], 0);
}

TEST(Mixer, RecursiveAddChannelAfterInactivePhase)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.phaseData[1].swtch = SWSRC_ID1;
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_CH2;
  g_model.mixData[0].phases = 0b11110;
  g_model.mixData[0].weight = 50;
  g_model.mixData[1].destCh = 0;
  g_model.mixData[1].mltpx = MLTPX_ADD;
  g_model.mixData[1].srcRaw = MIXSRC_MAX;
  g_model.mixData[1].phases = 0b11101;
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

#define CHECK_SLOW_MOVEMENT(channel, sign, duration) \
    do { \
    for (int i=1; i<=(duration); i++) { \
      perOut(e_perout_mode_normal, 1); \
      lastAct = lastAct + (sign) * (1<<19)/500; /* 100 on ARM */ \
      EXPECT_EQ(chans[(channel)], 256 * (lastAct >> 8)); \
    } \
    } while (0)

#define CHECK_DELAY(channel, duration) \
    do { \
      int32_t value = chans[(channel)]; \
      for (int i=1; i<=(duration); i++) { \
        perOut(e_perout_mode_normal, 1); \
        EXPECT_EQ(chans[(channel)], value); \
      } \
    } while (0)

TEST(Mixer, SlowOnSwitch)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_MAX;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].swtch = SWSRC_THR;
  g_model.mixData[0].speedUp = 10;
  g_model.mixData[0].speedDown = 10;

  perOut(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);

  simuSetSwitch(0, 1);
  CHECK_SLOW_MOVEMENT(0, +1, 250);

  simuSetSwitch(0, 0);
  CHECK_SLOW_MOVEMENT(0, -1, 250);
}

TEST(Mixer, SlowOnPhase)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.phaseData[1].swtch = SWSRC_THR;
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_MAX;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].phases = 0x2 + 0x4 + 0x8 + 0x10 /*only enabled in phase 0*/;
  g_model.mixData[0].speedUp = 10;
  g_model.mixData[0].speedDown = 10;

  s_perout_flight_phase = 0;
  perOut(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);

  CHECK_SLOW_MOVEMENT(0, +1, 250);

  s_perout_flight_phase = 1;
  CHECK_SLOW_MOVEMENT(0, -1, 250);
}

TEST(Mixer, SlowOnSwitchAndPhase)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.phaseData[1].swtch = SWSRC_THR;
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_MAX;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].swtch = SWSRC_THR;
  g_model.mixData[0].phases = 0x2 + 0x4 + 0x8 + 0x10 /*only enabled in phase 0*/;
  g_model.mixData[0].speedUp = 10;
  g_model.mixData[0].speedDown = 10;

  perOut(e_perout_mode_normal, 0);
  EXPECT_EQ(chans[0], 0);

  simuSetSwitch(0, 1);
  s_perout_flight_phase = 0;
  CHECK_SLOW_MOVEMENT(0, +1, 250);

  simuSetSwitch(0, 0);
  s_perout_flight_phase = 1;
  CHECK_SLOW_MOVEMENT(0, -1, 250);
}

TEST(Mixer, SlowOnSwitchSource)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_THR;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].speedUp = 10;
  g_model.mixData[0].speedDown = 10;

  simuSetSwitch(0, 0);
  CHECK_SLOW_MOVEMENT(0, -1, 250);
  EXPECT_EQ(chans[0], -CHANNEL_MAX);

  simuSetSwitch(0, 1);
  CHECK_SLOW_MOVEMENT(0, +1, 500);
}

TEST(Mixer, SlowAndDelayOnReplace3POSSource)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_REP;
  g_model.mixData[0].srcRaw = MIXSRC_3POS;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].delayUp = 10;
  g_model.mixData[0].speedUp = 10;
  g_model.mixData[0].speedDown = 10;

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
  g_model.mixData[1].speedDown = 10;

  simuSetSwitch(0, 0);
  perOut(e_perout_mode_normal, 1);
  EXPECT_EQ(chans[0], CHANNEL_MAX/2);

  simuSetSwitch(0, 1);
  perOut(e_perout_mode_normal, 1);
  // slow is not applied, but it's better than the first mix not applied at all!
  EXPECT_EQ(chans[0], CHANNEL_MAX);

  simuSetSwitch(0, 0);
  perOut(e_perout_mode_normal, 1);
  // slow is not applied, but it's better than the first mix not applied at all!
  EXPECT_EQ(chans[0], CHANNEL_MAX/2);
}

TEST(Mixer, NoTrimOnInactiveMix)
{
  MODEL_RESET();
  MIXER_RESET();
  g_model.mixData[0].destCh = 0;
  g_model.mixData[0].mltpx = MLTPX_ADD;
  g_model.mixData[0].srcRaw = MIXSRC_Thr;
  g_model.mixData[0].weight = 100;
  g_model.mixData[0].swtch = SWSRC_THR;
  g_model.mixData[0].speedUp = 10;
  g_model.mixData[0].speedDown = 10;
  setTrimValue(0, 2, 256);

  simuSetSwitch(0, 1);
  CHECK_SLOW_MOVEMENT(0, 1, 100);

  simuSetSwitch(0, 0);
  CHECK_SLOW_MOVEMENT(0, -1, 100);
}

TEST(Curves, LinearIntpol)
{
  MODEL_RESET();
  for (int8_t i=-2; i<=2; i++) {
    g_model.points[2+i] = 50*i;
  }
  EXPECT_EQ(intpol(-1024, 0), -1024);
  EXPECT_EQ(intpol(0, 0), 0);
  EXPECT_EQ(intpol(1024, 0), 1024);
  EXPECT_EQ(intpol(-192, 0), -192);
}

int main(int argc, char **argv) {
  StartEepromThread(NULL);
  g_menuStackPtr = 0;
  g_menuStack[0] = menuMainView;
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
