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

#define CS_LAST_VALUE_INIT -32768

#if defined(CPUARM)

enum LogicalSwitchContextState {
  SWITCH_START,
  SWITCH_DELAY,
  SWITCH_ENABLE
};

PACK(typedef struct {
  uint8_t state:1;
  uint8_t timerState:2;
  uint8_t spare:5;
  uint8_t timer;
  int16_t lastValue;
}) LogicalSwitchContext;

PACK(typedef struct {
  LogicalSwitchContext lsw[NUM_LOGICAL_SWITCH];
}) LogicalSwitchesFlightModeContext;
LogicalSwitchesFlightModeContext lswFm[MAX_FLIGHT_MODES];

#define LS_LAST_VALUE(fm, idx) lswFm[fm].lsw[idx].lastValue
        
#else

int16_t lsLastValue[NUM_LOGICAL_SWITCH];
#define LS_LAST_VALUE(fm, idx) lsLastValue[idx]

volatile GETSWITCH_RECURSIVE_TYPE s_last_switch_used = 0;
volatile GETSWITCH_RECURSIVE_TYPE s_last_switch_value = 0;

#endif

#if defined(PCBTARANIS)
tmr10ms_t switchesMidposStart[6] = { 0 };
uint64_t  switchesPos = 0;
tmr10ms_t potsLastposStart[NUM_XPOTS];
uint8_t   potsPos[NUM_XPOTS];

int switchConfig(int idx)
{
  uint32_t config = SWITCH_CONFIG(idx);
  if (config == SWITCH_DEFAULT)
    config = SWITCH_DEFAULT_CONFIG(idx);
  return config;
}

uint64_t check2PosSwitchPosition(EnumKeys sw)
{
  uint64_t result;
  uint32_t index;

  if (switchState(sw))
    index = sw - SW_SA0;
  else
    index = sw - SW_SA0 + 1;

  result = ((int64_t)1 << index);

  if (!(switchesPos & result)) {
    PLAY_SWITCH_MOVED(index);
  }

  return result;
}

uint32_t check3PosSwitchPosition(uint8_t idx, EnumKeys sw, bool startup)
{
  uint32_t result;
  uint32_t index;

  if (switchState(sw)) {
    index = sw - SW_SA0;
    result = (1 << index);
    switchesMidposStart[idx] = 0;
  }
  else if (switchState(EnumKeys(sw+2))) {
    index = sw - SW_SA0 + 2;
    result = (1 << index);
    switchesMidposStart[idx] = 0;
  }
  else if (startup || (switchesPos & (1 << (sw - SW_SA0 + 1))) || g_eeGeneral.switchesDelay==SWITCHES_DELAY_NONE || (switchesMidposStart[idx] && (tmr10ms_t)(get_tmr10ms() - switchesMidposStart[idx]) > SWITCHES_DELAY())) {
    index = sw - SW_SA0 + 1;
    result = (1 << index);
    switchesMidposStart[idx] = 0;
  }
  else {
    index = sw - SW_SA0 + 1;
    if (!switchesMidposStart[idx]) {
      switchesMidposStart[idx] = get_tmr10ms();
    }
    result = (switchesPos & (0x7 << (sw - SW_SA0)));
  }

  if (!(switchesPos & result)) {
    PLAY_SWITCH_MOVED(index);
  }

  return result;
}

#define CHECK_2POS(sw)       newPos |= check2PosSwitchPosition(sw ## 0)
#define CHECK_3POS(idx, sw)  newPos |= check3PosSwitchPosition(idx, sw ## 0, startup)

