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

#include <opentx.h>
#include "opentx.h"

class Pxx2Telemetry
{
  protected:
    static void processGetHardwareInfoFrame(uint8_t module, uint8_t *frame);

    static void processReceiverSettingsFrame(uint8_t module, uint8_t *frame);

    static void processRegisterFrame(uint8_t module, uint8_t * frame);

    static void processBindFrame(uint8_t module, uint8_t * frame);

    static void processTelemetryFrame(uint8_t module, uint8_t * frame);

    static void processSpectrumFrame(uint8_t module, uint8_t * frame);

    static void processRadioFrame(uint8_t module, uint8_t * frame);

    static void processPowerMeterFrame(uint8_t module, uint8_t * frame);

  public:
};

void processGetHardwareInfoFrame(uint8_t module, uint8_t * frame)
{
  if (moduleSettings[module].mode != MODULE_MODE_GET_HARDWARE_INFO) {
    return;
  }

  uint8_t index = frame[3];
  if (index == 0xFF)
    memcpy(&reusableBuffer.hardwareAndSettings.modules[module].information, &frame[4], sizeof(PXX2HardwareInformation));
  else if (index < PXX2_MAX_RECEIVERS_PER_MODULE)
    memcpy(&reusableBuffer.hardwareAndSettings.modules[module].receivers[index].information, &frame[4], sizeof(PXX2HardwareInformation));
}

void processModuleSettingsFrame(uint8_t module, uint8_t * frame)
{
  if (moduleSettings[module].mode != MODULE_MODE_MODULE_SETTINGS) {
    return;
  }

  // Flag1
  reusableBuffer.hardwareAndSettings.moduleSettings.txPower = frame[4] >> 4;
  if (frame[4] & PXX2_TX_SETTINGS_FLAG1_EXTERNAL_ANTENNA)
    reusableBuffer.hardwareAndSettings.moduleSettings.externalAntenna = 1;

  // Power
  reusableBuffer.hardwareAndSettings.moduleSettings.txPower = frame[5];

  reusableBuffer.hardwareAndSettings.moduleSettings.state = PXX2_SETTINGS_OK;
  reusableBuffer.hardwareAndSettings.moduleSettings.timeout = 0;
  moduleSettings[module].mode = MODULE_MODE_NORMAL;
}

void processReceiverSettingsFrame(uint8_t module, uint8_t * frame)
{
  if (moduleSettings[module].mode != MODULE_MODE_RECEIVER_SETTINGS) {
    return;
  }

  if (frame[4] & PXX2_RX_SETTINGS_FLAG1_FASTPWM)
    reusableBuffer.hardwareAndSettings.receiverSettings.pwmRate = 1;

  if (frame[4] & PXX2_RX_SETTINGS_FLAG1_TELEMETRY_DISABLED)
    reusableBuffer.hardwareAndSettings.receiverSettings.telemetryDisabled = 1;

  uint8_t outputsCount = min<uint8_t>(16, frame[0] - 4);
  reusableBuffer.hardwareAndSettings.receiverSettings.outputsCount = outputsCount;
  for (uint8_t pin = 0; pin < outputsCount; pin++) {
    reusableBuffer.hardwareAndSettings.receiverSettings.outputsMapping[pin] = frame[5 + pin];
  }

  reusableBuffer.hardwareAndSettings.receiverSettings.state = PXX2_SETTINGS_OK;
  reusableBuffer.hardwareAndSettings.receiverSettings.timeout = 0;
  moduleSettings[module].mode = MODULE_MODE_NORMAL;
}

void processRegisterFrame(uint8_t module, uint8_t * frame)
{
  if (moduleSettings[module].mode != MODULE_MODE_REGISTER) {
    return;
  }

  switch(frame[3]) {
    case 0x00:
      if (reusableBuffer.moduleSetup.pxx2.registerStep == REGISTER_START) {
        // RX_NAME follows, we store it for the next step
        str2zchar(reusableBuffer.moduleSetup.pxx2.registerRxName, (const char *)&frame[4], PXX2_LEN_RX_NAME);
        reusableBuffer.moduleSetup.pxx2.registerLoopIndex = frame[12];
        reusableBuffer.moduleSetup.pxx2.registerStep = REGISTER_RX_NAME_RECEIVED;
      }
      break;

    case 0x01:
      if (reusableBuffer.moduleSetup.pxx2.registerStep == REGISTER_RX_NAME_SELECTED) {
        // RX_NAME + PASSWORD follow, we check they are good
        if (cmpStrWithZchar((char *)&frame[4], reusableBuffer.moduleSetup.pxx2.registerRxName, PXX2_LEN_RX_NAME) &&
            cmpStrWithZchar((char *)&frame[12], g_model.modelRegistrationID, PXX2_LEN_REGISTRATION_ID)) {
          reusableBuffer.moduleSetup.pxx2.registerStep = REGISTER_OK;
          moduleSettings[module].mode = MODULE_MODE_NORMAL;
          POPUP_INFORMATION(STR_REG_OK);
        } else {
          TRACE("KO %s %s", &frame[4], reusableBuffer.moduleSetup.pxx2.registerRxName);
        }
      }
      break;
  }
}

