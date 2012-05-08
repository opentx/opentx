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

#ifdef SPLASH
const pm_uchar splashdata[] PROGMEM = { 'S','P','S',0,
#include "s9xsplash.lbm"
	'S','P','E',0};
const pm_uchar * s9xsplash = splashdata+4;
#endif

#include "menus.h"

// MM/SD card Disk IO Support
#if defined (PCBV4)
gtime_t g_unixTime; // Global date/time register, incremented each second in per10ms()
#endif

EEGeneral  g_eeGeneral;
ModelData  g_model;

#if !defined(PCBARM)
uint8_t g_tmr1Latency_max;
uint8_t g_tmr1Latency_min;
#endif
uint16_t g_timeMain;
#ifdef DEBUG
uint16_t g_time_per10;
#endif

#ifdef AUDIO
audioQueue  audio;
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

void generalDefault()
{
  memset(&g_eeGeneral, 0, sizeof(g_eeGeneral));
  g_eeGeneral.lightSw  = SWITCH_ON;
  g_eeGeneral.myVers   = EEPROM_VER;
  g_eeGeneral.fwOptions = FW_OPTIONS;
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
#define D9 (RESX * 2 / 8)
#define D5 (RESX * 2 / 4)
  bool    cv9 = idx >= MAX_CURVE5;
  int8_t *crv = cv9 ? g_model.curves9[idx-MAX_CURVE5] : g_model.curves5[idx];
  int16_t erg;

  x+=RESXu;
  if(x < 0) {
    erg = (int16_t)crv[0] * (RESX/4);
  } else if(x >= (RESX*2)) {
    erg = (int16_t)crv[(cv9 ? 8 : 4)] * (RESX/4);
  } else {
    int16_t a,dx;
    if(cv9){
      a   = (uint16_t)x / D9;
      dx  =((uint16_t)x % D9) * 2;
    } else {
      a   = (uint16_t)x / D5;
      dx  = (uint16_t)x % D5;
    }
    erg  = (int16_t)crv[a]*((D5-dx)/2) + (int16_t)crv[a+1]*(dx/2);
  }
  return erg / 25; // 100*D5/RESX;
}

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
  if(k == 0) return x;
  int16_t   y;
  bool    neg =  x < 0;
  if(neg)   x = -x;
  if(k<0){
    y = RESXu-expou(RESXu-x,-k);
  }else{
    y = expou(x,k);
  }
  return neg? -y:y;
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


void applyExpos(int16_t *anas, uint8_t phase)
{
  static int16_t anas2[NUM_STICKS]; // values before expo, to ensure same expo base when multiple expo lines are used
  memcpy(anas2, anas, sizeof(anas2));

  if (phase == 255)
    phase = getFlightPhase();
  phase++;

  int8_t cur_chn = -1;

  for (uint8_t i=0; i<MAX_EXPOS; i++) {
    ExpoData &ed = g_model.expoData[i];
#if defined(PCBARM)
    int8_t ed_phase = ed.phase;
#else
    uint8_t ed_phase = ed.phase;
#endif
    if (ed.mode==0) break; // end of list
    if (ed.chn == cur_chn)
      continue;
    if (ed_phase != 0) {
#if defined(PCBARM)
      if (ed_phase < 0) {
        if (phase == -ed_phase)
          continue;
      }
#else
      if (ed.negPhase) {
        if (phase == ed_phase)
          continue;
      }
#endif
      else {
        if (phase != ed_phase)
          continue;
      }
    }
    if (getSwitch(ed.swtch, 1)) {
      int16_t v = anas2[ed.chn];
      if((v<0 && ed.mode&1) || (v>=0 && ed.mode&2)) {
        cur_chn = ed.chn;
        int16_t k = ed.expo;
        v = expo(v, k);
        uint8_t ed_curve = ed.curve;
        if (ed_curve) v = applyCurve(v, ed_curve > 10 ? ed_curve + 4 : ed_curve);
        v = ((int32_t)v * ed.weight) / 100;
        anas[cur_chn] = v;
      }
    }
  }
}

int16_t applyLimits(uint8_t channel, int32_t value)
{
  int16_t ofs = g_model.limitData[channel].offset;
  int16_t lim_p = 10 * (g_model.limitData[channel].max + 100);
  int16_t lim_n = 10 * (g_model.limitData[channel].min - 100); //multiply by 10 to get same range as ofs (-1000..1000)
  if (ofs > lim_p) ofs = lim_p;
  if (ofs < lim_n) ofs = lim_n;

  if (value) value =
      (value > 0) ? value * ((int32_t) lim_p - ofs) / 100000 :
          -value * ((int32_t) lim_n - ofs) / 100000; //div by 100000 -> output = -1024..1024

  value += calc1000toRESX(ofs);
  lim_p = calc1000toRESX(lim_p);
  lim_n = calc1000toRESX(lim_n);
  if (value > lim_p) value = lim_p;
  if (value < lim_n) value = lim_n;

  ofs = value; // we convert value to a 16bit value and reuse ofs
  if (g_model.limitData[channel].revert) ofs = -ofs; // finally do the reverse.

  if (safetyCh[channel] != -128) // if safety channel available for channel check
    ofs = calc100toRESX(safetyCh[channel]);

  return ofs;
}

int16_t ex_chans[NUM_CHNOUT] = {0}; // Outputs (before LIMITS) of the last perMain
#ifdef HELI
int16_t cyc_anas[3] = {0};
#endif

