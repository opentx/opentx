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
#include "io/frsky_pxx2.h"
#include "pulses/pxx2.h"

uint8_t s_pulses_paused = 0;
ModuleState moduleState[NUM_MODULES];
InternalModulePulsesData intmodulePulsesData __DMA;
ExternalModulePulsesData extmodulePulsesData __DMA;
TrainerPulsesData trainerPulsesData __DMA;
AbstractModule* modules[NUM_MODULES][PROTOCOL_CHANNELS_COUNT];

#if defined(AFHDS2)
afhds2 afhds2int = afhds2(modules[INTERNAL_MODULE], INTERNAL_MODULE, PROTOCOL_CHANNELS_AFHDS2, &intmodulePulsesData.afhds2);
#endif
#if defined(AFHDS3)
#include "../telemetry/telemetry.h"
afhds3::afhds3 afhds3ext = afhds3::afhds3(modules[EXTERNAL_MODULE], EXTERNAL_MODULE, PROTOCOL_CHANNELS_AFHDS3, &extmodulePulsesData.afhds3, processFlySkySensor);
#endif

void ModuleState::startBind(BindInformation * destination, ModuleCallback bindCallback)
{
  bindInformation = destination;
  callback = bindCallback;
  setMode(MODULE_MODE_BIND);
#if defined(SIMU)
  bindInformation->candidateReceiversCount = 2;
  strcpy(bindInformation->candidateReceiversNames[0], "SimuRX1");
  strcpy(bindInformation->candidateReceiversNames[1], "SimuRX2");
#endif
}

void ModuleState::setMode(uint8_t targetMode, asyncOperationCallback_t callback) {
  uint8_t oldMode = mode;
  mode = targetMode;
  if (oldMode != mode) {
    uint8_t moduleIndex = &moduleState[EXTERNAL_MODULE] == this ? EXTERNAL_MODULE : INTERNAL_MODULE;
    AbstractModule* module = modules[moduleIndex][protocol];
    if (!module) return;
    switch (targetMode) {
    case MODULE_MODE_BIND:
      module->beginBind(callback);
      break;
    case MODULE_MODE_RANGECHECK:
      module->beginRangeTest(callback);
      break;
    case MODULE_MODE_NORMAL:
      module->cancelOperations();
      module->init();
      break;
    }
  }
}

void resetModuleSettings(uint8_t moduleIndex) {
  AbstractModule* module = modules[moduleIndex][moduleState[moduleIndex].protocol];
  if(module)
    module->setModuleSettingsToDefault();
  else {
    g_model.moduleData[moduleIndex].channelsStart = 0;
    g_model.moduleData[moduleIndex].channelsCount = defaultModuleChannels_M8(moduleIndex);
    g_model.moduleData[moduleIndex].subType = 0;
    if (isModulePPM(moduleIndex)) setDefaultPpmFrameLength(moduleIndex);
  }
  moduleState[moduleIndex].setMode(MODULE_MODE_NORMAL);
}

uint8_t getModuleType(uint8_t module)
{
  uint8_t type = g_model.moduleData[module].type;

#if defined(HARDWARE_INTERNAL_MODULE)
  if (module == INTERNAL_MODULE && isInternalModuleAvailable(type)) {
    return type;
  }
#endif

  if (module == EXTERNAL_MODULE && isExternalModuleAvailable(type)) {
    return type;
  }

  return MODULE_TYPE_NONE;
}

