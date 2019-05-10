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
  }

  for (uint8_t i=0; i<MAX_OUTPUT_CHANNELS_218; i++) {
    memmove(&newModel.limitData[i], &oldModel.limitData[i], sizeof(LimitData));
  }

  for (uint8_t i=0; i<MAX_EXPOS_218; i++) {
    memmove(&newModel.expoData[i], &oldModel.expoData[i], sizeof(ExpoData_v218));
    newModel.expoData[i].offset = oldModel.expoData[i].offset;
    newModel.expoData[i].curve = oldModel.expoData[i].curve;
  }

  for (uint8_t i=0; i<MAX_CURVES_218; i++) {
    memmove(&newModel.curves[i], &oldModel.curves[i], sizeof(CurveData_v218));
  }

  for (uint32_t i=0; i<MAX_CURVE_POINTS_218; i++) {
    newModel.points[i] = oldModel.points[i];
  }

  for (uint8_t i=0; i<MAX_LOGICAL_SWITCHES_218; i++) {
    memmove(&newModel.logicalSw[i], &oldModel.logicalSw[i], sizeof(LogicalSwitchData_v218));
  }

  for (uint8_t i=0; i<MAX_SPECIAL_FUNCTIONS_218; i++) {
    memmove(&newModel.customFn[i], &oldModel.customFn[i], sizeof(CustomFunctionData_v218));
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

#if defined(PCBX9)
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
  memcpy(newModel.screenData, oldModel.screenData,
          sizeof(newModel.screenData) +
          sizeof(newModel.topbarData))
#endif
}
