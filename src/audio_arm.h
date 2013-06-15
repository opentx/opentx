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

#include "FatFs/ff.h"

#define AUDIO_FILENAME_MAXLEN (40)
#define AUDIO_QUEUE_LENGTH    (20)

#define AUDIO_SAMPLE_RATE     (32000)
#define AUDIO_BUFFER_DURATION (10)
#define AUDIO_BUFFER_SIZE     (AUDIO_SAMPLE_RATE*AUDIO_BUFFER_DURATION/1000)
#define AUDIO_BUFFER_COUNT    (3)

#define BEEP_MIN_FREQ         (440)
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

#define FRAGMENT_EMPTY     0
#define FRAGMENT_TONE      1
#define FRAGMENT_FILE      2

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
    } tone;

    char file[AUDIO_FILENAME_MAXLEN+1];
  };

  void clear()
  {
    memset(this, 0, sizeof(AudioFragment));
  }
};

class AudioContext {
  public:
    AudioFragment fragment;

    union {
#if defined(SDCARD)
      struct {
        FIL      file;
        uint8_t  codec;
        uint32_t freq;
        uint32_t size;
        uint8_t  resampleRatio;
        uint16_t readSize;
      } wav;
#endif

      struct {
        uint16_t points[100];
        uint8_t  count;
        uint8_t  idx;
        uint16_t freq;
      } tone;
    } state;

    inline void clear()
    {
      fragment.clear();
    }
};

bool dacQueue(AudioBuffer *buffer);

class AudioQueue {

  friend void audioTask(void* pdata);

  public:

    AudioQueue();

    void start();

    void play(uint16_t freq, uint16_t len, uint16_t pause=0, uint8_t flags=0, int8_t freqIncr=0);

    void playFile(const char *filename, uint8_t flags=0, uint8_t id=0);

    void stopPlay(uint8_t id);

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

    void reset();

    inline AudioBuffer * getNextFilledBuffer()
    {
      if (buffers[bufferRIdx].state == AUDIO_BUFFER_PLAYING) {
        buffers[bufferRIdx].state = AUDIO_BUFFER_FREE;
        bufferRIdx = nextBufferIdx(bufferRIdx);
      }

      uint8_t idx = bufferRIdx;
      while (idx != bufferWIdx) {
        AudioBuffer * buffer = &buffers[idx];
        if (buffer->state == AUDIO_BUFFER_FILLED) {
          buffer->state = AUDIO_BUFFER_PLAYING;
          return buffer;
        }
        idx = nextBufferIdx(idx);
      }

      return NULL;
    }

  protected:

    void wakeup();

    int mixAudioContext(AudioContext &context, AudioBuffer *buffer, unsigned int weight);
    int mixTone(AudioContext &context, AudioBuffer *buffer, unsigned int weight);
    int mixWav(AudioContext &context, AudioBuffer *buffer, unsigned int weight);

    volatile bool state;
    uint8_t ridx;
    uint8_t widx;

    AudioFragment fragments[AUDIO_QUEUE_LENGTH];

    AudioContext backgroundContext; // for background music / vario
    AudioContext currentContext;
    AudioContext foregroundContext; // for important beeps

    AudioBuffer buffers[AUDIO_BUFFER_COUNT];
    uint8_t bufferRIdx;
    uint8_t bufferWIdx;

    inline uint8_t nextBufferIdx(uint8_t idx)
    {
      return (idx == AUDIO_BUFFER_COUNT-1 ? 0 : idx+1);
    }

    inline AudioBuffer * getEmptyBuffer()
    {
      AudioBuffer * buffer = &buffers[bufferWIdx];
      if (buffer->state == AUDIO_BUFFER_FREE)
        return buffer;
      else
        return NULL;
    }

    void pushBuffer(AudioBuffer *buffer);
};

extern AudioQueue audioQueue;

void codecsInit();
void audioEvent(uint8_t e, uint16_t f=BEEP_DEFAULT_FREQ);
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
  #define AUDIO_TIMER_MINUTE(t)  playDuration(t, 0)
#else
  #define AUDIO_ERROR_MESSAGE(e) audioEvent(AU_ERROR)
  #define AUDIO_TIMER_MINUTE(t)  audioDefevent(AU_WARNING1)
#endif

