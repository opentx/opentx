#ifndef eepromv3_h
#define eepromv3_h

#include <inttypes.h>

namespace EEPROM_V3 {

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
  //  uint8_t   warnOpts; //bitset for several warnings
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
  //expo[3][2][2] //[Norm/Dr][expo/weight][R/L]

  typedef struct t_ExpoData {
    int8_t  expo[3][2][2];
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

  typedef struct t_ModelData {
    char      name[10];             // 10 must be first for eeLoadModelName
    uint8_t   mdVers;
    int8_t    tmrMode;              // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
    uint8_t   tmrDir;               // 0=>Count Down, 1=>Count Up
    uint16_t  tmrVal;
    uint8_t   protocol;
    int8_t    ppmNCH;
    int8_t    thrTrim:4;            // Enable Throttle Trim
    int8_t    thrExpo:4;            // Enable Throttle Expo
    int8_t    trimInc;              // Trim Increments
    int8_t    ppmDelay;
    int8_t    trimSw;
    uint8_t   beepANACenter;        // 1<<0->A1.. 1<<6->A7
    uint8_t   pulsePol;
    char      res[3];
    MixData   mixData[32];
    LimitData limitData[16];
    ExpoData  expoData[4];
    int8_t    trim[4];
    int8_t    curves5[8][5];
    int8_t    curves9[8][9];
    CSwData   customSw[6];
    SwashRingData swashR;
  } __attribute__((packed)) ModelData;

}

#endif

