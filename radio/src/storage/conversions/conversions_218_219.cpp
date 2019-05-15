/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"
#include "datastructs_218.h"

/*
 * 60 (Horus / X9) / 40 (others) telemetry sensors instead of 32
 * ALL: ReceiverData array added
 * ALL: registrationId added
 * ALL: failsafeChannels moved from ModuleData to ModelData
 * ALL: ModuleData / TrainerModuleData modified
 * PCBX9 : 6 chars for expos / mixes names instead of 8
 */

typedef ModelData ModelData_v219;

int convertSource_218_to_219(int source)
{
  // on X7: 2 additional switches

#if defined(PCBX7)
  if (source >= MIXSRC_SI)
    source += 2;
#endif

#if defined(PCBHORUS)
  if (source >= MIXSRC_GMBL)
    source += 2;
#endif

  return source;
}

int convertSwitch_218_to_219(int swtch)
{
  // on X7: 2 additional switches

#if defined(PCBX7) || defined(PCBHORUS)
  if (swtch < 0)
    return -convertSwitch_218_to_219(-swtch);
#endif

#if defined(PCBX7)
  if (swtch >= SWSRC_SI0)
    swtch += 2 * 3;
#endif

#if defined(PCBHORUS)
  if (swtch >= SWSRC_GMBL0)
    swtch += 2 * 3;
  if (swtch >= SWSRC_FIRST_MULTIPOS_SWITCH + 3 * XPOTS_MULTIPOS_COUNT)
    swtch += 2 * XPOTS_MULTIPOS_COUNT;
#endif

  return swtch;
}

