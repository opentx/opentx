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
    0, 196, 392, 588, 784, 980, 1175, 1370, 1564, 1758,
    1951, 2143, 2335, 2525, 2715, 2904, 3091, 3278, 3463, 3647,
    3829, 4011, 4190, 4369, 4545, 4720, 4894, 5065, 5235, 5403,
    5569, 5733, 5895, 6055, 6213, 6369, 6522, 6673, 6822, 6969,
    7113, 7255, 7395, 7532, 7667, 7799, 7929, 8056, 8180, 8302,
    8422, 8539, 8653, 8765, 8874, 8980, 9084, 9185, 9283, 9379,
    9472, 9562, 9650, 9735, 9818, 9898, 9975, 10050, 10123, 10192,
    10260, 10324, 10387, 10447, 10504, 10559, 10612, 10663, 10711, 10757,
    10801, 10843, 10882, 10920, 10955, 10989, 11020, 11050, 11078, 11104,
    11128, 11151, 11172, 11191, 11209, 11225, 11240, 11254, 11266, 11277,
    11287, 11296, 11303, 11310, 11316, 11320, 11324, 11327, 11330, 11331,
    11332, 11333, 11333, 11333, 11332, 11331, 11329, 11328, 11326, 11324,
    11323, 11321, 11319, 11318, 11316, 11315, 11314, 11313, 11313, 11313,
    11314, 11315, 11317, 11319, 11323, 11326, 11331, 11336, 11342, 11349,
    11356, 11365, 11375, 11385, 11397, 11409, 11423, 11437, 11453, 11470,
    11488, 11507, 11527, 11548, 11571, 11595, 11620, 11646, 11673, 11702,
    11732, 11763, 11795, 11828, 11863, 11899, 11936, 11974, 12013, 12054,
    12095, 12138, 12182, 12227, 12273, 12320, 12368, 12417, 12467, 12518,
    12570, 12623, 12676, 12731, 12786, 12842, 12898, 12956, 13014, 13072,
    13131, 13191, 13251, 13311, 13372, 13433, 13495, 13556, 13618, 13680,
    13743, 13805, 13867, 13929, 13991, 14053, 14115, 14177, 14238, 14299,
    14359, 14419, 14479, 14538, 14597, 14655, 14712, 14768, 14824, 14879,
    14933, 14986, 15039, 15090, 15140, 15189, 15237, 15284, 15330, 15375,
    15418, 15460, 15500, 15539, 15577, 15614, 15648, 15682, 15714, 15744,
    15772, 15799, 15825, 15849, 15871, 15891, 15910, 15927, 15942, 15955,
    15967, 15977, 15985, 15991, 15996, 15999, 16000, 15999, 15996, 15991,
    15985, 15977, 15967, 15955, 15942, 15927, 15910, 15891, 15871, 15849,
    15825, 15799, 15772, 15744, 15714, 15682, 15648, 15614, 15577, 15539,
    15500, 15460, 15418, 15375, 15330, 15284, 15237, 15189, 15140, 15090,
    15039, 14986, 14933, 14879, 14824, 14768, 14712, 14655, 14597, 14538,
    14479, 14419, 14359, 14299, 14238, 14177, 14115, 14053, 13991, 13929,
    13867, 13805, 13743, 13680, 13618, 13556, 13495, 13433, 13372, 13311,
    13251, 13191, 13131, 13072, 13014, 12956, 12898, 12842, 12786, 12731,
    12676, 12623, 12570, 12518, 12467, 12417, 12368, 12320, 12273, 12227,
    12182, 12138, 12095, 12054, 12013, 11974, 11936, 11899, 11863, 11828,
    11795, 11763, 11732, 11702, 11673, 11646, 11620, 11595, 11571, 11548,
    11527, 11507, 11488, 11470, 11453, 11437, 11423, 11409, 11397, 11385,
    11375, 11365, 11356, 11349, 11342, 11336, 11331, 11326, 11323, 11319,
    11317, 11315, 11314, 11313, 11313, 11313, 11314, 11315, 11316, 11318,
    11319, 11321, 11323, 11324, 11326, 11328, 11329, 11331, 11332, 11333,
    11333, 11333, 11332, 11331, 11330, 11327, 11324, 11320, 11316, 11310,
    11303, 11296, 11287, 11277, 11266, 11254, 11240, 11225, 11209, 11191,
    11172, 11151, 11128, 11104, 11078, 11050, 11020, 10989, 10955, 10920,
    10882, 10843, 10801, 10757, 10711, 10663, 10612, 10559, 10504, 10447,
    10387, 10324, 10260, 10192, 10123, 10050, 9975, 9898, 9818, 9735,
    9650, 9562, 9472, 9379, 9283, 9185, 9084, 8980, 8874, 8765,
    8653, 8539, 8422, 8302, 8180, 8056, 7929, 7799, 7667, 7532,
    7395, 7255, 7113, 6969, 6822, 6673, 6522, 6369, 6213, 6055,
    5895, 5733, 5569, 5403, 5235, 5065, 4894, 4720, 4545, 4369,
    4190, 4011, 3829, 3647, 3463, 3278, 3091, 2904, 2715, 2525,
    2335, 2143, 1951, 1758, 1564, 1370, 1175, 980, 784, 588,
    392, 196, 0, -196, -392, -588, -784, -980, -1175, -1370,
    -1564, -1758, -1951, -2143, -2335, -2525, -2715, -2904, -3091, -3278,
    -3463, -3647, -3829, -4011, -4190, -4369, -4545, -4720, -4894, -5065,
    -5235, -5403, -5569, -5733, -5895, -6055, -6213, -6369, -6522, -6673,
    -6822, -6969, -7113, -7255, -7395, -7532, -7667, -7799, -7929, -8056,
    -8180, -8302, -8422, -8539, -8653, -8765, -8874, -8980, -9084, -9185,
    -9283, -9379, -9472, -9562, -9650, -9735, -9818, -9898, -9975, -10050,
    -10123, -10192, -10260, -10324, -10387, -10447, -10504, -10559, -10612, -10663,
    -10711, -10757, -10801, -10843, -10882, -10920, -10955, -10989, -11020, -11050,
    -11078, -11104, -11128, -11151, -11172, -11191, -11209, -11225, -11240, -11254,
    -11266, -11277, -11287, -11296, -11303, -11310, -11316, -11320, -11324, -11327,
    -11330, -11331, -11332, -11333, -11333, -11333, -11332, -11331, -11329, -11328,
    -11326, -11324, -11323, -11321, -11319, -11318, -11316, -11315, -11314, -11313,
    -11313, -11313, -11314, -11315, -11317, -11319, -11323, -11326, -11331, -11336,
    -11342, -11349, -11356, -11365, -11375, -11385, -11397, -11409, -11423, -11437,
    -11453, -11470, -11488, -11507, -11527, -11548, -11571, -11595, -11620, -11646,
    -11673, -11702, -11732, -11763, -11795, -11828, -11863, -11899, -11936, -11974,
    -12013, -12054, -12095, -12138, -12182, -12227, -12273, -12320, -12368, -12417,
    -12467, -12518, -12570, -12623, -12676, -12731, -12786, -12842, -12898, -12956,
    -13014, -13072, -13131, -13191, -13251, -13311, -13372, -13433, -13495, -13556,
    -13618, -13680, -13743, -13805, -13867, -13929, -13991, -14053, -14115, -14177,
    -14238, -14299, -14359, -14419, -14479, -14538, -14597, -14655, -14712, -14768,
    -14824, -14879, -14933, -14986, -15039, -15090, -15140, -15189, -15237, -15284,
    -15330, -15375, -15418, -15460, -15500, -15539, -15577, -15614, -15648, -15682,
    -15714, -15744, -15772, -15799, -15825, -15849, -15871, -15891, -15910, -15927,
    -15942, -15955, -15967, -15977, -15985, -15991, -15996, -15999, -16000, -15999,
    -15996, -15991, -15985, -15977, -15967, -15955, -15942, -15927, -15910, -15891,
    -15871, -15849, -15825, -15799, -15772, -15744, -15714, -15682, -15648, -15614,
    -15577, -15539, -15500, -15460, -15418, -15375, -15330, -15284, -15237, -15189,
    -15140, -15090, -15039, -14986, -14933, -14879, -14824, -14768, -14712, -14655,
    -14597, -14538, -14479, -14419, -14359, -14299, -14238, -14177, -14115, -14053,
    -13991, -13929, -13867, -13805, -13743, -13680, -13618, -13556, -13495, -13433,
    -13372, -13311, -13251, -13191, -13131, -13072, -13014, -12956, -12898, -12842,
    -12786, -12731, -12676, -12623, -12570, -12518, -12467, -12417, -12368, -12320,
    -12273, -12227, -12182, -12138, -12095, -12054, -12013, -11974, -11936, -11899,
    -11863, -11828, -11795, -11763, -11732, -11702, -11673, -11646, -11620, -11595,
    -11571, -11548, -11527, -11507, -11488, -11470, -11453, -11437, -11423, -11409,
    -11397, -11385, -11375, -11365, -11356, -11349, -11342, -11336, -11331, -11326,
    -11323, -11319, -11317, -11315, -11314, -11313, -11313, -11313, -11314, -11315,
    -11316, -11318, -11319, -11321, -11323, -11324, -11326, -11328, -11329, -11331,
    -11332, -11333, -11333, -11333, -11332, -11331, -11330, -11327, -11324, -11320,
    -11316, -11310, -11303, -11296, -11287, -11277, -11266, -11254, -11240, -11225,
    -11209, -11191, -11172, -11151, -11128, -11104, -11078, -11050, -11020, -10989,
    -10955, -10920, -10882, -10843, -10801, -10757, -10711, -10663, -10612, -10559,
    -10504, -10447, -10387, -10324, -10260, -10192, -10123, -10050, -9975, -9898,
    -9818, -9735, -9650, -9562, -9472, -9379, -9283, -9185, -9084, -8980,
    -8874, -8765, -8653, -8539, -8422, -8302, -8180, -8056, -7929, -7799,
    -7667, -7532, -7395, -7255, -7113, -6969, -6822, -6673, -6522, -6369,
    -6213, -6055, -5895, -5733, -5569, -5403, -5235, -5065, -4894, -4720,
    -4545, -4369, -4190, -4011, -3829, -3647, -3463, -3278, -3091, -2904,
    -2715, -2525, -2335, -2143, -1951, -1758, -1564, -1370, -1175, -980,
    -784, -588, -392, -196,
};

