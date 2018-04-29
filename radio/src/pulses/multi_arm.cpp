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

// for the  MULTI protocol definition
// see https://github.com/pascallanger/DIY-Multiprotocol-TX-Module
// file Multiprotocol/multiprotocol.h

#define MULTI_SEND_BIND                     (1 << 7)
#define MULTI_SEND_RANGECHECK               (1 << 5)
#define MULTI_SEND_AUTOBIND                 (1 << 6)

#define MULTI_CHANS                         16
#define MULTI_CHAN_BITS                     11

static void sendFrameProtocolHeader(uint8_t port, bool failsafe);

void sendChannels(uint8_t port);

static void sendSetupFrame()
{
  // Old multi firmware will mark config messsages as invalid frame and throw them away
  sendByteSbus('M');
  sendByteSbus('P');
  sendByteSbus(0x80);           // Module Configuration
  sendByteSbus(1);              // 1 byte data
  uint8_t config = 0x01 | 0x02; // inversion + multi_telemetry
#if !defined(PPM_PIN_SERIAL)
  // TODO why PPM_PIN_SERIAL would change MULTI protocol?
  config |= 0x04;               // input synchronsisation
#endif

  sendByteSbus(config);
}

static void sendFailsafeChannels(uint8_t port)
{
  uint32_t bits = 0;
  uint8_t bitsavailable = 0;

  for (int i = 0; i < MULTI_CHANS; i++) {
    int16_t failsafeValue = g_model.moduleData[port].failsafeChannels[i];
    int pulseValue;
    if (g_model.moduleData[port].failsafeMode == FAILSAFE_HOLD)
      failsafeValue = FAILSAFE_CHANNEL_HOLD;

    if (g_model.moduleData[port].failsafeMode == FAILSAFE_NOPULSES)
      failsafeValue = FAILSAFE_CHANNEL_NOPULSE;

    if (failsafeValue == FAILSAFE_CHANNEL_HOLD) {
      pulseValue = 0;
    }
    else if (failsafeValue == FAILSAFE_CHANNEL_NOPULSE) {
      pulseValue = 2047;
    }
    else {
      failsafeValue += 2 * PPM_CH_CENTER(g_model.moduleData[port].channelsStart + i) - 2 * PPM_CENTER;
      pulseValue = limit(1, (failsafeValue * 800 / 1000) + 1024, 2047);
    }

    bits |= pulseValue << bitsavailable;
    bitsavailable += MULTI_CHAN_BITS;
    while (bitsavailable >= 8) {
      sendByteSbus((uint8_t) (bits & 0xff));
      bits >>= 8;
      bitsavailable -= 8;
    }
  }
}

void setupPulsesMultimodule(uint8_t port)
{
  static int counter = 0;

#if defined(PPM_PIN_SERIAL)
  modulePulsesData[EXTERNAL_MODULE].dsm2.serialByte = 0 ;
  modulePulsesData[EXTERNAL_MODULE].dsm2.serialBitCount = 0 ;
#else
  modulePulsesData[EXTERNAL_MODULE].dsm2.rest = multiSyncStatus.getAdjustedRefreshRate();
  modulePulsesData[EXTERNAL_MODULE].dsm2.index = 0;
#endif

  modulePulsesData[EXTERNAL_MODULE].dsm2.ptr = modulePulsesData[EXTERNAL_MODULE].dsm2.pulses;

  // Every 1000 cycles (=9s) send a config packet that configures the multimodule (inversion, telemetry type)
  counter++;
  if (counter  % 1000== 500) {
    sendSetupFrame();
  } else if (counter % 1000 == 0 && g_model.moduleData[port].failsafeMode != FAILSAFE_NOT_SET && g_model.moduleData[port].failsafeMode != FAILSAFE_RECEIVER) {
    sendFrameProtocolHeader(port, true);
    sendFailsafeChannels(port);
  } else {
    // Normal Frame
    sendFrameProtocolHeader(port, false);
    sendChannels(port);
  }

  putDsm2Flush();
}


