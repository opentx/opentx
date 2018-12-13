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
uint8_t s_current_protocol[NUM_MODULES] = { MODULES_INIT(255) };
uint16_t failsafeCounter[NUM_MODULES] = { MODULES_INIT(100) };
uint8_t moduleFlag[NUM_MODULES] = { 0 };

ModulePulsesData modulePulsesData[NUM_MODULES] __DMA;
TrainerPulsesData trainerPulsesData __DMA;

#if defined(CROSSFIRE)
uint8_t createCrossfireChannelsFrame(uint8_t * frame, int16_t * pulses);
#endif

uint8_t getRequiredProtocol(uint8_t port)
{
  uint8_t required_protocol;

  switch (port) {
#if defined(PCBTARANIS) || defined(PCBHORUS)
    case INTERNAL_MODULE:
      switch (g_model.moduleData[INTERNAL_MODULE].type) {
#if defined(TARANIS_INTERNAL_PPM)
        case MODULE_TYPE_PPM:
          required_protocol = PROTOCOL_CHANNELS_PPM;
          break;
#endif
        case MODULE_TYPE_XJT:
          required_protocol = PROTOCOL_CHANNELS_PXX;
          break;
        default:
          required_protocol = PROTOCOL_CHANNELS_NONE;
          break;
      }
      break;
#endif

    default:
      switch (g_model.moduleData[EXTERNAL_MODULE].type) {
        case MODULE_TYPE_PPM:
          required_protocol = PROTOCOL_CHANNELS_PPM;
          break;
        case MODULE_TYPE_XJT:
          required_protocol = PROTOCOL_CHANNELS_PXX;
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
          required_protocol = limit<uint8_t>(PROTOCOL_CHANNELS_DSM2_LP45, PROTOCOL_CHANNELS_DSM2_LP45+g_model.moduleData[EXTERNAL_MODULE].rfProtocol, PROTOCOL_CHANNELS_DSM2_DSMX);
          // The module is set to OFF during one second before BIND start
          {
            static tmr10ms_t bindStartTime = 0;
            if (moduleFlag[EXTERNAL_MODULE] == MODULE_BIND) {
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
      break;
  }

  if (s_pulses_paused) {
    required_protocol = PROTOCOL_CHANNELS_NONE;
  }

#if 0
  // will need an EEPROM conversion
  if (moduleFlag[port] == MODULE_OFF) {
    required_protocol = PROTOCOL_CHANNELS_NONE;
  }
#endif

  return required_protocol;
}

void setupPulsesPXX(uint8_t port)
{
#if defined(INTMODULE_USART) && defined(EXTMODULE_USART)
  modulePulsesData[port].pxx_uart.setupFrame(port);
#elif !defined(INTMODULE_USART) && !defined(EXTMODULE_USART)
  modulePulsesData[port].pxx.setupFrame(port);
#else
  if (IS_UART_MODULE(port))
    modulePulsesData[port].pxx_uart.setupFrame(port);
  else
    modulePulsesData[port].pxx.setupFrame(port);
#endif
}

void setupPulses(uint8_t port)
{
  bool init_needed = false;
  uint8_t required_protocol = getRequiredProtocol(port);

  heartbeat |= (HEART_TIMER_PULSES << port);

  if (s_current_protocol[port] != required_protocol) {
    init_needed = true;
    switch (s_current_protocol[port]) { // stop existing protocol hardware
      case PROTOCOL_CHANNELS_PXX:
        disable_pxx(port);
        break;

#if defined(DSM2)
      case PROTOCOL_CHANNELS_DSM2_LP45:
      case PROTOCOL_CHANNELS_DSM2_DSM2:
      case PROTOCOL_CHANNELS_DSM2_DSMX:
        disable_serial(port);
        break;
#endif

#if defined(CROSSFIRE)
      case PROTOCOL_CHANNELS_CROSSFIRE:
        disable_module_timer(port);
        break;
#endif

      case PROTOCOL_CHANNELS_PXX2:
        disable_module_timer(port);
        break;

#if defined(MULTIMODULE)
      case PROTOCOL_CHANNELS_MULTIMODULE:
#endif
      case PROTOCOL_CHANNELS_SBUS:
        disable_serial(port);
        break;

      case PROTOCOL_CHANNELS_PPM:
        disable_ppm(port);
        break;

      default:
        disable_no_pulses(port);
        break;
    }
    s_current_protocol[port] = required_protocol;
  }

  // Set up output data here
  switch (required_protocol) {
    case PROTOCOL_CHANNELS_PXX:
      setupPulsesPXX(port);
      scheduleNextMixerCalculation(port, PXX_PERIOD);
      break;

    case PROTOCOL_CHANNELS_SBUS:
      setupPulsesSbus(port);
      scheduleNextMixerCalculation(port, SBUS_PERIOD);
      break;

#if defined(DSM2)
    case PROTOCOL_CHANNELS_DSM2_LP45:
    case PROTOCOL_CHANNELS_DSM2_DSM2:
    case PROTOCOL_CHANNELS_DSM2_DSMX:
      setupPulsesDSM2(port);
      scheduleNextMixerCalculation(port, DSM2_PERIOD);
      break;
#endif

#if defined(CROSSFIRE)
    case PROTOCOL_CHANNELS_CROSSFIRE:
      if (telemetryProtocol == PROTOCOL_TELEMETRY_CROSSFIRE && !init_needed) {
        uint8_t * crossfire = modulePulsesData[port].crossfire.pulses;
        uint8_t len;
#if defined(LUA)
        if (outputTelemetryBufferTrigger != 0x00 && outputTelemetryBufferSize > 0) {
          memcpy(crossfire, outputTelemetryBuffer, outputTelemetryBufferSize);
          len = outputTelemetryBufferSize;
          outputTelemetryBufferTrigger = 0x00;
          outputTelemetryBufferSize = 0;
        }
        else
#endif
        {
          len = createCrossfireChannelsFrame(crossfire, &channelOutputs[g_model.moduleData[port].channelsStart]);
        }
        sportSendBuffer(crossfire, len);
      }
      scheduleNextMixerCalculation(port, CROSSFIRE_PERIOD);
      break;
#endif

    case PROTOCOL_CHANNELS_PXX2:
      // here we have to wait that telemetryInit() is called, hence this test
      if (telemetryProtocol == PROTOCOL_TELEMETRY_PXX2 && !init_needed) {
        modulePulsesData[port].pxx2.setupFrame(port);
        sportSendBuffer(modulePulsesData[port].pxx2.getData(), modulePulsesData[port].pxx2.getSize());
      }
      scheduleNextMixerCalculation(port, PXX2_PERIOD);
      break;

#if defined(MULTIMODULE)
    case PROTOCOL_CHANNELS_MULTIMODULE:
      setupPulsesMultimodule(port);
      scheduleNextMixerCalculation(port, MULTIMODULE_PERIOD);
      break;
#endif

    case PROTOCOL_CHANNELS_PPM:
#if defined(PCBSKY9X)
    case PROTOCOL_CHANNELS_NONE:
#endif
      setupPulsesPPMModule(port);
      scheduleNextMixerCalculation(port, PPM_PERIOD(port));
      break;

    default:
      break;
  }

  if (init_needed) {
    switch (required_protocol) { // Start new protocol hardware here
      case PROTOCOL_CHANNELS_PXX:
        init_pxx(port);
        break;

#if defined(DSM2)
      case PROTOCOL_CHANNELS_DSM2_LP45:
      case PROTOCOL_CHANNELS_DSM2_DSM2:
      case PROTOCOL_CHANNELS_DSM2_DSMX:
        init_serial(port, DSM2_BAUDRATE, DSM2_PERIOD * 2000);
        break;
#endif

#if defined(CROSSFIRE)
      case PROTOCOL_CHANNELS_CROSSFIRE:
        init_module_timer(port, CROSSFIRE_PERIOD, true);
        break;
#endif

      case PROTOCOL_CHANNELS_PXX2:
        init_module_timer(port, PXX2_PERIOD, true);
        break;

#if defined(MULTIMODULE)
      case PROTOCOL_CHANNELS_MULTIMODULE:
        init_serial(port, MULTIMODULE_BAUDRATE, MULTIMODULE_PERIOD * 2000);
        break;
#endif

      case PROTOCOL_CHANNELS_SBUS:
        init_serial(port, SBUS_BAUDRATE, SBUS_PERIOD_HALF_US);
        break;

      case PROTOCOL_CHANNELS_PPM:
        init_ppm(port);
        break;

      default:
        init_no_pulses(port);
        break;
    }
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