void convertModelData_218_to_219(ModelData &model)
{
  static_assert(sizeof(ModelData_v218) <= sizeof(ModelData), "ModelData size has been reduced");

  ModelData_v218 oldModel;
  memcpy(&oldModel, &model, sizeof(oldModel));
  ModelData_v219 & newModel = (ModelData_v219 &) model;

  memclear(newModel.mixData, sizeof(ModelData_v219) - offsetof(ModelData_v219, mixData));

  char name[LEN_MODEL_NAME+1];
  zchar2str(name, oldModel.header.name, LEN_MODEL_NAME);
  TRACE("Model %s conversion from v218 to v219", name);

  for (uint8_t i=0; i<MAX_MIXERS_218; i++) {
    memmove(&newModel.mixData[i], &oldModel.mixData[i], sizeof(MixData_v218));
    newModel.mixData[i].srcRaw = convertSource_218_to_219(newModel.mixData[i].srcRaw); // from newModel to avoid overwrite
    newModel.mixData[i].swtch = convertSwitch_218_to_219(newModel.mixData[i].swtch); // from newModel to avoid overwrite
  }

  for (uint8_t i=0; i<MAX_OUTPUT_CHANNELS_218; i++) {
    memmove(&newModel.limitData[i], &oldModel.limitData[i], sizeof(LimitData));
  }

  for (uint8_t i=0; i<MAX_EXPOS_218; i++) {
    memmove(&newModel.expoData[i], &oldModel.expoData[i], sizeof(ExpoData_v218));
    newModel.expoData[i].srcRaw = convertSource_218_to_219(newModel.expoData[i].srcRaw); // from newModel to avoid overwrite
    newModel.expoData[i].swtch = convertSwitch_218_to_219(newModel.expoData[i].swtch); // from newModel to avoid overwrite
  }

  for (uint8_t i=0; i<MAX_CURVES_218; i++) {
    memmove(&newModel.curves[i], &oldModel.curves[i], sizeof(CurveData_v218));
  }

  for (uint32_t i=0; i<MAX_CURVE_POINTS_218; i++) {
    newModel.points[i] = oldModel.points[i];
  }

  for (uint8_t i=0; i<MAX_LOGICAL_SWITCHES_218; i++) {
    memmove(&newModel.logicalSw[i], &oldModel.logicalSw[i], sizeof(LogicalSwitchData_v218));
    LogicalSwitchData & sw = newModel.logicalSw[i];
    uint8_t cstate = lswFamily(sw.func);
    if (cstate == LS_FAMILY_OFS || cstate == LS_FAMILY_COMP || cstate == LS_FAMILY_DIFF) {
      LogicalSwitchData & sw = newModel.logicalSw[i];
      sw.v1 = convertSource_218_to_219((uint8_t)sw.v1);
      if (cstate == LS_FAMILY_COMP) {
        sw.v2 = convertSource_218_to_219((uint8_t)sw.v2);
      }
    }
    else if (cstate == LS_FAMILY_BOOL || cstate == LS_FAMILY_STICKY) {
      sw.v1 = convertSwitch_218_to_219(sw.v1);
      sw.v2 = convertSwitch_218_to_219(sw.v2);
    }
    else if (cstate == LS_FAMILY_EDGE) {
      sw.v1 = convertSwitch_218_to_219(sw.v1);
    }
  }

  for (uint8_t i=0; i<MAX_SPECIAL_FUNCTIONS_218; i++) {
    memmove(&newModel.customFn[i], &oldModel.customFn[i], sizeof(CustomFunctionData_v218));
    CustomFunctionData & cf = newModel.customFn[i];
    cf.swtch = convertSwitch_218_to_219(cf.swtch);
    if (cf.func == FUNC_PLAY_VALUE || cf.func == FUNC_VOLUME || (IS_ADJUST_GV_FUNC(cf.func) && cf.all.mode == FUNC_ADJUST_GVAR_SOURCE)) {
      cf.all.val = convertSource_218_to_219(cf.all.val);
    }
  }

  newModel.swashR = oldModel.swashR;

  for (uint8_t i=0; i<MAX_FLIGHT_MODES_218; i++) {
    memmove(&newModel.flightModeData[i], &oldModel.flightModeData[i], sizeof(FlightModeData_v218));
  }

  newModel.thrTraceSrc = oldModel.thrTraceSrc;
  newModel.switchWarningState = oldModel.switchWarningState;
#if !defined(COLORLCD)
  newModel.switchWarningEnable = oldModel.switchWarningEnable;
#endif

  for (uint8_t i=0; i<MAX_GVARS_218; i++) {
    memmove(&newModel.gvars[i], &oldModel.gvars[i], sizeof(GVarData_v218));
  }

  newModel.varioData.source = oldModel.frsky.varioSource;
  newModel.varioData.centerSilent = oldModel.frsky.varioCenterSilent;
  newModel.varioData.centerMax = oldModel.frsky.varioCenterMax;
  newModel.varioData.centerMin = oldModel.frsky.varioCenterMin;
  newModel.varioData.min = oldModel.frsky.varioMin;
  newModel.varioData.max = oldModel.frsky.varioMax;

#if defined(PCBX9D) || defined(PCBX9DP) || defined(PCBX9E)
  newModel.voltsSource = oldModel.frsky.voltsSource;
  newModel.altitudeSource = oldModel.frsky.altitudeSource;
#endif

  newModel.rssiAlarms = oldModel.rssiAlarms;
  newModel.potsWarnMode = oldModel.potsWarnMode;

  for (int i=0; i<NUM_MODULES; i++) {
    memcpy(&newModel.moduleData[i], &oldModel.moduleData[i], 4);
    memcpy(((uint8_t *)&newModel.moduleData[i]) + 4, ((uint8_t *)&oldModel.moduleData[i]) + 64 + 4, 2);
    if (newModel.moduleData[i].type >= MODULE_TYPE_ACCESS_ISRM)
      newModel.moduleData[i].type += 1;
    if (newModel.moduleData[i].type >= MODULE_TYPE_ACCESS_R9M)
      newModel.moduleData[i].type += 4;
  }

  for (uint8_t module=0; module<2; module++) {
    if (oldModel.moduleData[module].failsafeMode == FAILSAFE_CUSTOM) {
      memcpy(newModel.failsafeChannels, oldModel.moduleData[module].failsafeChannels, sizeof(newModel.failsafeChannels));
    }
  }

#if !defined(PCBSKY9X)
  newModel.trainerData.mode = oldModel.trainerMode;
#endif
  newModel.trainerData.channelsStart = oldModel.moduleData[NUM_MODULES].channelsStart;
  newModel.trainerData.channelsCount = oldModel.moduleData[NUM_MODULES].channelsCount;
  newModel.trainerData.frameLength = oldModel.moduleData[NUM_MODULES].ppm.frameLength;
  newModel.trainerData.delay = oldModel.moduleData[NUM_MODULES].ppm.delay;
  newModel.trainerData.pulsePol = oldModel.moduleData[NUM_MODULES].ppm.pulsePol;

#if defined(PCBHORUS) || defined(PCBTARANIS)
  memmove(newModel.scriptsData, oldModel.scriptsData,
         sizeof(newModel.scriptsData) +
         sizeof(newModel.inputNames) +
         sizeof(newModel.potsWarnEnabled) +
         sizeof(newModel.potsWarnPosition));
#else
  memmove(newModel.inputNames, oldModel.inputNames,
         sizeof(newModel.inputNames) +
         sizeof(newModel.potsWarnEnabled) +
         sizeof(newModel.potsWarnPosition));
#endif

  for (uint8_t i=0; i<MAX_TELEMETRY_SENSORS_218; i++) {
    memmove(&newModel.telemetrySensors[i], &oldModel.telemetrySensors[i], 7);
    newModel.telemetrySensors[i].subId = oldModel.telemetrySensors[i].subId;
    newModel.telemetrySensors[i].type = oldModel.telemetrySensors[i].type;
    newModel.telemetrySensors[i].unit = oldModel.telemetrySensors[i].unit;
    if (newModel.telemetrySensors[i].unit >= UNIT_MILLILITERS_PER_MINUTE)
      newModel.telemetrySensors[i].unit += 11;
    newModel.telemetrySensors[i].prec = oldModel.telemetrySensors[i].prec;
    newModel.telemetrySensors[i].autoOffset = oldModel.telemetrySensors[i].autoOffset;
    newModel.telemetrySensors[i].filter = oldModel.telemetrySensors[i].filter;
    newModel.telemetrySensors[i].logs = oldModel.telemetrySensors[i].logs;
    newModel.telemetrySensors[i].persistent = oldModel.telemetrySensors[i].persistent;
    newModel.telemetrySensors[i].onlyPositive = oldModel.telemetrySensors[i].onlyPositive;
    memmove(((uint8_t *)&newModel.telemetrySensors[i]) + 10, ((uint8_t *)&oldModel.telemetrySensors[i]) + 9, 4);
  }

#if defined(PCBX9E)
  newModel.toplcdTimer = oldModel.toplcdTimer;
#endif

#if defined(PCBHORUS)
//  memcpy(newModel.screenData, oldModel.screenData,
//          sizeof(newModel.screenData) +
//          sizeof(newModel.topbarData))
#else
  newModel.screensType = oldModel.frsky.screensType;
  memmove(&newModel.screens, &oldModel.frsky.screens, sizeof(newModel.screens));
#endif

#if defined(PCBX7)
  for (int i=0; i<MAX_TELEMETRY_SCREENS; i++) {
    uint8_t screenType = (newModel.screensType >> (2*i)) & 0x03;
    if (screenType == TELEMETRY_SCREEN_TYPE_VALUES) {
      for (int j = 0; j < 4; j++) {
        for (int k = 0; k < NUM_LINE_ITEMS; k++) {
          newModel.screens[i].lines[j].sources[k] = convertSource_218_to_219(newModel.screens[i].lines[j].sources[k]);
        }
      }
    }
    else if (screenType == TELEMETRY_SCREEN_TYPE_GAUGES) {
      for (int j = 0; j < 4; j++) {
        newModel.screens[i].bars[j].source = convertSource_218_to_219(newModel.screens[i].bars[j].source);
      }
    }
  }
#endif
}