#if defined(SDCARD)
const char * const audioFilenames[] = {
  "tada",
  "bye",
  "thralert",
  "swalert",
  "eebad",
  "eeformat",
  "lowbatt",
  "inactiv",
  "a1_org",
  "a1_red",
  "a2_org",
  "a2_red",
  "a3_org",
  "a3_red",
  "a4_org",
  "a4_red",
  "rssi_org",
  "rssi_red",
  "swr_red",
  "telemko",
  "telemok",
  "trainko",
  "trainok",
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
#if defined(PCBTARANIS) || defined(PCBFLAMENCO) || defined(PCBHORUS)
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
  "timer30"
};

uint64_t sdAvailableSystemAudioFiles = 0;
uint32_t sdAvailablePhaseAudioFiles = 0;
uint64_t sdAvailableSwitchAudioFiles = 0;
uint64_t sdAvailableLogicalSwitchAudioFiles = 0;

#define MASK_SYSTEM_AUDIO_FILE(index)                 ((uint64_t)1 << (index))
#define MASK_PHASE_AUDIO_FILE(index, event)           ((uint32_t)1 << (2*(index)+(event)))
#define MASK_SWITCH_AUDIO_FILE(index)                 ((uint64_t)1 << (index))
#define MASK_LOGICAL_SWITCH_AUDIO_FILE(index, event)  ((uint64_t)1 << (2*(index)+(event)))

