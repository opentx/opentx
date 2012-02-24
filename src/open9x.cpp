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

uint8_t g_tmr1Latency_max;
uint8_t g_tmr1Latency_min;
uint16_t g_timeMain;
#ifdef DEBUG
uint16_t g_time_per10;
#endif

#ifdef AUDIO
//new audio object
audioQueue  audio;
#endif

uint8_t heartbeat;

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
  if (idx <= ZCHAR_MAX) return pgm_read_byte(s_charTab+idx-37);
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

int16_t applyCurve(int16_t x, uint8_t idx, uint8_t srcRaw)
{
  switch(idx) {
  case 0:
    return x;
  case 1:
    if (srcRaw == MIX_FULL) { //FULL
      if (x<0 ) x=-RESX;   //x|x>0
      else x=-RESX+2*x;
    }
    else {
      if (x<0) x=0;   //x|x>0
    }
    return x;
  case 2:
    if (srcRaw == MIX_FULL) { //FULL
      if (x>0) x=RESX;   //x|x<0
      else x=RESX+2*x;
    }
    else {
      if (x>0) x=0;   //x|x<0
    }
    return x;
  case 3:       // x|abs(x)
    return abs(x);
  case 4:       //f|f>0
    return x>0 ? RESX : 0;
  case 5:       //f|f<0
    return x<0 ? -RESX : 0;
  case 6:       //f|abs(f)
    return x>0 ? RESX : -RESX;
  }
  return intpol(x, idx-7);
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
  static int16_t anas2[4]; // values before expo, to ensure same expo base when multiple expo lines are used
  memcpy(anas2, anas, sizeof(anas2));

  if (phase == 255)
    phase = getFlightPhase();

  int8_t cur_chn = -1;
  for (uint8_t i=0; i<DIM(g_model.expoData); i++) {
    ExpoData &ed = g_model.expoData[i];
    if (ed.mode==0) break; // end of list
    if (ed.chn == cur_chn)
      continue;
    if (ed.phase != 0) {
      if (ed.negPhase) {
        if (phase+1 == -ed.phase)
          continue;
      }
      else {
        if (phase+1 != ed.phase)
          continue;
      }
    }
    if (getSwitch(ed.swtch, 1)) {
      int16_t v = anas2[ed.chn];
      if((v<0 && ed.mode&1) || (v>=0 && ed.mode&2)) {
        cur_chn = ed.chn;
        int16_t k = ed.expo;
        v = expo(v, k);
        if (ed.curve) v = applyCurve(v, ed.curve > 10 ? ed.curve + 4 : ed.curve, 0);
        v = ((int32_t)v * ed.weight) / 100;
        anas[cur_chn] = v;
      }
    }
  }
}

/*TODO evaluate impact FORCEINLINE */
bool s_noStickInputs = false;
int16_t getValue(uint8_t i)
{
    if(i<NUM_STICKS+NUM_POTS) return (s_noStickInputs ? 0 : calibratedStick[i]);
    else if(i<MIX_FULL/*srcRaw is shifted +1!*/) return 1024; //FULL/MAX
    else if(i<PPM_BASE+NUM_CAL_PPM) return (g_ppmIns[i-PPM_BASE] - g_eeGeneral.trainer.calib[i-PPM_BASE])*2;
    else if(i<PPM_BASE+NUM_PPM) return g_ppmIns[i-PPM_BASE]*2;
    else if(i<CHOUT_BASE+NUM_CHNOUT) return ex_chans[i-CHOUT_BASE];
    else if(i<CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS) return s_timerVal[i-CHOUT_BASE-NUM_CHNOUT];
#if defined(FRSKY)
    else if(i<CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS+2) return frskyTelemetry[i-CHOUT_BASE-NUM_CHNOUT-MAX_TIMERS].value;
#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
    else if(i<CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS+3) return frskyHubData.baroAltitude_bp + frskyHubData.baroAltitudeOffset;
#endif
#if defined(FRSKY_HUB)
    else if(i<CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS+4) return (frskyHubData.rpm / 2);
    else if(i<CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS+5) return frskyHubData.fuelLevel;
    else if(i<CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS+6) return frskyHubData.temperature1;
    else if(i<CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS+7) return frskyHubData.temperature2;
    else if(i<CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS+8) return frskyHubData.gpsSpeed_ap;
    else if(i<CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS+9) return frskyHubData.minCellVolts;
#endif
#endif
    else return 0;
}

volatile uint16_t s_last_switch_used;
volatile uint16_t s_last_switch_value;
bool __getSwitch(int8_t swtch)
{
  bool result;

  if (swtch == 0)
    return s_last_switch_used & (1<<15);

  uint8_t cs_idx = abs(swtch);

  if (cs_idx == MAX_SWITCH) {
    result = true;
  }
  else if (cs_idx < MAX_SWITCH-NUM_CSW) {
    result = keyState((EnumKeys)(SW_BASE+cs_idx-1));
  }
  else {
    cs_idx -= MAX_SWITCH-NUM_CSW;
    volatile CustomSwData &cs = g_model.customSw[cs_idx];
    if (cs.func == CS_OFF) return false;

    uint8_t s = CS_STATE(cs.func);
    if (s == CS_VBOOL) {
      uint16_t mask = (1 << cs_idx);
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
        s_last_switch_value |= (1<<cs_idx);
      else
        s_last_switch_value &= ~(1<<cs_idx);
    }
    else {
      int16_t x = getValue(cs.v1-1);
      int16_t y;
      if (s == CS_VOFS) {
#ifdef FRSKY
#if defined(FRSKY_HUB)
        // Fill the threshold array
        if (cs.v1 > CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS+2)
          barsThresholds[cs.v1-CHOUT_BASE-NUM_CHNOUT-MAX_TIMERS-3] = 128 + cs.v2;
        // TODO CELL?
        // FUEL, T1, T2, SPEED
        if (cs.v1 > CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS+4)
          y = (128+cs.v2);
        // RPMs
        else if (cs.v1 > CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS+3)
          y = (128+cs.v2) * 25;
        else
#endif
#if defined(FRSKY_HUB) || defined(WS_HOW_HIGH)
        // ALT
        if (cs.v1 > CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS+2)
          y = (128+cs.v2) * 4;
        else
#endif
        // Volts
        if (cs.v1 > CHOUT_BASE+NUM_CHNOUT+MAX_TIMERS)
          y = 128+cs.v2;
        else
#endif
        // Timers
        if (cs.v1 > CHOUT_BASE+NUM_CHNOUT)
          y = 98+cs.v2;
        else
          y = calc100toRESX(cs.v2);

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
        }
      }
    }
  }

  return swtch > 0 ? result : !result;
}

