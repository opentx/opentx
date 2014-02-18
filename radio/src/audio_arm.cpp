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
  0, 218, 437, 655, 874, 1092, 1311, 1529, 1747, 1966,
  2184, 2401, 2619, 2837, 3054, 3271, 3488, 3705, 3921, 4138,
  4354, 4569, 4785, 5000, 5215, 5429, 5643, 5857, 6071, 6284,
  6496, 6709, 6920, 7132, 7343, 7553, 7763, 7973, 8182, 8390,
  8598, 8806, 9012, 9219, 9425, 9630, 9834, 10038, 10242, 10444,
  10646, 10848, 11049, 11249, 11448, 11647, 11845, 12042, 12238, 12434,
  12629, 12823, 13017, 13209, 13401, 13592, 13782, 13972, 14160, 14348,
  14535, 14721, 14906, 15090, 15273, 15456, 15637, 15818, 15997, 16176,
  16354, 16531, 16706, 16881, 17055, 17228, 17400, 17570, 17740, 17909,
  18077, 18243, 18409, 18574, 18737, 18899, 19061, 19221, 19380, 19538,
  19695, 19851, 20006, 20160, 20312, 20464, 20614, 20763, 20911, 21058,
  21204, 21348, 21491, 21634, 21775, 21914, 22053, 22191, 22327, 22462,
  22596, 22728, 22860, 22990, 23119, 23247, 23373, 23499, 23623, 23746,
  23867, 23988, 24107, 24225, 24342, 24457, 24571, 24684, 24796, 24906,
  25016, 25123, 25230, 25336, 25440, 25543, 25644, 25745, 25844, 25942,
  26039, 26134, 26228, 26321, 26412, 26503, 26592, 26680, 26766, 26852,
  26936, 27018, 27100, 27180, 27259, 27337, 27413, 27489, 27563, 27635,
  27707, 27777, 27846, 27914, 27980, 28046, 28110, 28173, 28234, 28295,
  28354, 28412, 28468, 28524, 28578, 28631, 28683, 28733, 28783, 28831,
  28878, 28924, 28968, 29011, 29054, 29095, 29134, 29173, 29210, 29247,
  29282, 29316, 29349, 29380, 29411, 29440, 29468, 29495, 29521, 29545,
  29569, 29591, 29613, 29633, 29652, 29670, 29686, 29702, 29717, 29730,
  29743, 29754, 29764, 29773, 29781, 29788, 29794, 29799, 29802, 29805,
  29806, 29807, 29806, 29805, 29802, 29799, 29794, 29788, 29781, 29773,
  29765, 29755, 29744, 29732, 29719, 29705, 29690, 29675, 29658, 29640,
  29621, 29601, 29580, 29559, 29536, 29512, 29488, 29462, 29435, 29408,
  29379, 29350, 29320, 29288, 29256, 29223, 29189, 29154, 29118, 29081,
  29044, 29005, 28966, 28925, 28884, 28842, 28799, 28755, 28710, 28664,
  28617, 28570, 28522, 28472, 28422, 28371, 28320, 28267, 28214, 28159,
  28104, 28048, 27991, 27934, 27875, 27816, 27756, 27695, 27633, 27570,
  27507, 27443, 27378, 27312, 27245, 27178, 27109, 27040, 26971, 26900,
  26829, 26756, 26683, 26610, 26535, 26460, 26384, 26307, 26229, 26151,
  26072, 25992, 25911, 25830, 25748, 25665, 25581, 25497, 25412, 25326,
  25239, 25152, 25064, 24975, 24885, 24795, 24704, 24612, 24520, 24427,
  24333, 24238, 24143, 24047, 23950, 23853, 23755, 23656, 23556, 23456,
  23355, 23254, 23151, 23048, 22945, 22840, 22735, 22630, 22523, 22416,
  22308, 22200, 22091, 21981, 21871, 21760, 21648, 21536, 21423, 21309,
  21194, 21079, 20964, 20848, 20731, 20613, 20495, 20376, 20257, 20137,
  20016, 19894, 19773, 19650, 19527, 19403, 19279, 19154, 19028, 18902,
  18775, 18647, 18519, 18391, 18262, 18132, 18001, 17870, 17739, 17607,
  17474, 17341, 17207, 17073, 16938, 16802, 16666, 16530, 16393, 16255,
  16117, 15978, 15839, 15699, 15559, 15418, 15277, 15135, 14992, 14850,
  14706, 14562, 14418, 14273, 14128, 13982, 13836, 13689, 13542, 13394,
  13246, 13097, 12948, 12799, 12649, 12498, 12347, 12196, 12044, 11892,
  11740, 11587, 11433, 11280, 11125, 10971, 10816, 10661, 10505, 10349,
  10192, 10036, 9878, 9721, 9563, 9405, 9246, 9087, 8928, 8768,
  8609, 8448, 8288, 8127, 7966, 7805, 7643, 7481, 7319, 7156,
  6994, 6831, 6667, 6504, 6340, 6176, 6012, 5847, 5683, 5518,
  5353, 5188, 5022, 4857, 4691, 4525, 4359, 4192, 4026, 3859,
  3692, 3526, 3358, 3191, 3024, 2857, 2689, 2521, 2354, 2186,
  2018, 1850, 1682, 1514, 1346, 1178, 1009, 841, 673, 505,
  336, 168, 0, -168, -336, -505, -673, -841, -1009, -1178,
  -1346, -1514, -1682, -1850, -2018, -2186, -2354, -2521, -2689, -2857,
  -3024, -3191, -3358, -3526, -3692, -3859, -4026, -4192, -4359, -4525,
  -4691, -4857, -5022, -5188, -5353, -5518, -5683, -5847, -6012, -6176,
  -6340, -6504, -6667, -6831, -6994, -7156, -7319, -7481, -7643, -7805,
  -7966, -8127, -8288, -8448, -8609, -8768, -8928, -9087, -9246, -9405,
  -9563, -9721, -9878, -10036, -10192, -10349, -10505, -10661, -10816, -10971,
  -11125, -11280, -11433, -11587, -11740, -11892, -12044, -12196, -12347, -12498,
  -12649, -12799, -12948, -13097, -13246, -13394, -13542, -13689, -13836, -13982,
  -14128, -14273, -14418, -14562, -14706, -14850, -14992, -15135, -15277, -15418,
  -15559, -15699, -15839, -15978, -16117, -16255, -16393, -16530, -16666, -16802,
  -16938, -17073, -17207, -17341, -17474, -17607, -17739, -17870, -18001, -18132,
  -18262, -18391, -18519, -18647, -18775, -18902, -19028, -19154, -19279, -19403,
  -19527, -19650, -19773, -19894, -20016, -20137, -20257, -20376, -20495, -20613,
  -20731, -20848, -20964, -21079, -21194, -21309, -21423, -21536, -21648, -21760,
  -21871, -21981, -22091, -22200, -22308, -22416, -22523, -22630, -22735, -22840,
  -22945, -23048, -23151, -23254, -23355, -23456, -23556, -23656, -23755, -23853,
  -23950, -24047, -24143, -24238, -24333, -24427, -24520, -24612, -24704, -24795,
  -24885, -24975, -25064, -25152, -25239, -25326, -25412, -25497, -25581, -25665,
  -25748, -25830, -25911, -25992, -26072, -26151, -26229, -26307, -26384, -26460,
  -26535, -26610, -26683, -26756, -26829, -26900, -26971, -27040, -27109, -27178,
  -27245, -27312, -27378, -27443, -27507, -27570, -27633, -27695, -27756, -27816,
  -27875, -27934, -27991, -28048, -28104, -28159, -28214, -28267, -28320, -28371,
  -28422, -28472, -28522, -28570, -28617, -28664, -28710, -28755, -28799, -28842,
  -28884, -28925, -28966, -29005, -29044, -29081, -29118, -29154, -29189, -29223,
  -29256, -29288, -29320, -29350, -29379, -29408, -29435, -29462, -29488, -29512,
  -29536, -29559, -29580, -29601, -29621, -29640, -29658, -29675, -29690, -29705,
  -29719, -29732, -29744, -29755, -29765, -29773, -29781, -29788, -29794, -29799,
  -29802, -29805, -29806, -29807, -29806, -29805, -29802, -29799, -29794, -29788,
  -29781, -29773, -29764, -29754, -29743, -29730, -29717, -29702, -29686, -29670,
  -29652, -29633, -29613, -29591, -29569, -29545, -29521, -29495, -29468, -29440,
  -29411, -29380, -29349, -29316, -29282, -29247, -29210, -29173, -29134, -29095,
  -29054, -29011, -28968, -28924, -28878, -28831, -28783, -28733, -28683, -28631,
  -28578, -28524, -28468, -28412, -28354, -28295, -28234, -28173, -28110, -28046,
  -27980, -27914, -27846, -27777, -27707, -27635, -27563, -27489, -27413, -27337,
  -27259, -27180, -27100, -27018, -26936, -26852, -26766, -26680, -26592, -26503,
  -26412, -26321, -26228, -26134, -26039, -25942, -25844, -25745, -25644, -25543,
  -25440, -25336, -25230, -25123, -25016, -24906, -24796, -24684, -24571, -24457,
  -24342, -24225, -24107, -23988, -23867, -23746, -23623, -23499, -23373, -23247,
  -23119, -22990, -22860, -22728, -22596, -22462, -22327, -22191, -22053, -21914,
  -21775, -21634, -21491, -21348, -21204, -21058, -20911, -20763, -20614, -20464,
  -20312, -20160, -20006, -19851, -19695, -19538, -19380, -19221, -19061, -18899,
  -18737, -18574, -18409, -18243, -18077, -17909, -17740, -17570, -17400, -17228,
  -17055, -16881, -16706, -16531, -16354, -16176, -15997, -15818, -15637, -15456,
  -15273, -15090, -14906, -14721, -14535, -14348, -14160, -13972, -13782, -13592,
  -13401, -13209, -13017, -12823, -12629, -12434, -12238, -12042, -11845, -11647,
  -11448, -11249, -11049, -10848, -10646, -10444, -10242, -10038, -9834, -9630,
  -9425, -9219, -9012, -8806, -8598, -8390, -8182, -7973, -7763, -7553,
  -7343, -7132, -6920, -6709, -6496, -6284, -6071, -5857, -5643, -5429,
  -5215, -5000, -4785, -4569, -4354, -4138, -3921, -3705, -3488, -3271,
  -3054, -2837, -2619, -2401, -2184, -1966, -1747, -1529, -1311, -1092,
  -874, -655, -437, -218,
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

  context.clear();
  return -result;
}
#else
int AudioQueue::mixWav(AudioContext &context, AudioBuffer *buffer, int volume, unsigned int fade)
{
  return 0;
}
#endif

