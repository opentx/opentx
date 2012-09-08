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

#include "open9x.h"

#if defined(PCBARM)
#define MIXER_STACK_SIZE    500
#define MENUS_STACK_SIZE    2000
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

#if defined(DEBUG)
OS_TID debugTaskId;
OS_STK debugStack[DEBUG_STACK_SIZE];
#endif

OS_TCID audioTimer;
OS_FlagID audioFlag;

OS_MutexID sdMutex;
OS_MutexID audioMutex;
OS_MutexID mixerMutex;
#endif

#if defined(SPLASH)
const pm_uchar splashdata[] PROGMEM = { 'S','P','S',0,
#include "s9xsplash.lbm"
	'S','P','E',0};
const pm_uchar * splash_lbm = splashdata+4;
#endif

#if defined(PCBV4) || defined(PCBARM) || defined(M128) || defined(EXTSTD)
const pm_uchar asterisk_lbm[] PROGMEM = {
#include "asterisk.lbm"
};
#endif

#include "menus.h"

EEGeneral  g_eeGeneral;
ModelData  g_model;

#if !defined(PCBARM)
uint8_t g_tmr1Latency_max;
uint8_t g_tmr1Latency_min;
#endif

uint8_t unexpectedShutdown = 0;

uint16_t g_timeMainMax;
#if defined(PCBV4)
uint8_t  g_timeMainLast;
#endif

#if defined(AUDIO) && !defined(PCBARM)
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
  if (size % 2 == 0) {
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

#if defined(PCBSTD) && !defined(M128)
void memclear(void *ptr, uint8_t size)
{
  memset(ptr, 0, size);
}
#endif

void generalDefault()
{
  memclear(&g_eeGeneral, sizeof(g_eeGeneral));
  g_eeGeneral.myVers   = EEPROM_VER;
  g_eeGeneral.variants = EEPROM_VARIANTS;
  g_eeGeneral.contrast = 25;
  g_eeGeneral.vBatWarn = 90;
  for (int i = 0; i < 7; ++i) {
    g_eeGeneral.calibMid[i]     = 0x200;
    g_eeGeneral.calibSpanNeg[i] = 0x180;
    g_eeGeneral.calibSpanPos[i] = 0x180;
  }
  g_eeGeneral.chkSum = (0x200 * 7) + (0x180 * 5);
}

uint16_t evalChkSum()
{
  uint16_t sum=0;
  for (int i=0; i<12;i++)
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

void resetProto()
{
#if defined(DSM2_SERIAL)
  if (g_model.protocol == PROTO_DSM2) {
    cli();
#if defined(FRSKY)
    DSM2_Init();
#endif
    sei();
  }
  else {
    cli();
#if defined(FRSKY)
    FRSKY_Init();
#else
    DSM2_Done();
#endif
    sei();
#if defined(FRSKY)
    FRSKY_setModelAlarms();
#endif
  }
#elif defined(FRSKY)
  FRSKY_setModelAlarms();
#endif
}

int16_t intpol(int16_t x, uint8_t idx) // -100, -75, -50, -25, 0 ,25 ,50, 75, 100
{
  CurveInfo crv = curveinfo(idx);
  int16_t erg = 0;

  x+=RESXu;
  if (x < 0) {
    erg = (int16_t)crv.crv[0] * (RESX/4);
  }
  else if (x >= (RESX*2)) {
    erg = (int16_t)crv.crv[crv.points-1] * (RESX/4);
  }
  else {
    uint16_t a=0, b=0; // TODO init not needed
    uint8_t i;
    if (crv.custom) {
      for (i=0; i<crv.points-1; i++) {
        a = (i==0 ? 0 : (100 * (RESX/4) / 25 + crv.crv[crv.points+i-1] * (RESX/4) / 25));
        b = (i==crv.points-2 ? 2*RESX : (100 * (RESX/4) / 25 + crv.crv[crv.points+i] * (RESX/4) / 25));
        if ((uint16_t)x>=a && (uint16_t)x<=b) break;
      }
    }
    else {
      uint16_t d = (RESX * 2) / (crv.points-1);
      i = (uint16_t)x / d;
      a = i * d;
      b = a + d;
    }
    erg = (int16_t)crv.crv[i]*(RESX/4) + ((int32_t)(x-a) * (crv.crv[i+1]-crv.crv[i]) * (RESX/4)) / ((b-a));
  }

  return erg / 25; // 100*D5/RESX;
}

#if defined(CURVES)
int16_t applyCurve(int16_t x, int8_t idx)
{
  /* already tried to have only one return at the end */
  switch(idx) {
    case 0:
      return x;
    case 1:
      if (x < 0) x = 0; //x|x>0
      return x;
    case 2:
      if (x > 0) x = 0; //x|x<0
      return x;
    case 3: // x|abs(x)
      return abs(x);
    case 4: //f|f>0
      return x > 0 ? RESX : 0;
    case 5: //f|f<0
      return x < 0 ? -RESX : 0;
    case 6: //f|abs(f)
      return x > 0 ? RESX : -RESX;
  }
  if (idx < 0) {
    x = -x;
    idx = -idx + 6;
  }
  return intpol(x, idx - 7);
}
#else
#define applyCurve(x, idx) (x)
#endif

// expo-funktion:
// ---------------
// kmplot
// f(x,k)=exp(ln(x)*k/10) ;P[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]
// f(x,k)=x*x*x*k/10 + x*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]
// f(x,k)=x*x*k/10 + x*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]
// f(x,k)=1+(x-1)*(x-1)*(x-1)*k/10 + (x-1)*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]

uint16_t expou(uint16_t x, uint16_t k)
{
  // k*x*x*x + (1-k)*x
  return ((unsigned long)x*x*x/0x10000*k/(RESXul*RESXul/0x10000) + (RESKul-k)*x+RESKul/2)/RESKul;
}

int16_t expo(int16_t x, int16_t k)
{
  if (k == 0) return x;
  int16_t y;
  bool neg =  x < 0;
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
#ifdef BOLD_FONT
      activeExpos |= ((ACTIVE_EXPOS_TYPE)1 << i);
#endif
      int16_t v = anas2[ed.chn];
      if((v<0 && ed.mode&1) || (v>=0 && ed.mode&2)) {
        cur_chn = ed.chn;
        if (ed.curveParam) {
          if (ed.curveMode == MODE_CURVE)
            v = applyCurve(v, ed.curveParam);
          else
            v = expo(v, ed.curveParam);
        }
        v = ((int32_t)v * ed.weight) / 100;
        anas[cur_chn] = v;
      }
    }
  }
}

#if !defined(PCBARM)
int16_t calc100toRESX(int8_t x)
{
  // return (int16_t)x*10 + x/4 - x/64;
  return ((x*41)>>2) - x/64;
}

int16_t calc1000toRESX(int16_t x) // improve calc time by Pat MacKenzie
{
  // return x + x/32 - x/128 + x/512;
  int16_t y = x>>5;
  x+=y;
  y=y>>2;
  x-=y;
  return x+(y>>2);
}

int16_t calcRESXto1000(int16_t x)
{
// *1000/1024 = x - x/32 + x/128
  return (x - x/32 + x/128);
}

#endif

int16_t applyLimits(uint8_t channel, int32_t value)
{
  LimitData * limit = limitaddress(channel);
  int16_t ofs = limit->offset;
  int16_t lim_p = 10 * (limit->max + 100);
  int16_t lim_n = 10 * (limit->min - 100); //multiply by 10 to get same range as ofs (-1000..1000)
  if (ofs > lim_p) ofs = lim_p;
  if (ofs < lim_n) ofs = lim_n;

#if defined(PPM_LIMITS_SYMETRICAL)
  if (value) {
    int32_t tmp;
    if (limit->symetrical)
      tmp = (value > 0) ? ((int32_t) lim_p) : ((int32_t) -lim_n);
    else
      tmp = (value > 0) ? ((int32_t) lim_p - ofs) : ((int32_t) -lim_n + ofs);
    value = (value * tmp) / 100000;
  }
#else
  if (value) {
    int32_t tmp = (value > 0) ? ((int32_t) lim_p - ofs) : ((int32_t) -lim_n + ofs);
    value = (value * tmp) / 100000; //div by 100000 -> output = -1024..1024
  }
#endif

  value += calc1000toRESX(ofs);
  lim_p = calc1000toRESX(lim_p);
  lim_n = calc1000toRESX(lim_n);
  if (value > lim_p) value = lim_p;
  if (value < lim_n) value = lim_n;

  ofs = value; // we convert value to a 16bit value and reuse ofs
  if (limit->revert) ofs = -ofs; // finally do the reverse.

  if (safetyCh[channel] != -128) // if safety channel available for channel check
    ofs = calc100toRESX(safetyCh[channel]);

  return ofs;
}

int16_t calibratedStick[NUM_STICKS+NUM_POTS];
int16_t g_chans512[NUM_CHNOUT];
int16_t ex_chans[NUM_CHNOUT] = {0}; // Outputs (before LIMITS) of the last perMain
#ifdef HELI
int16_t cyc_anas[3] = {0};
#endif

int16_t getValue(uint8_t i)
{
  /*srcRaw is shifted +1!*/

  if(i<NUM_STICKS+NUM_POTS) return calibratedStick[i];
#if defined(PCBV4) || defined(PCBARM)
  else if(i<NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS) return getRotaryEncoder(i-(NUM_STICKS+NUM_POTS));
#endif
  else if(i<MIXSRC_TrimAil) return calc1000toRESX((int16_t)8 * getTrimValue(s_perout_flight_phase, i-(NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS)));
  else if(i<MIXSRC_MAX) return 1024;
  else if(i<MIXSRC_3POS) return (keyState(SW_ID0) ? -1024 : (keyState(SW_ID1) ? 0 : 1024));
  else if(i<MIXSRC_3POS+3)
#if defined(HELI)
    return cyc_anas[i-MIXSRC_3POS];
#else
    return 0;
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
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_VSPD) return frskyData.varioSpeed;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_MIN_A1) return frskyData.analog[0].min;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_MIN_A2) return frskyData.analog[1].min;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_MAX_CURRENT) return *(((int16_t*)(&frskyData.hub.minAltitude))+i-(CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_MIN_ALT-1));
#endif
#endif
  else return 0;
}

#if defined(PCBARM)
#define GETSWITCH_RECURSIVE_TYPE uint32_t
volatile bool s_default_switch_value;
#else
#define GETSWITCH_RECURSIVE_TYPE uint16_t
#endif

