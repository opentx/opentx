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

#ifndef OTX_MATH_H
#define OTX_MATH_H

#include "definitions.h"

#if defined(CPUARM) && defined(__GNUC__) && !defined(SIMU)
  #include "board.h"
  #include "arm_math.h"
#endif

#if !defined(QT_CORE_LIB)
  #define _USE_MATH_DEFINES
#endif

#include <cmath>
#include <inttypes.h>
#include <type_traits>

#if !defined(M_PI)
  #define M_PI         3.14159265358979323846   // pi
  #define M_PI_2       1.57079632679489661923   // pi/2
  #define M_PI_4       0.785398163397448309616  // pi/4
  #define M_1_PI       0.318309886183790671538  // 1/pi
  #define M_2_PI       0.636619772367581343076  // 2/pi
  #define M_2_SQRTPI   1.12837916709551257390   // 2/sqrt(pi)
  #define M_SQRT2      1.41421356237309504880   // sqrt(2)
  #define M_SQRT1_2    0.707106781186547524401  // 1/sqrt(2)
  #define M_E          2.71828182845904523536   // e
  #define M_LOG2E      1.44269504088896340736   // log2(e)
  #define M_LOG10E     0.434294481903251827651  // log10(e)
  #define M_LN2        0.693147180559945309417  // ln(2)
  #define M_LN10       2.30258509299404568402   // ln(10)
#endif

// RESX range is used for internal calculation; The menu says -100.0 to 100.0; internally it is -1024 to 1024 to allow some optimizations
#define RESX_SHIFT 10
#define RESX       1024
#define RESXu      1024u
#define RESXul     1024ul
#define RESXl      1024l


// these are typically not standard even in the non-standard math defines :)
#ifndef   M_LNPI
  #define M_LNPI       1.144729885849400174143  // ln(pi)
#endif
#ifndef   M_EULER
  #define M_EULER      0.577215664901532860606  // Euler constant
#endif
#ifndef   M_PI2
  #define M_PI2        6.28318530717958647692   // pi*2
#endif
#ifndef   M_PI_3Q
  #define M_PI_3Q      2.356194490192344928845  // pi/4*3
#endif

#define RAD_TO_DEG     (180.0 / M_PI)
#define DEG_TO_RAD     (M_PI / 180.0)

// we like floats!
#define M_PIf          (float)(M_PI)
#define M_PI_2f        (float)(M_PI_2)
#define M_PI_4f        (float)(M_PI_4)
#define M_1_PIf        (float)(M_1_PI)
#define M_2_PIf        (float)(M_2_PI)
#define M_2_SQRTPIf    (float)(M_2_SQRTPI)
#define M_SQRT2f       (float)(M_SQRT2)
#define M_SQRT1_2f     (float)(M_SQRT1_2)
#define M_PI2f         (float)(M_PI2)
#define M_PI_3Qf       (float)(M_PI_3Q)
#define RAD_TO_DEGf    (float)(RAD_TO_DEG)
#define DEG_TO_RADf    (float)(DEG_TO_RAD)


#define OTXM_CST_ST_INL         constexpr static inline
#define OTXM_TEMPL_ST_INL       template <typename T> static inline
#define OTXM_TEMPL_CST_ST_INL   template <typename T> OTXM_CST_ST_INL

#define calc100to256_16Bits(x) calc100to256(x)
#define calc100toRESX_16Bits(x) calc100toRESX(x)


inline int divRoundClosest(const int n, const int d)
{
  if (d == 0)
    return 0;
  else
    return ((n < 0) ^ (d < 0)) ? ((n - d/2)/d) : ((n + d/2)/d);
}

inline int calc100to256(int x)
{
  return divRoundClosest(x*256, 100);
}

inline int calc100toRESX(int x)
{
  return divRoundClosest(x*RESX, 100);
}

inline int calc1000toRESX(int x)
{
  return divRoundClosest(x*RESX, 1000);
}

inline int calcRESXto1000(int x)
{
  return divRoundClosest(x*1000, RESX);
}

