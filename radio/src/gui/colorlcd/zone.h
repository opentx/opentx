
#ifndef _ZONE_H_
#define _ZONE_H_

#include <inttypes.h>

#define LEN_ZONE_OPTION_STRING         8

#if defined(_MSC_VER)
  #define OPTION_VALUE_UNSIGNED(x)    { ZOV_Unsigned, uint32_t(x) }
  #define OPTION_VALUE_SIGNED(x)      { ZOV_Signed, uint32_t(x) }
  #define OPTION_VALUE_BOOL(x)        { ZOV_Bool, bool(x) }
  #define OPTION_VALUE_STRING(...)    { ZOV_String, *(ZoneOptionValue *)(const char *) #__VA_ARGS__ }
#else
  #define OPTION_VALUE_UNSIGNED(x) { .type=ZOV_Unsigned, .unsignedValue = (x) }
  #define OPTION_VALUE_SIGNED(x)   { .type=ZOV_Signed, .signedValue = (x) }
  #define OPTION_VALUE_BOOL(x)     { .type=ZOV_Bool, .boolValue = (x) }
  #define OPTION_VALUE_STRING(...) { .type=ZOV_String, .stringValue = {__VA_ARGS__} }
#endif

struct Zone
{
  uint16_t x, y, w, h;
};

enum ZoneOptionValueType {
  ZOV_Unsigned=0,
  ZOV_Signed,
  ZOV_Bool,
  ZOV_String
};

struct ZoneOptionValue
{
  ZoneOptionValueType type;

  union {
    uint32_t unsignedValue;
    int32_t signedValue;
    bool boolValue;
    char stringValue[LEN_ZONE_OPTION_STRING];
  } NAME(val);
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
