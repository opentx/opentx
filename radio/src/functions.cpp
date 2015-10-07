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
#include "timers.h"

CustomFunctionsContext modelFunctionsContext = { 0 };

#if defined(CPUARM)
CustomFunctionsContext globalFunctionsContext = { 0 };
#endif


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

#if defined(VOICE)
PLAY_FUNCTION(playValue, source_t idx)
{
  if (IS_FAI_FORBIDDEN(idx))
    return;

  if (idx == MIXSRC_NONE)
    return;

  getvalue_t val = getValue(idx);

#if defined(CPUARM)
  if (idx >= MIXSRC_FIRST_TELEM) {
    TelemetrySensor & telemetrySensor = g_model.telemetrySensors[(idx-MIXSRC_FIRST_TELEM) / 3];
    uint8_t attr = 0;
    if (telemetrySensor.prec > 0) {
      if (telemetrySensor.prec == 2) {
        if (val >= 5000) {
          val = div100_and_round(val);
        }
        else {
          val = div10_and_round(val);
          attr = PREC1;
        }
      }
      else {
        if (val >= 500) {
          val = div10_and_round(val);
        }
        else {
          attr = PREC1;
        }
      }
    }
    PLAY_NUMBER(val, telemetrySensor.unit == UNIT_CELLS ? UNIT_VOLTS : telemetrySensor.unit, attr);
  }
  else if (idx >= MIXSRC_FIRST_TIMER && idx <= MIXSRC_LAST_TIMER) {
    PLAY_DURATION(val, 0);
  }
  else if (idx == MIXSRC_TX_TIME) {
    PLAY_DURATION(val*60, PLAY_TIME);
  }
  else if (idx == MIXSRC_TX_VOLTAGE) {
    PLAY_NUMBER(val, UNIT_VOLTS, PREC1);
  }
  else {
    if (idx <= MIXSRC_LAST_CH) {
      val = calcRESXto100(val);
    }
    PLAY_NUMBER(val, 0, 0);
  }
#else
  switch (idx) {
    case MIXSRC_FIRST_TELEM+TELEM_TX_VOLTAGE-1:
      PLAY_NUMBER(val, 1+UNIT_VOLTS, PREC1);
      break;
    case MIXSRC_FIRST_TELEM+TELEM_TIMER1-1:
    case MIXSRC_FIRST_TELEM+TELEM_TIMER2-1:
      PLAY_DURATION(val, 0);
      break;
#if defined(FRSKY)
    case MIXSRC_FIRST_TELEM+TELEM_RSSI_TX-1:
    case MIXSRC_FIRST_TELEM+TELEM_RSSI_RX-1:
      PLAY_NUMBER(val, 1+UNIT_DB, 0);
      break;
    case MIXSRC_FIRST_TELEM+TELEM_MIN_A1-1:
    case MIXSRC_FIRST_TELEM+TELEM_MIN_A2-1:
      idx -= TELEM_MIN_A1-TELEM_A1;
      // no break
    case MIXSRC_FIRST_TELEM+TELEM_A1-1:
    case MIXSRC_FIRST_TELEM+TELEM_A2-1:
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
#endif
}
#endif

#if defined(CPUARM)
void playCustomFunctionFile(const CustomFunctionData *sd, uint8_t id)
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

#if defined(CPUARM)
#define VOLUME_HYSTERESIS 10            // how much must a input value change to actually be considered for new volume setting
getvalue_t requiredSpeakerVolumeRawLast = 1024 + 1; //initial value must be outside normal range
#endif

#if defined(CPUARM)
void evalFunctions(const CustomFunctionData * functions, CustomFunctionsContext & functionsContext)
#else
#define functions g_model.customFn
#define functionsContext modelFunctionsContext
void evalFunctions()
#endif
{
  MASK_FUNC_TYPE newActiveFunctions  = 0;
  MASK_CFN_TYPE  newActiveSwitches = 0;

#if defined(CPUARM)
  uint8_t playFirstIndex = (functions == g_model.customFn ? 1 : 1+NUM_CFN);
  #define PLAY_INDEX   (i+playFirstIndex)
#else
  #define PLAY_INDEX   (i+1)
#endif

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
    const CustomFunctionData * cfn = &functions[i];
    int8_t swtch = CFN_SWITCH(cfn);
    if (swtch) {
      MASK_CFN_TYPE switch_mask = ((MASK_CFN_TYPE)1 << i);

#if defined(CPUARM)
      bool active = getSwitch(swtch, IS_PLAY_FUNC(CFN_FUNC(cfn)) ? GETSWITCH_MIDPOS_DELAY : 0);
#else
      bool active = getSwitch(swtch);
#endif

      if (HAS_ENABLE_PARAM(CFN_FUNC(cfn))) {
        active &= (bool)CFN_ACTIVE(cfn);
      }

      if (active || IS_PLAY_BOTH_FUNC(CFN_FUNC(cfn))) {

        switch (CFN_FUNC(cfn)) {

#if defined(OVERRIDE_CHANNEL_FUNCTION)
          case FUNC_OVERRIDE_CHANNEL:
            safetyCh[CFN_CH_INDEX(cfn)] = CFN_PARAM(cfn);
            break;
#endif

          case FUNC_TRAINER:
          {
            uint8_t mask = 0x0f;
            if (CFN_CH_INDEX(cfn) > 0) {
              mask = (1<<(CFN_CH_INDEX(cfn)-1));
            }
            newActiveFunctions |= mask;
            break;
          }

          case FUNC_INSTANT_TRIM:
            newActiveFunctions |= (1 << FUNCTION_INSTANT_TRIM);
            if (!isFunctionActive(FUNCTION_INSTANT_TRIM)) {
#if defined(GUI)
              if (g_menuStack[0] == menuMainView
#if defined(FRSKY) && !defined(PCBFLAMENCO) && !defined(PCBHORUS)
                || g_menuStack[0] == menuTelemetryFrsky
#endif
#if defined(PCBTARANIS)
                || g_menuStack[0] == menuMainViewChannelsMonitor
                || g_menuStack[0] == menuChannelsView
#endif
              )
#endif
              {
                instantTrim();
              }
            }
            break;

          case FUNC_RESET:
            switch (CFN_PARAM(cfn)) {
              case FUNC_RESET_TIMER1:
              case FUNC_RESET_TIMER2:
#if defined(CPUARM)
              case FUNC_RESET_TIMER3:
#endif
                timerReset(CFN_PARAM(cfn));
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
                g_rotenc[CFN_PARAM(cfn)-FUNC_RESET_ROTENC1] = 0;
                break;
#endif
            }
#if defined(CPUARM)
            if (CFN_PARAM(cfn)>=FUNC_RESET_PARAM_FIRST_TELEM) {
              TelemetryItem * telemetryItem = & telemetryItems[CFN_PARAM(cfn)-FUNC_RESET_PARAM_FIRST_TELEM];
              telemetryItem->clear();
            }
#endif
            break;

#if defined(CPUARM)
          case FUNC_SET_TIMER:
          {
            timerSet(CFN_TIMER_INDEX(cfn), CFN_PARAM(cfn));
            break;
          }
#endif

#if defined(CPUARM)
          case FUNC_SET_FAILSAFE:
          {
            unsigned int moduleIndex = CFN_PARAM(cfn);
            if (moduleIndex < NUM_MODULES) {
              for (int ch=0; ch<NUM_CHNOUT; ch++) {
                if (ch < g_model.moduleData[moduleIndex].channelsStart || ch >= NUM_CHANNELS(moduleIndex) + g_model.moduleData[moduleIndex].channelsStart) {
                  g_model.moduleData[moduleIndex].failsafeChannels[ch] = 0;
                }
                else if (g_model.moduleData[moduleIndex].failsafeChannels[ch] < FAILSAFE_CHANNEL_HOLD) {
                  g_model.moduleData[moduleIndex].failsafeChannels[ch] = channelOutputs[ch];
                }
              }
            }
          }
#endif

#if defined(DANGEROUS_MODULE_FUNCTIONS)
          case FUNC_RANGECHECK:
          case FUNC_BIND:
          {
            unsigned int moduleIndex = CFN_PARAM(cfn);
            if (moduleIndex < NUM_MODULES) {
              moduleFlag[moduleIndex] = 1 + CFN_FUNC(cfn) - FUNC_RANGECHECK;
            }
            break;
          }
#endif

#if defined(GVARS)
          case FUNC_ADJUST_GVAR:
            if (CFN_GVAR_MODE(cfn) == 0) {
              SET_GVAR(CFN_GVAR_INDEX(cfn), CFN_PARAM(cfn), mixerCurrentFlightMode);
            }
            else if (CFN_GVAR_MODE(cfn) == 2) {
              SET_GVAR(CFN_GVAR_INDEX(cfn), GVAR_VALUE(CFN_PARAM(cfn), mixerCurrentFlightMode), mixerCurrentFlightMode);
            }
            else if (CFN_GVAR_MODE(cfn) == 3) {
              if (!(functionsContext.activeSwitches & switch_mask)) {
                SET_GVAR(CFN_GVAR_INDEX(cfn), GVAR_VALUE(CFN_GVAR_INDEX(cfn), getGVarFlightPhase(mixerCurrentFlightMode, CFN_GVAR_INDEX(cfn))) + (CFN_PARAM(cfn) ? +1 : -1), mixerCurrentFlightMode);
              }
            }
            else if (CFN_PARAM(cfn) >= MIXSRC_TrimRud && CFN_PARAM(cfn) <= MIXSRC_TrimAil) {
              trimGvar[CFN_PARAM(cfn)-MIXSRC_TrimRud] = CFN_GVAR_INDEX(cfn);
            }
#if defined(ROTARY_ENCODERS)
            else if (CFN_PARAM(cfn) >= MIXSRC_REa && CFN_PARAM(cfn) < MIXSRC_TrimRud) {
              int8_t scroll = rePreviousValues[CFN_PARAM(cfn)-MIXSRC_REa] - (g_rotenc[CFN_PARAM(cfn)-MIXSRC_REa] / ROTARY_ENCODER_GRANULARITY);
              if (scroll) {
                SET_GVAR(CFN_GVAR_INDEX(cfn), GVAR_VALUE(CFN_GVAR_INDEX(cfn), getGVarFlightPhase(mixerCurrentFlightMode, CFN_GVAR_INDEX(cfn))) + scroll, mixerCurrentFlightMode);
              }
            }
#endif
            else {
              SET_GVAR(CFN_GVAR_INDEX(cfn), calcRESXto100(getValue(CFN_PARAM(cfn))), mixerCurrentFlightMode);
            }
            break;
#endif

#if defined(CPUARM) && defined(SDCARD)
          case FUNC_VOLUME:
          {
            getvalue_t raw = getValue(CFN_PARAM(cfn));
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
            uint8_t repeatParam = CFN_PLAY_REPEAT(cfn);
            if (!IS_SILENCE_PERIOD_ELAPSED() && repeatParam == CFN_PLAY_REPEAT_NOSTART) {
              functionsContext.lastFunctionTime[i] = tmr10ms;
            }
            if (!functionsContext.lastFunctionTime[i] || (repeatParam && repeatParam!=CFN_PLAY_REPEAT_NOSTART && (signed)(tmr10ms-functionsContext.lastFunctionTime[i])>=100*repeatParam)) {
              if (!IS_PLAYING(PLAY_INDEX)) {
                functionsContext.lastFunctionTime[i] = tmr10ms;
                if (CFN_FUNC(cfn) == FUNC_PLAY_SOUND) {
                  AUDIO_PLAY(AU_FRSKY_FIRST+CFN_PARAM(cfn));
                }
                else if (CFN_FUNC(cfn) == FUNC_PLAY_VALUE) {
                  PLAY_VALUE(CFN_PARAM(cfn), PLAY_INDEX);
                }
#if defined(HAPTIC)
                else if (CFN_FUNC(cfn) == FUNC_HAPTIC) {
                  haptic.event(AU_FRSKY_LAST+CFN_PARAM(cfn));
                }
#endif
                else {
                  playCustomFunctionFile(cfn, PLAY_INDEX);
                }
              }
            }
            break;
          }

          case FUNC_BACKGND_MUSIC:
            if (!(newActiveFunctions & (1 << FUNCTION_BACKGND_MUSIC))) {
              newActiveFunctions |= (1 << FUNCTION_BACKGND_MUSIC);
              if (!IS_PLAYING(PLAY_INDEX)) {
                playCustomFunctionFile(cfn, PLAY_INDEX);
              }
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
            uint8_t repeatParam = CFN_PLAY_REPEAT(cfn);
            if (!functionsContext.lastFunctionTime[i] || (CFN_FUNC(cfn)==FUNC_PLAY_BOTH && active!=(bool)(functionsContext.activeSwitches&switch_mask)) || (repeatParam && (signed)(tmr10ms-functionsContext.lastFunctionTime[i])>=1000*repeatParam)) {
              functionsContext.lastFunctionTime[i] = tmr10ms;
              uint8_t param = CFN_PARAM(cfn);
              if (CFN_FUNC(cfn) == FUNC_PLAY_SOUND) {
                AUDIO_PLAY(AU_FRSKY_FIRST+param);
              }
              else if (CFN_FUNC(cfn) == FUNC_PLAY_VALUE) {
                PLAY_VALUE(param, PLAY_INDEX);
              }
              else {
#if defined(GVARS)
                if (CFN_FUNC(cfn) == FUNC_PLAY_TRACK && param > 250)
                  param = GVAR_VALUE(param-251, getGVarFlightPhase(mixerCurrentFlightMode, param-251));
#endif
                PUSH_CUSTOM_PROMPT(active ? param : param+1, PLAY_INDEX);
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
            uint8_t repeatParam = CFN_PLAY_REPEAT(cfn);
            if (!functionsContext.lastFunctionTime[i] || (repeatParam && (signed)(tmr10ms-functionsContext.lastFunctionTime[i])>=1000*repeatParam)) {
              functionsContext.lastFunctionTime[i] = tmr10ms;
              AUDIO_PLAY(AU_FRSKY_FIRST+CFN_PARAM(cfn));
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
            uint8_t repeatParam = CFN_PLAY_REPEAT(cfn);
            if (!functionsContext.lastFunctionTime[i] || (repeatParam && (signed)(tmr10ms-functionsContext.lastFunctionTime[i])>=1000*repeatParam)) {
              functionsContext.lastFunctionTime[i] = tmr10ms;
              haptic.event(AU_FRSKY_LAST+CFN_PARAM(cfn));
            }
            break;
          }
#endif

#if defined(SDCARD)
          case FUNC_LOGS:
            if (CFN_PARAM(cfn)) {
              newActiveFunctions |= (1 << FUNCTION_LOGS);
              logDelay = CFN_PARAM(cfn);
            }
            break;
#endif

          case FUNC_BACKLIGHT:
            newActiveFunctions |= (1 << FUNCTION_BACKLIGHT);
            break;

#if defined(PCBTARANIS)
          case FUNC_SCREENSHOT:
            if (!(functionsContext.activeSwitches & switch_mask)) {
              requestScreenshot = true;
            }
            break;
#endif

#if defined(DEBUG)
          case FUNC_TEST:
            testFunc();
            break;
#endif
        }

        newActiveSwitches |= switch_mask;
      }
      else {
        functionsContext.lastFunctionTime[i] = 0;
#if defined(DANGEROUS_MODULE_FUNCTIONS)
        if (functionsContext.activeSwitches & switch_mask) {
          switch (CFN_FUNC(cfn)) {
            case FUNC_RANGECHECK:
            case FUNC_BIND:
            {
              unsigned int moduleIndex = CFN_PARAM(cfn);
              if (moduleIndex < NUM_MODULES) {
                moduleFlag[moduleIndex] = 0;
              }
              break;
            }
          }
        }
#endif
      }
    }
  }

  functionsContext.activeSwitches   = newActiveSwitches;
  functionsContext.activeFunctions  = newActiveFunctions;

#if defined(ROTARY_ENCODERS) && defined(GVARS)
  for (uint8_t i=0; i<ROTARY_ENCODERS; i++) {
    rePreviousValues[i] = (g_rotenc[i] / ROTARY_ENCODER_GRANULARITY);
  }
#endif
}

#if !defined(CPUARM)
#undef functions
#undef functionsContext
#endif
