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

#ifndef _MODULES_HELPERS_H_
#define _MODULES_HELPERS_H_

#include "bitfield.h"
#include "definitions.h"
#include "opentx_helpers.h"
#include "telemetry/telemetry.h"
#if defined(MULTIMODULE)
#include "telemetry/multi.h"
#endif

#define CROSSFIRE_CHANNELS_COUNT        16

#if defined(MULTIMODULE)
inline bool isModuleMultimodule(uint8_t idx)
{
  return g_model.moduleData[idx].type == MODULE_TYPE_MULTIMODULE;
}

inline bool isModuleMultimoduleDSM2(uint8_t idx)
{
  return isModuleMultimodule(idx) && g_model.moduleData[idx].getMultiProtocol(true) == MODULE_SUBTYPE_MULTI_DSM2;
}
#else
inline bool isModuleMultimodule(uint8_t)
{
  return false;
}

inline bool isModuleMultimoduleDSM2(uint8_t)
{
  return false;
}
#endif

inline bool isModuleTypeXJT(uint8_t type)
{
  return type == MODULE_TYPE_XJT_PXX1 || type == MODULE_TYPE_XJT_LITE_PXX2;
}

inline bool isModuleXJT(uint8_t idx)
{
  return isModuleTypeXJT(g_model.moduleData[idx].type);
}

inline bool isModuleXJTD8(uint8_t idx)
{
  return isModuleXJT(idx) && g_model.moduleData[idx].subType == MODULE_SUBTYPE_PXX1_ACCST_D8;
}

inline bool isModuleXJTLR12(uint8_t idx)
{
  return isModuleXJT(idx) && g_model.moduleData[idx].subType == MODULE_SUBTYPE_PXX1_ACCST_LR12;
}

inline bool isModuleXJTD16(uint8_t idx)
{
  return isModuleXJT(idx) && g_model.moduleData[idx].subType == MODULE_SUBTYPE_PXX1_ACCST_D16;
}

inline bool isModuleISRM(uint8_t idx)
{
  return g_model.moduleData[idx].type == MODULE_TYPE_ISRM_PXX2;
}

inline bool isModuleISRMD16(uint8_t idx)
{
  return g_model.moduleData[idx].type == MODULE_TYPE_ISRM_PXX2 && g_model.moduleData[idx].subType == MODULE_SUBTYPE_ISRM_PXX2_ACCST_D16;
}

inline bool isModuleD16(uint8_t idx)
{
  return isModuleXJTD16(idx) || isModuleISRMD16(idx);
}

inline bool isModuleISRMAccess(uint8_t idx)
{
  return g_model.moduleData[idx].type == MODULE_TYPE_ISRM_PXX2 && g_model.moduleData[idx].subType == MODULE_SUBTYPE_ISRM_PXX2_ACCESS;
}

#if defined(CROSSFIRE)
inline bool isModuleCrossfire(uint8_t idx)
{
  return idx == EXTERNAL_MODULE && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_CROSSFIRE;
}
#else
inline bool isModuleCrossfire(uint8_t idx)
{
  return false;
}
#endif

#if defined(PCBFLYSKY)
inline bool isModuleFlysky(uint8_t idx)
{
  return g_model.moduleData[idx].type == MODULE_TYPE_FLYSKY;
}
#else
inline bool isModuleFlysky(uint8_t idx)
{
  return idx == EXTERNAL_MODULE && g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_FLYSKY;
}
#endif

#if defined(PCBSKY9X)
inline bool isExtraModule(uint8_t idx)
{
  return idx == EXTRA_MODULE;
}
#else
inline bool isExtraModule(uint8_t)
{
  return false;
}
#endif

inline bool isModuleTypePPM(uint8_t type)
{
  return type == MODULE_TYPE_PPM;
}

inline bool isModulePPM(uint8_t idx)
{
  return isModuleTypePPM(g_model.moduleData[idx].type);
}

inline bool isModuleTypeR9MNonAccess(uint8_t type)
{
  return type == MODULE_TYPE_R9M_PXX1 || type == MODULE_TYPE_R9M_LITE_PXX1 || type == MODULE_TYPE_R9M_LITE_PRO_PXX1;
}

inline bool isModuleR9MNonAccess(uint8_t idx)
{
  return isModuleTypeR9MNonAccess(g_model.moduleData[idx].type);
}

inline bool isModuleTypeR9MAccess(uint8_t type)
{
  return type == MODULE_TYPE_R9M_PXX2 || type == MODULE_TYPE_R9M_LITE_PXX2 || type == MODULE_TYPE_R9M_LITE_PRO_PXX2;
}

