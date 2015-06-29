/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "../opentx.h"

uint8_t s_pulses_paused = 0;
uint8_t s_current_protocol[NUM_MODULES] = { MODULES_INIT(255) };
uint16_t failsafeCounter[NUM_MODULES] = { MODULES_INIT(100) };
uint8_t moduleFlag[NUM_MODULES] = { 0 };

ModulePulsesData modulePulsesData[NUM_MODULES] _NOCCM;
TrainerPulsesData trainerPulsesData _NOCCM;

void setupPulses(unsigned int port)
{
  uint8_t required_protocol;

  heartbeat |= (HEART_TIMER_PULSES << port);

  switch (port) {
#if defined(PCBTARANIS)
    case INTERNAL_MODULE:
  #if defined(TARANIS_INTERNAL_PPM)
      switch (g_model.moduleData[INTERNAL_MODULE].type) {
        case MODULE_TYPE_PPM:
          required_protocol = PROTO_PPM;
          break;
        case MODULE_TYPE_XJT:
          required_protocol = PROTO_PXX;
          break;
        default:
          required_protocol = PROTO_NONE;
          break;
      }
  #else
      required_protocol = g_model.moduleData[INTERNAL_MODULE].rfProtocol == RF_PROTO_OFF ? PROTO_NONE : PROTO_PXX;
  #endif
      break;
#endif

    default:
      port = EXTERNAL_MODULE; // ensure it's external module only
      switch (g_model.moduleData[EXTERNAL_MODULE].type) {
        case MODULE_TYPE_PPM:
          required_protocol = PROTO_PPM;
          break;
        case MODULE_TYPE_XJT:
          required_protocol = PROTO_PXX;
          break;
#if defined(PCBTARANIS) && defined(DSM2)
        case MODULE_TYPE_DSM2:
          required_protocol = limit<uint8_t>(PROTO_DSM2_LP45, PROTO_DSM2_LP45+g_model.moduleData[EXTERNAL_MODULE].rfProtocol, PROTO_DSM2_DSMX);
          // The module is set to OFF during one second before BIND start
          {
            static tmr10ms_t bindStartTime = 0;
            if (moduleFlag[EXTERNAL_MODULE] == MODULE_BIND) {
              if (bindStartTime == 0) bindStartTime = get_tmr10ms();
              if ((tmr10ms_t)(get_tmr10ms() - bindStartTime) < 100) {
                required_protocol = PROTO_NONE;
                break;
              }
            }
            else {
              bindStartTime = 0;
            }
          }
          break;
#endif
        default:
          required_protocol = PROTO_NONE;
          break;
      }
      break;
  }

  if (s_pulses_paused) {
    required_protocol = PROTO_NONE;
  }

  if (moduleFlag[port] == MODULE_OFF) {
    required_protocol = PROTO_NONE;
  }

  if (s_current_protocol[port] != required_protocol) {

    switch (s_current_protocol[port]) { // stop existing protocol hardware
      case PROTO_PXX:
        disable_pxx(port);
        break;
#if defined(DSM2)
      case PROTO_DSM2_LP45:
      case PROTO_DSM2_DSM2:
      case PROTO_DSM2_DSMX:
        disable_dsm2(port);
        break;
#endif
      case PROTO_PPM:
        disable_ppm(port);
        break;
      default:
        disable_no_pulses(port);
        break;
    }

    s_current_protocol[port] = required_protocol;

    switch (required_protocol) { // Start new protocol hardware here
      case PROTO_PXX:
        init_pxx(port);
        break;
#if defined(DSM2)
      case PROTO_DSM2_LP45:
      case PROTO_DSM2_DSM2:
      case PROTO_DSM2_DSMX:
        init_dsm2(port);
        break;
#endif
      case PROTO_PPM:
        init_ppm(port);
        break;
      default:
        init_no_pulses(port);
        break;
    }
  }

  // Set up output data here
  switch (required_protocol) {
    case PROTO_PXX:
      setupPulsesPXX(port);
      break;
#if defined(DSM2)
    case PROTO_DSM2_LP45:
    case PROTO_DSM2_DSM2:
    case PROTO_DSM2_DSMX:
      setupPulsesDSM2(port);
      break;
#endif
    case PROTO_PPM:
      setupPulsesPPM(port);
      break ;
    default:
      break;
  }
}