uint8_t getRequiredProtocol(uint8_t module)
{
  uint8_t protocol;

  switch (getModuleType(module)) {
    case MODULE_TYPE_PPM:
      protocol = PROTOCOL_CHANNELS_PPM;
      break;

    case MODULE_TYPE_XJT_PXX1:
#if defined(INTMODULE_USART)
      if (module == INTERNAL_MODULE) {
        protocol = PROTOCOL_CHANNELS_PXX1_SERIAL;
        break;
      }
#endif
      protocol = PROTOCOL_CHANNELS_PXX1_PULSES;
      break;

    case MODULE_TYPE_R9M_PXX1:
      protocol = PROTOCOL_CHANNELS_PXX1_PULSES;
      break;

#if defined(HARDWARE_EXTERNAL_MODULE_SIZE_SML)
    case MODULE_TYPE_R9M_LITE_PXX1:
    case MODULE_TYPE_R9M_LITE_PRO_PXX1:
      protocol = PROTOCOL_CHANNELS_PXX1_SERIAL;
      break;

    case MODULE_TYPE_R9M_LITE_PXX2:
      protocol = PROTOCOL_CHANNELS_PXX2_LOWSPEED;
      break;
#endif

    case MODULE_TYPE_ISRM_PXX2:
    case MODULE_TYPE_R9M_PXX2:
#if defined(HARDWARE_EXTERNAL_MODULE_SIZE_SML)
    case MODULE_TYPE_XJT_LITE_PXX2:
    case MODULE_TYPE_R9M_LITE_PRO_PXX2:
#endif
      protocol = PROTOCOL_CHANNELS_PXX2_HIGHSPEED;
      break;

    case MODULE_TYPE_SBUS:
      protocol = PROTOCOL_CHANNELS_SBUS;
      break;

#if defined(MULTIMODULE)
    case MODULE_TYPE_MULTIMODULE:
      protocol = PROTOCOL_CHANNELS_MULTIMODULE;
      break;
#endif

#if defined(DSM2)
    case MODULE_TYPE_DSM2:
      protocol = limit<uint8_t>(PROTOCOL_CHANNELS_DSM2_LP45, PROTOCOL_CHANNELS_DSM2_LP45+g_model.moduleData[module].subType, PROTOCOL_CHANNELS_DSM2_DSMX);
      // The module is set to OFF during one second before BIND start
      {
        static tmr10ms_t bindStartTime = 0;
        if (moduleState[module].mode == MODULE_MODE_BIND) {
          if (bindStartTime == 0) bindStartTime = get_tmr10ms();
          if ((tmr10ms_t)(get_tmr10ms() - bindStartTime) < 100) {
            protocol = PROTOCOL_CHANNELS_NONE;
            break;
          }
        }
        else {
          bindStartTime = 0;
        }
      }
      break;
#endif

#if defined(CROSSFIRE)
    case MODULE_TYPE_CROSSFIRE:
      protocol = PROTOCOL_CHANNELS_CROSSFIRE;
      break;
#endif

#if defined(AFHDS2)
    case MODULE_TYPE_AFHDS2:
      protocol = PROTOCOL_CHANNELS_AFHDS2;
      break;
#endif

#if defined(AFHDS3)
    case MODULE_TYPE_AFHDS3:
      protocol = PROTOCOL_CHANNELS_AFHDS3;
      break;
#endif

    default:
      protocol = PROTOCOL_CHANNELS_NONE;
      break;
  }

  if (s_pulses_paused) {
    protocol = PROTOCOL_CHANNELS_NONE;
  }

#if 0
  // will need an EEPROM conversion
  if (moduleState[module].mode == MODULE_OFF) {
    protocol = PROTOCOL_CHANNELS_NONE;
  }
#endif

  return protocol;
}

void enablePulsesExternalModule(uint8_t protocol)
{
  // start new protocol hardware here
  auto module = modules[EXTERNAL_MODULE][protocol];
  switch (protocol) {
#if defined(PXX1)
    case PROTOCOL_CHANNELS_PXX1_PULSES:
      extmodulePxx1PulsesStart();
      break;
#endif

#if defined(PXX1) && defined(HARDWARE_EXTERNAL_MODULE_SIZE_SML)
    case PROTOCOL_CHANNELS_PXX1_SERIAL:
      extmodulePxx1SerialStart();
      break;
#endif

#if defined(DSM2)
    case PROTOCOL_CHANNELS_DSM2_LP45:
    case PROTOCOL_CHANNELS_DSM2_DSM2:
    case PROTOCOL_CHANNELS_DSM2_DSMX:
      extmoduleSoftSerialStart(DSM2_BAUDRATE, DSM2_PERIOD * 2000, false);
      break;
#endif

#if defined(CROSSFIRE)
    case PROTOCOL_CHANNELS_CROSSFIRE:
      EXTERNAL_MODULE_ON();
      break;
#endif

#if defined(PXX2) && defined(EXTMODULE_USART)
    case PROTOCOL_CHANNELS_PXX2_HIGHSPEED:
      extmoduleSerialStart(PXX2_HIGHSPEED_BAUDRATE, 0, true, USART_Parity_No, USART_StopBits_1, USART_WordLength_8b);
      break;

    case PROTOCOL_CHANNELS_PXX2_LOWSPEED:
      extmoduleSerialStart(PXX2_LOWSPEED_BAUDRATE, 0, true, USART_Parity_No, USART_StopBits_1, USART_WordLength_8b);
      TRACE("PROTOCOL_CHANNELS_PXX2_LOWSPEED");
      break;
#endif

#if defined(MULTIMODULE)
    case PROTOCOL_CHANNELS_MULTIMODULE:
      extmoduleSoftSerialStart(MULTIMODULE_BAUDRATE, MULTIMODULE_PERIOD * 2000, true);
      break;
#endif

#if defined(SBUS)
    case PROTOCOL_CHANNELS_SBUS:
      extmoduleSoftSerialStart(SBUS_BAUDRATE, SBUS_PERIOD_HALF_US, false);
      break;
#endif

#if defined(PPM)
    case PROTOCOL_CHANNELS_PPM:
      extmodulePpmStart();
      break;
#endif
    default:
      if(module) {
        module->init();
        module->start();
      }
      break;
  }
}

