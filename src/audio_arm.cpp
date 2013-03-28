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

// Must NOT be in flash, PDC needs a RAM source.
// Amplitude reduced to 30% to allow for voice volume
uint16_t Sine_values[] =
{
  2048,2085,2123,2160,2197,2233,2268,2303,2336,2369,
  2400,2430,2458,2485,2510,2533,2554,2573,2590,2605,
  2618,2629,2637,2643,2646,2648,2646,2643,2637,2629,
  2618,2605,2590,2573,2554,2533,2510,2485,2458,2430,
  2400,2369,2336,2303,2268,2233,2197,2160,2123,2085,
  2048,2010,1972,1935,1898,1862,1826,1792,1758,1726,
  1695,1665,1637,1610,1585,1562,1541,1522,1505,1490,
  1477,1466,1458,1452,1448,1448,1448,1452,1458,1466,
  1477,1490,1505,1522,1541,1562,1585,1610,1637,1665,
  1695,1726,1758,1792,1826,1862,1898,1935,1972,2010
};

#if defined(SDCARD)
const char * audioFilenames[] = {
  "inactiv",
  "lowbatt",
  "highmah",
  "hightemp",
  "thralert",
  "swalert",
  "eebad",
  "eeformat",
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
};


uint32_t sdAvailableSystemAudioFiles = 0;
uint8_t sdAvailablePhaseAudioFiles[MAX_PHASES] = { 0 };
uint8_t sdAvailableMixerAudioFiles[MAX_MIXERS] = { 0 };

void refreshSystemAudioFiles()
{
  FILINFO info;
#if _USE_LFN
  TCHAR lfn[_MAX_LFN + 1];
  info.lfname = lfn;
  info.lfsize = sizeof(lfn);
#endif

  char filename[32] = SYSTEM_SOUNDS_PATH "/";

  assert(sizeof(audioFilenames)==AU_FRSKY_FIRST*sizeof(char *));
  assert(sizeof(sdAvailableSystemAudioFiles)*8 > AU_FRSKY_FIRST);

  uint32_t availableAudioFiles = 0;

  for (uint32_t i=0; i<AU_FRSKY_FIRST; i++) {
    strcpy(filename+sizeof(SYSTEM_SOUNDS_PATH), audioFilenames[i]);
    strcat(filename+sizeof(SYSTEM_SOUNDS_PATH), SOUNDS_EXT);
    if (f_stat(filename, &info) == FR_OK)
      availableAudioFiles |= ((uint32_t)1 << i);
  }

  sdAvailableSystemAudioFiles = availableAudioFiles;
}

const char * suffixes[] = { "-OFF", "-ON", /*"-BG", */NULL };

inline uint8_t getAvailableFiles(char *prefix, FILINFO &info, char *filename)
{
  uint8_t result = 0;

  for (uint8_t i=0; suffixes[i]; i++) {
    strcpy(prefix, suffixes[i]);
    strcat(prefix, SOUNDS_EXT);
    if (f_stat(filename, &info) == FR_OK)
      result |= ((uint8_t)1 << i);
  }

  return result;
}

void refreshModelAudioFiles()
{
  FILINFO info;
#if _USE_LFN
  TCHAR lfn[_MAX_LFN + 1];
  info.lfname = lfn;
  info.lfsize = sizeof(lfn);
#endif

  char filename[AUDIO_FILENAME_MAXLEN+1] = SOUNDS_PATH "/";

  if (sdMounted()) {
    char *buf = strcat_modelname(&filename[sizeof(SOUNDS_PATH)], g_eeGeneral.currModel);
    *buf++ = '/';

    for (uint32_t i=0; i<MAX_PHASES; i++) {
      char *tmp = strcat_phasename(buf, i);
      sdAvailablePhaseAudioFiles[i] = (tmp != buf ? getAvailableFiles(strcat_phasename(buf, i), info, filename) : 0);
    }

    /* for (uint32_t i=0; i<MAX_MIXERS; i++) {
      char *tmp = strcat_mixername_nodefault(buf, i);
      sdAvailableMixerAudioFiles[i] = (tmp != buf ? getAvailableFiles(tmp, info, filename) : 0);
    } */
  }
}