char * getAudioPath(char * path)
{
  strcpy(path, SOUNDS_PATH "/");
  strncpy(path+SOUNDS_PATH_LNG_OFS, currentLanguagePack->id, 2);
  return path + sizeof(SOUNDS_PATH);
}

char * getSystemAudioPath(char * path)
{
  char * str = getAudioPath(path);
  strcpy(str, SYSTEM_SUBDIR "/");
  return str + sizeof(SYSTEM_SUBDIR);
}

void getSystemAudioFile(char * filename, int index)
{
  char * str = getSystemAudioPath(filename);
  strcpy(str, audioFilenames[index]);
  strcat(str, SOUNDS_EXT);
}

void referenceSystemAudioFiles()
{
  char path[AUDIO_FILENAME_MAXLEN+1];
  FILINFO fno;
  DIR dir;
  char *fn;   /* This function is assuming non-Unicode cfg. */
  TCHAR lfn[_MAX_LFN + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);

  uint64_t availableAudioFiles = 0;

  assert(sizeof(audioFilenames)==AU_FRSKY_FIRST*sizeof(char *));
  assert(sizeof(sdAvailableSystemAudioFiles)*8 >= AU_FRSKY_FIRST);

  char * filename = getSystemAudioPath(path);
  *(filename-1) = '\0';

  FRESULT res = f_opendir(&dir, path);        /* Open the directory */
  if (res == FR_OK) {
    for (;;) {
      res = f_readdir(&dir, &fno);                   /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
      fn = *fno.lfname ? fno.lfname : fno.fname;
      uint8_t len = strlen(fn);

      TRACE("%s", fn);

      // Eliminates directories / non wav files
      if (len < 5 || strcasecmp(fn+len-4, SOUNDS_EXT) || (fno.fattrib & AM_DIR)) continue;

      for (int i=0; i<AU_FRSKY_FIRST; i++) {
        getSystemAudioFile(path, i);
        if (!strcasecmp(filename, fn)) {
          availableAudioFiles |= MASK_SYSTEM_AUDIO_FILE(i);
          break;
        }
      }
    }
    f_closedir(&dir);
  }

  sdAvailableSystemAudioFiles = availableAudioFiles;
}

