/*
 * Author - Erez Raviv <erezraviv@gmail.com>
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
#ifndef eeprom_h
#define eeprom_h

#include <inttypes.h>


//eeprom data
//#define EE_VERSION 2
#define MAX_MODELS  32
#define MAX_MIXERS  32
#define MAX_SKYMIXERS  48
#define MAX_CURVE5  8
#define MAX_CURVE9  8
#define MDVERS_r9   1
#define MDVERS_r14  2
#define MDVERS_r22  3
#define MDVERS_r77  4
#define MDVERS_r85  5
#define MDVERS_r261 6
#define MDVERS_r352 7
#define MDVERS_r365 8
#define MDVERS_r668 9
#define MDVERS_e119 10
#define MDVERS      11

#define MDSKYVERS   2

#define	NUM_VOICE		8

//OBSOLETE - USE ONLY MDVERS NOW
//#define GENERAL_MYVER_r261 3
//#define GENERAL_MYVER_r365 4
//#define GENERAL_MYVER      5


// eeprom ver <9 => mdvers == 1
// eeprom ver >9 => mdvers ==2

#define WARN_THR_BIT  0x01
#define WARN_BEP_BIT  0x80
#define WARN_SW_BIT   0x02
#define WARN_MEM_BIT  0x04
#define WARN_BVAL_BIT 0x38

#define WARN_THR     (!(g_eeGeneral.warnOpts & WARN_THR_BIT))
#define WARN_BEP     (!(g_eeGeneral.warnOpts & WARN_BEP_BIT))
#define WARN_SW      (!(g_eeGeneral.warnOpts & WARN_SW_BIT))
#define WARN_MEM     (!(g_eeGeneral.warnOpts & WARN_MEM_BIT))
#define BEEP_VAL     ( (g_eeGeneral.warnOpts & WARN_BVAL_BIT) >>3 )

// Bits in stickGain
#define STICK_LV_GAIN	0x40
#define STICK_LH_GAIN	0x10
#define STICK_RV_GAIN	0x04
#define STICK_RH_GAIN	0x01

#define GENERAL_OWNER_NAME_LEN 10
#define MODEL_NAME_LEN         10

#define MAX_GVARS 7

#define MAX_PHASES		6

#ifndef PACK
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

PACK(typedef struct t_TrainerMix {
  uint8_t srcChn:3; //0-7 = ch1-8
  int8_t  swtch:5;
  int8_t  studWeight:6;
  uint8_t mode:2;   //off,add-mode,subst-mode
}) TrainerMix; //
 
PACK(typedef struct t_TrainerData {
  int16_t        calib[4];
  TrainerMix     mix[4];
}) TrainerData;

PACK(typedef struct t_EEGeneral {
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
  uint8_t   view;
  uint8_t   disableThrottleWarning:1;
  uint8_t   disableSwitchWarning:1;
  uint8_t   disableMemoryWarning:1;
  uint8_t   beeperVal:3;
  uint8_t   reserveWarning:1;
  uint8_t   disableAlarmWarning:1;
  uint8_t   stickMode;
  int8_t    inactivityTimer;
  uint8_t   throttleReversed:1;
  uint8_t   minuteBeep:1;
  uint8_t   preBeep:1;
  uint8_t   flashBeep:1;
  uint8_t   disableSplashScreen:1;
  uint8_t   disablePotScroll:1;
  uint8_t   stickScroll:1 ;
  uint8_t   frskyinternalalarm:1;
  uint8_t   filterInput;
  uint8_t   lightAutoOff;
  uint8_t   templateSetup;  //RETA order according to chout_ar array 
  int8_t    PPM_Multiplier;
  uint8_t	FRSkyYellow:4;
  uint8_t	FRSkyOrange:4;
  uint8_t	FRSkyRed:4;  //mike please check these are correct
  uint8_t   hideNameOnSplash:1;
  uint8_t   optrexDisplay:1;
  uint8_t   unexpectedShutdown:1;
  uint8_t   spare:1;
  uint8_t   speakerPitch;
  uint8_t	hapticStrength;
  uint8_t	speakerMode;
  uint8_t   lightOnStickMove;
  char      ownerName[GENERAL_OWNER_NAME_LEN];
  uint8_t   switchWarningStates;
	uint8_t		volume ;
	uint8_t 	bright ;			// backlight
  uint8_t   stickGain;
	uint8_t		mAh_alarm ;
	uint16_t	mAh_used ;
	uint16_t	run_time ;
	int8_t		current_calib ;
	uint8_t		bt_baudrate ;
	uint8_t		rotaryDivisor ;
	uint8_t   crosstrim:1;
	uint8_t   spare9:7;
	int8_t   rtcCal ;
  int16_t   x9dcalibMid ;			// X9D
  int16_t   x9dcalibSpanNeg ;	// X9D
  int16_t   x9dcalibSpanPos ;	// X9D
	uint8_t		stickReverse ;
	uint8_t		language ;
}) EEGeneral;





//eeprom modelspec
//expo[3][2][2] //[Norm/Dr][expo/weight][R/L]

PACK(typedef struct t_ExpoData {
  int8_t  expo[3][2][2];
  int8_t  drSw1;
  int8_t  drSw2;
}) ExpoData;


PACK(typedef struct t_LimitData {
  int8_t  min;
  int8_t  max;
  bool    revert;
  int16_t  offset;
}) LimitData;

#define MLTPX_ADD  0
#define MLTPX_MUL  1
#define MLTPX_REP  2

PACK(typedef struct t_MixData {
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
  uint8_t mixWarn:2;         // mixer warning
  uint8_t enableFmTrim:1;
  uint8_t mixres:1;
  int8_t  sOffset;
  int8_t  res;
}) MixData;


PACK(typedef struct t_CSwData { // Custom Switches data
  int8_t  v1; //input
  int8_t  v2; 		//offset
	uint8_t func:4;
	uint8_t andsw:4;
}) CSwData;

PACK(typedef struct t_SafetySwData { // Custom Switches data
	union opt
	{
		struct ss
		{	
	    int8_t  swtch:6;
			uint8_t mode:2;
    	int8_t  val;
		} ss ;
		struct vs
		{
  		uint8_t vswtch:5 ;
			uint8_t vmode:3 ; // ON, OFF, BOTH, 15Secs, 30Secs, 60Secs, Varibl
    	uint8_t vval;
		} vs ;
	} opt ;
}) SafetySwData;

PACK(typedef struct t_FrSkyChannelData {
  uint8_t   ratio;                // 0.0 means not used, 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   alarms_value[2];      // 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   alarms_level:4;
  uint8_t   alarms_greater:2;     // 0=LT(<), 1=GT(>)
  uint8_t   type:2;               // 0=volts, 1=raw, 2=volts*2, 3=Amps
}) FrSkyChannelData;

PACK(typedef struct t_FrSkyData {
  FrSkyChannelData channels[2];
}) FrSkyData;

PACK(typedef struct t_FrSkyalarms
{
	uint8_t frskyAlarmType ;
	uint8_t frskyAlarmLimit ;
	uint8_t frskyAlarmSound ;
}) FrSkyAlData;

PACK(typedef struct t_FrSkyAlarmData {
		FrSkyAlData alarmData[8] ;
}) FrSkyAlarmData;

PACK(typedef struct t_TimerMode
{
		uint8_t   tmrModeA:7 ;          // timer trigger source -> off, abs, stk, stk%, cx%
    uint8_t   tmrDir:1 ;						// Timer direction
    int8_t    tmrModeB ;            // timer trigger source -> !sw, none, sw, m_sw
    uint16_t  tmrVal ;
}) TimerMode ;

//PACK(typedef struct t_swVoice {
//  uint8_t  swtch:5 ;
//	uint8_t mode:3 ; // ON, OFF, BOTH, 15Secs, 30Secs, 60Secs
//  uint8_t  val ;
//}) old_voiceSwData ;


PACK(typedef struct t_ModelData {
  char      name[MODEL_NAME_LEN];             // 10 must be first for eeLoadModelName
  uint8_t   modelVoice ;		// Index to model name voice (261+value)
  uint8_t   RxNum ;						// was timer trigger source, now RxNum for model match
  uint8_t   sparex:1;				// was tmrDir, now use tmrVal>0 => count down
  uint8_t   traineron:1;  		// 0 disable trainer, 1 allow trainer
  uint8_t   spare22:1 ;  			// Start timer2 using throttle
  uint8_t   FrSkyUsrProto:1 ; // Protocol in FrSky User Data, 0=FrSky Hub, 1=WS HowHigh
  uint8_t   FrSkyGpsAlt:1 ;		// Use Gps Altitude as main altitude reading
  uint8_t   FrSkyImperial:1 ; // Convert FrSky values to imperial units
  uint8_t   FrSkyAltAlarm:2;
	uint8_t		version ;
  uint8_t   spare_u8 ;				// Was timerval
  uint8_t   protocol;
  int8_t    ppmNCH;
  uint8_t   thrTrim:1;            // Enable Throttle Trim
	uint8_t   numBlades:2;					// RPM scaling
	uint8_t   spare10:1;
  uint8_t   thrExpo:1;            // Enable Throttle Expo
	uint8_t   spare11:3;
  int8_t    trimInc;          // Trim Increments
  int8_t    ppmDelay;
  int8_t    trimSw;
  uint8_t   beepANACenter;    // 1<<0->A1.. 1<<6->A7
  uint8_t   pulsePol:1;
  uint8_t   extendedLimits:1;
  uint8_t   swashInvertELE:1;
  uint8_t   swashInvertAIL:1;
  uint8_t   swashInvertCOL:1;
  uint8_t   swashType:3;
  uint8_t   swashCollectiveSource;
  uint8_t   swashRingValue;
  int8_t    ppmFrameLength;   //0=22.5  (10msec-30msec) 0.5msec increments
  MixData   mixData[MAX_MIXERS];
  LimitData limitData[NUM_CHNOUT];
  ExpoData  expoData[4];
  int8_t    trim[4];
  int8_t    curves5[MAX_CURVE5][5];
  int8_t    curves9[MAX_CURVE9][9];
  CSwData   customSw[NUM_CSW];
//  uint8_t   rxnum;
  uint8_t   frSkyVoltThreshold ;
  uint8_t   bt_telemetry;
  uint8_t   numVoice;		// 0-16, rest are Safety switches
  SafetySwData  safetySw[NUM_CHNOUT];
  FrSkyData frsky;
	TimerMode timer[2] ;
	FrSkyAlarmData frskyAlarms ;
// Add 6 bytes for custom telemetry screen
	uint8_t customDisplayIndex[6] ;
}) ModelData;

#define TOTAL_EEPROM_USAGE (sizeof(ModelData)*MAX_MODELS + sizeof(EEGeneral))


extern EEGeneral g_eeGeneral;
extern ModelData g_oldmodel;

PACK(typedef struct t_PhaseData {
  int16_t trim[4];     // -500..500 => trim value, 501 => use trim of phase 0, 502, 503, 504 => use trim of phases 1|2|3|4 instead
  int8_t swtch;       // swtch of phase[0] is not used
  char name[6];
  uint8_t fadeIn:4;
  uint8_t fadeOut:4;
	uint16_t spare ;		// Future expansion
}) PhaseData;

PACK(typedef struct te_MixData {
  uint8_t destCh;            //        1..NUM_CHNOUT
  uint8_t srcRaw;            //
  int8_t  weight;
  int8_t  swtch;
  int8_t curve;             //0=symmetrisch 1=no neg 2=no pos
  uint8_t delayUp;
  uint8_t delayDown;
  uint8_t speedUp;         // Servogeschwindigkeit aus Tabelle (10ms Cycle)
  uint8_t speedDown;       // 0 nichts
  uint8_t carryTrim:1;
  uint8_t mltpx:2;           // multiplex method 0=+ 1=* 2=replace
  uint8_t lateOffset:1;      // Add offset later
  uint8_t mixWarn:2;         // mixer warning
  uint8_t enableFmTrim:1;
  uint8_t differential:1 ;
  int8_t  sOffset;
  uint8_t  res[4];
}) SKYMixData;

PACK(typedef struct te_CSwData { // Custom Switches data
  int8_t  v1; //input
  int8_t  v2; 		//offset
	uint8_t func;
	int8_t andsw;
	uint8_t res ;
}) SKYCSwData;

PACK(typedef struct te_SafetySwData { // Safety Switches data
	union opt
	{
		struct ss
		{	
	    int8_t  swtch ;
			uint8_t mode ;
    	int8_t  val ;
			uint8_t res ;
		} ss ;
		struct vs
		{
  		uint8_t vswtch ;
			uint8_t vmode ; // ON, OFF, BOTH, 15Secs, 30Secs, 60Secs, Varibl
    	uint8_t vval;
			uint8_t vres ;
		} vs ;
	} opt ;
}) SKYSafetySwData;

PACK(typedef struct te_FrSkyChannelData {
  uint8_t   ratio ;               // 0.0 means not used, 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   offset ;              // 
  uint8_t   gain ;                // 
  uint8_t   alarms_value[2] ;     // 0.1V steps EG. 6.6 Volts = 66. 25.1V = 251, etc.
  uint8_t   alarms_level ;
  uint8_t   alarms_greater ;      // 0=LT(<), 1=GT(>)
  uint8_t   type ;                // 0=volts, 1=raw, 2=volts*2, 3=Amps
}) SKYFrSkyChannelData;

PACK(typedef struct te_FrSkyData {
  SKYFrSkyChannelData channels[2];
}) SKYFrSkyData;

PACK(typedef struct te_swVoice {
  uint8_t  swtch ;
	uint8_t mode ; // ON, OFF, BOTH, 15Secs, 30Secs, 60Secs
  uint8_t  val ;
  uint8_t vres ;
}) voiceSwData ;

PACK(typedef struct t_FunctionData { // Function data
  int8_t  swtch ; //input
  uint8_t func ;
  uint8_t param[6] ;
  uint8_t spare1 ;
  uint8_t spare2 ;
}) FunctionData;

PACK(typedef struct t_Vario
{
  uint8_t varioSource ;
  int8_t  swtch ;
  uint8_t sinkTones:1 ;
  uint8_t spare:1 ;
  uint8_t param:6 ;
}) VarioData ;	

PACK(typedef struct t_gvar {
	int8_t gvar ;
	uint8_t gvsource ;
//	int8_t gvswitch ;
}) GvarData ;

PACK(typedef struct te_ModelData {
  char      name[MODEL_NAME_LEN];             // 10 must be first for eeLoadModelName
  uint8_t   modelVoice ;		// Index to model name voice (261+value)
  uint8_t   RxNum ;						// was timer trigger source, now RxNum for model match
  uint8_t   telemetryRxInvert:1 ;	// was tmrDir, now use tmrVal>0 => count down
  uint8_t   traineron:1;  		// 0 disable trainer, 1 allow trainer
  uint8_t   spare15:1 ;
  uint8_t   FrSkyUsrProto:1 ; // Protocol in FrSky User Data, 0=FrSky Hub, 1=WS HowHigh
  uint8_t   FrSkyGpsAlt:1 ;		// Use Gps Altitude as main altitude reading
  uint8_t   FrSkyImperial:1 ; // Convert FrSky values to imperial units
  uint8_t   FrSkyAltAlarm:2;
	uint8_t 	modelVersion ;
  uint8_t   protocol:4 ;
  uint8_t   country:2 ;
  uint8_t   sub_protocol:2 ;
  int8_t    ppmNCH;
  uint8_t   thrTrim:1;            // Enable Throttle Trim
	uint8_t   xnumBlades:2;					// RPM scaling, now elsewhere as uint8_t
	uint8_t   extendedTrims:1;			// Only applies to phases
  uint8_t   thrExpo:1;            // Enable Throttle Expo
	uint8_t   frskyComPort:1;
	uint8_t   spare11:2;
  int8_t    trimInc;          // Trim Increments
  int8_t    ppmDelay;
  int8_t    trimSw;
  uint8_t   beepANACenter;    // 1<<0->A1.. 1<<6->A7
  uint8_t   pulsePol:1;
  uint8_t   extendedLimits:1;
  uint8_t   swashInvertELE:1;
  uint8_t   swashInvertAIL:1;
  uint8_t   swashInvertCOL:1;
  uint8_t   swashType:3;
  uint8_t   swashCollectiveSource;
  uint8_t   swashRingValue;
  int8_t    ppmFrameLength;   //0=22.5  (10msec-30msec) 0.5msec increments
  SKYMixData   mixData[MAX_SKYMIXERS];
  LimitData limitData[NUM_SKYCHNOUT];
  ExpoData  expoData[4];
  int8_t    trim[4];
  int8_t    curves5[MAX_CURVE5][5];
  int8_t    curves9[MAX_CURVE9][9];
  int8_t    curvexy[18];
  SKYCSwData   customSw[NUM_SKYCSW];
//  uint8_t   rxnum;
  uint8_t   frSkyVoltThreshold ;
  uint8_t   bt_telemetry;
  uint8_t   numVoice;		// 0-16, rest are Safety switches
  SKYSafetySwData  safetySw[NUM_SKYCHNOUT];
	voiceSwData	voiceSwitches[NUM_VOICE] ;
  SKYFrSkyData frsky;
	TimerMode timer[2] ;
	FrSkyAlarmData frskyAlarms ;
// Add 6 bytes for custom telemetry screen
	uint8_t		customDisplayIndex[6] ;
  FunctionData   functionData[NUM_FSW];
	PhaseData phaseData[MAX_PHASES] ;
	GvarData	gvars[MAX_GVARS] ;
	uint8_t   numBlades ;					// RPM scaling
	uint8_t		startChannel ;			// for main output 0 = ch1
	uint8_t		startPPM2channel ;	// for PPM2 output 0 follow
  int8_t		ppm2NCH ;
	uint8_t 	sub_trim_limit ;
	uint8_t 	FASoffset ;			// 0.0 to 1.5
	VarioData varioData ;
	uint8_t		anaVolume ;	// analog volume control
	int8_t pxxFailsafe[16] ;
	int8_t logSwitch ;
	uint8_t logRate ;
  // X9D ext module
	uint8_t   xprotocol:4 ;
  uint8_t   xcountry:2 ;
  uint8_t   xsub_protocol:2 ;
  int8_t    xppmNCH ;
  int8_t    xppmDelay ;
  uint8_t   xpulsePol ;
  int8_t    xppmFrameLength;  //0=22.5  (10msec-30msec) 0.5msec increments
	uint8_t		xstartChannel ;		// for output 0 = ch1
	uint8_t		pxxRxNum ;
  int8_t    dsmMode ;
	uint8_t		xPxxRxNum ;				// For external module
  uint16_t  modelswitchWarningStates ;	// Enough bits for Taranis
	int8_t		enRssiOrange:2 ;
	int8_t		rssiOrange:6 ;
	uint8_t		enRssiRed:2 ;
	int8_t		rssiRed:6 ;
//	uint8_t		spare24:6 ;
//	uint8_t   curentSource ;
//	uint8_t   altSource ;
}) SKYModelData;


extern SKYModelData g_model;




#endif
/*eof*/