bool isAudioFileAvailable(uint32_t i, char * filename)
{
  uint8_t category = (i >> 24);
  uint8_t index = (i >> 16) & 0xFF;
  uint8_t event = i & 0xFF;

#if 0
  printf("isAudioFileAvailable(%08x)\n", i); fflush(stdout);
#endif

  if (category == SYSTEM_AUDIO_CATEGORY) {
    if (sdAvailableSystemAudioFiles & ((uint32_t)1 << event)) {
      strcpy(filename, SYSTEM_SOUNDS_PATH "/");
      strcpy(filename+sizeof(SYSTEM_SOUNDS_PATH), audioFilenames[i]);
      strcat(filename+sizeof(SYSTEM_SOUNDS_PATH), SOUNDS_EXT);
      return true;
    }
  }
  else if (category == PHASE_AUDIO_CATEGORY) {
    if (sdAvailablePhaseAudioFiles[index] & ((uint32_t)1 << event)) {
      strcpy(filename, SOUNDS_PATH "/");
      char *str = strcat_modelname(filename+sizeof(SOUNDS_PATH), g_eeGeneral.currModel);
      *str++ = '/';
      char * tmp = strcat_phasename(str, index);
      if (tmp != str) {
        strcpy(tmp, suffixes[event]);
        strcat(tmp, SOUNDS_EXT);
        return true;
      }
    }
  }
  else if (category == MIXER_AUDIO_CATEGORY) {
    if (sdAvailableMixerAudioFiles[index] & ((uint32_t)1 << event)) {
      strcpy(filename, SOUNDS_PATH "/");
      char *str = strcat_modelname(filename+sizeof(SOUNDS_PATH), g_eeGeneral.currModel);
      *str++ = '/';
      char * tmp = strcat_mixername(str, index);
      if (tmp != str) {
        strcpy(tmp, suffixes[event]);
        strcat(tmp, SOUNDS_EXT);
        return true;
      }
    }
  }

  return false;
}
#else
#define isAudioFileAvailable(i, f) false
#endif

uint16_t alawTable[256];
uint16_t ulawTable[256];

#define         SIGN_BIT        (0x80)      /* Sign bit for a A-law byte. */
#define         QUANT_MASK      (0xf)       /* Quantization field mask. */
#define         SEG_SHIFT       (4)         /* Left shift for segment number. */
#define         SEG_MASK        (0x70)      /* Segment field mask. */
#define         BIAS            (0x84)      /* Bias for linear code. */

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

static short ulaw2linear(unsigned char u_val)
{
  int t;

  /* Complement to obtain normal u-law value. */
  u_val = ~u_val;

  /*
   * Extract and bias the quantization bits. Then
   * shift up by the segment number and subtract out the bias.
   */
  t = ((u_val & QUANT_MASK) << 3) + BIAS;
  t <<= ((unsigned)u_val & SEG_MASK) >> SEG_SHIFT;

  return (u_val & SIGN_BIT) ? (BIAS - t) : (t - BIAS);
}

void codecsInit()
{
  for (uint32_t i=0; i<256; i++) {
    alawTable[i] = (0x8000 + alaw2linear(i)) >> 4;
    ulawTable[i] = (0x8000 + ulaw2linear(i)) >> 4;
  }
}

AudioQueue audioQueue;

AudioQueue::AudioQueue()
{
  memset(this, 0, sizeof(AudioQueue));
  prioIdx = -1;
}

void AudioQueue::start()
{
  state = AUDIO_SLEEPING;
  CoSetFlag(audioFlag);
}

void audioTimerHandle(void)
{
  CoSetFlag(audioFlag);
}