const char * const suffixes[] = { "-off", "-on" };

char * getModelAudioPath(char * path)
{
  strcpy(path, SOUNDS_PATH "/");
  strncpy(path+SOUNDS_PATH_LNG_OFS, currentLanguagePack->id, 2);
  char * result = strcat_modelname(path+sizeof(SOUNDS_PATH), g_eeGeneral.currModel);
  *result++ = '/';
  *result = '\0';
  return result;
}

void getPhaseAudioFile(char * filename, int index, unsigned int event)
{
  char * str = getModelAudioPath(filename);
  char * tmp = strcat_phasename(str, index);
  strcpy(tmp, suffixes[event]);
  strcat(tmp, SOUNDS_EXT);
}

void getSwitchAudioFile(char * filename, swsrc_t index)
{
  char * str = getModelAudioPath(filename);

#if defined(PCBTARANIS)
  if (index <= SWSRC_LAST_SWITCH) {
    div_t swinfo = switchInfo(index);
    *str++ = 'S';
    *str++ = 'A' + swinfo.quot;
    const char * positions[] = { "-up", "-mid", "-down" };
    strcpy(str, positions[swinfo.rem]);
  }
  else {
    div_t swinfo = div(index - SWSRC_FIRST_MULTIPOS_SWITCH, XPOTS_MULTIPOS_COUNT);
    *str++ = 'S';
    *str++ = '1' + swinfo.quot;
    *str++ = '1' + swinfo.rem;
    *str = '\0';
  }
#else
  int len = STR_VSWITCHES[0];
  strncpy(str, &STR_VSWITCHES[1+(len*index)], len);
  str += len;
  *str = '\0';
#endif
  strcat(str, SOUNDS_EXT);
}

void getLogicalSwitchAudioFile(char * filename, int index, unsigned int event)
{
  char * str = getModelAudioPath(filename);

#if defined(PCBTARANIS)
  *str++ = 'L';
  if (index >= 9) {
    div_t qr = div(index+1, 10);
    *str++ = '0' + qr.quot;
    *str++ = '0' + qr.rem;
  }
  else {
    *str++ = '1' + index;
  }
#else
  int len = STR_VSWITCHES[0];
  strncpy(str, &STR_VSWITCHES[1+len*(index+SWSRC_FIRST_LOGICAL_SWITCH)], len);
  str += len;
#endif

  strcpy(str, suffixes[event]);
  strcat(str, SOUNDS_EXT);
}

