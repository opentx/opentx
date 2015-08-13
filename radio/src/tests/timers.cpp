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

#include "gtests.h"
#include "opentx.h"
#include "timers.h"

/*
struct TimerState {
  uint16_t cnt;
  uint16_t sum;
  uint8_t  state;
  int16_t  val;
  uint8_t  val_10ms;
};

PACK(typedef struct t_TimerData {
  int8_t   mode;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint16_t start;
  uint8_t  countdownBeep:2;
  uint8_t  minuteBeep:1;
  uint8_t  persistent:2; // 0 off, 1 flight, 2 manual reset
  uint8_t  spare:3;
  uint16_t value;
}) TimerData;

enum TimerModes {
  TMRMODE_NONE,
  TMRMODE_ABS,
  TMRMODE_THR,
  TMRMODE_THR_REL,
  TMRMODE_THR_TRG,
  TMRMODE_COUNT
};

enum CountDownModes {
  COUNTDOWN_SILENT,
  COUNTDOWN_BEEPS,
  COUNTDOWN_VOICE
};

*/

#if !defined(CPUARM)
#undef timerSet
void timerSet(int idx, int16_t val)
{
  TimerState & timerState = timersStates[idx];
  timerState.state = TMR_OFF; // is changed to RUNNING dep from mode
  timerState.val = val;
  timerState.val_10ms = 0 ;
}
#endif // #if !defined(CPUARM)

#if defined(ACCURAT_THROTTLE_TIMER)
  #define THR_100    128      // approximately 10% full throttle
  #define THR_50      64      // approximately 10% full throttle
  #define THR_10      13      // approximately 10% full throttle
#else
  #define THR_100     32      // approximately 10% full throttle
  #define THR_50      16      // approximately 10% full throttle
  #define THR_10       3      // approximately 10% full throttle
#endif


#define TEST_AB_EQUAL(a, b) if (a != b) { return ::testing::AssertionFailure() << \
                            #a "= " << (uint32_t)a << ", " << #b "= " << (uint32_t)b; };

void initModelTimer(uint32_t idx, uint8_t mode, int16_t start = 0)
{
  g_model.timers[0].mode = mode;
  g_model.timers[0].start = start;
  g_model.timers[0].countdownBeep = COUNTDOWN_SILENT;
  g_model.timers[0].minuteBeep = 0;
  g_model.timers[0].spare = 0;
#if defined(CPUARM) || defined(CPUM2560)
  g_model.timers[0].persistent = 0;
  g_model.timers[0].value = 0;
#endif
}

/*
  Run timers for n seconds and test the end state
*/
::testing::AssertionResult evalTimersForNSecondsAndTest(unsigned int n, uint8_t throttle, uint32_t idx, uint8_t state, int value)
{
  unsigned int noLoops = n * 100;
  while (noLoops--) {
    evalTimers(throttle, 1);
  }
  TEST_AB_EQUAL(timersStates[idx].state, state);
  TEST_AB_EQUAL(timersStates[idx].val, value);
  return ::testing::AssertionSuccess();
}

TEST(Timers, timerReset)
{
  initModelTimer(0, TMRMODE_THR_REL, 200);
  timerReset(0);
  EXPECT_TRUE(evalTimersForNSecondsAndTest(0, THR_100, 0, TMR_OFF, 200));

  initModelTimer(1, TMRMODE_THR_REL, 0);
  timerReset(1);
  EXPECT_TRUE(evalTimersForNSecondsAndTest(0, THR_100, 1, TMR_OFF, 0));
}

#if defined(CPUARM)
TEST(Timers, timerSet)
{
  timerSet(0, 500);
  EXPECT_TRUE(evalTimersForNSecondsAndTest(0, THR_100, 0, TMR_OFF, 500));

  timerSet(1, 300);
  EXPECT_TRUE(evalTimersForNSecondsAndTest(0, THR_100, 1, TMR_OFF, 300));
}

TEST(Timers, timerGreaterThan9hours)
{
  initModelTimer(0, TMRMODE_ABS, 0);
  timerSet(0, 0);
  // test with 24 hours
  EXPECT_TRUE(evalTimersForNSecondsAndTest(24*3600, THR_100, 0, TMR_RUNNING, 24*3600));
}
#endif // #if defined(CPUARM)

#if defined(CPUARM) || defined(CPUM2560)
TEST(Timers, saveRestoreTimers)
{
  g_model.timers[0].persistent = 1;
  g_model.timers[1].persistent = 1;
  timerSet(0, 500);
  timerSet(1, 1500);
  saveTimers();
  EXPECT_EQ(g_model.timers[0].value, 500);
  EXPECT_EQ(g_model.timers[1].value, 1500);
  EXPECT_TRUE(evalTimersForNSecondsAndTest(0, THR_100, 0, TMR_OFF,  500));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(0, THR_100, 1, TMR_OFF, 1500));

  timerReset(0);
  timerReset(1);
  restoreTimers();
  EXPECT_TRUE(evalTimersForNSecondsAndTest(0, THR_100, 0, TMR_OFF,  500));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(0, THR_100, 1, TMR_OFF, 1500));
}
#endif

TEST(Timers, timerOff)
{
  initModelTimer(0, TMRMODE_NONE, 0);
  timerReset(0);

  EXPECT_TRUE(evalTimersForNSecondsAndTest(0,  THR_100, 0, TMR_OFF,  0));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(10, THR_100, 0, TMR_OFF,  0));
}

