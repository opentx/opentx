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

#include "../open9x.h"

const char * audioFilenames[] = {
  "inactiv",
  "lowbatt",
  "error",
  "keyup",
  "keydown",
  "menus",
  "trim",
  "warning1",
  "warning2",
  "warning3",
  "midtrim",
  "endtrim",
  "tada",
  "midpot",
  "mixwarn1",
  "mixwarn2",
  "mixwarn3",
  "timerlt3",
  "timer10",
  "timer20",
  "timer30"
#if 0
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
#endif
};

uint32_t sdAvailableAudioFiles;

#if defined(SDCARD)
void retrieveAvailableAudioFiles()
{
  FILINFO info;
  char filename[32] = SYSTEM_SOUNDS_PATH "/";

  assert(sizeof(audioFilenames)==AU_FRSKY_FIRST*sizeof(char *));
  assert(sizeof(sdAvailableAudioFiles)*8 > AU_FRSKY_FIRST);

  sdAvailableAudioFiles = 0;

  for (uint32_t i=0; i<AU_FRSKY_FIRST; i++) {
    strcpy(filename+sizeof(SYSTEM_SOUNDS_PATH), audioFilenames[i]);
    strcat(filename+sizeof(SYSTEM_SOUNDS_PATH), SOUNDS_EXT);
    if (f_stat(filename, &info) == FR_OK)
      sdAvailableAudioFiles |= ((uint32_t)1 << i);
  }
}

inline bool isAudioFileAvailable(uint8_t i, char * filename)
{
  if (sdAvailableAudioFiles & ((uint32_t)1 << i)) {
    strcpy(filename+sizeof(SYSTEM_SOUNDS_PATH), audioFilenames[i]);
    strcat(filename+sizeof(SYSTEM_SOUNDS_PATH), SOUNDS_EXT);
    return true;
  }
  else {
    return false;
  }
}
#else
#define isAudioFileAvailable(i, f) false
#endif

uint16_t alawTable[256];

#define         SIGN_BIT        (0x80)      /* Sign bit for a A-law byte. */
#define         QUANT_MASK      (0xf)       /* Quantization field mask. */
#define         SEG_SHIFT       (4)         /* Left shift for segment number. */
#define         SEG_MASK        (0x70)      /* Segment field mask. */

static short alaw2linear(unsigned char a_val)
{
  int t;
  int seg;

  a_val ^= 0x55;

  t = a_val & QUANT_MASK;
  seg = ((unsigned)a_val & SEG_MASK) >> SEG_SHIFT;
  if(seg) t= (t + t + 1 + 32) << (seg + 2);
  else    t= (t + t + 1     ) << 3;

  return (a_val & SIGN_BIT) ? t : -t;
}

void alawInit()
{
  for (uint32_t i=0; i<256; i++)
    alawTable[i] = (0x8000 + alaw2linear(i)) >> 4;
}

AudioQueue audioQueue;

AudioQueue::AudioQueue()
{
  memset(this, 0, sizeof(AudioQueue));
  prioIdx = -1;
}

void audioTimerHandle(void)
{
  CoSetFlag(audioFlag);
}

// TODO Should be here!
extern uint16_t Sine_values[];

#define WAV_HEADER_SIZE 44

#define CODEC_ID_PCM_S16LE  1
#define CODEC_ID_PCM_ALAW   6
uint8_t pcmCodec;
uint16_t pcmFreq = 8000;

void audioTask(void* pdata)
{
  while (1) {

    CoWaitForSingleFlag(audioFlag, 0);

    audioQueue.wakeup();

  }
}

#define WAV_BUFFER_SIZE 512
uint16_t wavSamplesArray[3*WAV_BUFFER_SIZE];
uint16_t * wavSamplesBuffer = &wavSamplesArray[0];
uint16_t * nextAudioData = NULL;
uint16_t nextAudioSize = 0;

