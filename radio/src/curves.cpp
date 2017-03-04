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

uint8_t s_curveChan;

#if defined(CPUARM)
int8_t * curveEnd[MAX_CURVES];
void loadCurves()
{
  int8_t * tmp = g_model.points;
  for (int i=0; i<MAX_CURVES; i++) {
    switch (g_model.curves[i].type) {
      case CURVE_TYPE_STANDARD:
        tmp += 5+g_model.curves[i].points;
        break;
      case CURVE_TYPE_CUSTOM:
        tmp += 8+2*g_model.curves[i].points;
        break;
      default:
        TRACE("Wrong curve type! Fixing...");
        g_model.curves[i].type = CURVE_TYPE_STANDARD;
        tmp += 5+g_model.curves[i].points;
        break;
    }
    curveEnd[i] = tmp;
  }
}

int8_t * curveAddress(uint8_t idx)
{
  return idx==0 ? g_model.points : curveEnd[idx-1];
}

bool moveCurve(uint8_t index, int8_t shift) // TODO bool?
{
  if (curveEnd[MAX_CURVES-1] + shift > g_model.points + sizeof(g_model.points)) {
    AUDIO_WARNING2();
    return false;
  }
  
  int8_t * nextCrv = curveAddress(index+1);
  memmove(nextCrv+shift, nextCrv, 5*(MAX_CURVES-index-1)+curveEnd[MAX_CURVES-1]-curveEnd[index]);
  if (shift < 0) memclear(&g_model.points[MAX_CURVE_POINTS-1] + shift, -shift);
  while (index<MAX_CURVES) {
    curveEnd[index++] += shift;
  }
  
  storageDirty(EE_MODEL);
  return true;
}

int8_t getCurveX(int noPoints, int point)
{
  return -100 + div_and_round((point*2000) / (noPoints-1), 10);
}

void resetCustomCurveX(int8_t * points, int noPoints)
{
  for (int i=0; i<noPoints-2; i++) {
    points[noPoints+i] = getCurveX(noPoints, i+1);
  }
}
#else
int8_t * curveAddress(uint8_t idx)
{
  return &g_model.points[idx==0 ? 0 : 5*idx+g_model.curves[idx-1]];
}

CurveInfo curveInfo(uint8_t idx)
{
  CurveInfo result;
  result.crv = curveAddress(idx);
  int8_t *next = curveAddress(idx+1);
  uint8_t size = next - result.crv;
  if ((size & 1) == 0) {
    result.points = (size / 2) + 1;
    result.custom = true;
  }
  else {
    result.points = size;
    result.custom = false;
  }
  return result;
}

bool moveCurve(uint8_t index, int8_t shift, int8_t custom) // TODO move?
{
  if (g_model.curves[MAX_CURVES-1] + shift > MAX_CURVE_POINTS-5*MAX_CURVES) {
    AUDIO_WARNING2();
    return false;
  }

  int8_t * crv = curveAddress(index);
  if (shift < 0) {
    for (uint8_t i=0; i<custom; i++)
      crv[i] = crv[2*i];
  }

  int8_t * nextCrv = curveAddress(index+1);
  memmove(nextCrv+shift, nextCrv, 5*(MAX_CURVES-index-1)+g_model.curves[MAX_CURVES-1]-g_model.curves[index]);
  if (shift < 0) memclear(&g_model.points[MAX_CURVE_POINTS-1] + shift, -shift);
  while (index<MAX_CURVES) {
    g_model.curves[index++] += shift;
  }

  for (uint8_t i=0; i<custom-2; i++) {
    crv[custom + i] = -100 + ((200 * (i + 1) + custom / 2) / (custom - 1));
  }

  storageDirty(EE_MODEL);
  return true;
}
#endif

