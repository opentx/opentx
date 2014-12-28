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
#define MENUS_STACK_SIZE    2000
#define MIXER_STACK_SIZE    500
#define AUDIO_STACK_SIZE    500
#define BT_STACK_SIZE       500
#define DEBUG_STACK_SIZE    500

#if defined(_MSC_VER)
  #define _ALIGNED(x) __declspec(align(x))
#elif defined(__GNUC__)
  #define _ALIGNED(x) __attribute__ ((aligned(x)))
#endif

OS_TID menusTaskId;
// stack must be aligned to 8 bytes otherwise printf for %f does not work!
OS_STK _ALIGNED(8) menusStack[MENUS_STACK_SIZE];

OS_TID mixerTaskId;
OS_STK mixerStack[MIXER_STACK_SIZE];

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

OS_MutexID audioMutex;
OS_MutexID mixerMutex;

#endif // defined(CPUARM)

#if defined(SPLASH)
const pm_uchar splashdata[] PROGMEM = { 'S','P','S',0,
#if defined(PCBTARANIS)
#include "bitmaps/splash_taranis.lbm"
#else
#include "bitmaps/splash_9x.lbm"
#endif
	'S','P','E',0};
const pm_uchar * splash_lbm = splashdata+4;
#endif

#if LCD_W >= 212
  const pm_uchar asterisk_lbm[] PROGMEM = {
    #include "bitmaps/asterisk_4bits.lbm"
  };
#else
  const pm_uchar asterisk_lbm[] PROGMEM = {
    #include "bitmaps/asterisk.lbm"
  };
#endif

#include "gui/menus.h"

EEGeneral  g_eeGeneral;
ModelData  g_model;

#if defined(PCBTARANIS) && defined(SDCARD)
uint8_t modelBitmap[MODEL_BITMAP_SIZE];
void loadModelBitmap(char *name, uint8_t *bitmap)
{
  uint8_t len = zlen(name, LEN_BITMAP_NAME);
  if (len > 0) {
    char lfn[] = BITMAPS_PATH "/xxxxxxxxxx.bmp";
    strncpy(lfn+sizeof(BITMAPS_PATH), name, len);
    strcpy(lfn+sizeof(BITMAPS_PATH)+len, BITMAPS_EXT);
    if (bmpLoad(bitmap, lfn, MODEL_BITMAP_WIDTH, MODEL_BITMAP_HEIGHT) == 0) {
      return;
    }
  }

  // In all error cases, we set the default logo
  memcpy(bitmap, logo_taranis, MODEL_BITMAP_SIZE);
}
#endif

#if !defined(CPUARM)
uint8_t g_tmr1Latency_max;
uint8_t g_tmr1Latency_min;
uint16_t lastMixerDuration;
#endif

uint8_t unexpectedShutdown = 0;

/* AVR: mixer duration in 1/16ms */
/* ARM: mixer duration in 0.5us */
uint16_t maxMixerDuration;

#if defined(AUDIO) && !defined(CPUARM)
audioQueue  audio;
#endif

#if defined(DSM2)
// TODO move elsewhere
uint8_t dsm2Flag = 0;
#if !defined(PCBTARANIS)
uint8_t s_bind_allowed = 255;
#endif
#endif

uint8_t heartbeat;

uint8_t stickMode;

#if defined(OVERRIDE_CHANNEL_FUNCTION)
safetych_t safetyCh[NUM_CHNOUT];
#endif

union ReusableBuffer reusableBuffer;

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
    0, 1, 2, 3,
    0, 2, 1, 3,
    3, 1, 2, 0,
    3, 2, 1, 0 };

volatile tmr10ms_t g_tmr10ms;

#if defined(CPUARM)
volatile uint8_t rtc_count = 0;
uint32_t watchdogTimeout = 0;

void watchdogSetTimeout(uint32_t timeout)
{
  watchdogTimeout = timeout;
}
#endif

void per10ms()
{
  g_tmr10ms++;

#if defined(CPUARM)
  if (watchdogTimeout) {
    watchdogTimeout -= 1;
    wdt_reset();  // Retrigger hardware watchdog
  }
  Tenms |= 1 ;                    // 10 mS has passed
#endif

  if (lightOffCounter) lightOffCounter--;
  if (flashCounter) flashCounter--;
  if (s_noHi) s_noHi--;
  if (trimsCheckTimer) trimsCheckTimer--;
  if (ppmInValid) ppmInValid--;

#if defined(RTCLOCK)
  /* Update global Date/Time every 100 per10ms cycles */
  if (++g_ms100 == 100) {
    g_rtcTime++;   // inc global unix timestamp one second
#if defined(COPROCESSOR)
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

#if defined(FRSKY) || defined(JETI)
  if (!IS_DSM2_SERIAL_PROTOCOL(s_current_protocol[0]))
    telemetryInterrupt10ms();
#endif

  // These moved here from evalFlightModeMixes() to improve beep trigger reliability.
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

  heartbeat |= HEART_TIMER_10MS;
}

FlightModeData *flightModeAddress(uint8_t idx)
{
  return &g_model.flightModeData[idx];
}

ExpoData *expoAddress(uint8_t idx )
{
  return &g_model.expoData[idx];
}

MixData *mixAddress(uint8_t idx)
{
  return &g_model.mixData[idx];
}

LimitData *limitAddress(uint8_t idx)
{
  return &g_model.limitData[idx];
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

#if defined(DEFAULT_MODE)
  g_eeGeneral.stickMode = DEFAULT_MODE-1;
#endif

#if defined(PCBTARANIS)
  g_eeGeneral.templateSetup = 17; /* TAER */
#endif

#if !defined(CPUM64)
  g_eeGeneral.backlightMode = e_backlight_mode_all;
  g_eeGeneral.lightAutoOff = 2;
  g_eeGeneral.inactivityTimer = 10;
#endif

#if defined(CPUARM)
  g_eeGeneral.wavVolume = 2;
  g_eeGeneral.backgroundVolume = 1;
#endif

  g_eeGeneral.chkSum = 0xFFFF;
}

uint16_t evalChkSum()
{
  uint16_t sum = 0;
  const int16_t *calibValues = (const int16_t *) &g_eeGeneral.calib[0];
  for (int i=0; i<12; i++)
    sum += calibValues[i];
  return sum;
}

#if defined(PCBTARANIS)
void clearInputs()
{
  memset(g_model.expoData, 0, sizeof(g_model.expoData)); // clear all expos
}

void defaultInputs()
{
  clearInputs();

  for (int i=0; i<NUM_STICKS; i++) {
    uint8_t stick_index = channel_order(i+1);
    ExpoData *expo = expoAddress(i);
    expo->srcRaw = MIXSRC_Rud - 1 + stick_index;
    expo->curve.type = CURVE_REF_EXPO;
    expo->chn = i;
    expo->weight = 100;
    expo->mode = 3; // TODO constant
    for (int c=0; c<4; c++) {
#if defined(TRANSLATIONS_CZ) && defined(CPUARM)
      g_model.inputNames[i][c] = char2idx(STR_INPUTNAMES[1+STR_INPUTNAMES[0]*(stick_index-1)+c]);
#else
      g_model.inputNames[i][c] = char2idx(STR_VSRCRAW[1+STR_VSRCRAW[0]*stick_index+c]);
#endif
    }
  }
  eeDirty(EE_MODEL);
}
#endif

#if defined(TEMPLATES)
inline void applyDefaultTemplate()
{
  applyTemplate(TMPL_SIMPLE_4CH);
}
#else
void applyDefaultTemplate()
{
  for (int i=0; i<NUM_STICKS; i++) {
#if defined(PCBTARANIS)
    uint8_t stick_index = channel_order(i+1);
    ExpoData *expo = expoAddress(i);
    expo->srcRaw = MIXSRC_Rud - 1 + stick_index;
    expo->curve.type = CURVE_REF_EXPO;
    expo->chn = i;
    expo->weight = 100;
    expo->mode = 3; // TODO constant
    for (int c=0; c<4; c++) {
#if defined(TRANSLATIONS_CZ) && defined(CPUARM)
      g_model.inputNames[i][c] = char2idx(STR_INPUTNAMES[1+STR_INPUTNAMES[0]*(stick_index-1)+c]);
#else
      g_model.inputNames[i][c] = char2idx(STR_VSRCRAW[1+STR_VSRCRAW[0]*stick_index+c]);
#endif
    }
#endif

    MixData *mix = mixAddress(i);
    mix->destCh = i;
    mix->weight = 100;

#if defined(PCBTARANIS)
    mix->srcRaw = i+1;
#else
    mix->srcRaw = MIXSRC_Rud - 1 + channel_order(i+1);
#endif
  }
  eeDirty(EE_MODEL);
}
#endif

#if defined(CPUARM)
void checkModelIdUnique(uint8_t id)
{
  for (uint8_t i=0; i<MAX_MODELS; i++) {
    if (i!=id && g_model.header.modelId!=0 && g_model.header.modelId==modelHeaders[i].modelId) {
      POPUP_WARNING(STR_MODELIDUSED);
      break;
    }
  }
}
#endif

#if defined(SDCARD)
bool isFileAvailable(const char * filename)
{
  FILINFO info;
  TCHAR lfn[_MAX_LFN + 1];
  info.lfname = lfn;
  info.lfsize = sizeof(lfn);
  return f_stat(filename, &info) == FR_OK;
}
#endif

void modelDefault(uint8_t id)
{
  memset(&g_model, 0, sizeof(g_model));

  applyDefaultTemplate();

#if defined(LUA)
  if (isFileAvailable(WIZARD_PATH "/" WIZARD_NAME)) {
    f_chdir(WIZARD_PATH);
    luaExec(WIZARD_NAME);
  }
#endif

#if defined(CPUARM) && !defined(PCBTARANIS)
  g_model.externalModule = MODULE_TYPE_PPM;
#endif

#if defined(PXX) && defined(CPUARM)
  modelHeaders[id].modelId = g_model.header.modelId = id+1;
  checkModelIdUnique(id);
#endif

#if defined(CPUARM) && defined(FLIGHT_MODES) && defined(GVARS)
  for (int p=1; p<MAX_FLIGHT_MODES; p++) {
    for (int i=0; i<MAX_GVARS; i++) {
      g_model.flightModeData[p].gvars[i] = GVAR_MAX+1;
    }
  }
#endif

#if defined(PCBTARANIS)
  g_model.frsky.channels[0].ratio = 132;
#endif

#if defined(MAVLINK)
  g_model.mavlink.rc_rssi_scale = 15;
  g_model.mavlink.pc_rssi_en = 1;
#endif
}

#if defined(PCBTARANIS)
bool isInputRecursive(int index)
{
  ExpoData * line = expoAddress(0);
  for (int i=0; i<MAX_EXPOS; i++, line++) {
    if (line->chn > index)
      break;
    else if (line->chn < index)
      continue;
    else if (line->srcRaw >= MIXSRC_FIRST_LOGICAL_SWITCH)
      return true;
  }
  return false;
}
#endif

#if defined(AUTOSOURCE)
int8_t getMovedSource(GET_MOVED_SOURCE_PARAMS)
{
  int8_t result = 0;
  static tmr10ms_t s_move_last_time = 0;

#if defined(PCBTARANIS)
  static int16_t inputsStates[MAX_INPUTS];
  if (min <= MIXSRC_FIRST_INPUT) {
    for (uint8_t i=0; i<MAX_INPUTS; i++) {
      if (abs(anas[i] - inputsStates[i]) > 512) {
        if (!isInputRecursive(i)) {
          result = MIXSRC_FIRST_INPUT+i;
          break;
        }
      }
    }
  }
#endif

  static int16_t sourcesStates[NUM_STICKS+NUM_POTS];
  if (result == 0) {
    for (uint8_t i=0; i<NUM_STICKS+NUM_POTS; i++) {
      if (abs(calibratedStick[i] - sourcesStates[i]) > 512) {
        result = MIXSRC_Rud+i;
        break;
      }
    }
  }

  bool recent = ((tmr10ms_t)(get_tmr10ms() - s_move_last_time) > 10);
  if (recent) {
    result = 0;
  }

  if (result || recent) {
#if defined(PCBTARANIS)
    memcpy(inputsStates, anas, sizeof(inputsStates));
#endif
    memcpy(sourcesStates, calibratedStick, sizeof(sourcesStates));
  }

  s_move_last_time = get_tmr10ms();
  return result;
}
#endif

#if defined(FLIGHT_MODES)
uint8_t getFlightMode()
{
  for (uint8_t i=1; i<MAX_FLIGHT_MODES; i++) {
    FlightModeData *phase = &g_model.flightModeData[i];
    if (phase->swtch && getSwitch(phase->swtch)) {
      return i;
    }
  }
  return 0;
}
#endif

trim_t getRawTrimValue(uint8_t phase, uint8_t idx)
{
  FlightModeData *p = flightModeAddress(phase);
#if defined(PCBSTD)
  return (((trim_t)p->trim[idx]) << 2) + ((p->trim_ext >> (2*idx)) & 0x03);
#else
  return p->trim[idx];
#endif
}

int getTrimValue(uint8_t phase, uint8_t idx)
{
#if defined(PCBTARANIS)
  int result = 0;
  for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
    trim_t v = getRawTrimValue(phase, idx);
    if (v.mode == TRIM_MODE_NONE) {
      return result;
    }
    else {
      unsigned int p = v.mode >> 1;
      if (p == phase || phase == 0) {
        return result + v.value;
      }
      else {
        phase = p;
        if (v.mode % 2 != 0) {
          result += v.value;
        }
      }
    }
  }
  return 0;
#else
  return getRawTrimValue(getTrimFlightPhase(phase, idx), idx);
#endif
}

