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
  0, 207, 414, 621, 829, 1036, 1243, 1450, 1657, 1864,
  2071, 2278, 2485, 2691, 2898, 3104, 3310, 3516, 3722, 3928,
  4133, 4338, 4543, 4748, 4953, 5157, 5361, 5565, 5769, 5972,
  6175, 6378, 6580, 6783, 6984, 7186, 7387, 7588, 7788, 7988,
  8188, 8387, 8586, 8785, 8983, 9180, 9378, 9574, 9771, 9967,
  10162, 10357, 10551, 10745, 10939, 11132, 11324, 11516, 11707, 11898,
  12088, 12278, 12467, 12656, 12844, 13031, 13218, 13404, 13589, 13774,
  13959, 14142, 14325, 14507, 14689, 14870, 15050, 15230, 15409, 15587,
  15764, 15941, 16117, 16293, 16467, 16641, 16814, 16986, 17158, 17329,
  17498, 17668, 17836, 18004, 18170, 18336, 18502, 18666, 18829, 18992,
  19154, 19315, 19475, 19634, 19793, 19950, 20107, 20262, 20417, 20571,
  20724, 20876, 21028, 21178, 21327, 21476, 21623, 21770, 21916, 22060,
  22204, 22347, 22489, 22630, 22770, 22909, 23047, 23184, 23320, 23455,
  23589, 23722, 23854, 23985, 24115, 24244, 24372, 24499, 24624, 24749,
  24873, 24996, 25118, 25239, 25358, 25477, 25595, 25711, 25827, 25941,
  26054, 26167, 26278, 26388, 26497, 26605, 26712, 26818, 26923, 27026,
  27129, 27231, 27331, 27430, 27529, 27626, 27722, 27817, 27910, 28003,
  28095, 28185, 28275, 28363, 28450, 28536, 28621, 28705, 28788, 28869,
  28950, 29029, 29107, 29184, 29260, 29335, 29409, 29481, 29553, 29623,
  29692, 29760, 29827, 29893, 29958, 30021, 30084, 30145, 30205, 30264,
  30322, 30378, 30434, 30488, 30542, 30594, 30645, 30694, 30743, 30791,
  30837, 30882, 30927, 30970, 31011, 31052, 31092, 31130, 31167, 31204,
  31239, 31272, 31305, 31337, 31367, 31397, 31425, 31452, 31478, 31502,
  31526, 31549, 31570, 31590, 31609, 31627, 31644, 31660, 31674, 31688,
  31700, 31711, 31721, 31730, 31738, 31744, 31750, 31754, 31757, 31760,
  31761, 31760, 31759, 31757, 31753, 31749, 31743, 31736, 31728, 31719,
  31709, 31698, 31685, 31672, 31657, 31641, 31624, 31606, 31587, 31567,
  31546, 31524, 31500, 31475, 31450, 31423, 31395, 31366, 31336, 31305,
  31273, 31239, 31205, 31169, 31133, 31095, 31056, 31016, 30975, 30933,
  30890, 30846, 30801, 30754, 30707, 30659, 30609, 30558, 30507, 30454,
  30400, 30345, 30289, 30232, 30174, 30115, 30055, 29994, 29932, 29868,
  29804, 29738, 29672, 29605, 29536, 29466, 29396, 29324, 29252, 29178,
  29103, 29027, 28951, 28873, 28794, 28714, 28633, 28552, 28469, 28385,
  28300, 28214, 28127, 28039, 27950, 27861, 27770, 27678, 27585, 27491,
  27396, 27301, 27204, 27106, 27007, 26908, 26807, 26705, 26603, 26499,
  26395, 26289, 26183, 26076, 25967, 25858, 25748, 25637, 25525, 25412,
  25298, 25183, 25067, 24951, 24833, 24715, 24595, 24475, 24354, 24232,
  24109, 23985, 23860, 23735, 23608, 23481, 23353, 23223, 23093, 22963,
  22831, 22698, 22565, 22431, 22296, 22160, 22023, 21885, 21747, 21608,
  21468, 21327, 21185, 21043, 20899, 20755, 20611, 20465, 20319, 20171,
  20023, 19875, 19725, 19575, 19424, 19272, 19120, 18967, 18813, 18658,
  18503, 18347, 18190, 18032, 17874, 17715, 17556, 17395, 17234, 17073,
  16910, 16747, 16584, 16420, 16255, 16089, 15923, 15756, 15589, 15421,
  15252, 15083, 14913, 14743, 14572, 14400, 14228, 14055, 13882, 13708,
  13534, 13359, 13183, 13007, 12831, 12654, 12476, 12298, 12120, 11941,
  11761, 11581, 11401, 11220, 11039, 10857, 10675, 10492, 10309, 10125,
  9941, 9757, 9572, 9387, 9202, 9016, 8830, 8643, 8456, 8269,
  8081, 7894, 7705, 7517, 7328, 7139, 6949, 6759, 6569, 6379,
  6189, 5998, 5807, 5615, 5424, 5232, 5040, 4848, 4656, 4463,
  4270, 4077, 3884, 3691, 3498, 3304, 3110, 2917, 2723, 2529,
  2334, 2140, 1946, 1752, 1557, 1362, 1168, 973, 779, 584,
  389, 194, 0, -194, -389, -584, -779, -973, -1168, -1362,
  -1557, -1752, -1946, -2140, -2334, -2529, -2723, -2917, -3110, -3304,
  -3498, -3691, -3884, -4077, -4270, -4463, -4656, -4848, -5040, -5232,
  -5424, -5615, -5807, -5998, -6189, -6379, -6569, -6759, -6949, -7139,
  -7328, -7517, -7705, -7894, -8081, -8269, -8456, -8643, -8830, -9016,
  -9202, -9387, -9572, -9757, -9941, -10125, -10309, -10492, -10675, -10857,
  -11039, -11220, -11401, -11581, -11761, -11941, -12120, -12298, -12476, -12654,
  -12831, -13007, -13183, -13359, -13534, -13708, -13882, -14055, -14228, -14400,
  -14572, -14743, -14913, -15083, -15252, -15421, -15589, -15756, -15923, -16089,
  -16255, -16420, -16584, -16747, -16910, -17073, -17234, -17395, -17556, -17715,
  -17874, -18032, -18190, -18347, -18503, -18658, -18813, -18967, -19120, -19272,
  -19424, -19575, -19725, -19875, -20023, -20171, -20319, -20465, -20611, -20755,
  -20899, -21043, -21185, -21327, -21468, -21608, -21747, -21885, -22023, -22160,
  -22296, -22431, -22565, -22698, -22831, -22963, -23093, -23223, -23353, -23481,
  -23608, -23735, -23860, -23985, -24109, -24232, -24354, -24475, -24595, -24715,
  -24833, -24951, -25067, -25183, -25298, -25412, -25525, -25637, -25748, -25858,
  -25967, -26076, -26183, -26289, -26395, -26499, -26603, -26705, -26807, -26908,
  -27007, -27106, -27204, -27301, -27396, -27491, -27585, -27678, -27770, -27861,
  -27950, -28039, -28127, -28214, -28300, -28385, -28469, -28552, -28633, -28714,
  -28794, -28873, -28951, -29027, -29103, -29178, -29252, -29324, -29396, -29466,
  -29536, -29605, -29672, -29738, -29804, -29868, -29932, -29994, -30055, -30115,
  -30174, -30232, -30289, -30345, -30400, -30454, -30507, -30558, -30609, -30659,
  -30707, -30754, -30801, -30846, -30890, -30933, -30975, -31016, -31056, -31095,
  -31133, -31169, -31205, -31239, -31273, -31305, -31336, -31366, -31395, -31423,
  -31450, -31475, -31500, -31524, -31546, -31567, -31587, -31606, -31624, -31641,
  -31657, -31672, -31685, -31698, -31709, -31719, -31728, -31736, -31743, -31749,
  -31753, -31757, -31759, -31760, -31761, -31760, -31757, -31754, -31750, -31744,
  -31738, -31730, -31721, -31711, -31700, -31688, -31674, -31660, -31644, -31627,
  -31609, -31590, -31570, -31549, -31526, -31502, -31478, -31452, -31425, -31397,
  -31367, -31337, -31305, -31272, -31239, -31204, -31167, -31130, -31092, -31052,
  -31011, -30970, -30927, -30882, -30837, -30791, -30743, -30694, -30645, -30594,
  -30542, -30488, -30434, -30378, -30322, -30264, -30205, -30145, -30084, -30021,
  -29958, -29893, -29827, -29760, -29692, -29623, -29553, -29481, -29409, -29335,
  -29260, -29184, -29107, -29029, -28950, -28869, -28788, -28705, -28621, -28536,
  -28450, -28363, -28275, -28185, -28095, -28003, -27910, -27817, -27722, -27626,
  -27529, -27430, -27331, -27231, -27129, -27026, -26923, -26818, -26712, -26605,
  -26497, -26388, -26278, -26167, -26054, -25941, -25827, -25711, -25595, -25477,
  -25358, -25239, -25118, -24996, -24873, -24749, -24624, -24499, -24372, -24244,
  -24115, -23985, -23854, -23722, -23589, -23455, -23320, -23184, -23047, -22909,
  -22770, -22630, -22489, -22347, -22204, -22060, -21916, -21770, -21623, -21476,
  -21327, -21178, -21028, -20876, -20724, -20571, -20417, -20262, -20107, -19950,
  -19793, -19634, -19475, -19315, -19154, -18992, -18829, -18666, -18502, -18336,
  -18170, -18004, -17836, -17668, -17498, -17329, -17158, -16986, -16814, -16641,
  -16467, -16293, -16117, -15941, -15764, -15587, -15409, -15230, -15050, -14870,
  -14689, -14507, -14325, -14142, -13959, -13774, -13589, -13404, -13218, -13031,
  -12844, -12656, -12467, -12278, -12088, -11898, -11707, -11516, -11324, -11132,
  -10939, -10745, -10551, -10357, -10162, -9967, -9771, -9574, -9378, -9180,
  -8983, -8785, -8586, -8387, -8188, -7988, -7788, -7588, -7387, -7186,
  -6984, -6783, -6580, -6378, -6175, -5972, -5769, -5565, -5361, -5157,
  -4953, -4748, -4543, -4338, -4133, -3928, -3722, -3516, -3310, -3104,
  -2898, -2691, -2485, -2278, -2071, -1864, -1657, -1450, -1243, -1036,
  -829, -621, -414, -207,
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