int16_t getValue(uint8_t i)
{
  /*srcRaw is shifted +1!*/

  if(i<NUM_STICKS+NUM_POTS) return calibratedStick[i];
#if defined(PCBV4)
  else if(i<NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS) return getRotaryEncoder(i-(NUM_STICKS+NUM_POTS));
#endif
  else if(i<MIXSRC_MAX) return 1024;
  else if(i<MIXSRC_3POS) return (keyState(SW_ID0) ? -1024 : (keyState(SW_ID1) ? 0 : 1024));
  else if(i<MIXSRC_3POS+3)
#ifdef HELI
    return cyc_anas[i-MIXSRC_3POS];
#else
    return 0;
#endif
  else if(i<CSW_PPM_BASE+NUM_CAL_PPM) return (g_ppmIns[i-CSW_PPM_BASE] - g_eeGeneral.trainer.calib[i-CSW_PPM_BASE])*2;
  else if(i<CSW_PPM_BASE+NUM_PPM) return g_ppmIns[i-CSW_PPM_BASE]*2;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT) return ex_chans[i-CSW_CHOUT_BASE];
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_TM2) return s_timerVal[i-CSW_CHOUT_BASE-NUM_CHNOUT];
#if defined(FRSKY)
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_A2) return frskyTelemetry[i-CSW_CHOUT_BASE-NUM_CHNOUT-2].value;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_RSSI_TX) return frskyRSSI[1].value;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_RSSI_RX) return frskyRSSI[0].value;
#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_ALT) return frskyHubData.baroAltitude_bp;
#endif
#if defined(FRSKY_HUB)
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_RPM) return frskyHubData.rpm;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_FUEL) return frskyHubData.fuelLevel;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_T1) return frskyHubData.temperature1;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_T2) return frskyHubData.temperature2;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_SPEED) return frskyHubData.gpsSpeed_bp;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_DIST) return frskyHubData.gpsDistance;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_GPSALT) return frskyHubData.gpsAltitude_bp;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_CELL) return (int16_t)frskyHubData.minCellVolts * 2;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_ACCx) return frskyHubData.accelX;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_ACCy) return frskyHubData.accelY;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_ACCz) return frskyHubData.accelZ;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_HDG) return frskyHubData.gpsCourse_bp;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_VSPD) return frskyHubData.varioSpeed;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_MIN_A1) return frskyTelemetry[0].min;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_MIN_A2) return frskyTelemetry[1].min;
  else if(i<CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_MAX_DIST) return *(((int16_t*)(&frskyHubData.minAltitude))+i-(CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_MIN_ALT-1));
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
    volatile CustomSwData &cs = g_model.customSw[cs_idx];
    if (cs.func == CS_OFF) return false;

    uint8_t s = CS_STATE(cs.func);
    if (s == CS_VBOOL) {
      GETSWITCH_RECURSIVE_TYPE mask = ((GETSWITCH_RECURSIVE_TYPE)1 << cs_idx);
      if (s_last_switch_used & mask) {
        result = (s_last_switch_value & mask);
      }
      else {
        s_last_switch_used |= mask;
        bool res1 = __getSwitch(cs.v1);
        bool res2 = __getSwitch(cs.v2);
        switch (cs.func) {
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
      if (result)
        s_last_switch_value |= ((GETSWITCH_RECURSIVE_TYPE)1<<cs_idx);
      else
        s_last_switch_value &= ~((GETSWITCH_RECURSIVE_TYPE)1<<cs_idx);
    }
    else {
      int16_t x = getValue(cs.v1-1);
      int16_t y;
      if (s == CS_VOFS) {
#if defined(FRSKY)
        // Telemetry
        if (cs.v1 > CSW_CHOUT_BASE+NUM_CHNOUT) {
          y = convertTelemValue(cs.v1-(CSW_CHOUT_BASE+NUM_CHNOUT), 128+cs.v2);
          if (cs.v1 >= CSW_CHOUT_BASE+NUM_CHNOUT+TELEM_ALT) {
            // Fill the threshold array
            barsThresholds[cs.v1-CSW_CHOUT_BASE-NUM_CHNOUT-TELEM_ALT] = 128 + cs.v2;
          }
        }
        else
#endif
        {
          y = calc100toRESX(cs.v2);
        }

        switch (cs.func) {
          case CS_VPOS:
            result = (x>y);
            break;
          case CS_VNEG:
            result = (x<y);
            break;
          case CS_APOS:
            result = (abs(x)>y);
            break;
          // case CS_ANEG:
          default:
            result = (abs(x)<y);
            break;
        }
      }
      else {
        y = getValue(cs.v2-1);

        switch (cs.func) {
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
    }
  }

  return swtch > 0 ? result : !result;
}

bool getSwitch(int8_t swtch, bool nc)
{
#if defined(PCBARM)
  s_last_switch_used = 0;
  s_default_switch_value = nc;
#else
  s_last_switch_used = ((GETSWITCH_RECURSIVE_TYPE)nc<<15);
#endif
  return __getSwitch(swtch);
}

#if defined(AUTOSWITCH)
int8_t getMovedSwitch()
{
  static uint8_t switches_states = 0;
  static uint16_t s_last_time = 0;

  int8_t result = 0;

  for (uint8_t i=MAX_PSWITCH; i>0; i--) {
    bool prev;
    uint8_t mask = 0;
    if (i <= 3) {
      mask = (1<<(i-1));
      prev = (switches_states & mask);
    }
    else if (i <= 6) {
      prev = ((switches_states & 0x18) == ((i-3) << 3));
    }
    else {
      mask = (1<<(i-2));
      prev = (switches_states & mask);
    }
    bool next = __getSwitch(i);
    if (prev != next) {
      if (i!=MAX_PSWITCH || next==true)
        result = i;
      if (mask)
        switches_states ^= mask;
      else
        switches_states = (switches_states & 0xE7) | ((i-3) << 3);
    }
  }

  if (get_tmr10ms() - s_last_time > 10)
    result = 0;

  s_last_time = get_tmr10ms();

  return result;
}
#endif

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
  int16_t result;
  if (s_trimPtr[idx]) {
    result = *s_trimPtr[idx];
  }
  else {
    PhaseData *p = phaseaddress(phase);
#if defined(PCBSTD)
    result = (((int16_t)p->trim[idx]) << 2) + ((p->trim_ext >> (2*idx)) & 0x03);
#else
    result = p->trim[idx];
#endif
  }
  return result;
}

int16_t getTrimValue(uint8_t phase, uint8_t idx)
{
  return getRawTrimValue(getTrimFlightPhase(phase, idx), idx);
}

void setTrimValue(uint8_t phase, uint8_t idx, int16_t trim)
{
  if (s_trimPtr[idx]) {
    *s_trimPtr[idx] = limit((int16_t)-125, trim, (int16_t)+125);
  }
  else {
    PhaseData *p = phaseaddress(phase);
#if defined(PCBSTD)
    p->trim[idx] = (int8_t)(trim >> 2);
    p->trim_ext = (p->trim_ext & ~(0x03 << (2*idx))) + (((trim & 0x03) << (2*idx)));
#else
    p->trim[idx] = trim;
#endif
  }
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

#if defined(PCBV4)
uint8_t s_perOut_flight_phase;

uint8_t getRotaryEncoderFlightPhase(uint8_t idx)
{
  uint8_t phase = s_perOut_flight_phase;
  for (uint8_t i=0; i<MAX_PHASES; i++) {
    if (phase == 0) return 0;
    int16_t value = phaseaddress(phase)->rotaryEncoders[idx];
    if (value <= ROTARY_ENCODER_MAX) return phase;
    uint8_t result = value-ROTARY_ENCODER_MAX-1;
    if (result >= phase) result++;
    phase = result;
  }
  return 0;
}

int16_t getRotaryEncoder(uint8_t idx)
{
  return phaseaddress(getRotaryEncoderFlightPhase(idx))->rotaryEncoders[idx];
}

void incRotaryEncoder(uint8_t idx, int8_t inc)
{
  g_rotenc[idx] += inc;
  int16_t *value = &(phaseaddress(getRotaryEncoderFlightPhase(idx))->rotaryEncoders[idx]);
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
    val = (val * 46) / 25;
  }
#endif
  lcd_outdezAtt(x, (att & DBLSIZE ? y - FH : y), val, att & (~NO_UNIT)); // TODO we could add this test inside lcd_outdezAtt!
  if (~att & NO_UNIT && unit != UNIT_RAW)
    lcd_putsiAtt(lcd_lastPos/*+1*/, y, STR_VTELEMUNIT, unit, 0);
}
#endif

void clearKeyEvents()
{
#ifdef SIMU
    while (keyDown() && main_thread_running) sleep(1/*ms*/);
#else
    while (keyDown());  // loop until all keys are up
#endif
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
  if (getSwitch(g_eeGeneral.lightSw, 0) || g_eeGeneral.lightAutoOff)
    BACKLIGHT_ON;
  else
    BACKLIGHT_OFF;
}

#ifdef SPLASH
void doSplash()
{
    if(!g_eeGeneral.disableSplashScreen)
    {
      checkBacklight() ;
      lcd_clear();
      lcd_img(0, 0, s9xsplash,0,0);
      refreshDisplay();
      lcdSetRefVolt(g_eeGeneral.contrast);
      clearKeyEvents();

#ifndef SIMU
      for(uint8_t i=0; i<32; i++)
        getADC_filt(); // init ADC array
#endif

      uint16_t inacSum = stickMoveValue();

      uint16_t tgtime = get_tmr10ms() + SPLASH_TIMEOUT;  //2sec splash screen
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

        checkBacklight();
      }
    }
}
#endif

