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

#ifndef BUZZER_H
#define BUZZER_H

#if defined(BUZZER)
extern uint8_t g_beepCnt;
extern uint8_t beepAgain;
extern uint8_t beepAgainOrig;
extern uint8_t beepOn;
extern bool warble;
extern bool warbleC;
#if defined(HAPTIC)
extern uint8_t hapticTick;
#endif /* HAPTIC */
#endif /* BUZZER */

#if defined(BUZZER)

#if defined(CPUARM) && !defined(SIMU)
inline void _beep(uint8_t b)
{
  buzzerSound(b);
}
#else /* CPUARM && !SIMU */
inline void _beep(uint8_t b)
{
  g_beepCnt = b;
}
#endif /* CPUARM && !SIMU */

void beep(uint8_t val);
#else /* BUZZER */
inline void beep(uint8_t) { }
#endif /* BUZZER */

#if !defined(AUDIO)

#if defined(BUZZER)
  #if defined(VOICE)
    #define AUDIO_TADA()           PUSH_SYSTEM_PROMPT(AU_TADA)
    #define AUDIO_BYE()
    #define AUDIO_TX_BATTERY_LOW() PUSH_SYSTEM_PROMPT(AU_TX_BATTERY_LOW)
    #define AUDIO_INACTIVITY()     PUSH_SYSTEM_PROMPT(AU_INACTIVITY)
    #define AUDIO_ERROR_MESSAGE(e) PUSH_SYSTEM_PROMPT((e))
    #define AUDIO_TIMER_MINUTE(t)  playDuration(t)
    #define AUDIO_TIMER_30()       PUSH_SYSTEM_PROMPT(AU_TIMER_30)
    #define AUDIO_TIMER_20()       PUSH_SYSTEM_PROMPT(AU_TIMER_20)
  #else
    #define AUDIO_TADA()
    #define AUDIO_BYE()
    #define AUDIO_TX_BATTERY_LOW() beep(4)
    #define AUDIO_INACTIVITY()     beep(3)
    #define AUDIO_ERROR_MESSAGE(e) beep(4)
    #define AUDIO_TIMER_MINUTE(t)  beep(2)
    #define AUDIO_TIMER_30()       { beepAgain=2; beep(2); }
    #define AUDIO_TIMER_20()       { beepAgain=1; beep(2); }
  #endif

  #define AUDIO_KEYPAD_UP()        beep(0)
  #define AUDIO_KEYPAD_DOWN()      beep(0)
  #define AUDIO_MENUS()            beep(0)
  #define AUDIO_WARNING2()         beep(2)
  #define AUDIO_WARNING1()         beep(3)
  #define AUDIO_ERROR()            beep(4)
  #define AUDIO_MIX_WARNING(x)     beep(1)
  #define AUDIO_POT_MIDDLE()       beep(2)
  #define AUDIO_TIMER_LT10(m, x)   beep(2)
  #define AUDIO_TIMER_00(m)        beep(3)
  #define AUDIO_VARIO_UP()         _beep(1)
  #define AUDIO_VARIO_DOWN()       _beep(1)
  #define AUDIO_TRIM(event, f)     { if (!IS_KEY_FIRST(event)) warble = true; beep(1); }
  #define AUDIO_TRIM_MIDDLE(f)     beep(2)
  #define AUDIO_TRIM_END(f)        beep(2)
  #define AUDIO_PLAY(p)            beep(3)

  #define IS_AUDIO_BUSY() (g_beepCnt || beepAgain || beepOn)
#else /* BUZZER */
  #define AUDIO_TADA()
  #define AUDIO_BYE()
  #define AUDIO_TX_BATTERY_LOW()
  #define AUDIO_INACTIVITY()
  #define AUDIO_ERROR_MESSAGE(e)
  #define AUDIO_TIMER_MINUTE(t)
  #define AUDIO_TIMER_30()
  #define AUDIO_TIMER_20()
  #define AUDIO_KEYPAD_UP()
  #define AUDIO_KEYPAD_DOWN()
  #define AUDIO_MENUS()
  #define AUDIO_WARNING2()
  #define AUDIO_WARNING1()
  #define AUDIO_ERROR()
  #define AUDIO_MIX_WARNING(x)
  #define AUDIO_POT_MIDDLE()
  #define AUDIO_TIMER_LT10(m, x)
  #define AUDIO_TIMER_00(m)
  #define AUDIO_VARIO_UP()
  #define AUDIO_VARIO_DOWN()
  #define AUDIO_TRIM(event, f)
  #define AUDIO_TRIM_MIDDLE(f)
  #define AUDIO_TRIM_END(f)
  #define AUDIO_PLAY(p)
  #define IS_AUDIO_BUSY() false
#endif /* BUZZER */

  #define AUDIO_RESET()
  #define AUDIO_FLUSH()

  #define PLAY_PHASE_OFF(phase)
  #define PLAY_PHASE_ON(phase)
  #define PLAY_SWITCH_MOVED(sw)
  #define PLAY_LOGICAL_SWITCH_OFF(sw)
  #define PLAY_LOGICAL_SWITCH_ON(sw)
  #define PLAY_MODEL_NAME()
  #define START_SILENCE_PERIOD()
#endif /* !AUDIO */

#if !defined(CPUARM)
#if defined(BUZZER)
inline void BUZZER_HEARTBEAT()
{
  if (g_beepCnt) {
    if (!beepAgainOrig) {
      beepAgainOrig = g_beepCnt;
      beepOn = true;
    }
    g_beepCnt--;
  }
  else {
    if (beepAgain && beepAgainOrig) {
      beepOn = !beepOn;
      g_beepCnt = beepOn ? beepAgainOrig : 8;
      if (beepOn) beepAgain--;
    }
    else {
      beepAgainOrig = 0;
      beepOn = false;
      warble = false;
    }
  }

  if (beepOn) {
    warbleC = warble && !warbleC;
    if (warbleC)
      buzzerOff();
    else
      buzzerOn();
  }
  else {
    buzzerOff();
  }
}
#else // BUZZER
#define BUZZER_HEARTBEAT()
#endif // BUZZER
#endif // CPUARM

#endif
