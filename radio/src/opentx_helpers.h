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

#ifndef _OPENTX_HELPERS_H_
#define _OPENTX_HELPERS_H_

#include <string.h>

template<class t> inline t min(t a, t b) { return a<b?a:b; }
template<class t> inline t max(t a, t b) { return a>b?a:b; }
template<class t> inline t sgn(t a) { return a>0 ? 1 : (a < 0 ? -1 : 0); }
template<class t> inline t limit(t mi, t x, t ma) { return min(max(mi,x),ma); }
template<class t> inline void SWAP(t & a, t & b) { t tmp = b; b = a; a = tmp; }

inline void memclear(void * p, size_t size)
{
  memset(p, 0, size);
}

inline bool is_memclear(void * p, size_t size)
{
  uint8_t * buf = (uint8_t *)p;
  return buf[0] == 0 && memcmp(buf, buf + 1, size - 1) == 0;
}

#endif // _OPENTX_HELPERS_H_
