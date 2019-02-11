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

#ifndef _LCD_TYPES_H_
#define _LCD_TYPES_H_

#include <inttypes.h>

#define CENTER

#define BSS                            0x00

/* lcd common flags */
#define BLINK                          0x01

/* drawText flags */
#define INVERS                         0x02
#define LEFT                           0x00 /* align left */
#define CENTERED                       0x04 /* align center */
#define RIGHT                          0x08 /* align right */
#define SHADOWED                       0x80 /* black copy at +1 +1 */

/* drawNumber flags */
#define LEADING0                       0x10
#define PREC1                          0x20
#define PREC2                          0x30
#define MODE(flags)                    ((((int8_t)(flags) & 0x30) - 0x10) >> 4)

#define ZCHAR                          0x10

/* rect, square flags */
#define ROUND                          0x04

/* telemetry flags */
#define NO_UNIT                        0x40

enum FontSizeIndex {
  STDSIZE_INDEX,
  TINSIZE_INDEX,
  SMLSIZE_INDEX,
  MIDSIZE_INDEX,
  DBLSIZE_INDEX,
  XXLSIZE_INDEX,
  SPARE6_INDEX,
  SPARE7_INDEX,
  STDSIZE_BOLD_INDEX,
  SPARE9_INDEX,
  SPAREa_INDEX,
  SPAREb_INDEX,
  SPAREc_INDEX,
  SPAREd_INDEX,
  SPAREe_INDEX,
  SPAREf_INDEX,
};

#define STDSIZE                        (STDSIZE_INDEX << 8)
#define TINSIZE                        (TINSIZE_INDEX << 8)
#define SMLSIZE                        (SMLSIZE_INDEX << 8)
#define MIDSIZE                        (MIDSIZE_INDEX << 8)
#define DBLSIZE                        (DBLSIZE_INDEX << 8)
#define XXLSIZE                        (XXLSIZE_INDEX << 8)
#define BOLD                           (STDSIZE_BOLD_INDEX << 8)
#define FONTSIZE_MASK                  0x0f00

#if !defined(BOOT)
#define FONTSIZE(flags)                ((flags) & FONTSIZE_MASK)
#define FONTINDEX(flags)               (FONTSIZE(flags) >> 8)
#else
#define FONTSIZE(flags)                STDSIZE
#define FONTINDEX(flags)               STDSIZE_INDEX
#endif

#define TIMEBLINK                      0x1000
#define TIMEHOUR                       0x2000
#define EXPANDED                       0x2000
#define VERTICAL                       0x4000

typedef int32_t coord_t;
typedef uint32_t LcdFlags;

template<class t> inline t min(t a, t b)
{
  return a < b ? a : b;
}

template<class t> inline t max(t a, t b)
{
  return a > b ? a : b;
}

template<class t> inline t limit(t mi, t x, t ma) { return min(max(mi,x),ma); }

#endif // _LCD_TYPES_H_