#if !defined(PCBARM)
void checkLowEEPROM()
{
  if(g_eeGeneral.disableMemoryWarning) return;
  if(EeFsGetFree() < 200)
  {
    alert(STR_EEPROMLOWMEM);
  }
}
#endif

void checkTHR()
{
  if(g_eeGeneral.disableThrottleWarning) return;

  int thrchn=(2-(stickMode&1));//stickMode=0123 -> thr=2121

  int16_t lowLim = THRCHK_DEADBAND + g_eeGeneral.calibMid[thrchn] - g_eeGeneral.calibSpanNeg[thrchn];

#ifndef SIMU
  getADC_single();   // if thr is down - do not display warning at all
#endif
  int16_t v = anaIn(thrchn);
  if (g_eeGeneral.throttleReversed) v = - v;

  if(v<=lowLim) return;

  // first - display warning
  message(STR_ALERT, STR_THROTTLENOTIDLE, STR_RESETTHROTTLE, STR_PRESSANYKEYTOSKIP);

  //loop until all switches are reset
  while (1)
  {
#ifdef SIMU
      if (!main_thread_running) return;
      sleep(1/*ms*/);
#else
      getADC_single();
#endif
      int16_t v = anaIn(thrchn);
      if (g_eeGeneral.throttleReversed) v = - v;

      if (check_soft_power() > e_power_trainer) return; // Usb on or power off

      if(v<=lowLim || keyDown()) {
        clearKeyEvents();
        return;
      }

      checkBacklight();
  }
}

void checkAlarm() // added by Gohst
{
  if (g_eeGeneral.disableAlarmWarning) return;
  if (g_eeGeneral.beeperMode == e_mode_quiet) alert(STR_ALARMSDISABLED);
}

void checkSwitches()
{
  if(!g_eeGeneral.switchWarning) return; // if warning is on

  bool state = (g_eeGeneral.switchWarning > 0);
  bool first = true;

  //loop until all switches are reset
  while (1)
  {
    uint8_t i;
    for(i=SW_BASE; i<SW_Trainer; i++)
    {
        if(i==SW_ID0) continue;
        if(getSwitch(i-SW_BASE+1,0) != state) break;
    }
    if(i==SW_Trainer || keyDown()) return;

    // first - display warning
    if (first) {
      message(STR_ALERT, STR_SWITCHESNOTOFF, STR_PLEASERESETTHEM, STR_PRESSANYKEYTOSKIP);
      first = false;
    }

    if (check_soft_power() > e_power_trainer) return; // Usb on or power off

    checkBacklight();

#ifdef SIMU
    if (!main_thread_running) return;
    sleep(1/*ms*/);
#endif
  }
}

void alert(const pm_char * s)
{
  message(STR_ALERT, s, 0, STR_PRESSANYKEY);

  while(1)
  {
#ifdef SIMU
    if (!main_thread_running) return;
    sleep(1/*ms*/);
#endif

    if (check_soft_power() > e_power_trainer) return; // Usb on or power off

    if (keyDown()) return;  // wait for key release

    checkBacklight();

    wdt_reset();
  }
}

void message(const pm_char *title, const pm_char *s, const pm_char *t, const char *last)
{
  lcd_clear();
  lcd_putsAtt(0, 0, title, DBLSIZE);
  lcd_putsLeft(4*FH, s);
  if (t)
    lcd_putsLeft(5*FH, t);
  if (last) {
    lcd_putsLeft(7*FH, last);
    AUDIO_ERROR();
  }
  refreshDisplay();
  lcdSetRefVolt(g_eeGeneral.contrast);
  clearKeyEvents();
}

int8_t *s_trimPtr[NUM_STICKS] = { NULL, NULL, NULL, NULL };