void referenceModelAudioFiles()
{
  char path[AUDIO_FILENAME_MAXLEN+1];
  FILINFO fno;
  DIR dir;
  char *fn;   /* This function is assuming non-Unicode cfg. */
  TCHAR lfn[_MAX_LFN + 1];
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);

  sdAvailablePhaseAudioFiles = 0;
  sdAvailableSwitchAudioFiles = 0;
  sdAvailableLogicalSwitchAudioFiles = 0;
  
  char * filename = getModelAudioPath(path);
  *(filename-1) = '\0';

  FRESULT res = f_opendir(&dir, path);        /* Open the directory */
  if (res == FR_OK) {
    for (;;) {
      res = f_readdir(&dir, &fno);                   /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
      fn = *fno.lfname ? fno.lfname : fno.fname;
      uint8_t len = strlen(fn);
      bool found = false;

      // Eliminates directories / non wav files
      if (len < 5 || strcasecmp(fn+len-4, SOUNDS_EXT) || (fno.fattrib & AM_DIR)) continue;
      TRACE("referenceModelAudioFiles(): using file: %s", fn);

      // Phases Audio Files <phasename>-[on|off].wav
      for (int i=0; i<MAX_FLIGHT_MODES && !found; i++) {
        for (int event=0; event<2; event++) {
          getPhaseAudioFile(path, i, event);
          // TRACE("referenceModelAudioFiles(): searching for %s in %s", filename, fn);
          if (!strcasecmp(filename, fn)) {
            sdAvailablePhaseAudioFiles |= MASK_PHASE_AUDIO_FILE(i, event);
            found = true;
            TRACE("\tfound: %s", filename);
            break;
          }
        }
      }

      // Switches Audio Files <switchname>-[up|mid|down].wav
      for (int i=SWSRC_FIRST_SWITCH; i<=SWSRC_LAST_SWITCH+NUM_XPOTS*XPOTS_MULTIPOS_COUNT && !found; i++) {
        getSwitchAudioFile(path, i);
        // TRACE("referenceModelAudioFiles(): searching for %s in %s", filename, fn);
        if (!strcasecmp(filename, fn)) {
          sdAvailableSwitchAudioFiles |= MASK_SWITCH_AUDIO_FILE(i-SWSRC_FIRST_SWITCH);
          found = true;
          TRACE("\tfound: %s", filename);
        }
      }

      // Logical Switches Audio Files <switchname>-[on|off].wav
      for (int i=0; i<NUM_LOGICAL_SWITCH && !found; i++) {
        for (int event=0; event<2; event++) {
          getLogicalSwitchAudioFile(path, i, event);
          // TRACE("referenceModelAudioFiles(): searching for %s in %s", filename, fn);
          if (!strcasecmp(filename, fn)) {
            sdAvailableLogicalSwitchAudioFiles |= MASK_LOGICAL_SWITCH_AUDIO_FILE(i, event);
            found = true;
            TRACE("\tfound: %s", filename);
            break;
          }
        }
      }
    }
    f_closedir(&dir);
  }
}

bool isAudioFileReferenced(uint32_t i, char * filename)
{
  uint8_t category = (i >> 24);
  uint8_t index = (i >> 16) & 0xFF;
  uint8_t event = i & 0xFF;

  // TRACE("isAudioFileReferenced(%08x)", i);

  if (category == SYSTEM_AUDIO_CATEGORY) {
    if (sdAvailableSystemAudioFiles & MASK_SYSTEM_AUDIO_FILE(event)) {
      getSystemAudioFile(filename, event);
      return true;
    }
  }
  else if (category == PHASE_AUDIO_CATEGORY) {
    if (sdAvailablePhaseAudioFiles & MASK_PHASE_AUDIO_FILE(index, event)) {
      getPhaseAudioFile(filename, index, event);
      return true;
    }
  }
  else if (category == SWITCH_AUDIO_CATEGORY) {
    if (sdAvailableSwitchAudioFiles & MASK_SWITCH_AUDIO_FILE(index)) {
      getSwitchAudioFile(filename, SWSRC_FIRST_SWITCH+index);
      return true;
    }
  }
  else if (category == LOGICAL_SWITCH_AUDIO_CATEGORY) {
    if (sdAvailableLogicalSwitchAudioFiles & MASK_LOGICAL_SWITCH_AUDIO_FILE(index, event)) {
      getLogicalSwitchAudioFile(filename, index, event);
      return true;
    }
  }

  return false;
}

tmr10ms_t timeAutomaticPromptsSilence = 0;

void playModelEvent(uint8_t category, uint8_t index, uint8_t event)
{
  char filename[AUDIO_FILENAME_MAXLEN+1];
  // TRACE("playModelEvent(): cat: %u, idx: %u, evt:%u", (uint32_t)category, (uint32_t)index, (uint32_t)event);
  if (IS_SILENCE_PERIOD_ELAPSED() && isAudioFileReferenced((category << 24) + (index << 16) + event, filename)) {
    audioQueue.playFile(filename);
  }
}


