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

#ifndef _LIBOPEUI_HELPERS_H_
#define _LIBOPEUI_HELPERS_H_

template<class T>
inline T min(T a, T b)
{
  return a < b ? a : b;
}

template<class T>
inline T max(T a, T b)
{
  return a > b ? a : b;
}

template<class T>
inline T limit(T vmin, T x, T vmax)
{
  return min(max(vmin, x), vmax);
}

#endif
