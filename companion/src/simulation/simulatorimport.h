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

#include <stdint.h>

#if !defined(MAX_LOGICAL_SWITCHES) && defined(NUM_CSW)
#define MAX_LOGICAL_SWITCHES NUM_CSW
#endif

#ifdef INIT_IMPORT
#undef INIT_IMPORT
#ifdef TELEMETRY_FRSKY
telemetryStreaming = 20;
#endif
#endif

#ifdef SETVALUES_IMPORT
#undef SETVALUES_IMPORT
for (int i=0; i<NUM_STICKS; i++)
  g_anas[i] = inputs.sticks[i];
for (int i=0; i<NUM_POTS+NUM_SLIDERS; i++)
  g_anas[NUM_STICKS+i] = inputs.pots[i];
for (int i=0; i<CPN_MAX_SWITCHES; i++)
  simuSetSwitch(i, inputs.switches[i]);
for (int i=0; i<CPN_MAX_KEYS; i++)
  simuSetKey(i, inputs.keys[i]);
for (int i=0; i<(NUM_STICKS+NUM_AUX_TRIMS)*2; i++)
  simuSetTrim(i, inputs.trims[i]);

#if defined(PCBGRUVIN9X)
// rotary encoders
pind = 0;
if (inputs.rotenc) pind |= 0x20;
#endif

#if defined(PCBSKY9X)
if (inputs.rotenc) PIOB->PIO_PDSR &= ~0x40; else PIOB->PIO_PDSR |= 0x40;
#endif

#if defined(PCBFLAMENCO) || defined(PCBX9E)
if (inputs.rotenc) simuSetKey(KEY_ENTER, true);
#endif
#endif

#ifdef GETVALUES_IMPORT
#undef GETVALUES_IMPORT
memset(outputs.chans, 0, sizeof(outputs.chans));
for (unsigned int i=0; i<DIM(g_chans512); i++)
  outputs.chans[i] = g_chans512[i];
for (int i=0; i<MAX_LOGICAL_SWITCHES; i++)
#if defined(BOLD_FONT)
  outputs.vsw[i] = getSwitch(SWSRC_SW1+i);
#else
  outputs.vsw[i] = getSwitch(SWSRC_SW1+i, 0);
#endif
#ifdef GVAR_VALUE // defined(GVARS)
/* TODO it could be a good idea instead of getPhase() / getPhaseName() outputs.phase = getFlightMode(); */
#if defined(GVARS)
for (int fm=0; fm<MAX_FLIGHT_MODES; fm++) {
  for (int gv=0; gv<MAX_GVARS; gv++) {
    outputs.gvars[fm][gv] = GVAR_VALUE(gv, getGVarFlightMode(fm, gv));
  }
}
#endif
#endif
#endif

#ifdef LCDCHANGED_IMPORT
#undef LCDCHANGED_IMPORT
if (simuLcdRefresh) {
  lightEnable = isBacklightEnabled();
  simuLcdRefresh = false;
  return true;
}
return false;
#endif

#ifdef TIMER10MS_IMPORT
#undef TIMER10MS_IMPORT
if (!main_thread_running)
  return false;
per10ms();
return true;
#endif

#ifdef GETERROR_IMPORT
#undef GETERROR_IMPORT
return main_thread_error;
#endif

#ifdef SETTRAINER_IMPORT
#undef SETTRAINER_IMPORT
  ppmInputValidityTimer = 100;
  ppmInput[inputNumber] = LIMIT< ::int16_t>(-512, value, 512);
#endif

