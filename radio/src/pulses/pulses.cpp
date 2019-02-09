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
#include "io/pxx2.h"
#include "pulses/pxx2.h"

uint8_t s_pulses_paused = 0;
ModuleSettings moduleSettings[NUM_MODULES];
InternalModulePulsesData intmodulePulsesData __DMA;
ExternalModulePulsesData extmodulePulsesData __DMA;
TrainerPulsesData trainerPulsesData __DMA;

uint8_t getModuleType(uint8_t module)
{
  uint8_t type = g_model.moduleData[module].type;
  if (module == INTERNAL_MODULE && isInternalModuleAvailable(type)) {
    return type;
  }
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

    case MODULE_TYPE_XJT:
      protocol = PROTOCOL_CHANNELS_PXX1;
      break;

    case MODULE_TYPE_XJT2:
    case MODULE_TYPE_R9M:
      protocol = PROTOCOL_CHANNELS_PXX2;
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
      protocol = limit<uint8_t>(PROTOCOL_CHANNELS_DSM2_LP45, PROTOCOL_CHANNELS_DSM2_LP45+g_model.moduleData[module].rfProtocol, PROTOCOL_CHANNELS_DSM2_DSMX);
      // The module is set to OFF during one second before BIND start
      {
        static tmr10ms_t bindStartTime = 0;
        if (moduleSettings[module].mode == MODULE_MODE_BIND) {
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

    default:
      protocol = PROTOCOL_CHANNELS_NONE;
      break;
  }

  if (s_pulses_paused) {
    protocol = PROTOCOL_CHANNELS_NONE;
  }

#if 0
  // will need an EEPROM conversion
  if (moduleSettings[module].mode == MODULE_OFF) {
    protocol = PROTOCOL_CHANNELS_NONE;
  }
#endif

  return protocol;
}

void disablePulses(uint8_t module, uint8_t protocol)
{
  // stop existing protocol hardware

  switch (protocol) {
    case PROTOCOL_CHANNELS_PXX1:
      disable_pxx(module);
      break;

#if defined(DSM2)
    case PROTOCOL_CHANNELS_DSM2_LP45:
    case PROTOCOL_CHANNELS_DSM2_DSM2:
    case PROTOCOL_CHANNELS_DSM2_DSMX:
      disable_serial(module);
      break;
#endif

#if defined(CROSSFIRE)
    case PROTOCOL_CHANNELS_CROSSFIRE:
      // TODO disable_module_timer(module);
      break;
#endif

    case PROTOCOL_CHANNELS_PXX2:
      disable_pxx2(module);
      break;

#if defined(MULTIMODULE)
      case PROTOCOL_CHANNELS_MULTIMODULE:
#endif
    case PROTOCOL_CHANNELS_SBUS:
      disable_serial(module);
      break;

    case PROTOCOL_CHANNELS_PPM:
      disable_ppm(module);
      break;
  }
}

void enablePulses(uint8_t module, uint8_t protocol)
{
  // start new protocol hardware here

  switch (protocol) {
    case PROTOCOL_CHANNELS_PXX1:
      init_pxx(module);
      break;

#if defined(DSM2)
    case PROTOCOL_CHANNELS_DSM2_LP45:
    case PROTOCOL_CHANNELS_DSM2_DSM2:
    case PROTOCOL_CHANNELS_DSM2_DSMX:
      init_serial(module, DSM2_BAUDRATE, DSM2_PERIOD * 2000);
      break;
#endif

#if defined(CROSSFIRE)
    case PROTOCOL_CHANNELS_CROSSFIRE:
      // TODO init_module_timer(module, CROSSFIRE_PERIOD, true);
      break;
#endif

    case PROTOCOL_CHANNELS_PXX2:
      init_pxx2(module);
      break;

#if defined(MULTIMODULE)
    case PROTOCOL_CHANNELS_MULTIMODULE:
      init_serial(module, MULTIMODULE_BAUDRATE, MULTIMODULE_PERIOD * 2000);
      break;
#endif

    case PROTOCOL_CHANNELS_SBUS:
      init_serial(module, SBUS_BAUDRATE, SBUS_PERIOD_HALF_US);
      break;

    case PROTOCOL_CHANNELS_PPM:
      init_ppm(module);
      break;
  }
}

#if defined(PXX1)
void setupPulsesPXXInternalModule()
{
#if defined(INTMODULE_USART)
  intmodulePulsesData.pxx_uart.setupFrame(INTERNAL_MODULE);
#else
  intmodulePulsesData.pxx.setupFrame(INTERNAL_MODULE);
#endif
}

void setupPulsesPXXExternalModule()
{
#if defined(EXTMODULE_USART)
  extmodulePulsesData.pxx_uart.setupFrame(EXTERNAL_MODULE);
#elif !defined(INTMODULE_USART) && !defined(EXTMODULE_USART)
  extmodulePulsesData.pxx.setupFrame(EXTERNAL_MODULE);
#endif
}
#endif

void setupPulsesInternalModule(uint8_t protocol)
{
  switch (protocol) {
#if defined(PXX1)
    case PROTOCOL_CHANNELS_PXX1:
      setupPulsesPXXInternalModule();
      scheduleNextMixerCalculation(INTERNAL_MODULE, PXX_PERIOD);
      break;
#endif

#if defined(PXX2)
    case PROTOCOL_CHANNELS_PXX2:
      intmodulePulsesData.pxx2.setupFrame(INTERNAL_MODULE);
      scheduleNextMixerCalculation(INTERNAL_MODULE, moduleSettings[INTERNAL_MODULE].mode == MODULE_MODE_SPECTRUM_ANALYSER ? 1 : PXX2_PERIOD);
      break;
#endif

#if defined(PCBSKY9X) || defined(TARANIS_INTERNAL_PPM)
    case PROTOCOL_CHANNELS_PPM:
      setupPulsesPPM(&extmodulePulsesData.ppm, g_model.moduleData[INTERNAL_MODULE].channelsStart, g_model.moduleData[INTERNAL_MODULE].channelsCount, g_model.moduleData[INTERNAL_MODULE].ppm.frameLength);
      scheduleNextMixerCalculation(INTERNAL_MODULE, PPM_PERIOD(INTERNAL_MODULE));
      break;
#endif

    default:
      break;
  }
}

void setupPulsesExternalModule(uint8_t protocol)
{
  switch (protocol) {
#if defined(PXX1)
    case PROTOCOL_CHANNELS_PXX1:
      setupPulsesPXXExternalModule();
      scheduleNextMixerCalculation(EXTERNAL_MODULE, PXX_PERIOD);
      break;
#endif

#if defined(PXX2)
    case PROTOCOL_CHANNELS_PXX2:
      extmodulePulsesData.pxx2.setupFrame(EXTERNAL_MODULE);
      scheduleNextMixerCalculation(EXTERNAL_MODULE, PXX2_PERIOD);
      break;
#endif

    case PROTOCOL_CHANNELS_SBUS:
      setupPulsesSbus();
      scheduleNextMixerCalculation(EXTERNAL_MODULE, SBUS_PERIOD);
      break;

#if defined(DSM2)
    case PROTOCOL_CHANNELS_DSM2_LP45:
    case PROTOCOL_CHANNELS_DSM2_DSM2:
    case PROTOCOL_CHANNELS_DSM2_DSMX:
      setupPulsesDSM2();
      scheduleNextMixerCalculation(EXTERNAL_MODULE, DSM2_PERIOD);
      break;
#endif

#if defined(CROSSFIRE)
    case PROTOCOL_CHANNELS_CROSSFIRE:
      setupPulsesCrossfire();
      scheduleNextMixerCalculation(EXTERNAL_MODULE, CROSSFIRE_PERIOD);
      break;
#endif

#if defined(MULTIMODULE)
    case PROTOCOL_CHANNELS_MULTIMODULE:
      setupPulsesMultimodule();
      scheduleNextMixerCalculation(EXTERNAL_MODULE, MULTIMODULE_PERIOD);
      break;
#endif

    case PROTOCOL_CHANNELS_PPM:
      setupPulsesPPMExternalModule();
      scheduleNextMixerCalculation(EXTERNAL_MODULE, PPM_PERIOD(EXTERNAL_MODULE));
      break;

    default:
      break;
  }
}

void setupPulses(uint8_t module, uint8_t protocol)
{
  switch (module) {
    case INTERNAL_MODULE:
      setupPulsesInternalModule(protocol);
      break;

    case EXTERNAL_MODULE:
      setupPulsesExternalModule(protocol);
      break;
  }
}

bool setupPulses(uint8_t module)
{
  uint8_t protocol = getRequiredProtocol(module);

  heartbeat |= (HEART_TIMER_PULSES << module);

  if (moduleSettings[module].protocol != protocol) {
    disablePulses(module, moduleSettings[module].protocol);
    moduleSettings[module].protocol = protocol;
    enablePulses(module, protocol);
    return false;
  }
  else {
    setupPulses(module, protocol);
    return true;
  }
}

void setCustomFailsafe(uint8_t moduleIndex)
{
  if (moduleIndex < NUM_MODULES) {
    for (int ch=0; ch<MAX_OUTPUT_CHANNELS; ch++) {
      if (ch < g_model.moduleData[moduleIndex].channelsStart || ch >= sentModuleChannels(moduleIndex) + g_model.moduleData[moduleIndex].channelsStart) {
        g_model.moduleData[moduleIndex].failsafeChannels[ch] = 0;
      }
      else if (g_model.moduleData[moduleIndex].failsafeChannels[ch] < FAILSAFE_CHANNEL_HOLD) {
        g_model.moduleData[moduleIndex].failsafeChannels[ch] = channelOutputs[ch];
      }
    }
  }
}
