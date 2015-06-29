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

#ifndef audio_h
#define audio_h

#include <stddef.h>
#include "ff.h"

#define AUDIO_FILENAME_MAXLEN (42)      //max length example: /SOUNDS/fr/123456789012/1234567890-off.wav
#define AUDIO_QUEUE_LENGTH    (20)

#define AUDIO_SAMPLE_RATE     (32000)
#define AUDIO_BUFFER_DURATION (10)
#define AUDIO_BUFFER_SIZE     (AUDIO_SAMPLE_RATE*AUDIO_BUFFER_DURATION/1000)
#if defined(SIMU_AUDIO)
  #define AUDIO_BUFFER_COUNT  (10)      // simulator needs more buffers for smooth audio
#else
  #define AUDIO_BUFFER_COUNT  (3)
#endif

#define BEEP_MIN_FREQ         (150)
#define BEEP_DEFAULT_FREQ     (2250)
#define BEEP_KEY_UP_FREQ      (BEEP_DEFAULT_FREQ+150)
#define BEEP_KEY_DOWN_FREQ    (BEEP_DEFAULT_FREQ-150)

#define AUDIO_BUFFER_FREE     (0)
#define AUDIO_BUFFER_FILLED   (1)
#define AUDIO_BUFFER_PLAYING  (2)

struct AudioBuffer {
  uint16_t data[AUDIO_BUFFER_SIZE];
  uint16_t size;
  uint8_t  state;
};

extern AudioBuffer audioBuffers[AUDIO_BUFFER_COUNT];

enum FragmentTypes {
  FRAGMENT_EMPTY,
  FRAGMENT_TONE,
  FRAGMENT_FILE,
};

struct AudioFragment {
  uint8_t type;
  uint8_t id;
  uint8_t repeat;
  union {
    struct {
      uint16_t freq;
      uint16_t duration;
      uint16_t pause;
      int8_t   freqIncr;
      uint8_t  reset;
    } tone;

    char file[AUDIO_FILENAME_MAXLEN+1];
  };

  void clear()
  {
    memset(this, 0, sizeof(AudioFragment));
  }
};

class ToneContext {
  public:
    AudioFragment fragment;

    struct {
      double step;
      double idx;
      float  volume;
      uint16_t freq;
      uint16_t duration;
      uint16_t pause;
    } state;

    inline void setFragment(AudioFragment & fragment)
    {
      this->fragment = fragment;
      memset(&state, 0, sizeof(state));
    }

    inline void clear()
    {
      memset(this, 0, sizeof(ToneContext));
    }

    int mixBuffer(AudioBuffer *buffer, int volume, unsigned int fade);
};

class WavContext {
  public:
    AudioFragment fragment;

    struct {
      FIL      file;
      uint8_t  codec;
      uint32_t freq;
      uint32_t size;
      uint8_t  resampleRatio;
      uint16_t readSize;
    } state;

    inline void clear()
    {
      fragment.clear();
    }

    int mixBuffer(AudioBuffer *buffer, int volume, unsigned int fade);
};

class MixedContext {
  public:
    union {
      AudioFragment fragment;
      ToneContext tone;
      WavContext wav;
    };

    int mixBuffer(AudioBuffer *buffer, int volume, unsigned int fade);
};

bool dacQueue(AudioBuffer *buffer);

class AudioQueue {

  friend void audioTask(void* pdata);
#if defined(SIMU_AUDIO)
  friend void *audioThread(void *);
#endif

  public:

    AudioQueue();

    void start();

    void playTone(uint16_t freq, uint16_t len, uint16_t pause=0, uint8_t flags=0, int8_t freqIncr=0);

    void playFile(const char *filename, uint8_t flags=0, uint8_t id=0);

    void stopPlay(uint8_t id);

    void stopAll();

    void flush();

    void pause(uint16_t tLen);

    void stopSD();

    bool isPlaying(uint8_t id);

    bool started()
    {
      return state;
    }

    bool empty()
    {
      return ridx == widx;
    }

    inline AudioBuffer * getNextFilledBuffer()
    {
      if (audioBuffers[bufferRIdx].state == AUDIO_BUFFER_PLAYING) {
        audioBuffers[bufferRIdx].state = AUDIO_BUFFER_FREE;
        bufferRIdx = nextBufferIdx(bufferRIdx);
      }

      uint8_t idx = bufferRIdx;
      do {
        AudioBuffer * buffer = &audioBuffers[idx];
        if (buffer->state == AUDIO_BUFFER_FILLED) {
          buffer->state = AUDIO_BUFFER_PLAYING;
          bufferRIdx = idx;
          return buffer;
        }
        idx = nextBufferIdx(idx);
      } while (idx != bufferWIdx);   //this fixes a bug if all buffers are filled

      return NULL;
    }

    bool filledAtleast(int noBuffers) {
      int count = 0;
      for(int n= 0; n<AUDIO_BUFFER_COUNT; ++n) {
        if (audioBuffers[n].state == AUDIO_BUFFER_FILLED) {
          if (++count >= noBuffers) {
            return true;
          }
        }
      }
      return false;
    }