void playModelName()
{
  char filename[AUDIO_FILENAME_MAXLEN+1];
  char * str = getModelAudioPath(filename);
  strcpy(str, "name.wav");
  audioQueue.playFile(filename);
}

#else   // defined(SDCARD)

#define isAudioFileReferenced(i, f) false

#endif  // defined(SDCARD)

const int16_t alawTable[256] = { -5504, -5248, -6016, -5760, -4480, -4224, -4992, -4736, -7552, -7296, -8064, -7808, -6528, -6272, -7040, -6784, -2752, -2624, -3008, -2880, -2240, -2112, -2496, -2368, -3776, -3648, -4032, -3904, -3264, -3136, -3520, -3392, -22016, -20992, -24064, -23040, -17920, -16896, -19968, -18944, -30208, -29184, -32256, -31232, -26112, -25088, -28160, -27136, -11008, -10496, -12032, -11520, -8960, -8448, -9984, -9472, -15104, -14592, -16128, -15616, -13056, -12544, -14080, -13568, -344, -328, -376, -360, -280, -264, -312, -296, -472, -456, -504, -488, -408, -392, -440, -424, -88, -72, -120, -104, -24, -8, -56, -40, -216, -200, -248, -232, -152, -136, -184, -168, -1376, -1312, -1504, -1440, -1120, -1056, -1248, -1184, -1888, -1824, -2016, -1952, -1632, -1568, -1760, -1696, -688, -656, -752, -720, -560, -528, -624, -592, -944, -912, -1008, -976, -816, -784, -880, -848, 5504, 5248, 6016, 5760, 4480, 4224, 4992, 4736, 7552, 7296, 8064, 7808, 6528, 6272, 7040, 6784, 2752, 2624, 3008, 2880, 2240, 2112, 2496, 2368, 3776, 3648, 4032, 3904, 3264, 3136, 3520, 3392, 22016, 20992, 24064, 23040, 17920, 16896, 19968, 18944, 30208, 29184, 32256, 31232, 26112, 25088, 28160, 27136, 11008, 10496, 12032, 11520, 8960, 8448, 9984, 9472, 15104, 14592, 16128, 15616, 13056, 12544, 14080, 13568, 344, 328, 376, 360, 280, 264, 312, 296, 472, 456, 504, 488, 408, 392, 440, 424, 88, 72, 120, 104, 24, 8, 56, 40, 216, 200, 248, 232, 152, 136, 184, 168, 1376, 1312, 1504, 1440, 1120, 1056, 1248, 1184, 1888, 1824, 2016, 1952, 1632, 1568, 1760, 1696, 688, 656, 752, 720, 560, 528, 624, 592, 944, 912, 1008, 976, 816, 784, 880, 848 };
const int16_t ulawTable[256] = { -32124, -31100, -30076, -29052, -28028, -27004, -25980, -24956, -23932, -22908, -21884, -20860, -19836, -18812, -17788, -16764, -15996, -15484, -14972, -14460, -13948, -13436, -12924, -12412, -11900, -11388, -10876, -10364, -9852, -9340, -8828, -8316, -7932, -7676, -7420, -7164, -6908, -6652, -6396, -6140, -5884, -5628, -5372, -5116, -4860, -4604, -4348, -4092, -3900, -3772, -3644, -3516, -3388, -3260, -3132, -3004, -2876, -2748, -2620, -2492, -2364, -2236, -2108, -1980, -1884, -1820, -1756, -1692, -1628, -1564, -1500, -1436, -1372, -1308, -1244, -1180, -1116, -1052, -988, -924, -876, -844, -812, -780, -748, -716, -684, -652, -620, -588, -556, -524, -492, -460, -428, -396, -372, -356, -340, -324, -308, -292, -276, -260, -244, -228, -212, -196, -180, -164, -148, -132, -120, -112, -104, -96, -88, -80, -72, -64, -56, -48, -40, -32, -24, -16, -8, 0, 32124, 31100, 30076, 29052, 28028, 27004, 25980, 24956, 23932, 22908, 21884, 20860, 19836, 18812, 17788, 16764, 15996, 15484, 14972, 14460, 13948, 13436, 12924, 12412, 11900, 11388, 10876, 10364, 9852, 9340, 8828, 8316, 7932, 7676, 7420, 7164, 6908, 6652, 6396, 6140, 5884, 5628, 5372, 5116, 4860, 4604, 4348, 4092, 3900, 3772, 3644, 3516, 3388, 3260, 3132, 3004, 2876, 2748, 2620, 2492, 2364, 2236, 2108, 1980, 1884, 1820, 1756, 1692, 1628, 1564, 1500, 1436, 1372, 1308, 1244, 1180, 1116, 1052, 988, 924, 876, 844, 812, 780, 748, 716, 684, 652, 620, 588, 556, 524, 492, 460, 428, 396, 372, 356, 340, 324, 308, 292, 276, 260, 244, 228, 212, 196, 180, 164, 148, 132, 120, 112, 104, 96, 88, 80, 72, 64, 56, 48, 40, 32, 24, 16, 8, 0 };

