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
#include "pulses/pxx.h"

void uartInitPcmArray(uint8_t port)
{
  modulePulsesData[port].pxx_uart.pcmCrc = 0;
  modulePulsesData[port].pxx_uart.ptr = modulePulsesData[port].pxx_uart.pulses;
}

void putPXX2Byte(uint8_t port, uint8_t byte)
{
  modulePulsesData[port].pxx_uart.pcmCrc = (modulePulsesData[port].pxx_uart.pcmCrc<<8) ^ (CRCTable[((modulePulsesData[port].pxx_uart.pcmCrc>>8) ^ byte) & 0xFF]);
  *modulePulsesData[port].pxx_uart.ptr++ = byte;
}

void createPXX2ChannelsFrame(uint8_t port)
{
  uint16_t pulseValue=0, pulseValueLow=0;

  static uint8_t pass[NUM_MODULES] = { MODULES_INIT(0) };
  uint8_t sendUpperChannels = 0;
  if (pass[port]++ & 0x01) {
    sendUpperChannels = g_model.moduleData[port].channelsCount;
  }

  uartInitPcmArray(port);

  // Model ID
  putPXX2Byte(port, g_model.header.modelId[port]);

  // Flag1
  uint8_t flag1 = (g_model.moduleData[port].rfProtocol << 6);
  if (moduleFlag[port] == MODULE_BIND) {
    flag1 |= (g_eeGeneral.countryCode << 1) | PXX_SEND_BIND;
  }
  else if (moduleFlag[port] == MODULE_RANGECHECK) {
    flag1 |= PXX_SEND_RANGECHECK;
  }
  else if (g_model.moduleData[port].failsafeMode != FAILSAFE_NOT_SET && g_model.moduleData[port].failsafeMode != FAILSAFE_RECEIVER) {
    if (failsafeCounter[port]-- == 0) {
      failsafeCounter[port] = 1000;
      flag1 |= PXX_SEND_FAILSAFE;
    }
    if (failsafeCounter[port] == 0 && g_model.moduleData[port].channelsCount > 0) {
      flag1 |= PXX_SEND_FAILSAFE;
    }
  }
  putPXX2Byte(port, flag1);

  // Flag2
  putPXX2Byte(port, 0);

  // Channels
  for (int i=0; i<8; i++) {
    if (flag1 & PXX_SEND_FAILSAFE) {
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
      putPXX2Byte(port, pulseValueLow); // Low byte of channel
      putPXX2Byte(port, ((pulseValueLow >> 8) & 0x0F) | (pulseValue << 4));  // 4 bits each from 2 channels
      putPXX2Byte(port, pulseValue >> 4);  // High byte of channel
    }
    else {
      pulseValueLow = pulseValue;
    }
  }

  // Ext. flag (holds antenna selection on Horus internal module, 0x00 otherwise)
  uint8_t extra_flags = 0;
#if defined(PCBHORUS) || defined(PCBXLITE)
  if (port == INTERNAL_MODULE) {
    extra_flags |= (g_model.moduleData[port].pxx.external_antenna << 0);
  }
#endif
  extra_flags |= (g_model.moduleData[port].pxx.receiver_telem_off << 1);
  extra_flags |= (g_model.moduleData[port].pxx.receiver_channel_9_16 << 2);
  if (isModuleR9M(port)) {
    extra_flags |= (min(g_model.moduleData[port].pxx.power, isModuleR9M_FCC_VARIANT(port) ? (uint8_t)R9M_FCC_POWER_MAX : (uint8_t)R9M_LBT_POWER_MAX) << 3);
    if (isModuleR9M_EUPLUS(port)) {
      extra_flags |= (1 << 6);
    }
  }
  // Disable S.PORT if internal module is active
  if (IS_TELEMETRY_INTERNAL_MODULE()) {
    extra_flags |= (1 << 5);
  }
  putPXX2Byte(port, extra_flags);

  // Flag3, reserved
  putPXX2Byte(port, 0);

#if defined(LUA)
  if (outputTelemetryBufferTrigger != 0x00 && outputTelemetryBufferSize > 0) {
    // CMD/Resp
    putPXX2Byte(port, 1);
    // primID (1 byte) + dataID (2 bytes) + value (4 bytes)
    for (uint8_t i=0; i<7; i++) {
      putPXX2Byte(port, outputTelemetryBuffer[i]);
    }
    outputTelemetryBufferTrigger = 0x00;
    outputTelemetryBufferSize = 0;
  }
  else {
    // CMD/Resp
    putPXX2Byte(port, 0);
  }
#else
  // CMD/Resp
  putPXX2Byte(port, 0);
#endif

  // CRC
  putPXX2Byte(port, modulePulsesData[port].pxx_uart.pcmCrc); // TODO need to check the CRC chosen algorithm
}