  protected:

    void wakeup();

    volatile bool state;
    uint8_t ridx;
    uint8_t widx;

    AudioFragment fragments[AUDIO_QUEUE_LENGTH];

    MixedContext normalContext;
    WavContext   backgroundContext;
    ToneContext  priorityContext;
    ToneContext  varioContext;

    uint8_t bufferRIdx;
    uint8_t bufferWIdx;

    inline uint8_t nextBufferIdx(uint8_t idx)
    {
      return (idx == AUDIO_BUFFER_COUNT-1 ? 0 : idx+1);
    }

    inline AudioBuffer * getEmptyBuffer()
    {
      AudioBuffer * buffer = &audioBuffers[bufferWIdx];
      if (buffer->state == AUDIO_BUFFER_FREE)
        return buffer;
      else
        return NULL;
    }
};

extern AudioQueue audioQueue;

enum {
  ID_PLAY_FROM_SD_MANAGER = 254,
  ID_PLAY_BYE = 255
};

void codecsInit();
void audioEvent(unsigned int index, unsigned int freq=BEEP_DEFAULT_FREQ);
void audioPlay(unsigned int index, uint8_t id=0);
void audioStart();

#if defined(AUDIO) && defined(BUZZER)
  #define AUDIO_BUZZER(a, b)  do { a; b; } while(0)
#elif defined(AUDIO)
  #define AUDIO_BUZZER(a, b)  a
#else
  #define AUDIO_BUZZER(a, b)  b
#endif

#if defined(VOICE)
  #define AUDIO_ERROR_MESSAGE(e) audioEvent(e)
  #define AUDIO_TIMER_MINUTE(t)  playDuration(t, 0, 0)
#else
  #define AUDIO_ERROR_MESSAGE(e) audioEvent(AU_ERROR)
  #define AUDIO_TIMER_MINUTE(t)  audioDefevent(AU_WARNING1)
#endif

#define AUDIO_TADA()             audioPlay(AU_TADA)
#define AUDIO_BYE()              audioPlay(AU_BYE, ID_PLAY_BYE)
#define AUDIO_KEYPAD_UP()        AUDIO_BUZZER(audioEvent(AU_KEYPAD_UP), beep(0))
#define AUDIO_KEYPAD_DOWN()      AUDIO_BUZZER(audioEvent(AU_KEYPAD_DOWN), beep(0))
#define AUDIO_MENUS()            AUDIO_BUZZER(audioEvent(AU_MENUS), beep(0))
#define AUDIO_WARNING1()         AUDIO_BUZZER(audioEvent(AU_WARNING1), beep(3))
#define AUDIO_WARNING2()         AUDIO_BUZZER(audioEvent(AU_WARNING2), beep(2))
#define AUDIO_TX_BATTERY_LOW()   AUDIO_BUZZER(audioEvent(AU_TX_BATTERY_LOW), beep(4))
#if defined(PCBSKY9X)
  #define AUDIO_TX_MAH_HIGH()    audioEvent(AU_TX_MAH_HIGH)
  #define AUDIO_TX_TEMP_HIGH()   audioEvent(AU_TX_TEMP_HIGH)
#endif
#define AUDIO_ERROR()            AUDIO_BUZZER(audioEvent(AU_ERROR), beep(4))
#define AUDIO_TIMER_30()         AUDIO_BUZZER(audioEvent(AU_TIMER_30), { beepAgain=2; beep(2); })
#define AUDIO_TIMER_20()         AUDIO_BUZZER(audioEvent(AU_TIMER_20), { beepAgain=1; beep(2); })
#define AUDIO_TIMER_LT10(m, x)   do { if (m==COUNTDOWN_VOICE) playNumber(x, 0, 0, 0); else AUDIO_BUZZER(audioEvent(AU_TIMER_LT10), beep(2)); } while(0)
#define AUDIO_TIMER_00(m)        do { if (m==COUNTDOWN_VOICE) playNumber(0, 0, 0, 0); else AUDIO_BUZZER(audioEvent(AU_TIMER_00), beep(3)); } while(0)
#define AUDIO_INACTIVITY()       AUDIO_BUZZER(audioEvent(AU_INACTIVITY), beep(3))
#define AUDIO_MIX_WARNING(x)     AUDIO_BUZZER(audioEvent(AU_MIX_WARNING_1+x-1), beep(1))
#define AUDIO_POT_MIDDLE(x)      AUDIO_BUZZER(audioEvent(AU_STICK1_MIDDLE+x), beep(2))
#define AUDIO_TRIM_MIDDLE(f)     AUDIO_BUZZER(audioEvent(AU_TRIM_MIDDLE, f), beep(2))
#define AUDIO_TRIM_END(f)        AUDIO_BUZZER(audioEvent(AU_TRIM_END, f), beep(2))
#define AUDIO_TRIM(event, f)     AUDIO_BUZZER(audioEvent(AU_TRIM_MOVE, f), { if (!IS_KEY_FIRST(event)) warble = true; beep(1); })
#define AUDIO_PLAY(p)            audioEvent(p)
#define AUDIO_VARIO(fq, t, p, f) audioQueue.playTone(fq, t, p, f)
#define AUDIO_A1_ORANGE()        audioEvent(AU_A1_ORANGE)
#define AUDIO_A1_RED()           audioEvent(AU_A1_RED)
#define AUDIO_A2_ORANGE()        audioEvent(AU_A2_ORANGE)
#define AUDIO_A2_RED()           audioEvent(AU_A2_RED)
#define AUDIO_A3_ORANGE()        audioEvent(AU_A3_ORANGE)
#define AUDIO_A3_RED()           audioEvent(AU_A3_RED)
#define AUDIO_A4_ORANGE()        audioEvent(AU_A4_ORANGE)
#define AUDIO_A4_RED()           audioEvent(AU_A4_RED)
#define AUDIO_RSSI_ORANGE()      audioEvent(AU_RSSI_ORANGE)
#define AUDIO_RSSI_RED()         audioEvent(AU_RSSI_RED)
#define AUDIO_SWR_RED()          audioEvent(AU_SWR_RED)
#define AUDIO_RXBATT_ORANGE()    audioEvent(AU_RXBATT_ORANGE)
#define AUDIO_RXBATT_RED()       audioEvent(AU_RXBATT_RED)
#define AUDIO_TELEMETRY_LOST()   audioEvent(AU_TELEMETRY_LOST)
#define AUDIO_TELEMETRY_BACK()   audioEvent(AU_TELEMETRY_BACK)

