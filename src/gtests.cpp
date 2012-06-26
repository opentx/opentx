/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
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

#include <gtest/gtest.h>
#include "open9x.h"

uint16_t anaIn(uint8_t chan)
{
  return 0;
}

TEST(trims, greaterTrimLink)
{
 memset(&g_model, 0, sizeof(g_model));
 setTrimValue(1, 0, TRIM_EXTENDED_MAX+3); // link to FP3 trim
 setTrimValue(3, 0, 32);
 EXPECT_EQ(getRawTrimValue(getTrimFlightPhase(0, 1), 0), 32);
}

TEST(trims, chainedTrims)
{
 memset(&g_model, 0, sizeof(g_model));
 setTrimValue(0, 0, 32);
 setTrimValue(1, 0, TRIM_EXTENDED_MAX+1); // link to FP0 trim
 setTrimValue(2, 0, TRIM_EXTENDED_MAX+2); // link to FP1 trim
 EXPECT_EQ(getRawTrimValue(getTrimFlightPhase(0, 2), 0), 32);
}

TEST(trims, infiniteChainedTrims)
{
 memset(&g_model, 0, sizeof(g_model));
 setTrimValue(0, 0, 32);
 setTrimValue(1, 0, TRIM_EXTENDED_MAX+3); // link to FP3 trim
 setTrimValue(2, 0, TRIM_EXTENDED_MAX+2); // link to FP1 trim
 setTrimValue(3, 0, TRIM_EXTENDED_MAX+3); // link to FP2 trim
 EXPECT_EQ(getRawTrimValue(getTrimFlightPhase(0, 2), 0), 32);
}

TEST(outdezNAtt, test_unsigned) {
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

TEST(EEPROM, 1000_random_writes) {
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

TEST(EEPROM, test2) {
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

TEST(EEPROM, eeCheckImmediately) {
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

TEST(EEPROM, copy) {
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

TEST(EEPROM, rm) {
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

extern void processFrskyPacket(uint8_t *packet);

TEST(FrSky, gpsNfuel) {
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

TEST(FrSky, dateNtime) {
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

TEST(getSwitch, undefCSW) {
  memset(&g_model, 0, sizeof(g_model));
  EXPECT_EQ(getSwitch(MAX_SWITCH-NUM_CSW, 0), false);
  EXPECT_EQ(getSwitch(-(MAX_SWITCH-NUM_CSW), 0), false);
}

TEST(getSwitch, circularCSW) {
  memset(&g_model, 0, sizeof(g_model));
  g_model.customSw[0] = { MAX_SWITCH-NUM_CSW, MAX_SWITCH-NUM_CSW, CS_OR };
  g_model.customSw[1] = { MAX_SWITCH-NUM_CSW, MAX_SWITCH-NUM_CSW, CS_AND };
  EXPECT_EQ(getSwitch(MAX_SWITCH-NUM_CSW, 0), false);
  EXPECT_EQ(getSwitch(-(MAX_SWITCH-NUM_CSW), 0), true);
  EXPECT_EQ(getSwitch(1+MAX_SWITCH-NUM_CSW, 0), false);
  EXPECT_EQ(getSwitch(-(1+MAX_SWITCH-NUM_CSW), 0), true);
}

TEST(getSwitch, nullSW) {
  memset(&g_model, 0, sizeof(g_model));
  EXPECT_EQ(getSwitch(0, 0), false);
  EXPECT_EQ(getSwitch(0, true), true);
  EXPECT_EQ(getSwitch(0, 0), false);
}

TEST(phases, nullFadeOut_posFadeIn) {
  memset(&g_model, 0, sizeof(g_model));
  g_model.phaseData[1].swtch = DSW_ID1;
  g_model.phaseData[1].fadeIn = 15;
  perMain();
  setSwitch(DSW_ID1);
  perMain();
}

int main(int argc, char **argv) {
  StartEepromThread(NULL);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