void getSwitchesPosition(bool startup)
{
  uint64_t newPos = 0;
  CHECK_3POS(0, SW_SA);
  CHECK_3POS(1, SW_SB);
  CHECK_3POS(2, SW_SC);
  CHECK_3POS(3, SW_SD);
  CHECK_3POS(4, SW_SE);
  CHECK_2POS(SW_SF);
  CHECK_3POS(5, SW_SG);
  CHECK_2POS(SW_SH);
  if (IS_2x2POS(0))
    CHECK_2POS(SW_SI);
  if (IS_2x2POS(1))
    CHECK_2POS(SW_SJ);
  if (IS_2x2POS(2))
    CHECK_2POS(SW_SK);
  if (IS_2x2POS(3))
    CHECK_2POS(SW_SL);
  if (IS_2x2POS(4))
    CHECK_2POS(SW_SM);
  if (IS_2x2POS(6))
    CHECK_2POS(SW_SN);

  switchesPos = newPos;

  for (int i=0; i<NUM_XPOTS; i++) {
    if (IS_POT_MULTIPOS(POT1+i)) {
      StepsCalibData * calib = (StepsCalibData *) &g_eeGeneral.calib[POT1+i];
      if (calib->count>0 && calib->count<XPOTS_MULTIPOS_COUNT) {
        uint8_t pos = anaIn(POT1+i) / (2*RESX/calib->count);
        uint8_t previousPos = potsPos[i] >> 4;
        uint8_t previousStoredPos = potsPos[i] & 0x0F;
        if (pos != previousPos) {
          potsLastposStart[i] = get_tmr10ms();
          potsPos[i] = (pos << 4) | previousStoredPos;
        }
        else if (startup || g_eeGeneral.switchesDelay==SWITCHES_DELAY_NONE || (tmr10ms_t)(get_tmr10ms() - potsLastposStart[i]) > SWITCHES_DELAY()) {
          potsLastposStart[i] = 0;
          potsPos[i] = (pos << 4) | pos;
          if (previousStoredPos != pos) {
            PLAY_SWITCH_MOVED(SWSRC_LAST_SWITCH+i*XPOTS_MULTIPOS_COUNT+pos);
          }
        }
      }
    }
  }
}
#define SWITCH_POSITION(sw)  (switchesPos & (1<<(sw)))
#define POT_POSITION(sw)     ((potsPos[(sw)/XPOTS_MULTIPOS_COUNT] & 0x0f) == ((sw) % XPOTS_MULTIPOS_COUNT))

getvalue_t getValueForLogicalSwitch(uint8_t i)
{
  getvalue_t result = getValue(i);
  if (i>=MIXSRC_FIRST_INPUT && i<=MIXSRC_LAST_INPUT) {
    int8_t trimIdx = virtualInputsTrims[i-MIXSRC_FIRST_INPUT];
    if (trimIdx >= 0) {
      int16_t trim = trims[trimIdx];
      if (trimIdx == THR_STICK && g_model.throttleReversed)
        result -= trim;
      else
        result += trim;
    }
  }
  return result;
}
#else
  #define getValueForLogicalSwitch(i) getValue(i)
#endif

PACK(typedef struct {
  uint8_t state;
  uint8_t last;
}) ls_sticky_struct;

PACK(typedef struct {
  uint16_t state:1;
  uint16_t duration:15;
}) ls_stay_struct;