#define AUDIO_HEARTBEAT()

enum AutomaticPromptsCategories {
  SYSTEM_AUDIO_CATEGORY,
  MODEL_AUDIO_CATEGORY,
  PHASE_AUDIO_CATEGORY,
  SWITCH_AUDIO_CATEGORY,
  LOGICAL_SWITCH_AUDIO_CATEGORY,
};

enum AutomaticPromptsEvents {
  AUDIO_EVENT_OFF,
  AUDIO_EVENT_ON,
  AUDIO_EVENT_MID,
};

void pushPrompt(uint16_t prompt, uint8_t id=0);

#define I18N_PLAY_FUNCTION(lng, x, ...) void lng ## _ ## x(__VA_ARGS__, uint8_t id)
#define PLAY_FUNCTION(x, ...)    void x(__VA_ARGS__, uint8_t id)
#define PUSH_NUMBER_PROMPT(p)    pushPrompt((p), id)
#define PLAY_NUMBER(n, u, a)     playNumber((n), (u), (a), id)
#define PLAY_DURATION(d, att)    playDuration((d), (att), id)
#define PLAY_DURATION_ATT        , uint8_t flags
#define PLAY_TIME                1
#define IS_PLAY_TIME()           (flags&PLAY_TIME)
#define IS_PLAYING(id)           audioQueue.isPlaying((id))
#define PLAY_VALUE(v, id)        playValue((v), (id))
#define PLAY_FILE(f, flags, id)  audioQueue.playFile((f), (flags), (id))
#define STOP_PLAY(id)            audioQueue.stopPlay((id))
#define AUDIO_RESET()            audioQueue.stopAll()
#define AUDIO_FLUSH()            audioQueue.flush()

#if defined(SDCARD)
  extern tmr10ms_t timeAutomaticPromptsSilence;
  void playModelEvent(uint8_t category, uint8_t index, uint8_t event=0);
  #define PLAY_PHASE_OFF(phase)         playModelEvent(PHASE_AUDIO_CATEGORY, phase, AUDIO_EVENT_OFF)
  #define PLAY_PHASE_ON(phase)          playModelEvent(PHASE_AUDIO_CATEGORY, phase, AUDIO_EVENT_ON)
  #define PLAY_SWITCH_MOVED(sw)         playModelEvent(SWITCH_AUDIO_CATEGORY, sw)
  #define PLAY_LOGICAL_SWITCH_OFF(sw)   playModelEvent(LOGICAL_SWITCH_AUDIO_CATEGORY, sw, AUDIO_EVENT_OFF)
  #define PLAY_LOGICAL_SWITCH_ON(sw)    playModelEvent(LOGICAL_SWITCH_AUDIO_CATEGORY, sw, AUDIO_EVENT_ON)
  #define START_SILENCE_PERIOD()        timeAutomaticPromptsSilence = get_tmr10ms()
  #define IS_SILENCE_PERIOD_ELAPSED()   (get_tmr10ms()-timeAutomaticPromptsSilence > 50)
#else
  #define PLAY_PHASE_OFF(phase)
  #define PLAY_PHASE_ON(phase)
  #define PLAY_SWITCH_MOVED(sw)
  #define PLAY_LOGICAL_SWITCH_OFF(sw)
  #define PLAY_LOGICAL_SWITCH_ON(sw)
  #define START_SILENCE_PERIOD()
#endif

char * getAudioPath(char * path);

void referenceSystemAudioFiles();
void referenceModelAudioFiles();

bool isAudioFileReferenced(uint32_t i, char * filename/*at least AUDIO_FILENAME_MAXLEN+1 long*/);

#endif