bool getSwitch(int8_t swtch, bool nc)
{
  s_last_switch_used = (nc<<15);
  return __getSwitch(swtch);
}

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

int16_t getTrimValue(uint8_t phase, uint8_t idx)
{
  int16_t result;
  if (s_trimPtr[idx]) {
    result = *s_trimPtr[idx];
  }
  else {
    PhaseData *p = phaseaddress(phase);
    result = (((int16_t)p->trim[idx]) << 2) + ((p->trim_ext >> (2*idx)) & 0x03);
  }
  return result;
}

void setTrimValue(uint8_t phase, uint8_t idx, int16_t trim)
{
  if (s_trimPtr[idx]) {
    *s_trimPtr[idx] = limit((int16_t)-125, trim, (int16_t)+125);
  }
  else {
    PhaseData *p = phaseaddress(phase);
    p->trim[idx] = (int8_t)(trim >> 2);
    p->trim_ext = (p->trim_ext & ~(0x03 << (2*idx))) + (((trim & 0x03) << (2*idx)));
  }
  STORE_MODELVARS;
}

uint8_t getTrimFlightPhase(uint8_t idx, uint8_t phase)
{
  for (uint8_t i=0; i<MAX_PHASES; i++) {
    if (phase == 0) return 0;
    int16_t trim = getTrimValue(phase, idx);
    if (trim <= TRIM_EXTENDED_MAX) return phase;
    uint8_t result = trim-TRIM_EXTENDED_MAX-1;
    if (result >= phase) result++;
    phase = result;
  }
  return 0;
}

FORCEINLINE uint8_t keyDown()
{
#if defined (PCBV4)
  return (~PINL) & 0x3F;
#else
  return (~PINB) & 0x7E;
#endif
}

void clearKeyEvents()
{
#ifdef SIMU
    while (keyDown() && main_thread_running) sleep(1/*ms*/);
#else
    while (keyDown());  // loop until all keys are up
#endif
    putEvent(0);
}

#ifdef SPLASH
void doSplash()
{
    if(!g_eeGeneral.disableSplashScreen)
    {
      if(getSwitch(g_eeGeneral.lightSw,0) || g_eeGeneral.lightAutoOff)
        BACKLIGHT_ON;
      else
        BACKLIGHT_OFF;

      lcd_clear();
      lcd_img(0, 0, s9xsplash,0,0);
      refreshDisplay();
      lcdSetRefVolt(g_eeGeneral.contrast);
      clearKeyEvents();

#ifndef SIMU
      for(uint8_t i=0; i<32; i++)
        getADC_filt(); // init ADC array
#endif

#define INAC_DEVISOR 256   // Bypass splash screen with stick movement
      uint16_t inacSum = 0;
      for(uint8_t i=0; i<4; i++)
        inacSum += anaIn(i)/INAC_DEVISOR;

      uint16_t tgtime = get_tmr10ms() + SPLASH_TIMEOUT;  //2sec splash screen
      while(tgtime != get_tmr10ms())
      {
#ifdef SIMU
        if (!main_thread_running) return;
        sleep(1/*ms*/);
#else
        getADC_filt();
#endif
        uint16_t tsum = 0;
        for(uint8_t i=0; i<4; i++)
          tsum += anaIn(i)/INAC_DEVISOR;

        if(keyDown() || (tsum!=inacSum))   return;  //wait for key release

        if(getSwitch(g_eeGeneral.lightSw,0) || g_eeGeneral.lightAutoOff)
          BACKLIGHT_ON;
        else
          BACKLIGHT_OFF;
      }
    }
}
#endif

void checkLowEEPROM()
{
  if(g_eeGeneral.disableMemoryWarning) return;
  if(EeFsGetFree() < 200)
  {
    alert(STR_EEPROMLOWMEM);
  }
}

void alertMessages( const pm_char * s, const pm_char * t )
{
  lcd_clear();
  lcd_putsAtt(64-5*FW,0*FH,STR_ALERT,DBLSIZE);
  lcd_putsLeft(4*FH,s);
  lcd_putsLeft(5*FH,t);
  lcd_putsLeft(6*FH,  STR_PRESSANYKEYTOSKIP ) ;
  refreshDisplay();
  lcdSetRefVolt(g_eeGeneral.contrast);

  clearKeyEvents();
}

void checkTHR()
{
  if(g_eeGeneral.disableThrottleWarning) return;

  int thrchn=(2-(g_eeGeneral.stickMode&1));//stickMode=0123 -> thr=2121

  int16_t lowLim = THRCHK_DEADBAND + g_eeGeneral.calibMid[thrchn] - g_eeGeneral.calibSpanNeg[thrchn];

#ifndef SIMU
  getADC_single();   // if thr is down - do not display warning at all
#endif
  int16_t v = anaIn(thrchn);
  if(v<=lowLim) return;

  // first - display warning
  alertMessages( STR_THROTTLENOTIDLE, STR_RESETTHROTTLE ) ;

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

      if(v<=lowLim || keyDown()) {
        clearKeyEvents();
        return;
      }

      if(getSwitch(g_eeGeneral.lightSw,0) || g_eeGeneral.lightAutoOff)
          BACKLIGHT_ON;
      else
          BACKLIGHT_OFF;
  }
}

void checkAlarm() // added by Gohst
{
  if (g_eeGeneral.disableAlarmWarning) return;
  if (g_eeGeneral.beeperMode == -2/*TODO constant*/) alert(STR_ALARMSDISABLED);
}

