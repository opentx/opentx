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
 * PCBHORUS: 64 telemetry sensors instead of 32
 * ALL: ReceiverData array added
 * ALL: registrationId added
 * ALL: failsafeChannels moved from ModuleData to ModelData
 * ALL: ModuleData / TrainerModuleData modified
 *
 */

typedef ModelData ModelData_v219;

void convertModelData_218_to_219(ModelData &model)
{
  static_assert(sizeof(ModelData_v218) <= sizeof(ModelData));

  ModelData_v218 oldModel;
  memcpy(&oldModel, &model, sizeof(oldModel));
  ModelData_v219 & newModel = (ModelData_v219 &) model;

  memset(&newModel.rssiAlarms + sizeof(RssiAlarmData), 0, sizeof(ModelData_v219) - offsetof(ModelData_v219, rssiAlarms) - sizeof(RssiAlarmData));

  char name[LEN_MODEL_NAME+1];
  zchar2str(name, oldModel.header.name, LEN_MODEL_NAME);
  TRACE("Model %s conversion from v218 to v219", name);

  for (int i=0; i<NUM_MODULES; i++) {
    memcpy(&newModel.moduleData[i], &oldModel.moduleData[i], 4);
    memcpy(((uint8_t *)&newModel.moduleData[i]) + 4, ((uint8_t *)&oldModel.moduleData[i]) + 64 + 4, 2);
    if (newModel.moduleData[i].type >= MODULE_TYPE_XJT2)
      newModel.moduleData[i].type += 1;
    if (newModel.moduleData[i].type >= MODULE_TYPE_R9M2)
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
  memcpy(newModel.scriptsData, oldModel.scriptsData,
         sizeof(newModel.scriptsData) +
         sizeof(newModel.inputNames) +
         sizeof(newModel.potsWarnEnabled) +
         sizeof(newModel.potsWarnPosition) +
         sizeof(oldModel.telemetrySensors));
#else
  memcpy(newModel.inputNames, oldModel.inputNames,
         sizeof(newModel.inputNames) +
         sizeof(newModel.potsWarnEnabled) +
         sizeof(newModel.potsWarnPosition) +
         sizeof(oldModel.telemetrySensors));
#endif

#if defined(PCBX9E)
  newModel.toplcdTimer = oldModel.toplcdTimer;
#endif

#if defined(PCBHORUS)
  memcpy(newModel.screenData, oldModel.screenData,
          sizeof(newModel.screenData) +
          sizeof(newModel.topbarData))
#endif
}