inline bool isModuleR9MAccess(uint8_t idx)
{
  return isModuleTypeR9MAccess(g_model.moduleData[idx].type);
}

inline bool isModuleTypeR9M(uint8_t type)
{
  return isModuleTypeR9MNonAccess(type) || isModuleTypeR9MAccess(type);
}

inline bool isModuleR9M(uint8_t idx)
{
  return isModuleTypeR9M(g_model.moduleData[idx].type);
}

inline bool isModuleTypeR9MLiteNonPro(uint8_t type)
{
  return type == MODULE_TYPE_R9M_LITE_PXX1 || type == MODULE_TYPE_R9M_LITE_PXX2;
}

inline bool isModuleR9MLiteNonPro(uint8_t idx)
{
  return isModuleTypeR9MLiteNonPro(g_model.moduleData[idx].type);
}

inline bool isModuleTypeR9MLitePro(uint8_t type)
{
  return type == MODULE_TYPE_R9M_LITE_PRO_PXX1 || type == MODULE_TYPE_R9M_LITE_PRO_PXX2;
}

inline bool isModuleTypeR9MLite(uint8_t type)
{
  return isModuleTypeR9MLiteNonPro(type) || isModuleTypeR9MLitePro(type);
}

inline bool isModuleR9MLite(uint8_t idx)
{
  return isModuleTypeR9MLite(g_model.moduleData[idx].type);
}

inline bool isModuleR9M_FCC(uint8_t idx)
{
  return isModuleR9MNonAccess(idx) && g_model.moduleData[idx].subType == MODULE_SUBTYPE_R9M_FCC;
}

inline bool isModuleTypeLite(uint8_t type)
{
  return isModuleTypeR9MLite(type) || type == MODULE_TYPE_XJT_LITE_PXX2;
}

inline bool isModuleR9M_LBT(uint8_t idx)
{
  return isModuleR9MNonAccess(idx) && g_model.moduleData[idx].subType == MODULE_SUBTYPE_R9M_EU;
}

inline bool isModuleR9M_FCC_VARIANT(uint8_t idx)
{
  return isModuleR9MNonAccess(idx) && g_model.moduleData[idx].subType != MODULE_SUBTYPE_R9M_EU;
}

inline bool isModuleR9M_EUPLUS(uint8_t idx)
{
  return isModuleR9MNonAccess(idx) && g_model.moduleData[idx].subType == MODULE_SUBTYPE_R9M_EUPLUS;
}

inline bool isModuleR9M_AU_PLUS(uint8_t idx)
{
  return isModuleR9MNonAccess(idx) && g_model.moduleData[idx].subType == MODULE_SUBTYPE_R9M_AUPLUS;
}

inline bool isModuleTypePXX1(uint8_t type)
{
  return isModuleTypeXJT(type) || isModuleTypeR9MNonAccess(type);
}

inline bool isModulePXX1(uint8_t idx)
{
  return isModuleTypePXX1(g_model.moduleData[idx].type);
}

inline bool isModulePXX2(uint8_t idx)
{
  return isModuleISRM(idx) || isModuleR9MAccess(idx);
}

inline bool isModuleRFAccess(uint8_t idx)
{
  if (isModuleISRM(idx)) {
    return g_model.moduleData[idx].subType == MODULE_SUBTYPE_ISRM_PXX2_ACCESS;
  }
  else if (isModuleR9MAccess(idx)) {
    return true;
  }
  else {
    return false;
  }
}

inline bool isModuleDSM2(uint8_t moduleIdx)
{
  return g_model.moduleData[moduleIdx].type == MODULE_TYPE_DSM2;
}

inline bool isModuleSBUS(uint8_t moduleIdx)
{
  return g_model.moduleData[moduleIdx].type == MODULE_TYPE_SBUS;
}

// order is the same as in enum Protocols in myeeprom.h (none, ppm, pxx, pxx2, dsm, crossfire, multi, r9m, r9m2, sbus)
static const int8_t maxChannelsModules_M8[] = { 0, 8, 8, 16, -2, 8, 4, 8, 16, 8}; // relative to 8!
static const int8_t maxChannelsXJT_M8[] = { 0, 8, 0, 4 }; // relative to 8!

constexpr int8_t MAX_TRAINER_CHANNELS_M8 = MAX_TRAINER_CHANNELS - 8;
constexpr int8_t MAX_EXTRA_MODULE_CHANNELS_M8 = 8; // only 16ch PPM