bool getLogicalSwitch(uint8_t idx)
{
  LogicalSwitchData * ls = lswAddress(idx);
  bool result;

#if defined(CPUARM)
  int8_t s = ls->andsw;
#else
  uint8_t s = ls->andsw;
  if (s > SWSRC_LAST_SWITCH) {
    s += SWSRC_SW1-SWSRC_LAST_SWITCH-1;
  }
#endif

  if (ls->func == LS_FUNC_NONE || (s && !getSwitch(s))) {
    if (ls->func != LS_FUNC_STICKY) {
      LS_LAST_VALUE(mixerCurrentFlightMode, idx) = CS_LAST_VALUE_INIT;
    }
    result = false;
  }
  else if ((s=lswFamily(ls->func)) == LS_FAMILY_BOOL) {
    bool res1 = getSwitch(ls->v1);
    bool res2 = getSwitch(ls->v2);
    switch (ls->func) {
      case LS_FUNC_AND:
        result = (res1 && res2);
        break;
      case LS_FUNC_OR:
        result = (res1 || res2);
        break;
      // case LS_FUNC_XOR:
      default:
        result = (res1 ^ res2);
        break;
    }
  }
  else if (s == LS_FAMILY_TIMER) {
    result = (LS_LAST_VALUE(mixerCurrentFlightMode, idx) <= 0);
  }
  else if (s == LS_FAMILY_STICKY) {
    result = (LS_LAST_VALUE(mixerCurrentFlightMode, idx) & (1<<0));
  }
#if defined(CPUARM)
  else if (s == LS_FAMILY_EDGE) {
    result = (LS_LAST_VALUE(mixerCurrentFlightMode, idx) & (1<<0));
  }
#endif
  else {
    getvalue_t x = getValueForLogicalSwitch(ls->v1);
    getvalue_t y;
    if (s == LS_FAMILY_COMP) {
      y = getValueForLogicalSwitch(ls->v2);

      switch (ls->func) {
        case LS_FUNC_EQUAL:
          result = (x==y);
          break;
        case LS_FUNC_GREATER:
          result = (x>y);
          break;
        default:
          result = (x<y);
          break;
      }
    }
    else {
      uint8_t v1 = ls->v1;
#if defined(FRSKY)
      // Telemetry
      if (v1 >= MIXSRC_FIRST_TELEM) {
#if defined(CPUARM)
        if (!TELEMETRY_STREAMING() || IS_FAI_FORBIDDEN(v1-1)) {
#else
        if ((!TELEMETRY_STREAMING() && v1 >= MIXSRC_FIRST_TELEM+TELEM_FIRST_STREAMED_VALUE-1) || IS_FAI_FORBIDDEN(v1-1)) {
#endif
          result = false;
          goto DurationAndDelayProcessing;
        }

        y = convertLswTelemValue(ls);

#if defined(GAUGES) && !defined(CPUARM)
        // Fill the telemetry bars threshold array
        if (s == LS_FAMILY_OFS) {
          uint8_t idx = v1-MIXSRC_FIRST_TELEM+1-TELEM_ALT;
          if (idx < THLD_MAX) {
            FILL_THRESHOLD(idx, ls->v2);
          }
        }
#endif

      }
      else if (v1 >= MIXSRC_GVAR1) {
        y = ls->v2;
      }
      else {
        y = calc100toRESX(ls->v2);
      }
#else
      if (v1 >= MIXSRC_FIRST_TELEM) {
        y = (int16_t)3 * (128+ls->v2); // it's a Timer
      }
      else if (v1 >= MIXSRC_GVAR1) {
        y = ls->v2; // it's a GVAR
      }
      else {
        y = calc100toRESX(ls->v2);
      }
#endif

      switch (ls->func) {
#if defined(CPUARM)
        case LS_FUNC_VEQUAL:
          result = (x==y);
          break;
#endif
        case LS_FUNC_VALMOSTEQUAL:
#if defined(GVARS)
          if (v1 >= MIXSRC_GVAR1 && v1 <= MIXSRC_LAST_GVAR)
            result = (x==y);
          else
#endif
          result = (abs(x-y) < (1024 / STICK_TOLERANCE));
          break;
        case LS_FUNC_VPOS:
          result = (x>y);
          break;
        case LS_FUNC_VNEG:
          result = (x<y);
          break;
        case LS_FUNC_APOS:
          result = (abs(x)>y);
          break;
        case LS_FUNC_ANEG:
          result = (abs(x)<y);
          break;
        default:
        {
          if (LS_LAST_VALUE(mixerCurrentFlightMode, idx) == CS_LAST_VALUE_INIT) {
            LS_LAST_VALUE(mixerCurrentFlightMode, idx) = x;
          }
          int16_t diff = x - LS_LAST_VALUE(mixerCurrentFlightMode, idx);
          bool update = false;
          if (ls->func == LS_FUNC_DIFFEGREATER) {
            if (y >= 0) {
              result = (diff >= y);
              if (diff < 0)
                update = true;
            }
            else {
              result = (diff <= y);
              if (diff > 0)
                update = true;
            }
          }
          else {
            result = (abs(diff) >= y);
          }
          if (result || update) {
            LS_LAST_VALUE(mixerCurrentFlightMode, idx) = x;
          }
          break;
        }
      }
    }
  }

#if defined(FRSKY)
DurationAndDelayProcessing:
#endif

#if defined(CPUARM)
    if (ls->delay || ls->duration) {
      LogicalSwitchContext &context = lswFm[mixerCurrentFlightMode].lsw[idx];
      if (result) {
        if (context.timerState == SWITCH_START) {
          // set delay timer
          context.timerState = SWITCH_DELAY;
          context.timer = ls->delay;
        }
        
        if (context.timerState == SWITCH_DELAY) {
          if (context.timer) {
            result = false;   // return false while delay timer running
          }
          else {
            // set duration timer
            context.timerState = SWITCH_ENABLE;
            context.timer = ls->duration;
          }
        }
        
        if (context.timerState == SWITCH_ENABLE) {
          result = (ls->duration==0 || context.timer>0); // return false after duration timer runs out
          if (!result && ls->func == LS_FUNC_STICKY) {
            ls_sticky_struct & lastValue = (ls_sticky_struct &)context.lastValue;
            lastValue.state = 0;
          }
        }
      }
      else if (context.timerState == SWITCH_ENABLE && ls->duration > 0 && context.timer > 0) {
        result = true;
      }
      else {        
        context.timerState = SWITCH_START;
        context.timer = 0;
      }
    }
#endif

  return result;
}

#if defined(CPUARM)
bool getSwitch(int8_t swtch, uint8_t flags)
#else
bool getSwitch(int8_t swtch)
#endif
{
  bool result;

  if (swtch == SWSRC_NONE)
    return true;

  uint8_t cs_idx = abs(swtch);

  if (cs_idx == SWSRC_One) {
    result = !s_mixer_first_run_done;
  }
  else if (cs_idx == SWSRC_ON) {
    result = true;
  }
  else if (cs_idx <= SWSRC_LAST_SWITCH) {
#if defined(PCBTARANIS)
    if (flags & GETSWITCH_MIDPOS_DELAY)
      result = SWITCH_POSITION(cs_idx-SWSRC_FIRST_SWITCH);
    else
#endif
    result = switchState((EnumKeys)(SW_BASE+cs_idx-SWSRC_FIRST_SWITCH));

#if defined(MODULE_ALWAYS_SEND_PULSES)
    if (startupWarningState < STARTUP_WARNING_DONE) {
      // if throttle or switch warning is currently active, ignore actual stick position and use wanted values
      if (cs_idx <= 3) {
        if (!(g_model.switchWarningEnable&1)) {     // ID1 to ID3 is just one bit in switchWarningEnable
          result = (cs_idx)==((g_model.switchWarningState&3)+1);  // overwrite result with desired value
        }
      }
      else if (!(g_model.switchWarningEnable & (1<<(cs_idx-3)))) {
        // current switch should not be ignored for warning
        result = g_model.switchWarningState & (1<<(cs_idx-2)); // overwrite result with desired value
      }
    }
#endif
  }
#if defined(PCBTARANIS)
  else if (cs_idx <= SWSRC_LAST_MULTIPOS_SWITCH) {
    result = POT_POSITION(cs_idx-SWSRC_FIRST_MULTIPOS_SWITCH);
  }
#endif
  else if (cs_idx <= SWSRC_LAST_TRIM) {
    uint8_t idx = cs_idx - SWSRC_FIRST_TRIM;
    idx = (CONVERT_MODE(idx/2) << 1) + (idx & 1);
    result = trimDown(idx);
  }
#if ROTARY_ENCODERS > 0
  else if (cs_idx == SWSRC_REa) {
    result = REA_DOWN();
  }
#endif
#if ROTARY_ENCODERS > 1
  else if (cs_idx == SWSRC_REb) {
    result = REB_DOWN();
  }
#endif
#if defined(CPUARM) && defined(FLIGHT_MODES)
  else if (cs_idx >= SWSRC_FIRST_FLIGHT_MODE) {
    uint8_t idx = cs_idx - SWSRC_FIRST_FLIGHT_MODE;
    if (flags & GETSWITCH_MIDPOS_DELAY)
      result = (idx == flightModeTransitionLast);
    else
      result = (idx == mixerCurrentFlightMode);
  }
#endif
  else {
    cs_idx -= SWSRC_FIRST_LOGICAL_SWITCH;
#if defined(CPUARM)
    result = lswFm[mixerCurrentFlightMode].lsw[cs_idx].state;
#else
    GETSWITCH_RECURSIVE_TYPE mask = ((GETSWITCH_RECURSIVE_TYPE)1 << cs_idx);
    if (s_last_switch_used & mask) {
      result = (s_last_switch_value & mask);
    }
    else {
      s_last_switch_used |= mask;
      result = getLogicalSwitch(cs_idx);
      if (result) {
        s_last_switch_value |= mask;
      }
      else {
        s_last_switch_value &= ~mask;
      }
    }      
#endif    
  }

  return swtch > 0 ? result : !result;
}

#if defined(CPUARM)
/**
  @brief Calculates new state of logical switches for mixerCurrentFlightMode
*/
void evalLogicalSwitches(bool isCurrentPhase)
{
  for (unsigned int idx=0; idx<NUM_LOGICAL_SWITCH; idx++) {
    LogicalSwitchContext &context = lswFm[mixerCurrentFlightMode].lsw[idx];
    bool result = getLogicalSwitch(idx);
    if (isCurrentPhase) {
      if (result) {
        if (!context.state) PLAY_LOGICAL_SWITCH_ON(idx);
      }
      else {
        if (context.state) PLAY_LOGICAL_SWITCH_OFF(idx);
      }
    }
    context.state = result;
  }
}
#endif

swarnstate_t switches_states = 0;
int8_t getMovedSwitch()
{
  static tmr10ms_t s_move_last_time = 0;
  int8_t result = 0;

#if defined(PCBTARANIS)
  for (uint8_t i=0; i<NUM_SWITCHES; i++) {
    swarnstate_t mask = (0x03 << (i*2));
    uint8_t prev = (switches_states & mask) >> (i*2);
    uint8_t next = (1024+getValue(MIXSRC_SA+i)) / 1024;
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
  // return delivers 1 to 3 for ID1 to ID3
  // 4..8 for all other switches if changed to true
  // -4..-8 for all other switches if changed to false
  // 9 for Trainer switch if changed to true; Change to false is ignored
  swarnstate_t mask = 0x80;
  for (uint8_t i=NUM_PSWITCH; i>1; i--) {
    bool prev;
    prev = (switches_states & mask);
    // don't use getSwitch here to always get the proper value, even getSwitch manipulates
    bool next = switchState((EnumKeys)(SW_BASE+i-1));
    if (prev != next) {
      if (((i<NUM_PSWITCH) && (i>3)) || next==true) 
        result = next ? i : -i;
      if (i<=3 && result==0) result = 1;
      switches_states ^= mask;
    }
    mask >>= 1;
  }
#endif

  if ((tmr10ms_t)(get_tmr10ms() - s_move_last_time) > 10)
    result = 0;

  s_move_last_time = get_tmr10ms();
  return result;
}

void checkSwitches()
{
#if defined(MODULE_ALWAYS_SEND_PULSES)
  static swarnstate_t last_bad_switches = 0xff;
#else
  swarnstate_t last_bad_switches = 0xff;
#endif
  swarnstate_t states = g_model.switchWarningState;
  
#if defined(PCBTARANIS)
  uint8_t bad_pots = 0, last_bad_pots = 0xff;
#endif

#if !defined(MODULE_ALWAYS_SEND_PULSES)
  while (1) {

#if defined(TELEMETRY_MOD_14051) || defined(PCBTARANIS)
    getADC();
#endif
#endif  // !defined(MODULE_ALWAYS_SEND_PULSES)

    getMovedSwitch();
  
    bool warn = false;
#if defined(PCBTARANIS)
    for (int i=0; i<NUM_SWITCHES; i++) {
      if (SWITCH_WARNING_ALLOWED(i) && !(g_model.switchWarningEnable & (1<<i))) {
        swarnstate_t mask = (0x03 << (i*2));
        if (!((states & mask) == (switches_states & mask))) {
          warn = true;
        }
      }
    }
    uint8_t potMode = g_model.nPotsToWarn >> 6;
    if (potMode) {
      evalFlightModeMixes(e_perout_mode_normal, 0);
      bad_pots = 0;
      for (uint8_t i=0; i<NUM_POTS; i++) {
#if !defined(REVPLUS)
        if (i == POT3-POT1) {
          continue;
        }
#endif
        if (!(g_model.nPotsToWarn & (1 << i)) && (abs(g_model.potPosition[i] - GET_LOWRES_POT_POSITION(i)) > 1)) {
          warn = true;
          bad_pots |= (1<<i);
        }
      }
    }
#else
    for (uint8_t i=0; i<NUM_SWITCHES-1; i++) {
      if (!(g_model.switchWarningEnable & (1<<i))) {
      	if (i == 0) {
      	  if ((states & 0x03) != (switches_states & 0x03)) {
      	    warn = true;
      	  }
      	}
        else if ((states & (1<<(i+1))) != (switches_states & (1<<(i+1)))) {
          warn = true;
        }
      }
    }
#endif

    if (!warn) {
#if defined(MODULE_ALWAYS_SEND_PULSES)
      startupWarningState = STARTUP_WARNING_SWITCHES+1;
      last_bad_switches = 0xff;
#endif
      return;
    }

    // first - display warning
#if defined(PCBTARANIS)
    if ((last_bad_switches != switches_states) || (last_bad_pots != bad_pots)) {
      MESSAGE(STR_SWITCHWARN, NULL, STR_PRESSANYKEYTOSKIP, ((last_bad_switches == 0xff) || (last_bad_pots == 0xff)) ? AU_SWITCH_ALERT : AU_NONE);
      int x = 60, y = 4*FH+3;
      for (int i=0; i<NUM_SWITCHES; ++i) {
        if (SWITCH_WARNING_ALLOWED(i) && !(g_model.switchWarningEnable & (1<<i))) {
          swarnstate_t mask = (0x03 << (i*2));
          uint8_t attr = ((states & mask) == (switches_states & mask)) ? 0 : INVERS;
          if (attr) {
            char c = "\300-\301"[(states & mask) >> (i*2)];
            putsMixerSource(x, y, MIXSRC_FIRST_SWITCH+i, attr);
            lcd_putcAtt(lcdNextPos, y, c, attr);
            x = lcdNextPos + 3;
            if (x >= LCD_W - 4*FW && y == 4*FH+3) {
              y = 6*FH-2;
              x = 60;
            }
          }
        }
      }
      if (potMode) {
        if (y == 4*FH+3) {
          y = 6*FH-2;
          x = 60;
        }
        for (uint8_t i=0; i<NUM_POTS; i++) {
#if !defined(REVPLUS)
          if (i == POT3-POT1) {
            continue;
          }
#endif
          if (!(g_model.nPotsToWarn & (1 << i))) {
            if (abs(g_model.potPosition[i] - GET_LOWRES_POT_POSITION(i)) > 1) {
              lcd_putsiAtt(x, y, STR_VSRCRAW, NUM_STICKS+1+i, INVERS);
              switch (i) {
                case 0:
                case 1:
                case 2: 
                  lcd_putcAtt(lcdNextPos, y, g_model.potPosition[i] > GET_LOWRES_POT_POSITION(i) ? 126 : 127, INVERS);
                  break;
                case 3:
                case 4:
                  lcd_putcAtt(lcdNextPos, y, g_model.potPosition[i] > GET_LOWRES_POT_POSITION(i) ? '\300' : '\301', INVERS);
                  break;
              }
              x = lcdNextPos + 3;
            }

          }
        }
      }
      last_bad_pots = bad_pots;
#else
    if (last_bad_switches != switches_states) {
      MESSAGE(STR_SWITCHWARN, NULL, STR_PRESSANYKEYTOSKIP, last_bad_switches == 0xff ? AU_SWITCH_ALERT : AU_NONE);
      uint8_t x = 2;
      for (uint8_t i=0; i<NUM_SWITCHES-1; i++) {
        uint8_t attr;
        if (i == 0)
          attr = ((states & 0x03) != (switches_states & 0x03)) ? INVERS : 0;
        else
          attr = (states & (1 << (i+1))) == (switches_states & (1 << (i+1))) ? 0 : INVERS;
        if (!(g_model.switchWarningEnable & (1<<i)))
          putsSwitches(x, 5*FH, (i>0?(i+3):(states&0x3)+1), attr);
        x += 3*FW+FW/2;
      }
#endif
      lcdRefresh();
      last_bad_switches = switches_states;
    }

#if defined(MODULE_ALWAYS_SEND_PULSES)
    if (pwrCheck()==e_power_off || keyDown()) {
      startupWarningState = STARTUP_WARNING_SWITCHES+1;
      last_bad_switches = 0xff;
    }
#else
    if (pwrCheck()==e_power_off || keyDown()) return;

    checkBacklight();

    wdt_reset();

    SIMU_SLEEP(1);
  }
#endif    
}

void logicalSwitchesTimerTick()
{
#if defined(CPUARM)
  for (uint8_t fm=0; fm<MAX_FLIGHT_MODES; fm++) {
#endif
    for (uint8_t i=0; i<NUM_LOGICAL_SWITCH; i++) {
      LogicalSwitchData * ls = lswAddress(i);
      if (ls->func == LS_FUNC_TIMER) {
        int16_t *lastValue = &LS_LAST_VALUE(fm, i);
        if (*lastValue == 0 || *lastValue == CS_LAST_VALUE_INIT) {
          *lastValue = -lswTimerValue(ls->v1);
        }
        else if (*lastValue < 0) {
          if (++(*lastValue) == 0)
            *lastValue = lswTimerValue(ls->v2);
        }
        else { // if (*lastValue > 0)
          *lastValue -= 1;
        }
      }
      else if (ls->func == LS_FUNC_STICKY) {
        ls_sticky_struct & lastValue = (ls_sticky_struct &)LS_LAST_VALUE(fm, i);
        bool before = lastValue.last & 0x01;
        if (lastValue.state) {
          bool now = getSwitch(ls->v2);
          if (now != before) {
            lastValue.last ^= 1;
            if (!before) {
              lastValue.state = 0;
            }
          }
        }
        else {
          bool now = getSwitch(ls->v1);
          if (before != now) {
            lastValue.last ^= 1;
            if (!before) {
              lastValue.state = 1;
            }
          }
        }
      }
#if defined(CPUARM)
      else if (ls->func == LS_FUNC_EDGE) {
        ls_stay_struct & lastValue = (ls_stay_struct &)LS_LAST_VALUE(fm, i);
        lastValue.state = false;
        bool state = getSwitch(ls->v1);
        if (state) {
          if (ls->v3 == -1 && lastValue.duration == lswTimerValue(ls->v2))
            lastValue.state = true;
          if (lastValue.duration < 1000)
            lastValue.duration++;
        }
        else {
          if (lastValue.duration > lswTimerValue(ls->v2) && (ls->v3 == 0 || lastValue.duration <= lswTimerValue(ls->v2+ls->v3)))
            lastValue.state = true;
          lastValue.duration = 0;
        }
      }
      
      // decrement delay/duration timer
      LogicalSwitchContext &context = lswFm[fm].lsw[i];
      if (context.timer) {
        context.timer--;
      }
#endif
    }
#if defined(CPUARM)
  }
#endif
}

LogicalSwitchData *lswAddress(uint8_t idx)
{
  return &g_model.logicalSw[idx];
}

uint8_t lswFamily(uint8_t func)
{
  if (func <= LS_FUNC_ANEG)
    return LS_FAMILY_OFS;
  else if (func <= LS_FUNC_XOR)
    return LS_FAMILY_BOOL;
#if defined(CPUARM)
  else if (func == LS_FUNC_EDGE)
    return LS_FAMILY_EDGE;
#endif
  else if (func <= LS_FUNC_LESS)
    return LS_FAMILY_COMP;
  else if (func <= LS_FUNC_ADIFFEGREATER)
    return LS_FAMILY_DIFF;
  else
    return LS_FAMILY_TIMER+func-LS_FUNC_TIMER;
}

int16_t lswTimerValue(delayval_t val)
{
  return (val < -109 ? 129+val : (val < 7 ? (113+val)*5 : (53+val)*10));
}

void logicalSwitchesReset()
{
#if defined(CPUARM)
  memset(lswFm, 0, sizeof(lswFm));
#else
  s_last_switch_value = 0;
#endif

#if defined(CPUARM)
  for (uint8_t fm=0; fm<MAX_FLIGHT_MODES; fm++) {
#endif
    for (uint8_t i=0; i<NUM_LOGICAL_SWITCH; i++) {
      LS_LAST_VALUE(fm, i) = CS_LAST_VALUE_INIT;
    }
#if defined(CPUARM)
  }
#endif
}

getvalue_t convertLswTelemValue(LogicalSwitchData * ls)
{
  getvalue_t val;
#if defined(CPUARM)
  val = convert16bitsTelemValue(ls->v1 - MIXSRC_FIRST_TELEM + 1, ls->v2);
#else
  if (lswFamily(ls->func)==LS_FAMILY_OFS)
    val = convert8bitsTelemValue(ls->v1 - MIXSRC_FIRST_TELEM + 1, 128+ls->v2);
  else
    val = convert8bitsTelemValue(ls->v1 - MIXSRC_FIRST_TELEM + 1, 128+ls->v2) - convert8bitsTelemValue(ls->v1 - MIXSRC_FIRST_TELEM + 1, 128);
#endif
  return val;
}

#if defined(CPUARM)
void logicalSwitchesCopyState(uint8_t src, uint8_t dst)
{
  lswFm[dst] = lswFm[src];
}
#endif
