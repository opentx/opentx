/*
 * Author - Bertrand Songis <bsongis@gmail.com>
 *
 * Based on th9x -> http://code.google.com/p/th9x/
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
#ifndef th9xeeprom_h
#define th9xeeprom_h

#include <inttypes.h>
#include "eeprominterface.h"

//eeprom data
#define TH9X_NUM_CHNOUT   8
#define TH9X_MAX_EXPOS    15
#define TH9X_MAX_MIXERS   25
#define TH9X_MAX_SWITCHES 16
#define TH9X_MAX_CURVES3  3
#define TH9X_MAX_CURVES5  2
#define TH9X_MAX_CURVES9  2
#define TH9X_MAX_CSFUNC 13

#define MDVERS      6

#define TH9X_NUM_CSW         12 //number of custom switches
#define NUM_STICKSnPOTS      7  //number of sticks and pots

PACK(typedef struct t_Th9xTrainerMix {
  uint8_t srcChn:3; //0-7 = ch1-8
  int8_t  swtch:5;
  int8_t  studWeight:6;
  uint8_t mode:2;   // off, add-mode, subst-mode

  operator TrainerMix();
  t_Th9xTrainerMix();
  t_Th9xTrainerMix(TrainerMix&);

}) Th9xTrainerMix; //

PACK(typedef struct t_Th9xTrainerData {
  int16_t        calib[8];
  Th9xTrainerMix mix[4];

  operator TrainerData();
  t_Th9xTrainerData();
  t_Th9xTrainerData(TrainerData&);

}) Th9xTrainerData;

PACK(typedef struct t_Th9xGeneral {
  uint8_t   myVers;
  int16_t   calibMid[NUM_STICKSnPOTS];
  int16_t   calibSpanNeg[NUM_STICKSnPOTS];
  int16_t   calibSpanPos[NUM_STICKSnPOTS];
  uint8_t   inactivityMin;    //ge150
  uint8_t   iTrimSwitch:4;    //ge192
  uint8_t   iTrimTme1:2;
  uint8_t   iTrimTme2:2;
  uint8_t   currModel; //0..15
  uint8_t   contrast;
  uint8_t   vBatWarn;
  int8_t    vBatCalib;
  int8_t    lightSw;
  Th9xTrainerData trainer;
  uint8_t   adcFilt:2;
  uint8_t   keySpeed:2;
  uint8_t   thr0pos:4;
  uint8_t   disableThrottleWarning:1;
  uint8_t   disableSwitchWarning:1;
  uint8_t   disableMemoryWarning:1;
  uint8_t   beeperVal:2;
  uint8_t   view:3;
  uint8_t   stickMode:2;
  uint8_t   naviMode:2;

  operator GeneralSettings();
  t_Th9xGeneral();
  t_Th9xGeneral(GeneralSettings&);

}) Th9xGeneral;

/*
 * ModelData
 */

PACK(typedef struct t_Th9xExpoData {
  int8_t  exp5:5;
  uint8_t mode3:3; //0=end 1=pos 2=neg 3=both 4=trimNeg

  int8_t  weight6:6;
  uint8_t chn:2;  //

  int8_t  drSw:5;
  uint8_t curve:3; //

  operator ExpoData();
  t_Th9xExpoData();
  t_Th9xExpoData(ExpoData&);
}) Th9xExpoData;


PACK(typedef struct t_Th9xLimitData {
  int8_t  min:7;
  bool    scale:1;
  int8_t  max:7;
  bool    resv:1;
  bool    revert:1;
  int8_t  offset:7;

  operator LimitData();
  t_Th9xLimitData();
  t_Th9xLimitData(LimitData&);
}) Th9xLimitData;

#define MLTPX_ADD  0
#define MLTPX_MUL  1
#define MLTPX_REP  2

PACK(typedef struct t_Th9xMixData {
  uint8_t destCh:4;     // 1..C9X_NUM_CHNOUT,X1-X4
  uint8_t mixMode:2;    // + * =
  uint8_t dmy1:2;       //

  uint8_t srcRaw:5;     // S1-4,P1-3,max,cur,X1-X4,p1-3,T1-8  24
  uint8_t switchMode:2; // switchMode oOff\tiNeg\tiNul\tiPos
  uint8_t curveNeg:1;   // Cv cV

  int8_t  weight;

  int8_t  swtch:5;
  uint8_t curve:3;      // 0=off 1..7=cv1..7

  uint8_t speedUp:4;    // Servogeschwindigkeit aus Tabelle (10ms Cycle)
  uint8_t speedDown:4;  // 0 nichts

  operator MixData();
  t_Th9xMixData();
  t_Th9xMixData(MixData&);
}) Th9xMixData;

PACK(typedef struct t_Th9xCustomSwData {
  uint8_t sw:3;    // 0..7
  uint8_t opCmp:2; // < & | ^
  uint8_t opRes:3; // 0 => 1=> 0=> !=> & | ^
  int8_t val1;
  int8_t val2;

  operator CustomSwData();
  t_Th9xCustomSwData() { memset(this, 0, sizeof(t_Th9xCustomSwData)); }
  t_Th9xCustomSwData(CustomSwData &);
  int8_t fromSource(RawSource source);
  RawSource toSource(int8_t value);
}) Th9xCustomSwData;

PACK(typedef struct t_Th9xTrimData {
  int8_t  itrim:6; //trim index
  uint8_t tmode:2;

  t_Th9xTrimData() { }
}) Th9xTrimData;

PACK(typedef struct t_Th9xModelData {
  char      name[10];             // 10 must be first for eeLoadModelName
  uint8_t   mdVers;               // 1
  uint8_t   tmrMode:3;            // 1
  int8_t    tmrSw:5;              //
  uint16_t  tmrVal;               // 2
  uint8_t   protocol:5;           // 1
  uint8_t   protocolPar:3;        //
  char      res[3];               // 3
  Th9xLimitData limitData[TH9X_NUM_CHNOUT];// 3*8
  Th9xExpoData  expoTab[TH9X_MAX_EXPOS];      // 5*4 -> 3*15
  Th9xMixData   mixData[TH9X_MAX_MIXERS];  //0 5*25
  int8_t    curves3[TH9X_MAX_CURVES3][3];        // 9  new143
  int8_t    curves5[TH9X_MAX_CURVES5][5];        // 10
  int8_t    curves9[TH9X_MAX_CURVES9][9];        // 18
  Th9xCustomSwData switchTab[TH9X_MAX_SWITCHES];//
  Th9xTrimData   trimData[NUM_STICKS];    // 3*4 -> 1*4
  operator ModelData();
  t_Th9xModelData();
  t_Th9xModelData(ModelData&);
}) Th9xModelData;


#endif
/*eof*/