void AudioQueue::wakeup()
{
#if defined(SDCARD) && !defined(SIMU)
  static FIL wavFile;
#endif

  if (prioIdx >= 0) {
    ridx = prioIdx;
    prioIdx = -1;
    toneStop();
    nextAudioData = NULL;
    memset(&current, 0, sizeof(current));
    if (state == AUDIO_PLAYING_WAV) {
      state = AUDIO_RESUMING;
      CoSetTmrCnt(audioTimer, 5/*10ms*/ + (WAV_BUFFER_SIZE * 1000) / pcmFreq, 0);
      CoStartTmr(audioTimer);
    }
    else if (state == AUDIO_PLAYING_TONE) {
      state = AUDIO_RESUMING;
      CoSetTmrCnt(audioTimer, 5/*10ms*/, 0);
      CoStartTmr(audioTimer);
    }
    else {
      CoSetFlag(audioFlag);
    }
  }
#if defined(SDCARD) && !defined(SIMU)
  else if (current.file[0]) {
    FRESULT result = FR_OK;
    UINT read = 0;
    if (current.file[1]) {
      pcmFreq = 8000;
      wavSamplesBuffer = wavSamplesArray;
      result = f_open(&wavFile, current.file, FA_OPEN_EXISTING | FA_READ);
      if (result == FR_OK) {
        result = f_read(&wavFile, (uint8_t *)wavSamplesArray, WAV_HEADER_SIZE, &read);
        if (result == FR_OK && read == WAV_HEADER_SIZE && !memcmp(wavSamplesArray, "RIFF", 4)) {
          pcmCodec = wavSamplesArray[10];
          pcmFreq = wavSamplesArray[12];
          if (pcmCodec != CODEC_ID_PCM_S16LE) {
            result = f_read(&wavFile, (uint8_t *)wavSamplesArray, 12, &read);
          }
        }
        else {
          result = FR_DENIED;
          state = AUDIO_SLEEPING;
          toneStop();
        }
      }
    }
    else {
      wavSamplesBuffer += WAV_BUFFER_SIZE;
      if (wavSamplesBuffer >= wavSamplesArray + 3*WAV_BUFFER_SIZE)
        wavSamplesBuffer = wavSamplesArray;
    }

    read = 0;
    uint16_t bufsize = (pcmCodec == CODEC_ID_PCM_S16LE ? WAV_BUFFER_SIZE*sizeof(uint16_t) : WAV_BUFFER_SIZE);
    if (result != FR_OK || f_read(&wavFile, (uint8_t *)wavSamplesBuffer, bufsize, &read) != FR_OK || read == 0) {
      toneStop();
      current.file[0] = 0;
      current.file[1] = 0;
      f_close(&wavFile);
      CoSetTmrCnt(audioTimer, (WAV_BUFFER_SIZE * 1000) / pcmFreq, 0);
      CoStartTmr(audioTimer);
    }

    if (pcmCodec == CODEC_ID_PCM_S16LE) {
      read /= 2;
      uint32_t i = 0;
      for (; i<read; i++)
        wavSamplesBuffer[i] = ((uint16_t)0x8000 + ((int16_t)(wavSamplesBuffer[i]))) >> 4;
      for (; i<WAV_BUFFER_SIZE; i++)
        wavSamplesBuffer[i] = 0x8000;
    }
    else if (pcmCodec == CODEC_ID_PCM_ALAW) {
      int32_t i;
      for (i=read-1; i>=0; i--)
        wavSamplesBuffer[i] = alawTable[((uint8_t *)wavSamplesBuffer)[i]];
      for (i=read; i<WAV_BUFFER_SIZE; i++)
        wavSamplesBuffer[i] = 0x8000;
    }

    read /= 2;
    register Dacc *dacptr = DACC;

    if (current.file[1]) {
      state = AUDIO_PLAYING_WAV;
      setFrequency(pcmFreq);
      dacptr->DACC_TPR = CONVERT_PTR(wavSamplesBuffer);
      dacptr->DACC_TCR = read;
      current.file[1] = 0;
      CoSetFlag(audioFlag);
    }
    else if (read) {
      if (dacptr->DACC_TNCR == 0) {
        dacptr->DACC_TNPR = CONVERT_PTR(wavSamplesBuffer);
        dacptr->DACC_TNCR = read;
        CoSetFlag(audioFlag);
      }
      else {
        nextAudioSize = read;
        nextAudioData = wavSamplesBuffer;
        toneStart();
      }
    }
  }
#endif
  else if (current.duration > 0) {
    if (state != AUDIO_PLAYING_TONE) {
      state = AUDIO_PLAYING_TONE;
      register Dacc *dacptr = DACC;
      dacptr->DACC_TPR = CONVERT_PTR(Sine_values);
      dacptr->DACC_TNPR = CONVERT_PTR(Sine_values);
      dacptr->DACC_TCR = 50 ;       // words, 100 16 bit values
      dacptr->DACC_TNCR = 50 ;      // words, 100 16 bit values
    }
    setFrequency(current.freq * 6100 / 2);
    if (current.freqIncr) {
      CoSetTmrCnt(audioTimer, 5/*10ms*/, 0);
      current.freq += current.freqIncr;
      current.duration--;
    }
    else {
      CoSetTmrCnt(audioTimer, current.duration*2, 0);
      current.duration = 0;
    }
    toneStart();
    CoStartTmr(audioTimer);
  }
  else if (current.pause > 0) {
    state = AUDIO_PLAYING_TONE;
    CoSetTmrCnt(audioTimer, current.pause*2, 0);
    current.pause = 0;
    toneStop();
    CoStartTmr(audioTimer);
  }
  else if (ridx != widx) {
    memcpy(&current, &fragments[ridx], sizeof(current));
    if (!fragments[ridx].repeat--) {
      ridx = (ridx + 1) % AUDIO_QUEUE_LENGTH;
    }
    CoSetFlag(audioFlag);
  }
  else if (background.duration > 0) {
    if (state != AUDIO_PLAYING_TONE) {
      state = AUDIO_PLAYING_TONE;
      register Dacc *dacptr = DACC;
      dacptr->DACC_TPR = CONVERT_PTR(Sine_values);
      dacptr->DACC_TNPR = CONVERT_PTR(Sine_values);
      dacptr->DACC_TCR = 50 ;       // words, 100 16 bit values
      dacptr->DACC_TNCR = 50 ;      // words, 100 16 bit values
    }
    CoSetTmrCnt(audioTimer, background.duration*2, 0);
    background.duration = 0;
    setFrequency(background.freq * 6100 / 2);
    toneStart();
    CoStartTmr(audioTimer);
  }
  else {
#ifndef SIMU
    CoEnterMutexSection(audioMutex);
#endif
    state = AUDIO_SLEEPING;
#ifndef SIMU
    CoLeaveMutexSection(audioMutex);
#endif
    toneStop();
  }
}

