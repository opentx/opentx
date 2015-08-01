/*
 * Author - Bertrand Songis <bsongis@gmail.com>
 * 
 * Based on th9x -> http://code.google.com/p/th9x/
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

#include <stdint.h>

#if !defined(NUM_LOGICAL_SWITCH) && defined(NUM_CSW)
#define NUM_LOGICAL_SWITCH NUM_CSW
#endif

#ifdef INIT_IMPORT
#undef INIT_IMPORT
#ifdef FRSKY
frskyStreaming = 20;
#endif
#endif

#ifdef SETVALUES_IMPORT
#undef SETVALUES_IMPORT
for (int i=0; i<NUM_STICKS; i++)
  g_anas[i] = inputs.sticks[i];
for (int i=0; i<NUM_POTS; i++)
  g_anas[NUM_STICKS+i] = inputs.pots[i];
for (int i=0; i<C9X_NUM_SWITCHES; i++)
  simuSetSwitch(i, inputs.switches[i]);
for (int i=0; i<C9X_NUM_KEYS; i++)
  simuSetKey(i, inputs.keys[i]);
for (int i=0; i<NUM_STICKS*2; i++)
  simuSetTrim(i, inputs.trims[i]);

#ifdef PCBGRUVIN9X
// rotary encoders
pind = 0;
if (inputs.rotenc) pind |= 0x20;
#endif

#if defined(PCBSKY9X)
if (inputs.rotenc) PIOB->PIO_PDSR &= ~0x40; else PIOB->PIO_PDSR |= 0x40;
#endif

#if defined(PCBTARANIS) && defined(REV9E)
if (inputs.rotenc) simuSetKey(KEY_ENTER, true);
#endif
#endif

#ifdef GETVALUES_IMPORT
#undef GETVALUES_IMPORT
memset(outputs.chans, 0, sizeof(outputs.chans));
for (unsigned int i=0; i<DIM(g_chans512); i++)
  outputs.chans[i] = g_chans512[i];
for (int i=0; i<NUM_LOGICAL_SWITCH; i++)
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
    outputs.gvars[fm][gv] = GVAR_VALUE(gv, getGVarFlightPhase(fm, gv));
  }
}
#endif
#endif
#endif   //GETVALUES_IMPORT

#ifdef LCDCHANGED_IMPORT
#undef LCDCHANGED_IMPORT
if (lcd_refresh) {
  lightEnable = isBacklightEnable();
  lcd_refresh = false;
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

#ifdef GETLCD_IMPORT
#undef GETLCD_IMPORT
return (::uint8_t *)lcd_buf;
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