#if defined(CPUARM)
#define CUSTOM_POINT_X(points, count, idx) ((idx)==0 ? -100 : (((idx)==(count)-1) ? 100 : points[(count)+(idx)-1]))
int32_t compute_tangent(CurveInfo * crv, int8_t * points, int i)
{
  int32_t m=0;
    uint8_t num_points = crv->points + 5;
    #define MMULT 1024
    if (i == 0) {
      //linear interpolation between 1st 2 points
      //keep 3 decimal-places for m
      if (crv->type == CURVE_TYPE_CUSTOM) {
        int8_t x0 = CUSTOM_POINT_X(points, num_points, 0);
        int8_t x1 = CUSTOM_POINT_X(points, num_points, 1);
        if (x1 > x0) m = (MMULT * (points[1] - points[0])) / (x1 - x0);
      }
      else {
        int32_t delta = (2 * 100) / (num_points - 1);
        m = (MMULT * (points[1] - points[0])) / delta;
      }
    }
    else if (i == num_points - 1) {
      //linear interpolation between last 2 points
      //keep 3 decimal-places for m
      if (crv->type == CURVE_TYPE_CUSTOM) {
        int8_t x0 = CUSTOM_POINT_X(points, num_points, num_points-2);
        int8_t x1 = CUSTOM_POINT_X(points, num_points, num_points-1);
        if (x1 > x0) m = (MMULT * (points[num_points-1] - points[num_points-2])) / (x1 - x0);
      }
      else {
        int32_t delta = (2 * 100) / (num_points - 1);
        m = (MMULT * (points[num_points-1] - points[num_points-2])) / delta;
      }
    }
    else {
        //apply monotone rules from
        //http://en.wikipedia.org/wiki/Monotone_cubic_interpolation
        //1) compute slopes of secant lines
      int32_t d0=0, d1=0;
        if (crv->type == CURVE_TYPE_CUSTOM) {
          int8_t x0 = CUSTOM_POINT_X(points, num_points, i-1);
          int8_t x1 = CUSTOM_POINT_X(points, num_points, i);
          int8_t x2 = CUSTOM_POINT_X(points, num_points, i+1);
          if (x1 > x0) d0 = (MMULT * (points[i] - points[i-1])) / (x1 - x0);
          if (x2 > x1) d1 = (MMULT * (points[i+1] - points[i])) / (x2 - x1);
        }
        else {
          int32_t delta = (2 * 100) / (num_points - 1);
          d0 = (MMULT * (points[i] - points[i-1])) / (delta);
          d1 = (MMULT * (points[i+1] - points[i])) / (delta);
        }
        //2) compute initial average tangent
        m = (d0 + d1) / 2;
        //3 check for horizontal lines
        if (d0 == 0 || d1 == 0 || (d0 > 0 && d1 < 0) || (d0 < 0 && d1 > 0)) {
          m = 0;
        }
        else if (MMULT * m / d0 >  3 * MMULT) {
          m = 3 * d0;
        }
        else if (MMULT * m / d1 > 3 * MMULT) {
          m = 3 * d1;
        }
    }
    return m;
}

/* The following is a hermite cubic spline.
   The basis functions can be found here:
   http://en.wikipedia.org/wiki/Cubic_Hermite_spline
   The tangents are computed via the 'cubic monotone' rules (allowing for local-maxima)
*/
int16_t hermite_spline(int16_t x, uint8_t idx)
{
  CurveInfo &crv = g_model.curves[idx];
  int8_t *points = curveAddress(idx);
  uint8_t count = crv.points+5;
  bool custom = (crv.type == CURVE_TYPE_CUSTOM);

  if (x < -RESX)
    x = -RESX;
  else if (x > RESX)
    x = RESX;

  for (int i=0; i<count-1; i++) {
    int32_t p0x, p3x;
    if (custom) {
      p0x = (i>0 ? calc100toRESX(points[count+i-1]) : -RESX);
      p3x = (i<count-2 ? calc100toRESX(points[count+i]) : RESX);
    }
    else {
      p0x = -RESX + (i*2*RESX)/(count-1);
      p3x = -RESX + ((i+1)*2*RESX)/(count-1);
    }

    if (x >= p0x && x <= p3x) {
      int32_t p0y = calc100toRESX(points[i]);
      int32_t p3y = calc100toRESX(points[i+1]);
      int32_t m0 = compute_tangent(&crv, points, i);
      int32_t m3 = compute_tangent(&crv, points, i+1);
      int32_t y;
      int32_t h = p3x - p0x;
      int32_t t = (h > 0 ? (MMULT * (x - p0x)) / h : 0);
      int32_t t2 = t * t / MMULT;
      int32_t t3 = t2 * t / MMULT;
      int32_t h00 = 2*t3 - 3*t2 + MMULT;
      int32_t h10 = t3 - 2*t2 + t;
      int32_t h01 = -2*t3 + 3*t2;
      int32_t h11 = t3 - t2;
      y = p0y * h00 + h * (m0 * h10 / MMULT) + p3y * h01 + h * (m3 * h11 / MMULT);
      y /= MMULT;
      return y;
    }
  }
  return 0;
}
#endif