inline int calcRESXto100(int x)
{
  return divRoundClosest(x*100, RESX);
}

//! abstract version, use \e MathUtil::sinf() instead
inline const float otx_sinf(float x)
{
#if defined(CPUARM) && defined(__GNUC__) && !defined(SIMU)
  return arm_sin_f32(x);
#else
  return sinf(x);
#endif
}

//! abstract version, use \e MathUtil::cosf() instead
inline const float otx_cosf(float x)
{
#if defined(CPUARM) && defined(__GNUC__) && !defined(SIMU)
  return arm_cos_f32(x);
#else
  return cosf(x);
#endif
}

//! abstract version, use \e MathUtil::sincosf() instead
inline const void otx_sincosf(float x, float * s, float * c)
{
#if defined(CPUARM) && defined(__GNUC__) && !defined(SIMU)
  arm_sin_cos_f32(x * RAD_TO_DEGf, s, c);  // for some reason the arm lib version takes degrees not radians... so we adapt.
#else
  // sincos() isn't really a standard function, eg. MSCVC does not implement it (of course)
  *s = otx_sinf(x);
  *c = otx_cosf(x);
#endif
}

//! A collection of optimized math routines, in some cases providing faster alternatives to standard versions. Geared towards float precision and integer types.
class MathUtil
{
  public:

  //! Faster version of standard sinf function at expense of some accuracy (see @ bottom for full stats)
  //! Error margin: [avg]  7.95405e-006 (0.0000080); [max]  1.86563e-005 (0.0000187);
  static inline const float sinf(float x) { return otx_sinf(x); }

  //! Faster version of standard sinf function at expense of some accuracy (see @ bottom for full stats)
  //! Error margin: [avg]  7.99415e-006 (0.0000080); [max]  1.86477e-005 (0.0000186);
  static inline const float cosf(float x) { return otx_cosf(x); }

  //! Faster version of common sincosf function, more accurate but slightly slower than the individual versions above (see @ bottom for full stats)
  //! Error margin:  Sin [avg]  6.08607e-008 (0.0000001); [max]  4.02331e-007 (0.0000004);
  //! Error margin:  Cos [avg]  5.97036e-008 (0.0000001); [max]  3.61353e-007 (0.0000004);
  static inline void sincosf(float x, float * s, float * c) { otx_sincosf(x, s, c); }

  //! Returns atan2 in radians, faster than default atan2f and plenty accurate for float precision. (see @ bottom for full stats)
  //! Avg. err: 1.50728e-008 (0.00000002); Max err: 1.69079e-006 (0.00000169)
  static const float atan2f(float y, float x)
  {
    float r, angle;
    const float abs_y = fabsf(y) + 1e-6f; // prevent 0/0 condition
    if ( x < 0.0f ) {
      r = (x + abs_y) / (abs_y - x);
      angle = M_PI_3Qf;
    }
    else {
      r = (x - abs_y) / (x + abs_y);
      angle = M_PI_4f;
    }
    angle += (0.1963f * r * r - 0.9817f) * r;
    if ( y < 0.0f )
      return( -angle );     // negate if in quad III or IV
    else
      return( angle );
  }

  //! unsigned integer square root
  static const uint16_t isqrt32(uint32_t n)
  {
    uint16_t c = 0x8000;
    uint16_t g = 0x8000;

    for (;;) {
      if ((uint32_t)g*g > n)
        g ^= c;
      c >>= 1;
      if(c == 0)
        return g;
      g |= c;
    }
  }

  //! djb2 hash algorithm
  static const uint32_t hash(const void * ptr, uint32_t size)
  {
    const uint8_t * data = (const uint8_t *)ptr;
    uint32_t hash = 5381;
    for (uint32_t i=0; i<size; i++) {
      hash = ((hash << 5) + hash) + data[i]; /* hash * 33 + c */
    }
    return hash;
  }

