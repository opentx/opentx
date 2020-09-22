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

// Range for pulses (channels output) is [-1024:+1024]
uint8_t createGhostChannelsFrame(uint8_t * frame, int16_t * pulses)
{
  static uint8_t lastGhostFrameId = GHST_UL_RC_CHANS_HS4_5TO8;
  uint8_t ghostUpper4Offset = 0;

  switch (lastGhostFrameId) {
    case GHST_UL_RC_CHANS_HS4_5TO8:
      ghostUpper4Offset = 0;
      break;
    case GHST_UL_RC_CHANS_HS4_9TO12:
      ghostUpper4Offset = 4;
      break;
    case GHST_UL_RC_CHANS_HS4_13TO16:
      ghostUpper4Offset = 8;
      break;
  }

  uint8_t * buf = frame;
#if SPORT_MAX_BAUDRATE < 400000
  *buf++ = g_eeGeneral.telemetryBaudrate == GHST_TELEMETRY_RATE_400K ? GHST_ADDR_MODULE_SYM : GHST_ADDR_MODULE_ASYM;
#else
  *buf++ = GHST_ADDR_MODULE_SYM;
#endif
  *buf++ = GHST_UL_RC_CHANS_SIZE;
  uint8_t * crc_start = buf;
  *buf++ = lastGhostFrameId;

  // first 4 high speed, 12 bit channels (11 relevant bits with openTx)
  uint32_t bits = 0;
  uint8_t bitsavailable = 0;
  for (int i = 0; i < 4; i++) {
    uint32_t value = limit(0, GHST_RC_CTR_VAL_12BIT + (((pulses[i]) << 3) / 5), 2 * GHST_RC_CTR_VAL_12BIT);
    bits |= value << bitsavailable;
    bitsavailable += GHST_CH_BITS_12;
    while (bitsavailable >= 8) {
      *buf++ = bits;
      bits >>= 8;
      bitsavailable -= 8;
    }
  }

  // second 4 lower speed, 8 bit channels
  for (int i = 4; i < 8; ++i) {
    *buf++ = limit(0, GHST_RC_CTR_VAL_8BIT + (((pulses[i + ghostUpper4Offset]) >> 1) / 5), 2 * GHST_RC_CTR_VAL_8BIT);
  }

  *buf++ = crc8(crc_start, GHST_UL_RC_CHANS_SIZE - 1);

  switch (lastGhostFrameId) {
    case GHST_UL_RC_CHANS_HS4_5TO8:
      lastGhostFrameId = GHST_UL_RC_CHANS_HS4_9TO12;
      break;
    case GHST_UL_RC_CHANS_HS4_9TO12:
      lastGhostFrameId = GHST_UL_RC_CHANS_HS4_13TO16;
      break;
    case GHST_UL_RC_CHANS_HS4_13TO16:
      lastGhostFrameId = GHST_UL_RC_CHANS_HS4_5TO8;
      break;
  }

  return buf - frame;
}

void setupPulsesGhost()
{
  if (telemetryProtocol == PROTOCOL_TELEMETRY_GHOST) {
    uint8_t * pulses = extmodulePulsesData.ghost.pulses;
    extmodulePulsesData.ghost.length = createGhostChannelsFrame(pulses, &channelOutputs[g_model.moduleData[EXTERNAL_MODULE].channelsStart]);
  }
}
