/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
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

#include "open9x.h"

#if defined(CPUARM)
#define MIXER_STACK_SIZE    500
#define MENUS_STACK_SIZE    1000
#define AUDIO_STACK_SIZE    500
#define BT_STACK_SIZE       500
#define DEBUG_STACK_SIZE    500

OS_TID mixerTaskId;
OS_STK mixerStack[MIXER_STACK_SIZE];

OS_TID menusTaskId;
OS_STK menusStack[MENUS_STACK_SIZE];

OS_TID audioTaskId;
OS_STK audioStack[AUDIO_STACK_SIZE];

#if defined(BLUETOOTH)
OS_TID btTaskId;
OS_STK btStack[BT_STACK_SIZE];
#endif

#if defined(PCBSKY9X) && defined(DEBUG)
OS_TID debugTaskId;
OS_STK debugStack[DEBUG_STACK_SIZE];
#endif

OS_TCID audioTimer;
OS_FlagID audioFlag;

OS_MutexID audioMutex;
OS_MutexID mixerMutex;

char modelNames[MAX_MODELS][sizeof(g_model.name)];

#endif // defined(CPUARM)

#if defined(SPLASH)
const pm_uchar splashdata[] PROGMEM = { 'S','P','S',0,
#if defined(PCBACT)
#include "splash_ACT.lbm"
#elif defined(PCBX9D)
#include "splash_X9D.lbm"
#else
#include "splash_9x.lbm"
#endif
	'S','P','E',0};
const pm_uchar * splash_lbm = splashdata+4;
#endif

#if !defined(CPUM64) || defined(EXTSTD)
const pm_uchar asterisk_lbm[] PROGMEM = {
#include "asterisk.lbm"
};
#endif

#include "menus.h"

EEGeneral  g_eeGeneral;
ModelData  g_model;

#if defined(PCBX9D) && defined(SDCARD)
uint8_t modelBitmap[MODEL_BITMAP_SIZE];
void loadModelBitmap()
{
  char lfn[] = BITMAPS_PATH "/xxxxxxxxxx.bmp";
  strncpy(lfn+sizeof(BITMAPS_PATH), g_model.bitmap, sizeof(g_model.bitmap));
  lfn[sizeof(BITMAPS_PATH)+sizeof(g_model.bitmap)] = '\0';
  strcat(lfn+sizeof(BITMAPS_PATH), BITMAPS_EXT);
  bmpLoad(modelBitmap, lfn, MODEL_BITMAP_WIDTH, MODEL_BITMAP_HEIGHT);
}
#endif

#if !defined(CPUARM)
uint8_t g_tmr1Latency_max;
uint8_t g_tmr1Latency_min;
#endif

uint8_t unexpectedShutdown = 0;

/* mixer duration in 1/16ms */
uint16_t maxMixerDuration;
uint16_t lastMixerDuration;

#if defined(AUDIO) && !defined(CPUARM)
audioQueue  audio;
#endif

#if defined(DSM2)
// TODO port to ARM Bryan's code
bool s_bind_mode = false;
bool s_rangecheck_mode = false;
uint8_t s_bind_allowed = 255;
#endif

uint8_t heartbeat;

uint8_t stickMode;

int8_t safetyCh[NUM_CHNOUT];

union ReusableBuffer reusableBuffer;

const pm_char s_charTab[] PROGMEM = "_-.,";

const pm_uint8_t bchout_ar[] PROGMEM = {
    0x1B, 0x1E, 0x27, 0x2D, 0x36, 0x39,
    0x4B, 0x4E, 0x63, 0x6C, 0x72, 0x78,
    0x87, 0x8D, 0x93, 0x9C, 0xB1, 0xB4,
    0xC6, 0xC9, 0xD2, 0xD8, 0xE1, 0xE4 };

uint8_t channel_order(uint8_t x)
{
  return ( ((pgm_read_byte(bchout_ar + g_eeGeneral.templateSetup) >> (6-(x-1) * 2)) & 3 ) + 1 );
}

/*
mode1 rud ele thr ail
mode2 rud thr ele ail
mode3 ail ele thr rud
mode4 ail thr ele rud
*/
const pm_uint8_t modn12x3[] PROGMEM = {
    1, 2, 3, 4,
    1, 3, 2, 4,
    4, 2, 3, 1,
    4, 3, 2, 1 };

char idx2char(int8_t idx)
{
  if (idx == 0) return ' ';
  if (idx < 0) {
    if (idx > -27) return 'a' - idx - 1;
    idx = -idx;
  }
  if (idx < 27) return 'A' + idx - 1;
  if (idx < 37) return '0' + idx - 27;
  if (idx <= 40) return pgm_read_byte(s_charTab+idx-37);
  if (idx <= ZCHAR_MAX) return 'z' + 5 + idx - 40;
  return ' ';
}

#if defined(CPUARM)
uint8_t zlen(const char *str, uint8_t size)
{
  while (size > 0) {
    if (str[size-1] != 0)
      return size;
    size--;
  }
  return size;
}

char * strcat_zchar(char * dest, char * name, uint8_t size, const char *defaultName, uint8_t defaultNameSize, uint8_t defaultIdx)
{
  int8_t len = 0;

  if (name) {
    memcpy(dest, name, size);
    dest[size] = '\0';

    int8_t i = size-1;

    while (i>=0) {
      if (!len && dest[i])
        len = i+1;
      if (len) {
        if (dest[i])
          dest[i] = idx2char(dest[i]);
        else
          dest[i] = '_';
      }
      i--;
    }
  }

  if (len == 0 && defaultName) {
    strcpy(dest, defaultName);
    dest[defaultNameSize] = (char)((defaultIdx / 10) + '0');
    dest[defaultNameSize + 1] = (char)((defaultIdx % 10) + '0');
    len = defaultNameSize + 2;
  }

  return &dest[len];
}
#endif

volatile tmr10ms_t g_tmr10ms;

#if defined(CPUARM)
volatile uint8_t rtc_count=0;
#endif

void per10ms()
{
  g_tmr10ms++;

#if defined(CPUARM)
  Tenms |= 1 ;                    // 10 mS has passed
#endif

  if (lightOffCounter) lightOffCounter--;
  if (s_noHi) s_noHi--;
  if (trimsCheckTimer) trimsCheckTimer --;
  if ((g_blinkTmr10ms & 0x1F) == 0) {
    inacCounter++;
    if ((((uint8_t)inacCounter)&0x0F)==0x01 && g_eeGeneral.inactivityTimer && g_vbat100mV>50 && inacCounter > ((uint16_t)g_eeGeneral.inactivityTimer*187))
      AUDIO_INACTIVITY();
  }

#if defined(RTCLOCK)
  /* Update global Date/Time every 100 per10ms cycles */
  if (++g_ms100 == 100) {
    g_rtcTime++;   // inc global unix timestamp one second
#if defined(PCBSKY9X)
    if (g_rtcTime < 60 || rtc_count<5) {
      rtcInit();
      rtc_count++;
    }
    else {
      coprocReadData(true);
    }
#endif
    g_ms100 = 0;
  }
#endif

  readKeysAndTrims();

#if defined(ROTARY_ENCODER_NAVIGATION)
  if (IS_RE_NAVIGATION_ENABLE()) {
    static rotenc_t rePreviousValue;
    rotenc_t reNewValue = (g_rotenc[NAVIGATION_RE_IDX()] / ROTARY_ENCODER_GRANULARITY);
    int8_t scrollRE = reNewValue - rePreviousValue;
    if (scrollRE) {
      rePreviousValue = reNewValue;
      putEvent(scrollRE < 0 ? EVT_ROTARY_LEFT : EVT_ROTARY_RIGHT);
    }
    extern uint8_t s_evt;
    uint8_t evt = s_evt;
    if (EVT_KEY_MASK(evt) == BTN_REa + NAVIGATION_RE_IDX()) {
      if (IS_KEY_BREAK(evt)) {
        putEvent(EVT_ROTARY_BREAK);
      }
      else if (IS_KEY_LONG(evt)) {
        putEvent(EVT_ROTARY_LONG);
      }
    }
  }
#endif

#if (defined(FRSKY) || defined(MAVLINK) || defined(JETI)) && !defined(CPUARM) && !(defined(PCBSTD) && (defined(AUDIO) || defined(VOICE)))
  if (!IS_DSM2_SERIAL_PROTOCOL(s_current_protocol))
    telemetryPoll10ms();
#endif

  // These moved here from perOut() to improve beep trigger reliability.
#if defined(PWM_BACKLIGHT)
  if ((g_tmr10ms&0x03) == 0x00)  //  if (g_tmr10ms % 4 == 0) // (should be faster)
    fadeBacklight(); // increment or decrement brightness until target brightness is reached
#endif

  if(mixWarning & 1) if(((g_tmr10ms&0xFF)==  0)) AUDIO_MIX_WARNING_1();
  if(mixWarning & 2) if(((g_tmr10ms&0xFF)== 64) || ((g_tmr10ms&0xFF)== 72)) AUDIO_MIX_WARNING_2();
  if(mixWarning & 4) if(((g_tmr10ms&0xFF)==128) || ((g_tmr10ms&0xFF)==136) || ((g_tmr10ms&0xFF)==144)) AUDIO_MIX_WARNING_3();

#if defined(SDCARD)
  sdPoll10ms();
#endif

  heartbeat |= HEART_TIMER10ms;
}

PhaseData *phaseaddress(uint8_t idx)
{
  return &g_model.phaseData[idx];
}

ExpoData *expoaddress(uint8_t idx )
{
  return &g_model.expoData[idx];
}

MixData *mixaddress(uint8_t idx)
{
  return &g_model.mixData[idx];
}

LimitData *limitaddress(uint8_t idx)
{
  return &g_model.limitData[idx];
}

int8_t *curveaddress(uint8_t idx)
{
  return &g_model.points[idx==0 ? 0 : 5*idx+g_model.curves[idx-1]];
}

CurveInfo curveinfo(uint8_t idx)
{
  CurveInfo result;
  result.crv = curveaddress(idx);
  int8_t *next = curveaddress(idx+1);
  uint8_t size = next - result.crv;
  if ((size & 1)==0) {  //  if (size % 2 == 0)  {
	result.points = (size / 2) + 1; // result.points = (size >> 1) + 1; done by the compiler anyway 
    result.custom = true;
  }
  else {
    result.points = size;
    result.custom = false;
  }
  return result;
}

CustomSwData *cswaddress(uint8_t idx)
{
  return &g_model.customSw[idx];
}

#if defined(CPUM64)
void memclear(void *ptr, uint8_t size)
{
  memset(ptr, 0, size);
}
#endif

void generalDefault()
{
  memclear(&g_eeGeneral, sizeof(g_eeGeneral));
  g_eeGeneral.version  = EEPROM_VER;
  g_eeGeneral.variant = EEPROM_VARIANT;
  g_eeGeneral.contrast = 25;
  g_eeGeneral.vBatWarn = 90;

  for (int i = 0; i < NUM_STICKS+NUM_POTS; ++i) {
    g_eeGeneral.calibMid[i]     = 0x200;
    g_eeGeneral.calibSpanNeg[i] = 0x180;
    g_eeGeneral.calibSpanPos[i] = 0x180;
  }
  g_eeGeneral.chkSum = 0x200 * (NUM_STICKS+NUM_POTS) + 0x180 * 5;
}

uint16_t evalChkSum()
{
  uint16_t sum=0;
  for (int i=0; i<NUM_STICKS+NUM_POTS+5;i++)
    sum += g_eeGeneral.calibMid[i];
  return sum;
}

#ifndef TEMPLATES
inline void applyDefaultTemplate()
{
  for (int i=0; i<NUM_STICKS; i++) {
    MixData *md = mixaddress(i);
    md->destCh = i;
    md->weight = 100;
    md->srcRaw = channel_order(i+1);
  }

  STORE_MODELVARS;
}
#endif

void modelDefault(uint8_t id)
{
  memset(&g_model, 0, sizeof(g_model));
  applyDefaultTemplate();
}

int16_t intpol(int16_t x, uint8_t idx) // -100, -75, -50, -25, 0, 25 ,50, 75, 100
{
  CurveInfo crv = curveinfo(idx);
  int16_t erg = 0;

  x+=RESXu;
  if (x < 0) {
    erg = (int16_t)crv.crv[0];
  }
  else if (x >= (RESX*2)) {
    erg = (int16_t)crv.crv[crv.points-1];
  }
  else {
    uint16_t a=0, b=0;
    uint8_t i;
    if (crv.custom) {
      for (i=0; i<crv.points-1; i++) {
        a = b;
        b = (i==crv.points-2 ? 2*RESX : RESX + calc100toRESX(crv.crv[crv.points+i]));
        if ((uint16_t)x<=b) break;
      }
    }
    else {
      uint16_t d = (RESX * 2) / (crv.points-1);
      i = (uint16_t)x / d;
      a = i * d;
      b = a + d;
    }
    erg = (int16_t)crv.crv[i] + ((int32_t)(x-a) * (crv.crv[i+1]-crv.crv[i])) / (b-a);
  }

  return calc100toRESX(erg);
}

#if defined(CURVES)
int16_t applyCurve(int16_t x, int8_t idx)
{
  /* already tried to have only one return at the end */
  switch(idx) {
    case CURVE_NONE:
      return x;
    case CURVE_X_GT0:
      if (x < 0) x = 0; //x|x>0
      return x;
    case CURVE_X_LT0:
      if (x > 0) x = 0; //x|x<0
      return x;
    case CURVE_ABS_X: // x|abs(x)
      return abs(x);
    case CURVE_F_GT0: //f|f>0
      return x > 0 ? RESX : 0;
    case CURVE_F_LT0: //f|f<0
      return x < 0 ? -RESX : 0;
    case CURVE_ABS_F: //f|abs(f)
      return x > 0 ? RESX : -RESX;
  }
  if (idx < 0) {
    x = -x;
    idx = -idx + CURVE_BASE - 1;
  }
  return intpol(x, idx - CURVE_BASE);
}
#else
#define applyCurve(x, idx) (x)
#endif

// maybe used in future?
uint16_t divu100(uint32_t A) {
    return (((A * 0x47AF) >> 16) + A) >> 7;
}

// expo-funktion:
// ---------------
// kmplot
// f(x,k)=exp(ln(x)*k/10) ;P[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]
// f(x,k)=x*x*x*k/10 + x*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]
// f(x,k)=x*x*k/10 + x*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]
// f(x,k)=1+(x-1)*(x-1)*(x-1)*k/10 + (x-1)*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]

uint16_t expou(uint16_t x, uint16_t k)
{
  // previous function was this one:
  // k*x*x*x + (1-k)*x
  // return ((unsigned long)x*x*x/0x10000*k/(RESXul*RESXul/0x10000) + (RESKul-k)*x+RESKul/2)/RESKul;

  uint32_t value = (uint32_t) x*x;
  value *= (uint32_t)k;
  value >>= 8;
  value *= (uint32_t)x;
  value >>= 12;
  value += (uint32_t)(100-k)*x+50;

  // return divu100(value);  // would speed up further by needs more code
  return value/100;
}

int16_t expo(int16_t x, int16_t k)
{
  if (k == 0) return x;
  int16_t y;
  bool neg = x < 0;
  if (neg) x = -x;
  if (k<0) {
    y = RESXu-expou(RESXu-x, -k);
  }
  else {
    y = expou(x, k);
  }
  return neg? -y : y;
}

#ifdef EXTENDED_EXPO
/// expo with y-offset
class Expo
{
  uint16_t   c;
  int16_t    d,drx;
public:
  void     init(uint8_t k, int8_t yo);
  static int16_t  expou(uint16_t x,uint16_t c, int16_t d);
  int16_t  expo(int16_t x);
};
void    Expo::init(uint8_t k, int8_t yo)
{
  c = (uint16_t) k  * 256 / 100;
  d = (int16_t)  yo * 256 / 100;
  drx = d * ((uint16_t)RESXu/256);
}
int16_t Expo::expou(uint16_t x,uint16_t c, int16_t d)
{
  uint16_t a = 256 - c - d;
  if( (int16_t)a < 0 ) a = 0;
  // a x^3 + c x + d
  //                         9  18  27        11  20   18
  uint32_t res =  ((uint32_t)x * x * x / 0x10000 * a / (RESXul*RESXul/0x10000) +
                   (uint32_t)x                   * c
  ) / 256;
  return (int16_t)res;
}
int16_t  Expo::expo(int16_t x)
{
  if(c==256 && d==0) return x;
  if(x>=0) return expou(x,c,d) + drx;
  return -expou(-x,c,-d) + drx;
}
#endif

#ifdef BOLD_FONT
ACTIVE_EXPOS_TYPE activeExpos;
#endif

