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
    0, 763, 1527, 2289, 3050, 3810, 4567, 5322, 6073, 6821,
    7565, 8305, 9039, 9769, 10493, 11211, 11922, 12626, 13324, 14013,
    14695, 15368, 16032, 16688, 17333, 17969, 18595, 19210, 19814, 20408,
    20989, 21559, 22117, 22663, 23196, 23716, 24223, 24717, 25197, 25664,
    26116, 26555, 26979, 27388, 27783, 28163, 28529, 28879, 29214, 29534,
    29838, 30127, 30401, 30659, 30901, 31128, 31340, 31535, 31716, 31880,
    32029, 32163, 32281, 32384, 32472, 32545, 32602, 32645, 32673, 32686,
    32685, 32670, 32640, 32596, 32539, 32468, 32384, 32287, 32176, 32054,
    31918, 31771, 31612, 31441, 31259, 31066, 30863, 30649, 30424, 30191,
    29947, 29695, 29434, 29165, 28887, 28602, 28309, 28010, 27704, 27392,
    27074, 26750, 26421, 26088, 25750, 25408, 25063, 24715, 24363, 24010,
    23654, 23296, 22938, 22578, 22218, 21857, 21497, 21138, 20779, 20422,
    20067, 19713, 19362, 19013, 18668, 18326, 17988, 17653, 17324, 16998,
    16678, 16363, 16054, 15750, 15453, 15162, 14877, 14600, 14329, 14066,
    13811, 13563, 13324, 13092, 12869, 12654, 12449, 12252, 12064, 11885,
    11715, 11555, 11404, 11263, 11132, 11010, 10898, 10796, 10704, 10622,
    10549, 10487, 10435, 10392, 10359, 10337, 10324, 10321, 10327, 10344,
    10369, 10405, 10449, 10503, 10566, 10638, 10719, 10808, 10906, 11013,
    11127, 11250, 11380, 11518, 11664, 11816, 11976, 12142, 12315, 12495,
    12680, 12871, 13067, 13269, 13476, 13688, 13904, 14124, 14348, 14576,
    14807, 15041, 15278, 15517, 15759, 16002, 16247, 16493, 16740, 16988,
    17236, 17484, 17732, 17979, 18225, 18471, 18714, 18957, 19197, 19435,
    19670, 19902, 20132, 20358, 20580, 20798, 21013, 21222, 21427, 21628,
    21823, 22012, 22196, 22375, 22547, 22713, 22873, 23026, 23172, 23311,
    23444, 23569, 23687, 23797, 23899, 23994, 24081, 24160, 24231, 24293,
    24348, 24394, 24432, 24461, 24483, 24495, 24500, 24495, 24483, 24461,
    24432, 24394, 24348, 24293, 24231, 24160, 24081, 23994, 23899, 23797,
    23687, 23569, 23444, 23311, 23172, 23026, 22873, 22713, 22547, 22375,
    22196, 22012, 21823, 21628, 21427, 21222, 21013, 20798, 20580, 20358,
    20132, 19902, 19670, 19435, 19197, 18957, 18714, 18471, 18225, 17979,
    17732, 17484, 17236, 16988, 16740, 16493, 16247, 16002, 15759, 15517,
    15278, 15041, 14807, 14576, 14348, 14124, 13904, 13688, 13476, 13269,
    13067, 12871, 12680, 12495, 12315, 12142, 11976, 11816, 11664, 11518,
    11380, 11250, 11127, 11013, 10906, 10808, 10719, 10638, 10566, 10503,
    10449, 10405, 10369, 10344, 10327, 10321, 10324, 10337, 10359, 10392,
    10435, 10487, 10549, 10622, 10704, 10796, 10898, 11010, 11132, 11263,
    11404, 11555, 11715, 11885, 12064, 12252, 12449, 12654, 12869, 13092,
    13324, 13563, 13811, 14066, 14329, 14600, 14877, 15162, 15453, 15750,
    16054, 16363, 16678, 16998, 17324, 17653, 17988, 18326, 18668, 19013,
    19362, 19713, 20067, 20422, 20779, 21138, 21497, 21857, 22218, 22578,
    22938, 23296, 23654, 24010, 24363, 24715, 25063, 25408, 25750, 26088,
    26421, 26750, 27074, 27392, 27704, 28010, 28309, 28602, 28887, 29165,
    29434, 29695, 29947, 30191, 30424, 30649, 30863, 31066, 31259, 31441,
    31612, 31771, 31918, 32054, 32176, 32287, 32384, 32468, 32539, 32596,
    32640, 32670, 32685, 32686, 32673, 32645, 32602, 32545, 32472, 32384,
    32281, 32163, 32029, 31880, 31716, 31535, 31340, 31128, 30901, 30659,
    30401, 30127, 29838, 29534, 29214, 28879, 28529, 28163, 27783, 27388,
    26979, 26555, 26116, 25664, 25197, 24717, 24223, 23716, 23196, 22663,
    22117, 21559, 20989, 20408, 19814, 19210, 18595, 17969, 17333, 16688,
    16032, 15368, 14695, 14013, 13324, 12626, 11922, 11211, 10493, 9769,
    9039, 8305, 7565, 6821, 6073, 5322, 4567, 3810, 3050, 2289,
    1527, 763, 0, -763, -1527, -2289, -3050, -3810, -4567, -5322,
    -6073, -6821, -7565, -8305, -9039, -9769, -10493, -11211, -11922, -12626,
    -13324, -14013, -14695, -15368, -16032, -16688, -17333, -17969, -18595, -19210,
    -19814, -20408, -20989, -21559, -22117, -22663, -23196, -23716, -24223, -24717,
    -25197, -25664, -26116, -26555, -26979, -27388, -27783, -28163, -28529, -28879,
    -29214, -29534, -29838, -30127, -30401, -30659, -30901, -31128, -31340, -31535,
    -31716, -31880, -32029, -32163, -32281, -32384, -32472, -32545, -32602, -32645,
    -32673, -32686, -32685, -32670, -32640, -32596, -32539, -32468, -32384, -32287,
    -32176, -32054, -31918, -31771, -31612, -31441, -31259, -31066, -30863, -30649,
    -30424, -30191, -29947, -29695, -29434, -29165, -28887, -28602, -28309, -28010,
    -27704, -27392, -27074, -26750, -26421, -26088, -25750, -25408, -25063, -24715,
    -24363, -24010, -23654, -23296, -22938, -22578, -22218, -21857, -21497, -21138,
    -20779, -20422, -20067, -19713, -19362, -19013, -18668, -18326, -17988, -17653,
    -17324, -16998, -16678, -16363, -16054, -15750, -15453, -15162, -14877, -14600,
    -14329, -14066, -13811, -13563, -13324, -13092, -12869, -12654, -12449, -12252,
    -12064, -11885, -11715, -11555, -11404, -11263, -11132, -11010, -10898, -10796,
    -10704, -10622, -10549, -10487, -10435, -10392, -10359, -10337, -10324, -10321,
    -10327, -10344, -10369, -10405, -10449, -10503, -10566, -10638, -10719, -10808,
    -10906, -11013, -11127, -11250, -11380, -11518, -11664, -11816, -11976, -12142,
    -12315, -12495, -12680, -12871, -13067, -13269, -13476, -13688, -13904, -14124,
    -14348, -14576, -14807, -15041, -15278, -15517, -15759, -16002, -16247, -16493,
    -16740, -16988, -17236, -17484, -17732, -17979, -18225, -18471, -18714, -18957,
    -19197, -19435, -19670, -19902, -20132, -20358, -20580, -20798, -21013, -21222,
    -21427, -21628, -21823, -22012, -22196, -22375, -22547, -22713, -22873, -23026,
    -23172, -23311, -23444, -23569, -23687, -23797, -23899, -23994, -24081, -24160,
    -24231, -24293, -24348, -24394, -24432, -24461, -24483, -24495, -24500, -24495,
    -24483, -24461, -24432, -24394, -24348, -24293, -24231, -24160, -24081, -23994,
    -23899, -23797, -23687, -23569, -23444, -23311, -23172, -23026, -22873, -22713,
    -22547, -22375, -22196, -22012, -21823, -21628, -21427, -21222, -21013, -20798,
    -20580, -20358, -20132, -19902, -19670, -19435, -19197, -18957, -18714, -18471,
    -18225, -17979, -17732, -17484, -17236, -16988, -16740, -16493, -16247, -16002,
    -15759, -15517, -15278, -15041, -14807, -14576, -14348, -14124, -13904, -13688,
    -13476, -13269, -13067, -12871, -12680, -12495, -12315, -12142, -11976, -11816,
    -11664, -11518, -11380, -11250, -11127, -11013, -10906, -10808, -10719, -10638,
    -10566, -10503, -10449, -10405, -10369, -10344, -10327, -10321, -10324, -10337,
    -10359, -10392, -10435, -10487, -10549, -10622, -10704, -10796, -10898, -11010,
    -11132, -11263, -11404, -11555, -11715, -11885, -12064, -12252, -12449, -12654,
    -12869, -13092, -13324, -13563, -13811, -14066, -14329, -14600, -14877, -15162,
    -15453, -15750, -16054, -16363, -16678, -16998, -17324, -17653, -17988, -18326,
    -18668, -19013, -19362, -19713, -20067, -20422, -20779, -21138, -21497, -21857,
    -22218, -22578, -22938, -23296, -23654, -24010, -24363, -24715, -25063, -25408,
    -25750, -26088, -26421, -26750, -27074, -27392, -27704, -28010, -28309, -28602,
    -28887, -29165, -29434, -29695, -29947, -30191, -30424, -30649, -30863, -31066,
    -31259, -31441, -31612, -31771, -31918, -32054, -32176, -32287, -32384, -32468,
    -32539, -32596, -32640, -32670, -32685, -32686, -32673, -32645, -32602, -32545,
    -32472, -32384, -32281, -32163, -32029, -31880, -31716, -31535, -31340, -31128,
    -30901, -30659, -30401, -30127, -29838, -29534, -29214, -28879, -28529, -28163,
    -27783, -27388, -26979, -26555, -26116, -25664, -25197, -24717, -24223, -23716,
    -23196, -22663, -22117, -21559, -20989, -20408, -19814, -19210, -18595, -17969,
    -17333, -16688, -16032, -15368, -14695, -14013, -13324, -12626, -11922, -11211,
    -10493, -9769, -9039, -8305, -7565, -6821, -6073, -5322, -4567, -3810,
    -3050, -2289, -1527, -763,

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

