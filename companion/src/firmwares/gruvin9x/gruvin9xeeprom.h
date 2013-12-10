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
#ifndef gruvin9xeeprom_h
#define gruvin9xeeprom_h

#include <inttypes.h>
#include "eeprominterface.h"
#define GR9X_MAX_CSFUNC 13

PACK(typedef struct t_Gruvin9xTrainerMix_v103 {
  uint8_t srcChn:3; //0-7 = ch1-8
  int8_t  swtch:5;
  int8_t  studWeight:6;
  uint8_t mode:2;   //off,add-mode,subst-mode

  operator TrainerMix();
  t_Gruvin9xTrainerMix_v103() { memset(this, 0, sizeof(t_Gruvin9xTrainerMix_v103)); }

}) Gruvin9xTrainerMix_v103; //

PACK(typedef struct t_Gruvin9xTrainerMix_v104 {
  uint8_t srcChn:6; // 0-7 = ch1-8
  uint8_t mode:2;   // off,add-mode,subst-mode
  int8_t  studWeight;

  operator TrainerMix();
  t_Gruvin9xTrainerMix_v104() { memset(this, 0, sizeof(t_Gruvin9xTrainerMix_v104)); }
  t_Gruvin9xTrainerMix_v104(TrainerMix&);

}) Gruvin9xTrainerMix_v104; //

PACK(typedef struct t_Gruvin9xTrainerData_v103 {
  int16_t        calib[4];
  Gruvin9xTrainerMix_v103     mix[4];

  operator TrainerData();
}) Gruvin9xTrainerData_v103;

PACK(typedef struct t_Gruvin9xTrainerData_v104 {
  int16_t        calib[4];
  Gruvin9xTrainerMix_v104     mix[4];

  operator TrainerData();
  t_Gruvin9xTrainerData_v104() { memset(this, 0, sizeof(t_Gruvin9xTrainerData_v104)); }
  t_Gruvin9xTrainerData_v104(TrainerData&);

}) Gruvin9xTrainerData_v104;

PACK(typedef struct t_Gruvin9xFrSkyRSSIAlarm {
  uint8_t       level:2;
  int8_t        value:6;
}) Gruvin9xFrSkyRSSIAlarm;

PACK(typedef struct t_Gruvin9xGeneral_v103 {
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
  Gruvin9xTrainerData_v103 trainer;
  uint8_t   view;      //index of subview in main scrren
  uint8_t   disableThrottleWarning:1;
  int8_t    switchWarning:2; // -1=down, 0=off, 1=up
  uint8_t   beeperVal:3;
  uint8_t   disableMemoryWarning:1;
  uint8_t   disableAlarmWarning:1;
  uint8_t   stickMode;
  uint8_t   inactivityTimer;
  uint8_t   throttleReversed:1;
  uint8_t   minuteBeep:1;
  uint8_t   preBeep:1;
  uint8_t   flashBeep:1;
  uint8_t   disableSplashScreen:1;
  uint8_t   enableTelemetryAlarm:1;   // 0=no, 1=yes (Sound alarm when there's no telem. data coming in)
  uint8_t   spare:2;
  uint8_t   filterInput;
  uint8_t   lightAutoOff;
  uint8_t   templateSetup;  //RETA order according to chout_ar array
  int8_t    PPM_Multiplier;
  Gruvin9xFrSkyRSSIAlarm frskyRssiAlarms[2];

  operator GeneralSettings();
  t_Gruvin9xGeneral_v103() { memset(this, 0, sizeof(t_Gruvin9xGeneral_v103)); }
}) Gruvin9xGeneral_v103;