void applyExpos(int16_t *anas)
{
  int16_t anas2[NUM_STICKS]; // values before expo, to ensure same expo base when multiple expo lines are used
  memcpy(anas2, anas, sizeof(anas2));

  int8_t cur_chn = -1;

#ifdef BOLD_FONT
  activeExpos = 0;
#endif

  for (uint8_t i=0; i<MAX_EXPOS; i++) {
    ExpoData &ed = g_model.expoData[i];
    if (ed.mode==0) break; // end of list
    if (ed.chn == cur_chn)
      continue;
    if (ed.phases & (1<<s_perout_flight_phase))
      continue;
    if (getSwitch(ed.swtch, 1)) {
      int16_t v = anas2[ed.chn];
      if((v<0 && (ed.mode&1)) || (v>=0 && (ed.mode&2))) {
#ifdef BOLD_FONT
        activeExpos |= ((ACTIVE_EXPOS_TYPE)1 << i);
#endif
        cur_chn = ed.chn;
        int8_t curveParam = ed.curveParam;
        if (curveParam) {
          if (ed.curveMode == MODE_CURVE)
            v = applyCurve(v, curveParam);
          else
            v = expo(v, GET_GVAR(curveParam, -100, 100, s_perout_flight_phase));
        }
        v = ((int32_t)v * GET_GVAR(ed.weight, 0, 100, s_perout_flight_phase)) / 100;
        anas[cur_chn] = v;
      }
    }
  }
}

#if !defined(CPUARM)


int16_t calc100to256(int8_t x) // return x*2.56
{
    // y = 2*x + x/2 +x/16-x/512-x/2048
    // 512 and 2048 are out of scope from int8 input --> forget it
    
    return ((int16_t)x<<1)+(x>>1)+(x>>4);
    // in case of negative values the routine above always generates a too small value, this could be corrected with this
    // I think it is not needed anyway
    // if (x<0) return -calc100to256(-x);  
    // else return ((int16_t)x<<1)+(x>>1)+(x>>4);
}

// return x*10.24
int16_t calc100toRESX_16Bits(int16_t x)  // @@@ open.20.fsguruh
{
  // return (int16_t)x*10 + x/4 - x/64;
  return ((x*41)>>2) - (x>>6);  // this implementation saves a add but reduces valid range by factor 4!!! by carefull
  // return (x*10)+(x>>2)-(x>>6);  // this would be the saver implementation
}

int16_t calc100toRESX(int8_t x) // return x*10.24
{
  // return (int16_t)x*10 + x/4 - x/64;
  return ((x*41)>>2) - x/64;
}

// return x*1.024
int16_t calc1000toRESX(int16_t x) // improve calc time by Pat MacKenzie
{
  // return x + x/32 - x/128 + x/512;
  int16_t y = x>>5;
  x+=y;
  y=y>>2;
  x-=y;
  return x+(y>>2);
}

int16_t calcRESXto1000(int16_t x)  // return x/1.024
{
// *1000/1024 = x - x/32 + x/128
  return (x - (x>>5) + (x>>7));
}
#endif


// @@@2 open.20.fsguruh ; 
// channel = channelnumber -1; 
// value = outputvalue with 100 mulitplied usual range -102400 to 102400; output -1024 to 1024
// changed rescaling from *100 to *256 to optimize performance
// rescaled to -262144)) to 262144))
int16_t applyLimits(uint8_t channel, int32_t value)
{
  LimitData * limit = limitaddress(channel);
  int16_t ofs   = calc1000toRESX(limit->offset);   // multiply to 1.24 to get range (-1024..1024)
  int16_t lim_p = calc100toRESX(limit->max + 100);
  int16_t lim_n = calc100toRESX(limit->min - 100); //multiply by 10.24 to get same range (-1024..1024)
  // int16_t ofs = limit->offset;
  // int16_t lim_p = 10 * (limit->max + 100);
  // int16_t lim_n = 10 * (limit->min - 100); //multiply by 10 to get same range as ofs (-1000..1000)  
  if (ofs > lim_p) ofs = lim_p;
  if (ofs < lim_n) ofs = lim_n;

#if defined(PPM_LIMITS_SYMETRICAL)
  if (value) {
	int16_t tmp;
    if (limit->symetrical)
      tmp = (value > 0) ? (lim_p) : (-lim_n);
    else
      tmp = (value > 0) ? (lim_p - ofs) : (-lim_n + ofs);
	value = (int32_t) value * tmp;   //  div by 1024*256 -> output = -1024..1024
	tmp=value>>16;   // that's quite tricky: the shiftright 16 operation is assmbled just with addressmove; just forget the two least significant bytes; 
	tmp>>=2;   // now one simple shift right for two bytes does the rest
// too pedantic or really useful?		
//	if ((tmp==-1) && (value&0x00020000)) tmp+=1; // check if negativ and least significant bit was set
	// if we are pedantic; a -1 result is wrong if we devide with 262144)) (-0x40000) That's why a +1 is needed
	ofs+=tmp;  // ofs can to added directly because already recalculated,

/*    int32_t tmp;
    if (limit->symetrical)
      tmp = (value > 0) ? ((int32_t) lim_p) : ((int32_t) -lim_n);
    else
      tmp = (value > 0) ? ((int32_t) lim_p - ofs) : ((int32_t) -lim_n + ofs);
    value = (value * tmp) / 100000; */
  }
#else
  if (value) {
    int16_t tmp = (value > 0) ? (lim_p - ofs) : (-lim_n + ofs);
	value = (int32_t) value * tmp;   //  div by 1024*256 -> output = -1024..1024
	tmp=value>>16;   // that's quite tricky: the shiftright 16 operation is assmbled just with addressmove; just forget the two least significant bytes; 
	tmp>>=2;   // now one simple shift right for two bytes does the rest
// too pedantic or really useful?		
//	if ((tmp==-1) && (value&0x00020000)) tmp+=1; // check if negativ and least significant bit was set
	// if we are pedantic; a -1 result is wrong if we devide with 262144)) (-0x40000) That's why a +1 is needed
	ofs+=tmp;  // ofs can to added directly because already recalculated,
	
    // int32_t tmp = (value > 0) ? ((int32_t) lim_p - ofs) : ((int32_t) -lim_n + ofs);
    // value = (value * tmp) / 100000; //div by 100000 -> output = -1024..1024
  }
#endif
  // ofs+=value;  // already done above
  // lim_p, lim_n already recalculated
  if (ofs > lim_p) ofs = lim_p;
  if (ofs < lim_n) ofs = lim_n;  
  
  // value += calc1000toRESX(ofs);
  // lim_p = calc1000toRESX(lim_p);
  // lim_n = calc1000toRESX(lim_n);
  // if (value > lim_p) value = lim_p;
  // if (value < lim_n) value = lim_n;
  // ofs = value; // we convert value to a 16bit value and reuse ofs
  
  if (limit->revert) ofs = -ofs; // finally do the reverse.

  if (safetyCh[channel] != -128) // if safety channel available for channel check
    ofs = calc100toRESX(safetyCh[channel]);

  return ofs;
}

int16_t calibratedStick[NUM_STICKS+NUM_POTS];
int16_t g_chans512[NUM_CHNOUT] = {0};
int16_t ex_chans[NUM_CHNOUT] = {0}; // Outputs (before LIMITS) of the last perMain
#ifdef HELI
int16_t cyc_anas[3] = {0};
#endif

int16_t getValue(uint8_t i)
{
  /*srcRaw is shifted +1!*/

  if (i<NUM_STICKS+NUM_POTS) return calibratedStick[i];
#if defined(PCBGRUVIN9X) || defined(PCBSKY9X)
  else if (i<NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS) return getRotaryEncoder(i-(NUM_STICKS+NUM_POTS));
#endif
  else if (i<MIXSRC_TrimAil) return calc1000toRESX((int16_t)8 * getTrimValue(s_perout_flight_phase, i-(NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS)));
  else if (i<MIXSRC_MAX) return 1024;
#if defined(PCBX9D) || defined(PCBACT)
  else if (i<MIXSRC_SA) return (switchState(SW_SA0) ? -1024 : (switchState(SW_SA1) ? 0 : 1024));
  else if (i<MIXSRC_SB) return (switchState(SW_SB0) ? -1024 : (switchState(SW_SB1) ? 0 : 1024));
  else if (i<MIXSRC_SC) return (switchState(SW_SC0) ? -1024 : (switchState(SW_SC1) ? 0 : 1024));
  else if (i<MIXSRC_SD) return (switchState(SW_SD0) ? -1024 : (switchState(SW_SD1) ? 0 : 1024));
  else if (i<MIXSRC_SE) return (switchState(SW_SE0) ? -1024 : (switchState(SW_SE1) ? 0 : 1024));
  else if (i<MIXSRC_SF) return (switchState(SW_SF0) ? -1024 : 1024);
  else if (i<MIXSRC_SG) return (switchState(SW_SG0) ? -1024 : (switchState(SW_SG1) ? 0 : 1024));
  else if (i<MIXSRC_SH) return (switchState(SW_SH0) ? -1024 : 1024);
#else
  else if (i<MIXSRC_3POS) return (switchState(SW_ID0) ? -1024 : (switchState(SW_ID1) ? 0 : 1024));
  // here the switches are skipped
  else if (i<MIXSRC_3POS+3)
#if defined(HELI)
    return cyc_anas[i-MIXSRC_3POS];
#else
    return 0;
#endif
#endif
  else if(i<CSW_PPM_BASE+NUM_CAL_PPM) return (g_ppmIns[i-CSW_PPM_BASE] - g_eeGeneral.trainer.calib[i-CSW_PPM_BASE])*2;
  else if(i<CSW_PPM_BASE+NUM_PPM) return g_ppmIns[i-CSW_PPM_BASE]*2;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT) return ex_chans[i-CSW_CHOUT_BASE];
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_TM2) return s_timerVal[i-CSW_CHOUT_BASE-NUM_CHNOUT];
#if defined(FRSKY)
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_RSSI_TX) return frskyData.rssi[1].value;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_RSSI_RX) return frskyData.rssi[0].value;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_A2) return frskyData.analog[i-CSW_CHOUT_BASE-NUM_CHNOUT-4].value;
#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_ALT) return frskyData.hub.baroAltitude_bp;
#endif
#if defined(FRSKY_HUB)
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_RPM) return frskyData.hub.rpm;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_FUEL) return frskyData.hub.fuelLevel;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_T1) return frskyData.hub.temperature1;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_T2) return frskyData.hub.temperature2;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_SPEED) return frskyData.hub.gpsSpeed_bp;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_DIST) return frskyData.hub.gpsDistance;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_GPSALT) return frskyData.hub.gpsAltitude_bp;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_CELL) return (int16_t)frskyData.hub.minCellVolts * 2;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_CELLS_SUM) return (int16_t)frskyData.hub.cellsSum;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_VFAS) return (int16_t)frskyData.hub.vfas;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_CURRENT) return (int16_t)frskyData.hub.current;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_CONSUMPTION) return frskyData.currentConsumption;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_POWER) return frskyData.power;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_ACCx) return frskyData.hub.accelX;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_ACCy) return frskyData.hub.accelY;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_ACCz) return frskyData.hub.accelZ;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_HDG) return frskyData.hub.gpsCourse_bp;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_VSPD) return frskyData.hub.varioSpeed;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_MIN_A1) return frskyData.analog[0].min;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_MIN_A2) return frskyData.analog[1].min;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_MAX_CURRENT) return *(((int16_t*)(&frskyData.hub.minAltitude))+i-(CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_MIN_ALT-1));
#endif
#endif
  else return 0;
}

#if defined(CPUARM)
#define GETSWITCH_RECURSIVE_TYPE uint32_t
volatile bool s_default_switch_value;
#else
#define GETSWITCH_RECURSIVE_TYPE uint16_t
#endif

volatile GETSWITCH_RECURSIVE_TYPE s_last_switch_used;
volatile GETSWITCH_RECURSIVE_TYPE s_last_switch_value;
/* recursive function. stack as of today (16/03/2012) grows by 8bytes at each call, which is ok! */

#if defined(CPUARM)
uint32_t cswDelays[NUM_CSW];
uint32_t cswDurations[NUM_CSW];
uint8_t  cswStates[NUM_CSW];
#endif

int16_t csLastValue[NUM_CSW];

bool __getSwitch(int8_t swtch)
{
  bool result;

  if (swtch == 0)
#if defined(CPUARM)
    return s_default_switch_value;
#else
    return s_last_switch_used & ((GETSWITCH_RECURSIVE_TYPE)1<<15);
#endif

  uint8_t cs_idx = abs(swtch);

  if (cs_idx == SWITCH_ON) {
    result = true;
  }
  else if (cs_idx <= MAX_PSWITCH) {
    result = switchState((EnumKeys)(SW_BASE+cs_idx-1));
  }
  else {
    cs_idx -= MAX_PSWITCH+1;
    CustomSwData * cs = cswaddress(cs_idx);
    if (cs->func == CS_OFF) return false;

    uint8_t s = CS_STATE(cs->func);
    if (s == CS_VBOOL) {
      GETSWITCH_RECURSIVE_TYPE mask = ((GETSWITCH_RECURSIVE_TYPE)1 << cs_idx);
      if (s_last_switch_used & mask) {
        result = (s_last_switch_value & mask);
      }
      else {
        s_last_switch_used |= mask;
        bool res1 = __getSwitch(cs->v1);
        bool res2 = __getSwitch(cs->v2);
        switch (cs->func) {
          case CS_AND:
            result = (res1 && res2);
            break;
          case CS_OR:
            result = (res1 || res2);
            break;
          // case CS_XOR:
          default:
            result = (res1 ^ res2);
            break;
        }
      }

#if !defined(CPUARM)
      if (result)
        s_last_switch_value |= ((GETSWITCH_RECURSIVE_TYPE)1<<cs_idx);
#endif
    }
    else {
      int16_t x = getValue(cs->v1-1);
      int16_t y;
      if (s == CS_VCOMP) {
        y = getValue(cs->v2-1);

        switch (cs->func) {
          case CS_EQUAL:
            result = (x==y);
            break;
          case CS_NEQUAL:
            result = (x!=y);
            break;
          case CS_GREATER:
            result = (x>y);
            break;
          case CS_LESS:
            result = (x<y);
            break;
          case CS_EGREATER:
            result = (x>=y);
            break;
          // case CS_ELESS:
          default:
            result = (x<=y);
            break;
        }
      }
      else {
#if defined(FRSKY)
        // Telemetry
        if (cs->v1 > CSW_CHOUT_BASE+NUM_CHNOUT) {
          if (frskyStreaming <= 0 && cs->v1 > CSW_CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS)
            return swtch > 0 ? false : true;

          y = convertCswTelemValue(cs);

#if defined(FRSKY_HUB)
          if (s == CS_VOFS) {
            uint8_t idx = cs->v1-CSW_CHOUT_BASE-NUM_CHNOUT-TELEM_ALT;
            if (idx < THLD_MAX) {
              // Fill the threshold array
              barsThresholds[idx] = 128 + cs->v2;
            }
          }
#endif
        }
        else {
          y = calc100toRESX(cs->v2);
        }
#else
        if (cs->v1 > CSW_CHOUT_BASE+NUM_CHNOUT) {
          y = cs->v2; // it's a timer
        }
        else {
          y = calc100toRESX(cs->v2);
        }
#endif

        switch (cs->func) {
          case CS_VPOS:
            result = (x>y);
            break;
          case CS_VNEG:
            result = (x<y);
            break;
          case CS_APOS:
            result = (abs(x)>y);
            break;
          case CS_ANEG:
            result = (abs(x)<y);
            break;
          default:
          {
            if (csLastValue[cs_idx] == -32668)
              csLastValue[cs_idx] = x;
            int16_t diff = x - csLastValue[cs_idx];
            if (cs->func == CS_DIFFEGREATER)
              result = (y >= 0 ? (diff >= y) : (diff <= y));
            else
              result = (abs(diff) >= y);
            if (result)
              csLastValue[cs_idx] = x;
            break;
          }
        }
      }
    }

#if defined(CPUARM)
    if (cs->delay) {
      if (result) {
        if (cswDelays[cs_idx] > get_tmr10ms())
          result = false;
      }
      else {
        cswDelays[cs_idx] = get_tmr10ms() + (cs->delay*50);
      }
    }
    if (cs->duration) {
      if (result && !cswStates[cs_idx])
        cswDurations[cs_idx] = get_tmr10ms() + (cs->duration*50);

      cswStates[cs_idx] = result;

      if (cswDurations[cs_idx] > get_tmr10ms()) {
        result = true;
        if (cs->delay) cswDelays[cs_idx] = get_tmr10ms() + (cs->delay*50);
      }
    }

    if (result)
      s_last_switch_value |= ((GETSWITCH_RECURSIVE_TYPE)1<<cs_idx);
#endif

  }

  return swtch > 0 ? result : !result;
}

bool getSwitch(int8_t swtch, bool nc)
{
#if defined(CPUARM)
  s_last_switch_used = 0;
  s_last_switch_value = 0;
  s_default_switch_value = nc;
#else
  s_last_switch_used = ((GETSWITCH_RECURSIVE_TYPE)nc<<15);
  s_last_switch_value = 0;
#endif
  return __getSwitch(swtch);
}