volatile GETSWITCH_RECURSIVE_TYPE s_last_switch_used;
volatile GETSWITCH_RECURSIVE_TYPE s_last_switch_value;
/* recursive function. stack as of today (16/03/2012) grows by 8bytes at each call, which is ok! */

#if defined(PCBARM)
uint32_t delays[NUM_CSW];
uint32_t durations[NUM_CSW];
#endif

int16_t csLastValue[NUM_CSW];

bool __getSwitch(int8_t swtch)
{
  bool result;

  if (swtch == 0)
#if defined(PCBARM)
    return s_default_switch_value;
#else
    return s_last_switch_used & ((GETSWITCH_RECURSIVE_TYPE)1<<15);
#endif

  uint8_t cs_idx = abs(swtch);

  if (cs_idx == SWITCH_ON) {
    result = true;
  }
  else if (cs_idx <= MAX_PSWITCH) {
    result = keyState((EnumKeys)(SW_BASE+cs_idx-1));
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

#if !defined(PCBARM)
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
          if (s == CS_VOFS) {
            y = convertTelemValue(cs->v1-(CSW_CHOUT_BASE+NUM_CHNOUT), 128+cs->v2);
#if defined(FRSKY_HUB)
            uint8_t idx = cs->v1-CSW_CHOUT_BASE-NUM_CHNOUT-TELEM_ALT;
            if (idx < THLD_MAX) {
              // Fill the threshold array
              barsThresholds[idx] = 128 + cs->v2;
            }
#endif
          }
          else {
            y = cs->v2;
          }
        }
        else
#endif
        {
          y = calc100toRESX(cs->v2);
        }

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

#if defined(PCBARM)
    if (cs->delay) {
      if (result) {
        if (delays[cs_idx] > get_tmr10ms())
          result = false;
      }
      else {
        delays[cs_idx] = get_tmr10ms() + (cs->delay*50);
      }
    }
    if (cs->duration) {
      if (result) {
        if (durations[cs_idx] < get_tmr10ms()) {
          result = false;
          if (cs->delay) delays[cs_idx] = get_tmr10ms() + (cs->delay*50);
        }
      }
      else {
        durations[cs_idx] = get_tmr10ms() + (cs->duration*50);
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
#if defined(PCBARM)
  s_last_switch_used = 0;
  s_last_switch_value = 0;
  s_default_switch_value = nc;
#else
  s_last_switch_used = ((GETSWITCH_RECURSIVE_TYPE)nc<<15);
  s_last_switch_value = 0;
#endif
  return __getSwitch(swtch);
}

uint8_t switches_states = 0;
int8_t getMovedSwitch()
{
  static tmr10ms_t s_last_time = 0;

  int8_t result = 0;

  for (uint8_t i=MAX_PSWITCH; i>0; i--) {
    bool prev;
    uint8_t mask = 0;
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
#if defined(EXTRA_ROTARY_ENCODERS)
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
#if defined(EXTRA_ROTARY_ENCODERS)
  if(idx >= (NUM_ROTARY_ENCODERS - NUM_ROTARY_ENCODERS_EXTRA))
    return g_model.rotaryEncodersExtra[getRotaryEncoderFlightPhase(idx)][idx-(NUM_ROTARY_ENCODERS - NUM_ROTARY_ENCODERS_EXTRA)];
#endif
  return phaseaddress(getRotaryEncoderFlightPhase(idx))->rotaryEncoders[idx];
}

void incRotaryEncoder(uint8_t idx, int8_t inc)
{
  g_rotenc[idx] += inc;
#if defined(EXTRA_ROTARY_ENCODERS)
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

#if defined(FRSKY) || defined(PCBARM)
void putsTelemetryValue(uint8_t x, uint8_t y, int16_t val, uint8_t unit, uint8_t att)
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
#else
  if (unit == UNIT_KTS) {
    // kts to km/h
    unit = UNIT_KMH;
    val = (val * 46) / 25;
  }
#endif
  lcd_outdezAtt(x, (att & DBLSIZE ? y - FH : y), val, att & (~NO_UNIT)); // TODO we could add this test inside lcd_outdezAtt!
  if (~att & NO_UNIT && unit != UNIT_RAW)
    lcd_putsiAtt(lcdLastPos/*+1*/, y, STR_VTELEMUNIT, unit, 0);
}
#endif

void clearKeyEvents()
{
#if defined(PCBARM)
  CoTickDelay(100);  // 200ms
#endif

#if defined(SIMU)
  while (keyDown() && main_thread_running) sleep(1/*ms*/);
#else
  while (keyDown()) WDT_RESET_STOCK();  // loop until all keys are up
#endif
  memclear(keys, sizeof(keys));
  putEvent(0);
}

#define INAC_DEVISOR 256   // Bypass splash screen with stick movement
uint16_t stickMoveValue()
{
  uint16_t sum = 0;
  for (uint8_t i=0; i<4; i++)
    sum += anaIn(i)/INAC_DEVISOR;
  return sum;
}

void checkBacklight()
{
  static uint8_t tmr10ms ;

  if (tmr10ms != g_blinkTmr10ms) {
    tmr10ms = g_blinkTmr10ms ;
    uint16_t tsum = stickMoveValue();
    if (tsum != inacSum) {
      inacSum = tsum;
      inacCounter = 0;
      if (g_eeGeneral.backlightMode & e_backlight_mode_sticks)
        backlightOn();
    }
    else if (g_eeGeneral.inactivityTimer && (!g_vbat100mV || g_vbat100mV>50)) {
      if (++inacPrescale > 15 ) {
        inacCounter++;
        inacPrescale = 0;
        if (inacCounter > ((uint16_t)g_eeGeneral.inactivityTimer*100*60/16))
          if ((inacCounter&0x1F)==1) AUDIO_INACTIVITY();
      }
    }

    if (g_eeGeneral.backlightMode == e_backlight_mode_on || g_LightOffCounter || isFunctionActive(FUNC_BACKLIGHT))
      BACKLIGHT_ON;
    else
      BACKLIGHT_OFF;

#if defined(PCBSTD) && defined(VOICE)
    Voice.voice_process() ;
#endif
  }
}

void backlightOn()
{
  g_LightOffCounter = ((uint16_t)g_eeGeneral.lightAutoOff*250) << 1;
}

#ifdef SPLASH

#ifdef DSM2
#define SPLASH_NEEDED() (g_model.protocol != PROTO_DSM2 && !g_eeGeneral.disableSplashScreen)
#else
#define SPLASH_NEEDED() (!g_eeGeneral.disableSplashScreen)
#endif

void doSplash()
{
  if (SPLASH_NEEDED()) {
    lcd_clear();
    lcd_img(0, 0, splash_lbm, 0, 0);
    refreshDisplay();

#if !defined(PCBARM)
    AUDIO_TADA();
#endif

#if defined(PCBSTD)
    lcdSetContrast();
#else
    tmr10ms_t curTime = get_tmr10ms() + 10;
    uint8_t contrast = 10;
    lcdSetRefVolt(contrast);
#endif

#if !defined(SIMU)
    for(uint8_t i=0; i<32; i++)
      getADC_filt(); // init ADC array
#endif

    uint16_t inacSum = stickMoveValue();

    tmr10ms_t tgtime = get_tmr10ms() + SPLASH_TIMEOUT;
    while (tgtime != get_tmr10ms())
    {
#ifdef SIMU
      if (!main_thread_running) return;
      sleep(1/*ms*/);
#else
      getADC_filt();
#endif
      uint16_t tsum = stickMoveValue();

      if(keyDown() || (tsum!=inacSum)) return;  //wait for key release

      if (check_soft_power() > e_power_trainer) return; // Usb on or power off

#if !defined(PCBSTD)
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

#if !defined(PCBARM)
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
  getADC_single();   // if thr is down - do not display warning at all
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
#ifdef SIMU
      if (!main_thread_running) return;
      sleep(1/*ms*/);
#else
      getADC_single();
#endif
      int16_t v = thrAnaIn(thrchn);

      if (check_soft_power() > e_power_trainer || v<=lowLim || keyDown())
        break;

      checkBacklight();

      WDT_RESET_STOCK();
  }
}

void checkAlarm() // added by Gohst
{
  if (g_eeGeneral.disableAlarmWarning) return;
  if (g_eeGeneral.beeperMode == e_mode_quiet) ALERT(STR_ALARMSWARN, STR_ALARMSDISABLED, AU_ERROR);
}

void checkSwitches()
{
  uint8_t last_bad_switches = 0xff;
  uint8_t states = g_model.switchWarningStates;

  while (1) {

    getMovedSwitch();

    switches_states <<= 1;

    if ((states & 0x01) || (states == switches_states)) {
      return;
    }

    // first - display warning
    if (last_bad_switches != switches_states) {
      MESSAGE(STR_SWITCHWARN, NULL, STR_PRESSANYKEYTOSKIP, AU_SWITCH_ALERT);
      uint8_t x = 2;
      for (uint8_t i=1; i<8; i++) {
        uint8_t attr = (states & (1 << i)) == (switches_states & (1 << i)) ? 0 : INVERS;
        putsSwitches(x, 5*FH, (i>5?(i+1):(i>=4?(4+((states>>4)&0x3)):i)), attr);
        if (i == 4 && attr) i++;
        if (i != 4) x += 3*FW+FW/2;
      }
      refreshDisplay();
      last_bad_switches = switches_states;
    }

    if (keyDown() || check_soft_power() > e_power_trainer) return; // Usb on or power off

    checkBacklight();

    WDT_RESET_STOCK();

#ifdef SIMU
    if (!main_thread_running) return;
    sleep(1/*ms*/);
#endif
  }
}

void alert(const pm_char * t, const pm_char *s MESSAGE_SOUND_ARG)
{
  MESSAGE(t, s, STR_PRESSANYKEY, sound);

  while(1)
  {
#ifdef SIMU
    if (!main_thread_running) return;
    sleep(1/*ms*/);
#endif


    if (check_soft_power() >= e_power_usb) return; // Usb on or power off

    if (keyDown()) return;  // wait for key release

    checkBacklight();

    WDT_RESET_STOCK();
  }
}

void message(const pm_char *title, const pm_char *t, const char *last MESSAGE_SOUND_ARG)
{
  lcd_clear();

#if defined(PCBV4) || defined(PCBARM) || defined(M128) || defined(EXTSTD)
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
  lcd_filled_rect(0, 0, 128, 32);
  if (t) lcd_putsLeft(5*FH, t);
  if (last) {
    lcd_putsLeft(7*FH, last);
    AUDIO_ERROR_MESSAGE(sound);
  }
  refreshDisplay();
  lcdSetContrast();
  clearKeyEvents();
}

#if defined(PCBSTD)
uint8_t checkTrim(uint8_t event)
{
  int8_t k = (event & EVT_KEY_MASK) - TRM_BASE;
  int8_t s = g_model.trimInc;
  if (k>=0 && k<8 && !IS_KEY_BREAK(event)) {
#else
void checkTrims()
{
  uint8_t event = getEvent(true);
  if (event && !IS_KEY_BREAK(event)) {
    int8_t k = (event & EVT_KEY_MASK) - TRM_BASE;
    int8_t s = g_model.trimInc;
#endif
    // LH_DWN LH_UP LV_DWN LV_UP RV_DWN RV_UP RH_DWN RH_UP
    uint8_t idx = CONVERT_MODE(1+k/2) - 1;
    uint8_t phase = getTrimFlightPhase(s_perout_flight_phase, idx);
    int16_t before = getRawTrimValue(phase, idx);
    int8_t  v = (s==0) ? min(32, abs(before)/4+1) : 1 << (s-1); // 1=>1  2=>2  3=>4  4=>8
    bool thro = (idx==THR_STICK && g_model.thrTrim);
    if (thro) v = 4; // if throttle trim and trim trottle then step=4
    int16_t after = (k&1) ? before + v : before - v;   // positive = k&1
#if defined(PCBARM)
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
      if (!g_model.extendedTrims) after = before;
    }

    if (after < TRIM_EXTENDED_MIN) {
      after = TRIM_EXTENDED_MIN;
    }
    if (after > TRIM_EXTENDED_MAX) {
      after = TRIM_EXTENDED_MAX;
    }

    setTrimValue(phase, idx, after);

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
#if defined(PCBSTD)
    return 0;
#endif
  }
#if defined(PCBSTD)
  return event;
#endif
}

#if defined(PCBARM) && defined(REVB)
uint16_t Current_analogue;
uint16_t Current_max;
uint32_t Current_accumulator;
uint32_t Current_used;
uint16_t sessionTimer;
#define NUMBER_ANALOG   9
#else
#define NUMBER_ANALOG   8
#endif

#if !defined(SIMU)
static uint16_t s_anaFilt[NUMBER_ANALOG];
#endif

#if defined(SIMU)
uint16_t BandGap = 225;
#elif defined(PCBV4)
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
  //                     ana-in:   3 1 2 0 4 5 6 7
  //static pm_char crossAna[] PROGMEM ={4,2,3,1,5,6,7,0}; // wenn schon Tabelle, dann muss sich auch lohnen
  //                        Google Translate (German): // if table already, then it must also be worthwhile
#if defined(PCBARM)
  static const uint8_t crossAna[]={1,5,7,0,4,6,2,3,8};
#if defined(REVB)
  if ( chan == 8 ) {
    return Current_analogue ;
  }
#endif
#else
  static const pm_char crossAna[] PROGMEM ={3,1,2,0,4,5,6,7};
#endif

  volatile uint16_t *p = &s_anaFilt[pgm_read_byte(crossAna+chan)];
  return *p;
}

#if defined(PCBARM)
void getADC_filt()
{
        register uint32_t x ;
        static uint16_t t_ana[2][NUMBER_ANALOG] ;

        read_9_adc() ;
        for( x = 0 ; x < NUMBER_ANALOG ; x += 1 )
        {
                s_anaFilt[x] = s_anaFilt[x]/2 + (t_ana[1][x] >> 2 ) ;
                t_ana[1][x] = ( t_ana[1][x] + t_ana[0][x] ) >> 1 ;
                t_ana[0][x] = ( t_ana[0][x] + Analog_values[x] ) >> 1 ;
        }
}

void getADC_osmp()
{
  register uint32_t x;
  register uint32_t y;
  uint16_t temp[NUMBER_ANALOG];

  for( x = 0; x < NUMBER_ANALOG; x += 1 )
  {
    temp[x] = 0;
  }
  for( y = 0; y < 4; y += 1 )
  {
    read_9_adc();
    for( x = 0; x < NUMBER_ANALOG; x += 1 )
    {
      temp[x] += Analog_values[x];
    }
  }
  for( x = 0; x < NUMBER_ANALOG; x += 1 )
  {
    s_anaFilt[x] = temp[x] >> 3;
  }
}

void getADC_single()
{
  register uint32_t x ;

  read_9_adc() ;

  for( x = 0 ; x < NUMBER_ANALOG ; x += 1 )
  {
    s_anaFilt[x] = Analog_values[x] >> 1 ;
  }
}

#else

void getADC_filt()
{
  static uint16_t t_ana[2][8];
  for (uint8_t adc_input=0; adc_input<8; adc_input++) {
      ADMUX=adc_input|ADC_VREF_TYPE;
      // Start the AD conversion
      ADCSRA|=0x40;

    // Do this while waiting
    s_anaFilt[adc_input] = (s_anaFilt[adc_input]/2 + t_ana[1][adc_input]) & 0xFFFE; //gain of 2 on last conversion - clear last bit
    t_ana[1][adc_input]  = (t_ana[1][adc_input] + t_ana[0][adc_input]) >> 1;

      // Wait for the AD conversion to complete
      while ((ADCSRA & 0x10)==0);
      ADCSRA|=0x10;

      t_ana[0][adc_input]  = (t_ana[0][adc_input]  + ADCW) >> 1;
  }
}

void getADC_osmp()
{
  uint16_t temp_ana;

  for (uint8_t adc_input=0; adc_input<8; adc_input++) {
    temp_ana = 0;
    ADMUX = adc_input|ADC_VREF_TYPE;
    for (uint8_t i=0; i<4;i++) {  // Going from 10bits to 11 bits.  Addition = n.  Loop 4^n times
      // Start the AD conversion
      ADCSRA|=0x40;
      // Wait for the AD conversion to complete
      while ((ADCSRA & 0x10)==0);
      ADCSRA|=0x10;
      temp_ana += ADCW;
    }
    s_anaFilt[adc_input] = temp_ana / 2; // divide by 2^n to normalize result.
  }
}

void getADC_single()
{
  for (uint8_t adc_input=0; adc_input<8; adc_input++) {
      ADMUX=adc_input|ADC_VREF_TYPE;
      // Start the AD conversion
      ADCSRA|=0x40;
      // Wait for the AD conversion to complete
      while ((ADCSRA & 0x10)==0);
      ADCSRA|=0x10;
      s_anaFilt[adc_input]= ADCW * 2; // use 11 bit numbers
    }
}
#endif

#if !defined(PCBARM)
void getADC_bandgap()
{
#if defined (PCBV4)
  static uint8_t s_bgCheck = 0;
  static uint16_t s_bgSum = 0;
  ADCSRA|=0x40; // request sample
  s_bgCheck += 32;
  while ((ADCSRA & 0x10)==0); ADCSRA|=0x10; // wait for sample
  if (s_bgCheck == 0) { // 8x over-sample (256/32=8)
    BandGap = s_bgSum+ADCW;
    s_bgSum = 0;
  }
  else {
    s_bgSum += ADCW;
  }
  ADCSRB |= (1<<MUX5);
#else
  // TODO is the next line needed (because it has been called before perMain)?
  ADMUX=0x1E|ADC_VREF_TYPE; // Switch MUX to internal 1.22V reference
  ADCSRA|=0x40;
  while ((ADCSRA & 0x10)==0);
  ADCSRA|=0x10; // take sample
  BandGap=ADCW;
#endif
}
#endif

#endif // SIMU

uint8_t g_vbat100mV = 0;
uint16_t g_LightOffCounter;

#if !defined(PCBARM)
FORCEINLINE bool checkSlaveMode()
{
  // no power -> only phone jack = slave mode

#if defined(PCBV4)
  return SLAVE_MODE;
#else
  static bool lastSlaveMode = false;
  static uint8_t checkDelay = 0;
  if (IS_AUDIO_BUSY()) {
    checkDelay = 20;
  }
  else if (checkDelay) {
    --checkDelay;
  }
  else {
    lastSlaveMode = SLAVE_MODE;
  }
  return lastSlaveMode;
#endif
}
#endif

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
  s_timerVal[idx] = g_model.timers[idx].val;
  s_timerVal_10ms[idx] = 0 ;
}

void resetAll()
{
  // TODO s_traceCnt to be reset?
  resetTimer(0);
  resetTimer(1);
#ifdef FRSKY
  resetTelemetry();
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

int16_t  sDelay[MAX_MIXERS] = {0};
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
      int32_t vv = ((int32_t)trim-TRIM_MIN)*(RESX-v)/(2*RESX);
      trim = vv;
    }
    else if (trimsCheckTimer > 0) {
      trim = 0;
    }

    trims[i] = trim*2;
  }
}

uint8_t s_perout_mode = e_perout_mode_normal;

BeepANACenter evalSticks()
{
  BeepANACenter anaCenter = 0;

#ifdef HELI
  uint16_t d = 0;
  if (g_model.swashR.value) {
    uint32_t v = (int32_t(calibratedStick[ELE_STICK])*calibratedStick[ELE_STICK] +
        int32_t(calibratedStick[AIL_STICK])*calibratedStick[AIL_STICK]);
    uint32_t q = int32_t(RESX)*g_model.swashR.value/100;
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

    if (g_eeGeneral.throttleReversed && ch==THR_STICK)
      v = -v;

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
      if (s_perout_mode == e_perout_mode_normal && (isFunctionActive(FUNC_TRAINER) || isFunctionActive(FUNC_TRAINER_RUD+ch))) {
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
        v = int32_t(v)*g_model.swashR.value*RESX/(int32_t(d)*100);
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

uint16_t activeFunctions = 0;
MASK_FSW_TYPE activeFunctionSwitches = 0;

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
        int16_t converted_value = applyChannelRatio(idx, val);
        if (g_model.frsky.channels[idx].type >= UNIT_RAW) {
          converted_value /= 10;
        }
        else {
          if (converted_value < 1000) {
            att |= PREC2;
          }
          else {
            converted_value /= 10;
            att |= PREC1;
          }
        }
        PLAY_NUMBER(converted_value, 1+g_model.frsky.channels[idx].type, att);
        break;
      }

    case NUM_XCHNRAW+TELEM_CELL-1:
      PLAY_NUMBER(val, 1+UNIT_VOLTS, PREC2);
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
      PLAY_NUMBER(val, 1+UNIT_G, PREC2);
      break;

    case NUM_XCHNRAW+TELEM_VSPD-1:
      PLAY_NUMBER(val, 1+UNIT_METERS_PER_SECOND, PREC2);
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

#if defined(PCBARM)
static uint8_t currentSpeakerVolume = 255;
uint8_t requiredSpeakerVolume;
#endif

void evalFunctions()
{
  assert((int)(sizeof(activeFunctions)*8) > (int)(FUNC_MAX-NUM_CHNOUT));

  uint16_t newActiveFunctions = 0;

  for (uint8_t i=0; i<NUM_CHNOUT; i++)
    safetyCh[i] = -128; // not defined

  for (uint8_t i=0; i<NUM_FSW; i++) {
    FuncSwData *sd = &g_model.funcSw[i];
    int8_t swtch = sd->swtch;
    if (swtch) {
      uint16_t function_mask = (sd->func >= FUNC_TRAINER ? (1 << (sd->func-FUNC_TRAINER)) : 0);
      MASK_FSW_TYPE switch_mask = ((MASK_FSW_TYPE)1 << i);
      uint8_t momentary = 0;
      if (swtch > MAX_SWITCH+1) {
        momentary = 1;
        swtch -= MAX_SWITCH+1;
      }
      if (swtch < -MAX_SWITCH-1) {
        momentary = 1;
        swtch += MAX_SWITCH+1;
      }
      if (getSwitch(swtch, 0)) {
        if (sd->delay) {
          if (sd->func < FUNC_TRAINER) {
            safetyCh[sd->func] = FSW_PARAM(sd);
          }

          if (~activeFunctions & function_mask) {
            if (sd->func == FUNC_INSTANT_TRIM) {
              if (g_menuStack[0] == menuMainView
#if defined(FRSKY)
                || g_menuStack[0] == menuProcFrsky
#endif
                )
                instantTrim();
            }
          }
        }
        else if (sd->func <= FUNC_INSTANT_TRIM) {
          function_mask = 0;
        }

#if defined(SDCARD)
        if (sd->func == FUNC_LOGS) {
          logDelay = FSW_PARAM(sd);
        }
#endif

        if (~activeFunctionSwitches & switch_mask) {
          if (sd->func == FUNC_RESET) {
            switch (FSW_PARAM(sd)) {
              case 0:
              case 1:
                resetTimer(FSW_PARAM(sd));
                break;
              case 2:
                resetAll();
                break;
#ifdef FRSKY
              case 3:
                resetTelemetry();
                break;
#endif
            }
          }
        }

        if ((!momentary) || (~activeFunctionSwitches & switch_mask)) {
          if (sd->func == FUNC_PLAY_SOUND) {
            AUDIO_PLAY(AU_FRSKY_FIRST+FSW_PARAM(sd));
          }

#if defined(HAPTIC)
          if (sd->func == FUNC_HAPTIC) {
            haptic.event(AU_FRSKY_LAST+FSW_PARAM(sd));
          }
#endif

#if defined(PCBARM) && defined(SDCARD)
          else if (sd->func == FUNC_PLAY_TRACK) {
            if (IS_PLAYING(i+1)) {
              switch_mask = 0;
            }
            else {
              char lfn[] = SOUNDS_PATH "/xxxxxx.wav";
              strncpy(lfn+sizeof(SOUNDS_PATH), sd->param, sizeof(sd->param));
              lfn[sizeof(SOUNDS_PATH)+sizeof(sd->param)] = '\0';
              strcat(lfn+sizeof(SOUNDS_PATH), SOUNDS_EXT);
              PLAY_FILE(lfn, i+1);
            }
          }
          else if (sd->func == FUNC_PLAY_VALUE) {
            if (IS_PLAYING(i+1)) {
              switch_mask = 0;
            }
            else {
              PLAY_VALUE(FSW_PARAM(sd), i+1);
            }
          }
          else if (sd->func == FUNC_VOLUME) {
            requiredSpeakerVolume = ((1024 + getValue(FSW_PARAM(sd))) * NUM_VOL_LEVELS) / 2048;
          }
#elif defined(VOICE)
          else if (sd->func == FUNC_PLAY_TRACK) {
            if (IS_PLAYING(i+1)) {
              switch_mask = 0;
            }
            else {
              PUSH_CUSTOM_PROMPT(sd->param, i+1);
            }
          }
          else if (sd->func == FUNC_PLAY_VALUE) {
            if (IS_PLAYING(i+1)) {
              switch_mask = 0;
            }
            else {
              PLAY_VALUE(FSW_PARAM(sd), i+1);
            }
          }
#endif

#if defined(DEBUG)
          else if (sd->func == FUNC_TEST) {
            testFunc();
          }
#endif
        }

        if (momentary) {
          if (~activeFunctionSwitches & switch_mask) {
            if (~activeFunctions & function_mask) {
              newActiveFunctions |= function_mask;
            }
          }
          else {
            newActiveFunctions |= (activeFunctions & function_mask);
          }
        }
        else {
          newActiveFunctions |= function_mask;
        }

        activeFunctionSwitches |= switch_mask;
      }
      else {
        activeFunctionSwitches &= (~switch_mask);
        if (momentary)
          newActiveFunctions |= (activeFunctions & function_mask);
      }
    }
  }

  activeFunctions = newActiveFunctions;
}

uint8_t s_perout_flight_phase;
void perOut(uint8_t tick10ms)
{
  BeepANACenter anaCenter = evalSticks();

  if (s_perout_mode == e_perout_mode_normal) {
    //===========BEEP CENTER================
    anaCenter &= g_model.beepANACenter;
    if(((bpanaCenter ^ anaCenter) & anaCenter)) AUDIO_POT_STICK_MIDDLE();
    bpanaCenter = anaCenter;
  }

#ifdef HELI
  if(g_model.swashR.value)
  {
    uint32_t v = ((int32_t)anas[ELE_STICK]*anas[ELE_STICK] + (int32_t)anas[AIL_STICK]*anas[AIL_STICK]);
    uint32_t q = (int32_t)RESX*g_model.swashR.value/100;
    q *= q;
    if(v>q)
    {
      uint16_t d = isqrt32(v);
      anas[ELE_STICK] = (int32_t)anas[ELE_STICK]*g_model.swashR.value*RESX/((int32_t)d*100);
      anas[AIL_STICK] = (int32_t)anas[AIL_STICK]*g_model.swashR.value*RESX/((int32_t)d*100);
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

  for (uint8_t i=0; i<MAX_MIXERS; i++) {

    MixData *md = mixaddress( i ) ;

    if (md->srcRaw==0) break;

    if (md->phases & (1<<s_perout_flight_phase)) continue;

    //========== SWITCH ===============
    bool sw = getSwitch(md->swtch, 1);

    //========== VALUE ===============
    //Notice 0 = NC switch means not used -> always on line
    uint8_t k = md->srcRaw-1;
    int16_t v = 0;
    if (s_perout_mode != e_perout_mode_normal) {
      if (!sw || k >= NUM_STICKS || (k == THR_STICK && g_model.thrTrim)) {
        continue;
      }
      else {
        if (!(s_perout_mode & e_perout_mode_nosticks))
          v = anas[k];
      }
    }
    else {
      if (k < NUM_STICKS)
        v = md->noExpo ? rawAnas[k] : anas[k]; //Switch is on. MAX=FULL=512 or value.
      else if (k>=MIXSRC_CH1-1 && k<=MIXSRC_CH16-1 && k-MIXSRC_CH1+1<md->destCh) // if we've already calculated the value - take it instead
        v = chans[k-MIXSRC_CH1+1] / 100;
      else if (k>=MIXSRC_THR-1 && k<=MIXSRC_SWC-1) {
        v = getSwitch(k-MIXSRC_THR+1+1, 0) ? +1024 : -1024;
        if (v<0 && !md->swtch)
          sw = false;
      }
      else {
        v = getValue(k <= MIXSRC_3POS ? k : k-MAX_SWITCH);
      }
    }

    //========== DELAYS ===============
    uint8_t swTog;
    bool apply_offset = true;
    if (sw) { // switch on?  (if no switch selected => on)
      swTog = !swOn[i];
      if (s_perout_mode == e_perout_mode_normal) {
        swOn[i] = true;
        if (md->delayUp) {
          if (swTog) {
            if (sDelay[i])
              sDelay[i] = 0;
            else
              sDelay[i] = md->delayUp * 50;
          }
          if (sDelay[i] > 0) { // perform delay
            sDelay[i] -= tick10ms;
            if (!md->swtch) {
              v = -1024;
            }
            else {
              continue;
            }
          }
        }
        if (md->mixWarn) lv_mixWarning |= 1<<(md->mixWarn-1); // Mix warning
      }
    }
    else {
      bool has_delay = false;
      swTog = swOn[i];
      swOn[i] = false;
      if (md->delayDown) {
        if (swTog) {
          if (sDelay[i])
            sDelay[i] = 0;
          else
            sDelay[i] = md->delayDown * 50;
        }
        if (sDelay[i] > 0) { // perform delay
          sDelay[i] -= tick10ms;
          if (!md->swtch) v = +1024;
          has_delay = true;
        }
        else if (!md->swtch) {
          v = -1024;
        }
      }
      if (!has_delay) {
        if (md->speedDown) {
          if (md->mltpx==MLTPX_REP) continue;
          if (md->swtch) { v = 0; apply_offset = false; }
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
    if (apply_offset && md->sOffset) v += calc100toRESX(md->sOffset);

    //========== TRIMS ===============
    if (!(s_perout_mode & e_perout_mode_notrims)) {
      int8_t mix_trim = md->carryTrim;
      if (mix_trim < TRIM_ON)
        mix_trim = -mix_trim-1;
      else if (mix_trim == TRIM_ON && k < NUM_STICKS)
        mix_trim = k;
      else
        mix_trim = -1;
      if (mix_trim >= 0)
        v += trims[mix_trim];
    }

    //========== SPEED ===============
    if (s_perout_mode == e_perout_mode_normal && (md->speedUp || md->speedDown))  // there are delay values
    {
#define DEL_MULT 256

      int16_t diff = v-act[i]/DEL_MULT;

      if (diff) {
        //rate = steps/sec => 32*1024/100*md->speedUp/Down
        //act[i] += diff>0 ? (32768)/((int16_t)100*md->speedUp) : -(32768)/((int16_t)100*md->speedDown);
        //-100..100 => 32768 ->  100*83886/256 = 32768,   For MAX we divide by 2 since it's asymmetrical
        if (tick10ms) {
            int32_t rate = (int32_t)DEL_MULT*2048*100*tick10ms;
            if(md->weight) rate /= abs(md->weight);

            act[i] = (diff>0) ? ((md->speedUp>0)   ? act[i]+(rate)/((int16_t)50*md->speedUp)   :  (int32_t)v*DEL_MULT) :
                                ((md->speedDown>0) ? act[i]-(rate)/((int16_t)50*md->speedDown) :  (int32_t)v*DEL_MULT) ;
        }

        {
          int32_t tmp = act[i]/DEL_MULT ;
          if(((diff>0) && (v<tmp)) || ((diff<0) && (v>tmp))) act[i]=(int32_t)v*DEL_MULT; //deal with overflow
        }

        v = act[i]/DEL_MULT;
      }
    }

    //========== CURVES ===============
    if (md->curveParam && md->curveMode == MODE_CURVE) {
      v = applyCurve(v, md->curveParam);
    }

    //========== WEIGHT ===============
    int32_t dv = (int32_t)v*md->weight;

    //========== DIFFERENTIAL =========
    if (md->curveMode == MODE_DIFFERENTIAL) {
      if (md->curveParam>0 && dv<0)
        dv = (dv * (100-md->curveParam)) / 100;
      else if (md->curveParam<0 && dv>0)
        dv = (dv * (100+md->curveParam)) / 100;
    }

    int32_t *ptr = &chans[md->destCh]; // Save calculating address several times
    switch(md->mltpx){
      case MLTPX_REP:
        *ptr = dv;
#ifdef BOLD_FONT
        for (uint8_t m=i-1; m<MAX_MIXERS && mixaddress(m)->destCh == md->destCh; m--)
          activeMixes &= ~((ACTIVE_MIXES_TYPE)1 << m);
#endif
        break;
      case MLTPX_MUL:
        dv /= 100;
        dv *= *ptr;
        dv /= RESXl;
        *ptr = dv;
        break;
      default:  // MLTPX_ADD
        *ptr += dv; //Mixer output add up to the line (dv + (dv>0 ? 100/2 : -100/2))/(100);
        break;
      }
  }

  mixWarning = lv_mixWarning;
}

#ifdef DISPLAY_USER_DATA
char userDataDisplayBuf[TELEM_SCREEN_BUFFER_SIZE];
#endif

#if defined(SIMU)
#define TIME_TO_WRITE s_eeDirtyMsk
#else
#define TIME_TO_WRITE (s_eeDirtyMsk && (tmr10ms_t)(get_tmr10ms() - s_eeDirtyTime10ms) >= (tmr10ms_t)WRITE_DELAY_10MS)
#endif

#ifdef BOLD_FONT
ACTIVE_MIXES_TYPE activeMixes;
#endif
int32_t sum_chans512[NUM_CHNOUT] = {0};
inline void doMixerCalculations(tmr10ms_t tmr10ms, uint8_t tick10ms)
{
#ifdef BOLD_FONT
  activeMixes = 0;
#endif

  if (g_eeGeneral.filterInput == 1) {
    getADC_filt() ;
  }
  else if ( g_eeGeneral.filterInput == 2) {
    getADC_osmp() ;
  }
  else {
    getADC_single() ;
  }

#if defined(PCBARM) && defined(REVB) && !defined(SIMU)
  Current_analogue = ( Current_analogue * 31 + s_anaFilt[8] ) >> 5 ;
  if (Current_analogue > Current_max)
    Current_max = Current_analogue ;
#elif defined(PCBV4) && !defined(SIMU)
  // For PCB V4, use our own 1.2V, external reference (connected to ADC3)
  ADCSRB &= ~(1<<MUX5);
  ADMUX = 0x03|ADC_VREF_TYPE; // Switch MUX to internal reference
#elif defined(PCBSTD) && !defined(SIMU)
  ADMUX = 0x1E|ADC_VREF_TYPE; // Switch MUX to internal reference
#endif

#define MAX_ACT 0xffff
  static uint16_t fp_act[MAX_PHASES] = {0};
  static uint16_t delta = 0;
  static uint8_t s_fade_flight_phases = 0;
  static uint8_t s_last_phase = 255;
  uint8_t phase = getFlightPhase();
  int32_t weight = 0;

  if (s_last_phase != phase) {
    if (s_last_phase == 255) {
      fp_act[phase] = MAX_ACT;
    }
    else {
      uint8_t fadeTime = max(g_model.phaseData[s_last_phase].fadeOut, g_model.phaseData[phase].fadeIn);
      if (fadeTime) {
        s_fade_flight_phases |= (1<<s_last_phase) + (1<<phase);
        delta = (MAX_ACT / 50) / fadeTime;
      }
      else {
        fp_act[s_last_phase] = 0;
        fp_act[phase] = MAX_ACT;
        s_fade_flight_phases &= ~((1<<s_last_phase) + (1<<phase));
      }
    }
    s_last_phase = phase;
  }

  if (s_fade_flight_phases) {
    memclear(sum_chans512, sizeof(sum_chans512));
    weight = 0;
    for (uint8_t p=0; p<MAX_PHASES; p++) {
      if (s_fade_flight_phases & (1<<p)) {
        s_perout_flight_phase = p;
        perOut(tick10ms);
        for (uint8_t i=0; i<NUM_CHNOUT; i++)
          sum_chans512[i] += (chans[i] / 16) * fp_act[p];
        weight += fp_act[p];
      }
    }
    assert(weight);
    s_perout_flight_phase = phase;
  }
  else {
    s_perout_flight_phase = phase;
    perOut(tick10ms);
  }

  //========== LIMITS ===============
  for (uint8_t i=0;i<NUM_CHNOUT;i++) {
    // chans[i] holds data from mixer.   chans[i] = v*weight => 1024*100
    // later we multiply by the limit (up to 100) and then we need to normalize
    // at the end chans[i] = chans[i]/100 =>  -1024..1024
    // interpolate value with min/max so we get smooth motion from center to stop
    // this limits based on v original values and min=-1024, max=1024  RESX=1024
    int32_t q = (s_fade_flight_phases ? (sum_chans512[i] / weight) * 16 : chans[i]);
    ex_chans[i] = q / 100; // for the next perMain

    int16_t value = applyLimits(i, q);

    cli();
    g_chans512[i] = value;  // copy consistent word to int-level
    sei();
  }

  // Bandgap has had plenty of time to settle...
#if !defined(PCBARM)
  getADC_bandgap();
#endif

  if (!tick10ms) return; //make sure the rest happen only every 10ms.

  /* Throttle trace */
  int16_t val;

  if (g_model.thrTraceSrc == 0) {
    val = calibratedStick[THR_STICK]; // get throttle channel value
  }
  else if (g_model.thrTraceSrc > NUM_POTS) {
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
    val = val * 10 / (10+(g_model.limitData[ch].max-g_model.limitData[ch].min)/20);
    val -= RESX;
  }
  else {
    val = calibratedStick[g_model.thrTraceSrc+NUM_STICKS-1];
  }

  val += RESX;
  val /= (RESX/16); // calibrate it

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

    if ((tmr10ms_t)(tmr10ms - s_time_trace) >= 1000) {// 10sec
      s_time_trace += 1000;
      val = s_sum_10s / s_cnt_10s;
      s_sum_10s = 0;
      s_cnt_10s = 0;

      s_traceBuf[s_traceWr++] = val;
      if (s_traceWr >= MAXTRACE) s_traceWr = 0;
      if (s_traceCnt >= 0) s_traceCnt++; // TODO to be checked
    }
#endif

    s_cnt_1s = 0;
    s_sum_1s = 0;
  }

  // Timers start
  for (uint8_t i=0; i<2; i++) {
    int8_t tm = g_model.timers[i].mode;
    uint16_t tv = g_model.timers[i].val;

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
      if (g_eeGeneral.preBeep && g_model.timers[0].val) { // beep when 30, 15, 10, 5,4,3,2,1 seconds remaining
        if(s_timerVal[0]==30) AUDIO_TIMER_30(); //beep three times
        if(s_timerVal[0]==20) AUDIO_TIMER_20(); //beep two times
        if(s_timerVal[0]==10) AUDIO_TIMER_10();
        if(s_timerVal[0]<= 3) AUDIO_TIMER_LT3(s_timerVal[0]);
      }

      if (g_eeGeneral.minuteBeep && (((g_model.timers[0].val ? g_model.timers[0].val-s_timerVal[0] : s_timerVal[0])%60)==0)) { // short beep every minute
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
    for (uint8_t p=0; p<MAX_PHASES; p++) {
      if (s_fade_flight_phases & (1<<p)) {
        if (p == phase) {
          if (MAX_ACT - fp_act[p] > delta * tick10ms)
            fp_act[p] += delta * tick10ms;
          else {
            fp_act[p] = MAX_ACT;
            s_fade_flight_phases -= (1<<p);
          }
        }
        else {
          if (fp_act[p] > delta * tick10ms)
            fp_act[p] -= delta * tick10ms;
          else {
            fp_act[p] = 0;
            s_fade_flight_phases -= (1<<p);
          }
        }
      }
    }
  }

#if defined(PCBARM)
  requiredSpeakerVolume = g_eeGeneral.speakerVolume;
#endif

  evalFunctions();

#if defined(DSM2)
  if (s_rangecheck_mode) AUDIO_PLAY(AU_FRSKY_CHEEP);
#endif
      
#if defined(PCBARM)
  if (currentSpeakerVolume != requiredSpeakerVolume) {
    setVolume(requiredSpeakerVolume);
    currentSpeakerVolume = requiredSpeakerVolume;
  }
#endif
}

void perMain()
{
  static tmr10ms_t lastTMR;
  tmr10ms_t tmr10ms = get_tmr10ms();
  uint8_t tick10ms = (tmr10ms >= lastTMR ? tmr10ms - lastTMR : 1);
  lastTMR = tmr10ms;

#if defined(PCBSTD) || defined(SIMU)
  doMixerCalculations(tmr10ms, tick10ms);
#endif

// TODO same code here + integrate the timer which could be common
#if defined(PCBARM)
  if (Tenms) {
    Tenms = 0 ;

    if (Eeprom32_process_state != E32_IDLE)
      ee32_process();
    else if (TIME_TO_WRITE)
      eeCheck();

    Current_accumulator += Current_analogue ;
    static uint32_t OneSecTimer;
    if (++OneSecTimer >= 100) {
      OneSecTimer -= 100 ;
      sessionTimer += 1;
      Current_used += Current_accumulator / 100 ;                     // milliAmpSeconds (but scaled)
      Current_accumulator = 0 ;
    }

#if defined(SDCARD) && !defined(SIMU)
    sdPoll10mS();
#endif
  }
#else
  if (!eeprom_buffer_size) {
    if (theFile.isWriting())
      theFile.nextWriteStep();
    else if (TIME_TO_WRITE)
      eeCheck();
  }
#endif

  if (!tick10ms) return; //make sure the rest happen only every 10ms.

#if defined(SDCARD)
#if defined(PCBV4)
  static uint8_t mountTimer;
  if (mountTimer-- == 0) {
    mountTimer = 100;
    if (g_FATFS_Obj.fs_type == 0) {
      f_mount(0, &g_FATFS_Obj);
    }
  }
#endif

  writeLogs();
#endif

#if defined(FRSKY) && defined(DISPLAY_USER_DATA)
  char userDataRxBuffer[21]; // Temp buffer used to collect fr-sky user data

  // retrieve bytes from user data buffer and insert into display string,
  // scrolling at the 21 character mark (edge of screen)
  uint8_t numbytes = frskyGetUserData(userDataRxBuffer, 21); // Get as many bytes as we can

  static uint8_t displayBufferIndex;
  for (uint8_t byt=0; byt < numbytes; byt++) 
  {
    displayBufferIndex++;
    if (displayBufferIndex > 20)
    {
      for (int xx=0; xx<20; xx++) // scroll one char left
        userDataDisplayBuf[xx] = userDataDisplayBuf[xx+1];
      displayBufferIndex = 20;
    }
    userDataDisplayBuf[displayBufferIndex] = userDataRxBuffer[byt];
  }
#endif

  lcd_clear();
  
#if defined(PCBARM) && defined(SIMU)
  checkTrims();
#endif

#if defined(PCBSTD)
  uint8_t evt = getEvent();
  evt = checkTrim(evt);
#else
  uint8_t evt = getEvent(false);
#endif

  if (g_LightOffCounter) g_LightOffCounter--;
  if (evt && (g_eeGeneral.backlightMode & e_backlight_mode_keys)) backlightOn(); // on keypress turn the light on

  checkBacklight();

#if defined(PCBARM) && defined(FRSKY)
  check_frsky();
#endif

  g_menuStack[g_menuStackPtr](evt);
  drawStatusLine();
  refreshDisplay();

#if defined(PCBARM)
  if ( check_soft_power() == e_power_trainer ) {          // On trainer power
    PIOC->PIO_PDR = PIO_PC22 ;                            // Disable bit C22 Assign to peripheral
  }
  else {
    PIOC->PIO_PER = PIO_PC22 ;                            // Enable bit C22 as input
  }
#elif defined(PCBV4)
  // PPM signal on phono-jack. In or out? ...
  if(checkSlaveMode()) {
    PORTG |= (1<<OUT_G_SIM_CTL); // 1=ppm out
  }
  else{
    PORTG &=  ~(1<<OUT_G_SIM_CTL); // 0=ppm in
  }
#elif defined(PCBSTD)
  // PPM signal on phono-jack. In or out? ...
  if(checkSlaveMode()) {
    PORTG &= ~(1<<OUT_G_SIM_CTL); // 0=ppm out
  }
  else{
    PORTG |=  (1<<OUT_G_SIM_CTL); // 1=ppm-in
  }
#endif

#if defined(PCBARM)
  static uint8_t counter = 0;
  if (counter-- == 0) {
    counter = 10;
#else
  if ((tmr10ms & 0x1f) == 2 /*every 10ms*32*/ || g_menuStack[g_menuStackPtr] == menuProcDiagAna) {
#endif

#if defined(PCBARM)
    int32_t instant_vbat = anaIn(7);
    instant_vbat = ( instant_vbat + instant_vbat*(g_eeGeneral.vBatCalib)/128 ) * 4191 ;
    instant_vbat /= 55296  ;
#elif defined(PCBV4)
    uint16_t instant_vbat = anaIn(7);
    instant_vbat = ((uint32_t)instant_vbat*1112 + (int32_t)instant_vbat*g_eeGeneral.vBatCalib + (BandGap<<2)) / (BandGap<<3);
#else
    uint16_t instant_vbat = anaIn(7);
    instant_vbat = (instant_vbat*16 + instant_vbat*g_eeGeneral.vBatCalib/8) / BandGap;
#endif

    static uint8_t  s_batCheck;
    static uint16_t s_batSum;

    s_batCheck += 32;
    s_batSum += instant_vbat;

    if (g_vbat100mV == 0 || g_menuStack[g_menuStackPtr] == menuProcDiagAna) {
      g_vbat100mV = instant_vbat;
      s_batSum = 0;
      s_batCheck = 0;
    }
    else if (s_batCheck == 0) {
      g_vbat100mV = s_batSum / 8;
      s_batSum = 0;
      if (g_vbat100mV <= g_eeGeneral.vBatWarn && g_vbat100mV>50) {
        AUDIO_TX_BATTERY_LOW();
      }
#if defined(PCBARM)
      else if (temperature >= g_eeGeneral.temperatureWarn+80) {
        AUDIO_TX_TEMP_HIGH();
      }
      else if (g_eeGeneral.mAhWarn && (g_eeGeneral.mAhUsed + Current_used * (488 + g_eeGeneral.currentCalib)/8192/36) / 500 >= g_eeGeneral.mAhWarn) {
        AUDIO_TX_MAH_HIGH();
      }
#endif
    }
  }
}

int16_t g_ppmIns[8];
uint8_t ppmInState = 0; //0=unsync 1..8= wait for value i-1

#if !defined(SIMU) && !defined(PCBARM)

volatile uint8_t g_tmr16KHz; //continuous timer 16ms (16MHz/1024/256) -- 8-bit counter overflow
#if defined (PCBV4)
ISR(TIMER2_OVF_vect)
#else
ISR(TIMER0_OVF_vect) // TODO now NOBLOCK in er9x
#endif
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
#if defined (PCBV4)
    uint8_t lb  = TCNT2;
#else
    uint8_t lb  = TCNT0;
#endif
    if(hb-g_tmr16KHz==0) return (hb<<8)|lb;
  }
}

#if defined (PCBV4)
ISR(TIMER5_COMPA_vect, ISR_NOBLOCK) // mixer interrupt
{
  cli();
  TIMSK5 &= ~(1<<OCIE5A); //stop reentrance
  OCR5A = 0x7d * 40; /* 20ms */
  sei();

  uint16_t t0 = getTmr16KHz();  
  
  static tmr10ms_t lastTMR;
  tmr10ms_t tmr10ms = get_tmr10ms();
  uint8_t tick10ms = (tmr10ms >= lastTMR ? tmr10ms - lastTMR : 1);
  lastTMR = tmr10ms;
  
  if (s_current_protocol < PROTO_NONE) {
    checkTrims();
    doMixerCalculations(tmr10ms, tick10ms);
  }

  heartbeat |= HEART_TIMER10ms;

  if (heartbeat == HEART_TIMER_PULSES+HEART_TIMER10ms) {
    wdt_reset();
    heartbeat = 0;
  }

  t0 = getTmr16KHz() - t0;
  g_timeMainLast = t0 / 8;
  if (t0 > g_timeMainMax) g_timeMainMax = t0 ;

  cli();
  TIMSK5 |= (1<<OCIE5A); //stop reentrance
  sei();
}
#endif

#if defined (PCBV4)
ISR(TIMER2_COMPA_vect, ISR_NOBLOCK) //10ms timer
#else
// Clocks every 64 uS
ISR(TIMER0_COMP_vect, ISR_NOBLOCK) //10ms timer
#endif
{
  cli();
  
#if defined(PCBV4)
  TIMSK2 &= ~(1<<OCIE2A); // stop reentrance
#else
  TIMSK &= ~(1<<OCIE0); // stop reentrance
#endif

  sei();

#if defined(PCBV4)
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

#if defined(FRSKY)
  if (cnt10ms == 30) {
    check_frsky();
  }
#endif

  if (cnt10ms-- == 0) { // BEGIN { ... every 10ms ... }
    // Begin 10ms event
    cnt10ms = 77;
#endif

    AUDIO_HEARTBEAT();

#ifdef HAPTIC
    HAPTIC_HEARTBEAT();
#endif

    per10ms();

#if defined(PCBV4) && defined(SDCARD)
    sdPoll10mS();
#endif

#if !defined(PCBV4)
    heartbeat |= HEART_TIMER10ms; // TODO check heartbeat everywhere!
#endif

#if defined(PCBSTD) && (defined(AUDIO) || defined(VOICE))
  } // end 10ms event
#endif

  cli();
  
#if defined(PCBV4)
  TIMSK2 |= (1<<OCIE2A);
#else
  TIMSK |= (1<<OCIE0);
#endif

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
#if defined (PCBV4)
  TIMSK3 &= ~(1<<ICIE3);
#else
  ETIMSK &= ~(1<<TICIE3);
#endif
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
#if defined (PCBV4)
  TIMSK3 |= (1<<ICIE3);
#else
  ETIMSK |= (1<<TICIE3);
#endif
}

/*
// gruvin: Fuse declarations work if we use the .elf file for AVR Studio (v4)
// instead of the Intel .hex files.  They should also work with AVRDUDE v5.10
// (reading from the .hex file), since a bug relating to Intel HEX file record
// interpretation was fixed. However, I leave these commented out, just in case
// it causes trouble for others.
#if defined (PCBV4)
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

#if defined (FRSKY) && !defined(PCBARM)
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

#if defined (DSM2_SERIAL) && !defined(PCBARM)
FORCEINLINE void DSM2_USART0_vect()
{
  UDR0 = *((uint16_t*)pulses2MHzRPtr);

  pulses2MHzRPtr += sizeof(uint16_t);
  if (pulses2MHzRPtr == pulses2MHzWPtr) {
    UCSR0B &= ~(1 << UDRIE0); // disable UDRE0 interrupt
  }
}
#endif

#if !defined(SIMU) && !defined(PCBARM)
#if defined (FRSKY) or defined(DSM2_SERIAL)
ISR(USART0_UDRE_vect)
{
#if defined (FRSKY) and defined (DSM2_SERIAL)
  if (g_model.protocol == PROTO_DSM2) {
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
  s_perout_mode = e_perout_mode_notrainer;
  evalSticks();
  s_perout_mode = e_perout_mode_normal;

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

  s_perout_mode = e_perout_mode_noinput;
  perOut(0); // do output loop - zero input sticks and trims
  for (uint8_t i=0; i<NUM_CHNOUT; i++) {
    zeros[i] = applyLimits(i, chans[i]);
  }

  s_perout_mode = e_perout_mode_nosticks+e_perout_mode_notrainer;
  perOut(0); // do output loop - only trims
  s_perout_mode = e_perout_mode_normal;

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

  STORE_MODELVARS;
  AUDIO_WARNING2();
}

#if defined(PCBARM) || defined(PCBV4)
void saveTimers()
{
  for (uint8_t i=0; i<MAX_TIMERS; i++) {
    if (g_model.timersXtra[i].remanent) {
      if (g_model.timersXtra[i].value != s_timerVal[i]) {
        g_model.timersXtra[i].value = s_timerVal[i];
        eeDirty(EE_MODEL);
      }
    }
  }
}
#endif

#if defined(ROTARY_ENCODERS)
// Rotary encoder interrupts
#if defined(PCBARM)
volatile uint32_t g_rotenc[ROTARY_ENCODERS] = {0};
#else
volatile uint8_t g_rotenc[ROTARY_ENCODERS] = {0};
#endif
#endif

#ifndef SIMU

#if defined(PCBV4) && defined(ROTARY_ENCODERS)

#if !defined(EXTRA_ROTARY_ENCODERS)
ISR(INT2_vect)
{
  uint8_t input = PIND & 0b00001100;
  if (input == 0 || input == 0b00001100) incRotaryEncoder(0, -1);
}
ISR(INT3_vect)
{
  uint8_t input = PIND & 0b00001100;
  if (input == 0 || input == 0b00001100) incRotaryEncoder(0, +1);
}
#endif //!EXTRA_ROTARY_ENCODERS

ISR(INT5_vect)
{
  uint8_t input = PINE & 0b01100000;
  if (input == 0 || input == 0b01100000) incRotaryEncoder(1, +1);
}
ISR(INT6_vect)
{
  uint8_t input = PINE & 0b01100000;
  if (input == 0 || input == 0b01100000) incRotaryEncoder(1, -1);
}
#endif //PCBV4+ROTARY_ENCODERS

#if !defined(PCBARM)
extern unsigned char __bss_end ;

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

#if defined(PCBV4)
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
  eeReadAll();

#if defined(PCBARM)
  setVolume(g_eeGeneral.speakerVolume);
  PWM->PWM_CH_NUM[0].PWM_CDTYUPD = g_eeGeneral.backlightBright;
#endif

#if defined(PCBARM) && defined(BLUETOOTH)
  btInit();
#endif

  if (g_eeGeneral.backlightMode != e_backlight_mode_off) backlightOn(); // on Tx start turn the light on

  if (UNEXPECTED_SHUTDOWN()) {
    unexpectedShutdown = 1;
  }
  else {
    doSplash();

#if defined(PCBARM) && defined(SDCARD)
    for (int i=0; i<500 && !Card_initialized; i++) {
      CoTickDelay(1);  // 2ms
    }
#endif

#if !defined(PCBARM)
    checkLowEEPROM();
#endif

    checkTHR();
    checkSwitches();
    checkAlarm();
  }

#if defined(PCBARM) || defined(PCBV4)
  if (!g_eeGeneral.unexpectedShutdown) {
    g_eeGeneral.unexpectedShutdown = 1;
    eeDirty(EE_GENERAL);
  }
#endif

  lcdSetContrast();
  backlightOn();

#if defined(PCBARM)
  start_ppm_capture();
  // TODO inside startPulses?
  s_pulses_paused = false;
  // TODO startPulses should be started after the first doMixerCalculations()
#endif

  startPulses();

  if (check_soft_power() <= e_power_trainer) {
    wdt_enable(WDTO_500MS);
  }
}

#if defined(PCBARM)
void mixerTask(void * pdata)
{
  s_pulses_paused = true;

  while(1) {

    if (!s_pulses_paused) {
      uint16_t t0 = getTmr2MHz();

      static tmr10ms_t lastTMR;
      tmr10ms_t tmr10ms = get_tmr10ms();
      uint8_t tick10ms = (tmr10ms >= lastTMR ? tmr10ms - lastTMR : 1);
      lastTMR = tmr10ms;

      if (s_current_protocol < PROTO_NONE) {
        if (tick10ms) checkTrims();
        CoEnterMutexSection(mixerMutex);
        doMixerCalculations(tmr10ms, tick10ms);
        CoLeaveMutexSection(mixerMutex);
      }

      heartbeat |= HEART_TIMER10ms;

      if (heartbeat == HEART_TIMER_PULSES+HEART_TIMER10ms) {
        wdt_reset();
        heartbeat = 0;
      }

      t0 = getTmr2MHz() - t0;
      if (t0 > g_timeMainMax) g_timeMainMax = t0 ;
    }

    CoTickDelay(1);  // 2ms for now
  }
}

#ifdef MASSSTORAGE
extern "C" {

#define TRACE_DEBUG(x, ...) debugPuts("-D- " x)
#define TRACE_DEBUG_WP(x, ...) debugPuts(x)
#define TRACE_INFO(x, ...) debugPuts("-I- " x)
#define TRACE_INFO_WP(x, ...) debugPuts(x)
#define TRACE_WARNING(x, ...) debugPuts("-W- " x)
#define TRACE_WARNING_WP(x, ...) debugPuts(x)
#define TRACE_ERROR(x, ...) debugPuts("-W- " x)

//#include "../Atmel/sam3s/sam3s-ek/libraries/libchip_sam3s/source/wdt.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/libboard_sam3s-ek/source/syscalls.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/libchip_sam3s/include/pio.h"
#include "../Atmel/sam3s/sam3s-ek/libraries/libchip_sam3s/include/pio_capture.h"
#include "../Atmel/sam3s/sam3s-ek/libraries/libchip_sam3s/source/pio_capture.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/libchip_sam3s/source/pmc.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/libchip_sam3s/source/pio_it.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/libchip_sam3s/source/tc.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/memories/include/Media.h"
#include "../Atmel/sam3s/sam3s-ek/libraries/memories/Media.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/memories/MEDSdcard.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/usb/include/MSDLun.h"
#include "../Atmel/sam3s/sam3s-ek/libraries/usb/include/MSDescriptors.h"
#include "../Atmel/sam3s/sam3s-ek/libraries/usb/include/MSD.h"
#include "../Atmel/sam3s/sam3s-ek/libraries/usb/include/USBDDriver.h"
#include "../Atmel/sam3s/sam3s-ek/libraries/usb/include/MSDDriver.h"
#include "../Atmel/sam3s/sam3s-ek/libraries/libchip_sam3s/source/pio.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/usb/device/massstorage/MSDLun.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/usb/device/massstorage/MSDDriver.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/usb/device/core/USBDDriver.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/usb/device/core/USBD.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/libchip_sam3s/source/USBD_HAL.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/usb/common/core/USBDescriptors.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/usb/device/massstorage/MSDFunction.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/usb/common/core/USBRequests.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/usb/device/massstorage/MSDDStateMachine.c"
#include "../Atmel/sam3s/sam3s-ek/libraries/usb/device/massstorage/SBCMethods.c"

#include "../Atmel/sam3s/sam3s-ek/examples_usb/usb_massstorage/device_descriptor.c"

/*----------------------------------------------------------------------------
 *        Local definitions
 *----------------------------------------------------------------------------*/

/** Maximum number of LUNs which can be defined. */
#define MAX_LUNS            1

/** Media index for different disks */
#define DRV_SDMMC           0    /** SD card */

/** Delay for pushbutton debouncing (ms) */
#define DEBOUNCE_TIME       10

/** PIT period value (seconds) */
#define PIT_PERIOD          1000

/** Delay for display view update (*250ms) */
#define UPDATE_DELAY        4

/** Delay for waiting DBGU input (*250ms) */
#define INPUT_DELAY         15

/** Size of one block in bytes. */
#define BLOCK_SIZE          512

/** Size of the MSD IO buffer in bytes (6K, more the better). */
#define MSD_BUFFER_SIZE     (12*BLOCK_SIZE)

/* Ramdisk size: 20K (WinXP can not format the disk if lower than 20K) */
/* #define RAMDISK_SIZE    (20*1024) */

/** Size of the reserved Nand Flash (4M) */
#define NF_RESERVE_SIZE     (4*1024*1024)

/** Size of the managed Nand Flash (128M) */
#define NF_MANAGED_SIZE     (128*1024*1024)

/*----------------------------------------------------------------------------
 *        Global variables
 *----------------------------------------------------------------------------*/

/** MSD Driver Descriptors List */
extern const USBDDriverDescriptors msdDriverDescriptors;

/** Available medias. */
Media medias[MAX_LUNS];

/*----------------------------------------------------------------------------
 *        Local variables
 *----------------------------------------------------------------------------*/

/** Device LUNs. */
MSDLun luns[MAX_LUNS];

/** LUN read/write buffer. */
uint8_t msdBuffer[MSD_BUFFER_SIZE];

/** Total data read/write by MSD */
unsigned int msdReadTotal = 0;
unsigned int msdWriteTotal = 0;
unsigned short msdFullCnt = 0;
unsigned short msdNullCnt = 0;

/** Update delay counter, tick is 250ms */
uint32_t updateDelay = UPDATE_DELAY;

/** Flag to update Display View */
uint8_t updateView = 0;

/** Pins used to access to nandflash. */
static const Pin pPinsNf[] = {PINS_NANDFLASH};
/** Nandflash device structure. */
// static struct TranslatedNandFlash translatedNf;
/** Address for transferring command bytes to the nandflash. */
static unsigned int cmdBytesAddr = BOARD_NF_COMMAND_ADDR;
/** Address for transferring address bytes to the nandflash. */
static unsigned int addrBytesAddr = BOARD_NF_ADDRESS_ADDR;
/** Address for transferring data bytes to the nandflash. */
static unsigned int dataBytesAddr = BOARD_NF_DATA_ADDR;
/** Nandflash chip enable pin. */
static const Pin nfCePin = BOARD_NF_CE_PIN;
/** Nandflash ready/busy pin. */
static const Pin nfRbPin = BOARD_NF_RB_PIN;


static void _ConfigureUsbClock(void)
{
    /* Enable PLLB for USB */
    PMC->CKGR_PLLBR = CKGR_PLLBR_DIVB(1)
                    | CKGR_PLLBR_MULB(7)
                    | CKGR_PLLBR_PLLBCOUNT_Msk;
    while((PMC->PMC_SR & PMC_SR_LOCKB) == 0);
    /* USB Clock uses PLLB */
    PMC->PMC_USB = PMC_USB_USBDIV(1)    /* /2   */
                 | PMC_USB_USBS;        /* PLLB */
}

static void _ConfigureTc0(void)
{
    uint32_t div;
    uint32_t tcclks;

    /* Enable peripheral clock */
    PMC->PMC_PCER0 = 1 << ID_TC0;

    /* Configure TC for a 4Hz frequency and trigger on RC compare */
    TC_FindMckDivisor(4, BOARD_MCK, &div, &tcclks, BOARD_MCK);
    TC_Configure(TC0, 0, tcclks | TC_CMR_CPCTRG);
    TC0->TC_CHANNEL[0].TC_RC = (BOARD_MCK / div) / 4;

    /* Configure and enable interrupt on RC compare */
    NVIC_EnableIRQ((IRQn_Type)ID_TC0);
    TC0->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;

    TC_Start(TC0, 0);
}


/*-----------------------------------------------------------------------------
 *         Callback re-implementation
 *-----------------------------------------------------------------------------*/

/**
 * Invoked after the USB driver has been initialized. By default, configures
 * the UDP/UDPHS interrupt.
 */
void USBDCallbacks_Initialized(void)
{
  TRACE_INFO("USBDCallbacks_Initialized\n\r");
  NVIC_EnableIRQ(UDP_IRQn);
}

/**
 * Invoked when a new SETUP request is received from the host. Forwards the
 * request to the Mass Storage device driver handler function.
 * \param request  Pointer to a USBGenericRequest instance.
 */
void USBDCallbacks_RequestReceived(const USBGenericRequest *request)
{
  TRACE_INFO("USBDCallbacks_RequestReceived\n\r");
  MSDDriver_RequestHandler(request);
}

/**
 * Invoked when the configuration of the device changes. Resets the mass
 * storage driver.
 * \param cfgnum New configuration number.
 */
void USBDDriverCallbacks_ConfigurationChanged(uint8_t cfgnum)
{
  TRACE_INFO("USBDDriverCallbacks_ConfigurationChanged\n\r");
  MSDDriver_ConfigurationChangeHandler(cfgnum);
}

void USBDDriverCallbacks_InterfaceSettingChanged(
    uint8_t interface,
    uint8_t setting)
{
    TRACE_INFO("USBDDriverCallbacks_InterfaceSettingChanged\n\r");
}

void USBDCallbacks_Suspended(void)
{
  TRACE_INFO("USBDCallbacks_Suspended");
}

void USBDCallbacks_Resumed(void)
{
  TRACE_INFO("USBDCallbacks_Resumed");
}

void USBDCallbacks_Reset(void)
{
  TRACE_INFO("USBDCallbacks_Reset");
}

void MSDCallbacks_Data(uint8_t flowDirection,
    uint32_t  dataLength,
    uint32_t  fifoNullCount,
    uint32_t  fifoFullCount)
{
    if (flowDirection) {

        msdReadTotal += dataLength;
    }
    else {

        msdWriteTotal += dataLength;
    }

    msdFullCnt += fifoFullCount;
    msdNullCnt += fifoNullCount;
}

/**
 * \brief Initialize all medias and LUNs.
 */
static void MemoryInitialization(void)
{
    uint32_t i;
    for (i = 0; i < MAX_LUNS; i ++)
      LUN_Init(&luns[i], 0, 0, 0, 0, 0, 0, 0, 0);

    unsigned char rc;
    if (1) {
      // Blocked SD access function
      rc = MEDSdcard_Initialize(&(medias[DRV_SDMMC]), 0);
      if(1) {
        LUN_Init(&(luns[DRV_SDMMC]), &(medias[DRV_SDMMC]),
            msdBuffer, MSD_BUFFER_SIZE,
            0, 0, 0, 0,
            MSDCallbacks_Data);
      }
    }
    else {
      LUN_Eject(&luns[DRV_SDMMC]);
    }
}

char usbState;

void usbMassStorage()
{
  static bool initialized = false;

  if (!initialized) {
    /* Disable watchdog */
    // WDT_Disable( WDT ) ;

    /* If they are present, configure Vbus & Wake-up pins */
    // PIO_InitializeInterrupts(0);

    /* Enable UPLL for USB */
    _ConfigureUsbClock();

    /* Start TC for timing & status update */
    _ConfigureTc0();

    /* Initialize memories and LUN */
    MemoryInitialization();

    /* BOT driver initialization */
    MSDDriver_Initialize(&msdDriverDescriptors,
                           luns, MAX_LUNS);

    /* connect if needed */
    USBD_Connect();

    /* Infinite loop */
    updateDelay = UPDATE_DELAY;
    updateView = 0;

    initialized = true;
  }

  // while (1) {

  usbState = '0' + USBD_GetState();

  /* Mass storage state machine */
  if (USBD_GetState() >= USBD_STATE_CONFIGURED) {

    MSDDriver_StateMachine();
  }
#if 0
      /* Update status view */
      if (updateView) {

          updateView = 0;

          if (msdWriteTotal < 50 * 1000) {
           //   MED_Flush(&medias[DRV_NAND]);
          }

/*          printf("Read %5dK, Write %5dK, IO %5dK; Null %4d, Full %4d\r",
              msdReadTotal/(UPDATE_DELAY*250),
              msdWriteTotal/(UPDATE_DELAY*250),
              (msdReadTotal+msdWriteTotal)/(UPDATE_DELAY*250),
              msdNullCnt, msdFullCnt);
*/
          msdReadTotal = 0;
          msdWriteTotal = 0;
          msdNullCnt = 0;
          msdFullCnt = 0;
      }
#endif
//  }
}
} // extern "C"
#endif

void menusTask(void * pdata)
{
  open9xInit();

  while (check_soft_power() != e_power_off) {
#ifdef MASSSTORAGE
    if (PIOC->PIO_PDSR & PIO_PC25) {
      usbMassStorage();
    }
#endif
    perMain();
    CoTickDelay(5);  // 10ms for now
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

  uint32_t mAhUsed = g_eeGeneral.mAhUsed + Current_used * (488 + g_eeGeneral.currentCalib) / 8192 / 36;
  if (g_eeGeneral.mAhUsed != mAhUsed)
    g_eeGeneral.mAhUsed = mAhUsed;
  if (sessionTimer > 0)
    g_eeGeneral.globalTimer += sessionTimer;
  g_eeGeneral.unexpectedShutdown = 0;

  eeDirty(EE_GENERAL);
  eeCheck(true);

  lcd_clear();
  refreshDisplay();
  lcdSetRefVolt(0);

  soft_power_off(); // Only turn power off if necessary
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
#if defined(PCBV4)
  uint8_t mcusr = MCUSR; // save the WDT (etc) flags
  MCUSR = 0; // must be zeroed before disabling the WDT
#elif defined(PCBSTD)
  uint8_t mcusr = MCUCSR;
  MCUCSR = 0;
#endif

  wdt_disable();

  board_init();

  lcd_init();

#if !defined(PCBARM)
  // Init Stack while interrupts are disabled
#define STACKPTR     _SFR_IO16(0x3D)
  {
    unsigned char *p ;
    unsigned char *q ;

    p = (unsigned char *) STACKPTR ;
    q = &__bss_end ;
    p -= 2 ;
    while ( p > q )
    {
      *p-- = 0x55 ;
    }
  }
#endif

  g_menuStack[0] = menuMainView;
  g_menuStack[1] = menuProcModelSelect;

  lcdSetRefVolt(25);

  sei(); // interrupts needed for FRSKY_Init and eeReadAll.

#if defined(FRSKY) and !defined(DSM2_SERIAL)
  FRSKY_Init();
#endif

#if defined(DSM2_SERIAL) and !defined(FRSKY)
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

#if !defined(PCBARM)
  open9xInit(mcusr);
#endif

#if defined(PCBARM)
  if (check_soft_power() == e_power_usb) {
    soft_power_off(); // Only turn power off if necessary

#if defined(HAPTIC)
    hapticOff();
#endif

    BACKLIGHT_ON;

    g_eeGeneral.contrast = 25;

    lcd_clear();

    lcd_putcAtt( 48, 24, 'U', DBLSIZE ) ;
    lcd_putcAtt( 60, 24, 'S', DBLSIZE ) ;
    lcd_putcAtt( 72, 24, 'B', DBLSIZE ) ;
    refreshDisplay() ;

    usb_mode();
  }

  CoInitOS();

#if defined(DEBUG)
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

  sdMutex = CoCreateMutex();
  audioMutex = CoCreateMutex();
  mixerMutex = CoCreateMutex();

  CoStartOS();
#else
#if defined(PCBV4)
  uint8_t shutdown_state = 0;
#endif

  while(1) {
#if defined(PCBV4)
    if ((shutdown_state=check_soft_power()) > e_power_trainer)
      break;
#endif

#if defined(PCBSTD)
    uint16_t t0 = getTmr16KHz();
#endif
  
    perMain();

#if defined(PCBSTD)
    if(heartbeat == HEART_TIMER_PULSES+HEART_TIMER10ms) {
      wdt_reset();
      heartbeat = 0;
    }

    t0 = getTmr16KHz() - t0;
    if (t0 > g_timeMainMax) g_timeMainMax = t0;
#endif
  }
#endif // PCBARM

#if defined(PCBV4)
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
  refreshDisplay() ;
  soft_power_off();            // Only turn power off if necessary
  wdt_disable();
  while(1); // never return from main() - there is no code to return back, if any delays occurs in physical power it does dead loop.
#endif
}
#endif // !SIMU

