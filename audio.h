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

#ifndef audio_h
#define audio_h

//#define ISER9X  //enable this define for er9x.  comment out for open9x

#if defined(ISER9X)
#define HAPTIC
#define PCBSTD
#endif

#if defined(PCBARM)
#include "ersky9x/sound.h"
#endif

#if defined(PCBSTD)
#define SPEAKER_ON   BUZZER_ON
#define SPEAKER_OFF  BUZZER_OFF
#endif

//audio
#define AUDIO_QUEUE_LENGTH (8)  //8 seems to suit most alerts
#define BEEP_DEFAULT_FREQ  (70)
#define BEEP_OFFSET        (10)
#define BEEP_KEY_UP_FREQ   (BEEP_DEFAULT_FREQ+5)
#define BEEP_KEY_DOWN_FREQ (BEEP_DEFAULT_FREQ-5)

/* make sure the defines below always go in numeric order */
enum AUDIO_SOUNDS {
    AU_INACTIVITY,
    AU_TX_BATTERY_LOW,
    AU_ERROR,
    AU_KEYPAD_UP,
    AU_KEYPAD_DOWN,
    AU_MENUS,
    AU_TRIM_MOVE,
    AU_TRIM_MIDDLE,
    AU_WARNING1,
    AU_WARNING2,
    AU_WARNING3,
    AU_TADA,
    AU_POT_STICK_MIDDLE,
    AU_MIX_WARNING_1,
    AU_MIX_WARNING_2,
    AU_MIX_WARNING_3,
    AU_TIMER_30,
    AU_TIMER_20,
    AU_TIMER_10,
    AU_TIMER_LT3,
    AU_FRSKY_FIRST,
    AU_FRSKY_WARN1 = AU_FRSKY_FIRST,
    AU_FRSKY_WARN2,
    AU_FRSKY_CHEEP,
    AU_FRSKY_RING,
    AU_FRSKY_SCIFI,
    AU_FRSKY_ROBOT,
    AU_FRSKY_CHIRP,
    AU_FRSKY_TADA,
    AU_FRSKY_CRICKET,
    AU_FRSKY_SIREN,
    AU_FRSKY_ALARMC,
    AU_FRSKY_RATATA,
    AU_FRSKY_TICK,
#ifdef HAPTIC
    AU_FRSKY_HAPTIC1,
    AU_FRSKY_HAPTIC2,
    AU_FRSKY_HAPTIC3,
#endif
    AU_FRSKY_LAST,
};

class audioQueue
{
  public:

    audioQueue();

    // only difference between these two functions is that one does the
    // interupt queue (Now) and the other queues for playing ASAP.
    void playNow(uint8_t tFreq, uint8_t tLen, uint8_t tPause, uint8_t tRepeat=0, uint8_t tHaptic=0, int8_t tFreqIncr=0);

    void playASAP(uint8_t tFreq, uint8_t tLen, uint8_t tPause, uint8_t tRepeat=0, uint8_t tHaptic=0, int8_t tFreqIncr=0);

    inline bool busy() { return (toneTimeLeft > 0); }

    void event(uint8_t e, uint8_t f=BEEP_DEFAULT_FREQ);

#if defined(ISER9X)
    inline void driver() {
      if (toneTimeLeft > 0) {
        switch (g_eeGeneral.speakerMode) {
          case 0:
            // stock beeper. simply turn port on for x time!
            if (toneTimeLeft > 0){
              PORTE |=  (1<<OUT_E_BUZZER); // speaker output 'high'
            }
            break;
          case 1:
            static uint8_t toneCounter;
            toneCounter += toneFreq;
            if ((toneCounter & 0x80) == 0x80) {
              PORTE |= (1 << OUT_E_BUZZER);
            }
            else {
              PORTE &= ~(1 << OUT_E_BUZZER);
            }
            break;
        }
      }
      else {
        PORTE &=  ~(1<<OUT_E_BUZZER); // speaker output 'low'
      }
    }
#elif defined(PCBSTD)
    inline void driver() {
      if (toneFreq && toneTimeLeft > 0) {
        toneCounter += toneFreq;
        if ((toneCounter & 0x80) == 0x80)
          BUZZER_ON;
        else
          BUZZER_OFF;
      }
    }
#endif

    // heartbeat is responsibile for issueing the audio tones and general square waves
    // it is essentially the life of the class.
    void heartbeat();

    // bool freeslots(uint8_t slots);

    inline bool empty() {
      return (t_queueRidx == t_queueWidx);
    }

  protected:
    inline uint8_t getToneLength(uint8_t tLen);

  private:
    uint8_t t_queueRidx;
    uint8_t t_queueWidx;

    uint8_t toneFreq;
    int8_t toneFreqIncr;
    uint8_t toneTimeLeft;
    uint8_t tonePause;

    // queue arrays
    uint8_t queueToneFreq[AUDIO_QUEUE_LENGTH];
    int8_t queueToneFreqIncr[AUDIO_QUEUE_LENGTH];
    uint8_t queueToneLength[AUDIO_QUEUE_LENGTH];
    uint8_t queueTonePause[AUDIO_QUEUE_LENGTH];
    uint8_t queueToneRepeat[AUDIO_QUEUE_LENGTH];

#ifdef HAPTIC
    uint8_t toneHaptic;
    uint8_t hapticTick;
    uint8_t queueToneHaptic[AUDIO_QUEUE_LENGTH];
#endif

#if defined(PCBSTD)
    uint8_t toneCounter;
#endif
};

//wrapper function - dirty but results in a space saving!!!
extern audioQueue audio;

void audioDefevent(uint8_t e);

#define AUDIO_KEYPAD_UP()   audioDefevent(AU_KEYPAD_UP)
#define AUDIO_KEYPAD_DOWN() audioDefevent(AU_KEYPAD_DOWN)
#define AUDIO_MENUS()       audioDefevent(AU_MENUS)
#define AUDIO_WARNING1()    audioDefevent(AU_WARNING1)
#define AUDIO_WARNING2()    audioDefevent(AU_WARNING2)
#define AUDIO_ERROR()       audioDefevent(AU_ERROR)

#define IS_AUDIO_BUSY()     audio.busy()

#define AUDIO_TIMER_30()    audioDefevent(AU_TIMER_30)
#define AUDIO_TIMER_20()    audioDefevent(AU_TIMER_20)
#define AUDIO_TIMER_10()    audioDefevent(AU_TIMER_10)
#define AUDIO_TIMER_LT3()   audioDefevent(AU_TIMER_LT3)
#define AUDIO_MINUTE_BEEP() audioDefevent(AU_WARNING1)
#define AUDIO_INACTIVITY()  audioDefevent(AU_INACTIVITY)
#define AUDIO_MIX_WARNING_1() audioDefevent(AU_MIX_WARNING_1)
#define AUDIO_MIX_WARNING_2() audioDefevent(AU_MIX_WARNING_2)
#define AUDIO_MIX_WARNING_3() audioDefevent(AU_MIX_WARNING_3)
#define AUDIO_POT_STICK_MIDDLE() audioDefevent(AU_POT_STICK_MIDDLE)
#define AUDIO_VARIO_UP()    audioDefevent(AU_KEYPAD_UP)
#define AUDIO_VARIO_DOWN()  audioDefevent(AU_KEYPAD_DOWN)

#if defined(PCBSTD)
#define AUDIO_DRIVER()      audio.driver()
#endif

#define AUDIO_HEARTBEAT()   audio.heartbeat()

#endif // audio_h