swstate_t switches_states = 0;
int8_t getMovedSwitch()
{
  static tmr10ms_t s_last_time = 0;

  int8_t result = 0;

#if defined(PCBX9D)
  for (uint8_t i=0; i<NUM_SWITCHES; i++) {
    swstate_t mask = (0x03 << (i*2));
    uint8_t prev = (switches_states & mask) >> (i*2);
    uint8_t next = (1024+getValue(MIXSRC_SA+i-1)) / 1024;
    if (prev != next) {
      switches_states = (switches_states & (~mask)) | (next << (i*2));
      if (i<5)
        result = 1+(3*i)+next;
      else if (i==5)
        result = 1+(3*5)+(next!=0);
      else if (i==6)
        result = 1+(3*5)+2+next;
      else
        result = 1+(3*5)+2+3+next;
    }
  }
#else
  for (uint8_t i=MAX_PSWITCH; i>0; i--) {
    bool prev;
    swstate_t mask = 0;
    if (i <= 3) {
      mask = (1<<(i-1));
      prev = (switches_states & mask);
    }
    else if (i <= 6) {
      prev = ((switches_states & 0x18) == ((i-4) << 3));
    }
    else {
      mask = (1<<(i-2));
      prev = (switches_states & mask);
    }
    bool next = __getSwitch(i);
    if (prev != next) {
      if (i!=MAX_PSWITCH || next==true)
        result = next ? i : -i;
      if (mask)
        switches_states ^= mask;
      else
        switches_states = (switches_states & 0xE7) | ((i-4) << 3);
    }
  }
#endif

  if ((tmr10ms_t)(get_tmr10ms() - s_last_time) > 10)
    result = 0;

  s_last_time = get_tmr10ms();

  return result;
}

#ifdef FLIGHT_PHASES
uint8_t getFlightPhase()
{
  for (uint8_t i=1; i<MAX_PHASES; i++) {
    PhaseData *phase = &g_model.phaseData[i];
    if (phase->swtch && getSwitch(phase->swtch, 0)) {
      return i;
    }
  }
  return 0;
}
#endif

int16_t getRawTrimValue(uint8_t phase, uint8_t idx)
{
  PhaseData *p = phaseaddress(phase);
#if defined(CPUM64)
  return (((int16_t)p->trim[idx]) << 2) + ((p->trim_ext >> (2*idx)) & 0x03);
#else
  return p->trim[idx];
#endif
}

int16_t getTrimValue(uint8_t phase, uint8_t idx)
{
  return getRawTrimValue(getTrimFlightPhase(phase, idx), idx);
}

void setTrimValue(uint8_t phase, uint8_t idx, int16_t trim)
{
  PhaseData *p = phaseaddress(phase);
#if defined(CPUM64)
  p->trim[idx] = (int8_t)(trim >> 2);
  p->trim_ext = (p->trim_ext & ~(0x03 << (2*idx))) + (((trim & 0x03) << (2*idx)));
#else
  p->trim[idx] = trim;
#endif
  STORE_MODELVARS;
}

uint8_t getTrimFlightPhase(uint8_t phase, uint8_t idx)
{
  for (uint8_t i=0; i<MAX_PHASES; i++) {
    if (phase == 0) return 0;
    int16_t trim = getRawTrimValue(phase, idx);
    if (trim <= TRIM_EXTENDED_MAX) return phase;
    uint8_t result = trim-TRIM_EXTENDED_MAX-1;
    if (result >= phase) result++;
    phase = result;
  }
  return 0;
}

#if defined(ROTARY_ENCODERS)
uint8_t getRotaryEncoderFlightPhase(uint8_t idx)
{
  uint8_t phase = s_perout_flight_phase;
  for (uint8_t i=0; i<MAX_PHASES; i++) {
    if (phase == 0) return 0;
#if ROTARY_ENCODERS > 2
    int16_t value;
    if(idx<(NUM_ROTARY_ENCODERS - NUM_ROTARY_ENCODERS_EXTRA))
      value = phaseaddress(phase)->rotaryEncoders[idx];
    else
      value = g_model.rotaryEncodersExtra[phase][idx-(NUM_ROTARY_ENCODERS - NUM_ROTARY_ENCODERS_EXTRA)];
#else
    int16_t value = phaseaddress(phase)->rotaryEncoders[idx];
#endif
    if (value <= ROTARY_ENCODER_MAX) return phase;
    uint8_t result = value-ROTARY_ENCODER_MAX-1;
    if (result >= phase) result++;
    phase = result;
  }
  return 0;
}

int16_t getRotaryEncoder(uint8_t idx)
{
#if ROTARY_ENCODERS > 2
  if(idx >= (NUM_ROTARY_ENCODERS - NUM_ROTARY_ENCODERS_EXTRA))
    return g_model.rotaryEncodersExtra[getRotaryEncoderFlightPhase(idx)][idx-(NUM_ROTARY_ENCODERS - NUM_ROTARY_ENCODERS_EXTRA)];
#endif
  return phaseaddress(getRotaryEncoderFlightPhase(idx))->rotaryEncoders[idx];
}

void incRotaryEncoder(uint8_t idx, int8_t inc)
{
  g_rotenc[idx] += inc;
#if ROTARY_ENCODERS > 2
  int16_t *value;
  if (idx < (NUM_ROTARY_ENCODERS - NUM_ROTARY_ENCODERS_EXTRA))
    value = &(phaseaddress(getRotaryEncoderFlightPhase(idx))->rotaryEncoders[idx]);
  else
    value = &(g_model.rotaryEncodersExtra[getRotaryEncoderFlightPhase(idx)][idx-(NUM_ROTARY_ENCODERS - NUM_ROTARY_ENCODERS_EXTRA)]);
#else
  int16_t *value = &(phaseaddress(getRotaryEncoderFlightPhase(idx))->rotaryEncoders[idx]);
#endif
  *value = limit((int16_t)-1024, (int16_t)(*value + (inc * 8)), (int16_t)+1024);
  eeDirty(EE_MODEL);
}
#endif

#if defined(GVARS)
uint8_t s_gvar_timer = 0;
uint8_t s_gvar_last = 0;

#if defined(CPUM64)
int16_t getGVarValue(int16_t x, int16_t min, int16_t max)
{
  if (x > max) {
    int8_t idx = (max <= 100 ? x - GV1_SMALL : x - GV1_LARGE);
    int8_t mul = 1;

    if (idx < 0) {
      idx = -1-idx;
      mul = -1;
    }

    x = GVAR_VALUE(idx, -1) * mul;
  }

  return limit(min, x, max);
}

void setGVarValue(uint8_t idx, int8_t value)
{
  if (GVAR_VALUE(idx, -1) != value) {
    GVAR_VALUE(idx, -1) = value;
    eeDirty(EE_MODEL);
    s_gvar_last = idx;
    s_gvar_timer = GVAR_DISPLAY_TIME;
  }
}
#else
uint8_t getGVarFlightPhase(uint8_t phase, uint8_t idx)
{
  for (uint8_t i=0; i<MAX_PHASES; i++) {
    if (phase == 0) return 0;
    int16_t trim = GVAR_VALUE(idx, phase); // TODO phase at the end everywhere to be consistent!
    if (trim <= GVAR_MAX) return phase;
    uint8_t result = trim-GVAR_MAX-1;
    if (result >= phase) result++;
    phase = result;
  }
  return 0;
}

int16_t getGVarValue(int16_t x, int16_t min, int16_t max, int8_t phase)
{
  if (x > max) {
    int8_t idx = (max <= 100 ? x - GV1_SMALL : x - GV1_LARGE);
    int8_t mul = 1;

    if (idx < 0) {
      idx = 1-idx;
      mul = -1;
    }

    x = GVAR_VALUE(idx, getGVarFlightPhase(phase, idx)) * mul;
  }

  return limit(min, x, max);
}

void setGVarValue(uint8_t idx, int8_t value, int8_t phase)
{
  phase = getGVarFlightPhase(phase, idx);
  if (GVAR_VALUE(idx, phase) != value) {
    GVAR_VALUE(idx, phase) = value;
    eeDirty(EE_MODEL);
    s_gvar_last = idx;
    s_gvar_timer = GVAR_DISPLAY_TIME;
  }
}
#endif

#endif

#if defined(FRSKY) || defined(CPUARM)
void putsTelemetryValue(xcoord_t x, uint8_t y, lcdint_t val, uint8_t unit, uint8_t att)
{
#ifdef IMPERIAL_UNITS
  if (unit == UNIT_DEGREES) {
    val += 18 ;
    val *= 115 ;
    val >>= 6 ;
  }
  if (unit == UNIT_METERS) {
    // m to ft *105/32
    val = val * 3 + ( val >> 2 ) + (val >> 5) ;
  }
  if (unit == UNIT_FEET) {
    unit = UNIT_METERS;
  }
  if (unit == UNIT_KTS) {
    unit = UNIT_KMH;
  }
#else
  if (unit == UNIT_KTS) {
    // kts to km/h
    unit = UNIT_KMH;
    val = (val * 46) / 25;
  }
#endif
  lcd_outdezAtt(x, (att & DBLSIZE ? y - FH : y), val, att & (~NO_UNIT));
  if (!(att & NO_UNIT) && unit != UNIT_RAW)
    lcd_putsiAtt(lcdLastPos/*+1*/, y, STR_VTELEMUNIT, unit, 0);
}
#endif

#define INAC_DEV_SHIFT 9   // @@@ shift right value for stick movement
#define INAC_DEVISOR 512   // Bypass splash screen with stick movement
uint16_t stickMoveValue()
{
  uint16_t sum = 0;
  for (uint8_t i=0; i<4; i++)
    sum += (anaIn(i)>>INAC_DEV_SHIFT); // sum += anaIn(i)/INAC_DEVISOR;
  return sum;
}

void checkBacklight()
{
  static uint8_t tmr10ms ;

#if defined(PCBSTD) && defined(ROTARY_ENCODER_NAVIGATION)
  rotencPoll();
#endif

  if (tmr10ms != g_blinkTmr10ms) {
    tmr10ms = g_blinkTmr10ms;
    uint16_t tsum = stickMoveValue();
    if (tsum != inacSum) {
      inacSum = tsum;
      inacCounter = 0;
      if (g_eeGeneral.backlightMode & e_backlight_mode_sticks)
        backlightOn();
    }

    if (g_eeGeneral.backlightMode == e_backlight_mode_on || lightOffCounter || isFunctionActive(FUNC_BACKLIGHT))
      BACKLIGHT_ON();
    else
      BACKLIGHT_OFF();

#if defined(PCBSTD) && defined(VOICE) && !defined(SIMU)
    Voice.voice_process() ;
#endif
  }
}

void backlightOn()
{
  lightOffCounter = ((uint16_t)g_eeGeneral.lightAutoOff*250) << 1;
}

#if defined(SPLASH)

#define SPLASH_NEEDED() (!IS_DSM2_PROTOCOL(g_model.protocol) && !g_eeGeneral.splashMode)

inline void Splash()
{
  lcd_clear();
#if defined(PCBX9D)
  lcd_bmp(0, 0, splash_lbm);
#else
  lcd_img(0, 0, splash_lbm, 0, 0);
#endif
  lcdRefresh();
}

void doSplash()
{
  if (SPLASH_NEEDED()) {
    Splash();

#if !defined(CPUARM)
    AUDIO_TADA();
#endif

#if defined(PCBSTD)
    lcdSetContrast();
#elif !defined(PCBX9D)
    tmr10ms_t curTime = get_tmr10ms() + 10;
    uint8_t contrast = 10;
    lcdSetRefVolt(contrast);
#endif

    getADC(); // init ADC array

    uint16_t inacSum = stickMoveValue();

    tmr10ms_t tgtime = get_tmr10ms() + SPLASH_TIMEOUT;
    while (tgtime != get_tmr10ms())
    {
#if defined(SIMU)
      SIMU_SLEEP(1);
#elif defined(CPUARM)
      CoTickDelay(1);
#endif

      getADC();

      uint16_t tsum = stickMoveValue();

#if defined(FSPLASH) || defined(XSPLASH)
      if (!(g_eeGeneral.splashMode & 0x04))
#endif
      if (keyDown() || tsum!=inacSum) return;  // wait for key release

      if (pwrCheck()==e_power_off) return;

#if !defined(PCBSTD) && !defined(PCBX9D)
      if (curTime < get_tmr10ms()) {
        curTime += 10;
        if (contrast < g_eeGeneral.contrast) {
          contrast += 1;
          lcdSetRefVolt(contrast);
        }
      }
#endif

      checkBacklight();
    }
  }
}
#else
#define doSplash()
#endif

void checkAll()
{
#if !defined(PCBSKY9X)
  checkLowEEPROM();
#endif

  checkTHR();
  checkSwitches();
  clearKeyEvents();
}

#if !defined(PCBSKY9X)
void checkLowEEPROM()
{
  if (g_eeGeneral.disableMemoryWarning) return;
  if (EeFsGetFree() < 100) {
    ALERT(STR_EEPROMWARN, STR_EEPROMLOWMEM, AU_ERROR);
  }
}
#endif

void checkTHR()
{
  if (g_model.disableThrottleWarning) return;

  uint8_t thrchn = (2-(stickMode&1)); //stickMode=0123 -> thr=2121

#ifdef SIMU
  int16_t lowLim = THRCHK_DEADBAND - 1024 ;
#else
  getADC();   // if thr is down - do not display warning at all
  int16_t lowLim = g_eeGeneral.calibMid[thrchn];
  lowLim = (g_eeGeneral.throttleReversed ? - lowLim - g_eeGeneral.calibSpanPos[thrchn] : lowLim - g_eeGeneral.calibSpanNeg[thrchn]);
  lowLim += THRCHK_DEADBAND;
#endif
  int16_t v = thrAnaIn(thrchn);

  if (v<=lowLim) return;

  // first - display warning
  MESSAGE(STR_THROTTLEWARN, STR_THROTTLENOTIDLE, STR_PRESSANYKEYTOSKIP, AU_THROTTLE_ALERT);

  while (1)
  {
      SIMU_SLEEP(1);

      getADC();

      int16_t v = thrAnaIn(thrchn);

      if (pwrCheck()==e_power_off || keyDown() || v<=lowLim)
        break;

      checkBacklight();

      wdt_reset();
  }
}

void checkAlarm() // added by Gohst
{
  if (g_eeGeneral.disableAlarmWarning) return;
  if (g_eeGeneral.beeperMode == e_mode_quiet) ALERT(STR_ALARMSWARN, STR_ALARMSDISABLED, AU_ERROR);
}

void checkSwitches()
{
  swstate_t last_bad_switches = 0xff;
  swstate_t states = g_model.switchWarningStates;

  while (1) {

    getMovedSwitch();

    switches_states <<= 1;

    if ((states & 0x01) || (states == switches_states)) {
      return;
    }

    // first - display warning
    if (last_bad_switches != switches_states) {
      MESSAGE(STR_SWITCHWARN, NULL, STR_PRESSANYKEYTOSKIP, last_bad_switches == 0xff ? AU_SWITCH_ALERT : AU_NONE);
#if defined(PCBX9D)
      for (uint8_t i=0; i<NUM_SWITCHES-1; i++) {
        swstate_t mask = (0x03 << (1+i*2));
        uint8_t attr = ((states & mask) == (switches_states & mask)) ? 0 : INVERS;
        char c = "\300-\301"[(states & mask) >> (1+i*2)];
        lcd_putcAtt(50+i*(2*FW+FW/2), 5*FH, 'A'+i, attr);
        lcd_putcAtt(50+i*(2*FW+FW/2)+FW, 5*FH, c, attr);
      }
#else
      uint8_t x = 2;
      for (uint8_t i=1; i<8; i++) {
        uint8_t attr = (states & (1 << i)) == (switches_states & (1 << i)) ? 0 : INVERS;
        putsSwitches(x, 5*FH, (i>5?(i+1):(i>=4?(4+((states>>4)&0x3)):i)), attr);
        if (i == 4 && attr) i++;
        if (i != 4) x += 3*FW+FW/2;
      }
#endif
      lcdRefresh();
      last_bad_switches = switches_states;
    }

    if (pwrCheck()==e_power_off || keyDown()) return; // Usb on or power off

    checkBacklight();

    wdt_reset();

    SIMU_SLEEP(1);
  }
}

void alert(const pm_char * t, const pm_char *s MESSAGE_SOUND_ARG)
{
  MESSAGE(t, s, STR_PRESSANYKEY, sound);

  while(1)
  {
    SIMU_SLEEP(1);

    if (pwrCheck() == e_power_off) return;

    if (keyDown()) return;  // wait for key release

    checkBacklight();

    wdt_reset();
  }
}

void message(const pm_char *title, const pm_char *t, const char *last MESSAGE_SOUND_ARG)
{
  lcd_clear();

#if LCD_W >= 212
  lcd_img(LCD_W-29, 0, asterisk_lbm, 0, 0);
#endif
#if !defined(CPUM64) || defined(EXTSTD)
  lcd_img(2, 0, asterisk_lbm, 0, 0);
#else
  lcd_putsAtt(0, 0, PSTR("(!)"), DBLSIZE);
#endif
#if defined(TRANSLATIONS_FR) || defined(TRANSLATIONS_IT) || defined(TRANSLATIONS_CZ)
  lcd_putsAtt(6*FW, 0, STR_WARNING, DBLSIZE);
  lcd_putsAtt(6*FW, 2*FH, title, DBLSIZE);
#else
  lcd_putsAtt(6*FW, 0, title, DBLSIZE);
  lcd_putsAtt(6*FW, 2*FH, STR_WARNING, DBLSIZE);
#endif
  lcd_filled_rect(0, 0, LCD_W, 32);
  if (t) lcd_putsLeft(5*FH, t);
  if (last) {
    lcd_putsLeft(7*FH, last);
    AUDIO_ERROR_MESSAGE(sound);
  }
  lcdRefresh();
  lcdSetContrast();
  clearKeyEvents();
}