inline int8_t maxModuleChannels_M8(uint8_t moduleIdx)
{
  if (isExtraModule(moduleIdx)) {
    return MAX_EXTRA_MODULE_CHANNELS_M8;
  }
  else if (isModuleXJT(moduleIdx)) {
    return maxChannelsXJT_M8[1 + g_model.moduleData[moduleIdx].subType];
  }
  else if (isModuleR9M(moduleIdx)) {
    if (isModuleR9M_LBT(moduleIdx)) {
      if (isModuleR9MLite(moduleIdx))
        return g_model.moduleData[moduleIdx].pxx.power == R9M_LITE_LBT_POWER_25_8CH ? 0 : 8;
      else
        return g_model.moduleData[moduleIdx].pxx.power == R9M_LBT_POWER_25_8CH ? 0 : 8;
    }
    else {
      return 8; // always 16 channels in FCC / FLEX
    }
  }
  else {
    return maxChannelsModules_M8[g_model.moduleData[moduleIdx].type];
  }
}

inline int8_t maxModuleChannels(uint8_t moduleIdx)
{
  return maxModuleChannels_M8(moduleIdx) + 8;
}

inline int8_t minModuleChannels(uint8_t idx)
{
  if (isModuleCrossfire(idx))
    return 16;
  else
    return 1;
}

inline int8_t defaultModuleChannels_M8(uint8_t idx)
{
  if (isModulePPM(idx))
    return 0; // 8 channels
  else if (isModuleDSM2(idx))
    return 0; // 8 channels
  else if (isModuleMultimoduleDSM2(idx))
    return -1; // 7 channels
  else if (isModuleXJTD8(idx))
    return 0;  // 8 channels
  else if (isModuleXJTLR12(idx))
    return 4;  // 12 channels
  else if (isModulePXX2(idx))
    return 8; // 16 channels
  else if (isModuleFlysky(idx))
    return 6; // 14 channels
  else
    return maxModuleChannels_M8(idx);
}

inline int8_t sentModuleChannels(uint8_t idx)
{
  if (isModuleCrossfire(idx))
    return CROSSFIRE_CHANNELS_COUNT;
  else if (isModuleMultimodule(idx) && !isModuleMultimoduleDSM2(idx))
    return 16;
  else
    return 8 + g_model.moduleData[idx].channelsCount;
}

inline bool isDefaultModelRegistrationID()
{
  return memcmp(g_model.modelRegistrationID, g_eeGeneral.ownerRegistrationID, PXX2_LEN_REGISTRATION_ID) == 0;
}

inline bool isModuleRxNumAvailable(uint8_t moduleIdx)
{
  if (isModuleXJT(moduleIdx))
    return g_model.moduleData[moduleIdx].subType != MODULE_SUBTYPE_PXX1_ACCST_D8;

  if (isModuleR9M(moduleIdx))
    return true;

  if (isModuleDSM2(moduleIdx))
    return true;

  if (isModuleISRM(moduleIdx))
    return true;

  if (isModuleMultimodule(moduleIdx))
    return true;

  if(isModuleMultimodule(moduleIdx))
    return true;

  return false;
}

inline bool isModuleModelIndexAvailable(uint8_t idx)
{
  if (isModuleXJT(idx))
    return g_model.moduleData[idx].subType != MODULE_SUBTYPE_PXX1_ACCST_D8;

  if (isModuleR9M(idx))
    return true;

  if (isModuleDSM2(idx))
    return true;

  if (isModuleISRM(idx))
    return true;

  if (isModuleMultimodule(idx))
    return true;

  return false;
}

inline bool isModuleFailsafeAvailable(uint8_t idx)
{
#if defined(PXX2)
  if (isModuleISRM(idx))
    return true;
#endif

  if (isModuleXJT(idx))
    return g_model.moduleData[idx].subType == MODULE_SUBTYPE_PXX1_ACCST_D16;

#if defined(MULTIMODULE)
  if (isModuleMultimodule(idx))
    return multiModuleStatus.isValid() && multiModuleStatus.supportsFailsafe();
#endif

  if (isModuleR9M(idx))
    return true;

  return false;
}

inline bool isModuleBindRangeAvailable(uint8_t moduleIdx)
{
  return isModulePXX2(moduleIdx) || isModulePXX1(moduleIdx) || isModuleDSM2(moduleIdx) || isModuleMultimodule(moduleIdx);
}

inline uint8_t getMaxRxNum(uint8_t idx)
{
  if (isModuleDSM2(idx))
    return 20;

#if defined(MULTIMODULE)
  if (isModuleMultimodule(idx))
    return g_model.moduleData[idx].getMultiProtocol(true) == MODULE_SUBTYPE_MULTI_OLRS ? 4 : 15;
#endif

  return 63;
}

