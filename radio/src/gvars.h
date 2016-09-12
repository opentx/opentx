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

#ifndef _GVARS_H_
#define _GVARS_H_

#if defined(PCBSTD)
  // GVars are common to all flight modes
  #define GVAR_VALUE(x, p)             g_model.gvars[x]
  #define SET_GVAR_VALUE(idx, phase, value) \
    (GVAR_VALUE(idx, phase) = value, storageDirty(EE_MODEL))
#else
  // GVars have one value per flight mode
  #define GVAR_VALUE(gv, fm)           g_model.flightModeData[fm].gvars[gv]
  #define SET_GVAR_VALUE(idx, phase, value) \
    GVAR_VALUE(idx, phase) = value; \
    storageDirty(EE_MODEL); \
    if (g_model.gvars[idx].popup) { \
      gvarLastChanged = idx; \
      gvarDisplayTimer = GVAR_DISPLAY_TIME; \
    }
#endif

#if defined(GVARS)
  #if defined(PCBSTD)
    int16_t getGVarFieldValue(int16_t x, int16_t min, int16_t max);
    void setGVarValue(uint8_t x, int8_t value);
    #define GET_GVAR(x, min, max, fm)  getGVarFieldValue(x, min, max)
    #define SET_GVAR(idx, val, fm)     setGVarValue(idx, val)
  #else
    uint8_t getGVarFlightMode(uint8_t fm, uint8_t gv);
    int16_t getGVarFieldValue(int16_t x, int16_t min, int16_t max, int8_t fm);
    int32_t getGVarFieldValuePrec1(int16_t x, int16_t min, int16_t max, int8_t fm);
    int16_t getGVarValue(int8_t gv, int8_t fm);
    int32_t getGVarValuePrec1(int8_t gv, int8_t fm);
    void setGVarValue(uint8_t x, int16_t value, int8_t fm);
    #define GET_GVAR(x, min, max, fm)  getGVarFieldValue(x, min, max, fm)
    #define SET_GVAR(idx, val, fm)     setGVarValue(idx, val, fm)
    #define GVAR_DISPLAY_TIME          100 /*1 second*/;
    #define GET_GVAR_PREC1(x, min, max, fm) getGVarFieldValuePrec1(x, min, max, fm)
    extern uint8_t gvarDisplayTimer;
    extern uint8_t gvarLastChanged;
  #endif
#else
  #define GET_GVAR(x, ...)             (x)
  #define GET_GVAR_PREC1(x, ...)       (x*10)
#endif

#if defined(CPUARM)
  #define GV_GET_GV1_VALUE(max)        ((max<=GV_RANGESMALL && min>=GV_RANGESMALL_NEG) ? GV1_SMALL : GV1_LARGE)
  #define GV_INDEX_CALCULATION(x,max)  ((max<=GV_RANGESMALL && min>=GV_RANGESMALL_NEG) ? (uint8_t) x-GV1_SMALL : ((x&(GV1_LARGE*2-1))-GV1_LARGE))
  #define GV_IS_GV_VALUE(x,min,max)    ((max>GV1_SMALL || min<-GV1_SMALL) ? (x>GV_RANGELARGE || x<GV_RANGELARGE_NEG) : (x>max) || (x<min))
#else
  #define GV_GET_GV1_VALUE(max)        ((max<=GV_RANGESMALL) ? GV1_SMALL : GV1_LARGE)
  #define GV_INDEX_CALCULATION(x,max)  ((max<=GV1_SMALL) ? (uint8_t) x-GV1_SMALL : ((x&(GV1_LARGE*2-1))-GV1_LARGE))
  #define GV_IS_GV_VALUE(x,min,max)    ((x>max) || (x<min) )
#endif

#define GV_INDEX_CALC_DELTA(x,delta)   ((x&(delta*2-1)) - delta)

#define GV_CALC_VALUE_IDX_POS(idx,delta) (-delta+idx)
#define GV_CALC_VALUE_IDX_NEG(idx,delta) (delta+idx)

#define GV_RANGESMALL                  (GV1_SMALL - (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGESMALL_NEG              (-GV1_SMALL + (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGELARGE                  (GV1_LARGE - (RESERVE_RANGE_FOR_GVARS+1))
#define GV_RANGELARGE_NEG              (-GV1_LARGE + (RESERVE_RANGE_FOR_GVARS+1))

#if defined(CPUARM)
  // the define GV1_LARGE marks the highest bit value used for this variables
  // because this would give too big numbers for ARM, we limit it further for
  // offset and weight
  #define GV_RANGELARGE_WEIGHT         (GV_RANGE_WEIGHT)
  #define GV_RANGELARGE_WEIGHT_NEG     (-GV_RANGE_WEIGHT)
  #define GV_RANGELARGE_OFFSET         (GV_RANGE_OFFSET)
  #define GV_RANGELARGE_OFFSET_NEG     (-GV_RANGE_OFFSET)
#else
  // for stock we just use as much as possible
  #define GV_RANGELARGE_WEIGHT         GV_RANGELARGE
  #define GV_RANGELARGE_WEIGHT_NEG     GV_RANGELARGE_NEG
  #define GV_RANGELARGE_OFFSET         GV_RANGELARGE
  #define GV_RANGELARGE_OFFSET_NEG     GV_RANGELARGE_NEG
#endif

#endif // _GVARS_H_