void checkSwitches()
{
  if(!g_eeGeneral.switchWarning) return; // if warning is on

  // first - display warning
  alertMessages( STR_SWITCHESNOTOFF, STR_PLEASERESETTHEM ) ;

  bool state = (g_eeGeneral.switchWarning > 0);

  //loop until all switches are reset
  while (1)
  {
#ifdef SIMU
    if (!main_thread_running) return;
    sleep(1/*ms*/);
#endif

    uint8_t i;
    for(i=SW_BASE; i<SW_Trainer; i++)
    {
        if(i==SW_ID0) continue;
        if(getSwitch(i-SW_BASE+1,0) != state) break;
    }
    if(i==SW_Trainer || keyDown()) return;

    if(getSwitch(g_eeGeneral.lightSw,0) || g_eeGeneral.lightAutoOff)
      BACKLIGHT_ON;
    else
      BACKLIGHT_OFF;
  }
}

void message(const pm_char * s)
{
  lcd_clear();
  lcd_putsAtt(64-5*FW, 0*FH, STR_MESSAGE, DBLSIZE);
  lcd_putsLeft(4*FW,s);
  refreshDisplay();
  lcdSetRefVolt(g_eeGeneral.contrast);
}

void alert(const pm_char * s, bool defaults)
{
  lcd_clear();
  lcd_putsAtt(64-5*FW, 0*FH, STR_ALERT, DBLSIZE);
  lcd_putsLeft(4*FH,s);
  lcd_puts(64-LEN_PRESSANYKEY*FW/2, 7*FH, STR_PRESSANYKEY);
  refreshDisplay();
  lcdSetRefVolt(defaults ? 25 : g_eeGeneral.contrast);
  AUDIO_ERROR();
  clearKeyEvents();
  while(1)
  {
#ifdef SIMU
    if (!main_thread_running) return;
    sleep(1/*ms*/);
#endif
    if(keyDown())   return;  //wait for key release

    if(getSwitch(g_eeGeneral.lightSw,0) || g_eeGeneral.lightAutoOff || defaults)
        BACKLIGHT_ON;
      else
        BACKLIGHT_OFF;

    wdt_reset();
  }
}

int8_t *s_trimPtr[NUM_STICKS] = { NULL, NULL, NULL, NULL };