#define CODEC_ID_PCM_S16LE  1
#define CODEC_ID_PCM_ALAW   6
#define CODEC_ID_PCM_MULAW  7

#ifndef SIMU
void audioTask(void* pdata)
{
  while (!audioQueue.started()) {
    CoWaitForSingleFlag(audioFlag, 0);
  }

#if defined(SDCARD)
  if (!unexpectedShutdown) {
    codecsInit();
    sdInit();
    AUDIO_TADA();
  }
#endif  

  while (1) {

    CoWaitForSingleFlag(audioFlag, 0);

    audioQueue.wakeup();

  }
}
#endif

#define WAV_BUFFER_SIZE 512
uint16_t wavSamplesArray[3*WAV_BUFFER_SIZE];
uint16_t * wavSamplesBuffer = &wavSamplesArray[0];
uint16_t * nextAudioData = NULL;
uint16_t nextAudioSize = 0;

#if defined(SDCARD) && !defined(SIMU)

#define RIFF_CHUNK_SIZE 12

void AudioQueue::sdWakeup(AudioContext & context)
{
  if (!nextAudioData) {
    AudioFragment & fragment = context.fragment;
    FRESULT result = FR_OK;
    UINT read = 0;
    if (fragment.file[1]) {
      result = f_open(&context.wavFile, fragment.file, FA_OPEN_EXISTING | FA_READ);
      fragment.file[1] = 0;
      if (result == FR_OK) {
        result = f_read(&context.wavFile, (uint8_t *)wavSamplesBuffer, RIFF_CHUNK_SIZE+8, &read);
        if (result == FR_OK && read == RIFF_CHUNK_SIZE+8 && !memcmp(wavSamplesBuffer, "RIFF", 4) && !memcmp(wavSamplesBuffer+4/*short*/, "WAVEfmt ", 8)) {
          uint32_t size = *((uint32_t *)(wavSamplesBuffer+8/*short*/));
          result = (size < 256 ? f_read(&context.wavFile, (uint8_t *)wavSamplesBuffer, size+8, &read) : FR_DENIED);
          if (result == FR_OK && read == size+8) {
            context.pcmCodec = wavSamplesBuffer[0];
            context.pcmFreq = wavSamplesBuffer[2];
            uint32_t *wavSamplesPtr = (uint32_t *)(wavSamplesBuffer + size/2);
            uint32_t size = wavSamplesPtr[1];
            while (result == FR_OK && memcmp(wavSamplesPtr, "data", 4) != 0) {
              result = (size < 256 ? f_read(&context.wavFile, (uint8_t *)wavSamplesBuffer, size+8, &read) : FR_DENIED);
              if (read != size+8) result = FR_DENIED;
              wavSamplesPtr = (uint32_t *)(wavSamplesBuffer + size/2);
              size = wavSamplesPtr[1];
            }
            context.wavSize = size;
          }
          else {
            result = FR_DENIED;
          }
        }
        else {
          result = FR_DENIED;
        }
      }
    }

    read = 0;
    uint16_t bufsize = (context.pcmCodec == CODEC_ID_PCM_S16LE ? WAV_BUFFER_SIZE * sizeof(uint16_t) : WAV_BUFFER_SIZE);
    if (result == FR_OK) {
      result = f_read(&context.wavFile, (uint8_t *)wavSamplesBuffer, bufsize, &read);
    }

    if (result != FR_OK) {
      dacStop();
      memset(&fragment, 0, sizeof(fragment));
      f_close(&context.wavFile);
      CoSetTmrCnt(audioTimer, (WAV_BUFFER_SIZE * 1000) / context.pcmFreq, 0);
      CoStartTmr(audioTimer);
      CoClearFlag(audioFlag);
      state = AUDIO_RESUMING;
    }
    else {
      if (read > context.wavSize)
        read = context.wavSize;
      context.wavSize -= read;

      if (read != bufsize) {
        memset(&fragment, 0, sizeof(fragment));
        f_close(&context.wavFile);
      }

      setFrequency(context.pcmFreq);

      if (context.pcmCodec == CODEC_ID_PCM_S16LE) {
        read /= 2;
        uint32_t i = 0;
        for (; i < read; i++)
          wavSamplesBuffer[i] = ((uint16_t) 0x8000 + ((int16_t) (wavSamplesBuffer[i]))) >> 4;
        for (; i < WAV_BUFFER_SIZE; i++)
          wavSamplesBuffer[i] = 0x8000;
      }
      else if (context.pcmCodec == CODEC_ID_PCM_ALAW) {
        int32_t i;
        for (i = read - 1; i >= 0; i--)
          wavSamplesBuffer[i] = alawTable[((uint8_t *) wavSamplesBuffer)[i]];
        for (i = read; i < WAV_BUFFER_SIZE; i++)
          wavSamplesBuffer[i] = 0x8000;
      }
      else if (context.pcmCodec == CODEC_ID_PCM_MULAW) {
        int32_t i;
        for (i = read - 1; i >= 0; i--)
          wavSamplesBuffer[i] = ulawTable[((uint8_t *) wavSamplesBuffer)[i]];
        for (i = read; i < WAV_BUFFER_SIZE; i++)
          wavSamplesBuffer[i] = 0x8000;
      }

      read /= 2;

      if (read) {
        state = AUDIO_PLAYING_WAV;
        if (dacQueue(wavSamplesBuffer, read)) {
          CoSetFlag(audioFlag);
        }
        else {
          nextAudioSize = read;
          nextAudioData = wavSamplesBuffer;
#if defined(PCBSKY9X)
          dacStart();
#endif
          CoSetTmrCnt(audioTimer, (WAV_BUFFER_SIZE * 500) / context.pcmFreq, 0);
          CoStartTmr(audioTimer);
        }
        wavSamplesBuffer += WAV_BUFFER_SIZE;
        if (wavSamplesBuffer >= wavSamplesArray + 3 * WAV_BUFFER_SIZE)
          wavSamplesBuffer = wavSamplesArray;
      }
      else {
        state = AUDIO_RESUMING;
        CoSetFlag(audioFlag);
      }
    }
  }
  else {
    CoSetTmrCnt(audioTimer, 5/*10ms*/, 0);
    CoStartTmr(audioTimer);
  }
}
#endif