#if defined(GVARS)
int8_t trimGvar[NUM_STICKS] = { -1, -1, -1, -1 };
#endif

#if defined(CPUARM)
void checkTrims()
{
  uint8_t event = getEvent(true);
  if (event && !IS_KEY_BREAK(event)) {
    int8_t k = EVT_KEY_MASK(event) - TRM_BASE;
    int8_t s = g_model.trimInc;
#else
uint8_t checkTrim(uint8_t event)
{
  int8_t k = EVT_KEY_MASK(event) - TRM_BASE;
  int8_t s = g_model.trimInc;
  if (k>=0 && k<8 && !IS_KEY_BREAK(event)) {
#endif
    // LH_DWN LH_UP LV_DWN LV_UP RV_DWN RV_UP RH_DWN RH_UP
    uint8_t idx = CONVERT_MODE(1+k/2) - 1;
    uint8_t phase;
    int16_t before;
    bool thro;

#if defined(GVARS)
#define TRIM_REUSED() trimGvar[idx] >= 0
    if (TRIM_REUSED()) {
#if defined(CPUM64)
      phase = 0;
#else
      phase = getGVarFlightPhase(s_perout_flight_phase, trimGvar[idx]);
#endif
      before = GVAR_VALUE(trimGvar[idx], phase);
      thro = false;
    }
    else {
      phase = getTrimFlightPhase(s_perout_flight_phase, idx);
      before = getRawTrimValue(phase, idx);
      thro = (idx==THR_STICK && g_model.thrTrim);
    }
#else
#define TRIM_REUSED() 0
    phase = getTrimFlightPhase(s_perout_flight_phase, idx);
    before = getRawTrimValue(phase, idx);
    thro = (idx==THR_STICK && g_model.thrTrim);
#endif
    int8_t  v = (s==0) ? min(32, abs(before)/4+1) : 1 << (s-1); // 1=>1  2=>2  3=>4  4=>8
    if (thro) v = 4; // if throttle trim and trim trottle then step=4
    int16_t after = (k&1) ? before + v : before - v;   // positive = k&1
#if defined(CPUARM)
    uint8_t beepTrim = 0;
#else
    bool beepTrim = false;
#endif
    for (int16_t mark=TRIM_MIN; mark<=TRIM_MAX; mark+=TRIM_MAX) {
      if ((mark!=0 || !thro) && ((mark!=TRIM_MIN && after>=mark && before<mark) || (mark!=TRIM_MAX && after<=mark && before>mark))) {
        after = mark;
        beepTrim = (mark == 0 ? 1 : 2);
      }
    }

    if ((before<after && after>TRIM_MAX) || (before>after && after<TRIM_MIN)) {
      if (!g_model.extendedTrims || TRIM_REUSED()) after = before;
    }

    if (after < TRIM_EXTENDED_MIN) {
      after = TRIM_EXTENDED_MIN;
    }
    if (after > TRIM_EXTENDED_MAX) {
      after = TRIM_EXTENDED_MAX;
    }

#if defined(GVARS)
    if (TRIM_REUSED()) {
      GVAR_VALUE(trimGvar[idx], phase) = after;
      eeDirty(EE_MODEL);
      s_gvar_last = trimGvar[idx];
      s_gvar_timer = GVAR_DISPLAY_TIME;
    }
    else {
      setTrimValue(phase, idx, after);
    }
#else
    setTrimValue(phase, idx, after);
#endif

#if defined (AUDIO)
    // toneFreq higher/lower according to trim position
    // limit the frequency, range -125 to 125 = toneFreq: 19 to 101
    if (after > TRIM_MAX)
      after = TRIM_MAX;
    if (after < TRIM_MIN)
      after = TRIM_MIN;
    after /= 4;
    after += 60;
#endif

    if (beepTrim) {
      if (beepTrim == 1) {
        AUDIO_TRIM_MIDDLE(after);
        pauseEvents(event);
      }
      else {
        AUDIO_TRIM_END(after);
        killEvents(event);
      }
    }
    else {
      AUDIO_TRIM(event, after);
    }
#if !defined(CPUARM)
    return 0;
#endif
  }
#if !defined(CPUARM)
  return event;
#endif
}

#if defined(PCBSKY9X) && !defined(REVA)
uint16_t Current_analogue;
uint16_t Current_max;
uint32_t Current_accumulator;
uint32_t Current_used;
#endif

#if defined(CPUARM) && !defined(REVA)
uint16_t sessionTimer;
#endif

#if !defined(SIMU)
static uint16_t s_anaFilt[NUMBER_ANALOG];
#endif

#if defined(SIMU)
uint16_t BandGap = 225;
#elif defined(PCBGRUVIN9X)
// #define STARTADCONV (ADCSRA  = (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2) | (1<<ADSC) | (1 << ADIE))
// G: Note that the above would have set the ADC prescaler to 128, equating to
// 125KHz sample rate. We now sample at 500KHz, with oversampling and other
// filtering options to produce 11-bit results.
uint16_t BandGap = 2040 ;
#elif defined(PCBSTD)
uint16_t BandGap ;
#endif

int16_t thrAnaIn(uint8_t chan)
{
  int16_t v = anaIn(chan);
  return (g_eeGeneral.throttleReversed) ? -v : v;
}

#if !defined(SIMU)
uint16_t anaIn(uint8_t chan)
{
#if defined(PCBX9D)
  // crossAna[]={LH,LV,RH,RV,S1,S2,LS,RS,BAT 
  // s_anaFilt[]={LH,LV,RH,RV,S1,S2,LS,RS,_BAT
  static const uint8_t crossAna[]={0,1,2,3,4,5,6,7,8};
#elif defined(PCBSKY9X) && !defined(REVA)
  static const uint8_t crossAna[]={1,5,7,0,4,6,2,3};
  if (chan == TX_CURRENT) {
    return Current_analogue ;
  }
#else
  static const pm_char crossAna[] PROGMEM ={3,1,2,0,4,5,6,7};
#endif

  volatile uint16_t *p = &s_anaFilt[pgm_read_byte(crossAna+chan)];
  return *p;
}

#if defined(CPUARM)
void getADC()
{
  uint16_t temp[NUMBER_ANALOG] = { 0 };

  for (uint32_t i=0; i<4; i++) {
    adcRead();
    for (uint32_t x=0; x<NUMBER_ANALOG; x++) {
      temp[x] += Analog_values[x];
    }
  }

  for (uint32_t x=0; x<NUMBER_ANALOG; x++) {
    s_anaFilt[x] = temp[x] >> 3;
  }
}
#else
void getADC()
{
  uint16_t temp_ana;
  for (uint8_t adc_input=0; adc_input<8; adc_input++) {
    ADMUX = adc_input|ADC_VREF_TYPE;
    ADCSRA|=0x40; // Start the AD conversion
    while (ADCSRA & 0x40); // Wait for the AD conversion to complete
    temp_ana = ADC;
    ADCSRA|=0x40; // Start the second AD conversion
    while (ADCSRA & 0x40); // Wait for the AD conversion to complete
    s_anaFilt[adc_input] = temp_ana + ADC;
  }
}
#endif

#if !defined(CPUARM)
void getADC_bandgap()
{
#if defined (PCBGRUVIN9X)
  static uint8_t s_bgCheck = 0;
  static uint16_t s_bgSum = 0;
  ADCSRA|=0x40; // request sample
  s_bgCheck += 32;
  while ((ADCSRA & 0x10)==0); ADCSRA|=0x10; // wait for sample
  if (s_bgCheck == 0) { // 8x over-sample (256/32=8)
    BandGap = s_bgSum+ADC;
    s_bgSum = 0;
  }
  else {
    s_bgSum += ADC;
  }
  ADCSRB |= (1<<MUX5);
#else
  // TODO is the next line needed (because it has been called before perMain)?
  ADMUX=0x1E|ADC_VREF_TYPE; // Switch MUX to internal 1.22V reference
  ADCSRA|=0x40;
  while ((ADCSRA & 0x10)==0);
  ADCSRA|=0x10; // take sample
  BandGap=ADC;
#endif
}
#endif

#endif // SIMU

uint8_t g_vbat100mV = 0;
uint16_t lightOffCounter;

uint16_t s_timeCumTot;
uint16_t s_timeCumThr;    // THR in 1/16 sec
uint16_t s_timeCum16ThrP; // THR% in 1/16 sec
uint8_t  s_timerState[2];
int16_t  s_timerVal[2];
uint8_t  s_timerVal_10ms[2] = {0, 0};

uint8_t  trimsCheckTimer = 0;

void resetTimer(uint8_t idx)
{
  s_timerState[idx] = TMR_OFF; // is changed to RUNNING dep from mode
  s_timerVal[idx] = g_model.timers[idx].start;
  s_timerVal_10ms[idx] = 0 ;
}

void resetAll()
{
  static bool firstReset = true;
  if (firstReset)
    firstReset = false;
  else
    AUDIO_RESET();

  resetTimer(0);
  resetTimer(1);
#if defined(FRSKY)
  resetTelemetry();
#endif
  for (uint8_t i=0; i<NUM_CSW; i++)
    csLastValue[i] = -32668;

#if defined(THRTRACE)
  s_traceCnt = 0;
  s_traceWr = 0;
#endif
}

static uint8_t lastSwPos[2] = {0, 0};
static uint16_t s_cnt[2] = {0, 0};
static uint16_t s_sum[2] = {0, 0};
static uint8_t sw_toggled[2] = {false, false};
static uint16_t s_time_cum_16[2] = {0, 0};

#if defined(THRTRACE)
uint8_t s_traceBuf[MAXTRACE];
uint8_t s_traceWr;
int8_t s_traceCnt;
#endif

#if defined(HELI) || defined(FRSKY_HUB)
uint16_t isqrt32(uint32_t n)
{
    uint16_t c = 0x8000;
    uint16_t g = 0x8000;

    for(;;) {
        if((uint32_t)g*g > n)
            g ^= c;
        c >>= 1;
        if(c == 0)
            return g;
        g |= c;
    }
}
#endif

// static variables used in perOut - moved here so they don't interfere with the stack
// It's also easier to initialize them here.
int16_t  rawAnas [NUM_STICKS] = {0};
int16_t  anas [NUM_STICKS] = {0};
int16_t  trims[NUM_STICKS] = {0};
int32_t  chans[NUM_CHNOUT] = {0};
uint8_t inacPrescale;
uint16_t inacCounter = 0;
uint16_t inacSum = 0;
BeepANACenter bpanaCenter = 0;

uint16_t sDelay[MAX_MIXERS] = {0};
int32_t  act   [MAX_MIXERS] = {0};
uint8_t  swOn  [MAX_MIXERS] = {0};
uint8_t mixWarning;

FORCEINLINE void evalTrims()
{
  uint8_t phase = s_perout_flight_phase;
  for (uint8_t i=0; i<NUM_STICKS; i++) {
    // do trim -> throttle trim if applicable
    int16_t trim = getTrimValue(phase, i);
    if (i==THR_STICK && g_model.thrTrim) {
      if (g_eeGeneral.throttleReversed)
        trim = -trim;
      int16_t v = anas[i];
	  int32_t vv = ((int32_t)trim-TRIM_MIN)*(RESX-v)>>(RESX_SHIFT+1);
      // int32_t vv = ((int32_t)trim-TRIM_MIN)*(RESX-v)/(2*RESX);	  
      trim = vv;
    }
    else if (trimsCheckTimer > 0) {
      trim = 0;
    }

    trims[i] = trim*2;
  }
}

BeepANACenter evalSticks(uint8_t mode)
{
  BeepANACenter anaCenter = 0;

#if defined(HELI)
  uint16_t d = 0;
  if (g_model.swashR.value) {
    uint32_t v = (int32_t(calibratedStick[ELE_STICK])*calibratedStick[ELE_STICK] +
        int32_t(calibratedStick[AIL_STICK])*calibratedStick[AIL_STICK]);
	uint32_t q = calc100toRESX(g_model.swashR.value);
    // uint32_t q = int32_t(RESX)*g_model.swashR.value/100;	
    q *= q;
    if (v>q)
      d = isqrt32(v);
  }
#endif

  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS; i++) {

    // normalization [0..2048] -> [-1024..1024]
    uint8_t ch = (i < NUM_STICKS ? CONVERT_MODE(i+1) - 1 : i);

#if defined(ROTARY_ENCODERS)
    int16_t v = ((i < NUM_STICKS+NUM_POTS) ? anaIn(i) : getRotaryEncoder(i-(NUM_STICKS+NUM_POTS)));
#else
    int16_t v = anaIn(i);
#endif

#ifndef SIMU
    if (i < NUM_STICKS+NUM_POTS) {
      v -= g_eeGeneral.calibMid[i];
      v  =  v * (int32_t)RESX /  (max((int16_t)100,(v>0 ?
                                       g_eeGeneral.calibSpanPos[i] :
                                       g_eeGeneral.calibSpanNeg[i])));
    }
#endif

    if(v < -RESX) v = -RESX;
    if(v >  RESX) v =  RESX;
    	
#if defined(PCBX9D)
    if (i == POT1 || i == SLIDER1)
      v = -v;
#endif

    if (g_eeGeneral.throttleReversed && ch==THR_STICK)
      v = -v;

#if defined(EXTRA_3POS)
    if (i == POT1+EXTRA_3POS-1) {
      if (v < -RESX/2)
        v = -RESX;
      else if (v > +RESX/2)
        v = +RESX;
      else
        v = 0;
    }
#endif

    if (i < NUM_STICKS+NUM_POTS) {
      calibratedStick[ch] = v; //for show in expo

      // filtering for center beep
      uint8_t tmp = (uint16_t)abs(v) / 16;
      if (tmp <= 1) anaCenter |= (tmp==0 ? (BeepANACenter)1<<ch : bpanaCenter & ((BeepANACenter)1<<ch));
    }
    else {
      if (v == 0) anaCenter |= (BeepANACenter)1<<ch;
    }

    if (ch < NUM_STICKS) { //only do this for sticks
      if (mode == e_perout_mode_normal && (isFunctionActive(FUNC_TRAINER) || isFunctionActive(FUNC_TRAINER_RUD+ch))) {
        // trainer mode
        TrainerMix* td = &g_eeGeneral.trainer.mix[ch];
        if (td->mode) {
          uint8_t chStud = td->srcChn;
          int32_t vStud  = (g_ppmIns[chStud]- g_eeGeneral.trainer.calib[chStud]);
          vStud *= td->studWeight;
          vStud /= 50;
          switch (td->mode) {
            case 1: v += vStud;   break; // add-mode
            case 2: v  = vStud;   break; // subst-mode
          }
        }
      }

#ifdef HELI
      if(d && (ch==ELE_STICK || ch==AIL_STICK))
	    v = (int32_t(v)*calc100toRESX(g_model.swashR.value))/int32_t(d);
      //  v = int32_t(v)*g_model.swashR.value*RESX/(int32_t(d)*100);
#endif

      rawAnas[ch] = v;
      anas[ch] = v; //set values for mixer
    }
  }

  /* EXPOs */
  applyExpos(anas);

  /* TRIMs */
  evalTrims();

  return anaCenter;
}

#ifdef DEBUG
/*
 * This is a test function for debugging purpose, you may insert there your code and compile with the option DEBUG=YES
 */
void testFunc()
{
#ifdef SIMU
  printf("testFunc\n"); fflush(stdout);
#endif
}
#endif

MASK_FUNC_TYPE activeFunctions  = 0;
MASK_CFN_TYPE  activeSwitches   = 0;
MASK_CFN_TYPE  activeFnSwitches = 0;
tmr10ms_t lastFunctionTime[NUM_CFN] = { 0 };

#if defined(VOICE)
PLAY_FUNCTION(playValue, uint8_t idx)
{
  int16_t val = getValue(idx);
  switch (idx) {
    case NUM_XCHNRAW+TELEM_TM1-1:
    case NUM_XCHNRAW+TELEM_TM2-1:
      PLAY_DURATION(val);
      break;
#if defined(FRSKY)
    case NUM_XCHNRAW+TELEM_RSSI_TX-1:
    case NUM_XCHNRAW+TELEM_RSSI_RX-1:
      PLAY_NUMBER(val, 1+UNIT_DBM, 0);
      break;
    case NUM_XCHNRAW+TELEM_MIN_A1-1:
    case NUM_XCHNRAW+TELEM_MIN_A2-1:
      idx -= TELEM_MIN_A1-TELEM_A1;
      // no break
    case NUM_XCHNRAW+TELEM_A1-1:
    case NUM_XCHNRAW+TELEM_A2-1:
      idx -= (NUM_XCHNRAW+TELEM_A1-1);
        // A1 and A2
      {
        uint8_t att = 0;
        int16_t converted_value = applyChannelRatio(idx, val) / 10;
        if (g_model.frsky.channels[idx].type < UNIT_RAW) {
          att = PREC1;
        }
        PLAY_NUMBER(converted_value, 1+g_model.frsky.channels[idx].type, att);
        break;
      }

    case NUM_XCHNRAW+TELEM_CELL-1:
      PLAY_NUMBER(val/10, 1+UNIT_VOLTS, PREC1);
      break;

    case NUM_XCHNRAW+TELEM_VFAS-1:
    case NUM_XCHNRAW+TELEM_CELLS_SUM-1:
      PLAY_NUMBER(val, 1+UNIT_VOLTS, PREC1);
      break;

    case NUM_XCHNRAW+TELEM_CURRENT-1:
    case NUM_XCHNRAW+TELEM_MAX_CURRENT-1:
      PLAY_NUMBER(val, 1+UNIT_AMPS, PREC1);
      break;

    case NUM_XCHNRAW+TELEM_ACCx-1:
    case NUM_XCHNRAW+TELEM_ACCy-1:
    case NUM_XCHNRAW+TELEM_ACCz-1:
      PLAY_NUMBER(val/10, 1+UNIT_G, PREC1);
      break;

    case NUM_XCHNRAW+TELEM_VSPD-1:
      PLAY_NUMBER(val/10, 1+UNIT_METERS_PER_SECOND, PREC1);
      break;

    case NUM_XCHNRAW+TELEM_CONSUMPTION-1:
      PLAY_NUMBER(val, 1+UNIT_MAH, 0);
      break;

    case NUM_XCHNRAW+TELEM_POWER-1:
      PLAY_NUMBER(val, 1+UNIT_WATTS, 0);
      break;

    case NUM_XCHNRAW+TELEM_ALT-1:
    case NUM_XCHNRAW+TELEM_MIN_ALT-1:
    case NUM_XCHNRAW+TELEM_MAX_ALT-1:
#if defined(IMPERIAL_UNITS)
      if (g_model.frsky.usrProto == USR_PROTO_WS_HOW_HIGH)
        PLAY_NUMBER(val, 1+UNIT_FEET, 0);
      else
#endif
        PLAY_NUMBER(val, 1+UNIT_METERS, 0);
      break;

    case NUM_XCHNRAW+TELEM_RPM-1:
    case NUM_XCHNRAW+TELEM_MAX_RPM-1:
      PLAY_NUMBER(val, 1+UNIT_RPMS, 0);
      break;

    case NUM_XCHNRAW+TELEM_HDG-1:
      PLAY_NUMBER(val, 1+UNIT_DEGREES, 0);
      break;

    default:
    {
      uint8_t unit = 1;
      if (idx < NUM_XCHNRAW+TELEM_TM1-1)
        val = (val * 25) / 256;
      if (idx >= NUM_XCHNRAW+TELEM_ALT-1 && idx <= NUM_XCHNRAW+TELEM_GPSALT-1)
        unit = idx - (NUM_XCHNRAW+TELEM_ALT-1);
      else if (idx >= NUM_XCHNRAW+TELEM_MAX_T1-1 && idx <= NUM_XCHNRAW+TELEM_MAX_DIST-1)
        unit = 3 + idx - (NUM_XCHNRAW+TELEM_MAX_T1-1);

      unit = pgm_read_byte(bchunit_ar+unit);
#if !defined(IMPERIAL_UNITS)
      if (unit == UNIT_KTS) {
        // kts to km/h
        unit = UNIT_KMH;
        val = (val * 46) / 25;
      }
#endif
      PLAY_NUMBER(val, unit == UNIT_RAW ? 0 : unit+1, 0);
      break;
    }
#else
    default:
    {
      PLAY_NUMBER(val, 0, 0);
      break;
    }
#endif
  }
}
#endif

#if defined(CPUARM)
uint8_t currentSpeakerVolume = 255;
uint8_t requiredSpeakerVolume;
uint8_t fnSwitchDuration[NUM_CFN] = { 0 };
#define CFN_PRESSLONG_DURATION   100

inline void playCustomFunctionFile(CustomFnData *sd, uint8_t id)
{
  char lfn[] = SOUNDS_PATH "/xxxxxx.wav";
  strncpy(lfn+sizeof(SOUNDS_PATH), sd->param, sizeof(sd->param));
  lfn[sizeof(SOUNDS_PATH)+sizeof(sd->param)] = '\0';
  strcat(lfn+sizeof(SOUNDS_PATH), SOUNDS_EXT);
  PLAY_FILE(lfn, sd->func==FUNC_BACKGND_MUSIC ? PLAY_BACKGROUND : 0, id);
}

#endif

void evalFunctions()
{
  MASK_FUNC_TYPE newActiveFunctions  = 0;
  MASK_CFN_TYPE  newActiveSwitches   = 0;
  MASK_CFN_TYPE  newActiveFnSwitches = 0;

#if defined(ROTARY_ENCODERS) && defined(GVARS)
  static rotenc_t rePreviousValues[ROTARY_ENCODERS];
#endif

  for (uint8_t i=0; i<NUM_CHNOUT; i++)
    safetyCh[i] = -128; // not defined

#if defined(GVARS)
  for (uint8_t i=0; i<4; i++)
    trimGvar[i] = -1;
#endif

  for (uint8_t i=0; i<NUM_CFN; i++) {
    CustomFnData *sd = &g_model.funcSw[i];
    int8_t swtch = sd->swtch;
    if (swtch) {
      MASK_FUNC_TYPE function_mask = (sd->func >= FUNC_TRAINER ? ((MASK_FUNC_TYPE)1 << (sd->func-FUNC_TRAINER)) : 0);
      MASK_CFN_TYPE  switch_mask   = ((MASK_CFN_TYPE)1 << i);
      uint8_t momentary = 0;

#if defined(CPUARM)

  #define MOMENTARY_START_TEST() ( (momentary && !(activeSwitches & switch_mask) && active) || \
                                   (shortPress && !active && fnSwitchDuration[i]>0 && fnSwitchDuration[i]<CFN_PRESSLONG_DURATION) || \
                                   (longPress && active && fnSwitchDuration[i]>=CFN_PRESSLONG_DURATION) )

      uint8_t shortPress=0, longPress=0;
      if (swtch > MAX_SWITCH+1+MAX_SWITCH+1+MAX_PSWITCH) {
        longPress = 1;
        swtch -= MAX_SWITCH+1+MAX_SWITCH+1+MAX_PSWITCH;
      }
      else if (swtch > MAX_SWITCH+1+MAX_SWITCH+1) {
        shortPress = 1;
        swtch -= MAX_SWITCH+1+MAX_SWITCH+1;
      }
      else

#else

  #define shortPress  0
  #define longPress   0
  #define MOMENTARY_START_TEST() (!(activeSwitches & switch_mask) && active)

#endif

      if (swtch > MAX_SWITCH+1) {
        momentary = 1;
        swtch -= MAX_SWITCH+1;
      }
      if (swtch < -MAX_SWITCH-1) {
        momentary = 1;
        swtch += MAX_SWITCH+1;
      }

      bool active = getSwitch(swtch, 0);
      if (active) newActiveSwitches |= switch_mask;
      if (momentary || longPress || shortPress) {

#if defined(CPUARM)
        bool swState = active;
#endif

        if (MOMENTARY_START_TEST()) {

          active = !(activeFnSwitches & switch_mask);
#if !defined(CPUARM)
          if (sd->func == FUNC_PLAY_BOTH && !active) {
            momentary = 1;
          }
          else
#endif
          {
            momentary = 0;
          }
        }
        else {
          active = (activeFnSwitches & switch_mask);
          momentary = 0;
        }
#if defined(CPUARM)
        if (shortPress || longPress) {
          if (swState) {
            if (fnSwitchDuration[i] < 255)
              fnSwitchDuration[i]++;
          }
          else {
            fnSwitchDuration[i] = 0;
          }
        }
#endif
      }
#if !defined(CPUARM)
      else if (sd->func == FUNC_PLAY_BOTH) {
        momentary = 1;
      }
#endif

      if (active || momentary) {
        if (sd->active) {
          if (sd->func < FUNC_TRAINER) {
            safetyCh[sd->func] = CFN_PARAM(sd);
          }

          if (!(activeFunctions & function_mask)) {
            if (sd->func == FUNC_INSTANT_TRIM) {
              if (g_menuStack[0] == menuMainView
#if defined(FRSKY)
                || g_menuStack[0] == menuTelemetryFrsky
#endif
                )
                instantTrim();
            }
          }
        }
        else if (sd->func <= FUNC_INSTANT_TRIM) {
          function_mask = 0;
          switch_mask = 0;
        }

#if defined(SDCARD)
        if (sd->func == FUNC_LOGS) {
          logDelay = CFN_PARAM(sd);
        }
#endif

        if (sd->func == FUNC_RESET) {
          switch (CFN_PARAM(sd)) {
            case 0:
            case 1:
              resetTimer(CFN_PARAM(sd));
              break;
            case 2:
              resetAll();
              break;
#if defined(FRSKY)
            case 3:
              resetTelemetry();
              break;
#endif
          }
        }

        if (sd->func == FUNC_PLAY_SOUND) {
          AUDIO_PLAY(AU_FRSKY_FIRST+CFN_PARAM(sd));
        }

#if defined(HAPTIC)
        if (sd->func == FUNC_HAPTIC) {
          haptic.event(AU_FRSKY_LAST+CFN_PARAM(sd));
        }
#endif

#if defined(CPUARM) && defined(SDCARD)
        else if (sd->func == FUNC_PLAY_TRACK || sd->func == FUNC_PLAY_VALUE) {
          tmr10ms_t tmr10ms = get_tmr10ms();
          uint8_t repeatParam = sd->active;
          if (!lastFunctionTime[i] || (repeatParam && (signed)(tmr10ms-lastFunctionTime[i])>=500*repeatParam)) {
            if (!IS_PLAYING(i+1)) {
              lastFunctionTime[i] = tmr10ms;
              if (sd->func == FUNC_PLAY_VALUE) {
                PLAY_VALUE(CFN_PARAM(sd), i+1);
              }
              else {
                playCustomFunctionFile(sd, i+1);
              }
            }
          }
        }
        else if (sd->func == FUNC_BACKGND_MUSIC) {
          if (!IS_PLAYING(i+1)) {
            playCustomFunctionFile(sd, i+1);
          }
        }
        else if (sd->func == FUNC_VOLUME) {
          requiredSpeakerVolume = ((1024 + getValue(CFN_PARAM(sd))) * VOLUME_LEVEL_MAX) / 2048;
        }
#elif defined(VOICE)
        else if (sd->func == FUNC_PLAY_TRACK || sd->func == FUNC_PLAY_BOTH || sd->func == FUNC_PLAY_VALUE) {
          tmr10ms_t tmr10ms = get_tmr10ms();
          uint8_t repeatParam = sd->active;
          if (!lastFunctionTime[i] || (sd->func==FUNC_PLAY_BOTH && active!=(bool)(activeFnSwitches&switch_mask)) || (repeatParam && (signed)(tmr10ms-lastFunctionTime[i])>=(1500<<(repeatParam-1)))) {
            lastFunctionTime[i] = tmr10ms;
            if (sd->func == FUNC_PLAY_VALUE) {
              PLAY_VALUE(CFN_PARAM(sd), i+1);
            }
            else {
              PUSH_CUSTOM_PROMPT(active ? sd->param : sd->param+1, i+1);
            }
          }
        }
#endif

#if defined(DEBUG)
        else if (sd->func == FUNC_TEST) {
          testFunc();
        }
#endif

#if defined(GVARS)
        else if (sd->func >= FUNC_ADJUST_GV1) {
          if (CFN_PARAM(sd) >= MIXSRC_TrimRud-1 && CFN_PARAM(sd) <= MIXSRC_TrimAil-1) {
            trimGvar[CFN_PARAM(sd)-MIXSRC_TrimRud+1] = sd->func-FUNC_ADJUST_GV1;
          }
#if defined(ROTARY_ENCODERS)
          else if (CFN_PARAM(sd) >= MIXSRC_REa-1 && CFN_PARAM(sd) < MIXSRC_TrimRud-1) {
            int8_t scroll = rePreviousValues[CFN_PARAM(sd)-MIXSRC_REa+1] - (g_rotenc[CFN_PARAM(sd)-MIXSRC_REa+1] / ROTARY_ENCODER_GRANULARITY);
            if (scroll) {
              SET_GVAR(sd->func-FUNC_ADJUST_GV1, GVAR_VALUE(sd->func-FUNC_ADJUST_GV1, s_perout_flight_phase) + scroll, s_perout_flight_phase);
            }
          }
#endif
          else {
            SET_GVAR(sd->func-FUNC_ADJUST_GV1, limit((int16_t)-1250, getValue(CFN_PARAM(sd)), (int16_t)1250) / 10, s_perout_flight_phase);
          }
        }
#endif

        if (active) {
          newActiveFnSwitches |= switch_mask;
          newActiveFunctions |= function_mask;
        }
      }
      else {
        lastFunctionTime[i] = 0;
#if defined(CPUARM)
        fnSwitchDuration[i] = 0;
#endif
#if defined(CPUARM) && defined(SDCARD)
        if (sd->func == FUNC_BACKGND_MUSIC && isFunctionActive(FUNC_BACKGND_MUSIC)) {
          STOP_PLAY(i+1);
        }
#endif
      }
    }
  }

  activeSwitches   = newActiveSwitches;
  activeFnSwitches = newActiveFnSwitches;
  activeFunctions  = newActiveFunctions;

#if defined(ROTARY_ENCODERS) && defined(GVARS)
  for (uint8_t i=0; i<ROTARY_ENCODERS; i++)
    rePreviousValues[i] = (g_rotenc[i] / ROTARY_ENCODER_GRANULARITY);
#endif
}