uint8_t checkTrim(uint8_t event)
{
  int8_t  k = (event & EVT_KEY_MASK) - TRM_BASE;
  int8_t  s = g_model.trimInc;

  if (k>=0 && k<8) { // && (event & _MSK_KEY_REPT))
    //LH_DWN LH_UP LV_DWN LV_UP RV_DWN RV_UP RH_DWN RH_UP
    uint8_t idx = CONVERT_MODE(1+k/2) - 1;
    uint8_t phase = getTrimFlightPhase(getFlightPhase(), idx);
    int16_t before = getRawTrimValue(phase, idx);
    int8_t  v = (s==0) ? min(32, abs(before)/4+1) : 1 << (s-1); // 1=>1  2=>2  3=>4  4=>8
    bool thro = (idx==THR_STICK && g_model.thrTrim);
    if (thro) v = 4; // if throttle trim and trim trottle then step=4
    int16_t after = (k&1) ? before + v : before - v;   // positive = k&1

    bool beepTrim = false;
    for (int16_t mark=TRIM_MIN; mark<=TRIM_MAX; mark+=TRIM_MAX) {
      if ((mark!=0 || !thro) && ((mark!=TRIM_MIN && after>=mark && before<mark) || (mark!=TRIM_MAX && after<=mark && before>mark))) {
        after = mark;
        beepTrim = true;
      }
    }

    if ((before<after && after>TRIM_MAX) || (before>after && after<TRIM_MIN)) {
      if (!g_model.extendedTrims) after = before;
      beepTrim = true; // no repetition, it could be dangerous
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
      killEvents(event);
      AUDIO_TRIM_MIDDLE(after);
    }
    else {
#if defined (AUDIO)
      audio.event(AU_TRIM_MOVE, after);
#else
      if (event & _MSK_KEY_REPT) warble = true;
      AUDIO_TRIM();
#endif
    }
    return 0;
  }
  return event;
}

#ifdef SIMU

uint16_t BandGap = 225;

#else

// #define STARTADCONV (ADCSRA  = (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2) | (1<<ADSC) | (1 << ADIE))
// G: Note that the above would have set the ADC prescaler to 128, equating to
// 125KHz sample rate. We now sample at 500KHz, with oversampling and other
// filtering options to produce 11-bit results.
#if defined(PCBV4)
uint16_t BandGap = 2040 ;
#elif defined(PCBSTD)
uint16_t BandGap ;
#endif
#if defined(PCBARM) and defined(REVB)
uint16_t Current_analogue;
#define NUMBER_ANALOG   9
#else
#define NUMBER_ANALOG   8
#endif
static uint16_t s_anaFilt[NUMBER_ANALOG];
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
#endif

#if defined(PCBARM)
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
#else
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
#endif

#if defined(PCBARM)
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

