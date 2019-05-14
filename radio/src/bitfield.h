/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * This file is based on work published at http://www.coranac.com/documents/working-with-bits-and-bitfields
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

#ifndef BITFIELD_H
#define BITFIELD_H

#include <inttypes.h>

// A set of bitfield handling macros

template <typename T>
inline T BF_BIT(uint8_t n)
{
  return T(1) << n;
}

#define BF_BIT_GET(y, mask)        ( y & (mask) )
#define BF_BIT_SET(y, mask)        ( y |=  (mask) )
#define BF_BIT_CLEAR(y, mask)      ( y &= ~(mask) )
#define BF_BIT_FLIP(y, mask)       ( y ^=  (mask) )
#define BF_SINGLE_BIT_GET(y, i)    BF_BIT_GET(y, BF_BIT(i))
#define BF_SINGLE_BIT_SET(y, i)    BF_BIT_SET(y, BF_BIT(i))

//! Create a bitmask of length 'len'.
template <typename T>
inline T BF_BITMASK(uint8_t len)
{
  return BF_BIT<T>(len) - 1;
}

//! Create a bitfield mask of length 'len' starting at bit 'start'.
template <typename T>
inline T BF_MASK(uint8_t start, uint8_t len)
{
  return BF_BITMASK<T>(len) << start;
}

//! Prepare a bitmask for insertion or combining.
template <typename T>
inline T BF_PREP(T x, uint8_t start, uint8_t len)
{
  return (x & BF_BITMASK<T>(len)) << start;
}

//! Extract a bitfield of length 'len' starting at bit 'start' from  'y'.
template <typename T>
inline T BF_GET(T y, uint8_t start, uint8_t len)
{
  return ((y)>>(start)) & BF_BITMASK<T>(len);
}

//! Insert 'len' bits of 'x 'into 'y', starting at bit 'start' from  'y'.
template <class T>
inline T BF_SET(T to, T from, uint8_t start, uint8_t len)
{
  return (to & ~BF_MASK<T>(start, len)) | BF_PREP<T>(from, start, len);
}

#endif //BITFIELD_H
