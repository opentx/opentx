#include "zone.h"

ZoneOptionValue setZoneOptionValue(uint32_t v)
{
  ZoneOptionValue ov;
  ov.unsignedValue = v;
  return ov;
}

ZoneOptionValue setZoneOptionValue(int32_t v)
{
  ZoneOptionValue ov;
  ov.signedValue = v;
  return ov;
}

ZoneOptionValue setZoneOptionValue(bool v)
{
  ZoneOptionValue ov;
  ov.boolValue = v;
  return ov; }

ZoneOptionValue setZoneOptionValue(const char * v)
{
  ZoneOptionValue ov;
  strncpy(ov.stringValue, v, LEN_ZONE_OPTION_STRING);
  return ov;
}
