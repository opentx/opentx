
#ifndef _ZONE_H_
#define _ZONE_H_

#include <inttypes.h>
#include "storage/yaml/yaml_defs.h"

#define LEN_ZONE_OPTION_STRING         8

#if defined(_MSC_VER)
  #define OPTION_VALUE_UNSIGNED(x)    { uint32_t(x) }
  #define OPTION_VALUE_SIGNED(x)      { uint32_t(x) }
  #define OPTION_VALUE_BOOL(x)        {  bool(x) }
  #define OPTION_VALUE_STRING(...)    { *(ZoneOptionValue *)(const char *) #__VA_ARGS__ }
#else
  #define OPTION_VALUE_UNSIGNED(x) { .unsignedValue = (x) }
  #define OPTION_VALUE_SIGNED(x)   { .signedValue = (x) }
  #define OPTION_VALUE_BOOL(x)     { .boolValue = (x) }
  #define OPTION_VALUE_STRING(...) { .stringValue = { __VA_ARGS__ } }
#endif

union ZoneOptionValue
{
  uint32_t unsignedValue;
  int32_t signedValue;
  uint32_t boolValue;
  char stringValue[LEN_ZONE_OPTION_STRING];
};

enum ZoneOptionValueEnum {
  ZOV_Unsigned=0,
  ZOV_Signed,
  ZOV_Bool,
  ZOV_String
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

struct ZoneOptionValueTyped
{
  ZoneOptionValueEnum type;
  ZoneOptionValue     value FUNC(select_zov);
};


inline ZoneOptionValueEnum zoneValueEnumFromType(ZoneOption::Type type)
{
  switch(type) {
  case ZoneOption::File:
  case ZoneOption::String:
    return ZOV_String;

  case ZoneOption::Integer:
    return ZOV_Signed;

  case ZoneOption::Bool:
    return ZOV_Bool;

  case ZoneOption::Color:
  case ZoneOption::Timer:
  case ZoneOption::Switch:
  case ZoneOption::Source:
  case ZoneOption::TextSize:
  default:
    return ZOV_Unsigned;
  }
}

#endif