#if not defined(PCBARM)
void getADC_bandgap()
{
#if defined (PCBV4)
  // For times over-sample with no divide, x2 to end at a half averaged, x8. DON'T ASK mmmkay? :P This is how I want it.
  ADCSRA|=0x40; while ((ADCSRA & 0x10)==0); ADCSRA|=0x10;
  BandGap=ADCW;
  ADCSRA|=0x40; while ((ADCSRA & 0x10)==0); ADCSRA|=0x10;
  BandGap+=ADCW;
  ADCSRA|=0x40; while ((ADCSRA & 0x10)==0); ADCSRA|=0x10;
  BandGap+=ADCW;
  ADCSRA|=0x40; while ((ADCSRA & 0x10)==0); ADCSRA|=0x10;
  BandGap+=ADCW;
  BandGap *= 2;
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

uint16_t g_vbat100mV = 0;

volatile uint8_t tick10ms = 0;
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

uint8_t s_traceBuf[MAXTRACE];
uint8_t s_traceWr;
int8_t s_traceCnt;

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
int16_t  anas [NUM_STICKS] = {0};
int16_t  trims[NUM_STICKS] = {0};
int32_t  chans[NUM_CHNOUT] = {0};
uint32_t inacCounter = 0;
uint16_t inacSum = 0;
BeepANACenter bpanaCenter = 0;

int16_t  sDelay[MAX_MIXERS] = {0};
int32_t  act   [MAX_MIXERS] = {0};
uint8_t  swOn  [MAX_MIXERS] = {0};
uint8_t mixWarning;

FORCEINLINE void evalTrims(uint8_t phase)
{
  for (uint8_t i=0; i<NUM_STICKS; i++) {
    // do trim -> throttle trim if applicable
    // TODO avoid int32_t vv
    int32_t vv = 2*RESX;
    int16_t trim = getTrimValue(phase, i);
    if (i==THR_STICK && g_model.thrTrim) {
      if (g_eeGeneral.throttleReversed)
        trim = -trim;
      int16_t v = anas[i];
      vv = ((int32_t)trim-TRIM_MIN)*(RESX-v)/(2*RESX);
    }
    else if (trimsCheckTimer > 0) {
      trim = 0;
    }

    trims[i] = (vv==2*RESX) ? trim*2 : (int16_t)vv*2; // if throttle trim -> trim low end
  }
}

enum PerOutMode {
  e_perout_mode_normal = 0,
  e_perout_mode_trims,
  e_perout_mode_zeros,
  e_instant_trim
};

uint8_t s_perout_mode = e_perout_mode_normal;

BeepANACenter evalSticks(uint8_t phase)
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

#if defined(PCBV4)
    int16_t v = ((i < NUM_STICKS+NUM_POTS) ? anaIn(i) : getRotaryEncoder(i-(NUM_STICKS+NUM_POTS)));
#else
    int16_t v = anaIn(i);
#endif

#ifndef SIMU
    if(i < NUM_STICKS+NUM_POTS){
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

    if (i < NUM_STICKS+NUM_POTS)
      calibratedStick[ch] = v; //for show in expo

    uint8_t tmp = (uint16_t)abs(v) / 16;
    if (tmp <= 1) anaCenter |= (tmp==0 ? (BeepANACenter)1<<ch : bpanaCenter & ((BeepANACenter)1<<ch));

    if (ch < NUM_STICKS) { //only do this for sticks
      if (s_perout_mode==e_perout_mode_normal && (isFunctionActive(FUNC_TRAINER) || isFunctionActive(FUNC_TRAINER_RUD+ch))) {
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

      anas[ch] = v; //set values for mixer
    }
  }

  /* EXPOs */
  applyExpos(anas, phase);

  /* TRIMs */
  evalTrims(phase);

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


#if defined(PCBARM)
#define MASK_FSW_TYPE uint32_t // current max = 32 function switches
#else
#define MASK_FSW_TYPE uint16_t // current max = 16 function switches
#endif

uint16_t active_functions = 0;
MASK_FSW_TYPE active_switches = 0;

void evalFunctions()
{
  assert((int)(sizeof(active_functions)*8) > (int)(FUNC_MAX-NUM_CHNOUT));

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
        if (sd->func < FUNC_TRAINER  && (g_menuStack[g_menuStackPtr] != menuProcFunctionSwitches || m_posVert != (i+1) || m_posHorz > 1)) {
          safetyCh[sd->func] = (int8_t)sd->param;
        }

        if (~active_functions & function_mask) {
          if (sd->func == FUNC_INSTANT_TRIM) {
            if (g_menuStack[0] == menuMainView
#if defined(FRSKY)
                || g_menuStack[0] == menuProcFrsky
#endif
                )
            instantTrim();
          }
        }

	if (~active_switches & switch_mask) {
          if (sd->func == FUNC_RESET) {
            switch (sd->param) {
              case 0:
              case 1:
                resetTimer(sd->param);
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

        if ((!momentary) || (~active_switches & switch_mask)) {
          if (sd->func == FUNC_PLAY_SOUND) {
#if defined(AUDIO)
            audioDefevent(AU_FRSKY_FIRST+sd->param);
#else
            beep(3);
#endif
          }

#if defined(HAPTIC)
          if (sd->func == FUNC_HAPTIC) {
            haptic.event(AU_FRSKY_LAST+sd->param);
          }
#endif

#if defined(SOMO)
          if (sd->func == FUNC_PLAY_SOMO) {
            somoPushPrompt(sd->param);
          }
#endif

#if defined(DEBUG)
          if (sd->func == FUNC_TEST) {
            testFunc();
          }
#endif
        }

        active_functions |= function_mask;
        active_switches |= switch_mask;
      }
      else {
        active_functions &= (~function_mask);
        active_switches &= (~switch_mask);
      }
    }
  }
}

void perOut(uint8_t phase)
{
#if defined(PCBV4)
  s_perOut_flight_phase = phase;
#endif

  BeepANACenter anaCenter = evalSticks(phase);

  if (s_perout_mode == e_perout_mode_normal) {
    //===========BEEP CENTER================
    anaCenter &= g_model.beepANACenter;
    if(((bpanaCenter ^ anaCenter) & anaCenter)) AUDIO_POT_STICK_MIDDLE();
    bpanaCenter = anaCenter;

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
        case (SWASH_TYPE_120):
          vp = REZ_SWASH_Y(vp);
          vr = REZ_SWASH_X(vr);
          cyc_anas[0] = vc - vp;
          cyc_anas[1] = vc + vp/2 + vr;
          cyc_anas[2] = vc + vp/2 - vr;
          break;
        case (SWASH_TYPE_120X):
          vp = REZ_SWASH_X(vp);
          vr = REZ_SWASH_Y(vr);
          cyc_anas[0] = vc - vr;
          cyc_anas[1] = vc + vr/2 + vp;
          cyc_anas[2] = vc + vr/2 - vp;
          break;
        case (SWASH_TYPE_140):
          vp = REZ_SWASH_Y(vp);
          vr = REZ_SWASH_Y(vr);
          cyc_anas[0] = vc - vp;
          cyc_anas[1] = vc + vp + vr;
          cyc_anas[2] = vc + vp - vr;
          break;
        case (SWASH_TYPE_90):
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

    s_trimPtr[0] = NULL;
    s_trimPtr[1] = NULL;
    s_trimPtr[2] = NULL;
    s_trimPtr[3] = NULL;
  }

  memset(chans, 0, sizeof(chans));        // All outputs to 0

  //========== MIXER LOOP ===============
  uint8_t lv_mixWarning = 0;
  for (uint8_t i=0; i<MAX_MIXERS; i++) {

    MixData *md = mixaddress( i ) ;

    if (md->srcRaw==0) break;

    if (md->phase != 0) {
      if (md->phase > 0) {
        if (phase+1 != md->phase)
          continue;
      }
      else {
        if (phase+1 == -md->phase)
          continue;
      }
    }

    //========== SWITCH ===============
    bool sw = getSwitch(md->swtch, 1);

    //========== VALUE ===============
    //Notice 0 = NC switch means not used -> always on line
    uint8_t k = md->srcRaw-1;
    int16_t v = 0;
    if (s_perout_mode != e_perout_mode_normal) {
      if (!sw || k >= NUM_STICKS || (k == THR_STICK && g_model.thrTrim))
        continue;
    }
    else {
      if (k < NUM_STICKS)
        v = anas[k]; //Switch is on. MAX=FULL=512 or value.
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
            sDelay[i] = md->delayUp * 100;
          }
          if (sDelay[i]) { // perform delay
            if(tick10ms) sDelay[i]--;
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
          sDelay[i] = md->delayDown * 100;
        }
        if (sDelay[i]) { // perform delay
          if(tick10ms) sDelay[i]--;
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

    //========== OFFSET ===============
    if (apply_offset && md->sOffset) v += calc100toRESX(md->sOffset);

    //========== SPEED ===============
    if (s_perout_mode==e_perout_mode_normal && (md->speedUp || md->speedDown))  // there are delay values
    {
#define DEL_MULT 256

      int16_t diff = v-act[i]/DEL_MULT;

      if (diff) {
        //rate = steps/sec => 32*1024/100*md->speedUp/Down
        //act[i] += diff>0 ? (32768)/((int16_t)100*md->speedUp) : -(32768)/((int16_t)100*md->speedDown);
        //-100..100 => 32768 ->  100*83886/256 = 32768,   For MAX we divide by 2 since it's asymmetrical
        if (tick10ms) {
            int32_t rate = (int32_t)DEL_MULT*2048*100;
            if(md->weight) rate /= abs(md->weight);
            // TODO port optim er9x by Mike
            act[i] = (diff>0) ? ((md->speedUp>0)   ? act[i]+(rate)/((int16_t)100*md->speedUp)   :  (int32_t)v*DEL_MULT) :
                                ((md->speedDown>0) ? act[i]-(rate)/((int16_t)100*md->speedDown) :  (int32_t)v*DEL_MULT) ;
        }

        if(((diff>0) && (v<(act[i]/DEL_MULT))) || ((diff<0) && (v>(act[i]/DEL_MULT)))) act[i]=(int32_t)v*DEL_MULT; //deal with overflow
        v = act[i]/DEL_MULT;
      }
    }

    //========== CURVES ===============
    if (md->curve)
      v = applyCurve(v, md->curve);

    //========== TRIMS ===============
    if (k < NUM_STICKS) {
      if (s_perout_mode < e_perout_mode_zeros && md->carryTrim == TRIM_ON) {
        v += trims[k];
      }
      if (s_perout_mode == e_perout_mode_normal && md->carryTrim == TRIM_OFFSET) {
        v = md->sOffset;
        v = calc1000toRESX(v << 3);
        s_trimPtr[k] = &md->sOffset;  // use the value stored here for the trim
      }
    }

    //========== MULTIPLEX ===============
    int32_t dv = (int32_t)v*md->weight;

    int8_t differential = md->differential;
    if (differential>0 && dv<0)
      dv = (dv * (50-differential)) / 50;
    else if (differential<0 && dv>0)
      dv = (dv * (50+differential)) / 50;

    int32_t *ptr = &chans[md->destCh]; // Save calculating address several times
    switch(md->mltpx){
      case MLTPX_REP:
        *ptr = dv;
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

#if (defined(PCBARM) && !defined(REVA) && !defined(SIMU)) || (defined(PCBV4) && !defined(REV0) && !defined(SIMU))
#define TIME_TO_WRITE (s_eeDirtyMsk && (get_tmr10ms() - s_eeDirtyTime10ms) >= WRITE_DELAY_10MS)
#else
#define TIME_TO_WRITE s_eeDirtyMsk
#endif

int32_t sum_chans512[NUM_CHNOUT] = {0};
void perMain()
{
  static uint16_t lastTMR;
  uint16_t tmr10ms = get_tmr10ms();
  tick10ms = (tmr10ms != lastTMR);
  lastTMR = tmr10ms;

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
        delta = (MAX_ACT / 100) / fadeTime;
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
    memset(sum_chans512, 0, sizeof(sum_chans512));
    weight = 0;
    for (uint8_t p=0; p<MAX_PHASES; p++) {
      if (s_fade_flight_phases & (1<<p)) {
        perOut(p);
        for (uint8_t i=0; i<NUM_CHNOUT; i++)
          sum_chans512[i] += (chans[i] / 16) * fp_act[p];
        weight += fp_act[p];
      }
    }
    // printf("sum=%d, weight=%d ", sum_chans512[2], weight); fflush(stdout);
    assert(weight);
  }
  else {
    perOut(phase);
  }

  //========== LIMITS ===============
  for (uint8_t i=0;i<NUM_CHNOUT;i++) {
    // chans[i] holds data from mixer.   chans[i] = v*weight => 1024*100
    // later we multiply by the limit (up to 100) and then we need to normalize
    // at the end chans[i] = chans[i]/100 =>  -1024..1024
    // interpolate value with min/max so we get smooth motion from center to stop
    // this limits based on v original values and min=-1024, max=1024  RESX=1024
    //printf("chans%d=%d\n", i, chans[i]);fflush(stdout);
    int32_t q = (s_fade_flight_phases ? (sum_chans512[i] / weight) * 16 : chans[i]);
    ex_chans[i] = q / 100; // for the next perMain

    int16_t value = applyLimits(i, q);

    cli();
    g_chans512[i] = value;  // copy consistent word to int-level
    sei();
  }

// TODO same code here + integrate the timer which could be common
#if defined(PCBARM)
  if (Tenms) {
    Tenms = 0 ;
    if (Eeprom32_process_state != E32_IDLE)
      ee32_process();
    else if (TIME_TO_WRITE)
      eeCheck();
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

  int16_t val;

  if (g_model.thrTraceSrc == 0) {
    val = calibratedStick[THR_STICK]; // get throttle channel value
  }
  else if (g_model.thrTraceSrc > NUM_POTS) {
    val = g_chans512[g_model.thrTraceSrc-NUM_POTS-1];
  }
  else {
    val = calibratedStick[g_model.thrTraceSrc+NUM_STICKS-1];
  }

  val += RESX;
  val /= (RESX/16); // calibrate it

  // Throttle trace start
  static uint16_t s_time_tot;
  static uint16_t s_time_trace;
  static uint8_t s_cnt_1s;
  static uint16_t s_sum_1s;
  static uint16_t s_cnt_10s;
  static uint16_t s_sum_10s;

  s_cnt_1s++;
  s_sum_1s += val;

  if ((uint16_t)(tmr10ms - s_time_tot) >= 100) { // 1sec
    s_time_tot += 100;
    s_timeCumTot += 1;

    val = s_sum_1s / s_cnt_1s;
    s_timeCum16ThrP += val / 2;
    if (val) s_timeCumThr += 1;

    s_cnt_10s += s_cnt_1s;
    s_sum_10s += s_sum_1s;
    s_cnt_1s = 0;
    s_sum_1s = 0;

    if ((uint16_t)(tmr10ms - s_time_trace) >= 1000) {// 10sec
      s_time_trace += 1000;
      val = s_sum_10s / s_cnt_10s;
      s_sum_10s = 0;
      s_cnt_10s = 0;

      s_traceBuf[s_traceWr++] = val;
      if (s_traceWr >= MAXTRACE) s_traceWr = 0;
      if (s_traceCnt >= 0) s_traceCnt++; // TODO to be checked
    }
  }
  // Throttle trace end

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
        if(!(sw_toggled[i] | s_sum[i] | s_cnt[i] | lastSwPos[i])) lastSwPos[i] = tm < 0;  // if initializing then init the lastSwPos
        uint8_t swPos = getSwitch(tm>0 ? tm-(TMR_VAROFS+MAX_SWITCH-1) : tm+(TMR_VAROFS+MAX_SWITCH-1), 0);
        if (swPos && !lastSwPos[i]) sw_toggled[i] = !sw_toggled[i];  // if switch is flipped first time -> change counter state
        lastSwPos[i] = swPos;
      }

      if ( (s_timerVal_10ms[i] += 1 ) >= 100 ) {
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
        if(s_timerVal[0]<= 3) AUDIO_TIMER_LT3();
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
          if (MAX_ACT - fp_act[p] > delta)
            fp_act[p] += delta;
          else {
            fp_act[p] = MAX_ACT;
            s_fade_flight_phases -= (1<<p);
          }
        }
        else {
          if (fp_act[p] > delta)
            fp_act[p] -= delta;
          else {
            fp_act[p] = 0;
            s_fade_flight_phases -= (1<<p);
          }
        }
      }
    }
  }
  
  evalFunctions();

  if (s_noHi) s_noHi--;
  if (trimsCheckTimer) trimsCheckTimer --;

  if (g_eeGeneral.inactivityTimer && g_vbat100mV>50) {
    inacCounter++;
    uint16_t tsum = 0;
    for(uint8_t i=0;i<4;i++) tsum += anaIn(i)/64;  // reduce sensitivity
    if(tsum!=inacSum){
      inacSum = tsum;
      inacCounter=0;
    }
    if(inacCounter>((uint32_t)g_eeGeneral.inactivityTimer*100*60))
      if((inacCounter&0x3F)==10) AUDIO_INACTIVITY();
  }

#if defined(SDCARD)
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

    // Write the raw byte out to log file, if open
    if (testLogOpen && (g_oLogFile.fs != 0))
      f_putc(userDataRxBuffer[byt], &g_oLogFile);

  }
#endif

  lcd_clear();
  uint8_t evt = getEvent();
  evt = checkTrim(evt);

  // TODO port lightOnStickMove from er9x + flash saving, call checkBacklight()
  if(g_LightOffCounter) g_LightOffCounter--;
  if(evt) g_LightOffCounter = g_eeGeneral.lightAutoOff*500; // on keypress turn the light on 5*100

  if (getSwitch(g_eeGeneral.lightSw,0) || g_LightOffCounter)
    BACKLIGHT_ON;
  else
    BACKLIGHT_OFF;

#if defined(PCBARM) && defined(FRSKY)
  check_frsky();
#endif

  g_menuStack[g_menuStackPtr](evt);
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

  switch( tmr10ms & 0x1f ) { //alle 10ms*32

    case 2:
      {
        int32_t instant_vbat = anaIn(7);
#if defined(PCBARM)
        instant_vbat = ( instant_vbat + instant_vbat*(g_eeGeneral.vBatCalib)/128 ) * 4191 ;
        instant_vbat /= 55296  ;
#elif defined(PCBV4)
        instant_vbat = ((uint32_t)instant_vbat*1112 + (int32_t)instant_vbat*g_eeGeneral.vBatCalib + (BandGap<<2)) / (BandGap<<3);
#else
        instant_vbat = (instant_vbat*16 + instant_vbat*g_eeGeneral.vBatCalib/8) / BandGap;
#endif
        if (g_vbat100mV == 0 || g_menuStack[g_menuStackPtr] != menuMainView) g_vbat100mV = instant_vbat;
        g_vbat100mV = (instant_vbat + g_vbat100mV*7) / 8;

        static uint8_t s_batCheck;
        s_batCheck+=32;
        if (s_batCheck==0 && g_vbat100mV<g_eeGeneral.vBatWarn && g_vbat100mV>50) {
          AUDIO_ERROR(); // TODO AUDIO_TX_BATTERY_LOW()
        }
      }
      break;
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
ISR(TIMER2_COMPA_vect, ISR_NOBLOCK) //10ms timer
#else
ISR(TIMER0_COMP_vect, ISR_NOBLOCK) //10ms timer
#endif
{
  cli();
  
#if defined (PCBV4)
  static uint8_t accuracyWarble = 4; // becasue 16M / 1024 / 100 = 156.25. So bump every 4.
  uint8_t bump = (!(accuracyWarble++ & 0x03)) ? 157 : 156;
  TIMSK2 &= ~(1<<OCIE2A); //stop reentrance
  OCR2A += bump;
#else
  TIMSK &= ~(1<<OCIE0); //stop reentrance
#if defined (AUDIO)
  OCR0 += 2; // run much faster, to generate speaker tones
#else
  static uint8_t accuracyWarble = 4; // becasue 16M / 1024 / 100 = 156.25. So bump every 4.
  uint8_t bump = (!(accuracyWarble++ & 0x03)) ? 157 : 156;
  OCR0 += bump;
#endif
#endif

  sei();
  
#if defined (PCBSTD) && defined (AUDIO)
  AUDIO_DRIVER();
  static uint8_t cnt10ms = 77; // execute 10ms code once every 78 ISRs
  if (cnt10ms-- == 0) { // BEGIN { ... every 10ms ... }
    // Begin 10ms event
    cnt10ms = 77;
#endif

    AUDIO_HEARTBEAT();

#ifdef HAPTIC
    HAPTIC_HEARTBEAT();
#endif

#ifdef DEBUG
    // Record start time from TCNT1 to record excution time
    cli();
    uint16_t dt=TCNT1;// TCNT1 (used for PPM out pulse generation) is running at 2MHz
    sei();
#endif

    per10ms();

#if defined (PCBV4)
    disk_timerproc();
#endif

    heartbeat |= HEART_TIMER10ms;
    
#ifdef DEBUG
    // Record per10ms ISR execution time, in us(x2) for STAT2 page
    cli();
    uint16_t dt2 = TCNT1; // capture end time
    sei();
    g_time_per10 = dt2 - dt; // NOTE: These spike to nearly 65535 just now and then. Why? :/
#endif

#if defined (PCBSTD) && defined (AUDIO)
  } // end 10ms event
#endif

  cli();
#if defined (PCBV4)
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

  // G: We prcoess g_ppmIns immediately here, to make servo movement as smooth as possible
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

extern uint16_t g_timeMain;

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

#if defined (PCBV4)
/*---------------------------------------------------------*/
/* User Provided Date/Time Function for FatFs module       */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support a real time clock.          */
/* This is not required in read-only configuration.        */

uint32_t o9x_get_fattime(void) // TODO why not in ff.cpp?
{
  struct gtm t;
  filltm(&g_unixTime, &t); // create a struct tm date/time structure from global unix time stamp

  /* Pack date and time into a DWORD variable */
  return    ((DWORD)(t.tm_year - 80) << 25)
    | ((uint32_t)(t.tm_mon+1) << 21)
    | ((uint32_t)t.tm_mday << 16)
    | ((uint32_t)t.tm_hour << 11)
    | ((uint32_t)t.tm_min << 5)
    | ((uint32_t)t.tm_sec >> 1);
}
#endif

void instantTrim()
{
  uint8_t phase = getFlightPhase();

  for (uint8_t i=0; i<NUM_STICKS; i++) {
    if (i!=THR_STICK) {
      // don't instant trim the throttle stick
      uint8_t trim_phase = getTrimFlightPhase(phase, i);
      s_perout_mode = e_instant_trim;
      evalSticks(phase);
      s_perout_mode = e_perout_mode_normal;
      int16_t trim = (anas[i] + trims[i]) / 2;
      if (trim < TRIM_EXTENDED_MIN) {
        trim = TRIM_EXTENDED_MIN;
      }
      if (trim > TRIM_EXTENDED_MAX) {
        trim = TRIM_EXTENDED_MAX;
      }
      setTrimValue(trim_phase, i, trim);
    }
  }

  STORE_MODELVARS;
  AUDIO_WARNING2();
}

void moveTrimsToOffsets() // copy state of 3 primary to subtrim
{
  int16_t zeros[NUM_CHNOUT];
  uint8_t phase = getFlightPhase();

  s_perout_mode = e_perout_mode_zeros;
  perOut(phase); // do output loop - zero input sticks and trims
  for (uint8_t i=0; i<NUM_CHNOUT; i++) {
    zeros[i] = applyLimits(i, chans[i]);
  }

  s_perout_mode = e_perout_mode_trims;
  perOut(phase); // do output loop - only trims
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
      int16_t original_trim = getTrimValue(phase, i);
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

#if defined (PCBV4)
// Rotary encoder interrupts
volatile uint8_t g_rotenc[2] = {0};
#endif

#ifndef SIMU

#if defined (PCBV4)
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
#endif

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

int main(void)
{
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

#if defined (FRSKY) and !defined (DSM2_SERIAL)
  FRSKY_Init();
#endif

#if defined (DSM2_SERIAL) and !defined (FRSKY)
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

  eeReadAll();

  uint8_t cModel = g_eeGeneral.currModel;

#if defined(PCBARM)
  set_volume(g_eeGeneral.speakerVolume);
  PWM->PWM_CH_NUM[0].PWM_CDTYUPD = g_eeGeneral.backlightBright;
#endif

#if defined(PCBV4)
  if (MCUSR != (1 << PORF) && !g_eeGeneral.unexpectedShutdown)
#elif defined(PCBSTD)
  if (MCUCSR != (1 << PORF))
#else
  if (!g_eeGeneral.unexpectedShutdown)
#endif
   {
#ifdef SPLASH
#ifdef DSM2
    // TODO rather use another Model Parameter
    if (g_model.protocol != PROTO_DSM2)
#endif
      doSplash();
#endif

#if !defined(PCBARM)
    checkLowEEPROM();
#endif

    getADC_single();
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

  clearKeyEvents(); //make sure no keys are down before proceeding

  lcdSetRefVolt(g_eeGeneral.contrast);
  g_LightOffCounter = g_eeGeneral.lightAutoOff*500; //turn on light for x seconds - no need to press key Issue 152

  if(cModel!=g_eeGeneral.currModel) eeDirty(EE_GENERAL); // if model was quick-selected, make sure it sticks


#if defined(PCBARM)
  start_ppm_capture();
  // TODO inside startPulses?
#endif

  startPulses();

  if (check_soft_power() <= e_power_trainer) {
    wdt_enable(WDTO_500MS);
  }

#if defined(PCBARM)
  register uint32_t shutdown_state = 0;
#elif defined(PCBV4)
  uint8_t shutdown_state = 0;
#endif

  while(1) {
#if defined(PCBARM) || defined(PCBV4)
    if ((shutdown_state=check_soft_power()) > e_power_trainer)
      break;
#endif

#if defined(PCBARM)
    uint16_t t0 = getTmr2MHz();
#else
    uint16_t t0 = getTmr16KHz();
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

#if defined(PCBARM) && defined(REVB)
    Current_analogue = ( Current_analogue * 31 + s_anaFilt[8] ) >> 5 ;
#elif defined(PCBV4)
    // For PCB V4, use our own 1.2V, external reference (connected to ADC3)
    ADCSRB &= ~(1<<MUX5);
    ADMUX = 0x03|ADC_VREF_TYPE; // Switch MUX to internal reference
#elif defined(PCBSTD)
    ADMUX = 0x1E|ADC_VREF_TYPE; // Switch MUX to internal reference
#endif
  
    perMain();
    
    // Bandgap has had plenty of time to settle...
#if not defined(PCBARM)
    getADC_bandgap();
#endif

    if(heartbeat == 0x3)
    {
      wdt_reset();
      heartbeat = 0;
    }
#if defined(PCBARM)
    t0 = getTmr2MHz() - t0;
#else
    t0 = getTmr16KHz() - t0;
#endif

    if (t0 > g_timeMain) g_timeMain = t0 ;
  }

#if defined(PCBARM) || defined(PCBV4)
  // Time to switch off
  lcd_clear() ;
  displayPopup(STR_SHUTDOWN);
  g_eeGeneral.unexpectedShutdown=0;
  eeDirty(EE_GENERAL);
  eeCheck(true);
  lcd_clear() ;
  refreshDisplay() ;
  soft_power_off();            // Only turn power off if necessary
#endif

#if defined(PCBARM)
  if (shutdown_state == e_power_usb) {
    lcd_putcAtt( 48, 24, 'U', DBLSIZE ) ;
    lcd_putcAtt( 60, 24, 'S', DBLSIZE ) ;
    lcd_putcAtt( 72, 24, 'B', DBLSIZE ) ;
    refreshDisplay() ;
    usb_mode();
  }
#endif

#if defined(PCBARM) || defined(PCBV4)
  lcdSetRefVolt(0); // TODO before soft_power_off?
#endif

#if defined(PCBV4)
  //never return from main() - there is no code to return back, if any daelays occurs in physical power it does dead loop.
  wdt_disable();
  for(;;){}
#endif
}
#endif