uint8_t s_perout_flight_phase;
void perOut(uint8_t mode, uint8_t tick10ms)
{
  BeepANACenter anaCenter = evalSticks(mode);

  if (mode == e_perout_mode_normal) {
    //===========BEEP CENTER================
    anaCenter &= g_model.beepANACenter;
    if(((bpanaCenter ^ anaCenter) & anaCenter)) AUDIO_POT_STICK_MIDDLE();
    bpanaCenter = anaCenter;
  }

#if defined(HELI)
  if(g_model.swashR.value)
  {
    uint32_t v = ((int32_t)anas[ELE_STICK]*anas[ELE_STICK] + (int32_t)anas[AIL_STICK]*anas[AIL_STICK]);
    // uint32_t q = (int32_t)RESX*g_model.swashR.value/100;
	uint32_t q = calc100toRESX(g_model.swashR.value);		
    q *= q;
    if(v>q)
    {
      uint16_t d = isqrt32(v);
	  // @@@ open.20.fsguruh
      // anas[ELE_STICK] = (int32_t)anas[ELE_STICK]*g_model.swashR.value*RESX/((int32_t)d*100);
      // anas[AIL_STICK] = (int32_t)anas[AIL_STICK]*g_model.swashR.value*RESX/((int32_t)d*100);
	  int16_t tmp = calc100toRESX(g_model.swashR.value);
      anas[ELE_STICK] = (int32_t) anas[ELE_STICK]*tmp/d; 
      anas[AIL_STICK] = (int32_t) anas[AIL_STICK]*tmp/d; 
    }
  }

#define REZ_SWASH_X(x)  ((x) - (x)/8 - (x)/128 - (x)/512)   //  1024*sin(60) ~= 886
#define REZ_SWASH_Y(x)  ((x))   //  1024 => 1024

  if(g_model.swashR.type)
  {
    int16_t vp = anas[ELE_STICK]+trims[ELE_STICK];
    int16_t vr = anas[AIL_STICK]+trims[AIL_STICK];
    int16_t vc = 0;
    if (g_model.swashR.collectiveSource)
      vc = getValue(g_model.swashR.collectiveSource-1);

    if(g_model.swashR.invertELE) vp = -vp;
    if(g_model.swashR.invertAIL) vr = -vr;
    if(g_model.swashR.invertCOL) vc = -vc;

    switch (g_model.swashR.type)
    {
      case SWASH_TYPE_120:
        vp = REZ_SWASH_Y(vp);
        vr = REZ_SWASH_X(vr);
        cyc_anas[0] = vc - vp;
        cyc_anas[1] = vc + vp/2 + vr;
        cyc_anas[2] = vc + vp/2 - vr;
        break;
      case SWASH_TYPE_120X:
        vp = REZ_SWASH_X(vp);
        vr = REZ_SWASH_Y(vr);
        cyc_anas[0] = vc - vr;
        cyc_anas[1] = vc + vr/2 + vp;
        cyc_anas[2] = vc + vr/2 - vp;
        break;
      case SWASH_TYPE_140:
        vp = REZ_SWASH_Y(vp);
        vr = REZ_SWASH_Y(vr);
        cyc_anas[0] = vc - vp;
        cyc_anas[1] = vc + vp + vr;
        cyc_anas[2] = vc + vp - vr;
        break;
      case SWASH_TYPE_90:
        vp = REZ_SWASH_Y(vp);
        vr = REZ_SWASH_Y(vr);
        cyc_anas[0] = vc - vp;
        cyc_anas[1] = vc + vr;
        cyc_anas[2] = vc - vr;
        break;
      default:
        break;
    }
  }
#endif

  memclear(chans, sizeof(chans));        // All outputs to 0
  
  //========== MIXER LOOP ===============
  uint8_t lv_mixWarning = 0;

  uint8_t pass = 0;

  bitfield_channels_t dirtyChannels = (bitfield_channels_t)-1; // all dirty when mixer starts

  do {

    // printf("[pass %d]\n", pass); fflush(stdout);

    bitfield_channels_t passDirtyChannels = 0;

    for (uint8_t i = 0; i < MAX_MIXERS; i++) {

      MixData *md = mixaddress(i);

      if (md->srcRaw == 0) break;

      uint8_t k = md->srcRaw - 1;

      if (!(dirtyChannels & ((bitfield_channels_t)1 << md->destCh))) continue;

      if (md->phases & (1 << s_perout_flight_phase)) continue;

      //========== SWITCH ===============
      bool sw = getSwitch(md->swtch, 1);

      //========== VALUE ===============
      int16_t v = 0;
      if (mode != e_perout_mode_normal) {
        if (!sw || k >= NUM_STICKS || (k == THR_STICK && g_model.thrTrim)) {
          continue;
        }
        else {
          if (!(mode & e_perout_mode_nosticks)) v = anas[k];
        }
      }
      else {
        if (k < NUM_STICKS)
          v = md->noExpo ? rawAnas[k] : anas[k]; //Switch is on. MAX=FULL=512 or value.
#if defined(PCBX9D) || defined(PCBACT)
        else {
          v = getValue(k);
          // TODO switches: if (v < 0 && !md->swtch) sw = false;
          if (k>=MIXSRC_CH1-1 && k<=MIXSRC_CHMAX-1 && md->destCh != k-MIXSRC_CH1+1) {
            if (dirtyChannels & ((bitfield_channels_t)1 << (k-MIXSRC_CH1+1)) & (passDirtyChannels|~(((bitfield_channels_t) 1 << md->destCh)-1)))
              passDirtyChannels |= (bitfield_channels_t) 1 << md->destCh;
            if (k-MIXSRC_CH1+1 < md->destCh || pass > 0)
			  v = chans[k-MIXSRC_CH1+1] >> 8;  // remove factor 256 from old mix loop; was 100 before
              // v = chans[k-MIXSRC_CH1+1] / 100;			  
          }
        }
#else
        else if (k >= MIXSRC_THR-1 && k <= MIXSRC_SWC-1) {
          v = getSwitch(k-MIXSRC_THR+1+1, 0) ? +1024 : -1024;
          if (v < 0 && !md->swtch) sw = false;
        }
        else {
          v = getValue(k<=MIXSRC_3POS ? k : k-MAX_SWITCH);
          if (k>=MIXSRC_CH1-1 && k<=MIXSRC_CHMAX-1 && md->destCh != k-MIXSRC_CH1+1) {
            if (dirtyChannels & ((bitfield_channels_t)1 << (k-MIXSRC_CH1+1)) & (passDirtyChannels|~(((bitfield_channels_t) 1 << md->destCh)-1)))
              passDirtyChannels |= (bitfield_channels_t) 1 << md->destCh;
            if (k-MIXSRC_CH1+1 < md->destCh || pass > 0)
			  v = chans[k-MIXSRC_CH1+1] >> 8;  // remove factor 256 from old mix loop; was 100 before
			  // v = chans[k-MIXSRC_CH1+1] / 100;
          }
        }
#endif
      }

      bool apply_offset_and_curve = true;

      //========== DELAYS ===============
      uint8_t swTog;
      if (sw) { // switch on?  (if no switch selected => on)
        swTog = !swOn[i];
        if (mode == e_perout_mode_normal) {
          swOn[i] = true;
          if (md->delayUp) {
            if (swTog) {
              sDelay[i] = (sDelay[i] ? 0 : (md->delayUp * (100/DELAY_STEP)));
            }
            if (sDelay[i] > 0) { // perform delay
              sDelay[i] = max<int16_t>(0, (int16_t)sDelay[i] - tick10ms);
              if (!md->swtch) {
                v = -1024;
              }
              else {
                continue;
              }
            }
          }
          if (md->mixWarn) lv_mixWarning |= 1 << (md->mixWarn - 1); // Mix warning
        }
      }
      else {
        bool has_delay = false;
        swTog = swOn[i];
        swOn[i] = false;
        if (md->delayDown) {
          uint16_t delay = sDelay[i];
          if (swTog) {
            delay = (delay ? 0 : (md->delayDown * (100/DELAY_STEP)));
          }
          if (delay > 0) { // perform delay
            delay = max<int16_t>(0, (int16_t)delay - tick10ms);
            if (!md->swtch) v = +1024;
            has_delay = true;
          }
          else if (!md->swtch) {
            v = -1024;
          }
          sDelay[i] = delay;
        }
        if (!has_delay) {
          if (md->speedDown) {
            if (md->mltpx == MLTPX_REP) continue;
            if (md->swtch) {
              v = 0;
              apply_offset_and_curve = false;
            }
          }
          else if (md->swtch) {
            continue;
          }
        }
      }

#ifdef BOLD_FONT
      activeMixes |= ((ACTIVE_MIXES_TYPE)1 << i);
#endif

      //========== OFFSET ===============
      if (apply_offset_and_curve) {
        int8_t offset = GET_GVAR(MD_OFFSET(md), -125, 125, s_perout_flight_phase);
        if (offset) v += calc100toRESX(offset);
      }

      //========== TRIMS ===============
      if (!(mode & e_perout_mode_notrims)) {
        int8_t mix_trim = md->carryTrim;
        if (mix_trim < TRIM_ON)
          mix_trim = -mix_trim - 1;
        else if (mix_trim == TRIM_ON && k < NUM_STICKS)
          mix_trim = k;
        else
          mix_trim = -1;
        if (mix_trim >= 0) v += trims[mix_trim];
      }

      //========== CURVES ===============
      if (apply_offset_and_curve && md->curveParam && md->curveMode == MODE_CURVE) {
        v = applyCurve(v, md->curveParam);
      }

      int16_t weight = GET_GVAR(MD_WEIGHT(md), -500, 500, s_perout_flight_phase);

      //========== SPEED ===============
      if (mode == e_perout_mode_normal && (md->speedUp || md->speedDown)) { // there are delay values
      
#define DEL_MULT 256
#define DEL_MULT_SHIFT 8

		int16_t diff = v - (act[i] >> DEL_MULT_SHIFT); // int16_t diff = v - act[i] / DEL_MULT;

        if (diff) {
          //rate = steps/sec => 32*1024/100*md->speedUp/Down
          //act[i] += diff>0 ? (32768)/((int16_t)100*md->speedUp) : -(32768)/((int16_t)100*md->speedDown);
          //-100..100 => 32768 ->  100*83886/256 = 32768,   For MAX we divide by 2 since it's asymmetrical
          if (tick10ms) {
            int32_t rate = (int32_t) DEL_MULT * 2048 * 100 * tick10ms;
            if (weight) rate /= abs(weight);

            act[i] = (diff>0) ? ((md->speedUp>0)   ? act[i]+(rate)/((int16_t)(100/SLOW_STEP)*md->speedUp)   :  (int32_t)v<<DEL_MULT_SHIFT) :
                                ((md->speedDown>0) ? act[i]-(rate)/((int16_t)(100/SLOW_STEP)*md->speedDown) :  (int32_t)v<<DEL_MULT_SHIFT) ;
			//act[i] = (diff>0) ? ((md->speedUp>0)   ? act[i]+(rate)/((int16_t)(100/SLOW_STEP)*md->speedUp)   :  (int32_t)v*DEL_MULT) :
            //                    ((md->speedDown>0) ? act[i]-(rate)/((int16_t)(100/SLOW_STEP)*md->speedDown) :  (int32_t)v*DEL_MULT) ;
          }

          {
            int32_t tmp = act[i] >> DEL_MULT_SHIFT;
            if (((diff > 0) && (v < tmp)) || ((diff < 0) && (v > tmp))) act[i] = (int32_t) v<<DEL_MULT_SHIFT; //deal with overflow		  
            // int32_t tmp = act[i] / DEL_MULT;
            // if (((diff > 0) && (v < tmp)) || ((diff < 0) && (v > tmp))) act[i] = (int32_t) v * DEL_MULT; //deal with overflow			
          }

          v = act[i] >> DEL_MULT_SHIFT;  // v = act[i] / DEL_MULT;		  
        }
      }

      //========== WEIGHT ===============
	  // @@@2 recalculate weight to a 256 basis which ease the calculation later a lot
      int32_t dv = (int32_t) v * calc100to256(weight);

      //========== DIFFERENTIAL =========
      if (md->curveMode == MODE_DIFFERENTIAL) {
/*        int8_t curveParam = GET_GVAR(md->curveParam, -100, 100, s_perout_flight_phase);
        if (curveParam > 0 && dv < 0)
          dv = (dv * (100 - curveParam)) / 100;
        else if (curveParam < 0 && dv > 0)
          dv = (dv * (100 + curveParam)) / 100; */
		  
	    // @@@2 also recalculate curveParam to a 256 basis which ease the calculation later a lot
		int16_t curveParam=calc100to256(GET_GVAR(md->curveParam, -100, 100, s_perout_flight_phase));
        if (curveParam > 0 && dv < 0)
          dv = (dv * (256 - curveParam)) >> 8;
        else if (curveParam < 0 && dv > 0)
          dv = (dv * (256 + curveParam)) >> 8;		  
      }

      int32_t *ptr = &chans[md->destCh]; // Save calculating address several times

      if (i == 0 || md->destCh != (md - 1)->destCh)
        *ptr = 0;

      switch (md->mltpx) {
        case MLTPX_REP:
          *ptr = dv;
#ifdef BOLD_FONT
          for (uint8_t m=i-1; m<MAX_MIXERS && mixaddress(m)->destCh==md->destCh; m--)
            activeMixes &= ~((ACTIVE_MIXES_TYPE)1 << m);
#endif
          break;
        case MLTPX_MUL:
		  // @@@2 we have to remove the weight factor of 256 in case of 100%; now we use the new base of 256
		  // dv /= 100;
          dv >>= 8;		
          dv *= *ptr;
		  dv >>= RESX_SHIFT;   // same as dv /= RESXl;
          *ptr = dv;
          break;
        default: // MLTPX_ADD
          *ptr += dv; //Mixer output add up to the line (dv + (dv>0 ? 100/2 : -100/2))/(100);
          break;
      }
    }

    tick10ms = 0;
    dirtyChannels &= passDirtyChannels;

  } while (++pass < 5 && dirtyChannels);

  mixWarning = lv_mixWarning;
}

