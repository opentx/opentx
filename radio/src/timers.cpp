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

#if TIMERS > MAX_TIMERS
#error "Timers cannot exceed " .. MAX_TIMERS
#endif

TimerState timersStates[TIMERS] = { { 0 } };

void timerReset(uint8_t idx)
{
  TimerState & timerState = timersStates[idx];
  timerState.state = TMR_OFF; // is changed to RUNNING dep from mode
  timerState.val = g_model.timers[idx].start;
  timerState.val_10ms = 0 ;
}

#if defined(CPUARM)
void timerSet(int idx, int16_t val)
{
  TimerState & timerState = timersStates[idx];
  timerState.state = TMR_OFF; // is changed to RUNNING dep from mode
  timerState.val = val;
  timerState.val_10ms = 0 ;
}
#endif // #if defined(CPUARM)

#if defined(CPUARM) || defined(CPUM2560)
void restoreTimers()
{
  for (uint8_t i=0; i<TIMERS; i++) {
    if (g_model.timers[i].persistent) {
      timersStates[i].val = g_model.timers[i].value;
    }
  }
}

void saveTimers()
{
  for (uint8_t i=0; i<TIMERS; i++) {
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
#endif  // #if defined(CPUARM) || defined(CPUM2560)

#if defined(ACCURAT_THROTTLE_TIMER)
  #define THR_TRG_TRESHOLD    13      // approximately 10% full throttle
#else
  #define THR_TRG_TRESHOLD    3       // approximately 10% full throttle
#endif

void evalTimers(int16_t throttle, uint8_t tick10ms)
{
  for (uint8_t i=0; i<TIMERS; i++) {
    int8_t tm = g_model.timers[i].mode;
    uint16_t tv = g_model.timers[i].start;
    TimerState * timerState = &timersStates[i];

    if (tm) {
      if ((timerState->state == TMR_OFF) && (tm != TMRMODE_THR_TRG)) {
        timerState->state = TMR_RUNNING;
        timerState->cnt = 0;
        timerState->sum = 0;
      }

      if (tm == TMRMODE_THR_REL) {
        timerState->cnt++;
        timerState->sum += throttle;
      }

      if ((timerState->val_10ms += tick10ms) >= 100) {
        if (timerState->val == TIMER_MAX) break;
        if (timerState->val == TIMER_MIN) break;

        timerState->val_10ms -= 100 ;
        int16_t newTimerVal = timerState->val;
        if (tv) newTimerVal = tv - newTimerVal;

        if (tm == TMRMODE_ABS) {
          newTimerVal++;
        }
        else if (tm == TMRMODE_THR) {
          if (throttle) newTimerVal++;
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
          // we can't rely on (throttle || newTimerVal > 0) as a detection if timer should be running
          // because having persistent timer brakes this rule
          if ((throttle > THR_TRG_TRESHOLD) && timerState->state == TMR_OFF) {
            timerState->state = TMR_RUNNING;  // start timer running
            timerState->cnt = 0;
            timerState->sum = 0;
            TRACE("Timer[%d] THr triggered", i);
          }
          if (timerState->state != TMR_OFF) newTimerVal++;
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
              TRACE("Timer[%d] negative", i);
            }
            break;
          case TMR_NEGATIVE:
            if (newTimerVal >= (int16_t)tv + MAX_ALERT_TIME) {
              timerState->state = TMR_STOPPED;
              TRACE("Timer[%d] stopped state at %d", i, newTimerVal);
            }
            break;
        }

        if (tv) newTimerVal = tv - newTimerVal; // if counting backwards - display backwards

        if (newTimerVal != timerState->val) {
          timerState->val = newTimerVal;
          if (timerState->state == TMR_RUNNING) {
            if (g_model.timers[i].countdownBeep && g_model.timers[i].start) {
              if (newTimerVal==30) { 
                AUDIO_TIMER_30(); 
                TRACE("Timer[%d] 30s announcement", i); 
              }
              if (newTimerVal==20) { 
                AUDIO_TIMER_20();
                TRACE("Timer[%d] 20s announcement", i); 
              }
              if (newTimerVal<=10) {
                AUDIO_TIMER_LT10(g_model.timers[i].countdownBeep, newTimerVal);
                TRACE("Timer[%d] %ds announcement", i, newTimerVal);
              }
            }
            if (g_model.timers[i].minuteBeep && (newTimerVal % 60)==0) {
              AUDIO_TIMER_MINUTE(newTimerVal);
              TRACE("Timer[%d] %d minute announcement", i, newTimerVal/60);
            }
          }
        }
      }
    }
  }
}
