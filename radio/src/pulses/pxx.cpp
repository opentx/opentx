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

template <class PxxTransport>
void PxxPulses<PxxTransport>::addChannels(uint8_t port, uint8_t sendFailsafe, uint8_t sendUpperChannels)
{
  uint16_t pulseValue = 0;
  uint16_t pulseValueLow = 0;

  for (int8_t i=0; i<8; i++) {
    if (sendFailsafe) {
      if (g_model.moduleData[port].failsafeMode == FAILSAFE_HOLD) {
        pulseValue = (i < sendUpperChannels ? 4095 : 2047);
      }
      else if (g_model.moduleData[port].failsafeMode == FAILSAFE_NOPULSES) {
        pulseValue = (i < sendUpperChannels ? 2048 : 0);
      }
      else {
        if (i < sendUpperChannels) {
          int16_t failsafeValue = g_model.moduleData[port].failsafeChannels[8+i];
          if (failsafeValue == FAILSAFE_CHANNEL_HOLD) {
            pulseValue = 4095;
          }
          else if (failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
            pulseValue = 2048;
          }
          else {
            failsafeValue += 2*PPM_CH_CENTER(8+g_model.moduleData[port].channelsStart+i) - 2*PPM_CENTER;
            pulseValue = limit(2049, (failsafeValue * 512 / 682) + 3072, 4094);
          }
        }
        else {
          int16_t failsafeValue = g_model.moduleData[port].failsafeChannels[i];
          if (failsafeValue == FAILSAFE_CHANNEL_HOLD) {
            pulseValue = 2047;
          }
          else if (failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
            pulseValue = 0;
          }
          else {
            failsafeValue += 2*PPM_CH_CENTER(g_model.moduleData[port].channelsStart+i) - 2*PPM_CENTER;
            pulseValue = limit(1, (failsafeValue * 512 / 682) + 1024, 2046);
          }
        }
      }
    }
    else {
      if (i < sendUpperChannels) {
        int channel = 8 + g_model.moduleData[port].channelsStart + i;
        int value = channelOutputs[channel] + 2*PPM_CH_CENTER(channel) - 2*PPM_CENTER;
        pulseValue = limit(2049, (value * 512 / 682) + 3072, 4094);
      }
      else if (i < sentModuleChannels(port)) {
        int channel = g_model.moduleData[port].channelsStart + i;
        int value = channelOutputs[channel] + 2*PPM_CH_CENTER(channel) - 2*PPM_CENTER;
        pulseValue = limit(1, (value * 512 / 682) + 1024, 2046);
      }
      else {
        pulseValue = 1024;
      }
    }

    if (i & 1) {
      PxxTransport::addByte(pulseValueLow); // Low byte of channel
      PxxTransport::addByte(((pulseValueLow >> 8) & 0x0F) | (pulseValue << 4));  // 4 bits each from 2 channels
      PxxTransport::addByte(pulseValue >> 4);  // High byte of channel
    }
    else {
      pulseValueLow = pulseValue;
    }
  }
}

#if defined(PXX1)
template class PxxPulses<StandardPxx1Transport<PwmPxxBitTransport> >;
template class PxxPulses<StandardPxx1Transport<SerialPxxBitTransport> >;
template class PxxPulses<UartPxx1Transport>;
#endif

#if defined(PXX2)
template class PxxPulses<Pxx2Transport>;
#endif
