/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 *
 * gruvin9x is based on code named er9x by
 * Author - Erez Raviv <erezraviv@gmail.com>, which is in turn
 * was based on the original (and ongoing) project by Thomas Husterer,
 * th9x -- http://code.google.com/p/th9x/
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
#include "gruvin9x.h"

uint16_t anaIn(uint8_t chan)
{
  return 0;
}

TEST(incSubtrim, test1) {
  memset(&g_model, 0, sizeof(g_model));
  incSubtrim(0, 10);
  for (int p=0; p<MAX_PHASES; p++) {
    PhaseData *phase = phaseaddress(p);
    EXPECT_EQ(phase->trim[0], -10) << "Should be equal to -10";
  }
  EXPECT_EQ(g_model.subtrim[0], 10) << "Should be equal to 10";
}

TEST(incSubtrim, test2) {
  memset(&g_model, 0, sizeof(g_model));
  phaseaddress(0)->trim[0] = -120;
  phaseaddress(1)->trim[0] = +125;
  incSubtrim(0, +10); // FP1 is trimmed by +10
  EXPECT_EQ(phaseaddress(0)->trim[0], TRIM_MIN);
  EXPECT_EQ(phaseaddress(1)->trim[0], +115); // and set to 125 just after
  EXPECT_EQ(g_model.subtrim[0], 10);
}

TEST(trims, greaterTrimLink)
{
 memset(&g_model, 0, sizeof(g_model));
 phaseaddress(1)->trim[0] = -127; // link to FP3 trim
 phaseaddress(3)->trim[0] = 32;
 EXPECT_EQ(phaseaddress(getTrimFlightPhase(0, 1))->trim[0], 32);
}

TEST(trims, chainedTrims)
{
 memset(&g_model, 0, sizeof(g_model));
 phaseaddress(0)->trim[0] = 32;
 phaseaddress(1)->trim[0] = +127; // link to FP0 trim
 phaseaddress(2)->trim[0] = -128; // link to FP1 trim
 EXPECT_EQ(phaseaddress(getTrimFlightPhase(0, 2))->trim[0], 32);
}

TEST(trims, infiniteChainedTrims)
{
 memset(&g_model, 0, sizeof(g_model));
 phaseaddress(0)->trim[0] = 32;
 phaseaddress(1)->trim[0] = -127; // link to FP3 trim
 phaseaddress(2)->trim[0] = -128; // link to FP1 trim
 phaseaddress(3)->trim[0] = -127; // link to FP2 trim
 EXPECT_EQ(phaseaddress(getTrimFlightPhase(0, 2))->trim[0], 32);
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

TEST(EEPROM, test1) {
  eepromFile = NULL; // in memory
  RlcFile f;
  uint8_t buf[1000];
  uint8_t buf2[1000];

  EeFsFormat();

  for(int i=0; i<10000; i++) {
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

TEST(EEPROM, eeDuplicateModel) {
  eepromFile = NULL; // in memory

  uint8_t buf[1000];

  EeFsFormat();

  for(int i=0; i<1000; i++) buf[i]='6'+i%4;

  theFile.writeRlc(5, 6, buf, 300, true);

  eeDuplicateModel(4);

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

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