const int16_t alawTable[256] = { -5504, -5248, -6016, -5760, -4480, -4224, -4992, -4736, -7552, -7296, -8064, -7808, -6528, -6272, -7040, -6784, -2752, -2624, -3008, -2880, -2240, -2112, -2496, -2368, -3776, -3648, -4032, -3904, -3264, -3136, -3520, -3392, -22016, -20992, -24064, -23040, -17920, -16896, -19968, -18944, -30208, -29184, -32256, -31232, -26112, -25088, -28160, -27136, -11008, -10496, -12032, -11520, -8960, -8448, -9984, -9472, -15104, -14592, -16128, -15616, -13056, -12544, -14080, -13568, -344, -328, -376, -360, -280, -264, -312, -296, -472, -456, -504, -488, -408, -392, -440, -424, -88, -72, -120, -104, -24, -8, -56, -40, -216, -200, -248, -232, -152, -136, -184, -168, -1376, -1312, -1504, -1440, -1120, -1056, -1248, -1184, -1888, -1824, -2016, -1952, -1632, -1568, -1760, -1696, -688, -656, -752, -720, -560, -528, -624, -592, -944, -912, -1008, -976, -816, -784, -880, -848, 5504, 5248, 6016, 5760, 4480, 4224, 4992, 4736, 7552, 7296, 8064, 7808, 6528, 6272, 7040, 6784, 2752, 2624, 3008, 2880, 2240, 2112, 2496, 2368, 3776, 3648, 4032, 3904, 3264, 3136, 3520, 3392, 22016, 20992, 24064, 23040, 17920, 16896, 19968, 18944, 30208, 29184, 32256, 31232, 26112, 25088, 28160, 27136, 11008, 10496, 12032, 11520, 8960, 8448, 9984, 9472, 15104, 14592, 16128, 15616, 13056, 12544, 14080, 13568, 344, 328, 376, 360, 280, 264, 312, 296, 472, 456, 504, 488, 408, 392, 440, 424, 88, 72, 120, 104, 24, 8, 56, 40, 216, 200, 248, 232, 152, 136, 184, 168, 1376, 1312, 1504, 1440, 1120, 1056, 1248, 1184, 1888, 1824, 2016, 1952, 1632, 1568, 1760, 1696, 688, 656, 752, 720, 560, 528, 624, 592, 944, 912, 1008, 976, 816, 784, 880, 848 };
const int16_t ulawTable[256] = { -32124, -31100, -30076, -29052, -28028, -27004, -25980, -24956, -23932, -22908, -21884, -20860, -19836, -18812, -17788, -16764, -15996, -15484, -14972, -14460, -13948, -13436, -12924, -12412, -11900, -11388, -10876, -10364, -9852, -9340, -8828, -8316, -7932, -7676, -7420, -7164, -6908, -6652, -6396, -6140, -5884, -5628, -5372, -5116, -4860, -4604, -4348, -4092, -3900, -3772, -3644, -3516, -3388, -3260, -3132, -3004, -2876, -2748, -2620, -2492, -2364, -2236, -2108, -1980, -1884, -1820, -1756, -1692, -1628, -1564, -1500, -1436, -1372, -1308, -1244, -1180, -1116, -1052, -988, -924, -876, -844, -812, -780, -748, -716, -684, -652, -620, -588, -556, -524, -492, -460, -428, -396, -372, -356, -340, -324, -308, -292, -276, -260, -244, -228, -212, -196, -180, -164, -148, -132, -120, -112, -104, -96, -88, -80, -72, -64, -56, -48, -40, -32, -24, -16, -8, 0, 32124, 31100, 30076, 29052, 28028, 27004, 25980, 24956, 23932, 22908, 21884, 20860, 19836, 18812, 17788, 16764, 15996, 15484, 14972, 14460, 13948, 13436, 12924, 12412, 11900, 11388, 10876, 10364, 9852, 9340, 8828, 8316, 7932, 7676, 7420, 7164, 6908, 6652, 6396, 6140, 5884, 5628, 5372, 5116, 4860, 4604, 4348, 4092, 3900, 3772, 3644, 3516, 3388, 3260, 3132, 3004, 2876, 2748, 2620, 2492, 2364, 2236, 2108, 1980, 1884, 1820, 1756, 1692, 1628, 1564, 1500, 1436, 1372, 1308, 1244, 1180, 1116, 1052, 988, 924, 876, 844, 812, 780, 748, 716, 684, 652, 620, 588, 556, 524, 492, 460, 428, 396, 372, 356, 340, 324, 308, 292, 276, 260, 244, 228, 212, 196, 180, 164, 148, 132, 120, 112, 104, 96, 88, 80, 72, 64, 56, 48, 40, 32, 24, 16, 8, 0 };

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

