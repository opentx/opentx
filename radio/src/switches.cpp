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

PACK(typedef struct {
  uint8_t state:1;
#if defined(CPUARM)
  uint8_t internalState:1;
  uint32_t delay;
  uint32_t duration;
#endif
  int16_t lastValue;
}) lsw_struct;

PACK(typedef struct {
  lsw_struct ls[NUM_LOGICAL_SWITCH];
}) lsw_array;

lsw_array lswFm[MAX_FLIGHT_MODES];

#if defined(PCBTARANIS)
tmr10ms_t switchesMidposStart[6] = { 0 };
uint32_t  switchesPos = 0;
tmr10ms_t potsLastposStart[NUM_XPOTS];
uint8_t   potsPos[NUM_XPOTS];

uint32_t check2PosSwitchPosition(EnumKeys sw)
{
  uint32_t result;
  uint32_t index;

  if (switchState(sw))
    index = sw - SW_SA0;
  else
    index = sw - SW_SA0 + 1;

  result = (1 << index);

  if (!(switchesPos & result)) {
    PLAY_SWITCH_MOVED(index);
  }

  return result;
}

#define DELAY_SWITCH_3POS    15/*150ms*/
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
  else if (startup || (switchesPos & (1 << (sw - SW_SA0 + 1))) || (switchesMidposStart[idx] && (tmr10ms_t)(get_tmr10ms() - switchesMidposStart[idx]) > DELAY_SWITCH_3POS)) {
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
  uint32_t newPos = 0;
  CHECK_3POS(0, SW_SA);
  CHECK_3POS(1, SW_SB);
  CHECK_3POS(2, SW_SC);
  CHECK_3POS(3, SW_SD);
  CHECK_3POS(4, SW_SE);
  CHECK_2POS(SW_SF);
  CHECK_3POS(5, SW_SG);
  CHECK_2POS(SW_SH);
  switchesPos = newPos;

  for (int i=0; i<NUM_XPOTS; i++) {
    if (g_eeGeneral.potsType & (1 << i)) {
      StepsCalibData * calib = (StepsCalibData *) &g_eeGeneral.calib[POT1+i];
      if (calib->count>0 && calib->count<XPOTS_MULTIPOS_COUNT) {
        uint8_t pos = anaIn(POT1+i) / (2*RESX/calib->count);
        uint8_t previousPos = potsPos[i] >> 4;
        uint8_t previousStoredPos = potsPos[i] & 0x0F;
        if (pos != previousPos) {
          potsLastposStart[i] = get_tmr10ms();
          potsPos[i] = (pos << 4) | previousStoredPos;
        }
        else if (startup || (tmr10ms_t)(get_tmr10ms() - potsLastposStart[i]) > DELAY_SWITCH_3POS) {
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
#else
#define SWITCH_POSITION(idx) switchState((EnumKeys)(SW_BASE+(idx)))
#endif

bool getSwitch(int8_t swtch)
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
    result = SWITCH_POSITION(cs_idx-SWSRC_FIRST_SWITCH);
#if defined(MODULE_ALWAYS_SEND_PULSES)
    if (startupWarningState < STARTUP_WARNING_DONE) {
      // if throttle or switch warning is currently active, ignore actual stick position and use wanted values
      if (cs_idx <= 3) {
        if (!(g_model.nSwToWarn&1)) {     // ID1 to ID3 is just one bit in nSwToWarn
          result = (cs_idx)==((g_model.switchWarningStates&3)+1);  // overwrite result with desired value
        }
      }
      else if (!(g_model.nSwToWarn & (1<<(cs_idx-3)))) {
        // current switch should not be ignored for warning
        result = g_model.switchWarningStates & (1<<(cs_idx-2)); // overwrite result with desired value
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
    result = (idx == s_current_mixer_flight_mode);
  }
#endif
  else {
    cs_idx -= SWSRC_FIRST_LOGICAL_SWITCH;
    result = lswFm[s_current_mixer_flight_mode].ls[cs_idx].state;
  }

  return swtch > 0 ? result : !result;
}

/**
  @brief Calculates new state of logical switches for s_current_mixer_flight_mode
*/
void evalLogicalSwitches(uint8_t mode)
{
  for(unsigned int cs_idx=0; cs_idx<NUM_LOGICAL_SWITCH; cs_idx++) {

    bool result = false;

    LogicalSwitchData * ls = lswAddress(cs_idx);
    lsw_struct * lsd = &lswFm[s_current_mixer_flight_mode].ls[cs_idx];

#if defined(CPUARM)
    int8_t s = ls->andsw;
#else
    uint8_t s = ls->andsw;
    if (s > SWSRC_LAST_SWITCH) {
      s += SWSRC_SW1-SWSRC_LAST_SWITCH-1;
    }
#endif
    if (ls->func == LS_FUNC_NONE || (s && !getSwitch(s))) {
      lsd->lastValue = CS_LAST_VALUE_INIT;
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
      result = (lsd->lastValue <= 0);
    }
    else if (s == LS_FAMILY_STICKY) {
      result = (lsd->lastValue & (1<<0));
    }
#if defined(CPUARM)
    else if (s == LS_FAMILY_STAY) {
      result = (lsd->lastValue & (1<<0));
    }
#endif
    else {
      getvalue_t x = getValue(ls->v1);
      getvalue_t y;
      if (s == LS_FAMILY_COMP) {
        y = getValue(ls->v2);

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
          if ((!TELEMETRY_STREAMING() && v1 >= MIXSRC_FIRST_TELEM+TELEM_FIRST_STREAMED_VALUE-1) || IS_FAI_FORBIDDEN(v1-1)) {
            lsd->state = false;
            continue;
          }


          y = convertLswTelemValue(ls);

#if defined(FRSKY_HUB) && defined(GAUGES)
          if (s == LS_FAMILY_OFS) {
            uint8_t idx = v1-MIXSRC_FIRST_TELEM+1-TELEM_ALT;
            if (idx < THLD_MAX) {
              // Fill the threshold array
              barsThresholds[idx] = 128 + ls->v2;
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
            if (lsd->lastValue == CS_LAST_VALUE_INIT)
              lsd->lastValue = x;
            int16_t diff = x - lsd->lastValue;
            if (ls->func == LS_FUNC_DIFFEGREATER)
              result = (y >= 0 ? (diff >= y) : (diff <= y));
            else
              result = (abs(diff) >= y);
            if (result)
              lsd->lastValue = x;
            break;
          }
        }
      }
    }

#if defined(CPUARM)
    if (ls->delay) {
      if (result) {
        if (lsd->delay > get_tmr10ms())
          result = false;
      }
      else {
        lsd->delay = get_tmr10ms() + (ls->delay*10);
      }
    }

    if (ls->duration) {
      if (result && !lsd->internalState) {
        lsd->duration = get_tmr10ms() + (ls->duration*10);
      }

      lsd->internalState = result;
      result = false;

      if (lsd->duration > get_tmr10ms()) {
        result = true;
      }
    }
#endif

    if (result) {
      if (!lsd->state && mode == e_perout_mode_normal) PLAY_LOGICAL_SWITCH_ON(cs_idx);
    }
    else {
      if (lsd->state && mode == e_perout_mode_normal) PLAY_LOGICAL_SWITCH_OFF(cs_idx);
    }
    lsd->state = result;
  }
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
  swstate_t mask = 0x80;
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
  static swstate_t last_bad_switches = 0xff;
#else
  swstate_t last_bad_switches = 0xff;
#endif
  swstate_t states = g_model.switchWarningStates;
  
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
    for (uint8_t i=0; i<NUM_SWITCHES-1; i++) {
      if (!(g_model.nSwToWarn & (1<<i))) {
        swstate_t mask = (0x03 << (i*2));
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
        if (!(g_model.nPotsToWarn & (1 << i)) && (abs(g_model.potPosition[i] - (getValue(MIXSRC_FIRST_POT+i) >> 4)) > 1)) {
          warn = true;
          bad_pots  |= (1<<i);
        }
      }
    }
#else
    for (uint8_t i=0; i<NUM_SWITCHES-1; i++) {
      if (!(g_model.nSwToWarn & (1<<i))) {
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
      for (uint8_t i=0; i<NUM_SWITCHES-1; i++) {
        if (!(g_model.nSwToWarn & (1<<i))) {
          swstate_t mask = (0x03 << (i*2));
          uint8_t attr = ((states & mask) == (switches_states & mask)) ? 0 : INVERS;
          char c = "\300-\301"[(states & mask) >> (i*2)];
          lcd_putcAtt(60+i*(2*FW+FW/2), 4*FH+3, 'A'+i, attr);
          lcd_putcAtt(60+i*(2*FW+FW/2)+FW, 4*FH+3, c, attr);
        }
      }
      if (potMode) {
        for (uint8_t i=0; i<NUM_POTS; i++) {
          if (!(g_model.nPotsToWarn & (1 << i))) {
            uint8_t flags = 0;
            if (abs(g_model.potPosition[i] - (getValue(MIXSRC_FIRST_POT+i) >> 4)) > 1) {
            	switch (i) {
                case 0:
                case 1:
                case 2: 
                  lcd_putc(60+i*(5*FW)+2*FW+2, 6*FH-2, g_model.potPosition[i] > (getValue(MIXSRC_FIRST_POT+i) >> 4) ? 126 : 127);
                  break;
                case 3:
                case 4:
                  lcd_putc(60+i*(5*FW)+2*FW+2, 6*FH-2, g_model.potPosition[i] > (getValue(MIXSRC_FIRST_POT+i) >> 4) ? '\300' : '\301');
                  break;
              }
              flags = INVERS;
            }
            lcd_putsiAtt(60+i*(5*FW), 6*FH-2, STR_VSRCRAW, NUM_STICKS+1+i, flags);
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
        if (!(g_model.nSwToWarn & (1<<i)))
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

void logicalSwitchesTimerTick() {
  for (uint8_t fm=0; fm<MAX_FLIGHT_MODES; fm++) {
    for (uint8_t i=0; i<NUM_LOGICAL_SWITCH; i++) {
      LogicalSwitchData * ls = lswAddress(i);
      lsw_struct * lsd = &lswFm[fm].ls[i];
      if (ls->func == LS_FUNC_TIMER) {
        int16_t *lastValue = &lsd->lastValue;
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
        PACK(typedef struct {
          uint8_t state;
          uint8_t last;
        }) ls_sticky_struct;
        ls_sticky_struct & lastValue = (ls_sticky_struct &)lsd->lastValue;;
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
      else if (ls->func == LS_FUNC_STAY) {
        PACK(typedef struct {
          uint16_t state:1;
          uint16_t duration:15;
        }) ls_stay_struct;

        ls_stay_struct & lastValue = (ls_stay_struct &)lsd->lastValue;;
        lastValue.state = false;
        bool state = getSwitch(ls->v1);
        if (state) {
          if (ls->v3 == 0 && lastValue.duration == lswTimerValue(ls->v2))
            lastValue.state = true;
          if (lastValue.duration < 1000)
            lastValue.duration++;
        }
        else {
          if (lastValue.duration > lswTimerValue(ls->v2) && lastValue.duration <= lswTimerValue(ls->v2+ls->v3))
            lastValue.state = true;
          lastValue.duration = 0;
        }
      }
  #endif
    }
  }
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
  else if (func == LS_FUNC_STAY)
    return LS_FAMILY_STAY;
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

void logicalSwitchesReset() {
  for (uint8_t fm=0; fm<MAX_FLIGHT_MODES; fm++) {
    lsw_array * lsa = &lswFm[fm];
    for (uint8_t i=0; i<NUM_LOGICAL_SWITCH; i++) {
      lsa->ls[i].lastValue = CS_LAST_VALUE_INIT;
      //todo other values
      lsa->ls[i].state = false;
    }
  }
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

void logicalSwitchesCopyState(uint8_t oldPhase, uint8_t newPhase) {
  for (uint8_t i=0; i<NUM_LOGICAL_SWITCH; i++) {
    lswFm[newPhase].ls[i].state = lswFm[oldPhase].ls[i].state;
  }
}