uint8_t checkTrim(uint8_t event)
{
  int8_t  k = (event & EVT_KEY_MASK) - TRM_BASE;
  int8_t  s = g_model.trimInc;

  if (k>=0 && k<8) { // && (event & _MSK_KEY_REPT))
    //LH_DWN LH_UP LV_DWN LV_UP RV_DWN RV_UP RH_DWN RH_UP
    uint8_t idx = CONVERT_MODE(1+k/2) - 1;
    uint8_t phase = getTrimFlightPhase(idx, getFlightPhase());
    int16_t before = getTrimValue(phase, idx);
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
#if defined (AUDIO)
      audio.event(AU_TRIM_MOVE, after);
#else
      warble = false;
      AUDIO_WARNING2();
#endif
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
#ifdef PCBV4
uint16_t BandGap = 2040 ;
#else
uint16_t BandGap ;
#endif
static uint16_t s_anaFilt[8];
uint16_t anaIn(uint8_t chan)
{
  //                     ana-in:   3 1 2 0 4 5 6 7
  //static pm_char crossAna[] PROGMEM ={4,2,3,1,5,6,7,0}; // wenn schon Tabelle, dann muss sich auch lohnen
  //                        Google Translate (German): // if table already, then it must also be worthwhile
  static const pm_char crossAna[] PROGMEM ={3,1,2,0,4,5,6,7};
  volatile uint16_t *p = &s_anaFilt[pgm_read_byte(crossAna+chan)];
  return *p;
}

#define ADC_VREF_TYPE 0x40 // AVCC with external capacitor at AREF pin
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

      t_ana[0][adc_input]  = (t_ana[0][adc_input]  + ADCW               ) >> 1;
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

getADCp getADC[3] = {
  getADC_single,
  getADC_osmp,
  getADC_filt
};

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

#endif // SIMU

uint16_t g_vbat100mV = 0;

volatile uint8_t tick10ms = 0;
uint16_t g_LightOffCounter;

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
  s_timerVal[idx] = (idx == 0 ? g_model.timer1.val : g_model.timer2.val);
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
int16_t  anas [NUM_XCHNRAW] = {0};
int16_t  trims[NUM_STICKS] = {0};
int32_t  chans[NUM_CHNOUT] = {0};
uint32_t inacCounter = 0;
uint16_t inacSum = 0;
uint8_t  bpanaCenter = 0;
int16_t  sDelay[MAX_MIXERS] = {0};
int32_t  act   [MAX_MIXERS] = {0};
uint8_t  swOn  [MAX_MIXERS] = {0};
uint8_t mixWarning;

FORCEINLINE void evalTrims(uint8_t phase)
{
  for (uint8_t i=0; i<NUM_STICKS; i++) {
    // do trim -> throttle trim if applicable
    int16_t v = anas[i];
    int32_t vv = 2*RESX;
    int16_t trim = getTrimValue(getTrimFlightPhase(i, phase), i);
    if (i==THR_STICK && g_model.thrTrim) {
      if (g_eeGeneral.throttleReversed)
        trim = -trim;
      vv = ((int32_t)trim-TRIM_MIN)*(RESX-v)/(2*RESX);
    }
    else if (trimsCheckTimer > 0) {
      trim = 0;
    }

    trims[i] = (vv==2*RESX) ? trim*2 : (int16_t)vv*2; // if throttle trim -> trim low end
  }
}

uint8_t evalSticks(uint8_t phase)
{
#ifdef HELI
  uint16_t d = 0;
  if(g_model.swashR.value) {
    uint32_t v = (int32_t(calibratedStick[ELE_STICK])*calibratedStick[ELE_STICK] +
        int32_t(calibratedStick[AIL_STICK])*calibratedStick[AIL_STICK]);
    uint32_t q = int32_t(RESX)*g_model.swashR.value/100;
    q *= q;
    if(v>q)
      d = isqrt32(v);
  }
#endif

  uint8_t anaCenter = 0;

  for(uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {

    // normalization [0..2048] -> [-1024..1024]
    uint8_t ch = (i < NUM_STICKS ? CONVERT_MODE(i+1) - 1 : i);
    int16_t v = anaIn(i);

#ifndef SIMU
    v -= g_eeGeneral.calibMid[i];
    v  =  v * (int32_t)RESX /  (max((int16_t)100,(v>0 ?
                                     g_eeGeneral.calibSpanPos[i] :
                                     g_eeGeneral.calibSpanNeg[i])));
#endif

    if(v <= -RESX) v = -RESX;
    if(v >=  RESX) v =  RESX;

    if (g_eeGeneral.throttleReversed && ch==THR_STICK)
      v = -v;

    calibratedStick[ch] = v; //for show in expo
    if(!(v/16)) anaCenter |= 1<<ch;


    if (ch < NUM_STICKS) { //only do this for sticks
      if (!s_noStickInputs && (isFunctionActive(FUNC_TRAINER) || isFunctionActive(FUNC_TRAINER_RUD+ch))) {
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

    }
    anas[ch] = v; //set values for mixer
  }

  /* EXPOs */
  applyExpos(anas, phase);

  /* TRIMs */
  evalTrims(phase);

  return anaCenter;
}

uint16_t active_functions = 0; // current max = 16 functions

void evalFunctions()
{
  assert(sizeof(active_functions)*8 > FUNC_MAX-NUM_CHNOUT);

  for (uint8_t i=0; i<NUM_CHNOUT; i++)
    safetyCh[i] = -128; // not defined

  for (uint8_t i=0; i<NUM_FSW; i++) {
    FuncSwData *sd = &g_model.funcSw[i];
    if (sd->swtch) {
      uint16_t mask = (sd->func >= FUNC_TRAINER ? (1 << (sd->func-FUNC_TRAINER)) : 0);
      if (getSwitch(sd->swtch, 0)) {
        if (sd->func < FUNC_TRAINER) {
          safetyCh[sd->func] = (int8_t)sd->param;
        }
        if (sd->func == FUNC_PLAY_SOUND) {
#if defined(AUDIO)
          audioDefevent(AU_FRSKY_FIRST+sd->param);
#else
          beep(3);
#endif
        }
#if defined(SOMO)
        if (sd->func == FUNC_PLAY_SOMO && (~active_functions & mask)) {
          somoPushPrompt(sd->param);
        }
#endif
        active_functions |= mask;
      }
      else {
        active_functions &= (~mask);
      }
    }
  }
}

void perOut(int16_t *chanOut, uint8_t phase)
{
  uint8_t anaCenter = evalSticks(phase);

  //===========BEEP CENTER================
  anaCenter &= g_model.beepANACenter;
  if(((bpanaCenter ^ anaCenter) & anaCenter)) AUDIO_WARNING1();
  bpanaCenter = anaCenter;

  anas[MIX_MAX-1]  = RESX;     // MAX
  anas[MIX_FULL-1] = RESX;     // FULL

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
      if(g_model.swashR.collectiveSource)
          vc = anas[g_model.swashR.collectiveSource-1];

      if(g_model.swashR.invertELE) vp = -vp;
      if(g_model.swashR.invertAIL) vr = -vr;
      if(g_model.swashR.invertCOL) vc = -vc;

      switch (g_model.swashR.type)
      {
      case (SWASH_TYPE_120):
          vp = REZ_SWASH_Y(vp);
          vr = REZ_SWASH_X(vr);
          anas[MIX_CYC1-1] = vc - vp;
          anas[MIX_CYC2-1] = vc + vp/2 + vr;
          anas[MIX_CYC3-1] = vc + vp/2 - vr;
          break;
      case (SWASH_TYPE_120X):
          vp = REZ_SWASH_X(vp);
          vr = REZ_SWASH_Y(vr);
          anas[MIX_CYC1-1] = vc - vr;
          anas[MIX_CYC2-1] = vc + vr/2 + vp;
          anas[MIX_CYC3-1] = vc + vr/2 - vp;
          break;
      case (SWASH_TYPE_140):
          vp = REZ_SWASH_Y(vp);
          vr = REZ_SWASH_Y(vr);
          anas[MIX_CYC1-1] = vc - vp;
          anas[MIX_CYC2-1] = vc + vp + vr;
          anas[MIX_CYC3-1] = vc + vp - vr;
          break;
      case (SWASH_TYPE_90):
          vp = REZ_SWASH_Y(vp);
          vr = REZ_SWASH_Y(vr);
          anas[MIX_CYC1-1] = vc - vp;
          anas[MIX_CYC2-1] = vc + vr;
          anas[MIX_CYC3-1] = vc - vr;
          break;
      default:
          break;
      }
  }
#endif

  if (s_noStickInputs) {
    for (uint8_t i=0; i<NUM_STICKS; i++) {
      if (i!=THR_STICK) {
        anas[i] = 0;
      }
    }
    for (uint8_t i=0; i<NUM_PPM; i++) anas[i+PPM_BASE] = 0;
  }
  else {
    for (uint8_t i=0; i<NUM_CAL_PPM; i++)       anas[i+PPM_BASE] = (g_ppmIns[i] - g_eeGeneral.trainer.calib[i])*2; // add ppm channels
    for (uint8_t i=NUM_CAL_PPM; i<NUM_PPM; i++) anas[i+PPM_BASE] = g_ppmIns[i]*2; // add ppm channels
  }
  
  for (uint8_t i=CHOUT_BASE; i<CHOUT_BASE+NUM_CHNOUT; i++) anas[i] = chans[i-CHOUT_BASE]; // other mixes previous outputs

  memset(chans, 0, sizeof(chans));        // All outputs to 0

  s_trimPtr[0] = NULL;
  s_trimPtr[1] = NULL;
  s_trimPtr[2] = NULL;
  s_trimPtr[3] = NULL;

  //========== MIXER LOOP ===============
  mixWarning = 0;
  for(uint8_t i=0; i<MAX_MIXERS; i++) {

    MixData *md = mixaddress( i ) ;

    if((md->destCh==0) || (md->destCh>NUM_CHNOUT)) break;

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

    //Notice 0 = NC switch means not used -> always on line
    int16_t v  = 0;
    uint8_t swTog;

    //swOn[i]=false;
    if (!getSwitch(md->swtch,1)) { // switch on?  if no switch selected => on
      swTog = swOn[i];
      swOn[i] = false;
      if(md->srcRaw!=MIX_MAX && md->srcRaw!=MIX_FULL) continue;// if not MAX or FULL - next loop
      if(md->mltpx==MLTPX_REP) continue; // if switch is off and REPLACE then off
      v = (md->srcRaw == MIX_FULL ? -RESX : 0); // switch is off and it is either MAX=0 or FULL=-512
    }
    else {
      swTog = !swOn[i];
      swOn[i] = true;
      uint8_t k = md->srcRaw-1;
      v = anas[k]; //Switch is on. MAX=FULL=512 or value.
      if (k>=CHOUT_BASE && (k<i)) v = chans[k]; // if we've already calculated the value - take it instead // anas[i+CHOUT_BASE] = chans[i]
      if (md->mixWarn) mixWarning |= 1<<(md->mixWarn-1); // Mix warning
      if (md->carryTrim == 2/*TODO constant*/ && md->srcRaw <= NUM_STICKS) {
        s_trimPtr[md->srcRaw-1] = &md->sOffset;  // use the value stored here for the trim
      }
    }

    //========== INPUT OFFSET ===============
    if(md->sOffset) v += calc100toRESX(md->sOffset);

    //========== DELAY and PAUSE ===============
    if (md->speedUp || md->speedDown || md->delayUp || md->delayDown)  // there are delay values
    {
#define DEL_MULT 256

      int16_t diff = v-act[i]/DEL_MULT;

      if(swTog) {
          //need to know which "v" will give "anas".
          //curves(v)*weight/100 -> anas
          // v * weight / 100 = anas => anas*100/weight = v
        if(md->mltpx==MLTPX_REP)
        {
            act[i] = (int32_t)anas[md->destCh-1+CHOUT_BASE]*DEL_MULT;
            act[i] *=100;
            if(md->weight) act[i] /= md->weight;
        }
        diff = v-act[i]/DEL_MULT;
        if(diff) sDelay[i] = (diff<0 ? md->delayUp :  md->delayDown) * 100;
      }

      if (sDelay[i]) { // perform delay
        if(tick10ms) sDelay[i]--;
        if (sDelay[i] != 0) {
          v = act[i]/DEL_MULT; // Stay in old position until delay over
          diff = 0;
        }
      }

      if (diff && (md->speedUp || md->speedDown)) {
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
      else if (diff) {
        act[i]=(int32_t)v*DEL_MULT;
      }
    }

    //========== CURVES ===============
    if (md->curve)
      v = applyCurve(v, md->curve, md->srcRaw);

    //========== TRIM ===============
    if((md->carryTrim==0) && (md->srcRaw>0) && (md->srcRaw<=4)) v += trims[md->srcRaw-1];  //  0 = Trim ON  =  Default

    //========== MULTIPLEX ===============
    int32_t dv = (int32_t)v*md->weight;
    int32_t *ptr = &chans[md->destCh-1]; // Save calculating address several times
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

  //========== MIXER WARNING ===============
  // 1,2 or 3 "bips" (short beeps)
  //1= 00,08
  //2= 24,32,40
  //3= 56,64,72,80
  // Gruvin:  Sometimes, one or more of these beeps do not fire. That will be because the tmr10ms counter
  //          may not necessarily be exactly (==) the below figures when queried from inside perOut().
  //          But we only ever want a beep to fire once, so we have to use anexact counter match (not a range).
  //          My solution was to make mixWarning global and have the counter checks done inside per10ms();
  /* {
    uint16_t tmr10ms ;
    tmr10ms = get_tmr10ms() ;
    if(mixWarning & 1) if(((tmr10ms&0xFF)==  0)) beepWarn1();
    if(mixWarning & 2) if(((tmr10ms&0xFF)== 64) || ((tmr10ms&0xFF)== 72)) beepWarn1();
    if(mixWarning & 4) if(((tmr10ms&0xFF)==128) || ((tmr10ms&0xFF)==136) || ((tmr10ms&0xFF)==144)) beepWarn1();
  } */

  //========== LIMITS ===============
  for (uint8_t i=0;i<NUM_CHNOUT;i++) {
      // chans[i] holds data from mixer.   chans[i] = v*weight => 1024*100
      // later we multiply by the limit (up to 100) and then we need to normalize
      // at the end chans[i] = chans[i]/100 =>  -1024..1024
      // interpolate value with min/max so we get smooth motion from center to stop
      // this limits based on v original values and min=-1024, max=1024  RESX=1024
      //printf("chans%d=%d\n", i, chans[i]);fflush(stdout);
      int32_t q = chans[i];// + (int32_t)g_model.limitData[i].offset*100; // offset before limit

      chans[i] /= 100; // chans back to -1024..1024
      ex_chans[i] = chans[i]; //for getswitch

      int16_t ofs = g_model.limitData[i].offset;
      int16_t lim_p = 10*(g_model.limitData[i].max+100);
      int16_t lim_n = 10*(g_model.limitData[i].min-100); //multiply by 10 to get same range as ofs (-1000..1000)
      if(ofs>lim_p) ofs = lim_p;
      if(ofs<lim_n) ofs = lim_n;

      if(q) q = (q>0) ?
                q*((int32_t)lim_p-ofs)/100000 :
               -q*((int32_t)lim_n-ofs)/100000 ; //div by 100000 -> output = -1024..1024

      q += calc1000toRESX(ofs);
      lim_p = calc1000toRESX(lim_p);
      lim_n = calc1000toRESX(lim_n);
      if(q>lim_p) q = lim_p;
      if(q<lim_n) q = lim_n;
      if(g_model.limitData[i].revert) q=-q;// finally do the reverse.

      if (safetyCh[i] != -128)  // if safety channel available for channel check and replace val if needed
          q = calc100toRESX(safetyCh[i]);

      chanOut[i] = q; //copy consistent word to int-level
  }
}

#ifdef DISPLAY_USER_DATA
char userDataDisplayBuf[TELEM_SCREEN_BUFFER_SIZE];
#endif

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
#ifdef PCBV4
  static int16_t s_fp_chans[NUM_CHNOUT][MAX_PHASES];
#else
  static int8_t s_fp_chans[NUM_CHNOUT][MAX_PHASES];
#endif
  uint8_t phase = getFlightPhase();

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

  int16_t next_chans512[NUM_CHNOUT];

  if (s_fade_flight_phases) {
    int32_t sum_chans512[NUM_CHNOUT] = {0};
    int32_t weight = 0;
    for (uint8_t p=0; p<MAX_PHASES; p++) {
      if (s_fade_flight_phases & (1<<p)) {
        for (uint8_t i=0; i<NUM_CHNOUT; i++) {
#ifdef PCBV4
          chans[i] = ((int32_t)s_fp_chans[i][p]) << 2;
#else
          chans[i] = ((int32_t)s_fp_chans[i][p]) << 10;
#endif
        }
        perOut(next_chans512, p);
        for (uint8_t i=0; i<NUM_CHNOUT; i++) {
#ifdef PCBV4
          s_fp_chans[i][p] = chans[i] >> 2;
#else
          s_fp_chans[i][p] = chans[i] >> 10;
#endif
          sum_chans512[i] += (int32_t)next_chans512[i] * fp_act[p];
        }
        weight += fp_act[p];
      }
    }
    // printf("sum=%d, weight=%d ", sum_chans512[2], weight); fflush(stdout);
    assert(weight);
    for (uint8_t i=0; i<NUM_CHNOUT; i++) {
      next_chans512[i] = ((int32_t)sum_chans512[i] / weight);
    }
    // printf("output = %d\n", next_chans512[2]); fflush(stdout);
  }
  else {
    perOut(next_chans512, phase);
  }

  for (uint8_t i=0; i<NUM_CHNOUT; i++) {
    cli();
    g_chans512[i] = next_chans512[i];
    sei();
  }

  if (!eeprom_buffer_size) {
    if (theFile.isWriting())
      theFile.nextWriteStep();
    else if (s_eeDirtyMsk)
      eeCheck();
  }

  if (!tick10ms) return; //make sure the rest happen only every 10ms.

  uint16_t val;

  if (g_model.thrTraceSrc == 0) {
    val = calibratedStick[THR_STICK]; // get throttle channel value
    val = (g_eeGeneral.throttleReversed ? RESX-val : val+RESX);
  }
  else if (g_model.thrTraceSrc > NUM_POTS) {
    val = RESX + g_chans512[g_model.thrTraceSrc-NUM_POTS-1];
  }
  else {
    val = RESX + calibratedStick[g_model.thrTraceSrc+NUM_STICKS-1];
  }

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

  if (tmr10ms - s_time_tot >= 100) { // 1sec
    s_time_tot += 100;
    s_timeCumTot += 1;

    val = s_sum_1s / s_cnt_1s;
    s_timeCum16ThrP += val / 2;
    if (val) s_timeCumThr += 1;

    s_cnt_10s += s_cnt_1s;
    s_sum_10s += s_sum_1s;
    s_cnt_1s = 0;
    s_sum_1s = 0;

    if (tmr10ms - s_time_trace >= 1000) {// 10sec
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
  uint8_t i = 0;
  do {
    int8_t tm;
    uint16_t tv;
    if (i == 0) {
      tm = g_model.timer1.mode;
      tv = g_model.timer1.val;
    }
    else if (i == 1) {
      tm = g_model.timer2.mode;
      tv = g_model.timer2.val;
    }
    else {
      break;
    }

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

      if (atm>=(TMR_VAROFS+MAX_SWITCH-1)){ // toggeled switch
        if(!(sw_toggled[i] | s_sum[i] | s_cnt[i] | lastSwPos[i])) lastSwPos[i] = tm < 0;  // if initializing then init the lastSwPos
        uint8_t swPos = getSwitch(tm>0 ? tm-(TMR_VAROFS+MAX_SWITCH-1-1) : tm+(TMR_VAROFS+MAX_SWITCH-1-1), 0);
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
          if (atm<(TMR_VAROFS+MAX_SWITCH-1))
            sw_toggled[i] = getSwitch((tm>0 ? tm-(TMR_VAROFS-1) : tm+(TMR_VAROFS-1)), 0); // normal switch
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
    ++i;
  } while(1);

  static int16_t last_tmr;
  if (last_tmr != s_timerVal[0]) { // beep only if seconds advance
    if (s_timerState[0] == TMR_RUNNING) {
      if (g_eeGeneral.preBeep && g_model.timer1.val) { // beep when 30, 15, 10, 5,4,3,2,1 seconds remaining
        if(s_timerVal[0]==30) AUDIO_TIMER_30(); //beep three times
        if(s_timerVal[0]==20) AUDIO_TIMER_20(); //beep two times
        if(s_timerVal[0]==10) AUDIO_TIMER_10();
        if(s_timerVal[0]<= 3) AUDIO_TIMER_LT3();

        if(g_eeGeneral.flashBeep && (s_timerVal[0]==30 || s_timerVal[0]==20 || s_timerVal[0]==10 || s_timerVal[0]<=3))
          g_LightOffCounter = FLASH_DURATION;
      }

      if (g_eeGeneral.minuteBeep && (((g_model.timer1.val ? g_model.timer1.val-s_timerVal[0] : s_timerVal[0])%60)==0)) { // short beep every minute
        AUDIO_MINUTE_BEEP();
        if(g_eeGeneral.flashBeep) g_LightOffCounter = FLASH_DURATION;
      }
    }
    else if(s_timerState[0] == TMR_BEEPING) {
      AUDIO_WARNING1();
      if(g_eeGeneral.flashBeep) g_LightOffCounter = FLASH_DURATION;
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

  if (trimsCheckTimer > 0)
    trimsCheckTimer -= 1;

#if defined (LOGS)
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

  // TODO port lightOnStickMove from er9x
  if(g_LightOffCounter) g_LightOffCounter--;
  if(evt) g_LightOffCounter = g_eeGeneral.lightAutoOff*500; // on keypress turn the light on 5*100

  if( getSwitch(g_eeGeneral.lightSw,0) || g_LightOffCounter)
    BACKLIGHT_ON;
  else
    BACKLIGHT_OFF;

  g_menuStack[g_menuStackPtr](evt);
  refreshDisplay();

#if defined (PCBV4)
  // PPM signal on phono-jack. In or out? ...
  if(checkSlaveMode()) {
    PORTG |= (1<<OUT_G_SIM_CTL); // 1=ppm out
  }
  else{
    PORTG &=  ~(1<<OUT_G_SIM_CTL); // 0=ppm in
  }
#else
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
#if defined(PCBV4)
        instant_vbat = ((uint32_t)instant_vbat*1112 + (int32_t)instant_vbat*g_eeGeneral.vBatCalib + (BandGap<<2)) / (BandGap<<3);
#else
        instant_vbat = (instant_vbat*16 + instant_vbat*g_eeGeneral.vBatCalib/8) / BandGap;
#endif
        if (g_vbat100mV == 0 || g_menuStack[0] != menuMainView) g_vbat100mV = instant_vbat;
        g_vbat100mV = (instant_vbat + g_vbat100mV*7) / 8;

        static uint8_t s_batCheck;
        s_batCheck+=32;
        if (s_batCheck==0 && g_vbat100mV<g_eeGeneral.vBatWarn && g_vbat100mV>50) {
          AUDIO_ERROR();
          if (g_eeGeneral.flashBeep) g_LightOffCounter = FLASH_DURATION;
        }
      }
      break;
  }
}
int16_t g_ppmIns[8];
uint8_t ppmInState = 0; //0=unsync 1..8= wait for value i-1

#ifndef SIMU

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

#ifdef DEBUG
extern uint16_t g_time_per10; // instantiated in menus.cpp
#endif

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

#ifdef DEBUG
uint16_t DEBUG1 = 0;
uint16_t DEBUG2 = 0;
#endif

#endif

/*
   USART0 Transmit Data Register Emtpy ISR
   Used to transmit FrSky data packets and DSM2 protocol
*/

#if defined (FRSKY)
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

#if defined (DSM2_SERIAL)
FORCEINLINE void DSM2_USART0_vect()
{
  UDR0 = *((uint16_t*)pulses2MHzRPtr);

  pulses2MHzRPtr += sizeof(uint16_t);
  if (pulses2MHzRPtr == pulses2MHzWPtr) {
    UCSR0B &= ~(1 << UDRIE0); // disable UDRE0 interrupt
  }
}
#endif

#ifndef SIMU
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
      uint8_t trim_phase = getTrimFlightPhase(i, phase);
      s_noStickInputs = true;
      evalSticks(phase);
      s_noStickInputs = false;
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
  AUDIO_WARNING1();
}

void moveTrimsToOffsets() // copy state of 3 primary to subtrim
{
  int16_t zero_chans512[NUM_CHNOUT];

  s_noStickInputs = true;
  perOut(zero_chans512, getFlightPhase()); // do output loop - zero input sticks
  s_noStickInputs = false;

  for (uint8_t i=0; i<NUM_CHNOUT; i++)
    g_model.limitData[i].offset = limit((int16_t)-1000, zero_chans512[i], (int16_t)1000); // make sure the offset doesn't go haywire

  // reset all trims, except throttle
  for (uint8_t i=0; i<NUM_STICKS; i++) {
    if (i!=THR_STICK) {
      for (uint8_t phase=0; phase<MAX_PHASES; phase++) {
        int16_t trim = getTrimValue(phase, i);
        if (trim <= TRIM_EXTENDED_MAX)
          setTrimValue(phase, i, 0);
      }
    }
  }

  STORE_MODELVARS;
  AUDIO_WARNING1();
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
  if (input == 0 || input == 0b00001100) g_rotenc[0]--;
}
ISR(INT3_vect)
{
  uint8_t input = PIND & 0b00001100;
  if (input == 0 || input == 0b00001100) g_rotenc[0]++;
}

ISR(INT5_vect)
{
  uint8_t input = PINE & 0b01100000;
  if (input == 0 || input == 0b01100000) g_rotenc[1]++;
}
ISR(INT6_vect)
{
  uint8_t input = PINE & 0b01100000;
  if (input == 0 || input == 0b01100000) g_rotenc[1]--;
}
#endif

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

int main(void)
{
  // Set up I/O port data directions and initial states
  DDRA = 0xff;  PORTA = 0x00; // LCD data

#if defined (PCBV4)
  DDRB = 0b11000111;  PORTB = 0b00111111; // 7:SPKR, 6:PPM_OUT,  5:TrainSW,  4:IDL2_SW, SDCARD[3:MISO 2:MOSI 1:SCK 0:CS]
  DDRC = 0x3f;  PORTC = 0xc0; // 7:AilDR, 6:EleDR, LCD[5,4,3,2,1], 0:BackLight
  DDRD = 0b11000000;  PORTD = 0b11111100; // 7:VIB, 6:LED BL, 5:RENC2_PUSH, 4:RENC1_PUSH, 3:RENC2_B, 2:RENC2_A, 1:I2C_SDA, 0:I2C_SCL
  DDRE = 0b00001010;  PORTE = 0b11110101; // 7:PPM_IN, 6: RENC1_B, 5:RENC1_A, 4:USB_DNEG, 3:BUZZER, 2:USB_DPOS, 1:TELEM_TX, 0:TELEM_RX
  DDRF = 0x00;  PORTF = 0x00; // 7-4:JTAG, 3:ADC_REF_1.2V input, 2-0:ADC_SPARE_2-0
  DDRG = 0b00010000;  PORTG = 0xff; // 7-6:N/A, 5:GearSW, 4: Sim_Ctrl[out], 3:IDL1_Sw, 2:TCut_Sw, 1:RF_Power[in], 0: RudDr_Sw
  DDRH = 0b00110000;  PORTH = 0b11011111; // 7:0 Spare port [6:SOMO14D-BUSY 5:SOMO14D-DATA 4:SOMO14D-CLK] [2:VIB_OPTION -- setting to input for now]
  DDRJ = 0x00;  PORTJ = 0xff; // 7-0:Trim switch inputs
  DDRK = 0x00;  PORTK = 0x00; // anain. No pull-ups!
  DDRL = 0x80;  PORTL = 0x7f; // 7: Hold_PWR_On (1=On, default Off), 6:Jack_Presence_TTL, 5-0: User Button inputs
#else
  DDRB = 0x81;  PORTB = 0x7e; //pullups keys+nc
  DDRC = 0x3e;  PORTC = 0xc1; //pullups nc
  DDRD = 0x00;  PORTD = 0xff; //pullups keys
  DDRE = (1<<OUT_E_BUZZER);  PORTE = 0xff-(1<<OUT_E_BUZZER); //pullups + buzzer 0
  DDRF = 0x00;  PORTF = 0x00; //anain
  DDRG = 0x14;  PORTG = 0xfb; //pullups + SIM_CTL=1 = phonejack = ppm_in, Haptic output and off (0)
#endif

  lcd_init();

  ADMUX=ADC_VREF_TYPE;
  ADCSRA=0x85; // ADC enabled, pre-scaler division=32 (no interrupt, no auto-triggering)
#if defined (PCBV4)
  ADCSRB=(1<<MUX5);
#endif

  /**** Set up timer/counter 0 ****/
#if defined (PCBV4)
  /** Move old 64A Timer0 functions to Timer2 and use WGM on OC0(A) (PB7) for spkear tone output **/

  // TCNT0  10ms = 16MHz/1024/156(.25) periodic timer (100ms interval)
  //        cycles at 9.984ms but includes 1:4 duty cycle correction to /157 to average at 10.0ms
  TCCR2B  = (0b111 << CS20); // Norm mode, clk/1024 (differs from ATmega64 chip)
  OCR2A   = 156;
  TIMSK2 |= (1<<OCIE2A) |  (1<<TOIE2); // Enable Output-Compare and Overflow interrrupts

  // Set up Phase correct Waveform Gen. mode, at clk/64 = 250,000 counts/second
  // (Higher speed allows for finer control of frquencies in the audio range.)
  // Used for audio tone generation
  TCCR0B  = (1<<WGM02) | (0b011 << CS00);
  TCCR0A  = (0b01<<WGM00);

#else

# if defined (AUDIO)
  // TCNT0  10ms = 16MHz/1024/2(/78) periodic timer (for speaker tone generation)
  //        Capture ISR 7812.5/second -- runs per-10ms code segment once every 78
  //        cycles (9.984ms). Timer overflows at about 61Hz or once every 16ms.
  TCCR0  = (0b111 << CS00);//  Norm mode, clk/1024
  OCR0 = 2;
# else
  // TCNT0  10ms = 16MHz/1024/156 periodic timer (9.984ms)
  // (with 1:4 duty at 157 to average 10.0ms)
  // Timer overflows at about 61Hz or once every 16ms.
  TCCR0  = (0b111 << CS00);//  Norm mode, clk/1024
  OCR0 = 156;
# endif

  TIMSK |= (1<<OCIE0) |  (1<<TOIE0); // Enable Output-Compare and Overflow interrrupts
  /********************************/

#endif

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

  eeReadAll();

  uint8_t cModel = g_eeGeneral.currModel;

#if defined (PCBV4)
   if (MCUSR != (1 << PORF))  {
#else
   if (MCUCSR != (1 << PORF))  {
#endif

#ifdef SPLASH
    doSplash();
#endif

    checkLowEEPROM();

    getADC_single();
    checkTHR();

    checkSwitches();
    checkAlarm();
  }

  clearKeyEvents(); //make sure no keys are down before proceeding

  perOut(g_chans512, getFlightPhase());

  lcdSetRefVolt(g_eeGeneral.contrast);
  g_LightOffCounter = g_eeGeneral.lightAutoOff*500; //turn on light for x seconds - no need to press key Issue 152

  if(cModel!=g_eeGeneral.currModel) eeDirty(EE_GENERAL); // if model was quick-selected, make sure it sticks

#if defined (PCBV4)
// Initialise global unix timestamp with current time from RTC chip on SD card interface
  RTC rtc;
  struct gtm utm;
  rtc_gettime(&rtc);
  utm.tm_year = rtc.year - 1900;
  utm.tm_mon =  rtc.month - 1;
  utm.tm_mday = rtc.mday;
  utm.tm_hour = rtc.hour;
  utm.tm_min =  rtc.min;
  utm.tm_sec =  rtc.sec;
  utm.tm_wday = rtc.wday - 1;
  g_unixTime = mktime(&utm);
#endif

#if defined (PCBV4)
  /***************************************************/
  /* Rotary encoder interrupt set-up (V4 board only) */

  // All external interrupts initialise to disabled. But maybe not after 
  // a WDT or BOD event? So to be safe ...
  EIMSK = 0; // disable ALL external interrupts.

  // encoder 1
  EICRB = (1<<ISC60) | (1<<ISC50); // 01 = interrupt on any edge
  EIFR = (3<<INTF5); // clear the int. flag in case it got set when changing modes

  // encoder 2
  EICRA = (1<<ISC30) | (1<<ISC20); // do the same for encoder 1
  EIFR = (3<<INTF2);

  EIMSK = (3<<INT5) | (3<<INT2); // enable the two rot. enc. ext. int. pairs.
  /***************************************************/

  /*
   * SOMO set-up (V4 board only)
   */
  OCR4A = 0x7d;
  TCCR4B = (1 << WGM42) | (3<<CS40); // CTC OCR1A, 16MHz / 64 (4us ticks)
#endif

  startPulses();

  wdt_enable(WDTO_500MS);

  while(1) {
    uint16_t t0 = getTmr16KHz();

    getADC[g_eeGeneral.filterInput]();

#if defined(PCBV4)
    // For PCB V4, use our own 1.2V, external reference (connected to ADC3)
    ADCSRB &= ~(1<<MUX5);
    ADMUX = 0x03|ADC_VREF_TYPE; // Switch MUX to internal reference
#else
    ADMUX = 0x1E|ADC_VREF_TYPE; // Switch MUX to internal reference
#endif
  
    perMain();
    
    // Bandgap has had plenty of time to settle...
    getADC_bandgap();

    if(heartbeat == 0x3)
    {
      wdt_reset();
      heartbeat = 0;
    }
    t0 = getTmr16KHz() - t0;
    g_timeMain = max(g_timeMain,t0);
  }
}
#endif