void mixSample(uint16_t * result, int sample, unsigned int fade)
{
  *result = limit(0, *result + ((sample >> fade) >> 4), 4095);
}

#if defined(SDCARD) && !defined(SIMU)

#define RIFF_CHUNK_SIZE 12
uint8_t wavBuffer[AUDIO_BUFFER_SIZE*2];

int WavContext::mixBuffer(AudioBuffer *buffer, int volume, unsigned int fade)
{
  FRESULT result = FR_OK;
  UINT read = 0;

  if (fragment.file[1]) {
    result = f_open(&state.file, fragment.file, FA_OPEN_EXISTING | FA_READ);
    fragment.file[1] = 0;
    if (result == FR_OK) {
      result = f_read(&state.file, wavBuffer, RIFF_CHUNK_SIZE+8, &read);
      if (result == FR_OK && read == RIFF_CHUNK_SIZE+8 && !memcmp(wavBuffer, "RIFF", 4) && !memcmp(wavBuffer+8, "WAVEfmt ", 8)) {
        uint32_t size = *((uint32_t *)(wavBuffer+16));
        result = (size < 256 ? f_read(&state.file, wavBuffer, size+8, &read) : FR_DENIED);
        if (result == FR_OK && read == size+8) {
          state.codec = ((uint16_t *)wavBuffer)[0];
          state.freq = ((uint16_t *)wavBuffer)[2];
          uint32_t *wavSamplesPtr = (uint32_t *)(wavBuffer + size);
          uint32_t size = wavSamplesPtr[1];
          if (state.freq != 0 && state.freq * (AUDIO_SAMPLE_RATE / state.freq) == AUDIO_SAMPLE_RATE) {
            state.resampleRatio = (AUDIO_SAMPLE_RATE / state.freq);
            state.readSize = (state.codec == CODEC_ID_PCM_S16LE ? 2*AUDIO_BUFFER_SIZE : AUDIO_BUFFER_SIZE) / state.resampleRatio;
          }
          else {
            result = FR_DENIED;
          }
          while (result == FR_OK && memcmp(wavSamplesPtr, "data", 4) != 0) {
            result = f_lseek(&state.file, f_tell(&state.file)+size);
            if (result == FR_OK) {
              result = f_read(&state.file, wavBuffer, 8, &read);
              if (read != 8) result = FR_DENIED;
              wavSamplesPtr = (uint32_t *)wavBuffer;
              size = wavSamplesPtr[1];
            }
          }
          state.size = size;
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
    result = f_read(&state.file, wavBuffer, state.readSize, &read);
    if (result == FR_OK) {
      if (read > state.size) {
        read = state.size;
      }
      state.size -= read;

      if (read != state.readSize) {
        f_close(&state.file);
        fragment.clear();
      }

      uint16_t * samples = buffer->data;
      if (state.codec == CODEC_ID_PCM_S16LE) {
        read /= 2;
        for (uint32_t i=0; i<read; i++) {
          for (uint8_t j=0; j<state.resampleRatio; j++)
            mixSample(samples++, ((int16_t *)wavBuffer)[i], fade+2-volume);
        }
      }
      else if (state.codec == CODEC_ID_PCM_ALAW) {
        for (uint32_t i=0; i<read; i++)
          for (uint8_t j=0; j<state.resampleRatio; j++)
            mixSample(samples++, alawTable[wavBuffer[i]], fade+2-volume);
      }
      else if (state.codec == CODEC_ID_PCM_MULAW) {
        for (uint32_t i=0; i<read; i++)
          for (uint8_t j=0; j<state.resampleRatio; j++)
            mixSample(samples++, ulawTable[wavBuffer[i]], fade+2-volume);
      }

      return samples - buffer->data;
    }
  }

  clear();
  return -result;
}
#else
int WavContext::mixBuffer(AudioBuffer *buffer, int volume, unsigned int fade)
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

int ToneContext::mixBuffer(AudioBuffer *buffer, int volume, unsigned int fade)
{
  int duration = 0;
  int result = 0;

  int remainingDuration = fragment.tone.duration - state.duration;
  if (remainingDuration > 0) {
    int points;
    double toneIdx = state.idx;

    if (fragment.tone.reset) {
      state.duration = 0;
      state.pause = 0;
    }

    if (fragment.tone.freq != state.freq) {
      state.freq = fragment.tone.freq;
      state.step = double(DIM(sineValues)*fragment.tone.freq) / AUDIO_SAMPLE_RATE;
      state.volume = evalVolumeRatio(fragment.tone.freq, volume);
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
      unsigned int end = toneIdx + (state.step * points);
      if (end > DIM(sineValues))
        end -= (end % DIM(sineValues));
      else
        end = DIM(sineValues);
      points = (double(end) - toneIdx) / state.step;
    }

    for (int i=0; i<points; i++) {
      int16_t sample = sineValues[int(toneIdx)] / state.volume;
      mixSample(&buffer->data[i], sample, fade);
      toneIdx += state.step;
      if ((unsigned int)toneIdx >= DIM(sineValues))
        toneIdx -= DIM(sineValues);
    }

    if (remainingDuration > AUDIO_BUFFER_DURATION) {
      state.duration += AUDIO_BUFFER_DURATION;
      state.idx = toneIdx;
      return AUDIO_BUFFER_SIZE;
    }
    else {
      state.duration = 32000; // once the tone is finished, it's not possible to update its frequency and duration
    }
  }

  remainingDuration = fragment.tone.pause - state.pause;
  if (remainingDuration > 0) {
    result = AUDIO_BUFFER_SIZE;
    state.pause += min<unsigned int>(AUDIO_BUFFER_DURATION-duration, fragment.tone.pause);
    if (fragment.tone.pause > state.pause)
      return result;
  }

  clear();
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

    // mix the priority context (only tones)
    result = priorityContext.mixBuffer(buffer, g_eeGeneral.beepVolume, fade);
    if (result > 0) {
      size = result;
      fade += 1;
    }

    // mix the normal context (tones and wavs)
    if (normalContext.fragment.type == FRAGMENT_TONE)
      result = normalContext.tone.mixBuffer(buffer, g_eeGeneral.beepVolume, fade);
    else if (normalContext.fragment.type == FRAGMENT_FILE)
      result = normalContext.wav.mixBuffer(buffer, g_eeGeneral.wavVolume, fade);
    else
      result = 0;
    if (result > 0) {
      size = max(size, result);
      fade += 1;
    }
    else {
      CoEnterMutexSection(audioMutex);
      if (ridx != widx) {
        normalContext.fragment = fragments[ridx];
        if (!fragments[ridx].repeat--) {
          ridx = (ridx + 1) % AUDIO_QUEUE_LENGTH;
        }
      }
      CoLeaveMutexSection(audioMutex);
    }

    // mix the vario context
    result = varioContext.mixBuffer(buffer, g_eeGeneral.varioVolume, fade);
    if (result > 0) {
      size = max(size, result);
      fade += 1;
    }

    // mix the background context
    if (!isFunctionActive(FUNCTION_BACKGND_MUSIC_PAUSE)) {
      result = backgroundContext.mixBuffer(buffer, g_eeGeneral.backgroundVolume, fade);
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
  if (normalContext.fragment.id == id || backgroundContext.fragment.id == id)
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
    AudioFragment & fragment = varioContext.fragment;
    fragment.type = FRAGMENT_TONE;
    fragment.tone.freq = freq;
    fragment.tone.duration = len;
    fragment.tone.pause = pause;
    fragment.tone.reset = (flags & PLAY_NOW);
  }
  else {
    freq += g_eeGeneral.speakerPitch * 15;
    len = getToneLength(len);

    if (flags & PLAY_NOW) {
      AudioFragment & fragment = priorityContext.fragment;
      if (fragment.type == FRAGMENT_EMPTY) {
        priorityContext.clear();
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
    AudioFragment & fragment = priorityContext.fragment;
    if (fragment.type == FRAGMENT_EMPTY) {
      priorityContext.clear();
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
  priorityContext.clear();
  normalContext.tone.clear();
  varioContext.clear();
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
