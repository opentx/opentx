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
ModulePulsesData modulePulsesData[NUM_MODULES] __DMA;
TrainerPulsesData trainerPulsesData __DMA;

#if defined(CROSSFIRE)
uint8_t createCrossfireChannelsFrame(uint8_t * frame, int16_t * pulses);
#endif

uint8_t getRequiredProtocol(uint8_t module)
{
  uint8_t required_protocol;

  switch (g_model.moduleData[module].type) {
    case MODULE_TYPE_PPM:
      required_protocol = PROTOCOL_CHANNELS_PPM;
      break;
    case MODULE_TYPE_XJT:
      required_protocol = PROTOCOL_CHANNELS_PXX;
      break;
    case MODULE_TYPE_XJT2:
      required_protocol = PROTOCOL_CHANNELS_PXX2;
      break;
    case MODULE_TYPE_R9M:
#if defined(PCBXLITE)
      required_protocol = PROTOCOL_CHANNELS_PXX;
#else
      required_protocol = PROTOCOL_CHANNELS_PXX2;
#endif
      break;
    case MODULE_TYPE_SBUS:
      required_protocol = PROTOCOL_CHANNELS_SBUS;
      break;
#if defined(MULTIMODULE)
    case MODULE_TYPE_MULTIMODULE:
      required_protocol = PROTOCOL_CHANNELS_MULTIMODULE;
      break;
#endif
#if defined(DSM2)
    case MODULE_TYPE_DSM2:
      required_protocol = limit<uint8_t>(PROTOCOL_CHANNELS_DSM2_LP45, PROTOCOL_CHANNELS_DSM2_LP45+g_model.moduleData[module].rfProtocol, PROTOCOL_CHANNELS_DSM2_DSMX);
      // The module is set to OFF during one second before BIND start
      {
        static tmr10ms_t bindStartTime = 0;
        if (moduleSettings[module].mode == MODULE_MODE_BIND) {
          if (bindStartTime == 0) bindStartTime = get_tmr10ms();
          if ((tmr10ms_t)(get_tmr10ms() - bindStartTime) < 100) {
            required_protocol = PROTOCOL_CHANNELS_NONE;
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
      required_protocol = PROTOCOL_CHANNELS_CROSSFIRE;
      break;
#endif
    default:
      required_protocol = PROTOCOL_CHANNELS_NONE;
      break;
  }

  if (s_pulses_paused) {
    required_protocol = PROTOCOL_CHANNELS_NONE;
  }

#if 0
  // will need an EEPROM conversion
  if (moduleSettings[module].mode == MODULE_OFF) {
    required_protocol = PROTOCOL_CHANNELS_NONE;
  }
#endif

  return required_protocol;
}

void setupPulsesPXX(uint8_t module)
{
#if defined(INTMODULE_USART) && defined(EXTMODULE_USART)
  modulePulsesData[module].pxx_uart.setupFrame(module);
#elif !defined(INTMODULE_USART) && !defined(EXTMODULE_USART)
  modulePulsesData[module].pxx.setupFrame(module);
#else
  if (IS_UART_MODULE(module))
    modulePulsesData[module].pxx_uart.setupFrame(module);
  else
    modulePulsesData[module].pxx.setupFrame(module);
#endif
}

void setupPulsesPXX2(uint8_t module)
{
  modulePulsesData[module].pxx2.setupFrame(module);

#if 0
  // here we have to wait that telemetryInit() is called, hence this test
  if (telemetryProtocol == PROTOCOL_TELEMETRY_PXX2) {
    modulePulsesData[module].pxx2.setupFrame(module);
    sportSendBuffer(modulePulsesData[module].pxx2.getData(), modulePulsesData[module].pxx2.getSize());
  }
#endif
}

void disablePulses(uint8_t module, uint8_t protocol)
{
  // stop existing protocol hardware

  switch (protocol) {
    case PROTOCOL_CHANNELS_PXX:
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

  #warning "CHECK THAT ALL PROTOCOL INIT WON'T SEND A FIRST WRONG FRAME HERE"

  switch (protocol) {
    case PROTOCOL_CHANNELS_PXX:
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

void setupPulses(uint8_t module, uint8_t protocol)
{
  switch (protocol) {
    case PROTOCOL_CHANNELS_PXX:
      setupPulsesPXX(module);
      scheduleNextMixerCalculation(module, PXX_PERIOD);
      break;

    case PROTOCOL_CHANNELS_SBUS:
      setupPulsesSbus(module);
      scheduleNextMixerCalculation(module, SBUS_PERIOD);
      break;

#if defined(DSM2)
    case PROTOCOL_CHANNELS_DSM2_LP45:
    case PROTOCOL_CHANNELS_DSM2_DSM2:
    case PROTOCOL_CHANNELS_DSM2_DSMX:
      setupPulsesDSM2(module);
      scheduleNextMixerCalculation(module, DSM2_PERIOD);
      break;
#endif

#if defined(CROSSFIRE)
    case PROTOCOL_CHANNELS_CROSSFIRE:
      setupPulsesCrossfire(module);
      scheduleNextMixerCalculation(module, CROSSFIRE_PERIOD);
      break;
#endif

    case PROTOCOL_CHANNELS_PXX2:
      setupPulsesPXX2(module);
      scheduleNextMixerCalculation(module, moduleSettings[module].mode == MODULE_MODE_SPECTRUM_ANALYSER ? 1 : PXX2_PERIOD);
      break;

#if defined(MULTIMODULE)
    case PROTOCOL_CHANNELS_MULTIMODULE:
      setupPulsesMultimodule(module);
      scheduleNextMixerCalculation(module, MULTIMODULE_PERIOD);
      break;
#endif

    case PROTOCOL_CHANNELS_PPM:
#if defined(PCBSKY9X)
      case PROTOCOL_CHANNELS_NONE:
#endif
      setupPulsesPPMModule(module);
      scheduleNextMixerCalculation(module, PPM_PERIOD(module));
      break;

    default:
      break;
  }
}

bool setupPulses(uint8_t module)
{
  uint8_t required_protocol = getRequiredProtocol(module);

  heartbeat |= (HEART_TIMER_PULSES << module);

  if (moduleSettings[module].protocol != required_protocol) {
    disablePulses(module, moduleSettings[module].protocol);
    moduleSettings[module].protocol = required_protocol;
    enablePulses(module, required_protocol);
    return false;
  }
  else {
    setupPulses(module, required_protocol);
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