PACK(typedef struct t_Gruvin9xGeneral_v104 {
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
  Gruvin9xTrainerData_v104 trainer;
  uint8_t   view;      //index of subview in main scrren
  uint8_t   disableThrottleWarning:1;
  int8_t    switchWarning:2; // -1=down, 0=off, 1=up
  uint8_t   beeperVal:3;
  uint8_t   disableMemoryWarning:1;
  uint8_t   disableAlarmWarning:1;
  uint8_t   stickMode;
  uint8_t   inactivityTimer;
  uint8_t   throttleReversed:1;
  uint8_t   minuteBeep:1;
  uint8_t   preBeep:1;
  uint8_t   flashBeep:1;
  uint8_t   disableSplashScreen:1;
  uint8_t   enableTelemetryAlarm:1;   // 0=no, 1=yes (Sound alarm when there's no telem. data coming in)
  uint8_t   spare:2;
  uint8_t   filterInput;
  uint8_t   lightAutoOff;
  uint8_t   templateSetup;  //RETA order according to chout_ar array
  int8_t    PPM_Multiplier;
  Gruvin9xFrSkyRSSIAlarm frskyRssiAlarms[2];

  operator GeneralSettings();
  t_Gruvin9xGeneral_v104() { memset(this, 0, sizeof(t_Gruvin9xGeneral_v104)); }
  t_Gruvin9xGeneral_v104(GeneralSettings&);

}) Gruvin9xGeneral_v104;

typedef Gruvin9xGeneral_v104 Gruvin9xGeneral;

// eeprom modelspec

PACK(typedef struct t_Gruvin9xExpoData {
  uint8_t mode:2;         // 0=end, 1=pos, 2=neg, 3=both
  uint8_t chn:2;
  uint8_t curve:4;        // 0=no curve, 1-6=std curves, 7-10=CV1-CV4, 11-15=CV9-CV13
  int8_t  swtch:5;
  uint8_t phase:3;        // if negPhase is 0: 0=normal, 5=FP4    if negPhase is 1: 5=!FP4
  uint8_t negPhase:1;
  uint8_t weight:7;
  int8_t  expo;

  operator ExpoData();
  t_Gruvin9xExpoData();
  t_Gruvin9xExpoData(ExpoData&);

}) Gruvin9xExpoData;

PACK(typedef struct t_Gruvin9xLimitData {
  int8_t  min;
  int8_t  max;
  bool    revert;
  int16_t  offset;

  operator LimitData();
  t_Gruvin9xLimitData();
  t_Gruvin9xLimitData(LimitData&);

}) Gruvin9xLimitData;

#define MLTPX_ADD  0
#define MLTPX_MUL  1
#define MLTPX_REP  2

PACK(typedef struct t_Gruvin9xMixData {
  uint8_t destCh:5;          // 0, 1..C9X_NUM_CHNOUT
  uint8_t mixWarn:3;         // mixer warning
  uint8_t srcRaw;            //
  int8_t  weight;
  int8_t  swtch;
  uint8_t curve;             // 0=symmetrisch, 1=no neg, 2=no pos
  uint8_t delayUp:4;
  uint8_t delayDown:4;
  uint8_t speedUp:4;         // Servogeschwindigkeit aus Tabelle (10ms Cycle)
  uint8_t speedDown:4;       // 0 nichts
  uint8_t carryTrim:1;
  uint8_t mltpx:3;           // multiplex method 0=+ 1=* 2=replace
  int8_t  phase:4;           // -5=!FP4, 0=normal, 5=FP4
  int8_t  sOffset;

  operator MixData();
  t_Gruvin9xMixData();
  t_Gruvin9xMixData(MixData&);

}) Gruvin9xMixData;

PACK(typedef struct t_Gruvin9xCustomSwData { // Custom Switches data
  int8_t  v1; //input
  int8_t  v2; //offset
  uint8_t func;

  operator CustomSwData();
  t_Gruvin9xCustomSwData() { memset(this, 0, sizeof(t_Gruvin9xCustomSwData)); }
  t_Gruvin9xCustomSwData(CustomSwData&);

}) Gruvin9xCustomSwData;