TEST(Timers, timerAbsolute)
{
  initModelTimer(0, TMRMODE_ABS, 0);
  timerReset(0);

  EXPECT_TRUE(evalTimersForNSecondsAndTest(1,   THR_100, 0, TMR_RUNNING,    1));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100, THR_100, 0, TMR_RUNNING,  101));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100,       0, 0, TMR_RUNNING,  201));

  // max timer value test
  timerSet(0, TIMER_MAX-10);
  EXPECT_TRUE(evalTimersForNSecondsAndTest(1,   THR_100, 0, TMR_RUNNING, TIMER_MAX-9));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100, THR_100, 0, TMR_RUNNING,   TIMER_MAX));

  // test down-running
  g_model.timers[0].start = 200;
  timerReset(0);
  EXPECT_TRUE(evalTimersForNSecondsAndTest(1,   THR_100, 0, TMR_RUNNING,  199));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100, THR_100, 0, TMR_RUNNING,   99));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100, THR_100, 0, TMR_NEGATIVE,  -1));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100, THR_100, 0, TMR_STOPPED, -101));

#if !defined(CPUARM)
  // min timer value test
  timerSet(0, TIMER_MIN+10);
  EXPECT_TRUE(evalTimersForNSecondsAndTest(1,   THR_100, 0, TMR_RUNNING, TIMER_MIN+9));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100, THR_100, 0, TMR_RUNNING,   TIMER_MIN));
#endif
}

TEST(Timers, timerThrottle)
{
  initModelTimer(0, TMRMODE_THR, 0);
  timerReset(0);

  EXPECT_TRUE(evalTimersForNSecondsAndTest(1,         0, 0, TMR_RUNNING,   0));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100, THR_100, 0, TMR_RUNNING, 100));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100,  THR_10, 0, TMR_RUNNING, 200));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100,       0, 0, TMR_RUNNING, 200));

  // test down-running
  g_model.timers[0].start = 200;
  timerReset(0);
  EXPECT_TRUE(evalTimersForNSecondsAndTest(1,   THR_100, 0, TMR_RUNNING, 199));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(1,         0, 0, TMR_RUNNING, 199));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100, THR_100, 0, TMR_RUNNING,  99));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100,  THR_50, 0, TMR_NEGATIVE, -1));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100, THR_100, 0, TMR_STOPPED,-101));
}

TEST(Timers, timerThrottleRelative)
{
  initModelTimer(0, TMRMODE_THR_REL, 0);

  timerReset(0);
  EXPECT_TRUE(evalTimersForNSecondsAndTest(1,         0, 0, TMR_RUNNING,   0));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100, THR_100, 0, TMR_RUNNING, 100));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100,  THR_50, 0, TMR_RUNNING, 150)); // 50% throttle == 50s
#if defined(ACCURAT_THROTTLE_TIMER)
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100,  THR_10, 0, TMR_RUNNING, 160)); // 10% throttle == 10s
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100,       0, 0, TMR_RUNNING, 160));
#else
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100,  THR_10, 0, TMR_RUNNING, 159)); // 10% throttle == 10s
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100,       0, 0, TMR_RUNNING, 159));
#endif

  // test down-running
  initModelTimer(0, TMRMODE_THR_REL, 200);
  timerReset(0);
  EXPECT_TRUE(evalTimersForNSecondsAndTest(1,   THR_100, 0, TMR_RUNNING, 199));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(1,         0, 0, TMR_RUNNING, 199));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100, THR_100, 0, TMR_RUNNING,  99));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(200,  THR_50, 0, TMR_NEGATIVE, -1)); // 50% throttle == 100s
#if defined(ACCURAT_THROTTLE_TIMER)
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100,  THR_10, 0, TMR_NEGATIVE,-11)); // 10% throttle == 10s
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100, THR_100, 0, TMR_STOPPED,-111));
#else
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100,  THR_10, 0, TMR_NEGATIVE,-10)); // 10% throttle == 10s
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100, THR_100, 0, TMR_STOPPED,-110));
#endif
}

TEST(Timers, timerThrottleTriggered)
{
  initModelTimer(0, TMRMODE_THR_TRG, 0);

  timerReset(0);
  EXPECT_TRUE(evalTimersForNSecondsAndTest(1,          0, 0, TMR_OFF,       0));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100, THR_10-1, 0, TMR_OFF,       0));  // below threshold
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100,   THR_50, 0, TMR_RUNNING, 100));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100,  THR_100, 0, TMR_RUNNING, 200));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100,        0, 0, TMR_RUNNING, 300));

  // test down-running
  initModelTimer(0, TMRMODE_THR_TRG, 200);
  timerReset(0);
  EXPECT_TRUE(evalTimersForNSecondsAndTest(1,          0, 0, TMR_OFF,     200));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100, THR_10-1, 0, TMR_OFF,     200));  // below threshold
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100,  THR_100, 0, TMR_RUNNING, 100));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(101,   THR_50, 0, TMR_NEGATIVE, -1));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(10,         0, 0, TMR_NEGATIVE,-11));
  EXPECT_TRUE(evalTimersForNSecondsAndTest(100,        0, 0, TMR_STOPPED,-111));
}