AudioQueue audioQueue;
AudioBuffer audioBuffers[AUDIO_BUFFER_COUNT] __DMA;

AudioQueue::AudioQueue()
{
  memset(this, 0, sizeof(AudioQueue));
  memset(audioBuffers, 0, sizeof(audioBuffers));
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
  TRACE("unexpectedShutdown = %d", unexpectedShutdown);
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

void mixSample(uint16_t * result, int sample, unsigned int fade)
{
#if defined(SIMU_AUDIO)
  *result = limit(0, *result + ((sample >> fade) ), 0xFFFF);
#else
  *result = limit(0, *result + ((sample >> fade) >> 4), 4095);
#endif
}

#if defined(SDCARD)

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
          for (uint8_t j=0; j<state.resampleRatio; j++) {
            mixSample(samples++, ((int16_t *)wavBuffer)[i], fade+2-volume);
          }
        }
      }
      else if (state.codec == CODEC_ID_PCM_ALAW) {
        for (uint32_t i=0; i<read; i++) {
          for (uint8_t j=0; j<state.resampleRatio; j++) {
            mixSample(samples++, alawTable[wavBuffer[i]], fade+2-volume);
          }
        }
      }
      else if (state.codec == CODEC_ID_PCM_MULAW) {
        for (uint32_t i=0; i<read; i++) {
          for (uint8_t j=0; j<state.resampleRatio; j++) {
            mixSample(samples++, ulawTable[wavBuffer[i]], fade+2-volume);
          }
        }
      }

      return samples - buffer->data;
    }
  }

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
      fragment.tone.reset = 0;
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
#if defined(SIMU_AUDIO)
      buffer->data[i] = 0x8000; /* silence */
#else
      buffer->data[i] = 0x8000 >> 4; /* silence */