#define AUDIO_TADA()             audioEvent(AU_TADA)
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
#define AUDIO_TIMER_LT10(x)      AUDIO_BUZZER(audioEvent(AU_TIMER_LT10), beep(2))
#define AUDIO_INACTIVITY()       AUDIO_BUZZER(audioEvent(AU_INACTIVITY), beep(3))
#define AUDIO_MIX_WARNING(x)     AUDIO_BUZZER(audioEvent(AU_MIX_WARNING_1+x-1), beep(1))
#define AUDIO_POT_STICK_MIDDLE() AUDIO_BUZZER(audioEvent(AU_POT_STICK_MIDDLE), beep(2))
#define AUDIO_VARIO_UP()         audioEvent(AU_KEYPAD_UP)
#define AUDIO_VARIO_DOWN()       audioEvent(AU_KEYPAD_DOWN)
#define AUDIO_TRIM_MIDDLE(f)     AUDIO_BUZZER(audioEvent(AU_TRIM_MIDDLE, f), beep(2))
#define AUDIO_TRIM_END(f)        AUDIO_BUZZER(audioEvent(AU_TRIM_END, f), beep(2))
#define AUDIO_TRIM(event, f)     AUDIO_BUZZER(audioEvent(AU_TRIM_MOVE, f), { if (!IS_KEY_FIRST(event)) warble = true; beep(1); })
#define AUDIO_PLAY(p)            audioEvent(p)
#define AUDIO_VARIO(f, t)        audioQueue.play(f, t, 0, PLAY_BACKGROUND)

#if defined(PCBTARANIS)
#define AUDIO_A1_ORANGE()        audioEvent(AU_A1_ORANGE)
#define AUDIO_A1_RED()           audioEvent(AU_A1_RED)
#define AUDIO_A2_ORANGE()        audioEvent(AU_A2_ORANGE)
#define AUDIO_A2_RED()           audioEvent(AU_A2_RED)
#define AUDIO_RSSI_ORANGE()      audioEvent(AU_RSSI_ORANGE)
#define AUDIO_RSSI_RED()         audioEvent(AU_RSSI_RED)
#define AUDIO_SWR_RED()          audioEvent(AU_SWR_RED)
#endif

#define AUDIO_HEARTBEAT()

#define SYSTEM_AUDIO_CATEGORY 0
#define MODEL_AUDIO_CATEGORY  1
#define PHASE_AUDIO_CATEGORY  2
#define MIXER_AUDIO_CATEGORY  3

#define AUDIO_EVENT_OFF       0
#define AUDIO_EVENT_ON        1
#define AUDIO_EVENT_BG        2

extern void pushPrompt(uint16_t prompt, uint8_t id=0);

#define I18N_PLAY_FUNCTION(lng, x, ...) void lng ## _ ## x(__VA_ARGS__, uint8_t id)
#define PLAY_FUNCTION(x, ...)    void x(__VA_ARGS__, uint8_t id)
#define PUSH_NUMBER_PROMPT(p)    pushPrompt((p), id)
#define PLAY_NUMBER(n, u, a)     playNumber((n), (u), (a), id)
#define PLAY_DURATION(d)         playDuration((d), id)
#define IS_PLAYING(id)           audioQueue.isPlaying((id))
#define PLAY_VALUE(v, id)        playValue((v), (id))
#define PLAY_FILE(f, flags, id)  audioQueue.playFile((f), (flags), (id))
#define STOP_PLAY(id)            audioQueue.stopPlay((id))
#define AUDIO_RESET()            audioQueue.reset()

#if defined(SDCARD)
  #define PLAY_PHASE_OFF(phase) do { char filename[AUDIO_FILENAME_MAXLEN+1]; if (isAudioFileAvailable((PHASE_AUDIO_CATEGORY << 24) + (phase << 16) + AUDIO_EVENT_OFF, filename)) audioQueue.playFile(filename); } while (0)
  #define PLAY_PHASE_ON(phase)  do { char filename[AUDIO_FILENAME_MAXLEN+1]; if (isAudioFileAvailable((PHASE_AUDIO_CATEGORY << 24) + (phase << 16) + AUDIO_EVENT_ON, filename)) audioQueue.playFile(filename); } while (0)
#else
  #define PLAY_PHASE_OFF(phase)
  #define PLAY_PHASE_ON(phase)
#endif

extern void refreshSystemAudioFiles();
extern void refreshModelAudioFiles();
extern bool isAudioFileAvailable(uint32_t i, char * filename);

// TODO not public, everything could be inside a function in audio_arm.cpp
extern OS_MutexID audioMutex;

#endif
