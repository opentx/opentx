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

const int16_t sineValues[] =
{
  0, 201, 402, 603, 804, 1005, 1206, 1406, 1607, 1808,
  2009, 2209, 2410, 2610, 2811, 3011, 3211, 3411, 3611, 3811,
  4011, 4210, 4409, 4609, 4807, 5006, 5205, 5403, 5601, 5799,
  5997, 6195, 6392, 6589, 6786, 6983, 7179, 7375, 7571, 7766,
  7961, 8156, 8351, 8545, 8739, 8933, 9126, 9319, 9511, 9704,
  9895, 10087, 10278, 10469, 10659, 10849, 11039, 11228, 11416, 11605,
  11792, 11980, 12167, 12353, 12539, 12725, 12910, 13094, 13278, 13462,
  13645, 13827, 14009, 14191, 14372, 14552, 14732, 14911, 15090, 15268,
  15446, 15623, 15799, 15975, 16151, 16325, 16499, 16673, 16845, 17017,
  17189, 17360, 17530, 17700, 17868, 18037, 18204, 18371, 18537, 18703,
  18867, 19031, 19195, 19357, 19519, 19680, 19841, 20000, 20159, 20317,
  20475, 20631, 20787, 20942, 21096, 21250, 21402, 21554, 21705, 21855,
  22005, 22153, 22301, 22448, 22594, 22739, 22884, 23027, 23170, 23311,
  23452, 23592, 23731, 23869, 24007, 24143, 24279, 24413, 24547, 24680,
  24811, 24942, 25072, 25201, 25329, 25456, 25582, 25707, 25832, 25955,
  26077, 26198, 26319, 26438, 26556, 26673, 26790, 26905, 27019, 27132,
  27245, 27356, 27466, 27575, 27683, 27790, 27896, 28001, 28105, 28208,
  28310, 28410, 28510, 28609, 28706, 28803, 28898, 28992, 29085, 29177,
  29268, 29358, 29447, 29534, 29621, 29706, 29791, 29874, 29956, 30037,
  30117, 30195, 30273, 30349, 30424, 30498, 30571, 30643, 30714, 30783,
  30852, 30919, 30985, 31050, 31113, 31176, 31237, 31297, 31356, 31414,
  31470, 31526, 31580, 31633, 31685, 31736, 31785, 31833, 31880, 31926,
  31971, 32014, 32057, 32098, 32137, 32176, 32213, 32250, 32285, 32318,
  32351, 32382, 32412, 32441, 32469, 32495, 32521, 32545, 32567, 32589,
  32609, 32628, 32646, 32663, 32678, 32692, 32705, 32717, 32728, 32737,
  32745, 32752, 32757, 32761, 32765, 32766, 32767, 32766, 32765, 32761,
  32757, 32752, 32745, 32737, 32728, 32717, 32705, 32692, 32678, 32663,
  32646, 32628, 32609, 32589, 32567, 32545, 32521, 32495, 32469, 32441,
  32412, 32382, 32351, 32318, 32285, 32250, 32213, 32176, 32137, 32098,
  32057, 32014, 31971, 31926, 31880, 31833, 31785, 31736, 31685, 31633,
  31580, 31526, 31470, 31414, 31356, 31297, 31237, 31176, 31113, 31050,
  30985, 30919, 30852, 30783, 30714, 30643, 30571, 30498, 30424, 30349,
  30273, 30195, 30117, 30037, 29956, 29874, 29791, 29706, 29621, 29534,
  29447, 29358, 29268, 29177, 29085, 28992, 28898, 28803, 28706, 28609,
  28510, 28410, 28310, 28208, 28105, 28001, 27896, 27790, 27683, 27575,
  27466, 27356, 27245, 27132, 27019, 26905, 26790, 26673, 26556, 26438,
  26319, 26198, 26077, 25955, 25832, 25707, 25582, 25456, 25329, 25201,
  25072, 24942, 24811, 24680, 24547, 24413, 24279, 24143, 24007, 23869,
  23731, 23592, 23452, 23311, 23170, 23027, 22884, 22739, 22594, 22448,
  22301, 22153, 22005, 21855, 21705, 21554, 21402, 21250, 21096, 20942,
  20787, 20631, 20475, 20317, 20159, 20000, 19841, 19680, 19519, 19357,
  19195, 19031, 18867, 18703, 18537, 18371, 18204, 18037, 17868, 17700,
  17530, 17360, 17189, 17017, 16845, 16673, 16499, 16325, 16151, 15975,
  15799, 15623, 15446, 15268, 15090, 14911, 14732, 14552, 14372, 14191,
  14009, 13827, 13645, 13462, 13278, 13094, 12910, 12725, 12539, 12353,
  12167, 11980, 11792, 11605, 11416, 11228, 11039, 10849, 10659, 10469,
  10278, 10087, 9895, 9704, 9511, 9319, 9126, 8933, 8739, 8545,
  8351, 8156, 7961, 7766, 7571, 7375, 7179, 6983, 6786, 6589,
  6392, 6195, 5997, 5799, 5601, 5403, 5205, 5006, 4807, 4609,
  4409, 4210, 4011, 3811, 3611, 3411, 3211, 3011, 2811, 2610,
  2410, 2209, 2009, 1808, 1607, 1406, 1206, 1005, 804, 603,
  402, 201, 0, -201, -402, -603, -804, -1005, -1206, -1406,
  -1607, -1808, -2009, -2209, -2410, -2610, -2811, -3011, -3211, -3411,
  -3611, -3811, -4011, -4210, -4409, -4609, -4807, -5006, -5205, -5403,
  -5601, -5799, -5997, -6195, -6392, -6589, -6786, -6983, -7179, -7375,
  -7571, -7766, -7961, -8156, -8351, -8545, -8739, -8933, -9126, -9319,
  -9511, -9704, -9895, -10087, -10278, -10469, -10659, -10849, -11039, -11228,
  -11416, -11605, -11792, -11980, -12167, -12353, -12539, -12725, -12910, -13094,
  -13278, -13462, -13645, -13827, -14009, -14191, -14372, -14552, -14732, -14911,
  -15090, -15268, -15446, -15623, -15799, -15975, -16151, -16325, -16499, -16673,
  -16845, -17017, -17189, -17360, -17530, -17700, -17868, -18037, -18204, -18371,
  -18537, -18703, -18867, -19031, -19195, -19357, -19519, -19680, -19841, -20000,
  -20159, -20317, -20475, -20631, -20787, -20942, -21096, -21250, -21402, -21554,
  -21705, -21855, -22005, -22153, -22301, -22448, -22594, -22739, -22884, -23027,
  -23170, -23311, -23452, -23592, -23731, -23869, -24007, -24143, -24279, -24413,
  -24547, -24680, -24811, -24942, -25072, -25201, -25329, -25456, -25582, -25707,
  -25832, -25955, -26077, -26198, -26319, -26438, -26556, -26673, -26790, -26905,
  -27019, -27132, -27245, -27356, -27466, -27575, -27683, -27790, -27896, -28001,
  -28105, -28208, -28310, -28410, -28510, -28609, -28706, -28803, -28898, -28992,
  -29085, -29177, -29268, -29358, -29447, -29534, -29621, -29706, -29791, -29874,
  -29956, -30037, -30117, -30195, -30273, -30349, -30424, -30498, -30571, -30643,
  -30714, -30783, -30852, -30919, -30985, -31050, -31113, -31176, -31237, -31297,
  -31356, -31414, -31470, -31526, -31580, -31633, -31685, -31736, -31785, -31833,
  -31880, -31926, -31971, -32014, -32057, -32098, -32137, -32176, -32213, -32250,
  -32285, -32318, -32351, -32382, -32412, -32441, -32469, -32495, -32521, -32545,
  -32567, -32589, -32609, -32628, -32646, -32663, -32678, -32692, -32705, -32717,
  -32728, -32737, -32745, -32752, -32757, -32761, -32765, -32766, -32767, -32766,
  -32765, -32761, -32757, -32752, -32745, -32737, -32728, -32717, -32705, -32692,
  -32678, -32663, -32646, -32628, -32609, -32589, -32567, -32545, -32521, -32495,
  -32469, -32441, -32412, -32382, -32351, -32318, -32285, -32250, -32213, -32176,
  -32137, -32098, -32057, -32014, -31971, -31926, -31880, -31833, -31785, -31736,
  -31685, -31633, -31580, -31526, -31470, -31414, -31356, -31297, -31237, -31176,
  -31113, -31050, -30985, -30919, -30852, -30783, -30714, -30643, -30571, -30498,
  -30424, -30349, -30273, -30195, -30117, -30037, -29956, -29874, -29791, -29706,
  -29621, -29534, -29447, -29358, -29268, -29177, -29085, -28992, -28898, -28803,
  -28706, -28609, -28510, -28410, -28310, -28208, -28105, -28001, -27896, -27790,
  -27683, -27575, -27466, -27356, -27245, -27132, -27019, -26905, -26790, -26673,
  -26556, -26438, -26319, -26198, -26077, -25955, -25832, -25707, -25582, -25456,
  -25329, -25201, -25072, -24942, -24811, -24680, -24547, -24413, -24279, -24143,
  -24007, -23869, -23731, -23592, -23452, -23311, -23170, -23027, -22884, -22739,
  -22594, -22448, -22301, -22153, -22005, -21855, -21705, -21554, -21402, -21250,
  -21096, -20942, -20787, -20631, -20475, -20317, -20159, -20000, -19841, -19680,
  -19519, -19357, -19195, -19031, -18867, -18703, -18537, -18371, -18204, -18037,
  -17868, -17700, -17530, -17360, -17189, -17017, -16845, -16673, -16499, -16325,
  -16151, -15975, -15799, -15623, -15446, -15268, -15090, -14911, -14732, -14552,
  -14372, -14191, -14009, -13827, -13645, -13462, -13278, -13094, -12910, -12725,
  -12539, -12353, -12167, -11980, -11792, -11605, -11416, -11228, -11039, -10849,
  -10659, -10469, -10278, -10087, -9895, -9704, -9511, -9319, -9126, -8933,
  -8739, -8545, -8351, -8156, -7961, -7766, -7571, -7375, -7179, -6983,
  -6786, -6589, -6392, -6195, -5997, -5799, -5601, -5403, -5205, -5006,
  -4807, -4609, -4409, -4210, -4011, -3811, -3611, -3411, -3211, -3011,
  -2811, -2610, -2410, -2209, -2009, -1808, -1607, -1406, -1206, -1005,
  -804, -603, -402, -201,
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
  "midstck1",
  "midstck2",
  "midstck3",
  "midstck4",
#if defined(PCBTARANIS)
  "midpot1",
  "midpot2",
  "midslid1",
  "midslid2",
#else
  "midpot1",
  "midpot2",
  "midpot3",
#endif
  "mixwarn1",
  "mixwarn2",
  "mixwarn3",
  "timer00",
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
  TCHAR lfn[_MAX_LFN + 1];
  info.lfname = lfn;
  info.lfsize = sizeof(lfn);

  char filename[AUDIO_FILENAME_MAXLEN+1] = SOUNDS_PATH "/";
  strncpy(filename+SOUNDS_PATH_LNG_OFS, currentLanguagePack->id, 2);
  
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

  __disable_irq();

  bufferWIdx = nextBufferIdx(bufferWIdx);

  if (dacQueue(buffer)) {
    buffer->state = AUDIO_BUFFER_PLAYING;
  }

  __enable_irq();
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
            result = f_lseek(&context.state.wav.file, f_tell(&context.state.wav.file)+size);
            if (result == FR_OK) {
              result = f_read(&context.state.wav.file, wavBuffer, 8, &read);
              if (read != 8) result = FR_DENIED;
              wavSamplesPtr = (uint32_t *)wavBuffer;
              size = wavSamplesPtr[1];
            }
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

const unsigned int toneVolumes[] = { 2, 4, 8, 12, 16 };

int AudioQueue::mixBeep(AudioContext &context, AudioBuffer *buffer, int volume, unsigned int fade)
{
  AudioFragment & fragment = context.fragment;
  int duration = 0;
  int result = 0;

  if (fragment.tone.duration > 0) {
    int points;
    double toneIdx = context.state.tone.idx;

    if (fragment.tone.freq) {
      context.state.tone.step = double(DIM(sineValues)*fragment.tone.freq) / AUDIO_SAMPLE_RATE;
      context.state.tone.volume = (fragment.tone.freq * fragment.tone.freq * 32) / (110 * 110 * toneVolumes[2+volume]);
      if (fragment.tone.freqIncr)
        fragment.tone.freq += AUDIO_BUFFER_DURATION * fragment.tone.freqIncr;
      else
        fragment.tone.freq = 0;
    }

    if (fragment.tone.duration <= AUDIO_BUFFER_DURATION) {
      duration = fragment.tone.duration;
      fragment.tone.duration = 0;
      points = (duration * AUDIO_BUFFER_SIZE) / AUDIO_BUFFER_DURATION;
      unsigned int end = toneIdx + (context.state.tone.step * points);
      if (end > DIM(sineValues))
        end -= (end % DIM(sineValues));
      else
        end = DIM(sineValues);
      points = (double(end) - toneIdx) / context.state.tone.step;
    }
    else {
      duration = AUDIO_BUFFER_DURATION;
      fragment.tone.duration -= AUDIO_BUFFER_DURATION;
      points = AUDIO_BUFFER_SIZE;
    }

    for (int i=0; i<points; i++) {
      int16_t sample = sineValues[int(toneIdx)] / context.state.tone.volume;
      mix(&buffer->data[i], sample, fade);
      toneIdx += context.state.tone.step;
      if ((unsigned int)toneIdx >= DIM(sineValues))
        toneIdx -= DIM(sineValues);
    }

    if (fragment.tone.duration > 0) {
      context.state.tone.idx = toneIdx;
      return AUDIO_BUFFER_SIZE;
    }
    else {
      context.state.tone.idx = 0;
    }
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
      size = result;
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
    if (!isFunctionActive(FUNCTION_BACKGND_MUSIC_PAUSE)) {
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

  if (g_eeGeneral.beepMode == e_mode_quiet)
    return;

  if (strlen(filename) > AUDIO_FILENAME_MAXLEN) {
    POPUP_WARNING(STR_PATH_TOO_LONG);
    return;
  }

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
        case AU_STICK1_MIDDLE:
        case AU_STICK2_MIDDLE:
        case AU_STICK3_MIDDLE:
        case AU_STICK4_MIDDLE:
        case AU_POT1_MIDDLE:
        case AU_POT2_MIDDLE:
#if defined(PCBTARANIS)
        case AU_SLIDER1_MIDDLE:
        case AU_SLIDER2_MIDDLE:
#else
        case AU_POT3_MIDDLE:
#endif
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
        // timer == 0
        case AU_TIMER_00:
          audioQueue.play(BEEP_DEFAULT_FREQ+150, 300, 20, PLAY_NOW);
          break;
        // timer <= 10 seconds left
        case AU_TIMER_LT10:
          audioQueue.play(BEEP_DEFAULT_FREQ+150, 120, 20, PLAY_NOW);
          break;
        // timer 20 seconds left
        case AU_TIMER_20:
          audioQueue.play(BEEP_DEFAULT_FREQ+150, 120, 20, PLAY_REPEAT(1)|PLAY_NOW);
          break;
        // timer 30 seconds left
        case AU_TIMER_30:
          audioQueue.play(BEEP_DEFAULT_FREQ+150, 120, 20, PLAY_REPEAT(2)|PLAY_NOW);
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