void convertRadioData_218_to_219(RadioData & settings)
{
  RadioData_v218 settings_v218 = (RadioData_v218 &)settings;

  settings.version = 219;

#if defined(PCBHORUS)
  memcpy(&settings.chkSum, &settings_v218.chkSum, offsetof(RadioData, serial2Mode) - offsetof(RadioData, chkSum));
  memcpy(&settings.calib[NUM_STICKS + 5], &settings_v218.calib[NUM_STICKS + 3], sizeof(CalibData) * (STORAGE_NUM_SLIDERS + STORAGE_NUM_MOUSE_ANALOGS));
  memclear(&settings.calib[NUM_STICKS + 3], sizeof(CalibData) * 2);

  settings.serial2Mode = settings_v218.serial2Mode;
  settings.switchConfig = settings_v218.switchConfig;
  settings.potsConfig = settings_v218.potsConfig;
  settings.slidersConfig = settings_v218.slidersConfig;

  memcpy(&settings.switchNames[0], &settings_v218.switchNames[0], 8 * LEN_SWITCH_NAME);
  memclear(&settings.switchNames[8], 2 * LEN_SWITCH_NAME);

  memcpy(&settings.anaNames[0], &settings_v218.anaNames[0], (NUM_STICKS + 3) * LEN_ANA_NAME);
  memclear(&settings.anaNames[NUM_STICKS + 3], 2 * LEN_SWITCH_NAME);
  memcpy(&settings.anaNames[NUM_STICKS + 5], &settings_v218.anaNames[NUM_STICKS + 3], STORAGE_NUM_SLIDERS * LEN_ANA_NAME);

  memcpy(&settings.currModelFilename[0], &settings_v218.currModelFilename[0], sizeof(RadioData_v218) - offsetof(RadioData_v218, currModelFilename[0]));
#endif
}
