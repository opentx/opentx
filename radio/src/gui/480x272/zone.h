
#ifndef _ZONE_H_
#define _ZONE_H_

#include <inttypes.h>
#include <string.h>

#define LEN_ZONE_OPTION_STRING         8

#define OPTION_VALUE_UNSIGNED(x)    setZoneOptionValue((uint32_t)x)
#define OPTION_VALUE_SIGNED(x)      setZoneOptionValue((int32_t)x)
#define OPTION_VALUE_BOOL(x)        setZoneOptionValue((bool)x)
#define OPTION_VALUE_STRING(x)      setZoneOptionValue(x)

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

ZoneOptionValue setZoneOptionValue(uint32_t v);
ZoneOptionValue setZoneOptionValue(int32_t v);
ZoneOptionValue setZoneOptionValue(bool v);
ZoneOptionValue setZoneOptionValue(const char * v);

#endif