const unsigned int toneVolumes[] = { 10, 8, 6, 4, 2 };
inline float evalVolumeRatio(int freq, int volume)
{
  float result = toneVolumes[2+volume];
  if (freq < 330) {
    result = (result * freq * freq) / (330 * 330);
  }
  return result;
}

int AudioQueue::mixTone(AudioContext &context, AudioBuffer *buffer, int volume, unsigned int fade)
{
  AudioFragment & fragment = context.fragment;
  int duration = 0;
  int result = 0;

  int remainingDuration = fragment.tone.duration - context.state.tone.duration;
  if (remainingDuration > 0) {
    int points;
    double toneIdx = context.state.tone.idx;

    if (fragment.tone.freq != context.state.tone.freq) {
      context.state.tone.freq = fragment.tone.freq;
      context.state.tone.step = double(DIM(sineValues)*fragment.tone.freq) / AUDIO_SAMPLE_RATE;
      context.state.tone.volume = evalVolumeRatio(fragment.tone.freq, volume);
    }

    if (fragment.tone.freqIncr) {
      fragment.tone.freq += AUDIO_BUFFER_DURATION * fragment.tone.freqIncr;
    }

    if (remainingDuration > AUDIO_BUFFER_DURATION) {
      duration = AUDIO_BUFFER_DURATION;
      points = AUDIO_BUFFER_SIZE;
    }
    else {
      duration = remainingDuration;
      points = (duration * AUDIO_BUFFER_SIZE) / AUDIO_BUFFER_DURATION;
      unsigned int end = toneIdx + (context.state.tone.step * points);
      if (end > DIM(sineValues))
        end -= (end % DIM(sineValues));
      else
        end = DIM(sineValues);
      points = (double(end) - toneIdx) / context.state.tone.step;
    }

    for (int i=0; i<points; i++) {
      int16_t sample = sineValues[int(toneIdx)] / context.state.tone.volume;
      mix(&buffer->data[i], sample, fade);
      toneIdx += context.state.tone.step;
      if ((unsigned int)toneIdx >= DIM(sineValues))
        toneIdx -= DIM(sineValues);
    }

    if (remainingDuration > AUDIO_BUFFER_DURATION) {
      context.state.tone.duration += AUDIO_BUFFER_DURATION;
      context.state.tone.idx = toneIdx;
      return AUDIO_BUFFER_SIZE;
    }
    else {
      context.state.tone.duration = 32000; // once the tone is finished, it's not possible to update its frequency and duration
      context.state.tone.idx = 0;
    }
  }

  remainingDuration = fragment.tone.pause - context.state.tone.pause;
  if (remainingDuration > 0) {
    result = AUDIO_BUFFER_SIZE;
    context.state.tone.pause += min<unsigned int>(AUDIO_BUFFER_DURATION-duration, fragment.tone.pause);
    if (fragment.tone.pause > context.state.tone.pause)
      return result;
  }

  context.clear();
  return result;
}