inline uint8_t getToneLength(uint8_t tLen)
{
  uint8_t result = tLen; // default
  if (g_eeGeneral.beeperLength < 0) {
    result /= (1-g_eeGeneral.beeperLength);
  }
  if (g_eeGeneral.beeperLength > 0) {
    result *= (1+g_eeGeneral.beeperLength);
  }
  return result;
}

void AudioQueue::pause(uint8_t tLen)
{
  play(0, 0, tLen); // a pause
}	

#ifndef SIMU
extern OS_MutexID audioMutex;
#endif

void AudioQueue::play(uint8_t tFreq, uint8_t tLen, uint8_t tPause, uint8_t tFlags, int8_t tFreqIncr)
{
#ifndef SIMU
  CoEnterMutexSection(audioMutex);
#endif

  if (tFlags & PLAY_SOUND_VARIO) {
    background.freq = tFreq;
    background.duration = tLen;
    background.pause = tPause;
    if (!busy()) {
      state = AUDIO_RESUMING;
      CoSetFlag(audioFlag);
    }
  }
  else {
    if (tFreq > 0) { //we dont add pitch if zero as this is a pause only event
      tFreq += g_eeGeneral.speakerPitch + BEEP_OFFSET; // add pitch compensator
    }
    tLen = getToneLength(tLen);

    uint8_t next_widx = (widx + 1) % AUDIO_QUEUE_LENGTH;
    if (next_widx != ridx) {
      AudioFragment & fragment = fragments[widx];
      memset(&fragment, 0, sizeof(fragment));
      fragment.freq = tFreq;
      fragment.duration = tLen;
      fragment.pause = tPause;
      fragment.repeat = tFlags & 0x0f;
      fragment.freqIncr = tFreqIncr;
      if (tFlags & PLAY_NOW)
        prioIdx = widx;
      widx = next_widx;
      if (!busy()) {
        state = AUDIO_RESUMING;
        CoSetFlag(audioFlag);
      }
    }
  }

#ifndef SIMU
  CoLeaveMutexSection(audioMutex);
#endif
}

