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

void resetModuleSettings(uint8_t module)
{
  g_model.moduleData[module].channelsStart = 0;
  g_model.moduleData[module].channelsCount = defaultModuleChannels_M8(module);
  g_model.moduleData[module].rfProtocol = 0;
  if (isModulePPM(module)) {
    SET_DEFAULT_PPM_FRAME_LENGTH(module);
  }
  else if (isModuleSBUS(module)) {
    g_model.moduleData[module].sbus.refreshRate = -31;
  }
}

uint8_t pxx2FindEmptyReceiverSlot()
{
  for (uint8_t slot=0; slot<NUM_RECEIVERS; slot++) {
    if (!g_model.receiverData[slot].used) {
      return slot + 1;
    }
  }
  return 0;
}

void pxx2AddReceiver(uint8_t moduleIndex, uint8_t receiverIndex)
{
  uint8_t slot = pxx2FindEmptyReceiverSlot();
  if (slot > 0) {
    g_model.moduleData[moduleIndex].pxx2.receivers |= (slot << (receiverIndex * 3));
    --slot;
    g_model.receiverData[slot].used = 1;
#warning "USE 32bits copy"
    g_model.receiverData[slot].channelMapping0 = (0 << 0) + (1 << 5) + (2 << 10) + (3 << 15) + (4 << 20) + (5 << 25) + ((uint64_t)6 << 30) + ((uint64_t)7 << 35) + ((uint64_t)8 << 40) + ((uint64_t)9 << 45) + ((uint64_t)10 << 50) + ((uint64_t)11 << 55);
    g_model.receiverData[slot].channelMapping1 = (12 << 0) + (13 << 5) + (14 << 10) + (15 << 15) + (16 << 20) + (17 << 25) + ((uint64_t)18 << 30) + ((uint64_t)19 << 35) + ((uint64_t)20 << 40) + ((uint64_t)21 << 45) + ((uint64_t)22 << 50) + ((uint64_t)23 << 55);
    storageDirty(EE_MODEL);
  }
}

void pxx2DeleteReceiver(uint8_t moduleIndex, uint8_t receiverIndex)
{
  uint8_t receiverSlot = g_model.moduleData[moduleIndex].pxx2.getReceiverSlot(receiverIndex) - 1;
  g_model.moduleData[moduleIndex].pxx2.receivers = (g_model.moduleData[moduleIndex].pxx2.receivers & BF_MASK<uint16_t>(0, receiverIndex * 3)) | ((g_model.moduleData[moduleIndex].pxx2.receivers & BF_MASK<uint16_t>((receiverIndex + 1) * 3, (MAX_RECEIVERS_PER_MODULE - 1 - receiverIndex) * 3)) >> 3);
  memclear(&g_model.receiverData[receiverSlot], sizeof(ReceiverData));
  storageDirty(EE_MODEL);
}