  // constexpr versions
  OTXM_TEMPL_CST_ST_INL T abs(const T & t) { return t >= T(0) ? t : -t; }
  OTXM_TEMPL_CST_ST_INL const T & min(const T & a, const T & b) { return (a < b) ? a : b; }
  OTXM_TEMPL_CST_ST_INL const T & max(const T & a, const T & b) { return (a < b) ? b : a; }
  OTXM_TEMPL_CST_ST_INL const T & bound(const T & min, const T & val, const T & max) { return MathUtil::max(min, MathUtil::min(max, val)); }

  //! float fuzzy zero check
  OTXM_CST_ST_INL bool fuzzyIsZero(float x, float epsilon = 1e-5f)   { return (MathUtil::abs(x) <= epsilon); }
  //! double fuzzy zero check
  OTXM_CST_ST_INL bool fuzzyIsZero(double x, double epsilon = 1e-12) { return (MathUtil::abs(x) <= epsilon); }
  //! float fuzzy compare
  OTXM_CST_ST_INL bool fuzzyCompare(float x, float y, float epsilon = 1e5f)    { return (MathUtil::abs(x - y) * epsilon <= min(MathUtil::abs(x), MathUtil::abs(y))); }
  //! double fuzzy compare
  OTXM_CST_ST_INL bool fuzzyCompare(double x, double y, double epsilon = 1e12) { return (MathUtil::abs(x - y) * epsilon <= min(MathUtil::abs(x), MathUtil::abs(y))); }

  //! wrap x -> [0, max) float
  static inline const float wrapMax(float x, float max) { return fmodf(max + fmodf(x, max), max); }
  //! wrap x -> [0, max) double
  static inline const double wrapMax(double x, double max) { return fmod(max + fmod(x, max), max); }
  //! wrap x -> [0, max) integers
  OTXM_TEMPL_CST_ST_INL typename std::enable_if<std::is_integral<T>::value, T>::type wrapMax(T x, T max) { return ((max + (x % max)) % max); }

  //! wrap x -> [min, max)
  OTXM_TEMPL_CST_ST_INL T wrapMinMax(T x, T min, T max) { return min + wrapMax(x - min, max - min); }

  //! Normalize/reduce/wrap an angle into a range, works with degrees or radians of any variable type. Convenience wrapper for \sa wrapMinMax() template.
  OTXM_TEMPL_ST_INL const T normalizeAngleRange(T angle, T min = T(-180), T max = T(180))
  {
    if (angle < min || angle > max)
      angle = wrapMinMax(angle, min, max);
    return angle;
  }

  //! Normalize degrees \a x into compass range [0, 360] and return result.
  OTXM_TEMPL_CST_ST_INL T normalizeCompassAngle(T x) { return normalizeAngleRange(x, T(0), T(360)); }

  //! Normalize/reduce/wrap angle radians \a x into range [-PI, PI] and return result.
  OTXM_TEMPL_CST_ST_INL T normalizeAnglePi(T x)      { return normalizeAngleRange(x, -T(M_PI), T(M_PI)); }

  //! Normalize/reduce/wrap angle radians \a x into range [-PI/2, PI/2] and return result.
  OTXM_TEMPL_CST_ST_INL T normalizeAngleHalfPi(T x)  { return normalizeAngleRange(x, -T(M_PI_2), T(M_PI_2)); }

  //! Normalize/reduce/wrap angle radians \a x into range [0, PI*2] and return result.
  OTXM_TEMPL_CST_ST_INL T normalizeAngleTwoPi(T x)   { return normalizeAngleRange(x, T(0), T(M_PI2)); }

};

