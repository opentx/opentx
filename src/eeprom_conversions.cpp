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

PACK(typedef struct {
  uint8_t  destCh;
  uint16_t phases;
  uint8_t curveMode:1;       // O=curve, 1=differential
  uint8_t noExpo:1;
  int8_t  carryTrim:3;
  uint8_t mltpx:2;           // multiplex method: 0 means +=, 1 means *=, 2 means :=
  uint8_t spare:1;
  int8_t  weight;
  int8_t  swtch;
  int8_t  curveParam;
  uint8_t mixWarn;         // mixer warning
  uint8_t delayUp;
  uint8_t delayDown;
  uint8_t speedUp;
  uint8_t speedDown;
  uint8_t srcRaw;
  int8_t  sOffset;
  char    name[LEN_EXPOMIX_NAME];
}) MixData_v212;

PACK(typedef struct {
  int8_t    mode;            // timer trigger source -> off, abs, stk, stk%, sw/!sw, !m_sw/!m_sw
  uint16_t  val;
}) TimerData_v212;

PACK(typedef struct {
  uint16_t persistent:1;
  uint16_t value:15;
}) TimerDataExtra_v212;

PACK(typedef struct {
  FrSkyChannelData channels[2];
  FrSkyRSSIAlarm rssiAlarms[2];
  uint8_t usrProto; // Protocol in FrSky user data, 0=None, 1=FrSky hub, 2=WS HowHigh
  uint8_t voltsSource;
  uint8_t blades;   // How many blades for RPMs, 0=2 blades, 1=3 blades
  uint8_t currentSource;
  FrSkyBarData bars[4];
  uint8_t lines[4*2*2];
  uint8_t varioSource;
  uint8_t varioCenterMin;
  uint8_t varioCenterMax;
  uint8_t spare[4];
}) FrSkyData_v212;

PACK(typedef struct {
  char      name[LEN_MODEL_NAME]; // must be first for eeLoadModelName
  TimerData_v212 timers[MAX_TIMERS];
  uint8_t   protocol:3;
  uint8_t   thrTrim:1;            // Enable Throttle Trim
  int8_t    ppmNCH:4;
  uint8_t   trimInc:3;            // Trim Increments
  uint8_t   disableThrottleWarning:1;
  uint8_t   pulsePol:1;
  uint8_t   extendedLimits:1;
  uint8_t   extendedTrims:1;
  uint8_t   spare1:1;
  int8_t    ppmDelay;
  BeepANACenter   beepANACenter;        // 1<<0->A1.. 1<<6->A7
  MixData_v212   mixData[MAX_MIXERS];
  LimitData limitData[NUM_CHNOUT];
  ExpoData  expoData[MAX_EXPOS];

  CURVTYPE  curves[MAX_CURVES];
  int8_t    points[NUM_POINTS];

  CustomSwData customSw[NUM_CSW];
  CustomFnData   funcSw[NUM_CFN];
  SwashRingData swashR;
  PhaseData phaseData[MAX_PHASES];

  int8_t    ppmFrameLength;       // 0=22.5ms  (10ms-30ms) 0.5ms increments
  uint8_t   thrTraceSrc;
  uint8_t   modelId;

  uint8_t   switchWarningStates;

  FrSkyData_v212 frsky;

  TimerDataExtra_v212 timersXtra[MAX_TIMERS];

}) ModelData_v212;

