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

#ifndef BEEPER_H
#define BEEPER_H

extern uint8_t g_beepCnt;
extern uint8_t beepAgain;
extern uint8_t beepAgainOrig;
extern uint8_t beepOn;
extern bool warble;
extern bool warbleC;
#if defined(HAPTIC)
extern uint8_t hapticTick;
#endif

#if defined(PCBARM)
#include "ersky9x/sound.h"
inline void _beep(uint8_t b) {
  buzzer_sound(b);
}
#else
inline void _beep(uint8_t b) {
  g_beepCnt = b;
}
#endif

extern void beep(uint8_t val);

#define AUDIO_KEYPAD_UP()     beep(0)
#define AUDIO_KEYPAD_DOWN()   beep(0)
#define AUDIO_MENUS()         beep(0)
#define AUDIO_TRIM()          beep(1)
#define AUDIO_WARNING2()      beep(2)
#define AUDIO_WARNING1()      beep(3)
#define AUDIO_ERROR()         beep(4)
#define AUDIO_TIMER_30()      { beepAgain=2; beep(2); }
#define AUDIO_TIMER_20()      { beepAgain=1; beep(2); }
#define AUDIO_TIMER_10()      beep(2)
#define AUDIO_TIMER_LT3()     beep(2)
#define AUDIO_MINUTE_BEEP()   beep(2)
#define AUDIO_INACTIVITY()    beep(3)
#define AUDIO_MIX_WARNING_1() beep(1)
#define AUDIO_MIX_WARNING_2() beep(1)
#define AUDIO_MIX_WARNING_3() beep(1)
#define AUDIO_POT_STICK_MIDDLE() beep(2)
#define AUDIO_VARIO_UP()      _beep(1)
#define AUDIO_VARIO_DOWN()    _beep(1)
#define AUDIO_TRIM_MIDDLE()   beep(2)
#define AUDIO_TELEMETRY_ALARM() { warble = !(g_tmr10ms % 60); AUDIO_WARNING2(); }

#define IS_AUDIO_BUSY() (g_beepCnt || beepAgain || beepOn)

#if !defined(PCBARM)
FORCEINLINE void AUDIO_HEARTBEAT()
{
    if(g_beepCnt) {
        if(!beepAgainOrig) {
            beepAgainOrig = g_beepCnt;
            beepOn = true;
        }
        g_beepCnt--;
    }
    else
    {
        if(beepAgain && beepAgainOrig) {
            beepOn = !beepOn;
            g_beepCnt = beepOn ? beepAgainOrig : 8;
            if(beepOn) beepAgain--;
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
        BUZZER_OFF;
      else
        BUZZER_ON;
    }
    else {
      BUZZER_OFF;
    }
}
#endif

#endif