#endif
    }

    // mix the priority context (only tones)
    result = priorityContext.mixBuffer(buffer, g_eeGeneral.beepVolume, fade);
    if (result > 0) {
      size = result;
      fade += 1;
    }

    // mix the normal context (tones and wavs)
    if (normalContext.fragment.type == FRAGMENT_TONE) {
      result = normalContext.tone.mixBuffer(buffer, g_eeGeneral.beepVolume, fade);
    }
    else if (normalContext.fragment.type == FRAGMENT_FILE) {
      result = normalContext.wav.mixBuffer(buffer, g_eeGeneral.wavVolume, fade);
      if (result < 0) {
        normalContext.wav.clear();
      }
    }
    else {
      result = 0;
    }
    if (result > 0) {
      size = max(size, result);
      fade += 1;
    }
    else {
      CoEnterMutexSection(audioMutex);
      if (ridx != widx) {
        normalContext.tone.setFragment(fragments[ridx]);
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
    if (isFunctionActive(FUNCTION_BACKGND_MUSIC) && !isFunctionActive(FUNCTION_BACKGND_MUSIC_PAUSE)) {
      result = backgroundContext.mixBuffer(buffer, g_eeGeneral.backgroundVolume, fade);
      if (result > 0) {
        size = max(size, result);
      }
    }

    // push the buffer if needed
    if (size > 0) {
      __disable_irq();
      // TRACE("pushing buffer %d\n", bufferWIdx);
      bufferWIdx = nextBufferIdx(bufferWIdx);
      buffer->size = size;
      buffer->state = dacQueue(buffer) ? AUDIO_BUFFER_PLAYING : AUDIO_BUFFER_FILLED;
      __enable_irq();
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
  if (normalContext.fragment.id == id || (isFunctionActive(FUNCTION_BACKGND_MUSIC) && backgroundContext.fragment.id == id))
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
#if defined(SIMU) && !defined(SIMU_AUDIO)
  return;
#endif

  CoEnterMutexSection(audioMutex);

  if (freq && freq < BEEP_MIN_FREQ) {
    freq = BEEP_MIN_FREQ;
  }

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
  TRACE("playFile(\"%s\", flags=%x, id=%d)", filename, flags, id);
  if (strlen(filename) > AUDIO_FILENAME_MAXLEN) {
    TRACE("file name too long! maximum length is %d characters", AUDIO_FILENAME_MAXLEN);
    return;
  }
  #if !defined(SIMU_AUDIO)
  return;
  #endif
#endif


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
}

void AudioQueue::stopPlay(uint8_t id)
{
#if defined(SIMU)
  TRACE("stopPlay(id=%d)", id);
#endif

#if defined(SIMU) && !defined(SIMU_AUDIO)
  return;
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
  stopAll();
  playTone(0, 0, 100, PLAY_NOW);        // insert a 100ms pause
}

#endif

void AudioQueue::stopAll()
{
  CoEnterMutexSection(audioMutex);
  widx = ridx;                      // clean the queue
  priorityContext.clear();
  normalContext.fragment.clear();
  varioContext.clear();
  backgroundContext.clear();
  CoLeaveMutexSection(audioMutex);
}

void AudioQueue::flush()
{
  CoEnterMutexSection(audioMutex);
  widx = ridx;                      // clean the queue
  varioContext.clear();
  backgroundContext.clear();
  CoLeaveMutexSection(audioMutex);
}

void audioPlay(unsigned int index, uint8_t id)
{
  if (g_eeGeneral.beepMode >= -1) {
    char filename[AUDIO_FILENAME_MAXLEN+1];
    if (isAudioFileReferenced(index, filename)) {
      audioQueue.playFile(filename, 0, id);
    }
  }
}

void audioEvent(unsigned int index, unsigned int freq)
{
  if (index == AU_NONE)
    return;

#if defined(HAPTIC)
  if (index >= AU_THROTTLE_ALERT) {
    haptic.event(index); //do this before audio to help sync timings
  }
#endif

  if (index <= AU_ERROR || (index >= AU_WARNING1 && index < AU_FRSKY_FIRST)) {
    if (g_eeGeneral.alarmsFlash) {
      flashCounter = FLASH_DURATION;
    }
  }

  if (g_eeGeneral.beepMode>0 || (g_eeGeneral.beepMode==0 && index>=AU_TRIM_MOVE) || (g_eeGeneral.beepMode>=-1 && index<=AU_ERROR)) {
#if defined(SDCARD)
    char filename[AUDIO_FILENAME_MAXLEN+1];
    if (index < AU_FRSKY_FIRST && isAudioFileReferenced(index, filename)) {
      audioQueue.playFile(filename);
    }
    else
#endif
    if (index < AU_FRSKY_FIRST || audioQueue.empty()) {
      switch (index) {
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
          audioQueue.playTone(freq, 40, 20, PLAY_NOW);
          break;
        // trim center
        case AU_TRIM_MIDDLE:
          audioQueue.playTone(freq, 80, 20, PLAY_NOW);
          break;
        // trim center
        case AU_TRIM_END:
          audioQueue.playTone(freq, 80, 20, PLAY_NOW);
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
#if defined(PCBTARANIS) || defined(PCBFLAMENCO) || defined(PCBHORUS)
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
        case AU_A1_ORANGE:
        case AU_A2_ORANGE:
        case AU_A3_ORANGE:
        case AU_A4_ORANGE:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+600, 200, 20, PLAY_NOW);
          break;
        case AU_A1_RED:
        case AU_A2_RED:
        case AU_A3_RED:
        case AU_A4_RED:
          audioQueue.playTone(BEEP_DEFAULT_FREQ+600, 200, 20, PLAY_REPEAT(1)|PLAY_NOW);
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
  char filename[AUDIO_FILENAME_MAXLEN+1];
  char * str = getSystemAudioPath(filename);
  strcpy(str, "0000" SOUNDS_EXT);
  for (int8_t i=3; i>=0; i--) {
    str[i] = '0' + (prompt%10);
    prompt /= 10;
  }
  audioQueue.playFile(filename, 0, id);
#endif
}