inline const char * getModuleDelay(uint8_t idx)
{
  if (isModuleISRMAccess(idx))
    return sentModuleChannels(idx) > 16 ? "(21ms)" : (sentModuleChannels(idx) > 8 ? "(14ms)" : "(7ms)");

  if (isModuleXJTD16(idx) || isModuleR9MNonAccess(idx))
    return sentModuleChannels(idx) > 8 ? "(18ms)" : "(9ms)";

  return nullptr;
}

inline bool isBindCh9To16Allowed(uint8_t moduleIndex)
{
  if (g_model.moduleData[moduleIndex].channelsCount <= 0) {
    return false;
  }

  if (isModuleR9M_LBT(moduleIndex)) {
    if (isModuleR9MLite(moduleIndex))
      return g_model.moduleData[moduleIndex].pxx.power != R9M_LBT_POWER_25_8CH;
    else
      return g_model.moduleData[moduleIndex].pxx.power != R9M_LITE_LBT_POWER_25_8CH;
  }
  else {
    return true;
  }
}

inline bool isTelemAllowedOnBind(uint8_t moduleIndex)
{
#if defined(HARDWARE_INTERNAL_MODULE)
  if (moduleIndex == INTERNAL_MODULE)
    return isModuleISRM(moduleIndex) || isSportLineUsedByInternalModule();

  if (isSportLineUsedByInternalModule())
    return false;
#endif

  if (g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_R9M_LITE_PXX1) {
    if (isModuleR9M_LBT(EXTERNAL_MODULE))
      return g_model.moduleData[EXTERNAL_MODULE].pxx.power < R9M_LITE_LBT_POWER_100_16CH_NOTELEM;
    else
      return true;
  }

  if (g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_R9M_PXX1 || g_model.moduleData[EXTERNAL_MODULE].type == MODULE_TYPE_R9M_LITE_PRO_PXX1) {
    if (isModuleR9M_LBT(EXTERNAL_MODULE))
      return g_model.moduleData[EXTERNAL_MODULE].pxx.power < R9M_LBT_POWER_200_16CH_NOTELEM;
    else
      return true;
  }

  return true;
}

inline bool isPXX2ReceiverUsed(uint8_t moduleIdx, uint8_t receiverIdx)
{
  return g_model.moduleData[moduleIdx].pxx2.receivers & (1 << receiverIdx);
}

inline void setPXX2ReceiverUsed(uint8_t moduleIdx, uint8_t receiverIdx)
{
  g_model.moduleData[moduleIdx].pxx2.receivers |= (1 << receiverIdx);
}

inline bool isPXX2ReceiverEmpty(uint8_t moduleIdx, uint8_t receiverIdx)
{
  return is_memclear(g_model.moduleData[moduleIdx].pxx2.receiverName[receiverIdx], PXX2_LEN_RX_NAME);
}

inline void removePXX2Receiver(uint8_t moduleIdx, uint8_t receiverIdx)
{
  memclear(g_model.moduleData[moduleIdx].pxx2.receiverName[receiverIdx], PXX2_LEN_RX_NAME);
  g_model.moduleData[moduleIdx].pxx2.receivers &= ~(1 << receiverIdx);
  storageDirty(EE_MODEL);
}

inline void removePXX2ReceiverIfEmpty(uint8_t moduleIdx, uint8_t receiverIdx)
{
  if (isPXX2ReceiverEmpty(moduleIdx, receiverIdx)) {
    removePXX2Receiver(moduleIdx, receiverIdx);
  }
}

inline void setDefaultPpmFrameLength(uint8_t moduleIdx)
{
  g_model.moduleData[moduleIdx].ppm.frameLength = 4 * max<int>(0, g_model.moduleData[moduleIdx].channelsCount);
}

inline void setModuleType(uint8_t moduleIdx, uint8_t moduleType)
{
  ModuleData & moduleData = g_model.moduleData[moduleIdx];
  memclear(&moduleData, sizeof(ModuleData));
  moduleData.type = moduleType;
  moduleData.channelsCount = defaultModuleChannels_M8(moduleIdx);
  if (moduleData.type == MODULE_TYPE_SBUS)
    moduleData.sbus.refreshRate = -31;
  else if (moduleData.type == MODULE_TYPE_PPM)
    setDefaultPpmFrameLength(moduleIdx);
}

#endif // _MODULES_HELPERS_H_