int intpol(int x, uint8_t idx) // -100, -75, -50, -25, 0 ,25 ,50, 75, 100
{
#if defined(CPUARM)
  CurveInfo & crv = g_model.curves[idx];
  int8_t * points = curveAddress(idx);
  uint8_t count = crv.points+5;
  bool custom = (crv.type == CURVE_TYPE_CUSTOM);
#else
  CurveInfo crv = curveInfo(idx);
  int8_t * points = crv.crv;
  uint8_t count = crv.points;
  bool custom = crv.custom;
#endif
  int16_t erg = 0;

  x += RESXu;

  if (x <= 0) {
    erg = (int16_t)points[0] * (RESX/4);
  }
  else if (x >= (RESX*2)) {
    erg = (int16_t)points[count-1] * (RESX/4);
  }
  else {
    uint16_t a=0, b=0;
    uint8_t i;
    if (custom) {
      for (i=0; i<count-1; i++) {
        a = b;
        b = (i==count-2 ? 2*RESX : RESX + calc100toRESX(points[count+i]));
        if ((uint16_t)x<=b) break;
      }
    }
    else {
      uint16_t d = (RESX * 2) / (count-1);
      i = (uint16_t)x / d;
      a = i * d;
      b = a + d;
    }
    erg = (int16_t)points[i]*(RESX/4) + ((int32_t)(x-a) * (points[i+1]-points[i]) * (RESX/4)) / ((b-a));
  }

  return erg / 25; // 100*D5/RESX;
}

#if defined(CPUARM)
int applyCurve(int x, CurveRef & curve)
{
  switch (curve.type) {
    case CURVE_REF_DIFF:
    {
      int curveParam = GET_GVAR_PREC1(curve.value, -100, 100, mixerCurrentFlightMode);
      if (curveParam > 0 && x < 0)
        x = (x * (1000 - curveParam)) / 1000;
      else if (curveParam < 0 && x > 0)
        x = (x * (1000 + curveParam)) / 1000;
      return x;
    }

    case CURVE_REF_EXPO:
    {
      int curveParam = GET_GVAR_PREC1(curve.value, -100, 100, mixerCurrentFlightMode) / 10;
      return expo(x, curveParam);
    }

    case CURVE_REF_FUNC:
      switch (curve.value) {
        case CURVE_X_GT0:
          if (x < 0) x = 0; //x|x>0
          return x;
        case CURVE_X_LT0:
          if (x > 0) x = 0; //x|x<0
          return x;
        case CURVE_ABS_X: // x|abs(x)
          return abs(x);
        case CURVE_F_GT0: //f|f>0
          return x > 0 ? RESX : 0;
        case CURVE_F_LT0: //f|f<0
          return x < 0 ? -RESX : 0;
        case CURVE_ABS_F: //f|abs(f)
          return x > 0 ? RESX : -RESX;
      }
      break;

    case CURVE_REF_CUSTOM:
    {
      int curveParam = curve.value;
      if (curveParam < 0) {
        x = -x;
        curveParam = -curveParam;
      }
      if (curveParam > 0 && curveParam <= MAX_CURVES) {
        return applyCustomCurve(x, curveParam - 1);
      }
      break;
    }
  }

  return x;
}

int applyCustomCurve(int x, uint8_t idx)
{
  if (idx >= MAX_CURVES)
    return 0;

  CurveInfo & crv = g_model.curves[idx];
  if (crv.smooth)
    return hermite_spline(x, idx);
  else
    return intpol(x, idx);
}
#else
int applyCurve(int x, int8_t idx)
{
  /* already tried to have only one return at the end */
  switch(idx) {
    case CURVE_NONE:
      return x;
    case CURVE_X_GT0:
      if (x < 0) x = 0; //x|x>0
      return x;
    case CURVE_X_LT0:
      if (x > 0) x = 0; //x|x<0
      return x;
    case CURVE_ABS_X: // x|abs(x)
      return abs(x);
    case CURVE_F_GT0: //f|f>0
      return x > 0 ? RESX : 0;
    case CURVE_F_LT0: //f|f<0
      return x < 0 ? -RESX : 0;
    case CURVE_ABS_F: //f|abs(f)
      return x > 0 ? RESX : -RESX;
  }
  if (idx < 0) {
    x = -x;
    idx = -idx + CURVE_BASE - 1;
  }
  return applyCustomCurve(x, idx - CURVE_BASE);
}
#endif

// #define EXTENDED_EXPO
// increases range of expo curve but costs about 82 bytes flash

