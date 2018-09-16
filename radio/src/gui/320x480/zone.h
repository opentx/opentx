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

#ifndef _ZONE_H_
#define _ZONE_H_

#include <inttypes.h>

#define LEN_ZONE_OPTION_STRING         8

#if defined(_MSC_VER)
  #define OPTION_VALUE_UNSIGNED(x)    uint32_t(x)
  #define OPTION_VALUE_SIGNED(x)      uint32_t(x)
  #define OPTION_VALUE_BOOL(x)        bool(x)
  #define OPTION_VALUE_STRING(...)    *(ZoneOptionValue *)(const char *) #__VA_ARGS__
#else
  #define OPTION_VALUE_UNSIGNED(x)    { .unsignedValue = (x) }
  #define OPTION_VALUE_SIGNED(x)      { .signedValue = (x) }
  #define OPTION_VALUE_BOOL(x)        { .boolValue = (x) }
  #define OPTION_VALUE_STRING(...)    { .stringValue = {__VA_ARGS__} }
#endif

struct Zone
{
  uint16_t x, y, w, h;
};

union ZoneOptionValue
{
  uint32_t unsignedValue;
  int32_t signedValue;
  bool boolValue;
  char stringValue[LEN_ZONE_OPTION_STRING];
};

struct ZoneOption
{
  enum Type {
    Integer,
    Source,
    Bool,
    String,
    File,
    TextSize,
    Timer,
    Switch,
    Color
  };

  const char * name;
  Type type;
  ZoneOptionValue deflt;
  ZoneOptionValue min;
  ZoneOptionValue max;
};

#endif