#if defined(PCBTARANIS)
bool setTrimValue(uint8_t phase, uint8_t idx, int trim)
{
  for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
    trim_t & v = flightModeAddress(phase)->trim[idx];
    if (v.mode == TRIM_MODE_NONE)
      return false;
    unsigned int p = v.mode >> 1;
    if (p == phase || phase == 0) {
      v.value = trim;
      break;
    }
    else if (v.mode % 2 == 0) {
      phase = p;
    }
    else {
      v.value = limit<int>(TRIM_EXTENDED_MIN, trim - getTrimValue(p, idx), TRIM_EXTENDED_MAX);
      break;
    }
  }
  eeDirty(EE_MODEL);
  return true;
}
#else
void setTrimValue(uint8_t phase, uint8_t idx, int trim)
{
#if defined(PCBSTD)
  FlightModeData *p = flightModeAddress(phase);
  p->trim[idx] = (int8_t)(trim >> 2);
  idx <<= 1;
  p->trim_ext = (p->trim_ext & ~(0x03 << idx)) + (((trim & 0x03) << idx));
#else
  FlightModeData *p = flightModeAddress(phase);
  p->trim[idx] = trim;
#endif
  eeDirty(EE_MODEL);
}
#endif

#if !defined(PCBTARANIS)
uint8_t getTrimFlightPhase(uint8_t phase, uint8_t idx)
{
  for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
    if (phase == 0) return 0;
    trim_t trim = getRawTrimValue(phase, idx);
    if (trim <= TRIM_EXTENDED_MAX) return phase;
    uint8_t result = trim-TRIM_EXTENDED_MAX-1;
    if (result >= phase) result++;
    phase = result;
  }
  return 0;
}
#endif

#if defined(ROTARY_ENCODERS)
uint8_t getRotaryEncoderFlightPhase(uint8_t idx)
{
  uint8_t phase = mixerCurrentFlightMode;
  for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
    if (phase == 0) return 0;
    int16_t value = flightModeAddress(phase)->rotaryEncoders[idx];
    if (value <= ROTARY_ENCODER_MAX) return phase;
    uint8_t result = value-ROTARY_ENCODER_MAX-1;
    if (result >= phase) result++;
    phase = result;
  }
  return 0;
}

int16_t getRotaryEncoder(uint8_t idx)
{
  return flightModeAddress(getRotaryEncoderFlightPhase(idx))->rotaryEncoders[idx];
}

void incRotaryEncoder(uint8_t idx, int8_t inc)
{
  g_rotenc[idx] += inc;
  int16_t *value = &(flightModeAddress(getRotaryEncoderFlightPhase(idx))->rotaryEncoders[idx]);
  *value = limit((int16_t)-1024, (int16_t)(*value + (inc * 8)), (int16_t)+1024);
  eeDirty(EE_MODEL);
}
#endif

#if defined(GVARS)

#if defined(PCBSTD)
  #define SET_GVAR_VALUE(idx, phase, value) \
    (GVAR_VALUE(idx, phase) = value, eeDirty(EE_MODEL))
#else
  #define SET_GVAR_VALUE(idx, phase, value) \
    GVAR_VALUE(idx, phase) = value; \
    eeDirty(EE_MODEL); \
    if (g_model.gvars[idx].popup) { \
      s_gvar_last = idx; \
      s_gvar_timer = GVAR_DISPLAY_TIME; \
    }
#endif

#if defined(PCBSTD)
int16_t getGVarValue(int16_t x, int16_t min, int16_t max)
{
  if (GV_IS_GV_VALUE(x, min, max)) {
    int8_t idx = GV_INDEX_CALCULATION(x, max);
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
    SET_GVAR_VALUE(idx, -1, value);
  }
}
#else
uint8_t s_gvar_timer = 0;
uint8_t s_gvar_last = 0;

uint8_t getGVarFlightPhase(uint8_t phase, uint8_t idx)
{
  for (uint8_t i=0; i<MAX_FLIGHT_MODES; i++) {
    if (phase == 0) return 0;
    int16_t val = GVAR_VALUE(idx, phase); // TODO phase at the end everywhere to be consistent!
    if (val <= GVAR_MAX) return phase;
    uint8_t result = val-GVAR_MAX-1;
    if (result >= phase) result++;
    phase = result;
  }
  return 0;
}

int16_t getGVarValue(int16_t x, int16_t min, int16_t max, int8_t phase)
{
  if (GV_IS_GV_VALUE(x, min, max)) {
    int8_t idx = GV_INDEX_CALCULATION(x, max);
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
    SET_GVAR_VALUE(idx, phase, value);
  }
}
#endif

#endif

#if defined(CPUARM) && defined(FRSKY)
ls_telemetry_value_t minTelemValue(uint8_t channel)
{
  switch (channel) {
    case TELEM_TIMER1:
    case TELEM_TIMER2:
      return -3600;
    case TELEM_ALT:
    case TELEM_MIN_ALT:
    case TELEM_MAX_ALT:
    case TELEM_GPSALT:
      return -500;
    case TELEM_T1:
    case TELEM_MAX_T1:
    case TELEM_T2:
    case TELEM_MAX_T2:
      return -30;
    case TELEM_ACCx:
    case TELEM_ACCy:
    case TELEM_ACCz:
      return -1000;
    case TELEM_VSPEED:
      return -3000;
    default:
      return 0;
  }
}
#endif

#if defined(FRSKY)
ls_telemetry_value_t maxTelemValue(uint8_t channel)
{
  switch (channel) {
#if defined(CPUARM)
    case TELEM_TX_TIME:
      return 24*60-1;
    case TELEM_TIMER1:
    case TELEM_TIMER2:
      return 60*60;
#endif
    case TELEM_FUEL:
#if defined(CPUARM)
    case TELEM_SWR:
#endif
    case TELEM_RSSI_TX:
    case TELEM_RSSI_RX:
      return 100;
    case TELEM_HDG:
      return 180;
#if defined(CPUARM)
    case TELEM_SPEED:
    case TELEM_MAX_SPEED:
    case TELEM_ASPEED:
    case TELEM_MAX_ASPEED:
      return 20000;
    case TELEM_CELL:
    case TELEM_MIN_CELL:
      return 510;
    case TELEM_CELLS_SUM:
    case TELEM_MIN_CELLS_SUM:
    case TELEM_VFAS:
    case TELEM_MIN_VFAS:
      return 1000;
    case TELEM_VSPEED:
      return 3000;
    case TELEM_ACCx:
    case TELEM_ACCy:
    case TELEM_ACCz:
      return 1000;
    default:
      return 30000;
#else
    default:
      return 255;
#endif
  }
}
#endif

#if defined(CPUARM)
getvalue_t convert16bitsTelemValue(uint8_t channel, ls_telemetry_value_t value)
{
  getvalue_t result;
  switch (channel) {
#if defined(FRSKY_SPORT)
    case TELEM_ALT:
      result = value * 100;
      break;
#endif
    case TELEM_VSPEED:
      result = value * 10;
      break;

    default:
      result = value;
      break;
  }
  return result;
}

ls_telemetry_value_t max8bitsTelemValue(uint8_t channel)
{
  return min<ls_telemetry_value_t>(255, maxTelemValue(channel));
}
#endif

getvalue_t convert8bitsTelemValue(uint8_t channel, ls_telemetry_value_t value)
{
  getvalue_t result;
  switch (channel) {
    case TELEM_TIMER1:
    case TELEM_TIMER2:
      result = value * 5;
      break;
#if defined(FRSKY)
    case TELEM_ALT:
#if defined(CPUARM)
      result = 100 * (value * 8 - 500);
      break;
#endif
    case TELEM_GPSALT:
    case TELEM_MAX_ALT:
    case TELEM_MIN_ALT:
      result = value * 8 - 500;
      break;
    case TELEM_RPM:
    case TELEM_MAX_RPM:
      result = value * 50;
      break;
    case TELEM_T1:
    case TELEM_T2:
    case TELEM_MAX_T1:
    case TELEM_MAX_T2:
      result = (getvalue_t)value - 30;
      break;
    case TELEM_CELL:
    case TELEM_HDG:
    case TELEM_SPEED:
    case TELEM_MAX_SPEED:
      result = value * 2;
      break;
    case TELEM_ASPEED:
    case TELEM_MAX_ASPEED:
      result = value * 20;
      break;
    case TELEM_DIST:
    case TELEM_MAX_DIST:
      result = value * 8;
      break;
    case TELEM_CURRENT:
    case TELEM_POWER:
    case TELEM_MAX_CURRENT:
    case TELEM_MAX_POWER:
      result = value * 5;
      break;
    case TELEM_CONSUMPTION:
      result = value * 100;
      break;
    case TELEM_VSPEED:
      result = ((getvalue_t)value - 125) * 10;
      break;
#endif
    default:
      result = value;
      break;
  }
  return result;
}

#if defined(FRSKY) || defined(CPUARM)
FORCEINLINE void convertUnit(getvalue_t & val, uint8_t & unit)
{
  if (IS_IMPERIAL_ENABLE()) {
    if (unit == UNIT_TEMPERATURE) {
      val += 18;
      val *= 115;
      val >>= 6;
    }
    if (unit == UNIT_DIST) {
      // m to ft *105/32
      val = val * 3 + (val >> 2) + (val >> 5);
    }
    if (unit == UNIT_FEET) {
      unit = UNIT_DIST;
    }
    if (unit == UNIT_KTS) {
      // kts to mph
      unit = UNIT_SPEED;
      val = (val * 23) / 20;
    }
  }
  else {
    if (unit == UNIT_KTS) {
      // kts to km/h
      unit = UNIT_SPEED;
#if defined(CPUARM)
      val = (val * 1852) / 1000;
#else
      val = (val * 50) / 27;
#endif
    }
  }

  if (unit == UNIT_HDG) {
    unit = UNIT_TEMPERATURE;
  }
}
#endif