PACK(typedef struct t_Gruvin9xSafetySwData { // Safety Switches data
  int8_t  swtch;
  int8_t  val;

  operator SafetySwData();
  t_Gruvin9xSafetySwData() { memset(this, 0, sizeof(t_Gruvin9xSafetySwData)); }
  t_Gruvin9xSafetySwData(SafetySwData&);

}) Gruvin9xSafetySwData;

PACK(typedef struct t_Gruvin9xFuncSwData { // Function Switches data
  int8_t  swtch; // input
  uint8_t func;

  operator FuncSwData();
  t_Gruvin9xFuncSwData() { memset(this, 0, sizeof(t_Gruvin9xFuncSwData)); }
  t_Gruvin9xFuncSwData(FuncSwData&);

}) Gruvin9xFuncSwData;

PACK(typedef struct t_Gruvin9xFrSkyChannelData {
  uint16_t  ratio:12;           // (Maximum resistor divider input volts +/- calibration. 0 means channel not used.
                                    // 0.01v steps from 0 to 40.95V. Ex. 6.60 Volts = 660. 40.95V = 4095
  uint16_t  type:4;             // channel display unit (0=volts, 1=raw, 2-15=reserverd.)
  uint8_t   alarms_value[2];    // raw sample values 0..255
  uint8_t   alarms_level:4;     // two pairs of 2bits. none=0, yel=1, org=2, red=3
  uint8_t   alarms_greater:2;   // two alarms, 1 bit each. 0=LT(<), 1=GT(>)
  uint8_t   spare:2;
  uint8_t   barMin;             // minimum for bar display (raw ADC value)
  uint8_t   barMax;             // ditto for max display (would G:NOT usually = ratio)

  operator FrSkyChannelData();
  t_Gruvin9xFrSkyChannelData();
  t_Gruvin9xFrSkyChannelData(FrSkyChannelData&);

}) Gruvin9xFrSkyChannelData;

PACK(typedef struct t_Gruvin9xFrSkyData {
	Gruvin9xFrSkyChannelData channels[2];

	operator FrSkyData();
	t_Gruvin9xFrSkyData();
	t_Gruvin9xFrSkyData(FrSkyData&);

}) Gruvin9xFrSkyData;

PACK(typedef struct t_Gruvin9xSwashRingData { // Swash Ring data
  uint8_t   invertELE:1;
  uint8_t   invertAIL:1;
  uint8_t   invertCOL:1;
  uint8_t   type:5;
  uint8_t   collectiveSource;
  uint8_t   value;

  operator SwashRingData();
  t_Gruvin9xSwashRingData();
  t_Gruvin9xSwashRingData(SwashRingData&);

}) Gruvin9xSwashRingData;

PACK(typedef struct t_Gruvin9xPhaseData_v102 {
  int8_t trim[4];     // -125..125 => trim value, 127 => use trim of phase 0, -128, -127, -126 => use trim of phases 1|2|3|4 instead
  int8_t swtch;       // swtch of phase[0] is not used
  char name[6];
  uint8_t fadeIn:4;
  uint8_t fadeOut:4;

  operator PhaseData();
  t_Gruvin9xPhaseData_v102() { memset(this, 0, sizeof(t_Gruvin9xPhaseData_v102)); }
}) Gruvin9xPhaseData_v102;


PACK(typedef struct t_Gruvin9xPhaseData_v106 {
  int8_t trim[4];     // -500..500 => trim value, 501 => use trim of phase 0, 502, 503, 504 => use trim of phases 1|2|3|4 instead
  int8_t trim_ext:8;  // 2 less significant extra bits per trim (10bits trims)
  int8_t swtch;       // swtch of phase[0] is not used
  char name[6];
  uint8_t fadeIn:4;
  uint8_t fadeOut:4;

  operator PhaseData();
  t_Gruvin9xPhaseData_v106() { memset(this, 0, sizeof(t_Gruvin9xPhaseData_v106)); }
  t_Gruvin9xPhaseData_v106(PhaseData &eepe);
}) Gruvin9xPhaseData_v106;