void sendChannels(uint8_t port)
{
  uint32_t bits = 0;
  uint8_t bitsavailable = 0;

  // byte 4-25, channels 0..2047
  // Range for pulses (channelsOutputs) is [-1024:+1024] for [-100%;100%]
  // Multi uses [204;1843] as [-100%;100%]
  for (int i = 0; i < MULTI_CHANS; i++) {
    int channel = g_model.moduleData[port].channelsStart + i;
    int value = channelOutputs[channel] + 2 * PPM_CH_CENTER(channel) - 2 * PPM_CENTER;

    // Scale to 80%
    value = value * 800 / 1000 + 1024;
    value = limit(0, value, 2047);

    bits |= value << bitsavailable;
    bitsavailable += MULTI_CHAN_BITS;
    while (bitsavailable >= 8) {
      sendByteSbus((uint8_t) (bits & 0xff));
      bits >>= 8;
      bitsavailable -= 8;
    }
  }
}

void sendFrameProtocolHeader(uint8_t port, bool failsafe)
{// byte 1+2, protocol information

  // Our enumeration starts at 0
  int type = g_model.moduleData[port].getMultiProtocol(false) + 1;
  int subtype = g_model.moduleData[port].subType;
  int8_t optionValue = g_model.moduleData[port].multi.optionValue;

  uint8_t protoByte = 0;
  if (moduleFlag[port] == MODULE_BIND)
    protoByte |= MULTI_SEND_BIND;
  else if (moduleFlag[port] == MODULE_RANGECHECK)
    protoByte |= MULTI_SEND_RANGECHECK;

  // rfProtocol
  if (g_model.moduleData[port].getMultiProtocol(true) == MM_RF_PROTO_DSM2) {

    // Autobinding should always be done in DSMX 11ms
    if (g_model.moduleData[port].multi.autoBindMode && moduleFlag[port] == MODULE_BIND)
      subtype = MM_RF_DSM2_SUBTYPE_AUTO;

    // Multi module in DSM mode wants the number of channels to be used as option value
    optionValue = NUM_CHANNELS(EXTERNAL_MODULE);

  }

  // 15  for Multimodule is FrskyX or D16 which we map as a subprotocol of 3 (FrSky)
  // all protos > frskyx are therefore also off by one
  if (type >= 15)
    type = type + 1;

  // 25 is again a FrSky protocol (FrskyV) so shift again
  if (type >= 25)
    type = type + 1;

  if (g_model.moduleData[port].getMultiProtocol(true) == MM_RF_PROTO_FRSKY) {
    if (subtype == MM_RF_FRSKY_SUBTYPE_D8) {
      //D8
      type = 3;
      subtype = 0;
    } else if (subtype == MM_RF_FRSKY_SUBTYPE_V8) {
      //V8
      type = 25;
      subtype = 0;
    } else {
      type = 15;
      if (subtype == MM_RF_FRSKY_SUBTYPE_D16_8CH) // D16 8ch
        subtype = 1;
      else if (subtype == MM_RF_FRSKY_SUBTYPE_D16)
        subtype = 0;  // D16
      else if (subtype == MM_RF_FRSKY_SUBTYPE_D16_LBT)
        subtype = 2;
      else
        subtype = 3; // MM_RF_FRSKY_SUBTYPE_D16_LBT_8CH
    }
  }

  // Set the highest bit of option byte in AFHDS2A protocol to instruct MULTI to passthrough telemetry bytes instead
  // of sending Frsky D telemetry
  if (g_model.moduleData[port].getMultiProtocol(false) == MM_RF_PROTO_FS_AFHDS2A)
    optionValue = optionValue | 0x80;

  // For custom protocol send unmodified type byte
  if (g_model.moduleData[port].getMultiProtocol(true) == MM_RF_CUSTOM_SELECTED)
    type = g_model.moduleData[port].getMultiProtocol(false);


  uint8_t headerByte = 0x54;
  if (failsafe)
    headerByte = 0x56;

    // header, byte 0,  0x55 for proto 0-31 0x54 for 32-63
  if (type <= 31)
    sendByteSbus(headerByte+1);
  else
    sendByteSbus(headerByte);


  // protocol byte
  protoByte |= (type & 0x1f);
  if (g_model.moduleData[port].getMultiProtocol(true) != MM_RF_PROTO_DSM2)
    protoByte |= (g_model.moduleData[port].multi.autoBindMode << 6);

  sendByteSbus(protoByte);

  // byte 2, subtype, powermode, model id
  sendByteSbus((uint8_t) ((g_model.header.modelId[port] & 0x0f)
                           | ((subtype & 0x7) << 4)
                           | (g_model.moduleData[port].multi.lowPowerMode << 7))
  );

  // byte 3
  sendByteSbus((uint8_t) optionValue);
}