int AudioQueue::mixAudioContext(AudioContext &context, AudioBuffer *buffer, int beepVolume, int wavVolume, unsigned int fade)
{
  int result;
  AudioFragment & fragment = context.fragment;

  if (fragment.type == FRAGMENT_TONE) {
    result = mixTone(context, buffer, beepVolume, fade);
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
  playTone(0, 0, len);
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

void AudioQueue::playTone(uint16_t freq, uint16_t len, uint16_t pause, uint8_t flags, int8_t freqIncr)
{
  CoEnterMutexSection(audioMutex);

  if (freq && freq < BEEP_MIN_FREQ)
    freq = BEEP_MIN_FREQ;

  if (flags & PLAY_BACKGROUND) {
    AudioFragment & fragment = backgroundContext.fragment;
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
  playTone(0, 0, 100, PLAY_NOW);        // insert a 100ms pause
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
          audioQueue.playTone(2250, 80, 20, PLAY_REPEAT(2));
          break;
        // low battery in tx
        case AU_TX_BATTERY_LOW:
          audioQueue.playTone(1950, 160, 20, PLAY_REPEAT(2), 1);
          audioQueue.playTone(2550, 160, 20, PLAY_REPEAT(2), -1);
          break;
#if defined(PCBSKY9X)
        case AU_TX_MAH_HIGH:
          // TODO Rob something better here?
          audioQueue.playTone(1950, 160, 20, PLAY_REPEAT(2), 1);
          audioQueue.playTone(2550, 160, 20, PLAY_REPEAT(2), -1);
          break;
        case AU_TX_TEMP_HIGH:
          // TODO Rob something better here?
          audioQueue.playTone(1950, 160, 20, PLAY_REPEAT(2), 1);
          audioQueue.playTone(2550, 160, 20, PLAY_REPEAT(2), -1);
          break;
#endif
#if defined(VOICE)
        case AU_THROTTLE_ALERT:
        case AU_SWITCH_ALERT:
#endif
        case AU_ERROR:
          audioQueue.playTone(BEEP_DEFAULT_FREQ, 200, 20, PLAY_NOW);
          break;
        // keypad up (seems to be used when going left/right through system menu options. 0-100 scales etc)
        case AU_KEYPAD_UP:
          audioQueue.playTone(BEEP_KEY_UP_FREQ, 80, 20, PLAY_NOW);
          break;
        // keypad down (seems to be used when going left/right through system menu options. 0-100 scales etc)
        case AU_KEYPAD_DOWN:
          audioQueue.playTone(BEEP_KEY_DOWN_FREQ, 80, 20, PLAY_NOW);
          break;
        // menu display (also used by a few generic beeps)
        case AU_MENUS:
          audioQueue.playTone(BEEP_DEFAULT_FREQ, 80, 20, PLAY_NOW);
          break;
        // trim move
        case AU_TRIM_MOVE:
          audioQueue.playTone(f, 40, 20, PLAY_NOW);
          break;
        // trim center
        case AU_TRIM_MIDDLE:
          audioQueue.playTone(f, 80, 20, PLAY_NOW);
          break;
        // trim center
        case AU_TRIM_END:
          audioQueue.playTone(f, 80, 20, PLAY_NOW);
          break;          
        // warning one
        case AU_WARNING1:
          audioQueue.playTone(BEEP_DEFAULT_FREQ, 80, 20, PLAY_NOW);
          break;
        // warning two
        case AU_WARNING2:
          audioQueue.playTone(BEEP_DEFAULT_FREQ, 160, 20, PLAY_NOW);
          break;
        // warning three
        case AU_WARNING3:
          audioQueue.playTone(BEEP_DEFAULT_FREQ, 200, 20, PLAY_NOW);
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
          audioQueue.playTone(BEEP_DEFAULT_FREQ+1500, 80, 20, PLAY_NOW);
          break;
        // mix warning 1
        case AU_MIX_WARNING_1:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+1440, 48, 32);
          break;
        // mix warning 2
        case AU_MIX_WARNING_2:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+1560, 48, 32, PLAY_REPEAT(1));
          break;
        // mix warning 3
        case AU_MIX_WARNING_3:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+1680, 48, 32, PLAY_REPEAT(2));
          break;
        // timer == 0
        case AU_TIMER_00:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+150, 300, 20, PLAY_NOW);
          break;
        // timer <= 10 seconds left
        case AU_TIMER_LT10:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+150, 120, 20, PLAY_NOW);
          break;
        // timer 20 seconds left
        case AU_TIMER_20:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+150, 120, 20, PLAY_REPEAT(1)|PLAY_NOW);
          break;
        // timer 30 seconds left
        case AU_TIMER_30:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+150, 120, 20, PLAY_REPEAT(2)|PLAY_NOW);
          break;
