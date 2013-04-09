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

#include "opentx.h"

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

#endif // defined(CPUARM)

#if defined(SPLASH)
const pm_uchar splashdata[] PROGMEM = { 'S','P','S',0,
#if defined(PCBTARANIS)
#include "splash_taranis.lbm"
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

#if defined(PCBTARANIS) && defined(SDCARD)
uint8_t modelBitmap[MODEL_BITMAP_SIZE];
pm_char * modelBitmapLoaded = NULL;
void loadModelBitmap()
{
  char lfn[] = BITMAPS_PATH "/xxxxxxxxxx.bmp";
  strncpy(lfn+sizeof(BITMAPS_PATH), g_model.bitmap, sizeof(g_model.bitmap));
  lfn[sizeof(BITMAPS_PATH)+sizeof(g_model.bitmap)] = '\0';
  strcat(lfn+sizeof(BITMAPS_PATH), BITMAPS_EXT);
  modelBitmapLoaded = bmpLoad(modelBitmap, lfn, MODEL_BITMAP_WIDTH, MODEL_BITMAP_HEIGHT);
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

#if defined(FRSKY)
  if (!IS_DSM2_SERIAL_PROTOCOL(s_current_protocol))
    telemetryInterrupt10ms();
#endif

  // These moved here from perOut() to improve beep trigger reliability.
#if defined(PWM_BACKLIGHT)
  if ((g_tmr10ms&0x03) == 0x00)
    backlightFade(); // increment or decrement brightness until target brightness is reached
#endif

#if !defined(AUDIO)
  if (mixWarning & 1) if(((g_tmr10ms&0xFF)==  0)) AUDIO_MIX_WARNING(1);
  if (mixWarning & 2) if(((g_tmr10ms&0xFF)== 64) || ((g_tmr10ms&0xFF)== 72)) AUDIO_MIX_WARNING(2);
  if (mixWarning & 4) if(((g_tmr10ms&0xFF)==128) || ((g_tmr10ms&0xFF)==136) || ((g_tmr10ms&0xFF)==144)) AUDIO_MIX_WARNING(3);
#endif

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
  if ((size & 1) == 0) {
    result.points = (size / 2) + 1;
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

#if defined(PCBTARANIS)
  g_eeGeneral.vBatWarn = 65;
  g_eeGeneral.vBatMin = -30;
  g_eeGeneral.vBatMax = -40;
#else
  g_eeGeneral.vBatWarn = 90;
#endif

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

#if !defined(TEMPLATES)
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

#if defined(PXX) && defined(CPUARM)
void checkModelIdUnique(uint8_t id)
{
  for (uint8_t i=0; i<MAX_MODELS; i++) {
    if (i != id && g_model.modelId!=0 && g_model.modelId == modelIds[i]) {
      s_warning = PSTR("Model ID already used");
      s_warning_type = WARNING_TYPE_ASTERISK;
    }
  }
}
#endif

void modelDefault(uint8_t id)
{
  memset(&g_model, 0, sizeof(g_model));
  applyDefaultTemplate();

#if defined(PXX) && defined(CPUARM)
  modelIds[id] = g_model.modelId = id+1;
  checkModelIdUnique(id);
#endif

#if defined(PCBTARANIS)
  g_model.frsky.channels[0].ratio = 132;
#endif
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

/* currently not needed
// maybe used in future?
uint16_t divu100(uint32_t A) {
    return (((A * 0x47AF) >> 16) + A) >> 7;
}
*/


// #define EXTENDED_EXPO
// increases range of expo curve but costs about 82 bytes flash

// expo-funktion:
// ---------------
// kmplot
// f(x,k)=exp(ln(x)*k/10) ;P[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]
// f(x,k)=x*x*x*k/10 + x*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]
// f(x,k)=x*x*k/10 + x*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]
// f(x,k)=1+(x-1)*(x-1)*(x-1)*k/10 + (x-1)*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]
// don't know what this above should be, just confusing in my opinion,

// here is the real explanation
// actually the real formula is  
/*
 f(x) = exp( ln(x) * 10^k)  
 if it is 10^k or e^k or 2^k etc. just defines the max distortion of the expo curve; I think 10 is useful
 this gives values from 0 to 1 for x and output; k must be between -1 and +1
 we do not like to calculate with floating point. Therefore we rescale for x from 0 to 1024 and for k from -100 to +100
 f(x) = 1024 * ( e^( ln(x/1024) * 10^(k/100) ) )
 This would be really hard to be calculated by such a microcontroller
 Therefore Thomas Husterer compared a few usual function something like x^3, x^4*something, which look similar
 Actually the formula 
 f(x) = k*x^3+x*(1-k) 
 gives a similar form and should have even advantages compared to a original exp curve.
 This function again expect x from 0 to 1 and k only from 0 to 1
 Therefore rescaling is needed like before:
 f(x) = 1024* ((k/100)*(x/1024)^3 + (x/1024)*(100-k)/100)
 some mathematical tricks 
 f(x) = (k*x*x*x/(1024*1024) + x*(100-k)) / 100
 for better rounding results we add the 50
 f(x) = (k*x*x*x/(1024*1024) + x*(100-k) + 50) / 100
 
 because we now understand the formula, we can optimize it further
 --> calc100to256(k) --> eliminates /100 by replacing with /256 which is just a simple shift right 8
 k is now between 0 and 256
 f(x) = (k*x*x*x/(1024*1024) + x*(256-k) + 128) / 256
 */
 
// input parameters; 
//  x 0 to 1024;
//  k 0 to 100;
// output between 0 and 1024
uint16_t expou(uint16_t x, uint16_t k)
{
#ifdef EXTENDED_EXPO  
  bool extended;
  if (k>80) {
    extended=true;
  }
  else {
    k+=(k>>2);  // use bigger values before extend, because the effect is anyway very very low
    extended=false;
  } // endif k > 50
#endif  

  k=calc100to256(k);    
  
  uint32_t value = (uint32_t) x*x;
  value *= (uint32_t)k;
  value >>= 8;
  value *= (uint32_t)x;

#ifdef EXTENDED_EXPO
  if (extended) {  // for higher values do more multiplications to get a stronger expo curve
    value>>=16;
    value*=(uint32_t)x;
    value>>=4;
    value*=(uint32_t)x;
  } //endif extend
#endif
  
  value >>= 12;
  value += (uint32_t)(256-k)*x+128;

  return value>>8;
}

int16_t expo(int16_t x, int16_t k)
{
  if (k == 0) return x;
  int16_t y;
  bool neg  = x < 0;

  if (neg) x = -x;
  if (k<0) {
    y = RESXu-expou(RESXu-x, -k);
  }
  else {
    y = expou(x, k);
  }
  return neg? -y : y;
}

// isn't the following useless? wouldn't be called even if EXTENDED_EXPO is defined
/*
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
*/

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
        int16_t weight = GET_GVAR(ed.weight, 0, 100, s_perout_flight_phase);
        weight = calc100to256(weight);
        v = ((int32_t)v * weight) >> 8;
        anas[cur_chn] = v;
      }
    }
  }
}

#if !defined(CPUARM)

// #define CORRECT_NEGATIVE_SHIFTS
// open.20.fsguruh; shift right operations do the rounding different for negative values compared to positive values
// so all negative divisions round always further down, which give absolute values bigger compared to a usual division
// this is noticable on the display, because instead of -100.0 -99.9 is shown; While in praxis I doublt somebody will notice a 
// difference this is more a mental thing. Maybe people are distracted, because the easy calculations are obviously wrong
// this define would correct this, but costs 34 bytes code for stock version

// currently we set this to active always, because it might cause a fault about 1% compared positive and negative values
// is done now in makefile

int16_t calc100to256_16Bits(int16_t x) // return x*2.56
{
    // y = 2*x + x/2 +x/16-x/512-x/2048
    // 512 and 2048 are out of scope from int8 input --> forget it
#ifdef CORRECT_NEGATIVE_SHIFTS
    int16_t res=(int16_t)x<<1;
    //int8_t  sign=(uint8_t) x>>7;
    int8_t sign=(x<0?1:0);
    
    x-=sign;    
    res+=(x>>1);
    res+=sign;
    res+=(x>>4);
    res+=sign;
    return res;
#else    
    return ((int16_t)x<<1)+(x>>1)+(x>>4);
#endif
}

int16_t calc100to256(int8_t x) // return x*2.56
{
  return calc100to256_16Bits(x);
}

int16_t calc100toRESX_16Bits(int16_t x) // return x*10.24
{
#ifdef CORRECT_NEGATIVE_SHIFTS
  int16_t res= ((int16_t)x*41)>>2;
  int8_t sign=(x<0?1:0);
  //int8_t  sign=(uint8_t) x>>7;  
  x-=sign;
  res-=(x>>6);
  res-=sign;
  return res;
#else
  // return (int16_t)x*10 + x/4 - x/64;
  return ((x*41)>>2) - (x>>6);
#endif
}