bool setupPulsesExternalModule(uint8_t protocol)
{
  auto module = modules[EXTERNAL_MODULE][protocol];
  uint32_t period_ms = 0;
  switch (protocol) {
#if defined(PXX1)
    case PROTOCOL_CHANNELS_PXX1_PULSES:
      extmodulePulsesData.pxx.setupFrame(EXTERNAL_MODULE);
      period_ms = PXX_PULSES_PERIOD;
      break;
#endif

#if defined(PXX1) && defined(HARDWARE_EXTERNAL_MODULE_SIZE_SML)
    case PROTOCOL_CHANNELS_PXX1_SERIAL:
      extmodulePulsesData.pxx_uart.setupFrame(EXTERNAL_MODULE);
      period_ms = EXTMODULE_PXX1_SERIAL_PERIOD;
      break;
#endif

#if defined(PXX2)
    case PROTOCOL_CHANNELS_PXX2_HIGHSPEED:
    case PROTOCOL_CHANNELS_PXX2_LOWSPEED:
      extmodulePulsesData.pxx2.setupFrame(EXTERNAL_MODULE);
      period_ms = PXX2_PERIOD;
      break;
#endif

#if defined(SBUS)
    case PROTOCOL_CHANNELS_SBUS:
      setupPulsesSbus();
      period_ms = SBUS_PERIOD;
      break;
#endif

#if defined(DSM2)
    case PROTOCOL_CHANNELS_DSM2_LP45:
    case PROTOCOL_CHANNELS_DSM2_DSM2:
    case PROTOCOL_CHANNELS_DSM2_DSMX:
      setupPulsesDSM2();
      period_ms = DSM2_PERIOD;
      break;
#endif

#if defined(CROSSFIRE)
    case PROTOCOL_CHANNELS_CROSSFIRE:
      setupPulsesCrossfire();
      period_ms = CROSSFIRE_PERIOD;
      break;
#endif

#if defined(MULTIMODULE)
    case PROTOCOL_CHANNELS_MULTIMODULE:
      setupPulsesMultiExternalModule();
      period_ms = MULTIMODULE_PERIOD;
      break;
#endif

#if defined(PPM)
    case PROTOCOL_CHANNELS_PPM:
      setupPulsesPPMExternalModule();
      period_ms = PPM_PERIOD(EXTERNAL_MODULE);
      break;
#endif
    default:
      if(module) {
        module->setupFrame();
        period_ms = module->getPeriodMS();
      }
      break;
  }
  if(period_ms) {
    scheduleNextMixerCalculation(EXTERNAL_MODULE, period_ms);
    return true;
  }
  return false;
}

#if defined(HARDWARE_INTERNAL_MODULE)
static void enablePulsesInternalModule(uint8_t protocol)
{
  // start new protocol hardware here
  auto module = modules[INTERNAL_MODULE][protocol];
  switch (protocol) {
#if defined(PXX1) && !defined(INTMODULE_USART)
    case PROTOCOL_CHANNELS_PXX1_PULSES:
      intmodulePxx1PulsesStart();
      break;
#endif

#if defined(PXX1) && defined(INTMODULE_USART)
    case PROTOCOL_CHANNELS_PXX1_SERIAL:
      intmodulePxx1SerialStart();
      break;
#endif

#if defined(PXX2)
    case PROTOCOL_CHANNELS_PXX2_HIGHSPEED:
      intmoduleSerialStart(PXX2_HIGHSPEED_BAUDRATE, true, USART_Parity_No, USART_StopBits_1, USART_WordLength_8b);
#if defined(HARDWARE_INTERNAL_MODULE) && defined(INTERNAL_MODULE_PXX2) && defined(ACCESS_LIB)
      globalData.authenticationCount = 0;
#endif
      break;
#endif

#if defined(INTERNAL_MODULE_MULTI)
    case PROTOCOL_CHANNELS_MULTIMODULE:
      intmodulePulsesData.multi.initFrame();
      intmoduleSerialStart(MULTIMODULE_BAUDRATE, true, USART_Parity_Even, USART_StopBits_2, USART_WordLength_9b);
      intmoduleTimerStart(MULTIMODULE_PERIOD);
      break;
#endif
    default:
      if(module) {
        module->init();
        module->start();
      }
      break;
  }
}

