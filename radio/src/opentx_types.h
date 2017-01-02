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

#ifndef _OTXTYPES_H_
#define _OTXTYPES_H_

#if defined(CPUARM)
  typedef uint32_t tmr10ms_t;
  typedef int32_t rotenc_t;
  typedef int32_t getvalue_t;
  typedef uint32_t mixsrc_t;
  typedef int32_t swsrc_t;
  typedef int16_t safetych_t;
  typedef uint16_t bar_threshold_t;
#else
  typedef uint16_t tmr10ms_t;
  typedef int8_t rotenc_t;
  typedef int16_t getvalue_t;
  typedef uint8_t mixsrc_t;
  typedef int8_t swsrc_t;
  typedef int8_t safetych_t;
  typedef uint8_t bar_threshold_t;
#endif

#endif // _OTXTYPES_H_