int16_t calc100toRESX(int8_t x) // return x*10.24
{
  return calc100toRESX_16Bits(x);
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

int8_t calcRESXto100(int16_t x)
{
  return (x*25) >> 8;
}

#endif


// #define PREVENT_ARITHMETIC_OVERFLOW
// because of optimizations the reserves before overruns occurs is only the half
// this defines enables some checks the greatly improves this situation
// It should nearly prevent all overruns (is still a chance for it, but quite low)
// negative side is code cost 96 bytes flash

// we do it now half way, only in applyLimits, which costs currently 50bytes
// according opinion poll this topic is currently not very important
// the change below improves already the situation 
// the check inside mixer would slow down mix a little bit and costs additionally flash
// also the check inside mixer still is not bulletproof, there may be still situations a overflow could occur
// a bulletproof implementation would take about additional 100bytes flash
// therefore with go with this compromize, interested people could activate this define

// @@@2 open.20.fsguruh ; 
// channel = channelnumber -1; 
// value = outputvalue with 100 mulitplied usual range -102400 to 102400; output -1024 to 1024
// changed rescaling from *100 to *256 to optimize performance
// rescaled from -262144 to 262144
int16_t applyLimits(uint8_t channel, int32_t value)
{
  LimitData * lim = limitaddress(channel);
  int16_t ofs   = calc1000toRESX(lim->offset);   // multiply to 1.24 to get range (-1024..1024)
  int16_t lim_p = calc100toRESX(lim->max + 100);
  int16_t lim_n = calc100toRESX(lim->min - 100); //multiply by 10.24 to get same range (-1024..1024)

  if (ofs > lim_p) ofs = lim_p;
  if (ofs < lim_n) ofs = lim_n;

  // because the rescaling optimization would reduce the calculation reserve we activate this for all builds
  // it increases the calculation reserve from factor 20,25x to 32x, which it slightly better as original
  // without it we would only have 16x which is slightly worse as original, we should not do this

  // thanks to gbirkus, he motivated this change, which greatly reduces overruns 
  // unfortunately the constants and 32bit compares generates about 50 bytes codes; didn't find a way to get it down.
  value = limit(int32_t(-RESXl*256), value, int32_t(RESXl*256));  // saves 2 bytes compared to other solutions up to now
  
#if defined(PPM_LIMITS_SYMETRICAL)
  if (value) {
    int16_t tmp;
    if (lim->symetrical)
      tmp = (value > 0) ? (lim_p) : (-lim_n);
    else
      tmp = (value > 0) ? (lim_p - ofs) : (-lim_n + ofs);
    value = (int32_t) value * tmp;   //  div by 1024*256 -> output = -1024..1024
#else    
  if (value) {
    int16_t tmp = (value > 0) ? (lim_p - ofs) : (-lim_n + ofs);
    value = (int32_t) value * tmp;   //  div by 1024*256 -> output = -1024..1024
#endif    
    
#ifdef CORRECT_NEGATIVE_SHIFTS
    int8_t sign = (value<0?1:0);
    value -= sign;
    tmp = value>>16;   // that's quite tricky: the shiftright 16 operation is assmbled just with addressmove; just forget the two least significant bytes;
    tmp >>= 2;   // now one simple shift right for two bytes does the rest
    tmp += sign;
#else
    tmp = value>>16;   // that's quite tricky: the shiftright 16 operation is assmbled just with addressmove; just forget the two least significant bytes;
    tmp >>= 2;   // now one simple shift right for two bytes does the rest
#endif
    
    ofs += tmp;  // ofs can to added directly because already recalculated,
  }

  if (ofs > lim_p) ofs = lim_p;
  if (ofs < lim_n) ofs = lim_n;  
  
  if (lim->revert) ofs = -ofs; // finally do the reverse.

  if (safetyCh[channel] != -128) // if safety channel available for channel check
    ofs = calc100toRESX(safetyCh[channel]);

  return ofs;
}

int16_t calibratedStick[NUM_STICKS+NUM_POTS];
int16_t channelOutputs[NUM_CHNOUT] = {0};
int16_t ex_chans[NUM_CHNOUT] = {0}; // Outputs (before LIMITS) of the last perMain;
#ifdef HELI
int16_t cyc_anas[3] = {0};
#endif

// TODO same naming convention than the putsMixerSource

bool __getSwitch(int8_t swtch);

getvalue_t getValue(uint8_t i)
{
  /*srcRaw is shifted +1!*/

  if (i<NUM_STICKS+NUM_POTS) return calibratedStick[i];
#if defined(PCBGRUVIN9X) || defined(PCBSKY9X)
  else if (i<NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS) return getRotaryEncoder(i-(NUM_STICKS+NUM_POTS));
#endif
  else if (i<MIXSRC_MAX) return 1024;
  else if (i<MIXSRC_CYC3)
#if defined(HELI)
    return cyc_anas[i+1-MIXSRC_CYC1];
#else
    return 0;
#endif
  else if (i<MIXSRC_TrimAil) return calc1000toRESX((int16_t)8 * getTrimValue(s_perout_flight_phase, i+1-MIXSRC_TrimRud));
#if defined(PCBTARANIS)
  else if (i<MIXSRC_SA) return (switchState(SW_SA0) ? -1024 : (switchState(SW_SA1) ? 0 : 1024));
  else if (i<MIXSRC_SB) return (switchState(SW_SB0) ? -1024 : (switchState(SW_SB1) ? 0 : 1024));
  else if (i<MIXSRC_SC) return (switchState(SW_SC0) ? -1024 : (switchState(SW_SC1) ? 0 : 1024));
  else if (i<MIXSRC_SD) return (switchState(SW_SD0) ? -1024 : (switchState(SW_SD1) ? 0 : 1024));
  else if (i<MIXSRC_SE) return (switchState(SW_SE0) ? -1024 : (switchState(SW_SE1) ? 0 : 1024));
  else if (i<MIXSRC_SF) return (switchState(SW_SF0) ? -1024 : 1024);
  else if (i<MIXSRC_SG) return (switchState(SW_SG0) ? -1024 : (switchState(SW_SG1) ? 0 : 1024));
  else if (i<MIXSRC_SH) return (switchState(SW_SH0) ? -1024 : 1024);
  else if (i<MIXSRC_LAST_CSW) return __getSwitch(SWSRC_SW1+i-MIXSRC_SH) ? 1024 : -1024;
#else
  else if (i<MIXSRC_3POS) return (switchState(SW_ID0) ? -1024 : (switchState(SW_ID1) ? 0 : 1024));
#if defined(EXTRA_3POS)
  else if (i<MIXSRC_3POS2) return (switchState(SW_ID3) ? -1024 : (switchState(SW_ID4) ? 0 : 1024));
#endif
  else if (i<MIXSRC_LAST_CSW) return __getSwitch(SWSRC_THR+i+1-MIXSRC_THR) ? 1024 : -1024;
#endif
  else if (i<MIXSRC_LAST_PPM) { int16_t x = g_ppmIns[i+1-MIXSRC_PPM1]; if (i<MIXSRC_PPM1+NUM_CAL_PPM) { x-= g_eeGeneral.trainer.calib[i+1-MIXSRC_PPM1]; } return x*2; }
  else if (i<MIXSRC_LAST_CH) return ex_chans[i+1-MIXSRC_CH1];
#if defined(GVARS)
  else if (i<MIXSRC_LAST_GVAR) return GVAR_VALUE(i+1-MIXSRC_GVAR1, getGVarFlightPhase(s_perout_flight_phase, i+1-MIXSRC_GVAR1));
#endif
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_TX_VOLTAGE) return g_vbat100mV;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_TM2) return s_timerVal[i+1-MIXSRC_FIRST_TELEM+1-TELEM_TM1];
#if defined(FRSKY)
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_RSSI_TX) return frskyData.rssi[1].value;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_RSSI_RX) return frskyData.rssi[0].value;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_A2) return frskyData.analog[i+1-MIXSRC_FIRST_TELEM+1-TELEM_A1].value;
#if defined(FRSKY_SPORT)
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_ALT) return frskyData.hub.baroAltitude;
#elif defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_ALT) return TELEMETRY_ALT_BP;
#endif
#if defined(FRSKY_HUB)
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_RPM) return frskyData.hub.rpm;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_FUEL) return frskyData.hub.fuelLevel;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_T1) return frskyData.hub.temperature1;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_T2) return frskyData.hub.temperature2;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_SPEED) return TELEMETRY_GPS_SPEED_BP;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_DIST) return frskyData.hub.gpsDistance;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_GPSALT) return TELEMETRY_GPS_ALT_BP;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_CELL) return (int16_t)frskyData.hub.minCellVolts * 2;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_CELLS_SUM) return (int16_t)frskyData.hub.cellsSum;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_VFAS) return (int16_t)frskyData.hub.vfas;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_CURRENT) return (int16_t)frskyData.hub.current;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_CONSUMPTION) return frskyData.currentConsumption;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_POWER) return frskyData.power;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_ACCx) return frskyData.hub.accelX;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_ACCy) return frskyData.hub.accelY;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_ACCz) return frskyData.hub.accelZ;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_HDG) return frskyData.hub.gpsCourse_bp;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_VSPD) return frskyData.hub.varioSpeed;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_MIN_A1) return frskyData.analog[0].min;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_MIN_A2) return frskyData.analog[1].min;
  else if (i<MIXSRC_FIRST_TELEM-1+TELEM_MAX_CURRENT) return *(((int16_t*)(&frskyData.hub.minAltitude))+i+1-(MIXSRC_FIRST_TELEM-1+TELEM_MAX_CURRENT));
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

  if (cs_idx == SWSRC_ON) {
    result = true;
  }
  else if (cs_idx <= MAX_PSWITCH) {
    result = switchState((EnumKeys)(SW_BASE+cs_idx-1));
  }
  else {
    cs_idx -= MAX_PSWITCH+1;

    GETSWITCH_RECURSIVE_TYPE mask = ((GETSWITCH_RECURSIVE_TYPE)1 << cs_idx);
    if (s_last_switch_used & mask) {
      result = (s_last_switch_value & mask);
    }
    else {
      s_last_switch_used |= mask;

      CustomSwData * cs = cswaddress(cs_idx);
      uint8_t s = cs->andsw;
      if (cs->func == CS_OFF || (s && !__getSwitch(s))) {
        result = false;
      }
      else if ((s=CS_STATE(cs->func)) == CS_VBOOL) {
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
      else {
        getvalue_t x = getValue(cs->v1-1);
        getvalue_t y;
        if (s == CS_VCOMP) {
          y = getValue(cs->v2-1);

          switch (cs->func) {
            case CS_EQUAL:
              result = (x==y);
              break;
            case CS_GREATER:
              result = (x>y);
              break;
            default:
              result = (x<y);
              break;
          }
        }
        else {
#if defined(FRSKY)
          // Telemetry
          if (cs->v1 >= MIXSRC_FIRST_TELEM) {
            if (frskyStreaming <= 0 && cs->v1 > MIXSRC_FIRST_TELEM-1+MAX_TIMERS)
              return swtch > 0 ? false : true;

            y = convertCswTelemValue(cs);

#if defined(FRSKY_HUB)
            if (s == CS_VOFS) {
              uint8_t idx = cs->v1-MIXSRC_FIRST_TELEM+1-TELEM_ALT;
              if (idx < THLD_MAX) {
                // Fill the threshold array
                barsThresholds[idx] = 128 + cs->v2;
              }
            }
#endif
          }
          else if (cs->v1 >= MIXSRC_GVAR1) {
            y = cs->v2;
          }
          else {
            y = calc100toRESX(cs->v2);
          }
#else
          if (cs->v1 >= MIXSRC_GVAR1) {
            y = cs->v2; // it's a GVAR or a Timer
          }
          else {
            y = calc100toRESX(cs->v2);
          }
#endif

          switch (cs->func) {
            case CS_VEQUAL:
#if defined(GVARS)
              if (cs->v1 >= MIXSRC_GVAR1 && cs->v1 <= MIXSRC_LAST_GVAR)
                result = (x==y);
              else
#endif
              result = (abs(x-y) < (1024 / STICK_TOLERANCE));
              break;
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
#endif

      if (result)
        s_last_switch_value |= ((GETSWITCH_RECURSIVE_TYPE)1<<cs_idx);
    }
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
  static tmr10ms_t s_move_last_time = 0;
  int8_t result = 0;

#if defined(PCBTARANIS)
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
        result = 1+(3*5)+2+3+(next!=0);
    }
  }
#else
  for (uint8_t i=MAX_PSWITCH; i>0; i--) {
    bool prev;
    swstate_t mask = 0;
    if (i <= 3) {
      prev = ((switches_states & 0x03) == (i-1));
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
      else {
        switches_states = (switches_states & 0xFC) | (i-1);
      }
    }
  }
#endif

  if ((tmr10ms_t)(get_tmr10ms() - s_move_last_time) > 10)
    result = 0;

  s_move_last_time = get_tmr10ms();
  return result;
}

#if defined(AUTOSOURCE)
int8_t getMovedSource()
{
  static tmr10ms_t s_move_last_time = 0;
  static int16_t sourcesStates[NUM_STICKS+NUM_POTS];

  int8_t result = 0;

  for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
    if (abs(calibratedStick[i] - sourcesStates[i]) > 512) {
      result = 1+i;
      break;
    }
  }

  bool recent = ((tmr10ms_t)(get_tmr10ms() - s_move_last_time) > 10);
  if (recent) {
    result = 0;
  }

  if (result || recent) {
    memcpy(sourcesStates, calibratedStick, sizeof(sourcesStates));
  }

  s_move_last_time = get_tmr10ms();
  return result;
}
#endif

#if defined(FLIGHT_MODES)
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
#if defined(PCBSTD)
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
#if defined(PCBSTD)
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

#if defined(PCBSTD)
int16_t getGVarValue(int16_t x, int16_t min, int16_t max)
{
  if (GV_IS_GV_VALUE(x,min,max)) {
    int8_t idx = GV_INDEX_CALCULATION(x,max);
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
  if (GV_IS_GV_VALUE(x,min,max)) {
    int8_t idx = GV_INDEX_CALCULATION(x,max);
    int8_t mul = 1;

    if (idx < 0) {
      idx = -1-idx;
      mul = -1;
    }

    x = GVAR_VALUE(idx, getGVarFlightPhase(phase, idx)) * mul;
  }

  return limit(min, x, max);
}

void setGVarValue(uint8_t idx, int16_t value, int8_t phase)
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
inline void convertUnit(getvalue_t & val, uint8_t & unit)
{
  if (IS_IMPERIAL_ENABLE()) {
    if (unit == UNIT_DEGREES) {
      val += 18;
      val *= 115;
      val >>= 6;
    }
    if (unit == UNIT_METERS) {
      // m to ft *105/32
      val = val * 3 + (val >> 2) + (val >> 5);
    }
    if (unit == UNIT_FEET) {
      unit = UNIT_METERS;
    }
    if (unit == UNIT_KTS) {
      unit = UNIT_KMH;
    }
  }
  else {
    if (unit == UNIT_KTS) {
      // kts to km/h
      unit = UNIT_KMH;
      val = (val * 46) / 25;
    }
  }
}

void putsTelemetryValue(xcoord_t x, uint8_t y, lcdint_t val, uint8_t unit, uint8_t att)
{
  convertUnit(val, unit);
  lcd_outdezAtt(x, y, val, att & (~NO_UNIT));
  if (!(att & NO_UNIT) && unit != UNIT_RAW)
    lcd_putsiAtt(lcdLastPos/*+1*/, y, STR_VTELEMUNIT, unit, 0);
}
#endif

#define INAC_DEVISOR 512   // bypass splash screen with stick movement
#define INAC_DEV_SHIFT 9   // shift right value for stick movement
uint16_t stickMoveValue()
{
  uint16_t sum = 0;
  for (uint8_t i=0; i<NUM_STICKS; i++)
    sum += anaIn(i) >> INAC_DEV_SHIFT;
  return sum;
}

void checkBacklight()
{
  static uint8_t tmr10ms ;

#if defined(PCBSTD) && defined(ROTARY_ENCODER_NAVIGATION)
  rotencPoll();
#endif

  uint8_t x = g_blinkTmr10ms;
  if (tmr10ms != x) {
    tmr10ms = x;
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
#if defined(PCBTARANIS)
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
#elif !defined(PCBTARANIS)
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

#if !defined(PCBSTD) && !defined(PCBTARANIS)
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
  lowLim = (g_model.throttleReversed ? - lowLim - g_eeGeneral.calibSpanPos[thrchn] : lowLim - g_eeGeneral.calibSpanNeg[thrchn]);
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
  if (IS_SOUND_OFF()) ALERT(STR_ALARMSWARN, STR_ALARMSDISABLED, AU_ERROR);
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
#if defined(PCBTARANIS)
      for (uint8_t i=0; i<NUM_SWITCHES-1; i++) {
        swstate_t mask = (0x03 << (1+i*2));
        uint8_t attr = ((states & mask) == (switches_states & mask)) ? 0 : INVERS;
        char c = "\300-\301"[(states & mask) >> (1+i*2)];
        lcd_putcAtt(50+i*(2*FW+FW/2), 5*FH, 'A'+i, attr);
        lcd_putcAtt(50+i*(2*FW+FW/2)+FW, 5*FH, c, attr);
      }
#else
      uint8_t x = 2;
      for (uint8_t i=1; i<MAX_PSWITCH-1; i++) {
        uint8_t attr = (states & (1 << i)) == (switches_states & (1 << i)) ? 0 : INVERS;
        putsSwitches(x, 5*FH, (i>2?(i+1):1+((states>>1)&0x3)), attr);
        if (i == 1 && attr) i++;
        if (i != 1) x += 3*FW+FW/2;
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
    uint8_t idx = CONVERT_MODE(1+(uint8_t)k/2) - 1;
    uint8_t phase;
    int16_t before;
    bool thro;

#if defined(GVARS)
#define TRIM_REUSED() trimGvar[idx] >= 0
    if (TRIM_REUSED()) {
#if defined(PCBSTD)
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
#if defined(CPUARM)
    after >>= 1;
    after += 120;
#else
    after >>= 2;
    after += 60;
#endif
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
  return (g_model.throttleReversed) ? -v : v;
}

#if !defined(SIMU)
uint16_t anaIn(uint8_t chan)
{
#if defined(PCBTARANIS)
  // crossAna[]={LH,LV,RH,RV,S1,S2,LS,RS,BAT 
  // s_anaFilt[]={LH,LV,RH,RV,S1,S2,LS,RS,_BAT
  return s_anaFilt[chan];
#elif defined(PCBSKY9X) && !defined(REVA)
  static const uint8_t crossAna[]={1,5,7,0,4,6,2,3};
  if (chan == TX_CURRENT) {
    return Current_analogue ;
  }
  volatile uint16_t *p = &s_anaFilt[pgm_read_byte(crossAna+chan)];
  return *p;
#else
  static const pm_char crossAna[] PROGMEM = {3,1,2,0,4,5,6,7};
  volatile uint16_t *p = &s_anaFilt[pgm_read_byte(crossAna+chan)];
  return *p;
#endif
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
/*
    ADMUX = adc_input|ADC_VREF_TYPE;
	// ADCSRA|=0x08; // enable ADC Interrupt Enable  should not be necessary? or is it
	MCUCR|=0x28;  // enable Sleep (bit5) enable ADC Noise Reduction (bit3)
	asm volatile(" sleep        \n\t");  // if _SLEEP() is not defined use this
    while (ADCSRA & 0x40); // Wait for the AD conversion to complete
    temp_ana = ADC;
    
	asm volatile(" sleep        \n\t");  // if _SLEEP() is not defined use this  
    while (ADCSRA & 0x40); // Wait for the AD conversion to complete
    s_anaFilt[adc_input] = temp_ana + ADC;    
   	MCUCR&=0x08;  // disable sleep  
    */
  
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
  ADMUX = 0x1E|ADC_VREF_TYPE; // Switch MUX to internal 1.22V reference
  
/*
  MCUCR|=0x28;  // enable Sleep (bit5) enable ADC Noise Reduction (bit2)
  asm volatile(" sleep        \n\t");  // if _SLEEP() is not defined use this
  // ADCSRA|=0x40;
  while ((ADCSRA & 0x10)==0);
  ADCSRA|=0x10; // take sample  clear flag?
  BandGap=ADC;    
  MCUCR&=0x08;  // disable sleep  
  */

  ADCSRA |= 0x40;
  while (ADCSRA & 0x40);
  BandGap = ADC;
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
int24_t  act   [MAX_MIXERS] = {0};
uint8_t  swOn  [MAX_MIXERS] = {0};

uint8_t mixWarning;

FORCEINLINE void evalTrims()
{
  uint8_t phase = s_perout_flight_phase;
  for (uint8_t i=0; i<NUM_STICKS; i++) {
    // do trim -> throttle trim if applicable
    int16_t trim = getTrimValue(phase, i);
    if (i==THR_STICK && g_model.thrTrim) {
      if (g_model.throttleReversed)
        trim = -trim;
      int16_t v = anas[i];
      int32_t vv = ((int32_t)trim-TRIM_MIN)*(RESX-v)>>(RESX_SHIFT+1);
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
    	
#if defined(PCBTARANIS)
    if (i == POT1 || i == SLIDER1)
      v = -v;
#endif

    if (g_model.throttleReversed && ch==THR_STICK)
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

    BeepANACenter mask = (BeepANACenter)1 << ch;

    if (i < NUM_STICKS+NUM_POTS) {
      calibratedStick[ch] = v; //for show in expo

      // filtering for center beep
      uint8_t tmp = (uint16_t)abs(v) / 16;
      if (tmp <= 1) anaCenter |= (tmp==0 ? mask : (bpanaCenter & mask));
    }
    else {
      // rotary encoders
      if (v == 0) anaCenter |= mask;
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
      if (d && (ch==ELE_STICK || ch==AIL_STICK))
        v = (int32_t(v)*calc100toRESX(g_model.swashR.value))/int32_t(d);
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
  getvalue_t val = getValue(idx);

  switch (idx) {
    case MIXSRC_FIRST_TELEM-1+TELEM_TX_VOLTAGE-1:
      PLAY_NUMBER(val, 1+UNIT_VOLTS, PREC1);
      break;
    case MIXSRC_FIRST_TELEM-1+TELEM_TM1-1:
    case MIXSRC_FIRST_TELEM-1+TELEM_TM2-1:
      PLAY_DURATION(val);
      break;
#if defined(FRSKY)
    case MIXSRC_FIRST_TELEM-1+TELEM_RSSI_TX-1:
    case MIXSRC_FIRST_TELEM-1+TELEM_RSSI_RX-1:
      PLAY_NUMBER(val, 1+UNIT_DBM, 0);
      break;
    case MIXSRC_FIRST_TELEM-1+TELEM_MIN_A1-1:
    case MIXSRC_FIRST_TELEM-1+TELEM_MIN_A2-1:
      idx -= TELEM_MIN_A1-TELEM_A1;
      // no break
    case MIXSRC_FIRST_TELEM-1+TELEM_A1-1:
    case MIXSRC_FIRST_TELEM-1+TELEM_A2-1:
      idx -= (MIXSRC_FIRST_TELEM-1+TELEM_A1-1);
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

    case MIXSRC_FIRST_TELEM-1+TELEM_CELL-1:
      PLAY_NUMBER(val/10, 1+UNIT_VOLTS, PREC1);
      break;

    case MIXSRC_FIRST_TELEM-1+TELEM_VFAS-1:
    case MIXSRC_FIRST_TELEM-1+TELEM_CELLS_SUM-1:
      PLAY_NUMBER(val, 1+UNIT_VOLTS, PREC1);
      break;

    case MIXSRC_FIRST_TELEM-1+TELEM_CURRENT-1:
    case MIXSRC_FIRST_TELEM-1+TELEM_MAX_CURRENT-1:
      PLAY_NUMBER(val, 1+UNIT_AMPS, PREC1);
      break;

    case MIXSRC_FIRST_TELEM-1+TELEM_ACCx-1:
    case MIXSRC_FIRST_TELEM-1+TELEM_ACCy-1:
    case MIXSRC_FIRST_TELEM-1+TELEM_ACCz-1:
      PLAY_NUMBER(val/10, 1+UNIT_G, PREC1);
      break;

    case MIXSRC_FIRST_TELEM-1+TELEM_VSPD-1:
      PLAY_NUMBER(val/10, 1+UNIT_METERS_PER_SECOND, PREC1);
      break;

    case MIXSRC_FIRST_TELEM-1+TELEM_CONSUMPTION-1:
      PLAY_NUMBER(val, 1+UNIT_MAH, 0);
      break;

    case MIXSRC_FIRST_TELEM-1+TELEM_POWER-1:
      PLAY_NUMBER(val, 1+UNIT_WATTS, 0);
      break;

    case MIXSRC_FIRST_TELEM-1+TELEM_ALT-1:
#if defined(PCBTARANIS)
      PLAY_NUMBER(val/100, 1+UNIT_METERS, 0);
      break;
#endif
    case MIXSRC_FIRST_TELEM-1+TELEM_MIN_ALT-1:
    case MIXSRC_FIRST_TELEM-1+TELEM_MAX_ALT-1:
#if defined(WS_HOW_HIGH)
      if (IS_IMPERIAL_ENABLE() && IS_USR_PROTO_WS_HOW_HIGH())
        PLAY_NUMBER(val, 1+UNIT_FEET, 0);
      else
#endif
        PLAY_NUMBER(val, 1+UNIT_METERS, 0);
      break;

    case MIXSRC_FIRST_TELEM-1+TELEM_RPM-1:
    case MIXSRC_FIRST_TELEM-1+TELEM_MAX_RPM-1:
      PLAY_NUMBER(val, 1+UNIT_RPMS, 0);
      break;

    case MIXSRC_FIRST_TELEM-1+TELEM_HDG-1:
      PLAY_NUMBER(val, 1+UNIT_DEGREES, 0);
      break;

    default:
    {
      uint8_t unit = 1;
      if (idx < MIXSRC_GVAR1-1)
        val = calcRESXto100(val);
      if (idx >= MIXSRC_FIRST_TELEM-1+TELEM_ALT-1 && idx <= MIXSRC_FIRST_TELEM-1+TELEM_GPSALT-1)
        unit = idx - (MIXSRC_FIRST_TELEM-1+TELEM_ALT-1);
      else if (idx >= MIXSRC_FIRST_TELEM-1+TELEM_MAX_T1-1 && idx <= MIXSRC_FIRST_TELEM-1+TELEM_MAX_DIST-1)
        unit = 3 + idx - (MIXSRC_FIRST_TELEM-1+TELEM_MAX_T1-1);

      unit = pgm_read_byte(bchunit_ar+unit);
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

#if !defined(PCBSTD)
uint8_t mSwitchDuration[1+NUM_ROTARY_ENCODERS] = { 0 };
#define CFN_PRESSLONG_DURATION   100
#endif

#if defined(CPUARM)
uint8_t currentSpeakerVolume = 255;
uint8_t requiredSpeakerVolume;
uint8_t fnSwitchDuration[NUM_CFN] = { 0 };

inline void playCustomFunctionFile(CustomFnData *sd, uint8_t id)
{
  char lfn[] = SOUNDS_PATH "/xxxxxx.wav";
  strncpy(lfn+sizeof(SOUNDS_PATH), sd->param.name, sizeof(sd->param.name));
  lfn[sizeof(SOUNDS_PATH)+sizeof(sd->param.name)] = '\0';
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
      MASK_FUNC_TYPE function_mask = (CFN_FUNC(sd) >= FUNC_TRAINER ? ((MASK_FUNC_TYPE)1 << (CFN_FUNC(sd)-FUNC_TRAINER)) : 0);
      MASK_CFN_TYPE  switch_mask   = ((MASK_CFN_TYPE)1 << i);
      uint8_t momentary = 0;

#if !defined(PCBSTD)

  #define MOMENTARY_START_TEST() ( (momentary && !(activeSwitches & switch_mask) && active) || \
                                   (short_long==1 && !active && mSwitchDuration[mswitch]>0 && mSwitchDuration[mswitch]<CFN_PRESSLONG_DURATION) || \
                                   (short_long==2 && active && mSwitchDuration[mswitch]>=CFN_PRESSLONG_DURATION) )

      uint8_t short_long = 0;
      uint8_t mswitch = 0;

      if (swtch == SWSRC_TRAINER_LONG) {
        short_long = 2;
        swtch = SWSRC_TRAINER;
        mswitch = 0;
      }
      else if (swtch == SWSRC_TRAINER_SHORT) {
        short_long = 1;
        swtch = SWSRC_TRAINER;
        mswitch = 0;
      }
      else

#else

  #define short_long  0
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
      if (momentary || short_long) {

#if !defined(PCBSTD)
        bool swState = active;
#endif

        if (MOMENTARY_START_TEST()) {

          if (short_long) {
            active = false;
            momentary = true;
          }
          else {
            active = !(activeFnSwitches & switch_mask);
#if !defined(CPUARM)
            if (CFN_FUNC(sd) == FUNC_PLAY_BOTH && !active) {
              momentary = true;
            }
            else
#endif
            {
              momentary = false;
            }
          }
        }
        else if (swtch == SWSRC_ON) {
          active = false;
          momentary = false;
        }
        else {
          active = (activeFnSwitches & switch_mask);
          momentary = false;
        }
#if !defined(PCBSTD)
        if (short_long) {
          if (swState) {
            if (mSwitchDuration[mswitch] < 255)
              mSwitchDuration[mswitch]++;
          }
          else {
            mSwitchDuration[mswitch] = 0;
          }
        }
#endif
      }
#if !defined(CPUARM)
      else if (CFN_FUNC(sd) == FUNC_PLAY_BOTH) {
        momentary = true;
      }
#endif

      if (active || momentary) {

        if (CFN_ACTIVE(sd)) {
          if (CFN_FUNC(sd) < FUNC_TRAINER) {
            safetyCh[CFN_CH_NUMBER(sd)] = CFN_PARAM(sd);
          }

          if (!(activeFunctions & function_mask)) {
            if (CFN_FUNC(sd) == FUNC_INSTANT_TRIM) {
              if (g_menuStack[0] == menuMainView
#if defined(FRSKY)
                || g_menuStack[0] == menuTelemetryFrsky
#endif
                )
                instantTrim();
            }
          }
        }
        else if (CFN_FUNC(sd) <= FUNC_INSTANT_TRIM) {
          active = false;
        }

#if defined(SDCARD)
        if (CFN_FUNC(sd) == FUNC_LOGS) {
          logDelay = CFN_PARAM(sd);
        }
#endif

        if (CFN_FUNC(sd) == FUNC_RESET) {
          switch (CFN_PARAM(sd)) {
            case FUNC_RESET_TIMER1:
            case FUNC_RESET_TIMER2:
              resetTimer(CFN_PARAM(sd));
              break;
            case FUNC_RESET_ALL:
              resetAll();
              break;
#if defined(FRSKY)
            case FUNC_RESET_TELEMETRY:
              resetTelemetry();
              break;
#endif
#if ROTARY_ENCODERS > 0
            case FUNC_RESET_ROTENC1:
#if ROTARY_ENCODERS > 1
            case FUNC_RESET_ROTENC2:
#endif
              g_rotenc[CFN_PARAM(sd)-FUNC_RESET_ROTENC1] = 0;
              break;
#endif
          }
        }

        if (CFN_FUNC(sd) == FUNC_PLAY_SOUND) {
          AUDIO_PLAY(AU_FRSKY_FIRST+CFN_PARAM(sd));
        }

#if defined(HAPTIC)
        if (CFN_FUNC(sd) == FUNC_HAPTIC) {
          haptic.event(AU_FRSKY_LAST+CFN_PARAM(sd));
        }
#endif

#if defined(CPUARM) && defined(SDCARD)
        else if (CFN_FUNC(sd) == FUNC_PLAY_TRACK || CFN_FUNC(sd) == FUNC_PLAY_VALUE) {
          tmr10ms_t tmr10ms = get_tmr10ms();
          uint8_t repeatParam = CFN_PLAY_REPEAT(sd);
          if (!lastFunctionTime[i] || (repeatParam && (signed)(tmr10ms-lastFunctionTime[i])>=500*repeatParam)) {
            if (!IS_PLAYING(i+1)) {
              lastFunctionTime[i] = tmr10ms;
              if (CFN_FUNC(sd) == FUNC_PLAY_VALUE) {
                PLAY_VALUE(CFN_PARAM(sd), i+1);
              }
              else {
                playCustomFunctionFile(sd, i+1);
              }
            }
          }
        }
        else if (CFN_FUNC(sd) == FUNC_BACKGND_MUSIC) {
          if (!IS_PLAYING(i+1)) {
            playCustomFunctionFile(sd, i+1);
          }
        }
        else if (CFN_FUNC(sd) == FUNC_VOLUME) {
          if (CFN_ACTIVE(sd)) {
            requiredSpeakerVolume = ((1024 + getValue(CFN_PARAM(sd))) * VOLUME_LEVEL_MAX) / 2048;
          }
          else {
            active = false;
          }
        }
#elif defined(VOICE)
        else if (CFN_FUNC(sd) == FUNC_PLAY_TRACK || CFN_FUNC(sd) == FUNC_PLAY_BOTH || CFN_FUNC(sd) == FUNC_PLAY_VALUE) {
          tmr10ms_t tmr10ms = get_tmr10ms();
          uint8_t repeatParam = CFN_PLAY_REPEAT(sd);
          if (!lastFunctionTime[i] || (CFN_FUNC(sd)==FUNC_PLAY_BOTH && active!=(bool)(activeFnSwitches&switch_mask)) || (repeatParam && (signed)(tmr10ms-lastFunctionTime[i])>=1000*repeatParam)) {
            lastFunctionTime[i] = tmr10ms;
            uint8_t param = CFN_PARAM(sd);
            if (CFN_FUNC(sd) == FUNC_PLAY_VALUE) {
              PLAY_VALUE(param, i+1);
            }
            else {
#if defined(GVARS)
              if (CFN_FUNC(sd) == FUNC_PLAY_TRACK && param > 250)
                param = GVAR_VALUE(param-251, getGVarFlightPhase(s_perout_flight_phase, param-251));
#endif
              PUSH_CUSTOM_PROMPT(active ? param : param+1, i+1);
            }
          }
        }
#endif

#if defined(DEBUG)
        else if (CFN_FUNC(sd) == FUNC_TEST) {
          testFunc();
        }
#endif

#if defined(GVARS)
        else if (CFN_FUNC(sd) >= FUNC_ADJUST_GV1) {
          if (CFN_ACTIVE(sd)) {
            if (CFN_GVAR_MODE(sd) == 0) {
              SET_GVAR(CFN_FUNC(sd)-FUNC_ADJUST_GV1, CFN_PARAM(sd), s_perout_flight_phase);
            }
            else if (CFN_GVAR_MODE(sd) == 2) {
              SET_GVAR(CFN_FUNC(sd)-FUNC_ADJUST_GV1, GVAR_VALUE(CFN_PARAM(sd), s_perout_flight_phase), s_perout_flight_phase);
            }
            else if (CFN_GVAR_MODE(sd) == 3) {
              if (!(activeFnSwitches & switch_mask)) {
                SET_GVAR(CFN_FUNC(sd)-FUNC_ADJUST_GV1, GVAR_VALUE(CFN_FUNC(sd)-FUNC_ADJUST_GV1, getGVarFlightPhase(s_perout_flight_phase, CFN_FUNC(sd)-FUNC_ADJUST_GV1)) + (CFN_PARAM(sd) ? +1 : -1), s_perout_flight_phase);
              }
            }
            else if (CFN_PARAM(sd) >= MIXSRC_TrimRud-1 && CFN_PARAM(sd) <= MIXSRC_TrimAil-1) {
              trimGvar[CFN_PARAM(sd)-MIXSRC_TrimRud+1] = CFN_FUNC(sd)-FUNC_ADJUST_GV1;
            }
#if defined(ROTARY_ENCODERS)
            else if (CFN_PARAM(sd) >= MIXSRC_REa-1 && CFN_PARAM(sd) < MIXSRC_TrimRud-1) {
              int8_t scroll = rePreviousValues[CFN_PARAM(sd)-MIXSRC_REa+1] - (g_rotenc[CFN_PARAM(sd)-MIXSRC_REa+1] / ROTARY_ENCODER_GRANULARITY);
              if (scroll) {
                SET_GVAR(CFN_FUNC(sd)-FUNC_ADJUST_GV1, GVAR_VALUE(CFN_FUNC(sd)-FUNC_ADJUST_GV1, getGVarFlightPhase(s_perout_flight_phase, CFN_FUNC(sd)-FUNC_ADJUST_GV1)) + scroll, s_perout_flight_phase);
              }
            }
#endif
            else {
              SET_GVAR(CFN_FUNC(sd)-FUNC_ADJUST_GV1, limit((getvalue_t)-1250, getValue(CFN_PARAM(sd)), (getvalue_t)1250) / 10, s_perout_flight_phase);
            }
          }
          else {
            active = false;
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
        if (CFN_FUNC(sd) == FUNC_BACKGND_MUSIC && isFunctionActive(FUNC_BACKGND_MUSIC)) {
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
    uint32_t q = calc100toRESX(g_model.swashR.value);
    q *= q;
    if (v>q) {
      uint16_t d = isqrt32(v);
      int16_t tmp = calc100toRESX(g_model.swashR.value);
      anas[ELE_STICK] = (int32_t) anas[ELE_STICK]*tmp/d;
      anas[AIL_STICK] = (int32_t) anas[AIL_STICK]*tmp/d;
    }
  }

#define REZ_SWASH_X(x)  ((x) - (x)/8 - (x)/128 - (x)/512)   //  1024*sin(60) ~= 886
#define REZ_SWASH_Y(x)  ((x))   //  1024 => 1024

  if(g_model.swashR.type)
  {
    getvalue_t vp = anas[ELE_STICK]+trims[ELE_STICK];
    getvalue_t vr = anas[AIL_STICK]+trims[AIL_STICK];
    getvalue_t vc = 0;
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
      getvalue_t v = 0;
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
#if defined(PCBTARANIS)
        else {
          v = getValue(k);
          if (k >= MIXSRC_SA-1 && k <= MIXSRC_LAST_CSW-1) {
            if (v < 0 && !md->swtch) sw = false;
          }
          if (k>=MIXSRC_CH1-1 && k<=MIXSRC_LAST_CH-1 && md->destCh != k-MIXSRC_CH1+1) {
            if (dirtyChannels & ((bitfield_channels_t)1 << (k-MIXSRC_CH1+1)) & (passDirtyChannels|~(((bitfield_channels_t) 1 << md->destCh)-1)))
              passDirtyChannels |= (bitfield_channels_t) 1 << md->destCh;
            if (k-MIXSRC_CH1+1 < md->destCh || pass > 0)
              v = chans[k-MIXSRC_CH1+1] >> 8;  // remove factor 256 from old mix loop; was 100 before
          }
        }
#else
        else {
          v = getValue(k);
          if (k >= MIXSRC_THR-1 && k <= MIXSRC_LAST_CSW-1) {
            if (v < 0 && !md->swtch) sw = false;
          }
          else if (k>=MIXSRC_CH1-1 && k<=MIXSRC_LAST_CH-1 && md->destCh != k-MIXSRC_CH1+1) {
            if (dirtyChannels & ((bitfield_channels_t)1 << (k-MIXSRC_CH1+1)) & (passDirtyChannels|~(((bitfield_channels_t) 1 << md->destCh)-1)))
              passDirtyChannels |= (bitfield_channels_t) 1 << md->destCh;
            if (k-MIXSRC_CH1+1 < md->destCh || pass > 0)
              v = chans[k-MIXSRC_CH1+1] >> 8;  // remove factor 256 from old mix loop; was 100 before
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
        int16_t offset = GET_GVAR(MD_OFFSET(md), GV_RANGELARGE_NEG, GV_RANGELARGE, s_perout_flight_phase); // open.20.fsguruh
        if (offset) v += calc100toRESX_16Bits(offset);  // @@@ open.20.fsguruh      
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
      
      // saves 12 bytes code if done here and not together with weight; unknown reason
      int16_t weight = GET_GVAR(MD_WEIGHT(md), GV_RANGELARGE_NEG, GV_RANGELARGE, s_perout_flight_phase);            
      weight=calc100to256_16Bits(weight);      
      
      //========== SPEED ===============
      // now its on input side, but without weight compensation. More like other remote controls
      // lower weight causes slower movement
      if (mode == e_perout_mode_normal && (md->speedUp || md->speedDown)) { // there are delay values
#define DEL_MULT_SHIFT 8
        // we recale to a mult 256 higher value for calculation
        int32_t tact = act[i];
        int16_t diff = v - (tact>>DEL_MULT_SHIFT);
        if (diff) {
          // open.20.fsguruh: speed is defined in % movement per second; In menu we specify the full movement (-100% to 100%) = 200% in total
          // the unit of the stored value is the value from md->speedUp or md->speedDown divide SLOW_STEP seconds; e.g. value 4 means 4/SLOW_STEP = 2 seconds for CPU64
          // because we get a tick each 10msec, we need 100 ticks for one second
          // the value in md->speedXXX gives the time it should take to do a full movement from -100 to 100 therefore 200%. This equals 2048 in recalculated internal range
          if (tick10ms) {
            // only if already time is passed add or substract a value according the speed configured
            int32_t rate = (int32_t) tick10ms << (DEL_MULT_SHIFT+11);  // = DEL_MULT*2048*tick10ms
            // rate equals a full range for one second; if less time is passed rate is accordingly smaller
            // if one second passed, rate would be 2048 (full motion)*256(recalculated weight)*100(100 ticks needed for one second)
            int32_t currentValue=((int32_t) v<<DEL_MULT_SHIFT);
            if (diff>0) {
              if (md->speedUp>0) {
                // if a speed upwards is defined recalculate the new value according configured speed; the higher the speed the smaller the add value is
                int32_t newValue = tact+rate/((int16_t)(100/SLOW_STEP)*md->speedUp);
                if (newValue<currentValue) currentValue=newValue; // Endposition; prevent toggling around the destination
              }
            }
            else {  // if is <0 because ==0 is not possible
              if (md->speedDown>0) {
                // see explanation in speedUp
                int32_t newValue = tact-rate/((int16_t)(100/SLOW_STEP)*md->speedDown);
                if (newValue>currentValue) currentValue=newValue; // Endposition; prevent toggling around the destination
              }            
            } //endif diff>0
            act[i] = tact = currentValue;
            // open.20.fsguruh: this implementation would save about 50 bytes code
          } // endif tick10ms ; in case no time passed assign the old value, not the current value from source
          v = (tact >> DEL_MULT_SHIFT);
        } //endif diff
      } //endif speed

      //========== CURVES ===============
      if (apply_offset_and_curve && md->curveParam && md->curveMode == MODE_CURVE) {
        v = applyCurve(v, md->curveParam);
      }

      //========== WEIGHT ===============
      int32_t dv = (int32_t) v * weight;
      
      //========== DIFFERENTIAL =========
      if (md->curveMode == MODE_DIFFERENTIAL) {
	// @@@2 also recalculate curveParam to a 256 basis which ease the calculation later a lot
        int16_t curveParam = calc100to256(GET_GVAR(md->curveParam, -100, 100, s_perout_flight_phase));
        if (curveParam > 0 && dv < 0)
          dv = (dv * (256 - curveParam)) >> 8;
        else if (curveParam < 0 && dv > 0)
          dv = (dv * (256 + curveParam)) >> 8;		  
      }

      int32_t *ptr = &chans[md->destCh]; // Save calculating address several times
      
      if (i == 0 || md->destCh != (md - 1)->destCh)
        *ptr = 0;
      // if this is the first calculation for the destination channel, initialize it with 0 (otherwise would be random)

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
          dv >>= 8;		
          dv *= *ptr;
          dv >>= RESX_SHIFT;   // same as dv /= RESXl;
          *ptr = dv;
          break;
        default: // MLTPX_ADD
          *ptr += dv; //Mixer output add up to the line (dv + (dv>0 ? 100/2 : -100/2))/(100);
          break;
      } //endswitch md->mltpx
#ifdef PREVENT_ARITHMETIC_OVERFLOW
/*      
      // a lot of assumptions must be true, for this kind of check; not really worth for only 4 bytes flash savings
      // this solution would save again 4 bytes flash
      int8_t testVar=(*ptr<<1)>>24;
      if ( (testVar!=-1) && (testVar!=0 ) ) {
        // this devices by 64 which should give a good balance between still over 100% but lower then 32x100%; should be OK
        *ptr >>= 6;  // this is quite tricky, reduces the value a lot but should be still over 100% and reduces flash need
      } */


      PACK( union u_int16int32_t {
        struct {
          int16_t lo;
          int16_t hi;
        } words_t;
        int32_t dword;
      });
      
      u_int16int32_t tmp;
      tmp.dword=*ptr;
      
      if (tmp.dword<0) {
        if ((tmp.words_t.hi&0xFF80)!=0xFF80) tmp.words_t.hi=0xFF86; // set to min nearly
      }
      else {
        if ((tmp.words_t.hi|0x007F)!=0x007F) tmp.words_t.hi=0x0079; // set to max nearly
      }
      *ptr=tmp.dword;
      // this implementation saves 18bytes flash

/*      dv=*ptr>>8;
      if (dv>(32767-RESXl)) {
        *ptr=(32767-RESXl)<<8;
      } else if (dv<(-32767+RESXl)) {
        *ptr=(-32767+RESXl)<<8;
      }*/
      // *ptr=limit( int32_t(int32_t(-1)<<23), *ptr, int32_t(int32_t(1)<<23));  // limit code cost 72 bytes
      // *ptr=limit( int32_t((-32767+RESXl)<<8), *ptr, int32_t((32767-RESXl)<<8));  // limit code cost 80 bytes
#endif        
     
    } //endfor mixers

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
  uint8_t tick10ms = (tmr10ms >= lastTMR ? tmr10ms - lastTMR : 1);  // handle tick10ms overrun
  //@@@ open.20.fsguruh: correct overflow handling costs a lot of code; happens only each 11 min;
  // therefore forget the exact calculation and use only 1 instead; good compromise
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

  if (tick10ms) {
#if defined(CPUARM)
    requiredSpeakerVolume = g_eeGeneral.speakerVolume + VOLUME_LEVEL_DEF;
#endif

    // the reason this needs to be done before limits is the applyLimit function; it checks for safety switches which would be not initialized otherwise
    evalFunctions();
  }

  int32_t weight = 0;
  if (s_fade_flight_phases) {
    memclear(sum_chans512, sizeof(sum_chans512));
    for (uint8_t p=0; p<MAX_PHASES; p++) {
      if (s_fade_flight_phases & ((ACTIVE_PHASES_TYPE)1 << p)) {
        s_perout_flight_phase = p;
        perOut(e_perout_mode_normal, tick10ms);
        for (uint8_t i=0; i<NUM_CHNOUT; i++)
          sum_chans512[i] += (chans[i] >> 4) * fp_act[p];
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
    // chans[i] holds data from mixer.   chans[i] = v*weight => 1024*256
    // later we multiply by the limit (up to 100) and then we need to normalize
    // at the end chans[i] = chans[i]/256 =>  -1024..1024
    // interpolate value with min/max so we get smooth motion from center to stop
    // this limits based on v original values and min=-1024, max=1024  RESX=1024
    int32_t q = (s_fade_flight_phases ? (sum_chans512[i] / weight) << 4 : chans[i]);
    ex_chans[i] = q>>8; // for the next perMain

    int16_t value = applyLimits(i, q);  // applyLimits will remove the 256 100% basis

    cli();
    channelOutputs[i] = value;  // copy consistent word to int-level
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
    val = channelOutputs[ch];
    
    int16_t gModelMax = calc100toRESX(g_model.limitData[ch].max)+1024;
    int16_t gModelMin = calc100toRESX(g_model.limitData[ch].min)-1024;
    
    if (g_model.limitData[ch].revert)
      val = -val + gModelMax;
    else
      val = val - gModelMin;
      
#if defined(PPM_LIMITS_SYMETRICAL)
    if (g_model.limitData[ch].symetrical)
      val -= calc1000toRESX(g_model.limitData[ch].offset);
#endif
    // @@@ open.20.fsguruh  optimized calculation; now *8 /8 instead of 10 base; (*16/16 already cause a overrun; unsigned calculation also not possible, because v may be negative)
    gModelMax -= gModelMin; // we compare difference between Max and Mix for recaling needed; Max and Min are shifted to 0 by default
    // usually max is 1024 min is -1024 --> max-min = 2048 full range / 128 = max 16 steps
    
    gModelMax >>= (10-2);
    if (gModelMax != 8)
      val = (val << 3) / (gModelMax); // recaling only needed if Min, Max differs
    
    // if (gModelMax!=2048) val = (int32_t) (val << 11) / (gModelMax); // recaling only needed if Min, Max differs
    // val = val * 10 / (10+(g_model.limitData[ch].max-g_model.limitData[ch].min)/20);
    if (val<0) val=0;  // prevent val be negative, which would corrupt throttle trace and timers; could occur if safetyswitch is smaller than limits
  }
  else {
    val = RESX + calibratedStick[g_model.thrTraceSrc == 0 ? THR_STICK : g_model.thrTraceSrc+NUM_STICKS-1];
  }

#if !defined(CPUM64)
  //  code cost is about 16 bytes for higher throttle accuracy for timer 
  //  would not be noticable anyway, because all version up to this change had only 16 steps; 
  //  now it has already 32  steps; this define would increase to 128 steps
  #define ACCURAT_THROTTLE_TIMER
#endif

#if defined(ACCURAT_THROTTLE_TIMER)
  val >>= (RESX_SHIFT-6); // calibrate it (resolution increased by factor 4)
#else
  val >>= (RESX_SHIFT-4); // calibrate it
#endif

  // Timers start
  for (uint8_t i=0; i<MAX_TIMERS; i++) {
    int8_t tm = g_model.timers[i].mode;
    uint16_t tv = g_model.timers[i].start;

    if (tm) {
      if (s_timerState[i] == TMR_OFF) {
        s_timerState[i] = TMR_RUNNING;
        s_cnt[i] = 0;
        s_sum[i] = 0;
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
        int16_t newTimerVal = s_timerVal[i];
        if (tv) newTimerVal = tv - newTimerVal;

        if (atm==TMRMODE_ABS) {
          newTimerVal++;
        }
        else if (atm==TMRMODE_THR) {
          if (val) newTimerVal++;
        }
        else if (atm==TMRMODE_THR_REL) {
          // @@@ open.20.fsguruh: why so complicated? we have already a s_sum field; use it for the half seconds (not showable) as well
          // check for s_cnt[i]==0 is not needed because we are shure it is at least 1
#if defined(ACCURAT_THROTTLE_TIMER)
          if ((s_sum[i]/s_cnt[i])>=128) {  // throttle was normalized to 0 to 128 value (throttle/64*2 (because - range is added as well)
            newTimerVal++;  // add second used of throttle
            s_sum[i]-=128*s_cnt[i];
          }
#else
          if ((s_sum[i]/s_cnt[i])>=32) {  // throttle was normalized to 0 to 32 value (throttle/16*2 (because - range is added as well)
            newTimerVal++;  // add second used of throttle
            s_sum[i]-=32*s_cnt[i];
          }
#endif
          s_cnt[i]=0;
        }
        else if (atm==TMRMODE_THR_TRG) {
          if (val || newTimerVal > 0)
            newTimerVal++;
        }
        else {
          if (atm<(TMR_VAROFS+MAX_SWITCH))
            sw_toggled[i] = tm>0 ? getSwitch(tm-(TMR_VAROFS-1), 0) : !getSwitch(-tm, 0); // normal switch
          if (sw_toggled[i])
            newTimerVal++;
        }

        switch(s_timerState[i])
        {
          case TMR_RUNNING:
            if (tv && newTimerVal>=(int16_t)tv) s_timerState[i]=TMR_BEEPING;
            break;
          case TMR_BEEPING:
            if (newTimerVal >= (int16_t)tv + MAX_ALERT_TIME) s_timerState[i]=TMR_STOPPED;
            break;
        }

        if (tv) newTimerVal = tv - newTimerVal; //if counting backwards - display backwards

        if (newTimerVal != s_timerVal[i]) { // beep only if seconds advance
          s_timerVal[i] = newTimerVal;
          if (s_timerState[i] == TMR_RUNNING) {
            if (g_model.timers[i].countdownBeep && g_model.timers[i].start) { // beep when 30, 15, 10, 5,4,3,2,1 seconds remaining
              if (newTimerVal==30) AUDIO_TIMER_30(); //beep three times
              if (newTimerVal==20) AUDIO_TIMER_20(); //beep two times
              if (newTimerVal==10) AUDIO_TIMER_10();
              if (newTimerVal<= 3) AUDIO_TIMER_LT3(newTimerVal);
            }

            if (g_model.timers[i].minuteBeep && (newTimerVal % 60)==0) { // short beep every minute
              AUDIO_TIMER_MINUTE(newTimerVal);
            }
          }
          else if (s_timerState[0] == TMR_BEEPING) {
            AUDIO_WARNING1();
          }
        }
      }
    }
  } //endfor timer loop (only two)

  static tmr10ms_t s_time_tot;
  static uint8_t s_cnt_1s;
  static uint16_t s_sum_1s;
#if defined(THRTRACE)
  static tmr10ms_t s_time_trace;
  static uint16_t s_cnt_10s;
  static uint16_t s_sum_10s;
#endif

  s_cnt_1s++;
  s_sum_1s+=val;
  
  // @@@ open.20.fsguruh: moved code here; at least val variable conflicts with caculations above, safer here?
  // even reduced code size about 8 bytes, why ever?
  if ((tmr10ms_t)(tmr10ms - s_time_tot) >= 100) { // 1sec
    s_time_tot += 100;
    s_timeCumTot += 1;

    inacCounter++;
    if ((((uint8_t)inacCounter)&0x07)==0x01 && g_eeGeneral.inactivityTimer && g_vbat100mV>50 && inacCounter > ((uint16_t)g_eeGeneral.inactivityTimer*60))
      AUDIO_INACTIVITY();

#if defined(AUDIO)
    if (mixWarning & 1) if ((s_timeCumTot&0x03)==0) AUDIO_MIX_WARNING(1);
    if (mixWarning & 2) if ((s_timeCumTot&0x03)==1) AUDIO_MIX_WARNING(2);
    if (mixWarning & 4) if ((s_timeCumTot&0x03)==2) AUDIO_MIX_WARNING(3);
#endif

#if defined(ACCURAT_THROTTLE_TIMER)
    val = s_sum_1s / s_cnt_1s;
    s_timeCum16ThrP += (val>>3);  // s_timeCum16ThrP would overrun if we would store throttle value with higher accuracy; therefore stay with 16 steps
    if (val) s_timeCumThr += 1;
    s_sum_1s>>=2;  // correct better accuracy now, because trace graph can show this information; in case thrtrace is not active, the compile should remove this
#else    
    val = s_sum_1s / s_cnt_1s;
    s_timeCum16ThrP += (val>>1);  
    if (val) s_timeCumThr += 1;
#endif    
    
#if defined(THRTRACE)
    // throttle trace is done every 10 seconds; Tracebuffer is adjusted to screen size.
    // in case buffer runs out, it wraps around
    // resolution for y axis is only 32, therefore no higher value makes sense
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
  } //endif s_fade_fligh_phases

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
  if (delta > 0 && delta < MAX_MIXER_DELTA) {
#if defined(PCBSTD) && defined(ROTARY_ENCODER_NAVIGATION)
    rotencPoll();
#endif

    // @@@ open.20.fsguruh
    // SLEEP();   // wouldn't that make sense? should save a lot of battery power!!!
/*  for future use; currently very very beta...  */
#if defined(POWER_SAVE)
    ADCSRA&=0x7F;   // disable ADC for power saving
    ACSR&=0xF7;   // disable ACIE Interrupts
    ACSR|=0x80;   // disable Analog Comparator
    // maybe we disable here a lot more hardware components in future to save even more power



    MCUCR|=0x20;  // enable Sleep (bit5)
    // MCUCR|=0x28;  // enable Sleep (bit5) enable ADC Noise Reduction (bit3)
    // first tests showed: simple sleep would reduce cpu current from 40.5mA to 32.0mA
    //                     noise reduction sleep would reduce it down to 28.5mA; However this would break pulses in theory
    // however with standard module, it will need about 95mA. Therefore the drop to 88mA is not much noticable
    do {
      asm volatile(" sleep        \n\t");  // if _SLEEP() is not defined use this
      t0=getTmr16KHz();
      delta= (nextMixerEndTime - lastMixerDuration) - t0;
    } while ((delta>0) && (delta<MAX_MIXER_DELTA));
    
    // reenabling of the hardware components needed here
    MCUCR&=0x00;  // disable sleep
    ADCSRA|=0x80;  // enable ADC
#endif
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

#if defined(FRSKY)
  telemetryWakeup();
#endif

  lcd_clear();
  const char *warn = s_warning;
  uint8_t menu = s_menu_count;

  if (EEPROM_MASSSTORAGE()) {
    menuMainView(0);
  }
  else {
    g_menuStack[g_menuStackPtr]((warn || menu) ? 0 : evt);
    if (warn) displayWarning(evt);
#if defined(NAVIGATION_MENUS)
    if (menu) {
      const char * result = displayMenu(evt);
      if (result) {
        menuHandler(result);
        putEvent(EVT_MENU_UP);
      }
    }
#endif
  }

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
#if defined(PCBTARANIS)
    int32_t instant_vbat = anaIn(TX_VOLTAGE);
    instant_vbat = ( instant_vbat + instant_vbat*(g_eeGeneral.vBatCalib)/128 ) * BATT_SCALE;
    instant_vbat >>= 11;
#elif defined(PCBSKY9X)
    int32_t instant_vbat = anaIn(TX_VOLTAGE);
    instant_vbat = ( instant_vbat + instant_vbat*(g_eeGeneral.vBatCalib)/128 ) * 4191;
    instant_vbat /= 55296;
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
} //endfunc void perMain()

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

#if defined(PCBSTD) && (defined(AUDIO) || defined(VOICE))
// Clocks every 128 uS
ISR(TIMER2_OVF_vect, ISR_NOBLOCK)
{
  cli();
  TIMSK &= ~(1<<TOIE2) ; // stop reentrance
  sei();

#if defined(AUDIO)
  AUDIO_DRIVER();
#endif

#if defined(VOICE)
  VOICE_DRIVER();
#endif

  cli();
  TIMSK |= (1<<TOIE2) ;
  sei();
}
#endif

// Clocks every 10ms
ISR(TIMER_10MS_VECT, ISR_NOBLOCK) 
{
  // without correction we are 0,16% too fast; that mean in one hour we are 5,76Sek too fast; we do not like that
  static uint8_t accuracyWarble; // because 16M / 1024 / 100 = 156.25. we need to correct the fault; no start value needed  

#if defined(AUDIO)
  AUDIO_HEARTBEAT();
#endif

#if defined(BUZZER)
  BUZZER_HEARTBEAT();
#endif

#if defined(HAPTIC)
  HAPTIC_HEARTBEAT();
#endif

  per10ms();

  uint8_t bump = (!(++accuracyWarble & 0x03)) ? 157 : 156;
  TIMER_10MS_COMPVAL += bump;
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
#elif defined(FRSKY)
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
    if (g_model.timers[i].persistent) {
      if (g_model.timers[i].value != (uint16_t)s_timerVal[i]) {
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
#define OPEN9X_INIT_ARGS const uint8_t mcusr
#elif defined(PCBSTD)
#define OPEN9X_INIT_ARGS const uint8_t mcusr
#else
#define OPEN9X_INIT_ARGS
#endif

inline void opentxInit(OPEN9X_INIT_ARGS)
{
#if defined(PCBTARANIS)
  BACKLIGHT_ON();
  CoTickDelay(10);  //20ms
  Splash();
#endif

  eeReadAll();

#if defined(CPUARM)
  if (UNEXPECTED_SHUTDOWN())
    unexpectedShutdown = 1;
#endif

#if defined(VOICE)
  setVolume(g_eeGeneral.speakerVolume+VOLUME_LEVEL_DEF);
#endif

#if defined(CPUARM)
  audioQueue.start();
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
#if !defined(CPUARM)
    // is done above on ARM
    unexpectedShutdown = 1;
#endif
#if defined(CPUARM)
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

#if defined(CPUARM)
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

      CoEnterMutexSection(mixerMutex);
      bool tick10ms = doMixerCalculations();
      CoLeaveMutexSection(mixerMutex);
      if (tick10ms) checkTrims();

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
  opentxInit();

  while (pwrCheck() != e_power_off) {
    perMain();
#if defined(PCBSKY9X)
    for (uint8_t i=0; i<5; i++) {
      usbMassStorage();
      CoTickDelay(1);  // 5*2ms for now
    }
#else
    CoTickDelay(5);  // 5*2ms for now
#endif
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
  opentxInit(mcusr);
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
  pwrOff(); // Only turn power off if necessary
  wdt_disable();
  while(1); // never return from main() - there is no code to return back, if any delays occurs in physical power it does dead loop.
#endif
}
#endif // !SIMU