void AudioQueue::wakeup()
{
  AudioFragment & fragment = currentContext.fragment;

  if (prioIdx >= 0) {
    memset(&fragment, 0, sizeof(fragment));
  }

#if defined(SDCARD) && !defined(SIMU)
  if (fragment.file[0]) {
    sdWakeup(currentContext);
  }
  else
#endif
  if (fragment.duration > 0) {
    if (state != AUDIO_PLAYING_TONE) {
      state = AUDIO_PLAYING_TONE;
      dacFill(Sine_values, 50/*100 samples*/);
    }

    setFrequency(fragment.freq * 6100 / 4);

    if (fragment.freqIncr) {
      CoSetTmrCnt(audioTimer, 2, 0);
      fragment.freq += fragment.freqIncr;
      fragment.duration--;
    }
    else {
      CoSetTmrCnt(audioTimer, fragment.duration*2, 0);
      fragment.duration = 0;
    }
    dacStart();
    CoClearFlag(audioFlag);
    CoStartTmr(audioTimer);
  }
  else if (fragment.pause > 0) {
    state = AUDIO_PLAYING_TONE;
    CoSetTmrCnt(audioTimer, fragment.pause*2, 0);
    fragment.pause = 0;
    dacStop();
    CoClearFlag(audioFlag);
    CoStartTmr(audioTimer);
  }
#if defined(SDCARD) && !defined(SIMU)
  else if (ridx == widx && prioIdx < 0 && backgroundContext.fragment.file[0]) {
    if (!isFunctionActive(FUNC_BACKGND_MUSIC_PAUSE)) {
      sdWakeup(backgroundContext);
    }
    else {
      CoSetTmrCnt(audioTimer, 5/*10ms*/, 0);
      CoStartTmr(audioTimer);
    }
  }
#endif
  else if (ridx == widx && prioIdx < 0 && backgroundContext.fragment.duration > 0) {
    if (state != AUDIO_PLAYING_TONE) {
      state = AUDIO_PLAYING_TONE;
      dacFill(Sine_values, 50/*100 samples*/);
    }
    CoSetTmrCnt(audioTimer, backgroundContext.fragment.duration*2, 0);
    backgroundContext.fragment.duration = 0;
    setFrequency(backgroundContext.fragment.freq * 6100 / 4);
    dacStart();
    CoClearFlag(audioFlag);
    CoStartTmr(audioTimer);
  }
  else {
    CoEnterMutexSection(audioMutex);

    if (ridx != widx) {
      if (prioIdx >= 0) {
        ridx = prioIdx;
        prioIdx = -1;
        dacStop();
        CoClearFlag(audioFlag);
        nextAudioData = NULL;
        if (state == AUDIO_PLAYING_WAV) {
          state = AUDIO_RESUMING;
          CoSetTmrCnt(audioTimer, 1/*2ms*/ + (WAV_BUFFER_SIZE * 1000) / getFrequency(), 0);
          CoStartTmr(audioTimer);
        }
        else if (state == AUDIO_PLAYING_TONE) {
          state = AUDIO_RESUMING;
          CoSetTmrCnt(audioTimer, 1/*2ms*/, 0);
          CoStartTmr(audioTimer);
        }
        else {
          CoSetFlag(audioFlag);
        }
      }
      else {
        CoSetFlag(audioFlag);
      }

      memcpy(&fragment, &fragments[ridx], sizeof(fragment));
      if (!fragments[ridx].repeat--) {
        ridx = (ridx + 1) % AUDIO_QUEUE_LENGTH;
      }
    }
    else {
      memset(&fragment, 0, sizeof(fragment));
      state = AUDIO_SLEEPING;
      dacStop();
    }

    CoLeaveMutexSection(audioMutex);
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

bool AudioQueue::isPlaying(uint8_t id)
{
  if (currentContext.fragment.id == id || backgroundContext.fragment.id == id)
    return true;

  uint8_t i = ridx;
  while (i != widx) {
    AudioFragment & fragment = fragments[i];
    if (fragment.id == id)
      return true;
    i = (i + 1) % AUDIO_QUEUE_LENGTH;
  }
  return false;
}

#ifndef SIMU
extern OS_MutexID audioMutex;
#endif

void AudioQueue::play(uint16_t tFreq, uint8_t tLen, uint8_t tPause, uint8_t tFlags, int8_t tFreqIncr)
{
  CoEnterMutexSection(audioMutex);

  if (tFlags & PLAY_BACKGROUND) {
    backgroundContext.fragment.freq = tFreq;
    backgroundContext.fragment.duration = tLen;
    backgroundContext.fragment.pause = tPause;
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

  CoLeaveMutexSection(audioMutex);
}

#if defined(SDCARD)
void AudioQueue::playFile(const char *filename, uint8_t flags, uint8_t id)
{
#if defined(SIMU)
  printf("playFile(\"%s\", flags=%x, id=%d)\n", filename, flags, id);
  fflush(stdout);
#else

  if (!sdMounted())
    return;

  CoEnterMutexSection(audioMutex);

  if (flags & PLAY_BACKGROUND) {
    memset(&backgroundContext.fragment, 0, sizeof(AudioFragment));
    strcpy(backgroundContext.fragment.file, filename);
    backgroundContext.fragment.id = id;
    if (!busy()) {
      state = AUDIO_RESUMING;
      CoSetFlag(audioFlag);
    }
  }
  else {
    uint8_t next_widx = (widx + 1) % AUDIO_QUEUE_LENGTH;
    if (next_widx != ridx) {
      AudioFragment & fragment = fragments[widx];
      memset(&fragment, 0, sizeof(fragment));
      strcpy(fragment.file, filename);
      fragment.repeat = flags & 0x0f;
      fragment.id = id;
      if (flags & PLAY_NOW)
        prioIdx = widx;
      widx = next_widx;
      if (!busy()) {
        state = AUDIO_RESUMING;
        CoSetFlag(audioFlag);
      }
    }
  }

  CoLeaveMutexSection(audioMutex);
#endif
}

void AudioQueue::stopPlay(uint8_t id)
{
#if defined(SIMU)
  printf("stopPlay(id=%d)\n", id); fflush(stdout);
#endif

  // For the moment it's only needed to stop the background music
  if (backgroundContext.fragment.id == id) {
    memset(&backgroundContext.fragment, 0, sizeof(AudioFragment));
  }
}

void AudioQueue::stopSD()
{
  sdAvailableSystemAudioFiles = 0;
  reset();
  play(0, 0, 100, PLAY_NOW);        // insert a 100ms pause
}

#endif

void AudioQueue::reset()
{
  CoEnterMutexSection(audioMutex);
  widx = ridx;                      // clean the queue
  memset(&currentContext.fragment, 0, sizeof(AudioFragment));
  memset(&backgroundContext.fragment, 0, sizeof(AudioFragment));
  CoLeaveMutexSection(audioMutex);
}

void audioEvent(uint8_t e, uint8_t f)
{
#if defined(SDCARD)
  char filename[AUDIO_FILENAME_MAXLEN+1];
#endif

#if defined(HAPTIC)
  haptic.event(e); //do this before audio to help sync timings
#endif

  if (g_eeGeneral.flashBeep && (e <= AU_ERROR || e >= AU_WARNING1)) {
    if (lightOffCounter < FLASH_DURATION)
      lightOffCounter = FLASH_DURATION;
  }

  if (g_eeGeneral.beeperMode>0 || (g_eeGeneral.beeperMode==0 && e>=AU_TRIM_MOVE) || (g_eeGeneral.beeperMode>=-1 && e<=AU_ERROR)) {
#if defined(SDCARD)
    if (e < AU_FRSKY_FIRST && isAudioFileAvailable(e, filename)) {
      audioQueue.playFile(filename);
    }
    else
#endif
    if (e < AU_FRSKY_FIRST || audioQueue.empty()) {
      switch (e) {
        // inactivity timer alert
        case AU_INACTIVITY:
          audioQueue.play(140, 20, 4, 2|PLAY_NOW);
          break;
        // low battery in tx
        case AU_TX_BATTERY_LOW:
          if (!audioQueue.busy()) {
            audioQueue.play(120, 40, 6, 2, 1);
            audioQueue.play(160, 40, 6, 2, -1);
          }
          break;
        case AU_TX_MAH_HIGH:
          if (!audioQueue.busy()) {
            // TODO Rob something better here?
            audioQueue.play(120, 40, 6, 2, 1);
            audioQueue.play(160, 40, 6, 2, -1);
          }
          break;
        case AU_TX_TEMP_HIGH:
          if (!audioQueue.busy()) {
            // TODO Rob something better here?
            audioQueue.play(120, 40, 6, 2, 1);
            audioQueue.play(160, 40, 6, 2, -1);
          }
          break;
        // error
#if defined(VOICE)
        case AU_THROTTLE_ALERT:
        case AU_SWITCH_ALERT:
#endif
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
        // pot/stick center
        case AU_POT_STICK_MIDDLE:
          audioQueue.play(BEEP_DEFAULT_FREQ+100, 20, 2, PLAY_NOW);
          break;
        // mix warning 1
        case AU_MIX_WARNING_1:
          audioQueue.play(BEEP_DEFAULT_FREQ+96, 12, 0);
          break;
        // mix warning 2
        case AU_MIX_WARNING_2:
          audioQueue.play(BEEP_DEFAULT_FREQ+104, 12, 8, PLAY_REPEAT(1));
          break;
        // mix warning 3
        case AU_MIX_WARNING_3:
          audioQueue.play(BEEP_DEFAULT_FREQ+112, 12, 8, PLAY_REPEAT(2));
          break;
        // time 30 seconds left
        case AU_TIMER_30:
          audioQueue.play(BEEP_DEFAULT_FREQ, 30, 6, PLAY_REPEAT(2)|PLAY_NOW);
          break;
        // time 20 seconds left
        case AU_TIMER_20:
          audioQueue.play(BEEP_DEFAULT_FREQ, 30, 6, PLAY_REPEAT(1)|PLAY_NOW);
          break;
        // time 10 seconds left
        case AU_TIMER_10:
          audioQueue.play(BEEP_DEFAULT_FREQ, 30, 6, PLAY_NOW);
          break;
        // time <3 seconds left
        case AU_TIMER_LT3:
          audioQueue.play(BEEP_DEFAULT_FREQ+10, 30, 6, PLAY_NOW);
          break;
        case AU_FRSKY_BEEP1:
          audioQueue.play(BEEP_DEFAULT_FREQ, 15, 2);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_BEEP2:
          audioQueue.play(BEEP_DEFAULT_FREQ, 30, 2);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_BEEP3:
          audioQueue.play(BEEP_DEFAULT_FREQ, 50, 2);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_WARN1:
          audioQueue.play(BEEP_DEFAULT_FREQ+40, 30, 10, 2);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_WARN2:
          audioQueue.play(BEEP_DEFAULT_FREQ+60,30,10,2);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_CHEEP:
          audioQueue.play(BEEP_DEFAULT_FREQ+60,20,4,2,2);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_RING:
          audioQueue.play(BEEP_DEFAULT_FREQ+50,10,4,10);
          audioQueue.play(BEEP_DEFAULT_FREQ+50,10,20,1);
          audioQueue.play(BEEP_DEFAULT_FREQ+50,10,4,10);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_SCIFI:
          audioQueue.play(160,20,6,2,-1);
          audioQueue.play(120,20,6,2,1);
          audioQueue.play(140,20,2,0);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_ROBOT:
          audioQueue.play(140,10,2,1);
          audioQueue.play(100,30,4,1);
          audioQueue.play(160,30,4,1);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_CHIRP:
          audioQueue.play(BEEP_DEFAULT_FREQ+80,10,2,2);
          audioQueue.play(BEEP_DEFAULT_FREQ+108,10,2,3);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_TADA:
          audioQueue.play(100,20,10);
          audioQueue.play(180,20,10);
          audioQueue.play(220,16,8,2);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_CRICKET:
          audioQueue.play(160,10,20,3);
          audioQueue.play(160,10,40,1);
          audioQueue.play(160,10,20,3);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_SIREN:
          audioQueue.play(20,40,10,2,1);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_ALARMC:
          audioQueue.play(100,8,20,2);
          audioQueue.play(140,16,40,1);
          audioQueue.play(100,16,20,2);
          audioQueue.play(140,8,40,1);
          audioQueue.pause(200);
          break;
        case AU_FRSKY_RATATA:
          audioQueue.play(BEEP_DEFAULT_FREQ+100,10,20,10);
          break;
        case AU_FRSKY_TICK:
          audioQueue.play(BEEP_DEFAULT_FREQ+100,10,100,2);
          audioQueue.pause(200);
          break;
        default:
          break;
      }
    }
  }
}

void pushPrompt(uint16_t prompt, uint8_t id)
{
#if defined(SDCARD)

  char filename[] = SYSTEM_SOUNDS_PATH "/0000" SOUNDS_EXT;

  for (int8_t i=3; i>=0; i--) {
    filename[sizeof(SYSTEM_SOUNDS_PATH)+i] = '0' + (prompt%10);
    prompt /= 10;
  }

  audioQueue.playFile(filename, 0, id);

#endif
}