#define TIME_TO_WRITE() (s_eeDirtyMsk && (tmr10ms_t)(get_tmr10ms() - s_eeDirtyTime10ms) >= (tmr10ms_t)WRITE_DELAY_10MS)

#ifdef BOLD_FONT
ACTIVE_MIXES_TYPE activeMixes;
#endif
int32_t sum_chans512[NUM_CHNOUT] = {0};

#if defined(CPUARM)
bool doMixerCalculations()
#else
void doMixerCalculations()
#endif
{
#if defined(PCBGRUVIN9X) && defined(DEBUG) && !defined(VOICE)
  PORTH |= 0x40; // PORTH:6 LOW->HIGH signals start of mixer interrupt
#endif

  static tmr10ms_t lastTMR;

  tmr10ms_t tmr10ms = get_tmr10ms();
  uint8_t tick10ms = (tmr10ms >= lastTMR ? tmr10ms - lastTMR : 1);
  lastTMR = tmr10ms;

#if defined(BOLD_FONT)
  activeMixes = 0;
#endif

  getADC();

#if defined(PCBSKY9X) && !defined(REVA) && !defined(SIMU)
  Current_analogue = (Current_analogue*31 + s_anaFilt[8] ) >> 5 ;
  if (Current_analogue > Current_max)
    Current_max = Current_analogue ;
#elif defined(PCBGRUVIN9X) && !defined(SIMU)
  // For PCB V4, use our own 1.2V, external reference (connected to ADC3)
  ADCSRB &= ~(1<<MUX5);
  ADMUX = 0x03|ADC_VREF_TYPE; // Switch MUX to internal reference
#elif defined(PCBSTD) && !defined(SIMU)
  ADMUX = 0x1E|ADC_VREF_TYPE; // Switch MUX to internal reference
#endif

#define MAX_ACT 0xffff
  static uint16_t fp_act[MAX_PHASES] = {0};
  static uint16_t delta = 0;
  static ACTIVE_PHASES_TYPE s_fade_flight_phases = 0;
  static uint8_t s_last_phase = 255; // TODO reinit everything here when the model changes, no???
  uint8_t phase = getFlightPhase();
  int32_t weight = 0;

  if (s_last_phase != phase) {
    if (s_last_phase != 255) PLAY_PHASE_OFF(s_last_phase);
    PLAY_PHASE_ON(phase);

    if (s_last_phase == 255) {
      fp_act[phase] = MAX_ACT;
    }
    else {
      uint8_t fadeTime = max(g_model.phaseData[s_last_phase].fadeOut, g_model.phaseData[phase].fadeIn);
      ACTIVE_PHASES_TYPE transitionMask = ((ACTIVE_PHASES_TYPE)1 << s_last_phase) + ((ACTIVE_PHASES_TYPE)1 << phase);
      if (fadeTime) {
        s_fade_flight_phases |= transitionMask;
        delta = (MAX_ACT / (100/SLOW_STEP)) / fadeTime;
      }
      else {
        s_fade_flight_phases &= ~transitionMask;
        fp_act[s_last_phase] = 0;
        fp_act[phase] = MAX_ACT;
      }
    }
    s_last_phase = phase;
  }

  if (s_fade_flight_phases) {
    memclear(sum_chans512, sizeof(sum_chans512));
    weight = 0;
    for (uint8_t p=0; p<MAX_PHASES; p++) {
      if (s_fade_flight_phases & ((ACTIVE_PHASES_TYPE)1 << p)) {
        s_perout_flight_phase = p;
        perOut(e_perout_mode_normal, tick10ms);
        for (uint8_t i=0; i<NUM_CHNOUT; i++)
          sum_chans512[i] += (chans[i]>>4) * fp_act[p];  // @@@ use shift right instead of division
		  // sum_chans512[i] += (chans[i] / 16) * fp_act[p];		
        weight += fp_act[p];
      }
    }
    assert(weight);
    s_perout_flight_phase = phase;
  }
  else {
    s_perout_flight_phase = phase;
    perOut(e_perout_mode_normal, tick10ms);
  }

  //========== LIMITS ===============
  for (uint8_t i=0; i<NUM_CHNOUT; i++) {
    // chans[i] holds data from mixer.   chans[i] = v*weight => 1024*100
    // later we multiply by the limit (up to 100) and then we need to normalize
    // at the end chans[i] = chans[i]/100 =>  -1024..1024
    // interpolate value with min/max so we get smooth motion from center to stop
    // this limits based on v original values and min=-1024, max=1024  RESX=1024
	
    // int32_t q = (s_fade_flight_phases ? (sum_chans512[i] / weight) * 16 : chans[i]);
	int32_t q = (s_fade_flight_phases ? (sum_chans512[i] / weight) << 4 : chans[i]);
	// @@@2 now remove the internal 256 100% basis by a simple shift operation
	ex_chans[i]=q>>8; // for the next perMain	

    int16_t value = applyLimits(i, q);  // applyLimits will remove the 256 100% basis

    cli();
    g_chans512[i] = value;  // copy consistent word to int-level
    sei();
  }

#if defined(PCBGRUVIN9X) && defined(DEBUG) && !defined(VOICE)
  PORTH &= ~0x40; // PORTH:6 HIGH->LOW signals end of mixer interrupt
#endif

  // Bandgap has had plenty of time to settle...
#if !defined(CPUARM)
  getADC_bandgap();
#endif

#if defined(CPUARM)
  if (!tick10ms) return false; //make sure the rest happen only every 10ms.
#else
  if (!tick10ms) return; //make sure the rest happen only every 10ms.
#endif

  /* Throttle trace */
  int16_t val;

  if (g_model.thrTraceSrc > NUM_POTS) {
    uint8_t ch = g_model.thrTraceSrc-NUM_POTS-1;
    val = g_chans512[ch];
    if (g_model.limitData[ch].revert)
      val = -val + calc100toRESX(g_model.limitData[ch].max + 100);
    else
      val = val - calc100toRESX(g_model.limitData[ch].min - 100);
#if defined(PPM_LIMITS_SYMETRICAL)
    if (g_model.limitData[ch].symetrical)
      val -= calc1000toRESX(g_model.limitData[ch].offset);
#endif
    // @@@ open.20.fsguruh  optimized calculation; now *16 /16 instead of 10 base
    val = (val << 4) / (16+((g_model.limitData[ch].max-g_model.limitData[ch].min)>>5));
	if (val<0) val=0;  // prevent val be negative, which would corrupt throttle trace and timers; could occur if safetyswitch is smaller than limits
	// val = val * 10 / (10+(g_model.limitData[ch].max-g_model.limitData[ch].min)/20);
  }
  else {
    val = RESX + calibratedStick[g_model.thrTraceSrc == 0 ? THR_STICK : g_model.thrTraceSrc+NUM_STICKS-1];
  }

  val >>= (RESX_SHIFT-4); // calibrate it @@@ open.20.fsguruh
  // val /= (RESX/16); // calibrate it
  
  static tmr10ms_t s_time_tot;
  static uint8_t s_cnt_1s;
  static uint16_t s_sum_1s;
#if defined(THRTRACE)
  static tmr10ms_t s_time_trace;
  static uint16_t s_cnt_10s;
  static uint16_t s_sum_10s;
#endif

  s_cnt_1s++;
  s_sum_1s += val;

  if ((tmr10ms_t)(tmr10ms - s_time_tot) >= 100) { // 1sec
    s_time_tot += 100;
    s_timeCumTot += 1;

    val = s_sum_1s / s_cnt_1s;
    s_timeCum16ThrP += val / 2;
    if (val) s_timeCumThr += 1;

#if defined(THRTRACE)
    s_cnt_10s += s_cnt_1s;
    s_sum_10s += s_sum_1s;

    if ((tmr10ms_t)(tmr10ms - s_time_trace) >= 1000) { // 10s
      s_time_trace += 1000;
      val = s_sum_10s / s_cnt_10s;
      s_sum_10s = 0;
      s_cnt_10s = 0;

      s_traceBuf[s_traceWr++] = val;
      if (s_traceWr >= MAXTRACE) s_traceWr = 0;
      if (s_traceCnt >= 0) s_traceCnt++;
    }
#endif

    s_cnt_1s = 0;
    s_sum_1s = 0;
  }

  // Timers start
  for (uint8_t i=0; i<2; i++) {
    int8_t tm = g_model.timers[i].mode;
    uint16_t tv = g_model.timers[i].start;

    if (tm) {
      if (s_timerState[i] == TMR_OFF) {
        s_timerState[i] = TMR_RUNNING;
        s_cnt[i] = 0;
        s_sum[i] = 0;
        s_time_cum_16[i] = 0;
      }

      uint8_t atm = (tm >= 0 ? tm : TMR_VAROFS-tm-1);

      // value for time described in timer->mode
      // OFFABSTHsTH%THt
      if (atm == TMRMODE_THR_REL) {
        s_cnt[i]++;
        s_sum[i]+=val;
      }

      if (atm>=(TMR_VAROFS+MAX_SWITCH)){ // toggeled switch
        if(!(sw_toggled[i] | s_sum[i] | s_cnt[i] | lastSwPos[i])) { lastSwPos[i] = tm < 0; s_sum[i] = 1; }  // if initializing then init the lastSwPos
        uint8_t swPos = getSwitch(tm>0 ? tm-(TMR_VAROFS+MAX_SWITCH-1) : tm+MAX_SWITCH, 0);
        if (swPos && !lastSwPos[i]) sw_toggled[i] = !sw_toggled[i];  // if switch is flipped first time -> change counter state
        lastSwPos[i] = swPos;
      }

      if ( (s_timerVal_10ms[i] += tick10ms ) >= 100 ) {
        s_timerVal_10ms[i] -= 100 ;

        if (tv) s_timerVal[i] = tv - s_timerVal[i];

        if (atm==TMRMODE_ABS) {
          s_timerVal[i]++;
        }
        else if (atm==TMRMODE_THR) {
          if (val) s_timerVal[i]++;
        }
        else if (atm==TMRMODE_THR_REL) {
          if (s_cnt[i]) {
            val       = s_sum[i]/s_cnt[i];
            s_sum[i] -= val*s_cnt[i]; //rest
            s_cnt[i]  = 0;
            s_time_cum_16[i] += val/2;
            if (s_time_cum_16[i] >= 16) {
              s_timerVal[i] ++;
              s_time_cum_16[i] -= 16;
            }
          }
        }
        else if (atm==TMRMODE_THR_TRG) {
          if (val || s_timerVal[i] > 0)
            s_timerVal[i]++;
        }
        else {
          if (atm<(TMR_VAROFS+MAX_SWITCH))
            sw_toggled[i] = tm>0 ? getSwitch(tm-(TMR_VAROFS-1), 0) : !getSwitch(-tm, 0); // normal switch
          if (sw_toggled[i])
            s_timerVal[i]++;
        }

        switch(s_timerState[i])
        {
          case TMR_RUNNING:
            if (tv && s_timerVal[i]>=(int16_t)tv) s_timerState[i]=TMR_BEEPING;
            break;
          case TMR_BEEPING:
            if (s_timerVal[i] >= (int16_t)tv + MAX_ALERT_TIME) s_timerState[i]=TMR_STOPPED;
            break;
        }

        if (tv) s_timerVal[i] = tv - s_timerVal[i]; //if counting backwards - display backwards
      }
    }
  };

  static int16_t last_tmr;
  if (last_tmr != s_timerVal[0]) { // beep only if seconds advance
    if (s_timerState[0] == TMR_RUNNING) {
      if (g_eeGeneral.preBeep && g_model.timers[0].start) { // beep when 30, 15, 10, 5,4,3,2,1 seconds remaining
        if(s_timerVal[0]==30) AUDIO_TIMER_30(); //beep three times
        if(s_timerVal[0]==20) AUDIO_TIMER_20(); //beep two times
        if(s_timerVal[0]==10) AUDIO_TIMER_10();
        if(s_timerVal[0]<= 3) AUDIO_TIMER_LT3(s_timerVal[0]);
      }

      if (g_eeGeneral.minuteBeep && (((g_model.timers[0].start ? g_model.timers[0].start-s_timerVal[0] : s_timerVal[0])%60)==0)) { // short beep every minute
        AUDIO_MINUTE_BEEP();
      }
    }
    else if(s_timerState[0] == TMR_BEEPING) {
      AUDIO_WARNING1();
    }
    last_tmr = s_timerVal[0];
  }
  // Timers end

  if (s_fade_flight_phases) {
    uint16_t tick_delta = delta * tick10ms;
    for (uint8_t p=0; p<MAX_PHASES; p++) {
      ACTIVE_PHASES_TYPE phaseMask = ((ACTIVE_PHASES_TYPE)1 << p);
      if (s_fade_flight_phases & phaseMask) {
        if (p == phase) {
          if (MAX_ACT - fp_act[p] > tick_delta)
            fp_act[p] += tick_delta;
          else {
            fp_act[p] = MAX_ACT;
            s_fade_flight_phases -= phaseMask;
          }
        }
        else {
          if (fp_act[p] > tick_delta)
            fp_act[p] -= tick_delta;
          else {
            fp_act[p] = 0;
            s_fade_flight_phases -= phaseMask;
          }
        }
      }
    }
  }

#if defined(CPUARM)
  requiredSpeakerVolume = g_eeGeneral.speakerVolume + VOLUME_LEVEL_DEF;
#endif

  evalFunctions();

#if defined(DSM2)
  if (s_rangecheck_mode) AUDIO_PLAY(AU_FRSKY_CHEEP);
#endif

#if defined(CPUARM)
  return true;
#endif
}