/*  Accuracy/benchmark tests & other notes

NOTE: For 240% speed boost to builtin sqrtf() function (and some others), be sure to use -fno-math-errno compiler flag to unleash full power of FPU!
  Benchmark sqrtf() gnu-arm defaults VS with -fno-math-errno (or native asm code) on STM32F4 (.5ns ticks)
    == default: iter cnt: 5;    avg:  8800; min: 8796; max: 8807; ttl:  44004;
    == fpu asm: iter cnt: 5;    avg:  2592; min: 2585; max: 2612; ttl:  12961;
    <> fpu asm VS default: dlta avg: -6208 (240%);                ttl: -31043 (240%);


Sin/Cos stdlib VS (arm_sinf_f32 & arm_cosf_f32) VS arm_sincosf_f32
  Accuracy - arm_sinf_f32 & arm_cosf_f32:
    Sin [avg]  7.95405e-006 (0.0000080); [mean] +5.59358e-007 (+0.0000006); [max]  1.86563e-005 (0.0000187);
    Cos [avg]  7.99415e-006 (0.0000080); [mean] -2.96762e-005 (-0.0000297); [max]  1.86477e-005 (0.0000186);
  Accuracy - arm_sincosf_f32
    Sin [avg]  6.08607e-008 (0.0000001); [mean] +1.65735e-007 (+0.0000002); [max]  4.02331e-007 (0.0000004);
    Cos [avg]  5.97036e-008 (0.0000001); [mean]  +1.6254e-005 (+0.0000163); [max]  3.61353e-007 (0.0000004);
  Benchmark Sin/Cos std VS arm_math_[sin|cos]_f32 on STM32F4 (.5ns ticks)
    == std default: iter cnt: 5; avg: 6187; min: 6173; max: 6199; ttl: 30936;
    == arm_sin&cos: iter cnt: 5; avg: 2041; min: 2037; max: 2045; ttl: 10206;
    == arm_sincosf: iter cnt: 5; avg: 2344; min: 2338; max: 2356; ttl: 11724;
  Deltas
    <> arm_sin&cos VS std default: dlta avg: -4312 (210%);        ttl: -21564 (210%);
    <> arm_sincosf VS std default: dlta avg: -4037 (173%);        ttl: -20186 (173%);
    <> arm_sincosf VS arm_sin&cos: dlta avg:   275  (12%);        ttl:   1378  (12%);


atan2f() vs. stdlib
  Accuracy
    Avg. err: 1.50728e-008 (0.00000002); Max err:  1.69079e-006 (0.00000169)
  Benchmark atan2f() std vs. alt on STM32F4 (.5ns ticks)
    == std: iter cnt: 5; avg: 4523; min: 4519; max: 4535; ttl: 22619;
    == alt: iter cnt: 5; avg: 1049; min: 1049; max: 1053; ttl: 5249;
    <> alt VS std:  dlta avg: -3474 (331%);               ttl: -17370 (331%);


NOTES:

Another atan2f() solution was tested but found to be less accurate and a bit slower.  This version was also common, eg. as found at:
https://gist.github.com/volkansalma/2972237 ("atan2_approximation2()")
https://github.com/libgdx/libgdx/blob/master/gdx/src/com/badlogic/gdx/math/MathUtils.java#L86
Accuracy: Avg. err:    0.00414816 (0.00414816);  Max err:    0.00414821 (0.00414821)
Benchmrk: iter cnt: 5; avg: 1841; min: 1840; max: 1843; ttl: 9206;

For sin and cos a couple other alternate solutions were tested, including CORDIC and several pre-generated Sin value lookup tables.
Most turned out to be slower than the default/stdlib sinf/cosf functions. The exception turned out to the implemented methods from ARM DSP Lib.
CORDIC was very accurate but much slower than default/standard esp. when the angles needed to be reduced.
CORDIC example: https://github.com/johnhw/cordic/
Sin table example: https://code.woboq.org/qt5/qtbase/src/corelib/kernel/qmath.h.html#_Z8qFastSind
  or: https://github.com/libgdx/libgdx/blob/master/gdx/src/com/badlogic/gdx/math/MathUtils.java#L51

In case for some reason -fno-math-errno compiler flag cannot be used, this function uses native FPU sqrt assy instuction where supported.
#ifndef __sqrtf
  static inline const float __sqrtf(float x)
  {
  #if defined(STM32F4) && !defined(SIMU)
    float d;
    __asm__ __volatile__ ( "vsqrt.f32 %0, %1" : "=&t" (d) : "t" (x) );
    return d;
  #else
    return sqrtf(x);
  #endif
  }
#endif

*/

#endif // OTX_MATH_H