// expo-funktion:
// ---------------
// kmplot
// f(x,k)=exp(ln(x)*k/10) ;P[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]
// f(x,k)=x*x*x*k/10 + x*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]
// f(x,k)=x*x*k/10 + x*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]
// f(x,k)=1+(x-1)*(x-1)*(x-1)*k/10 + (x-1)*(1-k/10) ;P[0,1,2,3,4,5,6,7,8,9,10]
// don't know what this above should be, just confusing in my opinion,

// here is the real explanation
// actually the real formula is
/*
 f(x) = exp( ln(x) * 10^k)
 if it is 10^k or e^k or 2^k etc. just defines the max distortion of the expo curve; I think 10 is useful
 this gives values from 0 to 1 for x and output; k must be between -1 and +1
 we do not like to calculate with floating point. Therefore we rescale for x from 0 to 1024 and for k from -100 to +100
 f(x) = 1024 * ( e^( ln(x/1024) * 10^(k/100) ) )
 This would be really hard to be calculated by such a microcontroller
 Therefore Thomas Husterer compared a few usual function something like x^3, x^4*something, which look similar
 Actually the formula
 f(x) = k*x^3+x*(1-k)
 gives a similar form and should have even advantages compared to a original exp curve.
 This function again expect x from 0 to 1 and k only from 0 to 1
 Therefore rescaling is needed like before:
 f(x) = 1024* ((k/100)*(x/1024)^3 + (x/1024)*(100-k)/100)
 some mathematical tricks
 f(x) = (k*x*x*x/(1024*1024) + x*(100-k)) / 100
 for better rounding results we add the 50
 f(x) = (k*x*x*x/(1024*1024) + x*(100-k) + 50) / 100

 because we now understand the formula, we can optimize it further
 --> calc100to256(k) --> eliminates /100 by replacing with /256 which is just a simple shift right 8
 k is now between 0 and 256
 f(x) = (k*x*x*x/(1024*1024) + x*(256-k) + 128) / 256
 */

// input parameters;
//  x 0 to 1024;
//  k 0 to 100;
// output between 0 and 1024
unsigned int expou(unsigned int x, unsigned int k)
{
#if defined(EXTENDED_EXPO)
  bool extended;
  if (k>80) {
    extended=true;
  }
  else {
    k += (k>>2);  // use bigger values before extend, because the effect is anyway very very low
    extended=false;
  }
#endif

  k = calc100to256(k);

  uint32_t value = (uint32_t) x*x;
  value *= (uint32_t)k;
  value >>= 8;
  value *= (uint32_t)x;

#if defined(EXTENDED_EXPO)
  if (extended) {  // for higher values do more multiplications to get a stronger expo curve
    value >>= 16;
    value *= (uint32_t)x;
    value >>= 4;
    value *= (uint32_t)x;
  }
#endif

  value >>= 12;
  value += (uint32_t)(256-k)*x+128;

  return value>>8;
}

int expo(int x, int k)
{
  if (k == 0) {
    return x;
  }
  
  int y;
  bool neg = (x < 0);

  if (neg) {
    x = -x;
  }
  if (k < 0) {
    y = RESXu - expou(RESXu-x, -k);
  }
  else {
    y = expou(x, k);
  }
  return neg ? -y : y;
}

point_t getPoint(uint8_t i)
{
  point_t result = {0, 0};
#if defined(CPUARM)
  CurveInfo & crv = g_model.curves[s_curveChan];
  int8_t * points = curveAddress(s_curveChan);
  bool custom = (crv.type == CURVE_TYPE_CUSTOM);
  uint8_t count = 5+crv.points;
#else
  CurveInfo crv = curveInfo(s_curveChan);
  int8_t * points = crv.crv;
  bool custom = crv.custom;
  uint8_t count = crv.points;
#endif
  if (i < count) {
    result.x = CURVE_CENTER_X-1-CURVE_SIDE_WIDTH + i*CURVE_SIDE_WIDTH*2/(count-1);
    result.y = CURVE_CENTER_Y - (points[i]) * (CURVE_SIDE_WIDTH-1) / 100;
    if (custom && i>0 && i<count-1) {
      result.x = CURVE_CENTER_X - 1 - CURVE_SIDE_WIDTH + (100 + (100 + points[count + i - 1]) * (2 * CURVE_SIDE_WIDTH)) / 200;
    }
  }
  return result;
}

int applyCurrentCurve(int x)
{
  return applyCustomCurve(x, s_curveChan);
}