PACK(typedef struct t_Gruvin9xTimerData {
  int8_t    mode:7;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint8_t   dir:1;             // 0=>Count Down, 1=>Count Up
  uint16_t  val;

  operator TimerData();
  t_Gruvin9xTimerData() { memset(this, 0, sizeof(t_Gruvin9xTimerData)); }
  t_Gruvin9xTimerData(TimerData &eepe);
}) Gruvin9xTimerData;

#define G9X_MAX_PHASES 5
#define G9X_MAX_MIXERS 32
#define G9X_MAX_EXPOS  14
#define G9X_MAX_CURVE5 8
#define G9X_MAX_CURVE9 8

#define G9X_NUM_CHNOUT   16 // number of real output channels CH1-CH16
#define G9X_NUM_CSW      12 // number of custom switches
#define G9X_NUM_FSW      12 // number of functions assigned to switches

// TODO
PACK(typedef struct t_Gruvin9xModelData_v102 {
  char      name[10];             // 10 must be first for eeLoadModelName
  Gruvin9xTimerData timer1;
  uint8_t   protocol:3;
  int8_t    ppmNCH:3;
  uint8_t   thrTrim:1;            // Enable Throttle Trim
  uint8_t   thrExpo:1;            // Enable Throttle Expo
  uint8_t   trimInc:3;            // Trim Increments
  uint8_t   spare1:1;
  uint8_t   pulsePol:1;
  uint8_t   extendedLimits:1;
  uint8_t   extendedTrims:1;
  uint8_t   spare2:1;
  int8_t    ppmDelay;
  uint8_t   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  Gruvin9xTimerData timer2;
  Gruvin9xMixData   mixData[G9X_MAX_MIXERS];
  Gruvin9xLimitData limitData[G9X_NUM_CHNOUT];
  Gruvin9xExpoData  expoData[G9X_MAX_EXPOS];
  int8_t    curves5[G9X_MAX_CURVE5][5];
  int8_t    curves9[G9X_MAX_CURVE9][9];
  Gruvin9xCustomSwData  customSw[G9X_NUM_CSW];
  Gruvin9xSafetySwData  safetySw[G9X_NUM_CHNOUT];
  Gruvin9xSwashRingData swashR;
  Gruvin9xPhaseData_v102 phaseData[G9X_MAX_PHASES];
  Gruvin9xFrSkyData frsky;

  operator ModelData();
  t_Gruvin9xModelData_v102() { memset(this, 0, sizeof(t_Gruvin9xModelData_v102)); }

}) Gruvin9xModelData_v102;

PACK(typedef struct t_Gruvin9xModelData_v103 {
  char      name[10];             // 10 must be first for eeLoadModelName
  Gruvin9xTimerData timer1;
  uint8_t   protocol:3;
  int8_t    ppmNCH:3;
  uint8_t   thrTrim:1;            // Enable Throttle Trim
  uint8_t   thrExpo:1;            // Enable Throttle Expo
  uint8_t   trimInc:3;            // Trim Increments
  uint8_t   spare1:1;
  uint8_t   pulsePol:1;
  uint8_t   extendedLimits:1;
  uint8_t   extendedTrims:1;
  uint8_t   spare2:1;
  int8_t    ppmDelay;
  uint8_t   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  Gruvin9xTimerData timer2;
  Gruvin9xMixData   mixData[G9X_MAX_MIXERS];
  Gruvin9xLimitData limitData[G9X_NUM_CHNOUT];
  Gruvin9xExpoData  expoData[G9X_MAX_EXPOS];
  int8_t    curves5[G9X_MAX_CURVE5][5];
  int8_t    curves9[G9X_MAX_CURVE9][9];
  Gruvin9xCustomSwData  customSw[G9X_NUM_CSW];
  Gruvin9xSafetySwData  safetySw[G9X_NUM_CHNOUT];
  Gruvin9xSwashRingData swashR;
  Gruvin9xPhaseData_v102 phaseData[G9X_MAX_PHASES];
  Gruvin9xFrSkyData frsky;

  operator ModelData();
  t_Gruvin9xModelData_v103() { memset(this, 0, sizeof(t_Gruvin9xModelData_v103)); }

}) Gruvin9xModelData_v103;