void processBindFrame(uint8_t module, uint8_t * frame)
{
  if (moduleSettings[module].mode != MODULE_MODE_BIND) {
    return;
  }

  switch(frame[3]) {
    case 0x00:
      if (reusableBuffer.moduleSetup.pxx2.bindStep == BIND_START) {
        bool found = false;
        for (uint8_t i=0; i<reusableBuffer.moduleSetup.pxx2.bindCandidateReceiversCount; i++) {
          if (memcmp(reusableBuffer.moduleSetup.pxx2.bindCandidateReceiversNames[i], &frame[4], PXX2_LEN_RX_NAME) == 0) {
            found = true;
            break;
          }
        }
        if (!found && reusableBuffer.moduleSetup.pxx2.bindCandidateReceiversCount < PXX2_MAX_RECEIVERS_PER_MODULE) {
          memcpy(reusableBuffer.moduleSetup.pxx2.bindCandidateReceiversNames[reusableBuffer.moduleSetup.pxx2.bindCandidateReceiversCount++], &frame[4], PXX2_LEN_RX_NAME);
        }
      }
      break;

    case 0x01:
      if (reusableBuffer.moduleSetup.pxx2.bindStep == BIND_RX_NAME_SELECTED) {
        if (memcmp(&reusableBuffer.moduleSetup.pxx2.bindCandidateReceiversNames[reusableBuffer.moduleSetup.pxx2.bindSelectedReceiverIndex], &frame[4], PXX2_LEN_RX_NAME) == 0) {
          memcpy(g_model.moduleData[module].pxx2.receiverName[reusableBuffer.moduleSetup.pxx2.bindReceiverIndex], &frame[4], PXX2_LEN_RX_NAME);
          storageDirty(EE_MODEL);
          reusableBuffer.moduleSetup.pxx2.bindStep = BIND_WAIT;
          reusableBuffer.moduleSetup.pxx2.bindWaitTimeout = get_tmr10ms() + 30;
        }
      }
      break;
  }
}

void processTelemetryFrame(uint8_t module, uint8_t * frame)
{
  uint8_t origin = (module << 2) + (frame[3] & 0x03);
  sportProcessTelemetryPacketWithoutCrc(origin, &frame[4]);
}

void processSpectrumAnalyserFrame(uint8_t module, uint8_t * frame)
{
  if (moduleSettings[module].mode != MODULE_MODE_SPECTRUM_ANALYSER) {
    return;
  }

  uint32_t * frequency = (uint32_t *)&frame[4];
  int8_t * power = (int8_t *)&frame[8];

  // center = 2440000000;  // 2440MHz
  // span = 40000000;  // 40MHz
  // left = 2440000000 - 20000000
  // step = 10000

  // TRACE("Fq=%u, Pw=%d, X=%d, Y=%d", *frequency, int32_t(*power), D * 128 / 40000000, int32_t(127 + *power));

  int32_t position = *frequency - reusableBuffer.spectrumAnalyser.freq + (reusableBuffer.spectrumAnalyser.span / 2);
  unsigned x = (position * LCD_W) / reusableBuffer.spectrumAnalyser.span;
  if (x < LCD_W) {
    reusableBuffer.spectrumAnalyser.bars[x] = 127 + *power;
  }
}

void processPowerMeterFrame(uint8_t module, uint8_t * frame)
{
  if (moduleSettings[module].mode != MODULE_MODE_POWER_METER) {
    return;
  }

  reusableBuffer.powerMeter.power = *((int16_t *)&frame[8]);
  if (!reusableBuffer.powerMeter.peak || reusableBuffer.powerMeter.power > reusableBuffer.powerMeter.peak) {
    reusableBuffer.powerMeter.peak = reusableBuffer.powerMeter.power;
  }
}

void processModuleFrame(uint8_t module, uint8_t *frame)
{
  switch (frame[2]) {
    case PXX2_TYPE_ID_HW_INFO:
      processGetHardwareInfoFrame(module, frame);
      break;

    case PXX2_TYPE_ID_TX_SETTINGS:
      processModuleSettingsFrame(module, frame);
      break;

    case PXX2_TYPE_ID_RX_SETTINGS:
      processReceiverSettingsFrame(module, frame);
      break;

    case PXX2_TYPE_ID_REGISTER:
      processRegisterFrame(module, frame);
      break;

    case PXX2_TYPE_ID_BIND:
      processBindFrame(module, frame);
      break;

    case PXX2_TYPE_ID_TELEMETRY:
      processTelemetryFrame(module, frame);
      break;
  }
}

void processToolsFrame(uint8_t module, uint8_t * frame)
{
  switch (frame[2]) {
    case PXX2_TYPE_ID_POWER_METER:
      processPowerMeterFrame(module, frame);
      break;

    case PXX2_TYPE_ID_SPECTRUM:
      processSpectrumAnalyserFrame(module, frame);
      break;
  }
}

void processPXX2Frame(uint8_t module, uint8_t *frame)
{
  switch (frame[1]) {
    case PXX2_TYPE_C_MODULE:
      processModuleFrame(module, frame);
      break;

    case PXX2_TYPE_C_POWER_METER:
      processToolsFrame(module, frame);
      break;

    default:
      break;
  }
}
