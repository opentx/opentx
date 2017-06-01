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

/*
 * ============================================================
 * Templates file
 *
 * eccpm
 * crow
 * throttle cut
 * flaperon
 * elevon
 * v-tail
 * throttle hold
 * Aileron Differential
 * Spoilers
 * Snap Roll
 * ELE->Flap
 * Flap->ELE
 *
 */

#ifndef _TEMPLATES_H_
#define _TEMPLATES_H_

#include <inttypes.h>

#define STK_RUD  1
#define STK_ELE  2
#define STK_THR  3
#define STK_AIL  4
#define STK_P1   5
#define STK_P2   6
#define STK_P3   7

#define CV(x) (CURVE_BASE+(x)-1)
#define CC(x) (MIXSRC_Rud - 1 + channel_order(x)) // need to invert this to work with dest

#if defined(PCBTARANIS)
  #define mixSetCurve(mix, idx) mix->curve.type=CURVE_REF_CUSTOM; mix->curve.value=idx
#else
  #define mixSetCurve(mix, idx) mix->curveMode=MODE_CURVE; mix->curveParam=CURVE_BASE+idx
#endif

enum Templates {
  TMPL_CLEAR_MIXES,
  TMPL_SIMPLE_4CH,
  TMPL_STI_THR_CUT,
  TMPL_V_TAIL,
  TMPL_ELEVON_DELTA,
  TMPL_ECCPM,
#if defined(HELI) && defined(CURVES)
  TMPL_HELI_SETUP,
#endif
  TMPL_SERVO_TEST,
  TMPL_COUNT
};

void clearMixes();
void clearCurves();
void applyTemplate(uint8_t idx);

#endif // _TEMPLATES_H_