PACK(typedef struct t_Gruvin9xModelData_v105 {
  char      name[10];             // 10 must be first for eeLoadModelName
  Gruvin9xTimerData timer1;
  uint8_t   protocol:3;
  int8_t    ppmNCH:3;
  uint8_t   thrTrim:1;            // Enable Throttle Trim
  uint8_t   thrExpo:1;            // Enable Throttle Expo
  uint8_t   trimInc:3;            // Trim Increments
  uint8_t   spare1:1;
  uint8_t   pulsePol:1;
  uint8_t   extendedLimits:1;
  uint8_t   extendedTrims:1;
  uint8_t   spare2:1;
  int8_t    ppmDelay;
  uint8_t   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  Gruvin9xTimerData timer2;
  Gruvin9xMixData   mixData[G9X_MAX_MIXERS];
  Gruvin9xLimitData limitData[G9X_NUM_CHNOUT];
  Gruvin9xExpoData  expoData[G9X_MAX_EXPOS];
  int8_t    curves5[G9X_MAX_CURVE5][5];
  int8_t    curves9[G9X_MAX_CURVE9][9];
  Gruvin9xCustomSwData  customSw[G9X_NUM_CSW];
  Gruvin9xSafetySwData  safetySw[G9X_NUM_CHNOUT];
  Gruvin9xFuncSwData    funcSw[G9X_NUM_FSW];
  Gruvin9xSwashRingData swashR;
  Gruvin9xPhaseData_v102 phaseData[G9X_MAX_PHASES];
  int16_t   subtrim[NUM_STICKS];
  Gruvin9xFrSkyData frsky;

  operator ModelData();
  t_Gruvin9xModelData_v105() { memset(this, 0, sizeof(t_Gruvin9xModelData_v105)); }
  t_Gruvin9xModelData_v105(ModelData&);

}) Gruvin9xModelData_v105;

PACK(typedef struct t_Gruvin9xModelData_v106 {
  char      name[10];             // 10 must be first for eeLoadModelName
  Gruvin9xTimerData timer1;
  uint8_t   protocol:3;
  uint8_t   thrTrim:1;            // Enable Throttle Trim
  int8_t    ppmNCH:4;
  uint8_t   trimInc:3;            // Trim Increments
  uint8_t   thrExpo:1;            // Enable Throttle Expo
  uint8_t   pulsePol:1;
  uint8_t   extendedLimits:1;
  uint8_t   extendedTrims:1;
  uint8_t   spare2:1;
  int8_t    ppmDelay;
  uint8_t   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  Gruvin9xTimerData timer2;
  Gruvin9xMixData   mixData[G9X_MAX_MIXERS];
  Gruvin9xLimitData limitData[G9X_NUM_CHNOUT];
  Gruvin9xExpoData  expoData[G9X_MAX_EXPOS];
  int8_t    curves5[G9X_MAX_CURVE5][5];
  int8_t    curves9[G9X_MAX_CURVE9][9];
  Gruvin9xCustomSwData  customSw[G9X_NUM_CSW];
  Gruvin9xSafetySwData  safetySw[G9X_NUM_CHNOUT];
  Gruvin9xFuncSwData    funcSw[G9X_NUM_FSW];
  Gruvin9xSwashRingData swashR;
  Gruvin9xPhaseData_v106 phaseData[G9X_MAX_PHASES];
  Gruvin9xFrSkyData frsky;

  operator ModelData();
  t_Gruvin9xModelData_v106() { memset(this, 0, sizeof(t_Gruvin9xModelData_v106)); }
  t_Gruvin9xModelData_v106(ModelData&);

}) Gruvin9xModelData_v106;

typedef Gruvin9xModelData_v106 Gruvin9xModelData;

#endif
/*eof*/
