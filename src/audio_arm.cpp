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
#include <math.h>

extern OS_MutexID audioMutex;

const unsigned int toneVolumes[] = {
  4000,
  8000,
  16000,
  24000,
  32000
};


#if defined(SDCARD)
const char * audioFilenames[] = {
  "tada",
  "thralert",
  "swalert",
  "eebad",
  "eeformat",
  "lowbatt",
  "inactiv",
#if defined(PCBSKY9X)
  "highmah",
  "hightemp",
#endif
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
  "midpot",
  "mixwarn1",
  "mixwarn2",
  "mixwarn3",
  "timer10",
  "timer20",
  "timer30",
#if defined(PCBTARANIS)
  "a1_org",
  "a1_red",
  "a2_org",
  "a2_red",
  "rssi_org",
  "rssi_red",
  "swr_red",
#endif
  "telemko",
  "telemok"
};

uint64_t sdAvailableSystemAudioFiles = 0;
uint8_t  sdAvailablePhaseAudioFiles[MAX_PHASES] = { 0 };
uint8_t  sdAvailableMixerAudioFiles[MAX_MIXERS] = { 0 };

void refreshSystemAudioFiles()
{
  FILINFO info;
#if _USE_LFN
  TCHAR lfn[_MAX_LFN + 1];
  info.lfname = lfn;
  info.lfsize = sizeof(lfn);
#endif

  char filename[32] = SYSTEM_SOUNDS_PATH "/";
  strncpy(filename+SOUNDS_PATH_LNG_OFS, currentLanguagePack->id, 2);

  assert(sizeof(audioFilenames)==AU_FRSKY_FIRST*sizeof(char *));
  assert(sizeof(sdAvailableSystemAudioFiles)*8 >= AU_FRSKY_FIRST);

  uint64_t availableAudioFiles = 0;

  for (uint32_t i=0; i<AU_FRSKY_FIRST; i++) {
    strcpy(filename+sizeof(SYSTEM_SOUNDS_PATH), audioFilenames[i]);
    strcat(filename+sizeof(SYSTEM_SOUNDS_PATH), SOUNDS_EXT);
    if (f_stat(filename, &info) == FR_OK)
      availableAudioFiles |= ((uint64_t)1 << i);
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
    if (sdAvailableSystemAudioFiles & ((uint64_t)1 << event)) {
      strcpy(filename, SYSTEM_SOUNDS_PATH "/");
      strncpy(filename+SOUNDS_PATH_LNG_OFS, currentLanguagePack->id, 2);
      strcpy(filename+sizeof(SYSTEM_SOUNDS_PATH), audioFilenames[i]);
      strcat(filename+sizeof(SYSTEM_SOUNDS_PATH), SOUNDS_EXT);
      return true;
    }
  }
  else if (category == PHASE_AUDIO_CATEGORY) {
    if (sdAvailablePhaseAudioFiles[index] & ((uint32_t)1 << event)) {
      strcpy(filename, SOUNDS_PATH "/");
      strncpy(filename+SOUNDS_PATH_LNG_OFS, currentLanguagePack->id, 2);
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
      strncpy(filename+SOUNDS_PATH_LNG_OFS, currentLanguagePack->id, 2);
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

int16_t alawTable[256];
int16_t ulawTable[256];

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
    alawTable[i] = alaw2linear(i);
    ulawTable[i] = ulaw2linear(i);
  }
}

AudioQueue audioQueue;

AudioQueue::AudioQueue()
{
  memset(this, 0, sizeof(AudioQueue));
}

void AudioQueue::start()
{
  state = 1;
}


#define CODEC_ID_PCM_S16LE  1
#define CODEC_ID_PCM_ALAW   6
#define CODEC_ID_PCM_MULAW  7

#ifndef SIMU
void audioTask(void* pdata)
{
  while (!audioQueue.started()) {
    CoTickDelay(1);
  }

  setSampleRate(AUDIO_SAMPLE_RATE);

#if defined(SDCARD)
  if (!unexpectedShutdown) {
    codecsInit();
    sdInit();
    AUDIO_TADA();
  }
#endif  

  while (1) {
    audioQueue.wakeup();
    CoTickDelay(2/*4ms*/);
  }
}
#endif

void AudioQueue::pushBuffer(AudioBuffer *buffer)
{
  buffer->state = AUDIO_BUFFER_FILLED;

  if (dacQueue(buffer))
    buffer->state = AUDIO_BUFFER_PLAYING;

  bufferWIdx = nextBufferIdx(bufferWIdx);
}

void mix(uint16_t * result, int sample, unsigned int fade)
{
  *result = limit(0, *result + ((sample >> fade) >> 4), 4095);
}

#if defined(SDCARD) && !defined(SIMU)

#define RIFF_CHUNK_SIZE 12
uint8_t wavBuffer[AUDIO_BUFFER_SIZE*2];

int AudioQueue::mixWav(AudioContext &context, AudioBuffer *buffer, int volume, unsigned int fade)
{
  FRESULT result = FR_OK;
  UINT read = 0;
  AudioFragment & fragment = context.fragment;

  if (fragment.file[1]) {
    result = f_open(&context.state.wav.file, fragment.file, FA_OPEN_EXISTING | FA_READ);
    fragment.file[1] = 0;
    if (result == FR_OK) {
      result = f_read(&context.state.wav.file, wavBuffer, RIFF_CHUNK_SIZE+8, &read);
      if (result == FR_OK && read == RIFF_CHUNK_SIZE+8 && !memcmp(wavBuffer, "RIFF", 4) && !memcmp(wavBuffer+8, "WAVEfmt ", 8)) {
        uint32_t size = *((uint32_t *)(wavBuffer+16));
        result = (size < 256 ? f_read(&context.state.wav.file, wavBuffer, size+8, &read) : FR_DENIED);
        if (result == FR_OK && read == size+8) {
          context.state.wav.codec = ((uint16_t *)wavBuffer)[0];
          context.state.wav.freq = ((uint16_t *)wavBuffer)[2];
          uint32_t *wavSamplesPtr = (uint32_t *)(wavBuffer + size);
          uint32_t size = wavSamplesPtr[1];
          if (context.state.wav.freq != 0 && context.state.wav.freq * (AUDIO_SAMPLE_RATE / context.state.wav.freq) == AUDIO_SAMPLE_RATE) {
            context.state.wav.resampleRatio = (AUDIO_SAMPLE_RATE / context.state.wav.freq);
            context.state.wav.readSize = (context.state.wav.codec == CODEC_ID_PCM_S16LE ? 2*AUDIO_BUFFER_SIZE : AUDIO_BUFFER_SIZE) / context.state.wav.resampleRatio;
          }
          else {
            result = FR_DENIED;
          }
          while (result == FR_OK && memcmp(wavSamplesPtr, "data", 4) != 0) {
            result = (size < 256 ? f_read(&context.state.wav.file, wavBuffer, size+8, &read) : FR_DENIED);
            if (read != size+8) result = FR_DENIED;
            wavSamplesPtr = (uint32_t *)(wavBuffer + size);
            size = wavSamplesPtr[1];
          }
          context.state.wav.size = size;
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
  if (result == FR_OK) {
    result = f_read(&context.state.wav.file, wavBuffer, context.state.wav.readSize, &read);
    if (result == FR_OK) {
      if (read > context.state.wav.size) {
        read = context.state.wav.size;
      }
      context.state.wav.size -= read;

      if (read != context.state.wav.readSize) {
        f_close(&context.state.wav.file);
        fragment.clear();
      }

      uint16_t * samples = buffer->data;
      if (context.state.wav.codec == CODEC_ID_PCM_S16LE) {
        read /= 2;
        for (uint32_t i=0; i<read; i++) {
          for (uint8_t j=0; j<context.state.wav.resampleRatio; j++)
            mix(samples++, ((int16_t *)wavBuffer)[i], fade+2-volume);
        }
      }
      else if (context.state.wav.codec == CODEC_ID_PCM_ALAW) {
        for (uint32_t i=0; i<read; i++)
          for (uint8_t j=0; j<context.state.wav.resampleRatio; j++)
            mix(samples++, alawTable[wavBuffer[i]], fade+2-volume);
      }
      else if (context.state.wav.codec == CODEC_ID_PCM_MULAW) {
        for (uint32_t i=0; i<read; i++)
          for (uint8_t j=0; j<context.state.wav.resampleRatio; j++)
            mix(samples++, ulawTable[wavBuffer[i]], fade+2-volume);
      }

      return samples - buffer->data;
    }
  }

  fragment.clear();
  return -result;
}
#else
int AudioQueue::mixWav(AudioContext &context, AudioBuffer *buffer, int volume, unsigned int fade)
{
  return 0;
}
#endif

int AudioQueue::mixBeep(AudioContext &context, AudioBuffer *buffer, int volume, unsigned int fade)
{
  AudioFragment & fragment = context.fragment;
  unsigned int duration = 0;
  int result = 0;

  if (fragment.tone.duration > 0) {
    result = AUDIO_BUFFER_SIZE;
    if (fragment.tone.freq && context.state.tone.freq!=fragment.tone.freq && (!fragment.tone.freqIncr || abs(context.state.tone.freq-fragment.tone.freq) > 100)) {
      int periods = DIM(context.state.tone.points) / ((AUDIO_SAMPLE_RATE / fragment.tone.freq) + 1);
      context.state.tone.count = (periods * AUDIO_SAMPLE_RATE) / fragment.tone.freq;
      if (context.state.tone.idx >= context.state.tone.count) context.state.tone.idx = 0;
      double t = (M_PI * 2 * periods) / context.state.tone.count;
      for (unsigned int i=0; i<context.state.tone.count; i++)
        context.state.tone.points[i] = sin(t*i) * (toneVolumes[2+volume]);
    }

    if (fragment.tone.freqIncr)
      fragment.tone.freq += AUDIO_BUFFER_DURATION * fragment.tone.freqIncr;
    else
      fragment.tone.freq = 0;

    duration = min<unsigned int>(AUDIO_BUFFER_DURATION, fragment.tone.duration);
    for (unsigned int i=0; i<duration*(AUDIO_BUFFER_SIZE/AUDIO_BUFFER_DURATION); i++) {
      mix(&buffer->data[i], context.state.tone.points[context.state.tone.idx], fade);
      context.state.tone.idx = context.state.tone.idx + 1;
      if (context.state.tone.idx >= context.state.tone.count) context.state.tone.idx = 0;
    }

    fragment.tone.duration -= duration;
    if (fragment.tone.duration > 0)
      return AUDIO_BUFFER_SIZE;
  }

  if (fragment.tone.pause > 0) {
    result = AUDIO_BUFFER_SIZE;
    fragment.tone.pause -= min<unsigned int>(AUDIO_BUFFER_DURATION-duration, fragment.tone.pause);
    if (fragment.tone.pause > 0)
      return AUDIO_BUFFER_SIZE;
  }

  fragment.clear();
  return result;
}

int AudioQueue::mixAudioContext(AudioContext &context, AudioBuffer *buffer, int beepVolume, int wavVolume, unsigned int fade)
{
  int result;
  AudioFragment & fragment = context.fragment;

  if (fragment.type == FRAGMENT_TONE) {
    result = mixBeep(context, buffer, beepVolume, fade);
  }
  else if (fragment.type == FRAGMENT_FILE) {
    result = mixWav(context, buffer, wavVolume, fade);
  }
  else {
    result = 0;
  }

  return result;
}

void AudioQueue::wakeup()
{
  int result;
  AudioBuffer *buffer = getEmptyBuffer();
  if (buffer) {
    unsigned int fade = 0;
    int size = 0;

    // write silence in the buffer
    for (uint32_t i=0; i<AUDIO_BUFFER_SIZE; i++) {
      buffer->data[i] = 0x8000 >> 4; /* silence */
    }

    // mix the foreground context
    result = mixAudioContext(foregroundContext, buffer, g_eeGeneral.beepVolume, g_eeGeneral.wavVolume, fade);
    if (result > 0) {
      size = max(size, result);
      fade += 1;
    }

    // mix the normal context
    result = mixAudioContext(currentContext, buffer, g_eeGeneral.beepVolume, g_eeGeneral.wavVolume, fade);
    if (result > 0) {
      size = max(size, result);
      fade += 1;
    }
    else {
      CoEnterMutexSection(audioMutex);
      if (ridx != widx) {
        currentContext.clear();
        currentContext.fragment = fragments[ridx];
        if (!fragments[ridx].repeat--) {
          ridx = (ridx + 1) % AUDIO_QUEUE_LENGTH;
        }
      }
      CoLeaveMutexSection(audioMutex);
    }

    // mix the background context
    if (!isFunctionActive(FUNC_BACKGND_MUSIC_PAUSE)) {
      result = mixAudioContext(backgroundContext, buffer, g_eeGeneral.varioVolume, g_eeGeneral.backgroundVolume, fade);
      if (result > 0) {
        size = max(size, result);
      }
    }

    // push the buffer if needed
    if (size > 0) {
      buffer->size = size;
      pushBuffer(buffer);
    }
  }
}

inline unsigned int getToneLength(uint16_t len)
{
  unsigned int result = len; // default
  if (g_eeGeneral.beepLength < 0) {
    result /= (1-g_eeGeneral.beepLength);
  }
  else if (g_eeGeneral.beepLength > 0) {
    result *= (1+g_eeGeneral.beepLength);
  }
  return result;
}

void AudioQueue::pause(uint16_t len)
{
  play(0, 0, len);
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

void AudioQueue::play(uint16_t freq, uint16_t len, uint16_t pause, uint8_t flags, int8_t freqIncr)
{
  CoEnterMutexSection(audioMutex);

  if (freq && freq < BEEP_MIN_FREQ)
    freq = BEEP_MIN_FREQ;

  if (flags & PLAY_BACKGROUND) {
    AudioFragment & fragment = backgroundContext.fragment;
    backgroundContext.clear();
    fragment.type = FRAGMENT_TONE;
    fragment.tone.freq = freq;
    fragment.tone.duration = len;
    fragment.tone.pause = pause;
  }
  else {
    freq += g_eeGeneral.speakerPitch * 15;
    len = getToneLength(len);

    if (flags & PLAY_NOW) {
      AudioFragment & fragment = foregroundContext.fragment;
      if (fragment.type == FRAGMENT_EMPTY) {
        foregroundContext.clear();
        fragment.type = FRAGMENT_TONE;
        fragment.repeat = flags & 0x0f;
        fragment.tone.freq = freq;
        fragment.tone.duration = len;
        fragment.tone.pause = pause;
        fragment.tone.freqIncr = freqIncr;
      }
    }
    else {
      uint8_t next_widx = (widx + 1) % AUDIO_QUEUE_LENGTH;
      if (next_widx != ridx) {
        AudioFragment & fragment = fragments[widx];
        fragment.clear();
        fragment.type = FRAGMENT_TONE;
        fragment.repeat = flags & 0x0f;
        fragment.tone.freq = freq;
        fragment.tone.duration = len;
        fragment.tone.pause = pause;
        fragment.tone.freqIncr = freqIncr;
        widx = next_widx;
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
    backgroundContext.clear();
    AudioFragment & fragment = backgroundContext.fragment;
    fragment.type = FRAGMENT_FILE;
    strcpy(fragment.file, filename);
    fragment.id = id;
  }
  else if (flags & PLAY_NOW) {
    AudioFragment & fragment = foregroundContext.fragment;
    if (fragment.type == FRAGMENT_EMPTY) {
      foregroundContext.clear();
      fragment.type = FRAGMENT_FILE;
      strcpy(fragment.file, filename);
      fragment.repeat = flags & 0x0f;
      fragment.id = id;
    }
  }
  else {
    uint8_t next_widx = (widx + 1) % AUDIO_QUEUE_LENGTH;
    if (next_widx != ridx) {
      AudioFragment & fragment = fragments[widx];
      fragment.clear();
      fragment.type = FRAGMENT_FILE;
      strcpy(fragment.file, filename);
      fragment.repeat = flags & 0x0f;
      fragment.id = id;
      widx = next_widx;
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
    backgroundContext.fragment.type = FRAGMENT_EMPTY;
    backgroundContext.fragment.id = 0;
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
  foregroundContext.clear();
  currentContext.clear();
  backgroundContext.clear();
  CoLeaveMutexSection(audioMutex);
}

void audioEvent(uint8_t e, uint16_t f)
{
#if defined(SDCARD)
  char filename[AUDIO_FILENAME_MAXLEN+1];
#endif

#if defined(HAPTIC)
  haptic.event(e); //do this before audio to help sync timings
#endif

  if (e <= AU_ERROR || (e >= AU_WARNING1 && e < AU_FRSKY_FIRST)) {
    if (g_eeGeneral.alarmsFlash) {
      flashCounter = FLASH_DURATION;
    }
  }

  if (g_eeGeneral.beepMode>0 || (g_eeGeneral.beepMode==0 && e>=AU_TRIM_MOVE) || (g_eeGeneral.beepMode>=-1 && e<=AU_ERROR)) {
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
          audioQueue.play(2250, 80, 20, PLAY_REPEAT(2));
          break;
        // low battery in tx
        case AU_TX_BATTERY_LOW:
          audioQueue.play(1950, 160, 20, PLAY_REPEAT(2), 1);
          audioQueue.play(2550, 160, 20, PLAY_REPEAT(2), -1);
          break;
#if defined(PCBSKY9X)
        case AU_TX_MAH_HIGH:
          // TODO Rob something better here?
          audioQueue.play(1950, 160, 20, PLAY_REPEAT(2), 1);
          audioQueue.play(2550, 160, 20, PLAY_REPEAT(2), -1);
          break;
        case AU_TX_TEMP_HIGH:
          // TODO Rob something better here?
          audioQueue.play(1950, 160, 20, PLAY_REPEAT(2), 1);
          audioQueue.play(2550, 160, 20, PLAY_REPEAT(2), -1);
          break;
#endif
#if defined(VOICE)
        case AU_THROTTLE_ALERT:
        case AU_SWITCH_ALERT:
#endif
        case AU_ERROR:
          audioQueue.play(BEEP_DEFAULT_FREQ, 200, 20, PLAY_NOW);
          break;
        // keypad up (seems to be used when going left/right through system menu options. 0-100 scales etc)
        case AU_KEYPAD_UP:
          audioQueue.play(BEEP_KEY_UP_FREQ, 80, 20, PLAY_NOW);
          break;
        // keypad down (seems to be used when going left/right through system menu options. 0-100 scales etc)
        case AU_KEYPAD_DOWN:
          audioQueue.play(BEEP_KEY_DOWN_FREQ, 80, 20, PLAY_NOW);
          break;
        // menu display (also used by a few generic beeps)
        case AU_MENUS:
          audioQueue.play(BEEP_DEFAULT_FREQ, 80, 20, PLAY_NOW);
          break;
        // trim move
        case AU_TRIM_MOVE:
          audioQueue.play(f, 40, 20, PLAY_NOW);
          break;
        // trim center
        case AU_TRIM_MIDDLE:
          audioQueue.play(f, 80, 20, PLAY_NOW);
          break;
        // trim center
        case AU_TRIM_END:
          audioQueue.play(f, 80, 20, PLAY_NOW);
          break;          
        // warning one
        case AU_WARNING1:
          audioQueue.play(BEEP_DEFAULT_FREQ, 80, 20, PLAY_NOW);
          break;
        // warning two
        case AU_WARNING2:
          audioQueue.play(BEEP_DEFAULT_FREQ, 160, 20, PLAY_NOW);
          break;
        // warning three
        case AU_WARNING3:
          audioQueue.play(BEEP_DEFAULT_FREQ, 200, 20, PLAY_NOW);
          break;
        // pot/stick center
        case AU_POT_STICK_MIDDLE:
          audioQueue.play(BEEP_DEFAULT_FREQ+1500, 80, 20, PLAY_NOW);
          break;
        // mix warning 1
        case AU_MIX_WARNING_1:
          audioQueue.play(BEEP_DEFAULT_FREQ+1440, 48, 32);
          break;
        // mix warning 2
        case AU_MIX_WARNING_2:
          audioQueue.play(BEEP_DEFAULT_FREQ+1560, 48, 32, PLAY_REPEAT(1));
          break;
        // mix warning 3
        case AU_MIX_WARNING_3:
          audioQueue.play(BEEP_DEFAULT_FREQ+1680, 48, 32, PLAY_REPEAT(2));
          break;
        // time <= 10 seconds left
        case AU_TIMER_LT10:
          audioQueue.play(BEEP_DEFAULT_FREQ+150, 120, 20, PLAY_NOW);
          break;
        // time 20 seconds left
        case AU_TIMER_20:
          audioQueue.play(BEEP_DEFAULT_FREQ, 120, 20, PLAY_REPEAT(1)|PLAY_NOW);
          break;
        // time 30 seconds left
        case AU_TIMER_30:
          audioQueue.play(BEEP_DEFAULT_FREQ, 120, 20, PLAY_REPEAT(2)|PLAY_NOW);
          break;
#if defined(PCBTARANIS)
        case AU_A1_ORANGE:
          audioQueue.play(BEEP_DEFAULT_FREQ+600, 200, 20, PLAY_NOW);
          break;
        case AU_A1_RED:
          audioQueue.play(BEEP_DEFAULT_FREQ+600, 200, 20, PLAY_REPEAT(1)|PLAY_NOW);
          break;
        case AU_A2_ORANGE:
          audioQueue.play(BEEP_DEFAULT_FREQ+1500, 200, 20, PLAY_NOW);
          break;
        case AU_A2_RED:
          audioQueue.play(BEEP_DEFAULT_FREQ+1500, 200, 20, PLAY_REPEAT(1)|PLAY_NOW);
          break;
        case AU_RSSI_ORANGE:
          audioQueue.play(BEEP_DEFAULT_FREQ+1500, 800, 20, PLAY_NOW);
          break;
        case AU_RSSI_RED:
          audioQueue.play(BEEP_DEFAULT_FREQ+1800, 800, 20, PLAY_REPEAT(1)|PLAY_NOW);
          break;
        case AU_SWR_RED:
          audioQueue.play(450, 160, 40, PLAY_REPEAT(2), 1);
          break;
#endif
        case AU_FRSKY_BEEP1:
          audioQueue.play(BEEP_DEFAULT_FREQ, 60, 20);
          break;
        case AU_FRSKY_BEEP2:
          audioQueue.play(BEEP_DEFAULT_FREQ, 120, 20);
          break;
        case AU_FRSKY_BEEP3:
          audioQueue.play(BEEP_DEFAULT_FREQ, 200, 20);
          break;
        case AU_FRSKY_WARN1:
          audioQueue.play(BEEP_DEFAULT_FREQ+600, 120, 40, PLAY_REPEAT(2));
          break;
        case AU_FRSKY_WARN2:
          audioQueue.play(BEEP_DEFAULT_FREQ+900, 120, 40, PLAY_REPEAT(2));
          break;
        case AU_FRSKY_CHEEP:
          audioQueue.play(BEEP_DEFAULT_FREQ+900, 80, 20, PLAY_REPEAT(2), 2);
          break;
        case AU_FRSKY_RING:
          audioQueue.play(BEEP_DEFAULT_FREQ+750, 40, 20, PLAY_REPEAT(10));
          audioQueue.play(BEEP_DEFAULT_FREQ+750, 40, 80, PLAY_REPEAT(1));
          audioQueue.play(BEEP_DEFAULT_FREQ+750, 40, 20, PLAY_REPEAT(10));
          break;
        case AU_FRSKY_SCIFI:
          audioQueue.play(2550, 80, 20, PLAY_REPEAT(2), -1);
          audioQueue.play(1950, 80, 20, PLAY_REPEAT(2), 1);
          audioQueue.play(2250, 80, 20, 0);
          break;
        case AU_FRSKY_ROBOT:
          audioQueue.play(2250, 40,  20,  PLAY_REPEAT(1));
          audioQueue.play(1650, 120, 20, PLAY_REPEAT(1));
          audioQueue.play(2550, 120, 20, PLAY_REPEAT(1));
          break;
        case AU_FRSKY_CHIRP:
          audioQueue.play(BEEP_DEFAULT_FREQ+1200, 40, 20, PLAY_REPEAT(2));
          audioQueue.play(BEEP_DEFAULT_FREQ+1620, 40, 20, PLAY_REPEAT(3));
          break;
        case AU_FRSKY_TADA:
          audioQueue.play(1650, 80, 40);
          audioQueue.play(2850, 80, 40);
          audioQueue.play(3450, 64, 36, PLAY_REPEAT(2));
          break;
        case AU_FRSKY_CRICKET:
          audioQueue.play(2550, 40, 80,  PLAY_REPEAT(3));
          audioQueue.play(2550, 40, 160, PLAY_REPEAT(1));
          audioQueue.play(2550, 40, 80,  PLAY_REPEAT(3));
          break;
        case AU_FRSKY_SIREN:
          audioQueue.play(450, 160, 40, PLAY_REPEAT(2), 2);
          break;
        case AU_FRSKY_ALARMC:
          audioQueue.play(1650, 32, 68,  PLAY_REPEAT(2));
          audioQueue.play(2250, 64, 156, PLAY_REPEAT(1));
          audioQueue.play(1650, 64, 76,  PLAY_REPEAT(2));
          audioQueue.play(2250, 32, 168, PLAY_REPEAT(1));
          break;
        case AU_FRSKY_RATATA:
          audioQueue.play(BEEP_DEFAULT_FREQ+1500, 40, 80, PLAY_REPEAT(10));
          break;
        case AU_FRSKY_TICK:
          audioQueue.play(BEEP_DEFAULT_FREQ+1500, 40, 400, PLAY_REPEAT(2));
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
  strncpy(filename+SOUNDS_PATH_LNG_OFS, currentLanguagePack->id, 2);

  for (int8_t i=3; i>=0; i--) {
    filename[sizeof(SYSTEM_SOUNDS_PATH)+i] = '0' + (prompt%10);
    prompt /= 10;
  }

  audioQueue.playFile(filename, 0, id);

#endif
}
