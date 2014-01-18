/*
 * Author - Mike Blandford
 *
 * Based on er9x by Erez Raviv <erezraviv@gmail.com>
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

// This version for ARM based ERSKY9X board

#ifndef ersky9x_h
#define ersky9x_h

#include <stdint.h>

#define FIX_MODE		1

#ifdef SIMU
#include "simpgmspace.h"
#else
// Items set to make things compile, will need to be sorted eventually
#define assert(x)
#ifdef PCBSKY
#define wdt_reset()	(WDT->WDT_CR = 0xA5000001)
#endif
#ifdef PCBX9D
#define wdt_reset()	(IWDG->KR = 0x0000AAAAL)
#endif
#endif

#define VERSION	"V0.26"

#define GVARS		1

//#define DATE_STR "xx.xx.2012"
//#define TIME_STR "xx:xx:xx"
//#define SUB_VERS VERSION
//#define SVN_VERS "trunk-rxxx"
//#define MOD_VERS "Normal"

//#define STR2(s) #s
//#define DEFNUMSTR(s)  STR2(s)

extern const char stamp1[] ;
extern const char stamp2[] ;
extern const char stamp3[] ;
extern const char stamp4[] ;
extern const char stamp5[] ;

//extern const char *Str_OFF ;
//extern const char *Str_ON ;
//extern const char *Str_Switches ;
//extern const char *Str_Switch_warn ;

#define CPU_INT		int32_t
#define CPU_UINT	uint32_t

#define BITMASK(bit) (1<<(bit))

extern const char * const *Language ;
extern const char * const English[] ;
extern const char * const French[] ;
extern const char * const German[] ;
extern const char * const Norwegian[] ;
extern const char * const Swedish[] ;

#define PSTR(a) Language[a]
#define XPSTR(a)  (char *)a

#define PROGMEM	 const unsigned char
#define strcpy_P(a,b)	strcpy(a,b)
#define strncpy_P(a,b,c)	strncpy(a,b,c)
#define pgm_read_byte(p)	(*(p))


#ifdef PCBX9D
 #define NUMBER_ANALOG		9
#else
#ifdef REVX
#define NUMBER_ANALOG		10
#define CURRENT_ANALOG	8
#else
 #ifdef REVB
 #define NUMBER_ANALOG		9
 #define CURRENT_ANALOG	8
 #else
 #define NUMBER_ANALOG	8
 #endif
#endif
#endif

#define DIM(arr) (sizeof((arr))/sizeof((arr)[0]))

enum EnumKeys {
    KEY_MENU ,
    KEY_EXIT ,
    KEY_DOWN ,
    KEY_UP  ,
    KEY_RIGHT ,
    KEY_LEFT ,
    TRM_LH_DWN  ,
    TRM_LH_UP   ,
    TRM_LV_DWN  ,
    TRM_LV_UP   ,
    TRM_RV_DWN  ,
    TRM_RV_UP   ,
    TRM_RH_DWN  ,
    TRM_RH_UP   ,
	  BTN_RE,

#ifdef PCBSKY
    //SW_NC     ,
    //SW_ON     ,
    SW_ThrCt  ,
    SW_RuddDR ,
    SW_ElevDR ,
    SW_ID0    ,
    SW_ID1    ,
    SW_ID2    ,
    SW_AileDR ,
    SW_Gear   ,
    SW_Trainer
#endif

#ifdef PCBX9D
  SW_SA0,
  SW_SA1,
  SW_SA2,
  SW_SB0,
  SW_SB1,
  SW_SB2,
  SW_SC0,
  SW_SC1,
  SW_SC2,
  SW_SD0,
  SW_SD1,
  SW_SD2,
  SW_SE0,
  SW_SE1,
  SW_SE2,
  SW_SF0,
  SW_SF2,
  SW_SG0,
  SW_SG1,
  SW_SG2,
  SW_SH0,
  SW_SH2
#endif
};

// c17-c24 added for timer mode A display
//#define CURV_STR "\003---x>0x<0|x|f>0f<0|f|c1 c2 c3 c4 c5 c6 c7 c8 c9 c10c11c12c13c14c15c16c17c18c19c20c21c22c23c24"
#define CURVE_BASE 7

//#define CSWITCH_STR  "----   v>ofs  v<ofs  |v|>ofs|v|<ofsAND    OR     XOR    ""v1==v2 ""v1!=v2 ""v1>v2  ""v1<v2  ""v1>=v2 ""v1<=v2 TimeOff"
#define CSW_LEN_FUNC 7


#define CS_OFF       0
#define CS_VPOS      1  //v>offset
#define CS_VNEG      2  //v<offset
#define CS_APOS      3  //|v|>offset
#define CS_ANEG      4  //|v|<offset
#define CS_AND       5
#define CS_OR        6
#define CS_XOR       7
#define CS_EQUAL     8
#define CS_NEQUAL    9
#define CS_GREATER   10
#define CS_LESS      11
#define CS_EGREATER  12
#define CS_ELESS     13
#define CS_TIME	     14
#define CS_MAXF      14  //max function

#define CS_VOFS       0
#define CS_VBOOL      1
#define CS_VCOMP      2
#define CS_TIMER			3
#define CS_STATE(x)   ((x)<CS_AND ? CS_VOFS : ((x)<CS_EQUAL ? CS_VBOOL : ((x)<CS_TIME ? CS_VCOMP : CS_TIMER)))

#ifdef PCBSKY
#define SW_BASE      SW_ThrCt
#define SW_BASE_DIAG SW_ThrCt
#define MAX_PSWITCH   (SW_Trainer-SW_ThrCt+1)  // 9 physical switches
#define MAX_DRSWITCH (1+SW_Trainer-SW_ThrCt+1+NUM_CSW)
#define MAX_SKYDRSWITCH (1+SW_Trainer-SW_ThrCt+1+NUM_SKYCSW)
#endif

#ifdef PCBX9D
#define SW_BASE      SW_SA0
#define SW_BASE_DIAG SW_SA0
#define MAX_PSWITCH   (SW_SH2-SW_SA0+1)  // 9 physical switches
#define MAX_DRSWITCH (1+SW_SH2-SW_SA0+1+NUM_CSW)
#define MAX_SKYDRSWITCH (1+SW_SH2-SW_SA0+1+NUM_SKYCSW)
#endif


#define SWP_RUD (SW_RuddDR-SW_BASE)
#define SWP_ELE (SW_ElevDR-SW_BASE)
#define SWP_AIL (SW_AileDR-SW_BASE)
#define SWP_THR (SW_ThrCt-SW_BASE)
#define SWP_GEA (SW_Gear-SW_BASE)

#define SWP_RUDB (1<<SWP_RUD)
#define SWP_ELEB (1<<SWP_ELE)
#define SWP_AILB (1<<SWP_AIL)
#define SWP_THRB (1<<SWP_THR)
#define SWP_GEAB (1<<SWP_GEA)

#define SWP_ID0 (SW_ID0-SW_BASE)
#define SWP_ID1 (SW_ID1-SW_BASE)
#define SWP_ID2 (SW_ID2-SW_BASE)
#define SWP_ID0B (1<<SWP_ID0)
#define SWP_ID1B (1<<SWP_ID1)
#define SWP_ID2B (1<<SWP_ID2)

//Switch Position Illigal states
#define SWP_IL1 (0)
#define SWP_IL2 (SWP_ID0B | SWP_ID1B)
#define SWP_IL3 (SWP_ID0B | SWP_ID2B)
#define SWP_IL4 (SWP_ID1B | SWP_ID2B)
#define SWP_IL5 (SWP_ID0B | SWP_ID1B | SWP_ID2B)

//#define SWITCHES_STR "THRRUDELEID0ID1ID2AILGEATRNSW1SW2SW3SW4SW5SW6SW7SW8SW9SWASWBSWCSWDSWESWFSWGSWHSWISWJSWKSWLSWMSWNSWO"
#define NUM_CSW  12 //number of custom switches
#define NUM_SKYCSW  24 //number of custom switches
#ifdef PCBSKY
#define CSW_INDEX	9	// Index of first custom switch
#endif
#ifdef PCBX9D
#define CSW_INDEX	22	// Index of first custom switch
#endif


#define NUM_KEYS BTN_RE+1
#define TRM_BASE TRM_LH_DWN

#define _MSK_KEY_REPT    0x40
#define _MSK_KEY_DBL     0x10
#define IS_KEY_BREAK(key)  (((key)&0xf0)        ==  0x20)
#define EVT_KEY_BREAK(key) ((key)|                  0x20)
#define EVT_KEY_FIRST(key) ((key)|    _MSK_KEY_REPT|0x20)
#define EVT_KEY_REPT(key)  ((key)|    _MSK_KEY_REPT     )
#define EVT_KEY_LONG(key)  ((key)|0x80)
#define EVT_KEY_DBL(key)   ((key)|_MSK_KEY_DBL)
//#define EVT_KEY_DBL(key)   ((key)|0x10)
#define EVT_ENTRY               (0xff - _MSK_KEY_REPT)
#define EVT_ENTRY_UP            (0xfe - _MSK_KEY_REPT)
#define EVT_KEY_MASK             0x0f

#define HEART_TIMER_PULSES 1 ;
#define HEART_TIMER10ms 2;


#define INP_D_TRM_LH_UP   7
#define INP_D_TRM_LH_DWN  6
#define INP_D_TRM_RV_DWN  5
#define INP_D_TRM_RV_UP   4
#define INP_D_TRM_LV_DWN  3
#define INP_D_TRM_LV_UP   2
#define INP_D_TRM_RH_DWN  1
#define INP_D_TRM_RH_UP   0

#define RESX    (1<<10) // 1024
#define RESXu   1024u
#define RESXul  1024ul
#define RESXl   1024l
#define RESKul  100ul
#define RESX_PLUS_TRIM (RESX+128)

#define TRIM_EXTENDED_MAX	500

#define NUM_PPM     8
//number of real outputchannels CH1-CH16
#define NUM_CHNOUT  16
#define NUM_SKYCHNOUT  24
///number of real input channels (1-9) plus virtual input channels X1-X4
#define PPM_BASE    MIX_CYC3
#define CHOUT_BASE  (PPM_BASE+NUM_PPM)

#define NUM_FSW			16

#define NUM_STICKS	4

//#define SWASH_TYPE_STR   "---   ""120   ""120X  ""140   ""90    "

#define SWASH_TYPE_120   1
#define SWASH_TYPE_120X  2
#define SWASH_TYPE_140   3
#define SWASH_TYPE_90    4
#define SWASH_TYPE_NUM   4


#define MIX_P1    5
#define MIX_P2    6
#ifdef PCBSKY
#define MIX_P3    7
#define MIX_MAX   8
#define MIX_FULL  9
#define MIX_CYC1  10
#define MIX_CYC2  11
#define MIX_CYC3  12
#endif
#ifdef PCBX9D
#define MIX_P3    7
#define MIX_P4    8
#define MIX_MAX   9
#define MIX_FULL  10
#define MIX_CYC1  11
#define MIX_CYC2  12
#define MIX_CYC3  13
#endif


#define DR_HIGH   0
#define DR_MID    1
#define DR_LOW    2
#define DR_EXPO   0
#define DR_WEIGHT 1
#define DR_RIGHT  0
#define DR_LEFT   1
#define DR_BOTH   2
#define DR_DRSW1  99
#define DR_DRSW2  98

#define DSW_THR  1
#define DSW_RUD  2
#define DSW_ELE  3
#define DSW_ID0  4
#define DSW_ID1  5
#define DSW_ID2  6
#define DSW_AIL  7
#define DSW_GEA  8
#define DSW_TRN  9
#define DSW_SW1  10
#define DSW_SW2  11
#define DSW_SW3  12
#define DSW_SW4  13
#define DSW_SW5  14
#define DSW_SW6  15
#define DSW_SW7   16
#define DSW_SW8   17
#define DSW_SW9   18
#define DSW_SWA   19
#define DSW_SWB   20
#define DSW_SWC   21

#define SCROLL_TH 64
#define INACTIVITY_THRESHOLD 256
#define THRCHK_DEADBAND 16
#define SPLASH_TIMEOUT  (4*100)  //400 msec - 4 seconds

uint8_t IS_THROTTLE( uint8_t x ) ;
uint8_t IS_EXPO_THROTTLE( uint8_t x ) ;

extern uint8_t Ee_lock ;

// Bit masks in Ee_lock
#define EE_LOCK      1
#define EE_TRIM_LOCK 2

#define EE_GENERAL 1
#define EE_MODEL   2
#define EE_TRIM    4           // Store model because of trim
#define INCDEC_SWITCH   0x08


#define TMR_VAROFS  4

#define TMRMODE_NONE     0
#define TMRMODE_ABS      1
#define TMRMODE_THR      2
#define TMRMODE_THR_REL  3
#define MAX_ALERT_TIME   60

#define PROTO_PPM        0
#define PROTO_PXX        1
#define PROTO_DSM2       2
#define PROT_MAX         2
#define PROTO_PPM16			 3		// No longer needed
#define PROT_STR_LEN     6
#define DSM2_STR "\011LP4/LP5  DSM2only DSM2/DSMX9XR-DSM  "
#define DSM2_STR_LEN   9
#define LPXDSM2          0
#define DSM2only         1
#define DSM2_DSMX        2
#define DSM_9XR		       3

// PXX_SEND_RXNUM == BIND
#define PXX_BIND			     0x01
#define PXX_SEND_FAILSAFE  0x10
#define PXX_RANGE_CHECK		 0x20

#define POWER_OFF			0
#define POWER_ON			1
#define POWER_TRAINER	2


extern uint8_t pxxFlag;
extern uint8_t stickMoved;

#define FLASH_DURATION 50

extern uint16_t g_LightOffCounter;


template<class t> inline t min(t a, t b){ return a<b?a:b; }
template<class t> inline t max(t a, t b){ return a>b?a:b; }
template<class t> inline t limit(t mi, t x, t ma){ return min(max(mi,x),ma); }

// This doesn't need protection on this processor
#define get_tmr10ms() g_tmr10ms

#define sysFLAG_OLD_EEPROM (0x01)
extern uint8_t sysFlags;
extern uint8_t StickScrollAllowed ;

const char s_charTab[]=" ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-.";
#define NUMCHARS (sizeof(s_charTab)-1)

///number of real input channels (1-9) plus virtual input channels X1-X4
#define PPM_BASE    MIX_CYC3
#define CHOUT_BASE  (PPM_BASE+NUM_PPM)

//extern const char *Str_telemItems ;
extern const int8_t TelemIndex[] ;
extern int16_t convertTelemConstant( int8_t channel, int8_t value) ;
#define NUM_TELEM_ITEMS 35

#define NUM_XCHNRAW (CHOUT_BASE+NUM_CHNOUT) // NUMCH + P1P2P3+ AIL/RUD/ELE/THR + MAX/FULL + CYC1/CYC2/CYC3
#define NUM_SKYXCHNRAW (CHOUT_BASE+NUM_SKYCHNOUT) // NUMCH + P1P2P3+ AIL/RUD/ELE/THR + MAX/FULL + CYC1/CYC2/CYC3
///number of real output channels (CH1-CH8) plus virtual output channels X1-X4
#define NUM_XCHNOUT (NUM_CHNOUT) //(NUM_CHNOUT)//+NUM_VIRT)
#define NUM_SKYXCHNOUT (NUM_SKYCHNOUT) //(NUM_CHNOUT)//+NUM_VIRT)

#define MIX_3POS	(NUM_SKYXCHNRAW+1)

inline int32_t calc100toRESX(register int8_t x)
{
  return ((uint32_t)x*655)>>6 ;
}

inline int16_t calc1000toRESX( register int32_t x)  // improve calc time by Pat MacKenzie
{
    register int32_t y = x>>5;
    x+=y;
    y=y>>2;
    x-=y;
    return x+(y>>2);
    //  return x + x/32 - x/128 + x/512;
}

extern uint32_t Current_used ;
extern uint16_t Current_max ;
extern uint16_t MAh_used ;
extern uint16_t Run_time ;

#ifdef FIX_MODE
extern const char stickScramble[] ;
//extern const char modeFix[] ;
uint8_t modeFixValue( uint8_t value ) ;
#else
extern const char modn12x3[] ;
#endif

extern const uint8_t bchout_ar[] ;

//convert from mode 1 to mode g_eeGeneral.stickMode
//NOTICE!  =>  1..4 -> 1..4
extern uint8_t convert_mode_helper(uint8_t x) ;

#define CONVERT_MODE(x)  (((x)<=4) ? convert_mode_helper(x) : (x))
//#define CHANNEL_ORDER(x) (chout_ar[g_eeGeneral.templateSetup*4 + (x)-1])
#define CHANNEL_ORDER(x) ( ( (bchout_ar[g_eeGeneral.templateSetup] >> (6-(x-1) * 2)) & 3 ) + 1 )
#define THR_STICK       (2-(g_eeGeneral.stickMode&1))
#define ELE_STICK       (1+(g_eeGeneral.stickMode&1))
#define AIL_STICK       ((g_eeGeneral.stickMode&2) ? 0 : 3)
#define RUD_STICK       ((g_eeGeneral.stickMode&2) ? 3 : 0)

#define STORE_MODELVARS_TRIM   eeDirty(EE_MODEL|EE_TRIM)
#define STORE_MODELVARS   eeDirty(EE_MODEL)
#define STORE_GENERALVARS eeDirty(EE_GENERAL)

#ifdef PCBSKY
#define BACKLIGHT_ON    (PWM->PWM_CH_NUM[0].PWM_CDTY = g_eeGeneral.bright)
#define BACKLIGHT_OFF   (PWM->PWM_CH_NUM[0].PWM_CDTY = 100)
#endif

#ifdef PCBX9D
extern void backlight_on( void ) ;
extern void backlight_off( void ) ;
#define BACKLIGHT_ON        backlight_on()
#define BACKLIGHT_OFF       backlight_off()
#endif

// Options for mainSequence()
#define NO_MENU		1
#define MENUS			0

extern uint16_t evalChkSum( void ) ;

struct t_calib
{
	int16_t midVals[8];
	int16_t loVals[8];
	int16_t hiVals[8];
	uint8_t idxState;
} ;

union t_xmem
{
//	struct MixTab s_mixTab[MAX_MIXERS+NUM_XCHNOUT+1] ;	
	struct t_calib Cal_data ;
#ifndef CPUARM
	char buf[sizeof(g_model.name)+5];
#endif
//#if defined(CPUM128) || defined(CPUM2561)
//  uint8_t file_buffer[256];
//#else
//  uint8_t file_buffer[128];
//#endif
} ;

extern union t_xmem Xmem ;


typedef void (*MenuFuncP)(uint8_t event);

typedef struct
{ 
	unsigned char second;   //enter the current time, date, month, and year
	unsigned char minute;
	unsigned char hour;                                     
	unsigned char date;       
	unsigned char month;
	unsigned int year;      
 } t_time ;

extern t_time Time ;

extern bool    checkIncDec_Ret;//global helper vars
extern uint8_t s_editMode;     //global editmode

int16_t checkIncDec16(uint8_t event, int16_t i_pval, int16_t i_min, int16_t i_max, uint8_t i_flags);
int8_t checkIncDec(uint8_t event, int8_t i_val, int8_t i_min, int8_t i_max, uint8_t i_flags);
int8_t checkIncDec_hm(uint8_t event, int8_t i_val, int8_t i_min, int8_t i_max);
int8_t checkIncDec_vm(uint8_t event, int8_t i_val, int8_t i_min, int8_t i_max);
int8_t checkIncDec_hg(uint8_t event, int8_t i_val, int8_t i_min, int8_t i_max);

#define CHECK_INCDEC_H_GENVAR( event, var, min, max)     \
    var = checkIncDec_hg(event,var,min,max)

#define CHECK_INCDEC_H_MODELVAR( event, var, min, max)     \
    var = checkIncDec_hm(event,var,min,max)

#define CHECK_INCDEC_MODELSWITCH(event, var, min, max) \
  var = checkIncDec(event,var,min,max,EE_MODEL|INCDEC_SWITCH)

#define CHECK_INCDEC_GENERALSWITCH( event, var, min, max) \
  var = checkIncDec(event,var,min,max,EE_GENERAL|INCDEC_SWITCH)


extern uint8_t heartbeat ;
extern int16_t g_chans512[NUM_SKYCHNOUT];
//extern uint8_t eeprom[4096] ;

uint8_t char2idx(char c);
char idx2char(uint8_t idx);

//extern volatile uint32_t Timer2_count ;		// Modified in interrupt routine
extern volatile uint16_t g_tmr10ms ;
extern volatile uint8_t  g_blinkTmr10ms;
extern volatile uint8_t tick10ms ;
extern uint32_t Master_frequency ;

extern void alert(const char * s, bool defaults=false);
extern void message(const char * s);

void resetTimer();


//extern uint8_t Timer2_running ;
//extern uint16_t Timer2 ;
//void resetTimer2( void ) ;

extern void putsTime(uint8_t x,uint8_t y,int16_t tme,uint8_t att,uint8_t att2) ;
extern void putsVolts(uint8_t x,uint8_t y, uint8_t volts, uint8_t att) ;
extern void putsVBat(uint8_t x,uint8_t y,uint8_t att) ;
extern void putsVBat(uint8_t x,uint8_t y,uint8_t att) ;
extern void putsChnRaw(uint8_t x,uint8_t y,uint8_t idx,uint8_t att) ;
extern void putsChn(uint8_t x,uint8_t y,uint8_t idx1,uint8_t att) ;
extern void putsDrSwitches(uint8_t x,uint8_t y,int8_t idx1,uint8_t att) ; //, bool nc) ;
extern void putsTmrMode(uint8_t x, uint8_t y, uint8_t attr, uint8_t timer, uint8_t type ) ;
extern const char *get_switches_string( void ) ;

extern void interrupt5ms() ;


extern int16_t intpol(int16_t x, uint8_t idx);

extern uint16_t anaIn(uint8_t chan) ;

extern int8_t phyStick[4] ;
extern int16_t ex_chans[NUM_SKYCHNOUT];

extern void modelDefault( uint8_t id ) ;
extern uint8_t VoiceCheckFlag ;

//void eeWaitComplete( void ) ;
void eeDirty(uint8_t msk);
//void eeCheck(bool immediately=false ) ;
void eeReadAll( void ) ;
//void eeLoadModelName(uint8_t id,char*buf,uint8_t len);
//uint16_t eeFileSize(uint8_t id);
//void eeLoadModel(uint8_t id);
//void eeSaveModel(uint8_t id);
bool eeDuplicateModel(uint8_t id);
//bool eeModelExists(uint8_t id);

extern char idx2char(uint8_t idx) ;
extern uint8_t char2idx(char c) ;

extern int16_t            g_ppmIns[];
extern uint8_t ppmInState ; //0=unsync 1..8= wait for value i-1
extern uint8_t ppmInValid ;

/// goto given Menu, but substitute current menu in menuStack
extern void    chainMenu(MenuFuncP newMenu);
/// goto given Menu, store current menu in menuStack
extern void    pushMenu(MenuFuncP newMenu);
///deliver address of last menu which was popped from
extern MenuFuncP lastPopMenu();
/// return to last menu in menustack
/// if uppermost is set true, thenmenu return to uppermost menu in menustack
void    popMenu(bool uppermost=false);

#define NO_TRAINER 0x01
#define NO_INPUT   0x02

// Timeout, in seconds, stick scroll remains active
#define STICK_SCROLL_TIMEOUT		5

extern bool getSwitch(int8_t swtch, bool nc, uint8_t level = 0 ) ;
extern int8_t getMovedSwitch( void ) ;
extern uint8_t g_vbat100mV ;
//extern uint16_t Timer2 ;
extern void doSplash( void ) ;
extern void mainSequence( uint32_t no_menu ) ;
#ifdef FRSKY
extern uint8_t putsTelemValue(uint8_t x, uint8_t y, uint8_t val, uint8_t channel, uint8_t att, uint8_t scale) ;
extern void telem_byte_to_bt( uint8_t data ) ;
extern uint16_t scale_telem_value( uint16_t val, uint8_t channel, uint8_t times2, uint8_t *p_att ) ;
#endif
uint8_t telemItemValid( uint8_t index ) ;

extern int16_t get_telemetry_value( int8_t channel ) ;

extern void putVoiceQueue( uint16_t value ) ;
void voice_numeric( int16_t value, uint8_t num_decimals, uint8_t units_index ) ;
extern void voice_telem_item( int8_t index ) ;
extern uint8_t *cpystr( uint8_t *dest, uint8_t *source ) ;
extern int16_t m_to_ft( int16_t metres ) ;

uint8_t getCurrentSwitchStates( void ) ;

extern uint32_t check_soft_power( void ) ;
extern uint32_t getFlightPhase( void ) ; 
extern int16_t getRawTrimValue( uint8_t phase, uint8_t idx ) ;
extern int16_t getTrimValue( uint8_t phase, uint8_t idx ) ;
extern void setTrimValue(uint8_t phase, uint8_t idx, int16_t trim) ;

extern void checkSwitches( void ) ;
extern void setLanguage( void ) ;

struct t_timer
{
	uint16_t s_sum ;
	uint8_t lastSwPos ;
	uint8_t sw_toggled ;
	uint16_t s_timeCumSw ;  //laufzeit in 1/16 sec
	uint8_t  s_timerState ;
	uint16_t s_timeCumThr ;  //gewichtete laufzeit in 1/16 sec
	uint16_t s_timeCum16ThrP ; //gewichtete laufzeit in 1/16 sec
	int16_t  s_timerVal ;
} ;

extern struct t_timer s_timer[] ;

// Rotary encoder movement states
#define	ROTARY_MENU_LR		0
#define	ROTARY_MENU_UD		1
#define	ROTARY_SUBMENU_LR	2
#define	ROTARY_VALUE			3

extern uint8_t RotaryState ;		// Defaults to ROTARY_MENU_LR

#if GVARS
extern int8_t REG(int8_t x, int8_t min, int8_t max) ;
#endif

enum PowerState
{
  e_power_on,
  e_power_trainer,
  e_power_usb,
  e_power_off
} ;

struct t_p1
{
	int16_t p1val ;
	int16_t p1valdiff ;
  int16_t p1valprev ;
	int16_t p2valprev ;
	int16_t p3valprev ;
} ;

extern struct t_p1 P1values ;

#ifdef PCBSKY
extern uint16_t ResetReason ;
#endif
#ifdef PCBX9D
extern uint32_t ResetReason ;
#endif
extern uint8_t unexpectedShutdown ;
extern uint8_t SdMounted ;

#ifdef PCBX9D
#include "stm32f2xx.h"
#include "rtc.h"
#include "stm32f2xx_rtc.h"
void rtcSetTime( t_time *t ) ;
void rtc_gettime( t_time *t ) ;
void rtcInit( void ) ;
#endif

extern void setVolume( uint8_t value ) ;
extern uint8_t HoldVolume ;

#ifdef PCBX9D
#define INTERNAL_MODULE 0
#define EXTERNAL_MODULE 1
#define TRAINER_MODULE  2
#endif

#endif
