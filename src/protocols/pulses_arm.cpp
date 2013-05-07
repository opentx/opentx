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

#include "opentx.h"

uint8_t s_pulses_paused = 0;
uint8_t s_current_protocol[NUM_MODULES] = { MODULES_INIT(255) };
uint32_t failsafeCounter[NUM_MODULES] = { MODULES_INIT(100) };

void setupPulses(unsigned int port)
{
  heartbeat |= (HEART_TIMER_PULSES << port);

#if defined(PCBTARANIS)
  if (port) port = 1;			// Ensure is 0 or 1 only
  uint8_t required_protocol;
  
  if (port == INTERNAL_MODULE) {
    required_protocol = g_model.moduleData[INTERNAL_MODULE].rfProtocol == RF_PROTO_OFF ? PROTO_NONE : PROTO_PXX;
  }
  else {
    switch (g_model.externalModule) {
      case MODULE_TYPE_PPM:
        required_protocol = PROTO_PPM;
        break;
      case MODULE_TYPE_XJT:
      case MODULE_TYPE_DJT:
      	required_protocol = PROTO_PXX;
        break;
      default:
        required_protocol = PROTO_NONE;
        break;
    }
  }
#else
  uint8_t required_protocol = g_model.protocol;
#endif

  if (s_pulses_paused)
    required_protocol = PROTO_NONE;

  if (s_current_protocol[port] != required_protocol) {

    switch (s_current_protocol[port]) { // stop existing protocol hardware
      case PROTO_PXX:
#if defined(PCBTARANIS)
        disable_pxx(port);
#else
        disable_pxx();
#endif
        break;
#if defined(DSM2)
      case PROTO_DSM2_LP45:
      case PROTO_DSM2_DSM2:
      case PROTO_DSM2_DSMX:
        disable_ssc();
        break;
#endif
      default:
#if defined(PCBTARANIS)
        disable_ppm(port);
#else
        disable_main_ppm();
#endif
        break;
    }

    s_current_protocol[port] = required_protocol;

#if defined(PCBTARANIS)
    switch (required_protocol) {
      case PROTO_PXX:
        init_pxx(port);
        break;
      case PROTO_PPM:
        init_ppm(port);
        break;
    }
#elif defined(PCBSKY9X)
    switch (required_protocol) { // Start new protocol hardware here
      case PROTO_PXX:
        init_main_ppm(5000, 0); // Initial period 2.5 mS, output off
        init_ssc();
        break;
      case PROTO_DSM2_LP45:
      case PROTO_DSM2_DSM2:
      case PROTO_DSM2_DSMX:
        init_main_ppm(5000, 0); // Initial period 2.5 mS, output off
        init_ssc();
        break;
      case PROTO_NONE:
        init_main_ppm(3000, 0); // Initial period 1.5 mS, output off
        break;
      default:
        init_main_ppm(3000, 1); // Initial period 1.5 mS, output on
        break;
    }
#endif
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
      setupPulsesDsm2(6);
      break;
#endif
#if defined(PCBTARANIS)
    case PROTO_PPM:
#else
    default:
#endif
      setupPulsesPPM(port); // Don't enable interrupts through here
      break ;
  }
}