#define INAC_STICKS_SHIFT   6
#define INAC_SWITCHES_SHIFT 8
bool inputsMoved()
{
  uint8_t sum = 0;
  for (uint8_t i=0; i<NUM_STICKS; i++)
    sum += anaIn(i) >> INAC_STICKS_SHIFT;
  for (uint8_t i=0; i<NUM_SWITCHES; i++)
    sum += getValue(MIXSRC_FIRST_SWITCH+i) >> INAC_SWITCHES_SHIFT;

  if (abs((int8_t)(sum-inactivity.sum)) > 1) {
    inactivity.sum = sum;
    return true;
  }
  else {
    return false;
  }
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
    if (inputsMoved()) {
      inactivity.counter = 0;
      if (g_eeGeneral.backlightMode & e_backlight_mode_sticks)
        backlightOn();
    }

    bool backlightOn = (g_eeGeneral.backlightMode == e_backlight_mode_on || lightOffCounter || isFunctionActive(FUNCTION_BACKLIGHT));
    if (flashCounter) backlightOn = !backlightOn;
    if (backlightOn)
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

#if MENUS_LOCK == 1
bool readonly = true;
bool readonlyUnlocked()
{
  if (readonly) {
    POPUP_WARNING(STR_MODS_FORBIDDEN);
    return false;
  }
  else {
    return true;
  }
}
#endif

#if defined(SPLASH)

inline void Splash()
{
  lcd_clear();
#if defined(PCBTARANIS)
  lcd_bmp(0, 0, splash_lbm);
#else
  lcd_img(0, 0, splash_lbm, 0, 0);
#endif

#if MENUS_LOCK == 1
  if (readonly == false) {
    lcd_filled_rect((LCD_W-(sizeof(TR_UNLOCKED)-1)*FW)/2 - 9, 50, (sizeof(TR_UNLOCKED)-1)*FW+16, 11, SOLID, ERASE|ROUND);
    lcd_puts((LCD_W-(sizeof(TR_UNLOCKED)-1)*FW)/2 , 53, STR_UNLOCKED);
  }
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

    inputsMoved();

    tmr10ms_t tgtime = get_tmr10ms() + SPLASH_TIMEOUT;
    while (tgtime != get_tmr10ms()) {
#if defined(SIMU)
      SIMU_SLEEP(1);
#elif defined(CPUARM)
      CoTickDelay(1);
#endif

      getADC();

#if defined(FSPLASH)
      // Splash is forced, we can't skip it
      if (!(g_eeGeneral.splashMode & 0x04)) {
#endif

      if (keyDown() || inputsMoved()) return;

#if defined(FSPLASH)
      }
#endif

      if (pwrCheck()==e_power_off) return;

#if !defined(PCBTARANIS) && !defined(PCBSTD)
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
#define Splash()
#define doSplash()
#endif

void checkAll()
{
#if !defined(PCBSKY9X)
  checkLowEEPROM();
#endif

#if defined(MODULE_ALWAYS_SEND_PULSES)
  startupWarningState = STARTUP_WARNING_THROTTLE;
#else
  checkTHR();
  checkSwitches();
#endif

#if defined(CPUARM)
  if (g_model.displayChecklist && modelHasNotes()) {
    pushModelNotes();
  }
#endif

  clearKeyEvents();

  START_SILENCE_PERIOD();
}

#if defined(MODULE_ALWAYS_SEND_PULSES)
void checkStartupWarnings()
{
  if (startupWarningState < STARTUP_WARNING_DONE) {
    if (startupWarningState == STARTUP_WARNING_THROTTLE)
      checkTHR();
    else
      checkSwitches();
  }
}
#endif

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
  uint8_t thrchn = ((g_model.thrTraceSrc==0) || (g_model.thrTraceSrc>NUM_POTS)) ? THR_STICK : g_model.thrTraceSrc+NUM_STICKS-1;
  // throttle channel is either the stick according stick mode (already handled in evalInputs)
  // or P1 to P3;
  // in case an output channel is choosen as throttle source (thrTraceSrc>NUM_POTS) we assume the throttle stick is the input
  // no other information available at the moment, and good enough to my option (otherwise too much exceptions...)

#if defined(MODULE_ALWAYS_SEND_PULSES)
  int16_t v = calibratedStick[thrchn];
  if (v<=THRCHK_DEADBAND-1024 || g_model.disableThrottleWarning || pwrCheck()==e_power_off || keyDown()) {
    startupWarningState = STARTUP_WARNING_THROTTLE+1;
  }
  else {
    calibratedStick[thrchn] = -1024;
#if !defined(PCBTARANIS)
    if (thrchn < NUM_STICKS) {
      rawAnas[thrchn] = anas[thrchn] = calibratedStick[thrchn];
    }
#endif
    MESSAGE(STR_THROTTLEWARN, STR_THROTTLENOTIDLE, STR_PRESSANYKEYTOSKIP, AU_THROTTLE_ALERT);
  }
#else
  if (g_model.disableThrottleWarning) return;
  getADC();
  evalInputs(e_perout_mode_notrainer); // let do evalInputs do the job

  int16_t v = calibratedStick[thrchn];
  if (v<=(THRCHK_DEADBAND-1024)) return;  // prevent warning if throttle input OK

  // first - display warning; also deletes inputs if any have been before
  MESSAGE(STR_THROTTLEWARN, STR_THROTTLENOTIDLE, STR_PRESSANYKEYTOSKIP, AU_THROTTLE_ALERT);

  while (1) {

    SIMU_SLEEP(1);

    getADC();

    evalInputs(e_perout_mode_notrainer); // let do evalInputs do the job
    v = calibratedStick[thrchn];

    if (pwrCheck()==e_power_off || keyDown() || v<=(THRCHK_DEADBAND-1024))
      break;

    checkBacklight();

    wdt_reset();
  }
#endif
}

void checkAlarm() // added by Gohst
{
  if (g_eeGeneral.disableAlarmWarning)
    return;

  if (IS_SOUND_OFF())
    ALERT(STR_ALARMSWARN, STR_ALARMSDISABLED, AU_ERROR);
}

void alert(const pm_char * t, const pm_char *s MESSAGE_SOUND_ARG)
{
  MESSAGE(t, s, STR_PRESSANYKEY, sound);

  while(1)
  {
    SIMU_SLEEP(1);

    if (pwrCheck() == e_power_off) {
      // the radio has been powered off during the ALERT
      pwrOff(); // turn power off now
    }

    if (keyDown()) return;  // wait for key release

    checkBacklight();

    wdt_reset();
  }
}

void message(const pm_char *title, const pm_char *t, const char *last MESSAGE_SOUND_ARG)
{
  lcd_clear();

#if LCD_W >= 212
  lcd_bmp(0, 0, asterisk_lbm);
  #define TITLE_LCD_OFFSET   60
  #define MESSAGE_LCD_OFFSET 60
#else
  lcd_img(2, 0, asterisk_lbm, 0, 0);
  #define TITLE_LCD_OFFSET   6*FW
  #define MESSAGE_LCD_OFFSET 0
#endif

#if defined(TRANSLATIONS_FR) || defined(TRANSLATIONS_IT) || defined(TRANSLATIONS_CZ)
  lcd_putsAtt(TITLE_LCD_OFFSET, 0, STR_WARNING, DBLSIZE);
  lcd_putsAtt(TITLE_LCD_OFFSET, 2*FH, title, DBLSIZE);
#else
  lcd_putsAtt(TITLE_LCD_OFFSET, 0, title, DBLSIZE);
  lcd_putsAtt(TITLE_LCD_OFFSET, 2*FH, STR_WARNING, DBLSIZE);
#endif

#if LCD_W >= 212
  lcd_filled_rect(60, 0, LCD_W-MESSAGE_LCD_OFFSET, 32);
  if (t) lcd_puts(MESSAGE_LCD_OFFSET, 5*FH, t);
  if (last) {
    lcd_puts(MESSAGE_LCD_OFFSET, 7*FH, last);
    AUDIO_ERROR_MESSAGE(sound);
  }
#else
  lcd_filled_rect(0, 0, LCD_W-MESSAGE_LCD_OFFSET, 32);
  if (t) lcd_putsLeft(5*FH, t);
  if (last) {
    lcd_putsLeft(7*FH, last);
    AUDIO_ERROR_MESSAGE(sound);
  }
#endif

  lcdRefresh();
  lcdSetContrast();
  clearKeyEvents();
}

#if defined(GVARS)
  int8_t trimGvar[NUM_STICKS] = { -1, -1, -1, -1 };
  #define TRIM_REUSED(idx) trimGvar[idx] >= 0
#else
  #define TRIM_REUSED(idx) 0
#endif

#if defined(CPUARM)
void checkTrims()
{
  uint8_t event = getEvent(true);
  if (event && !IS_KEY_BREAK(event)) {
    int8_t k = EVT_KEY_MASK(event) - TRM_BASE;
#else
uint8_t checkTrim(uint8_t event)
{
  int8_t k = EVT_KEY_MASK(event) - TRM_BASE;
  if (k>=0 && k<8 && !IS_KEY_BREAK(event)) {
#endif
    // LH_DWN LH_UP LV_DWN LV_UP RV_DWN RV_UP RH_DWN RH_UP
    uint8_t idx = CONVERT_MODE((uint8_t)k/2);
    uint8_t phase;
    int before;
    bool thro;

#if defined(GVARS)
    if (TRIM_REUSED(idx)) {
#if defined(PCBSTD)
      phase = 0;
#else
      phase = getGVarFlightPhase(mixerCurrentFlightMode, trimGvar[idx]);
#endif
      before = GVAR_VALUE(trimGvar[idx], phase);
      thro = false;
    }
    else {
      phase = getTrimFlightPhase(mixerCurrentFlightMode, idx);
#if defined(PCBTARANIS)
      before = getTrimValue(phase, idx);
#else
      before = getRawTrimValue(phase, idx);
#endif
      thro = (idx==THR_STICK && g_model.thrTrim);
    }
#else
    phase = getTrimFlightPhase(mixerCurrentFlightMode, idx);
#if defined(PCBTARANIS)
    before = getTrimValue(phase, idx);
#else
    before = getRawTrimValue(phase, idx);
#endif
    thro = (idx==THR_STICK && g_model.thrTrim);
#endif
    int8_t trimInc = g_model.trimInc + 1;
    int8_t v = (trimInc==-1) ? min(32, abs(before)/4+1) : (1 << trimInc); // TODO flash saving if (trimInc < 0)
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
      if (!g_model.extendedTrims || TRIM_REUSED(idx)) after = before;
    }

    if (after < TRIM_EXTENDED_MIN) {
      after = TRIM_EXTENDED_MIN;
    }
    if (after > TRIM_EXTENDED_MAX) {
      after = TRIM_EXTENDED_MAX;
    }

#if defined(GVARS)
    if (TRIM_REUSED(idx)) {
      SET_GVAR_VALUE(trimGvar[idx], phase, after);
    }
    else
#endif
    {
#if defined(PCBTARANIS)
      if (!setTrimValue(phase, idx, after)) {
        // we don't play a beep, so we exit now the function
        return;
      }
#else
      setTrimValue(phase, idx, after);
#endif
    }

#if defined(AUDIO)
    // toneFreq higher/lower according to trim position
    // limit the frequency, range -125 to 125 = toneFreq: 19 to 101
    if (after > TRIM_MAX)
      after = TRIM_MAX;
    if (after < TRIM_MIN)
      after = TRIM_MIN;
#if defined(CPUARM)
    after <<= 3;
    after += 120*16;
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

#if !defined(SIMU)
static uint16_t s_anaFilt[NUMBER_ANALOG];
#endif

#if defined(SIMU)
uint16_t BandGap = 225;
#elif defined(CPUM2560)
// #define STARTADCONV (ADCSRA  = (1<<ADEN) | (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2) | (1<<ADSC) | (1 << ADIE))
// G: Note that the above would have set the ADC prescaler to 128, equating to
// 125KHz sample rate. We now sample at 500KHz, with oversampling and other
// filtering options to produce 11-bit results.
uint16_t BandGap = 2040 ;
#elif defined(PCBSTD)
uint16_t BandGap ;
#endif

#if !defined(SIMU)
uint16_t anaIn(uint8_t chan)
{
#if defined(PCBTARANIS)
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
#if defined(FRSKY_STICKS)
  volatile uint16_t temp = s_anaFilt[pgm_read_byte(crossAna+chan)];  // volatile saves here 40 bytes; maybe removed for newer AVR when available
  if (chan < NUM_STICKS && (g_eeGeneral.stickReverse & (1 << chan))) {
    temp = 2048 - temp;
  }
  return temp;
#else
  volatile uint16_t *p = &s_anaFilt[pgm_read_byte(crossAna+chan)];
  return *p;  
#endif
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
#if defined(PCBTARANIS)
    if (calibrationState) break;
#endif
  }

  for (uint32_t x=0; x<NUMBER_ANALOG; x++) {
    uint16_t v = temp[x] >> 3;
#if defined(PCBTARANIS)
    if (calibrationState) v = temp[x] >> 1;
    StepsCalibData * calib = (StepsCalibData *) &g_eeGeneral.calib[x];
    if (!calibrationState && IS_POT_MULTIPOS(x) && calib->count>0 && calib->count<XPOTS_MULTIPOS_COUNT) {
      uint8_t vShifted = (v >> 4);
      s_anaFilt[x] = 2*RESX;
      for (int i=0; i<calib->count; i++) {
        if (vShifted < calib->steps[i]) {
          s_anaFilt[x] = i*2*RESX/calib->count;
          break;
        }
      }
    }
    else
#endif
    s_anaFilt[x] = v;
  }
}
#else

/**
 * Read ADC using 10 bits
 */
inline uint16_t read_adc10(uint8_t adc_input) 
{
  uint16_t temp_ana;
  ADMUX = adc_input|ADC_VREF_TYPE;
#if defined(TELEMETRY_MOD_14051)
  ADCSRA &= 0x87;
#endif
  ADCSRA |= 1 << ADSC; // Start the AD conversion
  while (ADCSRA & (1 << ADSC)); // Wait for the AD conversion to complete
  temp_ana = ADC;
  ADCSRA |= 1 << ADSC; // Start the second AD conversion
  while (ADCSRA & (1 << ADSC)); // Wait for the AD conversion to complete
  temp_ana += ADC;
  return temp_ana;
}

#if defined(TELEMETRY_MOD_14051)
enum MuxInput {
  MUX_BATT,
  MUX_THR,
  MUX_AIL,
  MUX_MAX = MUX_AIL
};

uint8_t pf7_digital[2];
/**
 * Update ADC PF7 using 14051 multiplexer
 * X0 : Battery voltage
 * X1 : THR SW
 * X2 : AIL SW
 */
void readMultiplexAna()
{
  static uint8_t muxNum = MUX_BATT;
  uint16_t temp_ana;
  uint8_t nextMuxNum = muxNum-1;

  DDRC |= 0xC1;
  temp_ana = read_adc10(7);

  switch (muxNum) {
    case MUX_BATT:
      s_anaFilt[TX_VOLTAGE] = temp_ana;
      nextMuxNum = MUX_MAX;
      break;
    case MUX_THR:
    case MUX_AIL:
      // Digital switch depend from input voltage
      // take half voltage to determine digital state
      pf7_digital[muxNum-1] = (temp_ana >= (s_anaFilt[TX_VOLTAGE] / 2)) ? 1 : 0;
      break;
  }

  // set the mux number for the next ADC convert,
  // stabilize voltage before ADC read.
  muxNum = nextMuxNum;
  PORTC &= ~((1 << PC7) | (1 << PC6) | (1 << PC0)); // Clear CTRL ABC
  switch (muxNum) {
    case 1:
      PORTC |= (1 << PC6); // Mux CTRL A : SW_THR
      break;
    case 2:
      PORTC |= (1 << PC7); // Mux CTRL B : SW_AIL
      break;
  }
}
#endif

void getADC()
{
#if defined(TELEMETRY_MOD_14051)
  readMultiplexAna();
  #define ADC_READ_COUNT 7
#else
  #define ADC_READ_COUNT 8
#endif
  
  for (uint8_t adc_input=0; adc_input<ADC_READ_COUNT; adc_input++) {
    s_anaFilt[adc_input] = read_adc10(adc_input);
  }
}
#endif

#if !defined(CPUARM)
void getADC_bandgap()
{
#if defined(CPUM2560)
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
uint8_t flashCounter = 0;

uint16_t sessionTimer;
uint16_t s_timeCumThr;    // THR in 1/16 sec
uint16_t s_timeCum16ThrP; // THR% in 1/16 sec

uint8_t  trimsCheckTimer = 0;

void timerReset(uint8_t idx)
{
  TimerState & timerState = timersStates[idx];
  timerState.state = TMR_OFF; // is changed to RUNNING dep from mode
  timerState.val = g_model.timers[idx].start;
  timerState.val_10ms = 0 ;
}

void flightReset()
{
  // we don't reset the whole audio here (the tada.wav would be cut, if a prompt is queued before FlightReset, it should be played)
  // TODO check if the vario / background music are stopped correctly if switching to a model which doesn't have these functions enabled

  if (!IS_MANUAL_RESET_TIMER(0)) {
    timerReset(0);
  }

  if (!IS_MANUAL_RESET_TIMER(1)) {
    timerReset(1);
  }

#if defined(FRSKY)
  telemetryReset();
#endif

  s_mixer_first_run_done = false;

  START_SILENCE_PERIOD();

  RESET_THR_TRACE();
}

TimerState timersStates[MAX_TIMERS] = { { 0 }, { 0 } };

#if defined(THRTRACE)
uint8_t  s_traceBuf[MAXTRACE];
uint8_t  s_traceWr;
int      s_traceCnt;
uint8_t  s_cnt_10s;
uint16_t s_cnt_samples_thr_10s;
uint16_t s_sum_samples_thr_10s;
#endif

FORCEINLINE void evalTrims()
{
  uint8_t phase = mixerCurrentFlightMode;
  for (uint8_t i=0; i<NUM_STICKS; i++) {
    // do trim -> throttle trim if applicable
    int16_t trim = getTrimValue(phase, i);
#if !defined(PCBTARANIS)
    if (i==THR_STICK && g_model.thrTrim) {
      int16_t trimMin = g_model.extendedTrims ? TRIM_EXTENDED_MIN : TRIM_MIN;
      trim = (((g_model.throttleReversed)?(int32_t)(trim+trimMin):(int32_t)(trim-trimMin)) * (RESX-anas[i])) >> (RESX_SHIFT+1);
    }
#endif
    if (trimsCheckTimer > 0) {
      trim = 0;
    }

    trims[i] = trim*2;
  }
}

#if defined(DEBUG)
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
MASK_CFN_TYPE  activeFnSwitches = 0;
tmr10ms_t lastFunctionTime[NUM_CFN] = { 0 };

#if defined(VOICE)
PLAY_FUNCTION(playValue, uint8_t idx)
{
  if (IS_FAI_FORBIDDEN(idx))
    return;

  getvalue_t val = getValue(idx);

  switch (idx) {
#if defined(CPUARM)
    case MIXSRC_FIRST_TELEM+TELEM_TX_TIME-1:
      PLAY_DURATION(val*60, PLAY_TIME);
      break;
#endif
    case MIXSRC_FIRST_TELEM+TELEM_TX_VOLTAGE-1:
      PLAY_NUMBER(val, 1+UNIT_VOLTS, PREC1);
      break;
    case MIXSRC_FIRST_TELEM+TELEM_TIMER1-1:
    case MIXSRC_FIRST_TELEM+TELEM_TIMER2-1:
      PLAY_DURATION(val, 0);
      break;
#if defined(CPUARM) && defined(FRSKY)
    case MIXSRC_FIRST_TELEM+TELEM_SWR-1:
      PLAY_NUMBER(val, 0, 0);
      break;
#endif
#if defined(FRSKY)
    case MIXSRC_FIRST_TELEM+TELEM_RSSI_TX-1:
    case MIXSRC_FIRST_TELEM+TELEM_RSSI_RX-1:
      PLAY_NUMBER(val, 1+UNIT_DBM, 0);
      break;
    case MIXSRC_FIRST_TELEM+TELEM_MIN_A1-1:
    case MIXSRC_FIRST_TELEM+TELEM_MIN_A2-1:
#if defined(CPUARM)
    case MIXSRC_FIRST_TELEM+TELEM_MIN_A3-1:
    case MIXSRC_FIRST_TELEM+TELEM_MIN_A4-1:
#endif
      idx -= TELEM_MIN_A1-TELEM_A1;
      // no break
    case MIXSRC_FIRST_TELEM+TELEM_A1-1:
    case MIXSRC_FIRST_TELEM+TELEM_A2-1:
#if defined(CPUARM)
    case MIXSRC_FIRST_TELEM+TELEM_A3-1:
    case MIXSRC_FIRST_TELEM+TELEM_A4-1:
#endif
      if (TELEMETRY_STREAMING()) {
        idx -= (MIXSRC_FIRST_TELEM+TELEM_A1-1);
        uint8_t att = 0;
        int16_t converted_value =  div10_and_round(applyChannelRatio(idx, val));;
        if (ANA_CHANNEL_UNIT(idx) < UNIT_RAW) {
          att = PREC1;
        }
        PLAY_NUMBER(converted_value, 1+ANA_CHANNEL_UNIT(idx), att);
      }
      break;
    case MIXSRC_FIRST_TELEM+TELEM_CELL-1:
    case MIXSRC_FIRST_TELEM+TELEM_MIN_CELL-1:
      PLAY_NUMBER(div10_and_round(val), 1+UNIT_VOLTS, PREC1);
      break;

    case MIXSRC_FIRST_TELEM+TELEM_VFAS-1:
    case MIXSRC_FIRST_TELEM+TELEM_CELLS_SUM-1:
    case MIXSRC_FIRST_TELEM+TELEM_MIN_CELLS_SUM-1:
    case MIXSRC_FIRST_TELEM+TELEM_MIN_VFAS-1:
      PLAY_NUMBER(val, 1+UNIT_VOLTS, PREC1);
      break;

    case MIXSRC_FIRST_TELEM+TELEM_CURRENT-1:
    case MIXSRC_FIRST_TELEM+TELEM_MAX_CURRENT-1:
      PLAY_NUMBER(val, 1+UNIT_AMPS, PREC1);
      break;

    case MIXSRC_FIRST_TELEM+TELEM_ACCx-1:
    case MIXSRC_FIRST_TELEM+TELEM_ACCy-1:
    case MIXSRC_FIRST_TELEM+TELEM_ACCz-1:
      PLAY_NUMBER(div10_and_round(val), 1+UNIT_G, PREC1);
      break;

    case MIXSRC_FIRST_TELEM+TELEM_VSPEED-1:
      PLAY_NUMBER(div10_and_round(val), 1+UNIT_METERS_PER_SECOND, PREC1);
      break;

    case MIXSRC_FIRST_TELEM+TELEM_ASPEED-1:
    case MIXSRC_FIRST_TELEM+TELEM_MAX_ASPEED-1:
      PLAY_NUMBER(val/10, 1+UNIT_KTS, 0);
      break;

    case MIXSRC_FIRST_TELEM+TELEM_CONSUMPTION-1:
      PLAY_NUMBER(val, 1+UNIT_MAH, 0);
      break;

    case MIXSRC_FIRST_TELEM+TELEM_POWER-1:
      PLAY_NUMBER(val, 1+UNIT_WATTS, 0);
      break;

    case MIXSRC_FIRST_TELEM+TELEM_ALT-1:
#if defined(PCBTARANIS)
      PLAY_NUMBER(div10_and_round(val), 1+UNIT_DIST, PREC1);
      break;
#endif
    case MIXSRC_FIRST_TELEM+TELEM_MIN_ALT-1:
    case MIXSRC_FIRST_TELEM+TELEM_MAX_ALT-1:
#if defined(WS_HOW_HIGH)
      if (IS_IMPERIAL_ENABLE() && IS_USR_PROTO_WS_HOW_HIGH())
        PLAY_NUMBER(val, 1+UNIT_FEET, 0);
      else
#endif
        PLAY_NUMBER(val, 1+UNIT_DIST, 0);
      break;

    case MIXSRC_FIRST_TELEM+TELEM_RPM-1:
    case MIXSRC_FIRST_TELEM+TELEM_MAX_RPM-1:
    {
      getvalue_t rpm = val;
      if (rpm > 100)
        rpm = 10 * div10_and_round(rpm);
      if (rpm > 1000)
        rpm = 10 * div10_and_round(rpm);
      PLAY_NUMBER(rpm, 1+UNIT_RPMS, 0);
      break;
    }

    case MIXSRC_FIRST_TELEM+TELEM_HDG-1:
      PLAY_NUMBER(val, 1+UNIT_HDG, 0);
      break;

    default:
    {
      uint8_t unit = 1;
      if (idx < MIXSRC_GVAR1)
        val = calcRESXto100(val);
      if (idx >= MIXSRC_FIRST_TELEM+TELEM_ALT-1 && idx <= MIXSRC_FIRST_TELEM+TELEM_GPSALT-1)
        unit = idx - (MIXSRC_FIRST_TELEM+TELEM_ALT-1);
      else if (idx >= MIXSRC_FIRST_TELEM+TELEM_MAX_T1-1 && idx <= MIXSRC_FIRST_TELEM+TELEM_MAX_DIST-1)
        unit = 3 + idx - (MIXSRC_FIRST_TELEM+TELEM_MAX_T1-1);

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
#define VOLUME_HYSTERESIS 10		// how much must a input value change to actually be considered for new volume setting
uint8_t currentSpeakerVolume = 255;
uint8_t requiredSpeakerVolume;
getvalue_t requiredSpeakerVolumeRawLast = 1024 + 1; //initial value must be outside normal range
uint8_t fnSwitchDuration[NUM_CFN] = { 0 };

inline void playCustomFunctionFile(CustomFnData *sd, uint8_t id)
{
  if (sd->play.name[0] != '\0') {
    char filename[sizeof(SOUNDS_PATH)+sizeof(sd->play.name)+sizeof(SOUNDS_EXT)] = SOUNDS_PATH "/";
    strncpy(filename+SOUNDS_PATH_LNG_OFS, currentLanguagePack->id, 2);
    strncpy(filename+sizeof(SOUNDS_PATH), sd->play.name, sizeof(sd->play.name));
    filename[sizeof(SOUNDS_PATH)+sizeof(sd->play.name)] = '\0';
    strcat(filename+sizeof(SOUNDS_PATH), SOUNDS_EXT);
    PLAY_FILE(filename, sd->func==FUNC_BACKGND_MUSIC ? PLAY_BACKGROUND : 0, id);
  }
}
#endif

void evalFunctions()
{
  MASK_FUNC_TYPE newActiveFunctions  = 0;
  MASK_CFN_TYPE  newActiveFnSwitches = 0;

#if defined(ROTARY_ENCODERS) && defined(GVARS)
  static rotenc_t rePreviousValues[ROTARY_ENCODERS];
#endif

#if defined(OVERRIDE_CHANNEL_FUNCTION)
  for (uint8_t i=0; i<NUM_CHNOUT; i++) {
    safetyCh[i] = OVERRIDE_CHANNEL_UNDEFINED;
  }
#endif

#if defined(GVARS)
  for (uint8_t i=0; i<NUM_STICKS; i++) {
    trimGvar[i] = -1;
  }
#endif

  for (uint8_t i=0; i<NUM_CFN; i++) {
    CustomFnData *sd = &g_model.funcSw[i];
    int8_t swtch = CFN_SWITCH(sd);
    if (swtch) {
      MASK_CFN_TYPE  switch_mask = ((MASK_CFN_TYPE)1 << i);

#if defined(CPUARM)
      bool active = getSwitch(swtch, IS_PLAY_FUNC(CFN_FUNC(sd)) ? GETSWITCH_MIDPOS_DELAY : 0);
#else
      bool active = getSwitch(swtch);
#endif


      if (HAS_ENABLE_PARAM(CFN_FUNC(sd))) {
        active &= (bool)CFN_ACTIVE(sd);
      }

      if (active || IS_PLAY_BOTH_FUNC(CFN_FUNC(sd))) {

        switch (CFN_FUNC(sd)) {

#if defined(OVERRIDE_CHANNEL_FUNCTION)
          case FUNC_OVERRIDE_CHANNEL:
            safetyCh[CFN_CH_INDEX(sd)] = CFN_PARAM(sd);
            break;
#endif

          case FUNC_TRAINER:
          {
            uint8_t mask = 0x0f;
            if (CFN_CH_INDEX(sd) > 0) {
              mask = (1<<(CFN_CH_INDEX(sd)-1));
            }
            newActiveFunctions |= mask;
            break;
          }

          case FUNC_INSTANT_TRIM:
            newActiveFunctions |= (1 << FUNCTION_INSTANT_TRIM);
            if (!isFunctionActive(FUNCTION_INSTANT_TRIM)) {
              if (g_menuStack[0] == menuMainView
#if defined(FRSKY)
                || g_menuStack[0] == menuTelemetryFrsky
#endif
#if defined(PCBTARANIS)
                || g_menuStack[0] == menuMainViewChannelsMonitor
                || g_menuStack[0] == menuChannelsView
#endif
              ) {
                instantTrim();
              }
            }
            break;

          case FUNC_RESET:
            switch (CFN_PARAM(sd)) {
              case FUNC_RESET_TIMER1:
              case FUNC_RESET_TIMER2:
                timerReset(CFN_PARAM(sd));
                break;
              case FUNC_RESET_FLIGHT:
                flightReset();
                break;
#if defined(FRSKY)
              case FUNC_RESET_TELEMETRY:
                telemetryReset();
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
            break;

#if defined(CPUARM)
          case FUNC_SET_TIMER:
          {
            TimerState & timerState = timersStates[CFN_TIMER_INDEX(sd)];
            timerState.state = TMR_OFF; // is changed to RUNNING dep from mode
            timerState.val = CFN_PARAM(sd);
            timerState.val_10ms = 0 ;
            break;
          }
#endif

#if defined(GVARS)
          case FUNC_ADJUST_GVAR:
            if (CFN_GVAR_MODE(sd) == 0) {
              SET_GVAR(CFN_GVAR_INDEX(sd), CFN_PARAM(sd), mixerCurrentFlightMode);
            }
            else if (CFN_GVAR_MODE(sd) == 2) {
              SET_GVAR(CFN_GVAR_INDEX(sd), GVAR_VALUE(CFN_PARAM(sd), mixerCurrentFlightMode), mixerCurrentFlightMode);
            }
            else if (CFN_GVAR_MODE(sd) == 3) {
              if (!(activeFnSwitches & switch_mask)) {
                SET_GVAR(CFN_GVAR_INDEX(sd), GVAR_VALUE(CFN_GVAR_INDEX(sd), getGVarFlightPhase(mixerCurrentFlightMode, CFN_GVAR_INDEX(sd))) + (CFN_PARAM(sd) ? +1 : -1), mixerCurrentFlightMode);
              }
            }
            else if (CFN_PARAM(sd) >= MIXSRC_TrimRud && CFN_PARAM(sd) <= MIXSRC_TrimAil) {
              trimGvar[CFN_PARAM(sd)-MIXSRC_TrimRud] = CFN_GVAR_INDEX(sd);
            }
#if defined(ROTARY_ENCODERS)
            else if (CFN_PARAM(sd) >= MIXSRC_REa && CFN_PARAM(sd) < MIXSRC_TrimRud) {
              int8_t scroll = rePreviousValues[CFN_PARAM(sd)-MIXSRC_REa] - (g_rotenc[CFN_PARAM(sd)-MIXSRC_REa] / ROTARY_ENCODER_GRANULARITY);
              if (scroll) {
                SET_GVAR(CFN_GVAR_INDEX(sd), GVAR_VALUE(CFN_GVAR_INDEX(sd), getGVarFlightPhase(mixerCurrentFlightMode, CFN_GVAR_INDEX(sd))) + scroll, mixerCurrentFlightMode);
              }
            }
#endif
            else {
              SET_GVAR(CFN_GVAR_INDEX(sd), calcRESXto100(getValue(CFN_PARAM(sd))), mixerCurrentFlightMode);
            }
            break;
#endif

#if defined(CPUARM) && defined(SDCARD)
          case FUNC_VOLUME:
          {
            getvalue_t raw = getValue(CFN_PARAM(sd));
            //only set volume if input changed more than hysteresis
            if (abs(requiredSpeakerVolumeRawLast - raw) > VOLUME_HYSTERESIS) {
              requiredSpeakerVolumeRawLast = raw;
            }
            requiredSpeakerVolume = ((1024 + requiredSpeakerVolumeRawLast) * VOLUME_LEVEL_MAX) / 2048;
            break;
          }
#endif

#if defined(CPUARM) && defined(SDCARD)
          case FUNC_PLAY_SOUND:
          case FUNC_PLAY_TRACK:
          case FUNC_PLAY_VALUE:
#if defined(HAPTIC)
          case FUNC_HAPTIC:
#endif
          {
            tmr10ms_t tmr10ms = get_tmr10ms();
            uint8_t repeatParam = CFN_PLAY_REPEAT(sd);
            if (!IS_SILENCE_PERIOD_ELAPSED() && repeatParam == CFN_PLAY_REPEAT_NOSTART)
              lastFunctionTime[i] = tmr10ms;
            if (!lastFunctionTime[i] || (repeatParam && repeatParam!=CFN_PLAY_REPEAT_NOSTART && (signed)(tmr10ms-lastFunctionTime[i])>=100*repeatParam)) {
              if (!IS_PLAYING(i+1)) {
                lastFunctionTime[i] = tmr10ms;
                if (CFN_FUNC(sd) == FUNC_PLAY_SOUND) {
                  AUDIO_PLAY(AU_FRSKY_FIRST+CFN_PARAM(sd));
                }
                else if (CFN_FUNC(sd) == FUNC_PLAY_VALUE) {
                  PLAY_VALUE(CFN_PARAM(sd), i+1);
                }
#if defined(HAPTIC)
                else if (CFN_FUNC(sd) == FUNC_HAPTIC) {
                  haptic.event(AU_FRSKY_LAST+CFN_PARAM(sd));
                }
#endif
                else {
                  playCustomFunctionFile(sd, i+1);
                }
              }
            }
            break;
          }

          case FUNC_BACKGND_MUSIC:
            newActiveFunctions |= (1 << FUNCTION_BACKGND_MUSIC);
            if (!IS_PLAYING(i+1)) {
              playCustomFunctionFile(sd, i+1);
            }
            break;

          case FUNC_BACKGND_MUSIC_PAUSE:
            newActiveFunctions |= (1 << FUNCTION_BACKGND_MUSIC_PAUSE);
            break;

#elif defined(VOICE)
          case FUNC_PLAY_SOUND:
          case FUNC_PLAY_TRACK:
          case FUNC_PLAY_BOTH:
          case FUNC_PLAY_VALUE:
          {
            tmr10ms_t tmr10ms = get_tmr10ms();
            uint8_t repeatParam = CFN_PLAY_REPEAT(sd);
            if (!lastFunctionTime[i] || (CFN_FUNC(sd)==FUNC_PLAY_BOTH && active!=(bool)(activeFnSwitches&switch_mask)) || (repeatParam && (signed)(tmr10ms-lastFunctionTime[i])>=1000*repeatParam)) {
              lastFunctionTime[i] = tmr10ms;
              uint8_t param = CFN_PARAM(sd);
              if (CFN_FUNC(sd) == FUNC_PLAY_SOUND) {
                AUDIO_PLAY(AU_FRSKY_FIRST+param);
              }
              else if (CFN_FUNC(sd) == FUNC_PLAY_VALUE) {
                PLAY_VALUE(param, i+1);
              }
              else {
#if defined(GVARS)
                if (CFN_FUNC(sd) == FUNC_PLAY_TRACK && param > 250)
                  param = GVAR_VALUE(param-251, getGVarFlightPhase(mixerCurrentFlightMode, param-251));
#endif
                PUSH_CUSTOM_PROMPT(active ? param : param+1, i+1);
              }
            }
            if (!active) {
              // PLAY_BOTH would change activeFnSwitches otherwise
              switch_mask = 0;
            }
            break;
          }
#else
          case FUNC_PLAY_SOUND:
          {
            tmr10ms_t tmr10ms = get_tmr10ms();
            uint8_t repeatParam = CFN_PLAY_REPEAT(sd);
            if (!lastFunctionTime[i] || (repeatParam && (signed)(tmr10ms-lastFunctionTime[i])>=1000*repeatParam)) {
              lastFunctionTime[i] = tmr10ms;
              AUDIO_PLAY(AU_FRSKY_FIRST+CFN_PARAM(sd));
            }
            break;
          }
#endif

#if defined(FRSKY) && defined(VARIO)
          case FUNC_VARIO:
            newActiveFunctions |= (1 << FUNCTION_VARIO);
            break;
#endif

#if defined(HAPTIC) && !defined(CPUARM)
          case FUNC_HAPTIC:
          {
            tmr10ms_t tmr10ms = get_tmr10ms();
            uint8_t repeatParam = CFN_PLAY_REPEAT(sd);
            if (!lastFunctionTime[i] || (repeatParam && (signed)(tmr10ms-lastFunctionTime[i])>=1000*repeatParam)) {
              lastFunctionTime[i] = tmr10ms;
              haptic.event(AU_FRSKY_LAST+CFN_PARAM(sd));
            }
            break;
          }
#endif

#if defined(SDCARD)
          case FUNC_LOGS:
            if (CFN_PARAM(sd)) {
              newActiveFunctions |= (1 << FUNCTION_LOGS);
              logDelay = CFN_PARAM(sd);
            }
            break;
#endif

          case FUNC_BACKLIGHT:
            newActiveFunctions |= (1 << FUNCTION_BACKLIGHT);
            break;

#if defined(DEBUG)
          case FUNC_TEST:
            testFunc();
            break;
#endif
        }

        newActiveFnSwitches |= switch_mask;
      }
      else {
        lastFunctionTime[i] = 0;
#if defined(CPUARM)
        fnSwitchDuration[i] = 0;
#endif
      }
    }
  }

  activeFnSwitches = newActiveFnSwitches;
  activeFunctions  = newActiveFunctions;

#if defined(ROTARY_ENCODERS) && defined(GVARS)
  for (uint8_t i=0; i<ROTARY_ENCODERS; i++) {
    rePreviousValues[i] = (g_rotenc[i] / ROTARY_ENCODER_GRANULARITY);
  }
#endif
}

uint8_t s_mixer_first_run_done = false;

void doMixerCalculations()
{
  static tmr10ms_t lastTMR = 0;

  tmr10ms_t tmr10ms = get_tmr10ms();
  uint8_t tick10ms = (tmr10ms >= lastTMR ? tmr10ms - lastTMR : 1);
  // handle tick10ms overrun
  // correct overflow handling costs a lot of code; happens only each 11 min;
  // therefore forget the exact calculation and use only 1 instead; good compromise

#if !defined(CPUARM)
  lastTMR = tmr10ms;
#endif

  getADC();

  getSwitchesPosition(!s_mixer_first_run_done);

#if defined(CPUARM)
  lastTMR = tmr10ms;
#endif

#if defined(PCBSKY9X) && !defined(REVA) && !defined(SIMU)
  Current_analogue = (Current_analogue*31 + s_anaFilt[8] ) >> 5 ;
  if (Current_analogue > Current_max)
    Current_max = Current_analogue ;
#elif defined(CPUM2560) && !defined(SIMU)
  // For PCB V4, use our own 1.2V, external reference (connected to ADC3)
  ADCSRB &= ~(1<<MUX5);
  ADMUX = 0x03|ADC_VREF_TYPE; // Switch MUX to internal reference
#elif defined(PCBSTD) && !defined(SIMU)
  ADMUX = 0x1E|ADC_VREF_TYPE; // Switch MUX to internal reference
#endif

  evalMixes(tick10ms);

#if !defined(CPUARM)
  // Bandgap has had plenty of time to settle...
  getADC_bandgap();
#endif

  if (tick10ms) {

#if !defined(CPUM64) && !defined(ACCURAT_THROTTLE_TIMER)
    //  code cost is about 16 bytes for higher throttle accuracy for timer
    //  would not be noticable anyway, because all version up to this change had only 16 steps;
    //  now it has already 32  steps; this define would increase to 128 steps
    #define ACCURAT_THROTTLE_TIMER
#endif

    /* Throttle trace */
    int16_t val;

    if (g_model.thrTraceSrc > NUM_POTS) {
      uint8_t ch = g_model.thrTraceSrc-NUM_POTS-1;
      val = channelOutputs[ch];

      LimitData *lim = limitAddress(ch);
      int16_t gModelMax = LIMIT_MAX_RESX(lim);
      int16_t gModelMin = LIMIT_MIN_RESX(lim);

      if (lim->revert)
        val = -val + gModelMax;
      else
        val = val - gModelMin;

#if defined(PPM_LIMITS_SYMETRICAL)
      if (lim->symetrical) {
        val -= calc1000toRESX(lim->offset);
      }
#endif

      gModelMax -= gModelMin; // we compare difference between Max and Mix for recaling needed; Max and Min are shifted to 0 by default
      // usually max is 1024 min is -1024 --> max-min = 2048 full range

#ifdef ACCURAT_THROTTLE_TIMER
      if (gModelMax!=0 && gModelMax!=2048) val = (int32_t) (val << 11) / (gModelMax); // rescaling only needed if Min, Max differs
#else
      // @@@ open.20.fsguruh  optimized calculation; now *8 /8 instead of 10 base; (*16/16 already cause a overrun; unsigned calculation also not possible, because v may be negative)
      gModelMax+=255; // force rounding up --> gModelMax is bigger --> val is smaller
      gModelMax >>= (10-2);

      if (gModelMax!=0 && gModelMax!=8) {
        val = (val << 3) / gModelMax; // rescaling only needed if Min, Max differs
      }
#endif

      if (val<0) val=0;  // prevent val be negative, which would corrupt throttle trace and timers; could occur if safetyswitch is smaller than limits
    }
    else {
#ifdef PCBTARANIS
      val = RESX + calibratedStick[g_model.thrTraceSrc == 0 ? THR_STICK : g_model.thrTraceSrc+NUM_STICKS-1];
#else
      val = RESX + (g_model.thrTraceSrc == 0 ? rawAnas[THR_STICK] : calibratedStick[g_model.thrTraceSrc+NUM_STICKS-1]);
#endif
    }

#if defined(ACCURAT_THROTTLE_TIMER)
    val >>= (RESX_SHIFT-6); // calibrate it (resolution increased by factor 4)
#else
    val >>= (RESX_SHIFT-4); // calibrate it
#endif

    // Timers start
    for (uint8_t i=0; i<MAX_TIMERS; i++) {
      int8_t tm = g_model.timers[i].mode;
      uint16_t tv = g_model.timers[i].start;
      TimerState * timerState = &timersStates[i];

      if (tm) {
        if (timerState->state == TMR_OFF) {
          timerState->state = TMR_RUNNING;
          timerState->cnt = 0;
          timerState->sum = 0;
        }

        if (tm == TMRMODE_THR_REL) {
          timerState->cnt++;
          timerState->sum+=val;
        }

        if ((timerState->val_10ms += tick10ms) >= 100) {
          timerState->val_10ms -= 100 ;
          int16_t newTimerVal = timerState->val;
          if (tv) newTimerVal = tv - newTimerVal;

          if (tm == TMRMODE_ABS) {
            newTimerVal++;
          }
          else if (tm == TMRMODE_THR) {
            if (val) newTimerVal++;
          }
          else if (tm == TMRMODE_THR_REL) {
            // @@@ open.20.fsguruh: why so complicated? we have already a s_sum field; use it for the half seconds (not showable) as well
            // check for s_cnt[i]==0 is not needed because we are shure it is at least 1
#if defined(ACCURAT_THROTTLE_TIMER)
            if ((timerState->sum/timerState->cnt) >= 128) {  // throttle was normalized to 0 to 128 value (throttle/64*2 (because - range is added as well)
              newTimerVal++;  // add second used of throttle
              timerState->sum -= 128*timerState->cnt;
            }
#else
            if ((timerState->sum/timerState->cnt) >= 32) {  // throttle was normalized to 0 to 32 value (throttle/16*2 (because - range is added as well)
              newTimerVal++;  // add second used of throttle
              timerState->sum -= 32*timerState->cnt;
            }
#endif
            timerState->cnt=0;
          }
          else if (tm == TMRMODE_THR_TRG) {
            if (val) {
              timerState->state = TMR_TRIGGED;
            }
            if (timerState->state == TMR_TRIGGED) {
              newTimerVal++;
            }
          }
          else {
            if (tm > 0) tm -= (TMRMODE_COUNT-1);
            if (getSwitch(tm))
              newTimerVal++;
          }

          switch (timerState->state) {
            case TMR_RUNNING:
              if (tv && newTimerVal>=(int16_t)tv) {
                AUDIO_TIMER_00(g_model.timers[i].countdownBeep);
                timerState->state = TMR_NEGATIVE;
              }
              break;
            case TMR_NEGATIVE:
              if (newTimerVal >= (int16_t)tv + MAX_ALERT_TIME) timerState->state = TMR_STOPPED;
              break;
          }

          if (tv) newTimerVal = tv - newTimerVal; // if counting backwards - display backwards

          if (newTimerVal != timerState->val) {
            timerState->val = newTimerVal;
            if (timerState->state == TMR_RUNNING) {
              if (g_model.timers[i].countdownBeep && g_model.timers[i].start) {
                if (newTimerVal==30) AUDIO_TIMER_30();
                if (newTimerVal==20) AUDIO_TIMER_20();
                if (newTimerVal<=10) AUDIO_TIMER_LT10(g_model.timers[i].countdownBeep, newTimerVal);
              }
              if (g_model.timers[i].minuteBeep && (newTimerVal % 60)==0) {
                AUDIO_TIMER_MINUTE(newTimerVal);
              }
            }
          }
        }
      }
    } //endfor timer loop (only two)

    static uint8_t  s_cnt_100ms;
    static uint8_t  s_cnt_1s;
    static uint8_t  s_cnt_samples_thr_1s;
    static uint16_t s_sum_samples_thr_1s;

    s_cnt_samples_thr_1s++;
    s_sum_samples_thr_1s+=val;

    if ((s_cnt_100ms += tick10ms) >= 10) { // 0.1sec
      s_cnt_100ms -= 10;
      s_cnt_1s += 1;

      logicalSwitchesTimerTick();

      if (s_cnt_1s >= 10) { // 1sec
        s_cnt_1s -= 10;
        sessionTimer += 1;

        struct t_inactivity *ptrInactivity = &inactivity;
        FORCE_INDIRECT(ptrInactivity) ;
        ptrInactivity->counter++;
        if ((((uint8_t)ptrInactivity->counter)&0x07)==0x01 && g_eeGeneral.inactivityTimer && g_vbat100mV>50 && ptrInactivity->counter > ((uint16_t)g_eeGeneral.inactivityTimer*60))
          AUDIO_INACTIVITY();

#if defined(AUDIO)
        if (mixWarning & 1) if ((sessionTimer&0x03)==0) AUDIO_MIX_WARNING(1);
        if (mixWarning & 2) if ((sessionTimer&0x03)==1) AUDIO_MIX_WARNING(2);
        if (mixWarning & 4) if ((sessionTimer&0x03)==2) AUDIO_MIX_WARNING(3);
#endif

#if defined(ACCURAT_THROTTLE_TIMER)
        val = s_sum_samples_thr_1s / s_cnt_samples_thr_1s;
        s_timeCum16ThrP += (val>>3);  // s_timeCum16ThrP would overrun if we would store throttle value with higher accuracy; therefore stay with 16 steps
        if (val) s_timeCumThr += 1;
        s_sum_samples_thr_1s>>=2;  // correct better accuracy now, because trace graph can show this information; in case thrtrace is not active, the compile should remove this
#else
        val = s_sum_samples_thr_1s / s_cnt_samples_thr_1s;
        s_timeCum16ThrP += (val>>1);
        if (val) s_timeCumThr += 1;
#endif

#if defined(THRTRACE)
        // throttle trace is done every 10 seconds; Tracebuffer is adjusted to screen size.
        // in case buffer runs out, it wraps around
        // resolution for y axis is only 32, therefore no higher value makes sense
        s_cnt_samples_thr_10s += s_cnt_samples_thr_1s;
        s_sum_samples_thr_10s += s_sum_samples_thr_1s;

        if (++s_cnt_10s >= 10) { // 10s
          s_cnt_10s -= 10;
          val = s_sum_samples_thr_10s / s_cnt_samples_thr_10s;
          s_sum_samples_thr_10s = 0;
          s_cnt_samples_thr_10s = 0;

          s_traceBuf[s_traceWr++] = val;
          if (s_traceWr >= MAXTRACE) s_traceWr = 0;
          if (s_traceCnt >= 0) s_traceCnt++;
        }
#endif

        s_cnt_samples_thr_1s = 0;
        s_sum_samples_thr_1s = 0;
      }
    }

#if defined(DSM2)
    static uint8_t count_dsm_range = 0;
    if (dsm2Flag & (DSM2_BIND_FLAG | DSM2_RANGECHECK_FLAG)) {
      if (++count_dsm_range >= 200) {
        AUDIO_PLAY(AU_FRSKY_CHEEP);
        count_dsm_range = 0;
      }
    }
#endif

#if defined(PXX)
    static uint8_t count_pxx = 0;
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
      if (pxxFlag[i] & (PXX_SEND_RANGECHECK | PXX_SEND_RXNUM)) {
        if (++count_pxx >= 250) {
          AUDIO_PLAY(AU_FRSKY_CHEEP);
          count_pxx = 0;
        }
      }
    }
#endif

#if defined(CPUARM)
    checkTrims();
#endif
  }

  s_mixer_first_run_done = true;
}

#define TIME_TO_WRITE() (s_eeDirtyMsk && (tmr10ms_t)(get_tmr10ms() - s_eeDirtyTime10ms) >= (tmr10ms_t)WRITE_DELAY_10MS)


#if defined(NAVIGATION_STICKS)
uint8_t StickScrollAllowed;
uint8_t StickScrollTimer;
static const pm_uint8_t rate[] PROGMEM = { 0, 0, 100, 40, 16, 7, 3, 1 } ;

uint8_t calcStickScroll( uint8_t index )
{
  uint8_t direction;
  int8_t value;

  if ( ( g_eeGeneral.stickMode & 1 ) == 0 )
    index ^= 3;

  value = calibratedStick[index] / 128;
  direction = value > 0 ? 0x80 : 0;
  if (value < 0)
    value = -value;                        // (abs)
  if (value > 7)
    value = 7;
  value = pgm_read_byte(rate+(uint8_t)value);
  if (value)
    StickScrollTimer = STICK_SCROLL_TIMEOUT;               // Seconds
  return value | direction;
}
#endif

void opentxStart()
{
  doSplash();

#if defined(DEBUG_TRACE_BUFFER)
  trace_event(trace_start, 0x12345678);
#endif 

#if defined(PCBSKY9X) && defined(SDCARD) && !defined(SIMU)
  for (int i=0; i<500 && !Card_initialized; i++) {
    CoTickDelay(1);  // 2ms
  }
#endif

#if defined(CPUARM)
  eeLoadModel(g_eeGeneral.currModel);
#endif

  checkAlarm();
  checkAll();

  if (g_eeGeneral.chkSum != evalChkSum()) {
    chainMenu(menuFirstCalib);
  }
}

#if defined(CPUARM) || defined(CPUM2560)
void opentxClose()
{
#if defined(FRSKY)
  // TODO needed? telemetryEnd();
#endif

#if defined(LUA)
  luaClose();
#endif

#if defined(SDCARD)
  closeLogs();
  sdDone();
#endif

#if defined(HAPTIC)
  hapticOff();
#endif

  saveTimers();

#if defined(CPUARM) && defined(FRSKY)
  if (g_model.frsky.mAhPersistent && g_model.frsky.storedMah!=frskyData.hub.currentConsumption) {
    g_model.frsky.storedMah = frskyData.hub.currentConsumption;
    eeDirty(EE_MODEL);
  }
  else if (!g_model.frsky.mAhPersistent && g_model.frsky.storedMah!=0) {
    g_model.frsky.storedMah = 0;
    eeDirty(EE_MODEL);
  }
#endif

#if defined(PCBSKY9X)
  uint32_t mAhUsed = g_eeGeneral.mAhUsed + Current_used * (488 + g_eeGeneral.currentCalib) / 8192 / 36;
  if (g_eeGeneral.mAhUsed != mAhUsed) {
    g_eeGeneral.mAhUsed = mAhUsed;
  }
#endif

#if defined(PCBTARANIS)
  if ((g_model.nPotsToWarn >> 6) == 2) {
    for (uint8_t i=0; i<NUM_POTS; i++)
      if (!(g_model.nPotsToWarn & (1 << i)))
        SAVE_POT_POSITION(i);
    eeDirty(EE_MODEL);
  }
#endif

  g_eeGeneral.unexpectedShutdown = 0;

  eeDirty(EE_GENERAL);
  eeCheck(true);
}
#endif


#if defined(USB_JOYSTICK) && defined(PCBTARANIS) && !defined(SIMU)
extern USB_OTG_CORE_HANDLE USB_OTG_dev;

/*
  Prepare and send new USB data packet

  The format of HID_Buffer is defined by
  USB endpoint description can be found in 
  file usb_hid_joystick.c, variable HID_JOYSTICK_ReportDesc
*/
void usbJoystickUpdate(void)
{
  static uint8_t HID_Buffer[HID_IN_PACKET];
  
  //buttons
  HID_Buffer[0] = 0; //buttons
  for (int i = 0; i < 8; ++i) {
    if ( channelOutputs[i+8] > 0 ) {
      HID_Buffer[0] |= (1 << i);
    } 
  }

  //analog values
  //uint8_t * p = HID_Buffer + 1;
  for (int i = 0; i < 8; ++i) {
    int16_t value = channelOutputs[i] / 8;
    if ( value > 127 ) value = 127;
    else if ( value < -127 ) value = -127;
    HID_Buffer[i+1] = static_cast<int8_t>(value);  
  }

  USBD_HID_SendReport (&USB_OTG_dev, HID_Buffer, HID_IN_PACKET );
}

#endif //#if defined(USB_JOYSTICK) && defined(PCBTARANIS) && !defined(SIMU)


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

#if defined(MODULE_ALWAYS_SEND_PULSES)
  if (startupWarningState < STARTUP_WARNING_DONE) {
    // don't do menu's until throttle and switch warnings are handled
    return;
  }
#endif

  if (!usbPlugged()) {
    // TODO merge these 2 branches
#if defined(PCBSKY9X)
    if (Eeprom32_process_state != E32_IDLE)
      ee32_process();
    else if (TIME_TO_WRITE())
      eeCheck(false);
#elif defined(CPUARM)
    if (theFile.isWriting())
      theFile.nextWriteStep();
    else if (TIME_TO_WRITE())
      eeCheck(false);
#else
    if (!eeprom_buffer_size) {
      if (theFile.isWriting())
        theFile.nextWriteStep();
      else if (TIME_TO_WRITE())
        eeCheck(false);
    }
#endif
  }

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

#if (!defined(CPUARM) || defined(SIMU)) && (defined(FRSKY) || defined(MAVLINK))
  telemetryWakeup();
#endif

#if defined(PCBTARANIS)
  uint8_t requiredTrainerMode = g_model.trainerMode;
  if (requiredTrainerMode != currentTrainerMode) {
    currentTrainerMode = requiredTrainerMode;
    if (requiredTrainerMode) {
      // slave
      stop_trainer_capture();
      init_trainer_ppm();
    }
    else {
      // master
      stop_trainer_ppm();
      init_trainer_capture();
    }
  }
#endif

#if defined(PCBTARANIS) && !defined(SIMU)
  static bool usbStarted = false;
  if (!usbStarted && usbPlugged()) {
#if defined(USB_MASS_STORAGE)
    opentxClose();
#endif
    usbStart();
#if defined(USB_MASS_STORAGE)
    usbPluggedIn();
#endif
    usbStarted = true;
  }
  
#if defined(USB_JOYSTICK)
  if (usbStarted) {
    if (!usbPlugged()) {
      //disable USB
      usbStop();
      usbStarted = false;
    }
    else {
      usbJoystickUpdate();
    }
  }
#endif

#endif //#if defined(PCBTARANIS) && !defined(SIMU)

#if defined(NAVIGATION_STICKS)
  if (StickScrollAllowed) {
    if ( StickScrollTimer ) {
      static uint8_t repeater;
      uint8_t direction;
      uint8_t value;

      if ( repeater < 128 )
      {
        repeater += 1;
      }
      value = calcStickScroll( 2 );
      direction = value & 0x80;
      value &= 0x7F;
      if ( value )
      {
        if ( repeater > value )
        {
          repeater = 0;
          if ( evt == 0 )
          {
            if ( direction )
            {
              evt = EVT_KEY_FIRST(KEY_UP);
            }
            else
            {
              evt = EVT_KEY_FIRST(KEY_DOWN);
            }
          }
        }
      }
      else
      {
        value = calcStickScroll( 3 );
        direction = value & 0x80;
        value &= 0x7F;
        if ( value )
        {
          if ( repeater > value )
          {
            repeater = 0;
            if ( evt == 0 )
            {
              if ( direction )
              {
                evt = EVT_KEY_FIRST(KEY_RIGHT);
              }
              else
              {
                evt = EVT_KEY_FIRST(KEY_LEFT);
              }
            }
          }
        }
      }
    }
  }
  else {
    StickScrollTimer = 0;          // Seconds
  }
  StickScrollAllowed = 1 ;
#endif

#if defined(USB_MASS_STORAGE)
  if (usbPlugged()) {
    lcd_clear();
    menuMainView(0);
  }
  else 
#endif
  {
    const char *warn = s_warning;
    uint8_t menu = s_menu_count;

    if (!LCD_LOCKED()) {
      lcd_clear();
      g_menuStack[g_menuStackPtr]((warn || menu) ? 0 : evt);
    }

#if defined(LUA)
    luaTask(evt);
#endif

    if (!LCD_LOCKED()) {
      if (warn) DISPLAY_WARNING(evt);
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
  }

  drawStatusLine();

  lcdRefresh(LCD_REFRESH_DONT_WAIT);

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
    int32_t instant_vbat = anaIn(TX_VOLTAGE);
#if defined(PCBTARANIS)
    instant_vbat = (instant_vbat + instant_vbat*(g_eeGeneral.vBatCalib)/128) * BATT_SCALE;
    instant_vbat >>= 11;
    instant_vbat += 2; // because of the diode
#elif defined(PCBSKY9X)
    instant_vbat = (instant_vbat + instant_vbat*(g_eeGeneral.vBatCalib)/128) * 4191;
    instant_vbat /= 55296;
#elif defined(CPUM2560)
    instant_vbat = (instant_vbat*1112 + instant_vbat*g_eeGeneral.vBatCalib + (BandGap<<2)) / (BandGap<<3);
#else
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
#if defined(PCBSKY9X)
      else if (g_eeGeneral.temperatureWarn && getTemperature() >= g_eeGeneral.temperatureWarn) {
        AUDIO_TX_TEMP_HIGH();
      }
      else if (g_eeGeneral.mAhWarn && (g_eeGeneral.mAhUsed + Current_used * (488 + g_eeGeneral.currentCalib)/8192/36) / 500 >= g_eeGeneral.mAhWarn) {
        AUDIO_TX_MAH_HIGH();
      }
#endif
    }
  }
}

int16_t g_ppmIns[NUM_TRAINER];
uint8_t ppmInState = 0; // 0=unsync 1..8= wait for value i-1
uint8_t ppmInValid = 0;

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
ISR(TIMER_AUDIO_VECT, ISR_NOBLOCK)
{
  cli();
  PAUSE_AUDIO_INTERRUPT(); // stop reentrance
  sei();

#if defined(AUDIO)
  AUDIO_DRIVER();
#endif

#if defined(VOICE)
  VOICE_DRIVER();
#endif

  cli();
  RESUME_AUDIO_INTERRUPT();
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
  if (val>4000 && val < 16000) { // G: Prioritize reset pulse. (Needed when less than 8 incoming pulses)
    ppmInState = 1; // triggered
  }
  else {
    if (ppmInState>0 && ppmInState<=8) {
      if (val>800 && val<2200) { // if valid pulse-width range
        ppmInValid = 100;
        g_ppmIns[ppmInState++ - 1] = (int16_t)(val - 1500) * (uint8_t)(g_eeGeneral.PPM_Multiplier+10)/10; //+-500 != 512, but close enough.
      }
      else {
        ppmInState = 0; // not triggered
      }
    }
  }

  lastCapt = capture;

  cli(); // disable other interrupts for stack pops before this function's RETI
  RESUME_PPMIN_INTERRUPT();
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

#if defined (FRSKY)

// USART0 Transmit Data Register Emtpy ISR
FORCEINLINE void FRSKY_USART0_vect()
{
  if (frskyTxBufferCount > 0) {
    UDR0 = frskyTxBuffer[--frskyTxBufferCount];
  }
  else {
    UCSR0B &= ~(1 << UDRIE0); // disable UDRE0 interrupt
  }
}

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

#if defined(PCBTARANIS)
  #define INSTANT_TRIM_MARGIN 10 /* around 1% */
#else
  #define INSTANT_TRIM_MARGIN 15 /* around 1.5% */
#endif

void instantTrim()
{
#if defined(PCBTARANIS)
  int16_t  anas_0[NUM_INPUTS];
  evalInputs(e_perout_mode_notrainer | e_perout_mode_nosticks);
  memcpy(anas_0, anas, sizeof(anas_0));
#endif

  evalInputs(e_perout_mode_notrainer);

  for (uint8_t stick=0; stick<NUM_STICKS; stick++) {
    if (stick!=THR_STICK) {
      // don't instant trim the throttle stick
      uint8_t trim_phase = getTrimFlightPhase(mixerCurrentFlightMode, stick);
#if defined(PCBTARANIS)
      int16_t delta = 0;
      for (int e=0; e<MAX_EXPOS; e++) {
        ExpoData * ed = expoAddress(e);
        if (!EXPO_VALID(ed)) break; // end of list
        if (ed->srcRaw-MIXSRC_Rud == stick) {
          delta = anas[ed->chn] - anas_0[ed->chn];
          break;
        }
      }
#else
      int16_t delta = anas[stick];
#endif
      if (abs(delta) >= INSTANT_TRIM_MARGIN) {
        int16_t trim = limit<int16_t>(TRIM_EXTENDED_MIN, (delta + trims[stick]) / 2, TRIM_EXTENDED_MAX);
        setTrimValue(trim_phase, stick, trim);
      }
    }
  }

  eeDirty(EE_MODEL);
  AUDIO_WARNING2();
}

void copySticksToOffset(uint8_t ch)
{
  pauseMixerCalculations();
  int32_t zero = (int32_t)channelOutputs[ch];

  evalFlightModeMixes(e_perout_mode_nosticks+e_perout_mode_notrainer, 0);
  int32_t val = chans[ch];
  LimitData *ld = limitAddress(ch);
  limit_min_max_t lim = LIMIT_MIN(ld);
  if (val < 0) {
    val = -val;
    lim = LIMIT_MIN(ld);
  }
#if defined(CPUARM)
  zero = (zero*256000 - val*lim) / (1024*256-val);
#else
  zero = (zero*25600 - val*lim) / (26214-val);
#endif
  ld->offset = (ld->revert ? -zero : zero);
  resumeMixerCalculations();
  eeDirty(EE_MODEL);
}

void copyTrimsToOffset(uint8_t ch)
{
  int16_t zero;

  pauseMixerCalculations();

  evalFlightModeMixes(e_perout_mode_noinput, 0); // do output loop - zero input sticks and trims
  zero = applyLimits(ch, chans[ch]);

  evalFlightModeMixes(e_perout_mode_noinput-e_perout_mode_notrims, 0); // do output loop - only trims

  int16_t output = applyLimits(ch, chans[ch]) - zero;
  int16_t v = g_model.limitData[ch].offset;
  if (g_model.limitData[ch].revert) output = -output;
#if defined(CPUARM)
  v += (output * 125) / 128;
#else
  v += output;
#endif
  g_model.limitData[ch].offset = limit((int16_t)-1000, (int16_t)v, (int16_t)1000); // make sure the offset doesn't go haywire

  resumeMixerCalculations();
  eeDirty(EE_MODEL);
}

void moveTrimsToOffsets() // copy state of 3 primary to subtrim
{
  int16_t zeros[NUM_CHNOUT];

  pauseMixerCalculations();

  evalFlightModeMixes(e_perout_mode_noinput, 0); // do output loop - zero input sticks and trims
  for (uint8_t i=0; i<NUM_CHNOUT; i++) {
    zeros[i] = applyLimits(i, chans[i]);
  }

  evalFlightModeMixes(e_perout_mode_noinput-e_perout_mode_notrims, 0); // do output loop - only trims

  for (uint8_t i=0; i<NUM_CHNOUT; i++) {
    int16_t output = applyLimits(i, chans[i]) - zeros[i];
    int16_t v = g_model.limitData[i].offset;
    if (g_model.limitData[i].revert) output = -output;
#if defined(CPUARM)
    v += (output * 125) / 128;
#else
    v += output;
#endif
    g_model.limitData[i].offset = limit((int16_t)-1000, (int16_t)v, (int16_t)1000); // make sure the offset doesn't go haywire
  }

  // reset all trims, except throttle (if throttle trim)
  for (uint8_t i=0; i<NUM_STICKS; i++) {
    if (i!=THR_STICK || !g_model.thrTrim) {
      int16_t original_trim = getTrimValue(mixerCurrentFlightMode, i);
      for (uint8_t phase=0; phase<MAX_FLIGHT_MODES; phase++) {
#if defined(PCBTARANIS)
        trim_t trim = getRawTrimValue(phase, i);
        if (trim.mode / 2 == phase)
          setTrimValue(phase, i, trim.value - original_trim);
#else
        trim_t trim = getRawTrimValue(phase, i);
        if (trim <= TRIM_EXTENDED_MAX)
          setTrimValue(phase, i, trim - original_trim);
#endif
      }
    }
  }

  resumeMixerCalculations();

  eeDirty(EE_MODEL);
  AUDIO_WARNING2();
}

#if defined(CPUARM) || defined(CPUM2560)
void saveTimers()
{
  for (uint8_t i=0; i<MAX_TIMERS; i++) {
    if (g_model.timers[i].persistent) {
      TimerState *timerState = &timersStates[i];
      if (g_model.timers[i].value != (uint16_t)timerState->val) {
        g_model.timers[i].value = timerState->val;
        eeDirty(EE_MODEL);
      }
    }
  }

#if defined(CPUARM) && !defined(REVA)
  if (sessionTimer > 0) {
    g_eeGeneral.globalTimer += sessionTimer;
    eeDirty(EE_GENERAL);
    sessionTimer = 0;
  }
#endif
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
    case 0:
      stack = menusStack;
      size = MENUS_STACK_SIZE;
      break;
    case 1:
      stack = mixerStack;
      size = MIXER_STACK_SIZE;
      break;
    case 2:
      stack = audioStack;
      size = AUDIO_STACK_SIZE;
      break;
#if 0 // defined(PCBTARANIS)
    case 255:
      // main stack
      stack = (OS_STK *)&_main_stack_start;
      size = ((unsigned char *)&_estack - (unsigned char *)&_main_stack_start) / 4;
      break;
#endif
    default:
      return 0;
  }

  uint16_t i=0;
  for (; i<size; i++)
    if (stack[i] != 0x55555555)
      break;
  return i*4;
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
  while ( p > q ) {
    *p-- = 0x55 ;
  }
}

uint16_t stack_free()
{
  unsigned char *p ;

  p = &__bss_end + 1 ;
  while ( *p++ == 0x55 );
  return p - &__bss_end ;
}
#endif

#if defined(CPUM2560)
  #define OPENTX_INIT_ARGS const uint8_t mcusr
#elif defined(PCBSTD)
  #define OPENTX_INIT_ARGS const uint8_t mcusr
#else
  #define OPENTX_INIT_ARGS
#endif

inline void opentxInit(OPENTX_INIT_ARGS)
{
  eeReadAll();

#if defined(CPUARM)
  if (UNEXPECTED_SHUTDOWN()) {
    unexpectedShutdown = 1;
  }
#endif

#if defined(PCBTARANIS)
  BACKLIGHT_ON();
#endif

#if MENUS_LOCK == 1
  getMovedSwitch();
  if (TRIMS_PRESSED() && g_eeGeneral.switchUnlockStates==switches_states) {
    readonly = false;
  }
#endif

#if defined(VOICE)
#if defined(CPUARM)
  currentSpeakerVolume = requiredSpeakerVolume = g_eeGeneral.speakerVolume+VOLUME_LEVEL_DEF;
#endif
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

#if defined(RTCLOCK) && !defined(COPROCESSOR)
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
    opentxStart();
  }

#if defined(CPUARM) || defined(CPUM2560)
  if (!g_eeGeneral.unexpectedShutdown) {
    g_eeGeneral.unexpectedShutdown = 1;
    eeDirty(EE_GENERAL);
  }
#endif
  
  lcdSetContrast();
  backlightOn();

#if defined(PCBTARANIS)
  uart3Init(g_eeGeneral.uart3Mode, MODEL_TELEMETRY_PROTOCOL());
#endif

#if defined(CPUARM)
  init_trainer_capture();
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
      doMixerCalculations();
      CoLeaveMutexSection(mixerMutex);

#if defined(FRSKY) || defined(MAVLINK)
      telemetryWakeup();
#endif

      if (heartbeat == HEART_WDT_CHECK) {
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
    // TODO remove completely massstorage from sky9x firmware
    CoTickDelay(5);  // 5*2ms for now
  }

  lcd_clear();
  displayPopup(STR_SHUTDOWN);

  opentxClose();

  lcd_clear();
  lcdRefresh();
  lcdOff();

  SysTick->CTRL = 0; // turn off systick

  pwrOff(); // Only turn power off if necessary
}

extern void audioTask(void* pdata);

#endif

int main(void)
{
  // G: The WDT remains active after a WDT reset -- at maximum clock speed. So it's
  // important to disable it before commencing with system initialisation (or
  // we could put a bunch more wdt_reset()s in. But I don't like that approach
  // during boot up.)
#if defined(CPUM2560) || defined(CPUM2561)
  uint8_t mcusr = MCUSR; // save the WDT (etc) flags
  MCUSR = 0; // must be zeroed before disabling the WDT
#elif defined(PCBSTD)
  uint8_t mcusr = MCUCSR;
  MCUCSR = 0;
#endif
#if defined(PCBTARANIS)
  g_eeGeneral.contrast=30;
#endif  
  wdt_disable();

  boardInit();
  
#if !defined(PCBTARANIS)
  lcdInit();
#endif

  stack_paint();

  g_menuStack[0] = menuMainView;
#if MENUS_LOCK != 2/*no menus*/
  g_menuStack[1] = menuModelSelect;
#endif

#if !defined(PCBTARANIS)
  lcdSetRefVolt(25);
#endif

  sei(); // interrupts needed for telemetryInit and eeReadAll.

#if defined(FRSKY) && !defined(DSM2_SERIAL)
  telemetryInit();
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
  CoInitOS();

#if defined(CPUARM) && defined(DEBUG)
  debugTaskId = CoCreateTaskEx(debugTask, NULL, 10, &debugStack[DEBUG_STACK_SIZE-1], DEBUG_STACK_SIZE, 1, false);
#endif

#if defined(BLUETOOTH)
  btTaskId = CoCreateTask(btTask, NULL, 15, &btStack[BT_STACK_SIZE-1], BT_STACK_SIZE);
#endif

  mixerTaskId = CoCreateTask(mixerTask, NULL, 5, &mixerStack[MIXER_STACK_SIZE-1], MIXER_STACK_SIZE);
  menusTaskId = CoCreateTask(menusTask, NULL, 10, &menusStack[MENUS_STACK_SIZE-1], MENUS_STACK_SIZE);
  audioTaskId = CoCreateTask(audioTask, NULL, 7, &audioStack[AUDIO_STACK_SIZE-1], AUDIO_STACK_SIZE);

  audioMutex = CoCreateMutex();
  mixerMutex = CoCreateMutex();

  CoStartOS();
#else
#if defined(CPUM2560)
  uint8_t shutdown_state = 0;
#endif

  while(1) {
#if defined(CPUM2560)
    if ((shutdown_state=pwrCheck()) > e_power_trainer)
      break;
#endif

    perMain();

    if (heartbeat == HEART_WDT_CHECK) {
      wdt_reset();
      heartbeat = 0;
    }
  }
#endif

#if defined(CPUM2560)
  // Time to switch off
  lcd_clear();
  displayPopup(STR_SHUTDOWN);
  opentxClose();
  lcd_clear() ;
  lcdRefresh() ;
  pwrOff(); // Only turn power off if necessary
  wdt_disable();
  while(1); // never return from main() - there is no code to return back, if any delays occurs in physical power it does dead loop.
#endif
}
#endif // !SIMU
