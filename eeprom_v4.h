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

#ifndef eepromv4_h
#define eepromv4_h

#include <inttypes.h>

namespace EEPROM_V4 {

typedef struct t_TrainerMix {
  uint8_t srcChn:3; //0-7 = ch1-8
  int8_t  swtch:5;
  int8_t  studWeight:6;
  uint8_t mode:2;   //off,add-mode,subst-mode
} __attribute__((packed)) TrainerMix; //

typedef struct t_TrainerData {
  int16_t        calib[4];
  TrainerMix     mix[4];
} __attribute__((packed)) TrainerData;

typedef struct t_EEGeneral {
  uint8_t   myVers;
  int16_t   calibMid[7];
  int16_t   calibSpanNeg[7];
  int16_t   calibSpanPos[7];
  uint16_t  chkSum;
  uint8_t   currModel; //0..15
  uint8_t   contrast;
  uint8_t   vBatWarn;
  int8_t    vBatCalib;
  int8_t    lightSw;
  TrainerData trainer;
  uint8_t   view;     //index of subview in main scrren
  uint8_t   disableThrottleWarning:1;
  uint8_t   disableSwitchWarning:1;
  uint8_t   disableMemoryWarning:1;
  uint8_t   beeperVal:3;
  uint8_t   reserveWarning:1;
  uint8_t   disableAlarmWarning:1;
  uint8_t   stickMode;
  uint8_t   inactivityTimer;
  uint8_t   throttleReversed:1;
  uint8_t   minuteBeep:1;
  uint8_t   preBeep:1;
  uint8_t   flashBeep:1;
  uint8_t   disableSplashScreen:1;
  uint8_t   res1:3;
  uint8_t   filterInput;
  uint8_t   lightAutoOff;
  uint8_t   templateSetup;  //RETA order according to chout_ar array
  int8_t    PPM_Multiplier;
  uint8_t   res[1];
} __attribute__((packed)) EEGeneral;

//eeprom modelspec

typedef struct t_ExpoData {
  int8_t  expo[3][2][2]; //[Norm/Dr][expo/weight][R/L]
  int8_t  drSw1;
  int8_t  drSw2;
} __attribute__((packed)) ExpoData;

typedef struct t_LimitData {
  int8_t  min;
  int8_t  max;
  bool    revert;
  int16_t  offset;
} __attribute__((packed)) LimitData;

typedef struct t_MixData {
  uint8_t destCh;            //        1..NUM_CHNOUT
  uint8_t srcRaw;            //
  int8_t  weight;
  int8_t  swtch;
  uint8_t curve;             //0=symmetrisch 1=no neg 2=no pos
  uint8_t delayUp:4;
  uint8_t delayDown:4;
  uint8_t speedUp:4;         // Servogeschwindigkeit aus Tabelle (10ms Cycle)
  uint8_t speedDown:4;       // 0 nichts
  uint8_t carryTrim:1;
  uint8_t mltpx:3;           // multiplex method 0=+ 1=* 2=replace
  uint8_t mixWarn:4;         // mixer warning
  int8_t  sOffset;
  int8_t  res;
} __attribute__((packed)) MixData;

typedef struct t_CSwData { // Custom Switches data
  int8_t  v1; //input
  int8_t  v2; //offset
  uint8_t func;
} __attribute__((packed)) CSwData;


typedef struct t_SwashRingData { // Swash Ring data
  uint8_t lim;   // 0 mean off 100 full deflection
  uint8_t chX; // 2 channels to limit
  uint8_t chY; // 2 channels to limit
} __attribute__((packed)) SwashRingData;

typedef struct t_SafetySwData { // Custom Switches data
  int8_t  swtch;
  int8_t  val;
} __attribute__((packed)) SafetySwData;

typedef struct t_FrSkyChannelData {
  uint8_t   ratio;                // 0.0 means not used, 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   alarms_value[2];      // 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   alarms_level:4;
  uint8_t   alarms_greater:2;     // 0=LT(<), 1=GT(>)
  uint8_t   type:2;               // future use: 0=volts, ...
} __attribute__((packed)) FrSkyChannelData;

typedef struct t_FrSkyData {
  FrSkyChannelData channels[2];
} __attribute__((packed)) FrSkyData;

typedef struct t_ModelData {
  char      name[10];             // 10 must be first for eeLoadModelName
  uint8_t   mdVers;
  int8_t    tmrMode;              // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint8_t   tmrDir:1;    //0=>Count Down, 1=>Count Up
  uint8_t   traineron:1;  // 0 disable trainer, 1 allow trainer
  uint8_t   spare:6;
  uint16_t  tmrVal;
  uint8_t   protocol;
  int8_t    ppmNCH;
  int8_t    thrTrim:4;            // Enable Throttle Trim
  int8_t    thrExpo:4;            // Enable Throttle Expo
  int8_t    trimInc;              // Trim Increments
  int8_t    ppmDelay;
  int8_t    trimSw;
  uint8_t   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  uint8_t   pulsePol:1;
  uint8_t   extendedLimits:1;
  uint8_t   swashInvertELE:1;
  uint8_t   swashInvertAIL:1;
  uint8_t   swashInvertCOL:1;
  uint8_t   swashType:3;
  uint8_t   swashCollectiveSource;
  uint8_t   swashRingValue;
  char      res1;
  MixData   mixData[32];
  LimitData limitData[16];
  ExpoData  expoData[4];
  int8_t    trim[4];
  int8_t    curves5[8][5];
  int8_t    curves9[8][9];
  CSwData   customSw[12/*er9x=12, gruvin9x=6*/];
  /* er9x only */
  uint8_t   a1voltRatio;  //FrSky
  uint8_t   a2voltRatio;  //FrSky
  uint8_t   res3;
  SafetySwData  safetySw[16];
  FrSkyData frsky;
} __attribute__((packed)) ModelData;

}

#endif