void AudioQueue::playFile(const char *filename, uint8_t flags)
{
#ifdef SIMU
  printf("playFile(\"%s\"\n", filename); fflush(stdout);
#else
  CoEnterMutexSection(audioMutex);
#endif

  uint8_t next_widx = (widx + 1) % AUDIO_QUEUE_LENGTH;
  if (next_widx != ridx) {
    AudioFragment & fragment = fragments[widx];
    memset(&fragment, 0, sizeof(fragment));
    strcpy(fragment.file, filename);
    fragment.repeat = flags & 0x0f;
    if (flags & PLAY_NOW)
      prioIdx = widx;
    widx = next_widx;
    if (!busy()) {
      state = AUDIO_RESUMING;
      CoSetFlag(audioFlag);
    }
  }

#ifndef SIMU
  CoLeaveMutexSection(audioMutex);
#endif
}

void audioEvent(uint8_t e, uint8_t f)
{
#ifdef SDCARD
  char filename[32] = SYSTEM_SOUNDS_PATH "/";
#endif

#ifdef HAPTIC
  haptic.event(e); //do this before audio to help sync timings
#endif

  if (g_eeGeneral.flashBeep && (e <= AU_ERROR || e >= AU_WARNING1)) {
    if (g_LightOffCounter < FLASH_DURATION)
      g_LightOffCounter = FLASH_DURATION;
  }

  if (g_eeGeneral.beeperMode>0 || (g_eeGeneral.beeperMode==0 && e>=AU_TRIM_MOVE) || (g_eeGeneral.beeperMode>=-1 && e<=AU_ERROR)) {
#ifdef SDCARD
    if (e < AU_FRSKY_FIRST && isAudioFileAvailable(e, filename)) {
      audioQueue.playFile(filename);
    }
    else
#endif
    if (e < AU_FRSKY_FIRST || !audioQueue.busy()) {
      switch (e) {
        // inactivity timer alert
        case AU_INACTIVITY:
          audioQueue.play(70, 20, 4, 2|PLAY_NOW);
          break;
        // low battery in tx
        case AU_TX_BATTERY_LOW:
          if (!audioQueue.busy()) {
            audioQueue.play(60, 40, 6, 2, 1);
            audioQueue.play(80, 40, 6, 2, -1);
          }
          break;
        // error
        case AU_ERROR:
          audioQueue.play(BEEP_DEFAULT_FREQ, 50, 2, PLAY_NOW);
          break;
        // keypad up (seems to be used when going left/right through system menu options. 0-100 scales etc)
        case AU_KEYPAD_UP:
          audioQueue.play(BEEP_KEY_UP_FREQ, 20, 2, PLAY_NOW);
          break;
        // keypad down (seems to be used when going left/right through system menu options. 0-100 scales etc)
        case AU_KEYPAD_DOWN:
          audioQueue.play(BEEP_KEY_DOWN_FREQ, 20, 2, PLAY_NOW);
          break;
        // menu display (also used by a few generic beeps)
        case AU_MENUS:
          audioQueue.play(BEEP_DEFAULT_FREQ, 20, 4, PLAY_NOW);
          break;
        // trim move
        case AU_TRIM_MOVE:
          audioQueue.play(f, 12, 2, PLAY_NOW);
          break;
        // trim center
        case AU_TRIM_MIDDLE:
          audioQueue.play(f, 20, 4, PLAY_NOW);
          break;
        // trim center
        case AU_TRIM_END:
          audioQueue.play(f, 20, 4, PLAY_NOW);
          break;          
        // warning one
        case AU_WARNING1:
          audioQueue.play(BEEP_DEFAULT_FREQ, 20, 2, PLAY_NOW);
          break;
        // warning two
        case AU_WARNING2:
          audioQueue.play(BEEP_DEFAULT_FREQ, 40, 2, PLAY_NOW);
          break;
        // warning three
        case AU_WARNING3:
          audioQueue.play(BEEP_DEFAULT_FREQ, 50, 2, PLAY_NOW);
          break;
        // startup tune
        case AU_TADA:
          audioQueue.play(50, 20, 10);
          audioQueue.play(90, 20, 10);
          audioQueue.play(110, 10, 8, 2);
          break;
        // pot/stick center
        case AU_POT_STICK_MIDDLE:
          audioQueue.play(BEEP_DEFAULT_FREQ + 50, 20, 2, PLAY_NOW);
          break;
        // mix warning 1
        case AU_MIX_WARNING_1:
          audioQueue.play(BEEP_DEFAULT_FREQ + 50, 12, 0, PLAY_NOW);
          break;
        // mix warning 2
        case AU_MIX_WARNING_2:
          audioQueue.play(BEEP_DEFAULT_FREQ + 52, 12, 0, PLAY_NOW);
          break;
        // mix warning 3
        case AU_MIX_WARNING_3:
          audioQueue.play(BEEP_DEFAULT_FREQ + 54, 12, 0, PLAY_NOW);
          break;
        // time 30 seconds left
        case AU_TIMER_30:
          audioQueue.play(BEEP_DEFAULT_FREQ + 50, 30, 6, 2|PLAY_NOW);
          break;
        // time 20 seconds left
        case AU_TIMER_20:
          audioQueue.play(BEEP_DEFAULT_FREQ + 50, 30, 6, 1|PLAY_NOW);
          break;
        // time 10 seconds left
        case AU_TIMER_10:
          audioQueue.play(BEEP_DEFAULT_FREQ + 50, 30, 6, PLAY_NOW);
          break;
        // time <3 seconds left
        case AU_TIMER_LT3:
          audioQueue.play(BEEP_DEFAULT_FREQ + 50, 30, 6, PLAY_NOW);
          break;
        case AU_FRSKY_WARN1:
          audioQueue.play(BEEP_DEFAULT_FREQ+20,30,10,2);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_WARN2:
          audioQueue.play(BEEP_DEFAULT_FREQ+30,30,10,2);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_CHEEP:
          audioQueue.play(BEEP_DEFAULT_FREQ+30,20,4,2,2);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_RING:
          audioQueue.play(BEEP_DEFAULT_FREQ+25,10,4,10);
          audioQueue.play(BEEP_DEFAULT_FREQ+25,10,20,1);
          audioQueue.play(BEEP_DEFAULT_FREQ+25,10,4,10);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_SCIFI:
          audioQueue.play(80,20,6,2,-1);
          audioQueue.play(60,20,6,2,1);
          audioQueue.play(70,20,2,0);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_ROBOT:
          audioQueue.play(70,10,2,1);
          audioQueue.play(50,30,4,1);
          audioQueue.play(80,30,4,1);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_CHIRP:
          audioQueue.play(BEEP_DEFAULT_FREQ+40,10,2,2);
          audioQueue.play(BEEP_DEFAULT_FREQ+54,10,2,3);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_TADA:
          audioQueue.play(50,10,10);
          audioQueue.play(90,10,10);
          audioQueue.play(110,6,8,2);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_CRICKET:
          audioQueue.play(80,10,20,3);
          audioQueue.play(80,10,40,1);
          audioQueue.play(80,10,20,3);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_SIREN:
          audioQueue.play(10,40,10,2,1);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_ALARMC:
          audioQueue.play(50,8,20,2);
          audioQueue.play(70,16,40,1);
          audioQueue.play(50,16,20,2);
          audioQueue.play(70,8,40,1);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_RATATA:
          audioQueue.play(BEEP_DEFAULT_FREQ+50,10,20,10);
          break;
        case AU_FRSKY_TICK:
          audioQueue.play(BEEP_DEFAULT_FREQ+50,10,100,2);
          audioQueue.pause(200);
          break;
        default:
          break;
      }
    }
  }
}

void pushPrompt(uint16_t prompt)
{
#if defined(SDCARD)

  char filename[] = SYSTEM_SOUNDS_PATH "/0000" SOUNDS_EXT;

  for (int8_t i=3; i>=0; i--) {
    filename[sizeof(SYSTEM_SOUNDS_PATH)+i] = '0' + (prompt%10);
    prompt /= 10;
  }

  audioQueue.playFile(filename);

#endif

}
