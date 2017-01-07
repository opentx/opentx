
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
