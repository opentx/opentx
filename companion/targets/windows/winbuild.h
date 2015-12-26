/*
 * Author - Bertrand Songis <bsongis@gmail.com>
 *
 * Based on th9x -> http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef WINBUILD_H
#define WINBUILD_H

#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#define FORCEINLINE inline
#define NOINLINE

#define strcasecmp  _stricmp
#define strncasecmp _tcsnicmp
#define snprintf    _snprintf

inline double round(double number)
{
  return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}

#endif
