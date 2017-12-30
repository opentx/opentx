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

#ifndef _DEFINITIONS_H_
#define _DEFINITIONS_H_

#if defined(SIMU) &&  __GNUC__
  #define __ALIGNED           __attribute__((aligned(32)))
  #define __SECTION_USED(s)   __attribute__((used))
#elif defined(SIMU)
  #define  __ALIGNED
  #define __SECTION_USED(s)
#else
  #define __ALIGNED          __attribute__((aligned(32)))
  #define __SECTION_USED(s)  __attribute__ ((section(s), used))
#endif

#if defined(SIMU)
  #define __DMA
#elif (defined(STM32F4) && !defined(BOOT)) || defined(PCBHORUS)
  #define __DMA __attribute__((section(".ram"), aligned(32)))
#else
  #define __DMA __ALIGNED
#endif

#if defined(PCBHORUS) && !defined(SIMU)
  #define __SDRAM   __attribute__((section(".sdram"), aligned(32)))
  #define __NOINIT  __attribute__((section(".noinit")))
#else
  #define __SDRAM   __DMA
  #define __NOINIT
#endif

#if defined(SIMU) || defined(CPUARM) || GCC_VERSION < 472
typedef int32_t int24_t;
#else
typedef __int24 int24_t;
#endif

#if __GNUC__
  #define PACK( __Declaration__ )      __Declaration__ __attribute__((__packed__))
#else
  #define PACK( __Declaration__ )      __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#endif

#if defined(SIMU)
  #if !defined(FORCEINLINE)
    #define FORCEINLINE inline
  #endif
  #if !defined(NOINLINE)
    #define NOINLINE
  #endif
  #define CONVERT_PTR_UINT(x) ((uint32_t)(uint64_t)(x))
  #define CONVERT_UINT_PTR(x) ((uint32_t*)(uint64_t)(x))
#else
  #define FORCEINLINE inline __attribute__ ((always_inline))
  #define NOINLINE    __attribute__ ((noinline))
  #define SIMU_SLEEP(x)
  #define SIMU_SLEEP_NORET(x)
  #define CONVERT_PTR_UINT(x) ((uint32_t)(x))
  #define CONVERT_UINT_PTR(x) ((uint32_t *)(x))
#endif

#endif // _DEFINITIONS_H_