#if defined(PCBTARANIS)
        case AU_A1_ORANGE:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+600, 200, 20, PLAY_NOW);
          break;
        case AU_A1_RED:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+600, 200, 20, PLAY_REPEAT(1)|PLAY_NOW);
          break;
        case AU_A2_ORANGE:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+1500, 200, 20, PLAY_NOW);
          break;
        case AU_A2_RED:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+1500, 200, 20, PLAY_REPEAT(1)|PLAY_NOW);
          break;
        case AU_RSSI_ORANGE:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+1500, 800, 20, PLAY_NOW);
          break;
        case AU_RSSI_RED:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+1800, 800, 20, PLAY_REPEAT(1)|PLAY_NOW);
          break;
        case AU_SWR_RED:
          audioQueue.playTone(450, 160, 40, PLAY_REPEAT(2), 1);
          break;
#endif
        case AU_FRSKY_BEEP1:
          audioQueue.playTone(BEEP_DEFAULT_FREQ, 60, 20);
          break;
        case AU_FRSKY_BEEP2:
          audioQueue.playTone(BEEP_DEFAULT_FREQ, 120, 20);
          break;
        case AU_FRSKY_BEEP3:
          audioQueue.playTone(BEEP_DEFAULT_FREQ, 200, 20);
          break;
        case AU_FRSKY_WARN1:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+600, 120, 40, PLAY_REPEAT(2));
          break;
        case AU_FRSKY_WARN2:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+900, 120, 40, PLAY_REPEAT(2));
          break;
        case AU_FRSKY_CHEEP:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+900, 80, 20, PLAY_REPEAT(2), 2);
          break;
        case AU_FRSKY_RING:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+750, 40, 20, PLAY_REPEAT(10));
          audioQueue.playTone(BEEP_DEFAULT_FREQ+750, 40, 80, PLAY_REPEAT(1));
          audioQueue.playTone(BEEP_DEFAULT_FREQ+750, 40, 20, PLAY_REPEAT(10));
          break;
        case AU_FRSKY_SCIFI:
          audioQueue.playTone(2550, 80, 20, PLAY_REPEAT(2), -1);
          audioQueue.playTone(1950, 80, 20, PLAY_REPEAT(2), 1);
          audioQueue.playTone(2250, 80, 20, 0);
          break;
        case AU_FRSKY_ROBOT:
          audioQueue.playTone(2250, 40,  20,  PLAY_REPEAT(1));
          audioQueue.playTone(1650, 120, 20, PLAY_REPEAT(1));
          audioQueue.playTone(2550, 120, 20, PLAY_REPEAT(1));
          break;
        case AU_FRSKY_CHIRP:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+1200, 40, 20, PLAY_REPEAT(2));
          audioQueue.playTone(BEEP_DEFAULT_FREQ+1620, 40, 20, PLAY_REPEAT(3));
          break;
        case AU_FRSKY_TADA:
          audioQueue.playTone(1650, 80, 40);
          audioQueue.playTone(2850, 80, 40);
          audioQueue.playTone(3450, 64, 36, PLAY_REPEAT(2));
          break;
        case AU_FRSKY_CRICKET:
          audioQueue.playTone(2550, 40, 80,  PLAY_REPEAT(3));
          audioQueue.playTone(2550, 40, 160, PLAY_REPEAT(1));
          audioQueue.playTone(2550, 40, 80,  PLAY_REPEAT(3));
          break;
        case AU_FRSKY_SIREN:
          audioQueue.playTone(450, 160, 40, PLAY_REPEAT(2), 2);
          break;
        case AU_FRSKY_ALARMC:
          audioQueue.playTone(1650, 32, 68,  PLAY_REPEAT(2));
          audioQueue.playTone(2250, 64, 156, PLAY_REPEAT(1));
          audioQueue.playTone(1650, 64, 76,  PLAY_REPEAT(2));
          audioQueue.playTone(2250, 32, 168, PLAY_REPEAT(1));
          break;
        case AU_FRSKY_RATATA:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+1500, 40, 80, PLAY_REPEAT(10));
          break;
        case AU_FRSKY_TICK:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+1500, 40, 400, PLAY_REPEAT(2));
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