bool setupPulsesInternalModule(uint8_t protocol)
{
  auto module = modules[INTERNAL_MODULE][protocol];
  switch (protocol) {
#if defined(HARDWARE_INTERNAL_MODULE) && defined(PXX1) && !defined(INTMODULE_USART)
    case PROTOCOL_CHANNELS_PXX1_PULSES:
      intmodulePulsesData.pxx.setupFrame(INTERNAL_MODULE);
      scheduleNextMixerCalculation(INTERNAL_MODULE, INTMODULE_PXX1_SERIAL_PERIOD);
      return true;
#endif

#if defined(PXX1) && defined(INTMODULE_USART)
    case PROTOCOL_CHANNELS_PXX1_SERIAL:
      intmodulePulsesData.pxx_uart.setupFrame(INTERNAL_MODULE);
#if defined(INTMODULE_HEARTBEAT)
      scheduleNextMixerCalculation(INTERNAL_MODULE, INTMODULE_PXX1_SERIAL_PERIOD + 1 /* backup */);
#else
      scheduleNextMixerCalculation(INTERNAL_MODULE, INTMODULE_PXX1_SERIAL_PERIOD);
#endif
      return true;
#endif

#if defined(PXX2)
    case PROTOCOL_CHANNELS_PXX2_HIGHSPEED:
    {
      bool result = intmodulePulsesData.pxx2.setupFrame(INTERNAL_MODULE);
      if (moduleState[INTERNAL_MODULE].mode == MODULE_MODE_SPECTRUM_ANALYSER || moduleState[INTERNAL_MODULE].mode == MODULE_MODE_POWER_METER) {
        scheduleNextMixerCalculation(INTERNAL_MODULE, PXX2_TOOLS_PERIOD);
      }
      else {
#if defined(INTMODULE_HEARTBEAT)
        scheduleNextMixerCalculation(INTERNAL_MODULE, PXX2_PERIOD + 1 /* backup */);
#else
        scheduleNextMixerCalculation(INTERNAL_MODULE, PXX2_PERIOD);
#endif
      }
      return result;
    }
#endif

#if defined(PCBTARANIS) && defined(INTERNAL_MODULE_PPM)
    case PROTOCOL_CHANNELS_PPM:
      setupPulsesPPMInternalModule();
      scheduleNextMixerCalculation(INTERNAL_MODULE, PPM_PERIOD(INTERNAL_MODULE));
      return true;
#endif

#if defined(INTERNAL_MODULE_MULTI)
    case PROTOCOL_CHANNELS_MULTIMODULE:
      setupPulsesMultiInternalModule();
      scheduleNextMixerCalculation(INTERNAL_MODULE, MULTIMODULE_PERIOD);
      return true;
#endif
    default:
      if(module) {
        module->setupFrame();
        scheduleNextMixerCalculation(INTERNAL_MODULE, module->getPeriodMS());
        return true;
      }
      else{
        scheduleNextMixerCalculation(INTERNAL_MODULE, 50/*ms*/);
        return false;
      }
  }
}

bool setupPulsesInternalModule()
{
  uint8_t protocol = getRequiredProtocol(INTERNAL_MODULE);
  auto module = modules[INTERNAL_MODULE][protocol];
  heartbeat |= (HEART_TIMER_PULSES << INTERNAL_MODULE);

  if (moduleState[INTERNAL_MODULE].protocol != protocol) {
    if(module) module->stop();
    intmoduleStop();
    moduleState[INTERNAL_MODULE].protocol = protocol;
    enablePulsesInternalModule(protocol);
    return false;
  }
  else {
    return setupPulsesInternalModule(protocol);
  }
}
#endif

bool setupPulsesExternalModule()
{
  uint8_t protocol = getRequiredProtocol(EXTERNAL_MODULE);
  auto module = modules[EXTERNAL_MODULE][protocol];
  heartbeat |= (HEART_TIMER_PULSES << EXTERNAL_MODULE);

  if (moduleState[EXTERNAL_MODULE].protocol != protocol) {
    if(module) module->stop();
    extmoduleStop();
    moduleState[EXTERNAL_MODULE].protocol = protocol;
    enablePulsesExternalModule(protocol);
    return false;
  }
  else {
    return setupPulsesExternalModule(protocol);
  }
}

void setCustomFailsafe(uint8_t moduleIndex)
{
  if (moduleIndex < NUM_MODULES) {
    for (int ch=0; ch<MAX_OUTPUT_CHANNELS; ch++) {
      if (ch < g_model.moduleData[moduleIndex].channelsStart || ch >= sentModuleChannels(moduleIndex) + g_model.moduleData[moduleIndex].channelsStart) {
        g_model.failsafeChannels[ch] = 0;
      }
      else if (g_model.failsafeChannels[ch] < FAILSAFE_CHANNEL_HOLD) {
        g_model.failsafeChannels[ch] = channelOutputs[ch];
      }
    }
  }
}

int32_t channelValue(uint8_t channel) {
  return channelOutputs[channel] + 2*PPM_CH_CENTER(channel) - 2*PPM_CENTER;
}
