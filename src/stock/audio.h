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

#if defined(PCBSTD)
#define SPEAKER_ON   BUZZER_ON
#define SPEAKER_OFF  toneFreq=0; BUZZER_OFF
#endif

//audio
#define AUDIO_QUEUE_LENGTH (8)  // 8 seems to suit most alerts
#define BEEP_DEFAULT_FREQ  (70)
#define BEEP_OFFSET        (10)
#define BEEP_KEY_UP_FREQ   (BEEP_DEFAULT_FREQ+5)
#define BEEP_KEY_DOWN_FREQ (BEEP_DEFAULT_FREQ-5)

class audioQueue
{
  public:

    audioQueue();

    void play(uint8_t tFreq, uint8_t tLen, uint8_t tPause, uint8_t tFlags=0, int8_t tFreqIncr=0);
    void pause(uint8_t tLen);
    
    inline bool busy() { return (toneTimeLeft > 0); }

    void event(uint8_t e, uint8_t f=BEEP_DEFAULT_FREQ);

    inline void driver() {
      if (toneFreq) {
        toneCounter += toneFreq;
        if ((toneCounter & 0x80) == 0x80)
          BUZZER_ON;
        else
          BUZZER_OFF;
      }
    }

    // heartbeat is responsibile for issueing the audio tones and general square waves
    // it is essentially the life of the class.
    void heartbeat();

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

    // vario has less priority
    uint8_t tone2Freq;
    uint8_t tone2TimeLeft;
    uint8_t tone2Pause;
	  
    // queue arrays
    uint8_t queueToneFreq[AUDIO_QUEUE_LENGTH];
    int8_t queueToneFreqIncr[AUDIO_QUEUE_LENGTH];
    uint8_t queueToneLength[AUDIO_QUEUE_LENGTH];
    uint8_t queueTonePause[AUDIO_QUEUE_LENGTH];
    uint8_t queueToneRepeat[AUDIO_QUEUE_LENGTH];

    uint8_t toneCounter;

};


extern audioQueue audio;

void audioDefevent(uint8_t e);

#define AUDIO_KEYPAD_UP()   audioDefevent(AU_KEYPAD_UP)
#define AUDIO_KEYPAD_DOWN() audioDefevent(AU_KEYPAD_DOWN)
#define AUDIO_MENUS()       audioDefevent(AU_MENUS)
#define AUDIO_WARNING1()    audioDefevent(AU_WARNING1)
#define AUDIO_WARNING2()    audioDefevent(AU_WARNING2)
#define AUDIO_ERROR()       audioDefevent(AU_ERROR)
#if defined(VOICE)
#define AUDIO_TADA()        pushPrompt(PROMPT_SYSTEM_BASE+AU_TADA)
#define AUDIO_TX_BATTERY_LOW() pushPrompt(PROMPT_SYSTEM_BASE+AU_TX_BATTERY_LOW)
#define AUDIO_INACTIVITY()  pushPrompt(PROMPT_SYSTEM_BASE+AU_INACTIVITY)
#define AUDIO_ERROR_MESSAGE(e) pushPrompt(PROMPT_SYSTEM_BASE+(e))
#define AUDIO_TIMER_30()    pushPrompt(PROMPT_SYSTEM_BASE+AU_TIMER_30)
#define AUDIO_TIMER_20()    pushPrompt(PROMPT_SYSTEM_BASE+AU_TIMER_20)
#define AUDIO_TIMER_10()    pushPrompt(PROMPT_SYSTEM_BASE+AU_TIMER_10)
#define AUDIO_TIMER_LT3(x)  pushPrompt(PROMPT_SYSTEM_BASE+AU_TIMER_LT3)
#else
#define AUDIO_TADA()
#define AUDIO_TX_BATTERY_LOW() audioDefevent(AU_TX_BATTERY_LOW)
#define AUDIO_INACTIVITY()  audioDefevent(AU_INACTIVITY)
#define AUDIO_ERROR_MESSAGE(e) audioDefevent(AU_ERROR)
#define AUDIO_TIMER_30()    audioDefevent(AU_TIMER_30)
#define AUDIO_TIMER_20()    audioDefevent(AU_TIMER_20)
#define AUDIO_TIMER_10()    audioDefevent(AU_TIMER_10)
#define AUDIO_TIMER_LT3(x)  audioDefevent(AU_TIMER_LT3)
#endif

#define AUDIO_MINUTE_BEEP() audioDefevent(AU_WARNING1)
#define AUDIO_MIX_WARNING_1() audioDefevent(AU_MIX_WARNING_1)
#define AUDIO_MIX_WARNING_2() audioDefevent(AU_MIX_WARNING_2)
#define AUDIO_MIX_WARNING_3() audioDefevent(AU_MIX_WARNING_3)
#define AUDIO_POT_STICK_MIDDLE() audioDefevent(AU_POT_STICK_MIDDLE)
#define AUDIO_VARIO_UP()    audioDefevent(AU_KEYPAD_UP)
#define AUDIO_VARIO_DOWN()  audioDefevent(AU_KEYPAD_DOWN)
#define AUDIO_TRIM_MIDDLE(f) audio.event(AU_TRIM_MIDDLE, f)
#define AUDIO_TRIM_END(f)    AUDIO_TRIM_MIDDLE(f)
#define AUDIO_TRIM(event, f) audio.event(AU_TRIM_MOVE, f)
#define AUDIO_PLAY(p)       audio.event(p)
#define AUDIO_VARIO(f, t)   audio.play(f, t, 0, PLAY_BACKGROUND)

#define AUDIO_DRIVER()      audio.driver()
#define AUDIO_HEARTBEAT()   audio.heartbeat()
#define IS_AUDIO_BUSY()     audio.busy()
#define AUDIO_RESET()