bool eeConvert()
{
  if (g_eeGeneral.version == 212) {
    g_eeGeneral.optrexDisplay = 0;
    g_eeGeneral.backlightBright = 0;
    g_eeGeneral.contrast = 25;
    ALERT(STR_EEPROMWARN, PSTR("EEprom Data v212"), AU_BAD_EEPROM);
  }
  else {
    return false;
  }

  MESSAGE(STR_EEPROMWARN, PSTR("EEPROM Converting"), NULL, AU_EEPROM_FORMATTING); // TODO translations

  if (g_eeGeneral.version == 212) {
    memmove(((uint8_t*)&g_eeGeneral)+3, ((uint8_t*)&g_eeGeneral)+1, 81);
    g_eeGeneral.variant = 0;
    g_eeGeneral.version = 213;

    Eeprom32_source_address = (uint8_t *)&g_eeGeneral ;               // Get data from here
    Eeprom32_data_size = sizeof(g_eeGeneral) ;                        // This much
    Eeprom32_file_index = 0 ;                                         // This file system entry
    Eeprom32_process_state = E32_BLANKCHECK ;
    eeWaitFinished();

    for (uint8_t id=0; id<MAX_MODELS; id++) {
      uint16_t size = File_system[id+1].size;
      if (size >= 256) {
        ModelData_v212 oldModel;
        memset(&oldModel, 0, sizeof(oldModel));
        if (size > sizeof(oldModel)) {
          size = sizeof(oldModel) ;
        }
        read32_eeprom_data((File_system[id+1].block_no << 12) + sizeof(struct t_eeprom_header), (uint8_t *)&oldModel, size);

        memcpy(g_model.name, oldModel.name, 10);
        for (uint8_t i=0; i<2; i++) {
          g_model.timers[i].mode = oldModel.timers[i].mode;
          g_model.timers[i].start = oldModel.timers[i].val;
          g_model.timers[i].persistent = oldModel.timersXtra[i].persistent;
          g_model.timers[i].value = oldModel.timersXtra[i].value;
        }
        g_model.protocol = oldModel.protocol;
        g_model.thrTrim = oldModel.thrTrim;
        g_model.ppmNCH = oldModel.ppmNCH;
        g_model.trimInc = oldModel.trimInc;
        g_model.disableThrottleWarning = oldModel.disableThrottleWarning;
        g_model.pulsePol = oldModel.pulsePol;
        g_model.extendedLimits = oldModel.extendedLimits;
        g_model.extendedTrims = oldModel.extendedTrims;
        g_model.ppmDelay = oldModel.ppmDelay;
        g_model.beepANACenter = oldModel.beepANACenter;
        for (uint8_t i=0; i<64; i++) {
          g_model.mixData[i].destCh = oldModel.mixData[i].destCh;
          g_model.mixData[i].phases = oldModel.mixData[i].phases;
          g_model.mixData[i].curveMode = oldModel.mixData[i].curveMode;
          g_model.mixData[i].noExpo = oldModel.mixData[i].noExpo;
          g_model.mixData[i].carryTrim = oldModel.mixData[i].carryTrim;
          g_model.mixData[i].mltpx = oldModel.mixData[i].mltpx;
          g_model.mixData[i].weight = oldModel.mixData[i].weight;
          g_model.mixData[i].swtch = oldModel.mixData[i].swtch;
          g_model.mixData[i].curveParam = oldModel.mixData[i].curveParam;
          g_model.mixData[i].mixWarn = oldModel.mixData[i].mixWarn;
          g_model.mixData[i].delayUp = oldModel.mixData[i].delayUp;
          g_model.mixData[i].delayDown = oldModel.mixData[i].delayDown;
          g_model.mixData[i].speedUp = oldModel.mixData[i].speedUp;
          g_model.mixData[i].speedDown = oldModel.mixData[i].speedDown;
          g_model.mixData[i].srcRaw = oldModel.mixData[i].srcRaw;
          g_model.mixData[i].offset = oldModel.mixData[i].sOffset;
          memcpy(g_model.mixData[i].name, oldModel.mixData[i].name, 6);
        }
        for (uint8_t i=0; i<32; i++) {
          g_model.limitData[i] = oldModel.limitData[i];
        }
        for (uint8_t i=0; i<32; i++) {
          g_model.expoData[i] = oldModel.expoData[i];
        }
        for (uint8_t i=0; i<16; i++) {
          g_model.curves[i] = oldModel.curves[i];
        }
        for (uint16_t i=0; i<512; i++) {
          g_model.points[i] = oldModel.points[i];
        }
        for (uint8_t i=0; i<32; i++) {
          g_model.customSw[i] = oldModel.customSw[i];
        }
        for (uint8_t i=0; i<32; i++) {
          g_model.funcSw[i] = oldModel.funcSw[i];
        }
        g_model.swashR = oldModel.swashR;
        for (uint8_t i=0; i<9; i++) {
          g_model.phaseData[i].swtch = oldModel.phaseData[i].swtch;
          g_model.phaseData[i].fadeIn = oldModel.phaseData[i].fadeIn;
          g_model.phaseData[i].fadeOut = oldModel.phaseData[i].fadeOut;
          memcpy(g_model.phaseData[i].name, oldModel.phaseData[i].name, 6);
          memcpy(g_model.phaseData[i].trim, oldModel.phaseData[i].trim, 4*sizeof(int16_t));
          memcpy(g_model.phaseData[i].rotaryEncoders, oldModel.phaseData[i].rotaryEncoders, 1*sizeof(int16_t));
        }
        g_model.ppmFrameLength = oldModel.ppmFrameLength;
        g_model.thrTraceSrc = oldModel.thrTraceSrc;
        g_model.modelId = oldModel.modelId;
        g_model.switchWarningStates = oldModel.switchWarningStates;
        g_model.frsky.usrProto = oldModel.frsky.usrProto;
        g_model.frsky.voltsSource = oldModel.frsky.voltsSource;
        g_model.frsky.blades = oldModel.frsky.blades;
        g_model.frsky.currentSource = oldModel.frsky.currentSource;
        g_model.frsky.varioSource = oldModel.frsky.varioSource;
        g_model.frsky.varioCenterMax = oldModel.frsky.varioCenterMax;
        g_model.frsky.varioCenterMin = oldModel.frsky.varioCenterMin;
        for (uint8_t i=0; i<2; i++) {
          g_model.frsky.channels[i] = oldModel.frsky.channels[i];
          g_model.frsky.rssiAlarms[i] = oldModel.frsky.rssiAlarms[i];
        }

        Eeprom32_source_address = (uint8_t *)&g_model ;           // Get data from here
        Eeprom32_data_size = sizeof(g_model) ;                    // This much
        Eeprom32_file_index = g_eeGeneral.currModel + 1 ;         // This file system entry
        Eeprom32_process_state = E32_BLANKCHECK ;
        eeWaitFinished();
      }
    }
  }

  return true;
}