void perMain()
{
#if defined(SIMU)
  doMixerCalculations();
#elif !defined(CPUARM)
  uint16_t t0 = getTmr16KHz();
  int16_t delta = (nextMixerEndTime - lastMixerDuration) - t0;
  if (delta > 0 && delta < MAX_MIXER_DELTA)  {
    // @@@ open.20.fsguruh
    // SLEEP();   // wouldn't that make sense? should save a lot of battery power!!!
	// asm volatile(" sleep        \n\t");  // if _SLEEP() is not defined use this
    return;
  }  

  nextMixerEndTime = t0 + MAX_MIXER_DELTA;
  // this is a very tricky implementation; lastMixerEndTime is just like a default value not to stop mixcalculations totally;
  // the real value for lastMixerEndTime is calculated inside pulses_XXX.cpp which aligns the timestamp to the pulses generated
  // nextMixerEndTime is actually defined inside pulses_XXX.h  

  doMixerCalculations();

  t0 = getTmr16KHz() - t0;
  lastMixerDuration = t0;
  if (t0 > maxMixerDuration) maxMixerDuration = t0;
#endif

// TODO same code here + integrate the timer which could be common
#if defined(CPUARM)
  if (!Tenms) return;
  Tenms = 0 ;
#endif

#if defined(PCBSKY9X)
  Current_accumulator += Current_analogue ;
  static uint32_t OneSecTimer;
  if (++OneSecTimer >= 100) {
    OneSecTimer -= 100 ;
    sessionTimer += 1;
    Current_used += Current_accumulator / 100 ;                     // milliAmpSeconds (but scaled)
    Current_accumulator = 0 ;
  }
#endif

#if defined(CPUARM)
  if (currentSpeakerVolume != requiredSpeakerVolume) {
    currentSpeakerVolume = requiredSpeakerVolume;
    setVolume(currentSpeakerVolume);
  }
#endif

// TODO merge these 2 branches
#if defined(PCBSKY9X)
  if (Eeprom32_process_state != E32_IDLE)
    ee32_process();
  else if (TIME_TO_WRITE())
    eeCheck();
#elif defined(CPUARM)
  if (TIME_TO_WRITE())
    eeCheck();
#else
  if (!eeprom_buffer_size) {
    if (theFile.isWriting())
      theFile.nextWriteStep();
    else if (TIME_TO_WRITE())
      eeCheck();
  }
#endif

#if defined(SDCARD)
  sdMountPoll();
  writeLogs();
#endif

#if defined(CPUARM) && defined(SIMU)
  checkTrims();
#endif

#if defined(CPUARM)
  uint8_t evt = getEvent(false);
#else
  uint8_t evt = getEvent();
  evt = checkTrim(evt);
#endif

  if (evt && (g_eeGeneral.backlightMode & e_backlight_mode_keys)) backlightOn(); // on keypress turn the light on

  checkBacklight();

#if defined(CPUARM) && defined(FRSKY)
  telemetryPoll10ms();
#endif

  lcd_clear();
  const char *warn = s_warning;
  g_menuStack[g_menuStackPtr](warn ? 0 : evt);
  if (warn) displayWarning(evt);
  drawStatusLine();
  lcdRefresh();

  if (SLAVE_MODE()) {
    JACK_PPM_OUT();
  }
  else {
    JACK_PPM_IN();
  }

  static uint8_t counter = 0;
  if (g_menuStack[g_menuStackPtr] == menuGeneralDiagAna) {
    g_vbat100mV = 0;
    counter = 0;
  }
  if (counter-- == 0) {
    counter = 10;
#if defined(PCBX9D)
    int32_t instant_vbat = anaIn(TX_VOLTAGE);
    instant_vbat = ( instant_vbat + instant_vbat*(g_eeGeneral.vBatCalib)/128 ) * 15 ;
    instant_vbat /= 256  ;
#elif defined(PCBSKY9X)
    int32_t instant_vbat = anaIn(TX_VOLTAGE);
    instant_vbat = ( instant_vbat + instant_vbat*(g_eeGeneral.vBatCalib)/128 ) * 4191 ;
    instant_vbat /= 55296  ;
#elif defined(PCBGRUVIN9X)
    uint16_t instant_vbat = anaIn(TX_VOLTAGE);
    instant_vbat = ((uint32_t)instant_vbat*1112 + (int32_t)instant_vbat*g_eeGeneral.vBatCalib + (BandGap<<2)) / (BandGap<<3);
#else
    uint16_t instant_vbat = anaIn(TX_VOLTAGE);
    instant_vbat = (instant_vbat*16 + instant_vbat*g_eeGeneral.vBatCalib/8) / BandGap;
#endif

    static uint8_t  s_batCheck;
    static uint16_t s_batSum;

#if defined(VOICE)
    s_batCheck += 8;
#else
    s_batCheck += 32;
#endif

    s_batSum += instant_vbat;

    if (g_vbat100mV == 0) {
      g_vbat100mV = instant_vbat;
      s_batSum = 0;
      s_batCheck = 0;
    }
#if defined(VOICE)
    else if (!(s_batCheck & 0x3f)) {
#else
    else if (s_batCheck == 0) {
#endif
      g_vbat100mV = s_batSum / 8;
      s_batSum = 0;
#if defined(VOICE)
      if (s_batCheck != 0) {
        // no alarms
      }
      else
#endif
      if (g_vbat100mV <= g_eeGeneral.vBatWarn && g_vbat100mV>50) {
        AUDIO_TX_BATTERY_LOW();
      }
#if defined(CPUARM)
      else if (g_eeGeneral.temperatureWarn && getTemperature() >= g_eeGeneral.temperatureWarn) {
        AUDIO_TX_TEMP_HIGH();
      }
#endif
#if defined(PCBSKY9X)
      else if (g_eeGeneral.mAhWarn && (g_eeGeneral.mAhUsed + Current_used * (488 + g_eeGeneral.currentCalib)/8192/36) / 500 >= g_eeGeneral.mAhWarn) {
        AUDIO_TX_MAH_HIGH();
      }
#endif
    }
  }
}

int16_t g_ppmIns[8];
uint8_t ppmInState = 0; //0=unsync 1..8= wait for value i-1

#if !defined(SIMU) && !defined(CPUARM)

volatile uint8_t g_tmr16KHz; //continuous timer 16ms (16MHz/1024/256) -- 8-bit counter overflow
ISR(TIMER_16KHZ_VECT, ISR_NOBLOCK)
{
  g_tmr16KHz++; // gruvin: Not 16KHz. Overflows occur at 61.035Hz (1/256th of 15.625KHz)
                // to give *16.384ms* intervals. Kind of matters for accuracy elsewhere. ;)
                // g_tmr16KHz is used to software-construct a 16-bit timer
                // from TIMER-0 (8-bit). See getTmr16KHz, below.
}

uint16_t getTmr16KHz()
{
  while(1){
    uint8_t hb  = g_tmr16KHz;
    uint8_t lb  = COUNTER_16KHZ;
    if(hb-g_tmr16KHz==0) return (hb<<8)|lb;
  }
}

ISR(TIMER_10MS_VECT, ISR_NOBLOCK) // 10ms timer
{
  cli();
  PAUSE_10MS_INTERRUPT();
  sei();

#if defined(PCBGRUVIN9X)
  static uint8_t accuracyWarble = 4; // because 16M / 1024 / 100 = 156.25. So bump every 4.
  uint8_t bump = (!(accuracyWarble++ & 0x03)) ? 157 : 156;
  OCR2A += bump;
#elif defined(AUDIO) || defined(VOICE)
  OCR0 += 2; // interrupt every 128us
#else
  static uint8_t accuracyWarble = 4; // because 16M / 1024 / 100 = 156.25. So bump every 4.
  uint8_t bump = (!(accuracyWarble++ & 0x03)) ? 157 : 156;
  OCR0 += bump;
#endif

#if defined(PCBSTD) && (defined(AUDIO) || defined(VOICE))

#if defined(AUDIO)
  AUDIO_DRIVER();
#endif

#if defined(VOICE)
  VOICE_DRIVER();
#endif

  static uint8_t cnt10ms = 77; // execute 10ms code once every 78 ISRs

#if defined(FRSKY) || defined(MAVLINK) || defined(JETI)
  if (cnt10ms == 30) {
    if (!IS_DSM2_SERIAL_PROTOCOL(s_current_protocol))
      telemetryPoll10ms();
  }
#endif

  if (cnt10ms-- == 0) { // BEGIN { ... every 10ms ... }
    // Begin 10ms event
    cnt10ms = 77;
#endif

    AUDIO_HEARTBEAT();

#if defined(HAPTIC)
    HAPTIC_HEARTBEAT();
#endif

    per10ms();

#if defined(PCBSTD) && (defined(AUDIO) || defined(VOICE))
  } // end 10ms event
#endif

  cli();
  RESUME_10MS_INTERRUPT();
  sei();
}

// Timer3 used for PPM_IN pulse width capture. Counter running at 16MHz / 8 = 2MHz
// equating to one count every half millisecond. (2 counts = 1ms). Control channel
// count delta values thus can range from about 1600 to 4400 counts (800us to 2200us),
// corresponding to a PPM signal in the range 0.8ms to 2.2ms (1.5ms at center).
// (The timer is free-running and is thus not reset to zero at each capture interval.)
ISR(TIMER3_CAPT_vect) // G: High frequency noise can cause stack overflo with ISR_NOBLOCK
{
  static uint16_t lastCapt;

  uint16_t capture=ICR3;

  // Prevent rentrance for this IRQ only
  PAUSE_PPMIN_INTERRUPT();
  sei(); // enable other interrupts

  uint16_t val = (capture - lastCapt) / 2;

  // G: We process g_ppmIns immediately here, to make servo movement as smooth as possible
  //    while under trainee control
  if (val>4000 && val < 16000) // G: Prioritize reset pulse. (Needed when less than 8 incoming pulses)
    ppmInState = 1; // triggered
  else
  {
    if (ppmInState && ppmInState<=8)
    {
      if (val>800 && val<2200) // if valid pulse-width range
      {
        g_ppmIns[ppmInState++ - 1] =
          (int16_t)(val - 1500)*(g_eeGeneral.PPM_Multiplier+10)/10; //+-500 != 512, but close enough.
      }
      else
        ppmInState = 0; // not triggered
    }
  }

  lastCapt = capture;

  cli(); // disable other interrupts for stack pops before this function's RETI
  RESUME_PPMIN_INTERRUPT();
}

/*
// gruvin: Fuse declarations work if we use the .elf file for AVR Studio (v4)
// instead of the Intel .hex files.  They should also work with AVRDUDE v5.10
// (reading from the .hex file), since a bug relating to Intel HEX file record
// interpretation was fixed. However, I leave these commented out, just in case
// it causes trouble for others.
#if defined (PCBGRUVIN9X)
// See fuses_2561.txt
  FUSES =
  {
    // BOD=4.3V, WDT OFF (enabled in code), Boot Flash 4096 bytes at 0x1F000,
    // JTAG and OCD enabled, EESAVE enabled, BOOTRST/CKDIV8/CKOUT disabled,
    // Full swing Xtal oscillator. Start-up 16K clks + 0ms. BOD enabled.
    0xD7, // .low
    0x11, // .high
    0xFC  // .extended
  };
#else
  FUSES =
  {
    // G: Changed 2011-07-04 to include EESAVE. Tested OK on stock 9X
    0x1F, // LFUSE
    0x11, // HFUSE
    0xFF  // EFUSE
  };
#endif
*/

#endif

/*
   USART0 Transmit Data Register Emtpy ISR
   Used to transmit FrSky data packets and DSM2 protocol
*/

// TODO serial_arm and serial_avr

#if defined(FRSKY) && !defined(CPUARM)
// TODO in frsky.cpp?
FORCEINLINE void FRSKY_USART0_vect()
{
  if (frskyTxBufferCount > 0) {
    UDR0 = frskyTxBuffer[--frskyTxBufferCount];
  }
  else {
    UCSR0B &= ~(1 << UDRIE0); // disable UDRE0 interrupt
  }
}
#endif

#if defined(DSM2_SERIAL) && !defined(CPUARM)
FORCEINLINE void DSM2_USART0_vect()
{
  UDR0 = *((uint16_t*)pulses2MHzRPtr); // transmit next byte

  pulses2MHzRPtr += sizeof(uint16_t);

  if (pulses2MHzRPtr == pulses2MHzWPtr) { // if reached end of DSM2 data buffer ...
    UCSR0B &= ~(1 << UDRIE0); // disable UDRE0 interrupt
  }
}
#endif

#if !defined(SIMU) && !defined(CPUARM)
#if defined (FRSKY) || defined(DSM2_SERIAL)
ISR(USART0_UDRE_vect)
{
#if defined(FRSKY) && defined(DSM2_SERIAL)
  if (IS_DSM2_PROTOCOL(g_model.protocol)) { // TODO not s_current_protocol?
    DSM2_USART0_vect();
  }
  else {
    FRSKY_USART0_vect();
  }
#elif defined (FRSKY)
  FRSKY_USART0_vect();
#else
  DSM2_USART0_vect();
#endif
}
#endif
#endif

void instantTrim()
{
  evalSticks(e_perout_mode_notrainer);

  for (uint8_t i=0; i<NUM_STICKS; i++) {
    if (i!=THR_STICK) {
      // don't instant trim the throttle stick
      uint8_t trim_phase = getTrimFlightPhase(s_perout_flight_phase, i);
      int16_t trim = limit((int16_t)TRIM_EXTENDED_MIN, (int16_t)((anas[i] + trims[i]) / 2), (int16_t)TRIM_EXTENDED_MAX);
      setTrimValue(trim_phase, i, trim);
    }
  }

  STORE_MODELVARS;
  AUDIO_WARNING2();
}

void moveTrimsToOffsets() // copy state of 3 primary to subtrim
{
  int16_t zeros[NUM_CHNOUT];

  pauseMixerCalculations();

  perOut(e_perout_mode_noinput, 0); // do output loop - zero input sticks and trims
  for (uint8_t i=0; i<NUM_CHNOUT; i++) {
    zeros[i] = applyLimits(i, chans[i]);
  }

  perOut(e_perout_mode_nosticks+e_perout_mode_notrainer, 0); // do output loop - only trims

  for (uint8_t i=0; i<NUM_CHNOUT; i++) {
    int16_t output = applyLimits(i, chans[i]) - zeros[i];
    int16_t v = g_model.limitData[i].offset;
    if (g_model.limitData[i].revert) output = -output;
    v += output;
    // TODO * 125 / 128 ?
    g_model.limitData[i].offset = limit((int16_t)-1000, (int16_t)v, (int16_t)1000); // make sure the offset doesn't go haywire
  }

  // reset all trims, except throttle (if throttle trim)
  for (uint8_t i=0; i<NUM_STICKS; i++) {
    if (i!=THR_STICK || !g_model.thrTrim) {
      int16_t original_trim = getTrimValue(s_perout_flight_phase, i);
      for (uint8_t phase=0; phase<MAX_PHASES; phase++) {
        int16_t trim = getRawTrimValue(phase, i);
        if (trim <= TRIM_EXTENDED_MAX)
          setTrimValue(phase, i, trim - original_trim);
      }
    }
  }

  resumeMixerCalculations();

  STORE_MODELVARS;
  AUDIO_WARNING2();
}

#if defined(CPUARM) || defined(PCBGRUVIN9X)
void saveTimers()
{
  for (uint8_t i=0; i<MAX_TIMERS; i++) {
    if (g_model.timers[i].remanent) {
      if (g_model.timers[i].value != s_timerVal[i]) {
        g_model.timers[i].value = s_timerVal[i];
        eeDirty(EE_MODEL);
      }
    }
  }
}
#endif

#if defined(ROTARY_ENCODERS)
volatile rotenc_t g_rotenc[ROTARY_ENCODERS] = {0};
#elif defined(ROTARY_ENCODER_NAVIGATION)
volatile rotenc_t g_rotenc[1] = {0};
#endif

#ifndef SIMU

#if defined(CPUARM)
void stack_paint()
{
  for (uint16_t i=0; i<MENUS_STACK_SIZE; i++)
    menusStack[i] = 0x55555555;
  for (uint16_t i=0; i<MIXER_STACK_SIZE; i++)
    mixerStack[i] = 0x55555555;
  for (uint16_t i=0; i<AUDIO_STACK_SIZE; i++)
    audioStack[i] = 0x55555555;
}

uint16_t stack_free(uint8_t tid)
{
  OS_STK *stack;
  uint16_t size;

  switch(tid) {
    case 1:
      stack = mixerStack;
      size = MIXER_STACK_SIZE;
      break;
    case 2:
      stack = audioStack;
      size = AUDIO_STACK_SIZE;
      break;
    default:
      stack = menusStack;
      size = MENUS_STACK_SIZE;
      break;
  }

  uint16_t i=0;
  for (; i<size; i++)
    if (stack[i] != 0x55555555)
      return i;
  return i;
}

uint16_t mixer_stack_free()
{
  for (uint16_t i=0; i<MENUS_STACK_SIZE; i++)
    if (menusStack[i] != 0x55555555)
      return i;
  return MENUS_STACK_SIZE;
}

#else
extern unsigned char __bss_end ;
#define STACKPTR     _SFR_IO16(0x3D)
void stack_paint()
{
  // Init Stack while interrupts are disabled
  unsigned char *p ;
  unsigned char *q ;

  p = (unsigned char *) STACKPTR ;
  q = &__bss_end ;
  p -= 2 ;
  while ( p > q )
    *p-- = 0x55 ;
}

uint16_t stack_free()
{
  unsigned char *p ;

  p = &__bss_end + 1 ;
  while ( *p == 0x55 )
  {
    p+= 1 ;
  }
  return p - &__bss_end ;
}
#endif

#if defined(PCBGRUVIN9X)
#define UNEXPECTED_SHUTDOWN() ((mcusr & (1 << WDRF)) || g_eeGeneral.unexpectedShutdown)
#define OPEN9X_INIT_ARGS const uint8_t mcusr
#elif defined(PCBSTD)
#define UNEXPECTED_SHUTDOWN() (mcusr & (1 << WDRF))
#define OPEN9X_INIT_ARGS const uint8_t mcusr
#else
#define UNEXPECTED_SHUTDOWN() (g_eeGeneral.unexpectedShutdown)
#define OPEN9X_INIT_ARGS
#endif

inline void open9xInit(OPEN9X_INIT_ARGS)
{
#if defined(PCBX9D)
  BACKLIGHT_ON();
  CoTickDelay(10);  //20ms
  Splash();
#endif

  eeReadAll();

#if defined(VOICE)
  setVolume(g_eeGeneral.speakerVolume+VOLUME_LEVEL_DEF);
#endif

#if defined(CPUARM)
  setBacklight(g_eeGeneral.backlightBright);
#endif

#if defined(PCBSKY9X)
  // Set ADC gains here
  setSticksGain(g_eeGeneral.sticksGain);
#endif

#if defined(BLUETOOTH)
  btInit();
#endif

#if defined(RTCLOCK)
  rtcInit();
#endif

  if (g_eeGeneral.backlightMode != e_backlight_mode_off) backlightOn(); // on Tx start turn the light on

  if (UNEXPECTED_SHUTDOWN()) {
    unexpectedShutdown = 1;
#if defined(PCBSKY9X)
    eeLoadModel(g_eeGeneral.currModel);
#endif
  }
  else {
    doSplash();

#if defined(PCBSKY9X) && defined(SDCARD)
    for (int i=0; i<500 && !Card_initialized; i++) {
      CoTickDelay(1);  // 2ms
    }
#endif

#if defined(PCBSKY9X)
    eeLoadModel(g_eeGeneral.currModel);
#endif

    checkAlarm();
    checkAll();
  }

#if defined(CPUARM) || defined(PCBGRUVIN9X)
  if (!g_eeGeneral.unexpectedShutdown) {
    g_eeGeneral.unexpectedShutdown = 1;
    eeDirty(EE_GENERAL);
  }
#endif
  
  lcdSetContrast();
  backlightOn();

#if defined(CPUARM)
  start_ppm_capture();
  // TODO inside startPulses?
  s_pulses_paused = false;
  // TODO startPulses should be started after the first doMixerCalculations()
#endif

#if defined(PCBX9D)
  // TODO remove this line, it only allows the Mixer to run (until pulses are there!)
  s_current_protocol = PROTO_PPM;
#endif

#if !defined(CPUARM)
  doMixerCalculations();
#endif

  startPulses();

  wdt_enable(WDTO_500MS);
}

#if defined(CPUARM)
void mixerTask(void * pdata)
{
  s_pulses_paused = true;

  while(1) {

    if (!s_pulses_paused) {
      uint16_t t0 = getTmr2MHz();

      if (s_current_protocol < PROTO_NONE) {
        CoEnterMutexSection(mixerMutex);
        bool tick10ms = doMixerCalculations();
        CoLeaveMutexSection(mixerMutex);
        if (tick10ms) checkTrims();
      }

      if (heartbeat == HEART_TIMER_PULSES+HEART_TIMER10ms) {
        wdt_reset();
        heartbeat = 0;
      }

      t0 = getTmr2MHz() - t0;
      if (t0 > maxMixerDuration) maxMixerDuration = t0 ;
    }

    CoTickDelay(1);  // 2ms for now
  }
}

void menusTask(void * pdata)
{
  open9xInit();

  while (pwrCheck() != e_power_off) {
    perMain();
    for (uint8_t i=0; i<5; i++) {
      usbMassStorage();
      CoTickDelay(1);  // 5*2ms for now
    }
  }

#if defined(SDCARD)
  closeLogs();
#endif

  SysTick->CTRL = 0; // turn off systick

#if defined(HAPTIC)
  hapticOff();
#endif

  lcd_clear();

  displayPopup(STR_SHUTDOWN);

  saveTimers();

#if defined(PCBSKY9X)
  uint32_t mAhUsed = g_eeGeneral.mAhUsed + Current_used * (488 + g_eeGeneral.currentCalib) / 8192 / 36;
  if (g_eeGeneral.mAhUsed != mAhUsed)
    g_eeGeneral.mAhUsed = mAhUsed;
#endif

  if (sessionTimer > 0)
    g_eeGeneral.globalTimer += sessionTimer;

  g_eeGeneral.unexpectedShutdown = 0;

  eeDirty(EE_GENERAL);
  eeCheck(true);

  lcd_clear();
  lcdRefresh();
  lcdSetRefVolt(0);

  pwrOff(); // Only turn power off if necessary
}

extern void audioTimerHandle(void);
extern void audioTask(void* pdata);

#endif

int main(void)
{
  // G: The WDT remains active after a WDT reset -- at maximum clock speed. So it's
  // important to disable it before commencing with system initialisation (or
  // we could put a bunch more wdt_reset()s in. But I don't like that approach
  // during boot up.)
#if defined(PCBGRUVIN9X)
  uint8_t mcusr = MCUSR; // save the WDT (etc) flags
  MCUSR = 0; // must be zeroed before disabling the WDT
#elif defined(PCBSTD)
  uint8_t mcusr = MCUCSR;
  MCUCSR = 0;
#endif

  wdt_disable();

  boardInit();

  lcdInit();

  stack_paint();

  g_menuStack[0] = menuMainView;
#if !defined(READONLY)
  g_menuStack[1] = menuModelSelect;
#endif

  lcdSetRefVolt(25);

  sei(); // interrupts needed for FRSKY_Init and eeReadAll.

#if defined(FRSKY) && !defined(DSM2_SERIAL)
  FRSKY_Init();
#endif

#if defined(DSM2_SERIAL) && !defined(FRSKY)
  DSM2_Init();
#endif

#ifdef JETI
  JETI_Init();
#endif

#ifdef ARDUPILOT
  ARDUPILOT_Init();
#endif

#ifdef NMEA
  NMEA_Init();
#endif

#ifdef MAVLINK
  MAVLINK_Init();
#endif

#ifdef MENU_ROTARY_SW
  init_rotary_sw();
#endif

#if !defined(CPUARM)
  open9xInit(mcusr);
#endif

#if defined(CPUARM)
  if (BOOTLOADER_REQUEST()) {
    pwrOff(); // Only turn power off if necessary

#if defined(HAPTIC)
    hapticOff();
#endif

    g_eeGeneral.optrexDisplay = 1;
    lcd_clear();
    lcdRefresh();

    g_eeGeneral.optrexDisplay = 0;
    g_eeGeneral.backlightBright = 0;
    g_eeGeneral.contrast = 25;

    BACKLIGHT_ON();

    lcd_clear();
    lcd_putcAtt( 48, 24, 'U', DBLSIZE ) ;
    lcd_putcAtt( 60, 24, 'S', DBLSIZE ) ;
    lcd_putcAtt( 72, 24, 'B', DBLSIZE ) ;
    lcdRefresh() ;

    usbBootloader();
  }

  CoInitOS();

#if defined(PCBSKY9X) && defined(DEBUG)
  debugTaskId = CoCreateTaskEx(debugTask, NULL, 10, &debugStack[DEBUG_STACK_SIZE-1], DEBUG_STACK_SIZE, 1, false);
#endif

#if defined(BLUETOOTH)
  btTaskId = CoCreateTask(btTask, NULL, 15, &btStack[BT_STACK_SIZE-1], BT_STACK_SIZE);
#endif

  mixerTaskId = CoCreateTask(mixerTask, NULL, 5, &mixerStack[MIXER_STACK_SIZE-1], MIXER_STACK_SIZE);
  menusTaskId = CoCreateTask(menusTask, NULL, 10, &menusStack[MENUS_STACK_SIZE-1], MENUS_STACK_SIZE);

  audioFlag = CoCreateFlag(true, false);          // Auto-reset, start FALSE
  audioTimer = CoCreateTmr(TMR_TYPE_ONE_SHOT, 1000/(1000/CFG_SYSTICK_FREQ), 1000/(1000/CFG_SYSTICK_FREQ), audioTimerHandle);
  audioTaskId = CoCreateTask(audioTask, NULL, 7, &audioStack[AUDIO_STACK_SIZE-1], AUDIO_STACK_SIZE);

  audioMutex = CoCreateMutex();
  mixerMutex = CoCreateMutex();

  CoStartOS();
#else
#if defined(PCBGRUVIN9X)
  uint8_t shutdown_state = 0;
#endif

  while(1) {
#if defined(PCBGRUVIN9X)
    if ((shutdown_state=pwrCheck()) > e_power_trainer)
      break;
#endif

    perMain();

    if (heartbeat == HEART_TIMER_PULSES+HEART_TIMER10ms) {
      wdt_reset();
      heartbeat = 0;
    }
  }
#endif

#if defined(PCBGRUVIN9X)
  // Time to switch off
  lcd_clear() ;
  displayPopup(STR_SHUTDOWN);
  saveTimers();
  g_eeGeneral.unexpectedShutdown=0;
  eeDirty(EE_GENERAL);
  eeCheck(true);
#if defined(SDCARD)
  closeLogs();
#endif
  lcd_clear() ;
  lcdRefresh() ;
  pwrOff();            // Only turn power off if necessary
  wdt_disable();
  while(1); // never return from main() - there is no code to return back, if any delays occurs in physical power it does dead loop.
#endif
}
#endif // !SIMU

