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

#include <ctype.h>
#include <stdio.h>
#include "opentx.h"
#include "stamp.h"
#include "lua_api.h"
#include "telemetry/frsky.h"

#if defined(PCBX12S)
  #include "lua/lua_exports_x12s.inc"   // this line must be after lua headers
#elif defined(PCBX10)
  #include "lua/lua_exports_x10.inc"
#elif defined(PCBX9E)
  #include "lua/lua_exports_x9e.inc"
#elif defined(PCBX7)
  #include "lua/lua_exports_x7.inc"
#elif defined(PCBXLITE)
  #include "lua/lua_exports_xlite.inc"
#elif defined(PCBTARANIS)
  #include "lua/lua_exports_x9d.inc"
#endif

#if defined(SIMU)
  #define RADIO_VERSION FLAVOUR "-simu"
#else
  #define RADIO_VERSION FLAVOUR
#endif

#define FIND_FIELD_DESC  0x01

/*luadoc
@function getVersion()

Return OpenTX version

@retval string OpenTX version (ie "2.1.5")

@retval multiple (available since 2.1.7) returns 5 values:
 * (string) OpenTX version (ie "2.1.5")
 * (string) radio type: `x12s`, `x10`, `x9e`, `x9d+`, `x9d` or `x7`.
If running in simulator the "-simu" is added
 * (number) major version (ie 2 if version 2.1.5)
 * (number) minor version (ie 1 if version 2.1.5)
 * (number) revision number (ie 5 if version 2.1.5)

@status current Introduced in 2.0.0, expanded in 2.1.7, radio type strings changed in 2.2.0

### Example

This example also runs in OpenTX versions where the function returned only one value:

```lua
local function run(event)
  local ver, radio, maj, minor, rev = getVersion()
  print("version: "..ver)
  if radio then print ("radio: "..radio) end
  if maj then print ("maj: "..maj) end
  if minor then print ("minor: "..minor) end
  if rev then print ("rev: "..rev) end
  return 1
end

return {  run=run }
```
Output of the above script in simulator:
```
version: 2.1.7
radio: taranis-simu
maj: 2
minor: 1
rev: 7
```
*/
static int luaGetVersion(lua_State * L)
{
  lua_pushstring(L, VERSION);
  lua_pushstring(L, RADIO_VERSION);
  lua_pushnumber(L, VERSION_MAJOR);
  lua_pushnumber(L, VERSION_MINOR);
  lua_pushnumber(L, VERSION_REVISION);
  return 5;
}

/*luadoc
@function getTime()

Return the time since the radio was started in multiple of 10ms

@retval number Number of 10ms ticks since the radio was started Example:
run time: 12.54 seconds, return value: 1254

The timer internally uses a 32-bit counter which is enough for 30 years so
overflows will not happen.

@status current Introduced in 2.0.0
*/
static int luaGetTime(lua_State * L)
{
  lua_pushunsigned(L, get_tmr10ms());
  return 1;
}

static void luaPushDateTime(lua_State * L, uint32_t year, uint32_t mon, uint32_t day,
                            uint32_t hour, uint32_t min, uint32_t sec)
{
  uint32_t hour12 = hour;

  if (hour == 0) {
    hour12 = 12;
  }
  else if (hour > 12) {
    hour12 = hour - 12;
  }
  lua_createtable(L, 0, 8);
  lua_pushtableinteger(L, "year", year);
  lua_pushtableinteger(L, "mon", mon);
  lua_pushtableinteger(L, "day", day);
  lua_pushtableinteger(L, "hour", hour);
  lua_pushtableinteger(L, "min", min);
  lua_pushtableinteger(L, "sec", sec);
  lua_pushtableinteger(L, "hour12", hour12);
  if (hour < 12) {
    lua_pushtablestring(L, "suffix", "am");
  }
  else {
    lua_pushtablestring(L, "suffix", "pm");
  }
}

/*luadoc
@function getDateTime()

Return current system date and time that is kept by the RTC unit

@retval table current date and time, table elements:
 * `year` (number) year
 * `mon` (number) month
 * `day` (number) day of month
 * `hour` (number) hours
 * `hour12` (number) hours in US format
 * `min` (number) minutes
 * `sec` (number) seconds
 * `suffix` (text) am or pm
*/
static int luaGetDateTime(lua_State * L)
{
  struct gtm utm;
  gettime(&utm);
  luaPushDateTime(L, utm.tm_year + TM_YEAR_BASE, utm.tm_mon + 1, utm.tm_mday, utm.tm_hour, utm.tm_min, utm.tm_sec);
  return 1;
}

/*luadoc
@function getRtcTime()

Return current RTC system date as unix timstamp (in seconds since 1. Jan 1970)

Please note the RTC timestamp is kept internally as a 32bit integer, which will overflow
in 2038.

@retval number Number of seconds elapsed since 1. Jan 1970
*/

#if defined(RTCLOCK)
static int luaGetRtcTime(lua_State * L)
{
  lua_pushunsigned(L, g_rtcTime);
  return 1;
}
#endif

static void luaPushLatLon(lua_State* L, TelemetrySensor & telemetrySensor, TelemetryItem & telemetryItem)
/* result is lua table containing members ["lat"] and ["lon"] as lua_Number (doubles) in decimal degrees */
{
  lua_createtable(L, 0, 4);
  lua_pushtablenumber(L, "lat", telemetryItem.gps.latitude * 0.000001); // floating point multiplication is faster than division
  lua_pushtablenumber(L, "pilot-lat", telemetryItem.pilotLatitude * 0.000001);
  lua_pushtablenumber(L, "lon", telemetryItem.gps.longitude * 0.000001);
  lua_pushtablenumber(L, "pilot-lon", telemetryItem.pilotLongitude * 0.000001);
}

static void luaPushTelemetryDateTime(lua_State* L, TelemetrySensor & telemetrySensor, TelemetryItem & telemetryItem)
{
  luaPushDateTime(L, telemetryItem.datetime.year, telemetryItem.datetime.month, telemetryItem.datetime.day,
                  telemetryItem.datetime.hour, telemetryItem.datetime.min, telemetryItem.datetime.sec);
}

static void luaPushCells(lua_State* L, TelemetrySensor & telemetrySensor, TelemetryItem & telemetryItem)
{
  if (telemetryItem.cells.count == 0)
    lua_pushinteger(L, (int)0); // returns zero if no cells
  else {
    lua_createtable(L, telemetryItem.cells.count, 0);
    for (int i = 0; i < telemetryItem.cells.count; i++) {
      lua_pushnumber(L, i + 1);
      lua_pushnumber(L, telemetryItem.cells.values[i].value * 0.01f);
      lua_settable(L, -3);
    }
  }
}

void luaGetValueAndPush(lua_State* L, int src)
{
  getvalue_t value = getValue(src); // ignored for GPS, DATETIME, and CELLS

  if (src >= MIXSRC_FIRST_TELEM && src <= MIXSRC_LAST_TELEM) {
    div_t qr = div(src-MIXSRC_FIRST_TELEM, 3);
    // telemetry values
    if (TELEMETRY_STREAMING() && telemetryItems[qr.quot].isAvailable()) {
      TelemetrySensor & telemetrySensor = g_model.telemetrySensors[qr.quot];
      switch (telemetrySensor.unit) {
        case UNIT_GPS:
          luaPushLatLon(L, telemetrySensor, telemetryItems[qr.quot]);
          break;
        case UNIT_DATETIME:
          luaPushTelemetryDateTime(L, telemetrySensor, telemetryItems[qr.quot]);
          break;
        case UNIT_TEXT:
          lua_pushstring(L, telemetryItems[qr.quot].text);
          break;
        case UNIT_CELLS:
          if (qr.rem == 0) {
            luaPushCells(L, telemetrySensor, telemetryItems[qr.quot]);
            break;
          }
          // deliberate no break here to properly return `Cels-` and `Cels+`
        default:
          if (telemetrySensor.prec > 0)
            lua_pushnumber(L, float(value)/telemetrySensor.getPrecDivisor());
          else
            lua_pushinteger(L, value);
          break;
      }
    }
    else {
      // telemetry not working, return zero for telemetry sources
      lua_pushinteger(L, (int)0);
    }
  }
  else if (src == MIXSRC_TX_VOLTAGE) {
    lua_pushnumber(L, float(value) * 0.1f);
  }
  else {
    lua_pushinteger(L, value);
  }
}

/**
  Return field data for a given field name
*/
bool luaFindFieldByName(const char * name, LuaField & field, unsigned int flags)
{
  // TODO better search method (binary lookup)
  for (unsigned int n=0; n<DIM(luaSingleFields); ++n) {
    if (!strcmp(name, luaSingleFields[n].name)) {
      field.id = luaSingleFields[n].id;
      if (flags & FIND_FIELD_DESC) {
        strncpy(field.desc, luaSingleFields[n].desc, sizeof(field.desc)-1);
        field.desc[sizeof(field.desc)-1] = '\0';
      }
      else {
        field.desc[0] = '\0';
      }
      return true;
    }
  }

  // search in multiples
  unsigned int len = strlen(name);
  for (unsigned int n=0; n<DIM(luaMultipleFields); ++n) {
    const char * fieldName = luaMultipleFields[n].name;
    unsigned int fieldLen = strlen(fieldName);
    if (!strncmp(name, fieldName, fieldLen)) {
      unsigned int index;
      if (len == fieldLen+1 && isdigit(name[fieldLen])) {
        index = name[fieldLen] - '1';
      }
      else if (len == fieldLen+2 && isdigit(name[fieldLen]) && isdigit(name[fieldLen+1])) {
        index = 10 * (name[fieldLen] - '0') + (name[fieldLen+1] - '1');
      }
      else {
        continue;
      }
      if (index < luaMultipleFields[n].count) {
        field.id = luaMultipleFields[n].id + index;
        if (flags & FIND_FIELD_DESC) {
          snprintf(field.desc, sizeof(field.desc)-1, luaMultipleFields[n].desc, index+1);
          field.desc[sizeof(field.desc)-1] = '\0';
        }
        else {
          field.desc[0] = '\0';
        }
        return true;
      }
    }
  }

  // search in telemetry
  field.desc[0] = '\0';
  for (int i=0; i<MAX_TELEMETRY_SENSORS; i++) {
    if (isTelemetryFieldAvailable(i)) {
      char sensorName[TELEM_LABEL_LEN+1];
      int len = zchar2str(sensorName, g_model.telemetrySensors[i].label, TELEM_LABEL_LEN);
      if (!strncmp(sensorName, name, len)) {
        if (name[len] == '\0') {
          field.id = MIXSRC_FIRST_TELEM + 3*i;
          field.desc[0] = '\0';
          return true;
        }
        else if (name[len] == '-' && name[len+1] == '\0') {
          field.id = MIXSRC_FIRST_TELEM + 3*i + 1;
          field.desc[0] = '\0';
          return true;
        }
        else if (name[len] == '+' && name[len+1] == '\0') {
          field.id = MIXSRC_FIRST_TELEM + 3*i + 2;
          field.desc[0] = '\0';
          return true;
        }
      }
    }
  }

  return false;  // not found
}

/*luadoc
@function sportTelemetryPop()

Pops a received SPORT packet from the queue. Please note that only packets using a data ID within 0x5000 to 0x52FF
(frame ID == 0x10), as well as packets with a frame ID equal 0x32 (regardless of the data ID) will be passed to
the LUA telemetry receive queue.

@retval nil queue does not contain any (or enough) bytes to form a whole packet

@retval multiple returns 4 values:
 * sensor ID (number)
 * frame ID (number)
 * data ID (number)
 * value (number)

@status current Introduced in 2.2.0
*/
static int luaSportTelemetryPop(lua_State * L)
{
  if (!luaInputTelemetryFifo) {
    luaInputTelemetryFifo = new Fifo<uint8_t, LUA_TELEMETRY_INPUT_FIFO_SIZE>();
    if (!luaInputTelemetryFifo) {
      return 0;
    }
  }

  if (luaInputTelemetryFifo->size() >= sizeof(SportTelemetryPacket)) {
    SportTelemetryPacket packet;
    for (uint8_t i=0; i<sizeof(packet); i++) {
      luaInputTelemetryFifo->pop(packet.raw[i]);
    }
    lua_pushnumber(L, packet.physicalId);
    lua_pushnumber(L, packet.primId);
    lua_pushnumber(L, packet.dataId);
    lua_pushunsigned(L, packet.value);
    return 4;
  }

  return 0;
}

#define BIT(x, index) (((x) >> index) & 0x01)
uint8_t getDataId(uint8_t physicalId)
{
  uint8_t result = physicalId;
  result += (BIT(physicalId, 0) ^ BIT(physicalId, 1) ^ BIT(physicalId, 2)) << 5;
  result += (BIT(physicalId, 2) ^ BIT(physicalId, 3) ^ BIT(physicalId, 4)) << 6;
  result += (BIT(physicalId, 0) ^ BIT(physicalId, 2) ^ BIT(physicalId, 4)) << 7;
  return result;
}

/*luadoc
@function sportTelemetryPush()

This functions allows for sending SPORT telemetry data toward the receiver,
and more generally, to anything connected SPORT bus on the receiver or transmitter.

When called without parameters, it will only return the status of the output buffer without sending anything.

@param sensorId  physical sensor ID

@param frameId   frame ID

@param dataId    data ID

@param value     value

@retval boolean  data queued in output buffer or not.

@status current Introduced in 2.2.0
*/
static int luaSportTelemetryPush(lua_State * L)
{
  if (lua_gettop(L) == 0) {
    lua_pushboolean(L, isSportOutputBufferAvailable());
  }
  else if (isSportOutputBufferAvailable()) {
    SportTelemetryPacket packet;
    packet.physicalId = getDataId(luaL_checkunsigned(L, 1));
    packet.primId = luaL_checkunsigned(L, 2);
    packet.dataId = luaL_checkunsigned(L, 3);
    packet.value = luaL_checkunsigned(L, 4);
    sportOutputPushPacket(&packet);
    lua_pushboolean(L, true);
  }
  else {
    lua_pushboolean(L, false);
  }
  return 1;
}

/*luadoc
@function crossfireTelemetryPop()

Pops a received Crossfire Telemetry packet from the queue.

@retval nil queue does not contain any (or enough) bytes to form a whole packet

@retval multiple returns 2 values:
 * command (number)
 * packet (table) data bytes

@status current Introduced in 2.2.0
*/
static int luaCrossfireTelemetryPop(lua_State * L)
{
  if (!luaInputTelemetryFifo) {
    luaInputTelemetryFifo = new Fifo<uint8_t, LUA_TELEMETRY_INPUT_FIFO_SIZE>();
    if (!luaInputTelemetryFifo) {
      return 0;
    }
  }

  uint8_t length = 0, data = 0;
  if (luaInputTelemetryFifo->probe(length) && luaInputTelemetryFifo->size() >= uint32_t(length)) {
    // length value includes the length field
    luaInputTelemetryFifo->pop(length);
    luaInputTelemetryFifo->pop(data); // command
    lua_pushnumber(L, data);
    lua_newtable(L);
    for (uint8_t i=1; i<length-1; i++) {
      luaInputTelemetryFifo->pop(data);
      lua_pushinteger(L, i);
      lua_pushinteger(L, data);
      lua_settable(L, -3);
    }
    return 2;
  }

  return 0;
}

/*luadoc
@function crossfireTelemetryPush()

This functions allows for sending telemetry data toward the TBS Crossfire link.

When called without parameters, it will only return the status of the output buffer without sending anything.

@param command command

@param data table of data bytes

@retval boolean  data queued in output buffer or not.

@status current Introduced in 2.2.0
*/
static int luaCrossfireTelemetryPush(lua_State * L)
{
  if (lua_gettop(L) == 0) {
    lua_pushboolean(L, isCrossfireOutputBufferAvailable());
  }
  else if (isCrossfireOutputBufferAvailable()) {
    uint8_t command = luaL_checkunsigned(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    uint8_t length = luaL_len(L, 2);
    telemetryOutputPushByte(MODULE_ADDRESS);
    telemetryOutputPushByte(2 + length); // 1(COMMAND) + data length + 1(CRC)
    telemetryOutputPushByte(command); // COMMAND
    for (int i=0; i<length; i++) {
      lua_rawgeti(L, 2, i+1);
      telemetryOutputPushByte(luaL_checkunsigned(L, -1));
    }
    telemetryOutputPushByte(crc8(outputTelemetryBuffer+2, 1 + length));
    telemetryOutputSetTrigger(command);
    lua_pushboolean(L, true);
  }
  else {
    lua_pushboolean(L, false);
  }
  return 1;
}

/*luadoc
@function getFieldInfo(name)

Return detailed information about field (source)

The list of valid sources is available:

| OpenTX Version | Radio |
|----------------|-------|
| 2.0 | [all](http://downloads-20.open-tx.org/firmware/lua_fields.txt) |
| 2.1 | [X9D and X9D+](http://downloads-21.open-tx.org/firmware/lua_fields_taranis.txt), [X9E](http://downloads-21.open-tx.org/firmware/lua_fields_taranis_x9e.txt) |
| 2.2 | [X9D and X9D+](http://downloads.open-tx.org/2.2/firmware/lua_fields_x9d.txt), [X9E](http://downloads.open-tx.org/2.2/firmware/lua_fields_x9e.txt), [Horus](http://downloads.open-tx.org/2.2/firmware/lua_fields_x12s.txt) |

@param name (string) name of the field

@retval table information about requested field, table elements:
 * `id`   (number) field identifier
 * `name` (string) field name
 * `desc` (string) field description
 * 'unit' (number) unit identifier [Full list](../appendix/units.html)

@retval nil the requested field was not found

@status current Introduced in 2.0.8, 'unit' field added in 2.2.0
*/
static int luaGetFieldInfo(lua_State * L)
{
  const char * what = luaL_checkstring(L, 1);
  LuaField field;
  bool found = luaFindFieldByName(what, field, FIND_FIELD_DESC);
  if (found) {
    lua_newtable(L);
    lua_pushtableinteger(L, "id", field.id);
    lua_pushtablestring(L, "name", what);
    lua_pushtablestring(L, "desc", field.desc);
    if (field.id >= MIXSRC_FIRST_TELEM && field.id <= MIXSRC_LAST_TELEM) {
      TelemetrySensor & telemetrySensor = g_model.telemetrySensors[(int)((field.id-MIXSRC_FIRST_TELEM)/3)];
      lua_pushtableinteger(L, "unit", telemetrySensor.unit);
    }
    else {
      lua_pushtablenil(L, "unit");
    }
    return 1;
  }
  return 0;
}

/*luadoc
@function getValue(source)

Returns the value of a source.

The list of fixed sources:

| OpenTX Version | Radio |
|----------------|-------|
| 2.0 | [all](http://downloads-20.open-tx.org/firmware/lua_fields.txt) |
| 2.1 | [X9D and X9D+](http://downloads-21.open-tx.org/firmware/lua_fields_taranis.txt), [X9E](http://downloads-21.open-tx.org/firmware/lua_fields_taranis_x9e.txt) |
| 2.2 | [X9D and X9D+](http://downloads.open-tx.org/2.2/firmware/lua_fields_x9d.txt), [X9E](http://downloads.open-tx.org/2.2/firmware/lua_fields_x9e.txt), [Horus](http://downloads.open-tx.org/2.2/firmware/lua_fields_x12s.txt) |


In OpenTX 2.1.x the telemetry sources no longer have a predefined name.
To get a telemetry value simply use it's sensor name. For example:
 * Altitude sensor has a name "Alt"
 * to get the current altitude use the source "Alt"
 * to get the minimum altitude use the source "Alt-", to get the maximum use "Alt+"

@param source  can be an identifier (number) (which was obtained by the getFieldInfo())
or a name (string) of the source.

@retval value current source value (number). Zero is returned for:
 * non-existing sources
 * for all telemetry source when the telemetry stream is not received
 * far all non allowed sensors while FAI MODE is active

@retval table GPS position is returned in a table:
 * `lat` (number) latitude, positive is North
 * `lon` (number) longitude, positive is East
 * `pilot-lat` (number) pilot latitude, positive is North
 * `pilot-lon` (number) pilot longitude, positive is East

@retval table GPS date/time, see getDateTime()

@retval table Cells are returned in a table
(except where no cells were detected in which
case the returned value is 0):
 * table has one item for each detected cell:
  * key (number) cell number (1 to number of cells)
  * value (number) current cell voltage

@status current Introduced in 2.0.0, changed in 2.1.0, `Cels+` and
`Cels-` added in 2.1.9

@notice Getting a value by its numerical identifier is faster then by its name.
While `Cels` sensor returns current values of all cells in a table, a `Cels+` or
`Cels-` will return a single value - the maximum or minimum Cels value.
*/
static int luaGetValue(lua_State * L)
{
  int src = 0;
  if (lua_isnumber(L, 1)) {
    src = luaL_checkinteger(L, 1);
  }
  else {
    // convert from field name to its id
    const char *name = luaL_checkstring(L, 1);
    LuaField field;
    bool found = luaFindFieldByName(name, field);
    if (found) {
      src = field.id;
    }
  }
  luaGetValueAndPush(L, src);
  return 1;
}

/*luadoc
@function getRAS()

Return the RAS value or nil if no valid hardware found

@retval number representing RAS value. Value bellow 0x33 (51 decimal) are all ok, value above 0x33 indicate a hardware antenna issue.
This is just a hardware pass/fail measure and does not represent the quality of the radio link

@notice RAS was called SWR in the past

@status current Introduced in 2.2.0
*/
static int luaGetRAS(lua_State * L)
{
  if (IS_RAS_VALUE_VALID()) {
    lua_pushinteger(L, telemetryData.swr.value);
  }
  else {
    lua_pushnil(L);
  }
  return 1;
}

/*luadoc
@function getTxGPS()

Return the internal GPS position or nil if no valid hardware found

@retval table representing the current radio position
 * `lat` (number) internal GPS latitude, positive is North
 * `lon` (number) internal GPS longitude, positive is East
 * 'numsat' (number) current number of sats locked in by the GPS sensor
 * 'fix' (boolean) fix status
 * 'alt' (number) internal GPS altitude in 0.1m
 * 'speed' (number) internal GPSspeed in 0.1m/s
 * 'heading'  (number) internal GPS ground course estimation in degrees * 10

@status current Introduced in 2.2.2
*/
static int luaGetTxGPS(lua_State * L)
{
#if defined(INTERNAL_GPS)
  lua_createtable(L, 0, 7);
  lua_pushtablenumber(L, "lat", gpsData.latitude * 0.000001);
  lua_pushtablenumber(L, "lon", gpsData.longitude * 0.000001);
  lua_pushtableinteger(L, "numsat", gpsData.numSat);
  lua_pushtableinteger(L, "alt", gpsData.altitude);
  lua_pushtableinteger(L, "speed", gpsData.speed);
  lua_pushtableinteger(L, "heading", gpsData.groundCourse);
  if (gpsData.fix)
    lua_pushtableboolean(L, "fix", true);
  else
    lua_pushtableboolean(L, "fix", false);
#else
    lua_pushnil(L);
#endif
  return 1;
}


/*luadoc
@function getFlightMode(mode)

Return flight mode data.

@param mode (number) flight mode number to return (0 - 8). If mode parameter
is not specified (or contains invalid value), then the current flight mode data is returned.

@retval multiple returns 2 values:
 * (number) (current) flight mode number (0 - 8)
 * (string) (current) flight mode name

@status current Introduced in 2.1.7
*/
static int luaGetFlightMode(lua_State * L)
{
  int mode = luaL_optinteger(L, 1, -1);
  if (mode < 0 || mode >= MAX_FLIGHT_MODES) {
    mode = mixerCurrentFlightMode;
  }
  lua_pushnumber(L, mode);
  char name[sizeof(g_model.flightModeData[0].name)+1];
  zchar2str(name, g_model.flightModeData[mode].name, sizeof(g_model.flightModeData[0].name));
  lua_pushstring(L, name);
  return 2;
}

/*luadoc
@function playFile(name)

Play a file from the SD card

@param path (string) full path to wav file (i.e. “/SOUNDS/en/system/tada.wav”)
Introduced in 2.1.0: If you use a relative path, the current language is appended
to the path (example: for English language: `/SOUNDS/en` is appended)

@status current Introduced in 2.0.0, changed in 2.1.0
*/
static int luaPlayFile(lua_State * L)
{
  const char * filename = luaL_checkstring(L, 1);
  if (filename[0] != '/') {
    // relative sound file path - use current language dir for absolute path
    char file[AUDIO_FILENAME_MAXLEN+1];
    char * str = getAudioPath(file);
    strncpy(str, filename, AUDIO_FILENAME_MAXLEN - (str-file));
    file[AUDIO_FILENAME_MAXLEN] = 0;
    PLAY_FILE(file, 0, 0);
  }
  else {
    PLAY_FILE(filename, 0, 0);
  }
  return 0;
}

/*luadoc
@function playNumber(value, unit [, attributes])

Play a numerical value (text to speech)

@param value (number) number to play. Value is interpreted as integer.

@param unit (number) unit identifier [Full list]((../appendix/units.html))

@param attributes (unsigned number) possible values:
 * `0 or not present` plays integral part of the number (for a number 123 it plays 123)
 * `PREC1` plays a number with one decimal place (for a number 123 it plays 12.3)
 * `PREC2` plays a number with two decimal places (for a number 123 it plays 1.23)

@status current Introduced in 2.0.0

*/
static int luaPlayNumber(lua_State * L)
{
  int number = luaL_checkinteger(L, 1);
  int unit = luaL_checkinteger(L, 2);
  unsigned int att = luaL_optunsigned(L, 3, 0);
  playNumber(number, unit, att, 0);
  return 0;
}

/*luadoc
@function playDuration(duration [, hourFormat])

Play a time value (text to speech)

@param duration (number) number of seconds to play. Only integral part is used.

@param hourFormat (number):
 * `0 or not present` play format: minutes and seconds.
 * `!= 0` play format: hours, minutes and seconds.

@status current Introduced in 2.1.0
*/
static int luaPlayDuration(lua_State * L)
{
  int duration = luaL_checkinteger(L, 1);
  bool playTime = (luaL_optinteger(L, 2, 0) != 0);
  playDuration(duration, playTime ? PLAY_TIME : 0, 0);
  return 0;
}

/*luadoc
@function playTone(frequency, duration, pause [, flags [, freqIncr]])

Play a tone

@param frequency (number) tone frequency in Hz (from 150 to 15000)

@param duration (number) length of the tone in milliseconds

@param pause (number) length of the silence after the tone in milliseconds

@param flags (number):
 * `0 or not present` play with normal priority.
 * `PLAY_BACKGROUND` play in background (built in vario function uses this context)
 * `PLAY_NOW` play immediately

@param freqIncr (number) positive number increases the tone pitch (frequency with time),
negative number decreases it. The frequency changes every 10 milliseconds, the change is `freqIncr * 10Hz`.
The valid range is from -127 to 127.

@status current Introduced in 2.1.0
*/
static int luaPlayTone(lua_State * L)
{
  int frequency = luaL_checkinteger(L, 1);
  int length = luaL_checkinteger(L, 2);
  int pause = luaL_checkinteger(L, 3);
  int flags = luaL_optinteger(L, 4, 0);
  int freqIncr = luaL_optinteger(L, 5, 0);
  audioQueue.playTone(frequency, length, pause, flags, freqIncr);
  return 0;
}

/*luadoc
@function playHaptic(duration, pause [, flags])

Generate haptic feedback

@param duration (number) length of the haptic feedback in milliseconds

@param pause (number) length of the silence after haptic feedback in milliseconds

@param flags (number):
 * `0 or not present` play with normal priority
 * `PLAY_NOW` play immediately

@status current Introduced in 2.2.0
*/
static int luaPlayHaptic(lua_State * L)
{
#if defined(HAPTIC)
  int length = luaL_checkinteger(L, 1);
  int pause = luaL_checkinteger(L, 2);
  int flags = luaL_optinteger(L, 3, 0);
  haptic.play(length, pause, flags);
#else
  UNUSED(L);
#endif
  return 0;
}

/*luadoc
@function killEvents(key)

Stops key state machine. See [Key Events](../key_events.md) for the detailed description.

@param key (number) key to be killed, can also include event type (only the key part is used)

@status current Introduced in 2.0.0

*/
static int luaKillEvents(lua_State * L)
{
  uint8_t key = EVT_KEY_MASK(luaL_checkinteger(L, 1));
  // prevent killing maskable keys (only in telemetry scripts)
  // TODO add which tpye of script is running before p_call()
  if (IS_MASKABLE(key)) {
    killEvents(key);
  }
  return 0;
}

#if LCD_DEPTH > 1 && !defined(COLORLCD)
/*luadoc
@function GREY()

Returns gray value which can be used in LCD functions

@retval (number) a value that represents amount of *greyness* (from 0 to 15)

@notice Only available on Taranis

@status current Introduced in 2.0.13
*/
static int luaGrey(lua_State * L)
{
  int index = luaL_checkinteger(L, 1);
  lua_pushunsigned(L, GREY(index));
  return 1;
}
#endif

/*luadoc
@function getGeneralSettings()

Returns (some of) the general radio settings

@retval table with elements:
 * `battWarn` (number) radio battery range - warning value
 * `battMin` (number) radio battery range - minimum value
 * `battMax` (number) radio battery range - maximum value
 * `imperial` (number) set to a value different from 0 if the radio is set to the
 IMPERIAL units
 * `language` (string) radio language (used for menus)
 * `voice` (string) voice language (used for speech)
 * `gtimer` (number) radio global timer in seconds (does not include current session)

@status current Introduced in 2.0.6, `imperial` added in TODO,
`language` and `voice` added in 2.2.0, gtimer added in 2.2.2.

*/
static int luaGetGeneralSettings(lua_State * L)
{
  lua_newtable(L);
  lua_pushtablenumber(L, "battWarn", (g_eeGeneral.vBatWarn) * 0.1f);
  lua_pushtablenumber(L, "battMin", (90+g_eeGeneral.vBatMin) * 0.1f);
  lua_pushtablenumber(L, "battMax", (120+g_eeGeneral.vBatMax) * 0.1f);
  lua_pushtableinteger(L, "imperial", g_eeGeneral.imperial);
  lua_pushtablestring(L, "language", TRANSLATIONS);
  lua_pushtablestring(L, "voice", currentLanguagePack->id);
  lua_pushtableinteger(L, "gtimer", g_eeGeneral.globalTimer);
  return 1;
}

/*luadoc
@function popupInput(title, event, input, min, max)

Raises a pop-up on screen that allows uses input

@param title (string) text to display

@param event (number) the event variable that is passed in from the
Run function (key pressed)

@param input (number) value that can be adjusted by the +/­- keys

@param min  (number) min value that input can reach (by pressing the -­ key)

@param max  (number) max value that input can reach

@retval number result of the input adjustment

@retval "OK" user pushed ENT key

@retval "CANCEL" user pushed EXIT key

@notice Use only from stand-alone and telemetry scripts.

@status current Introduced in 2.0.0
*/
static int luaPopupInput(lua_State * L)
{
  event_t event = luaL_checkinteger(L, 2);
  warningInputValue = luaL_checkinteger(L, 3);
  warningInputValueMin = luaL_checkinteger(L, 4);
  warningInputValueMax = luaL_checkinteger(L, 5);
  warningText = luaL_checkstring(L, 1);
  warningType = WARNING_TYPE_INPUT;
  runPopupWarning(event);
  if (warningResult) {
    warningResult = 0;
    lua_pushstring(L, "OK");
  }
  else if (!warningText) {
    lua_pushstring(L, "CANCEL");
  }
  else {
    lua_pushinteger(L, warningInputValue);
  }
  warningText = NULL;
  return 1;
}

/*luadoc
@function popupWarning(title, event)

Raises a pop-up on screen that shows a warning

@param title (string) text to display

@param event (number) the event variable that is passed in from the
Run function (key pressed)

@retval "CANCEL" user pushed EXIT key

@notice Use only from stand-alone and telemetry scripts.

@status current Introduced in 2.2.0
*/
static int luaPopupWarning(lua_State * L)
{
  event_t event = luaL_checkinteger(L, 2);
  warningText = luaL_checkstring(L, 1);
  warningType = WARNING_TYPE_ASTERISK;
  runPopupWarning(event);
  if (!warningText) {
    lua_pushstring(L, "CANCEL");
  }
  else {
    warningText = NULL;
    lua_pushnil(L);
  }
  return 1;
}

/*luadoc
@function popupConfirmation(title, event)

Raises a pop-up on screen that asks for confirmation

@param title (string) text to display

@param event (number) the event variable that is passed in from the
Run function (key pressed)

@retval "CANCEL" user pushed EXIT key

@notice Use only from stand-alone and telemetry scripts.

@status current Introduced in 2.2.0
*/
static int luaPopupConfirmation(lua_State * L)
{
  event_t event = luaL_checkinteger(L, 2);
  warningText = luaL_checkstring(L, 1);
  warningType = WARNING_TYPE_CONFIRM;
  runPopupWarning(event);
  if (!warningText) {
    lua_pushstring(L, warningResult ? "OK" : "CANCEL");
  }
  else {
    warningText = NULL;
    lua_pushnil(L);
  }
  return 1;
}

/*luadoc
@function defaultStick(channel)

Get stick that is assigned to a channel. See Default Channel Order in General Settings.

@param channel (number) channel number (0 means CH1)

@retval number Stick assigned to this channel (from 0 to 3)

@status current Introduced in 2.0.0
*/
static int luaDefaultStick(lua_State * L)
{
  uint8_t channel = luaL_checkinteger(L, 1);
  lua_pushinteger(L, channel_order(channel+1)-1);
  return 1;
}

/*luadoc
@function setTelemetryValue(id, subID, instance, value [, unit [, precision [, name]]])

@param id Id of the sensor, valid range is from 0 to 0xFFFF

@param subID subID of the sensor, usually 0, valid range is from 0 to 7

@param instance instance of the sensor (SensorID), valid range is from 0 to 0xFF

@param value fed to the sensor

@param unit unit of the sensor [Full list](../appendix/units.html)

@param precision the precision of the sensor
 * `0 or not present` no decimal precision.
 * `!= 0` value is divided by 10^precision, e.g. value=1000, prec=2 => 10.00.

@param name (string) Name of the sensor if it does not yet exist (4 chars).
 * `not present` Name defaults to the Id.
 * `present` Sensor takes name of the argument. Argument must have name surrounded by quotes: e.g., "Name"

@retval true, if the sensor was just added. In this case the value is ignored (subsequent call will set the value)

@notice All three parameters `id`, `subID` and `instance` can't be zero at the same time. At least one of them
must be different from zero.

@status current Introduced in 2.2.0
*/
static int luaSetTelemetryValue(lua_State * L)
{
  uint16_t id = luaL_checkunsigned(L, 1);
  uint8_t subId = luaL_checkunsigned(L, 2) & 0x7;
  uint8_t instance = luaL_checkunsigned(L, 3);
  int32_t value = luaL_checkinteger(L, 4);
  uint32_t unit = luaL_optunsigned(L, 5, 0);
  uint32_t prec = luaL_optunsigned(L, 6, 0);

  char zname[4];
  const char* name = luaL_optstring(L, 7, NULL);
  if (name != NULL && strlen(name) > 0) {
    str2zchar(zname, name, 4);
  } else {
    zname[0] = hex2zchar((id & 0xf000) >> 12);
    zname[1] = hex2zchar((id & 0x0f00) >> 8);
    zname[2] = hex2zchar((id & 0x00f0) >> 4);
    zname[3] = hex2zchar((id & 0x000f) >> 0);
  }
  if (id | subId | instance) {
    int index = setTelemetryValue(TELEM_PROTO_LUA, id, subId, instance, value, unit, prec);
    if (index >= 0) {
      TelemetrySensor &telemetrySensor = g_model.telemetrySensors[index];
      telemetrySensor.id = id;
      telemetrySensor.subId = subId;
      telemetrySensor.instance = instance;
      telemetrySensor.init(zname, unit, prec);
      lua_pushboolean(L, true);
    } else {
      lua_pushboolean(L, false);
    }
    return 1;
  }
  lua_pushboolean(L, false);
  return 1;
}

/*luadoc
@function defaultChannel(stick)

Get channel assigned to stick. See Default Channel Order in General Settings

@param stick (number) stick number (from 0 to 3)

@retval number channel assigned to this stick (from 0 to 3)

@retval nil stick not found

@status current Introduced in 2.0.0
*/
static int luaDefaultChannel(lua_State * L)
{
  uint8_t stick = luaL_checkinteger(L, 1);
  for (int i=1; i<=4; i++) {
    int tmp = channel_order(i) - 1;
    if (tmp == stick) {
      lua_pushinteger(L, i-1);
      return 1;
    }
  }
  lua_pushnil(L);
  return 1;
}

/*luadoc
@function getRSSI()

Get RSSI value as well as low and critical RSSI alarm levels (in dB)

@retval rssi RSSI value (0 if no link)

@retval alarm_low Configured low RSSI alarm level

@retval alarm_crit Configured critical RSSI alarm level

@status current Introduced in 2.2.0
*/
static int luaGetRSSI(lua_State * L)
{
  lua_pushunsigned(L, min((uint8_t)99, TELEMETRY_RSSI()));
  lua_pushunsigned(L, g_model.rssiAlarms.getWarningRssi());
  lua_pushunsigned(L, g_model.rssiAlarms.getCriticalRssi());
  return 3;
}

/*luadoc
@function loadScript(file [, mode], [,env])

Load a Lua script file. This is similar to Lua's own [loadfile()](https://www.lua.org/manual/5.2/manual.html#pdf-loadfile)
API method,  but it uses OpenTx's optional pre-compilation feature to save memory and time during load.

Return values are same as from Lua API loadfile() method: If the script was loaded w/out errors
then the loaded script (or "chunk") is returned as a function. Otherwise, returns nil plus the error message.

@param file (string) Full path and file name of script. The file extension is optional and ignored (see `mode` param to control
  which extension will be used). However, if an extension is specified, it should be ".lua" (or ".luac"), otherwise it is treated
  as part of the file name and the .lua/.luac will be appended to that.

@param mode (string) (optional) Controls whether to force loading the text (.lua) or pre-compiled binary (.luac)
  version of the script. By default OTx will load the newest version and compile a new binary if necessary (overwriting any
  existing .luac version of the same script, and stripping some debug info like line numbers).
  You can use `mode` to control the loading behavior more specifically. Possible values are:
   * `b` only binary.
   * `t` only text.
   * `T` (default on simulator) prefer text but load binary if that is the only version available.
   * `bt` (default on radio) either binary or text, whichever is newer (binary preferred when timestamps are equal).
   * Add `x` to avoid automatic compilation of source file to .luac version.
       Eg: "tx", "bx", or "btx".
   * Add `c` to force compilation of source file to .luac version (even if existing version is newer than source file).
       Eg: "tc" or "btc" (forces "t", overrides "x").
   * Add `d` to keep extra debug info in the compiled binary.
       Eg: "td", "btd", or "tcd" (no effect with just "b" or with "x").

@notice
  Note that you will get an error if you specify `mode` as "b" or "t" and that specific version of the file does not exist (eg. no .luac file when "b" is used).
  Also note that `mode` is NOT passed on to Lua's loader function, so unlike with loadfile() the actual file content is not checked (as if no mode or "bt" were passed to loadfile()).

@param env (integer) See documentation for Lua function loadfile().

@retval function The loaded script, or `nil` if there was an error (e.g. file not found or syntax error).

@retval string Error message(s), if any. Blank if no error occurred.

@status current Introduced in 2.2.0

### Example

```lua
  fun, err = loadScript("/SCRIPTS/FUNCTIONS/print.lua")
  if (fun ~= nil) then
     fun("Hello from loadScript()")
  else
     print(err)
  end
```

*/
static int luaLoadScript(lua_State * L)
{
  // this function is replicated pretty much verbatim from luaB_loadfile() and load_aux() in lbaselib.c
  const char *fname = luaL_optstring(L, 1, NULL);
  const char *mode = luaL_optstring(L, 2, NULL);
  int env = (!lua_isnone(L, 3) ? 3 : 0);  // 'env' index or 0 if no 'env'
  lua_settop(L, 0);
  if (fname != NULL && luaLoadScriptFileToState(L, fname , mode) == SCRIPT_OK) {
    if (env != 0) {  // 'env' parameter?
      lua_pushvalue(L, env);  // environment for loaded function
      if (!lua_setupvalue(L, -2, 1))  // set it as 1st upvalue
        lua_pop(L, 1);  // remove 'env' if not used by previous call
    }
    return 1;
  }
  else {
    // error (message should be on top of the stack)
    if (!lua_isstring(L, -1)) {
      // probably didn't find a file or had some other error before luaL_loadfile() was run
      lua_pushfstring(L, "loadScript(\"%s\", \"%s\") error: File not found", (fname != NULL ? fname : "nul"), (mode != NULL ? mode : "bt"));
    }
    lua_pushnil(L);
    lua_insert(L, -2);  // move nil before error message
    return 2;  // return nil plus error message
  }
}

/*luadoc
@function getUsage()

Get percent of already used Lua instructions in current script execution cycle.

@retval usage (number) a value from 0 to 100 (percent)

@status current Introduced in 2.2.1
*/
static int luaGetUsage(lua_State * L)
{
  lua_pushinteger(L, instructionsPercent);
  return 1;
}

/*luadoc
@function resetGlobalTimer()

Resets the radio global timer to 0.

@status current Introduced in 2.2.2
*/
static int luaResetGlobalTimer(lua_State * L)
{
  g_eeGeneral.globalTimer = 0;
  storageDirty(EE_GENERAL);
  return 0;
}

const luaL_Reg opentxLib[] = {
  { "getTime", luaGetTime },
  { "getDateTime", luaGetDateTime },
#if defined(RTCLOCK)
  { "getRtcTime", luaGetRtcTime },
#endif
  { "getVersion", luaGetVersion },
  { "getGeneralSettings", luaGetGeneralSettings },
  { "getValue", luaGetValue },
  { "getRAS", luaGetRAS },
  { "getTxGPS", luaGetTxGPS },
  { "getFieldInfo", luaGetFieldInfo },
  { "getFlightMode", luaGetFlightMode },
  { "playFile", luaPlayFile },
  { "playNumber", luaPlayNumber },
  { "playDuration", luaPlayDuration },
  { "playTone", luaPlayTone },
  { "playHaptic", luaPlayHaptic },
  { "popupInput", luaPopupInput },
  { "popupWarning", luaPopupWarning },
  { "popupConfirmation", luaPopupConfirmation },
  { "defaultStick", luaDefaultStick },
  { "defaultChannel", luaDefaultChannel },
  { "getRSSI", luaGetRSSI },
  { "killEvents", luaKillEvents },
  { "loadScript", luaLoadScript },
  { "getUsage", luaGetUsage },
  { "resetGlobalTimer", luaResetGlobalTimer },
#if LCD_DEPTH > 1 && !defined(COLORLCD)
  { "GREY", luaGrey },
#endif
  { "sportTelemetryPop", luaSportTelemetryPop },
  { "sportTelemetryPush", luaSportTelemetryPush },
  { "setTelemetryValue", luaSetTelemetryValue },
#if defined(CROSSFIRE)
  { "crossfireTelemetryPop", luaCrossfireTelemetryPop },
  { "crossfireTelemetryPush", luaCrossfireTelemetryPush },
#endif
  { NULL, NULL }  /* sentinel */
};

const luaR_value_entry opentxConstants[] = {
  { "FULLSCALE", RESX },
  { "XXLSIZE", XXLSIZE },
  { "DBLSIZE", DBLSIZE },
  { "MIDSIZE", MIDSIZE },
  { "SMLSIZE", SMLSIZE },
  { "INVERS", INVERS },
  { "BOLD", BOLD },
  { "BLINK", BLINK },
  { "RIGHT", RIGHT },
  { "LEFT", LEFT },
  { "PREC1", PREC1 },
  { "PREC2", PREC2 },
  { "VALUE", INPUT_TYPE_VALUE },
  { "SOURCE", INPUT_TYPE_SOURCE },
  { "REPLACE", MLTPX_REP },
  { "MIXSRC_FIRST_INPUT", MIXSRC_FIRST_INPUT },
  { "MIXSRC_Rud", MIXSRC_Rud },
  { "MIXSRC_Ele", MIXSRC_Ele },
  { "MIXSRC_Thr", MIXSRC_Thr },
  { "MIXSRC_Ail", MIXSRC_Ail },
  { "MIXSRC_SA", MIXSRC_SA },
  { "MIXSRC_SB", MIXSRC_SB },
  { "MIXSRC_SC", MIXSRC_SC },
  { "MIXSRC_SD", MIXSRC_SD },
#if !defined(PCBX7) && !defined(PCBXLITE)
  { "MIXSRC_SE", MIXSRC_SE },
  { "MIXSRC_SG", MIXSRC_SG },
#endif
#if !defined(PCBXLITE)
  { "MIXSRC_SF", MIXSRC_SF },
  { "MIXSRC_SH", MIXSRC_SH },
#endif
  { "MIXSRC_CH1", MIXSRC_CH1 },
  { "SWSRC_LAST", SWSRC_LAST_LOGICAL_SWITCH },
#if defined(COLORLCD)
  { "SHADOWED", SHADOWED },
  { "COLOR", ZoneOption::Color },
  { "BOOL", ZoneOption::Bool },
  { "CUSTOM_COLOR", CUSTOM_COLOR },
  { "TEXT_COLOR", TEXT_COLOR },
  { "TEXT_BGCOLOR", TEXT_BGCOLOR },
  { "TEXT_INVERTED_COLOR", TEXT_INVERTED_COLOR },
  { "TEXT_INVERTED_BGCOLOR", TEXT_INVERTED_BGCOLOR },
  { "LINE_COLOR", LINE_COLOR },
  { "SCROLLBOX_COLOR", SCROLLBOX_COLOR },
  { "MENU_TITLE_BGCOLOR", MENU_TITLE_BGCOLOR },
  { "MENU_TITLE_COLOR", MENU_TITLE_COLOR },
  { "MENU_TITLE_DISABLE_COLOR", MENU_TITLE_DISABLE_COLOR },
  { "ALARM_COLOR", ALARM_COLOR },
  { "WARNING_COLOR", WARNING_COLOR },
  { "TEXT_DISABLE_COLOR", TEXT_DISABLE_COLOR },
  { "HEADER_COLOR", HEADER_COLOR },
  { "CURVE_AXIS_COLOR", CURVE_AXIS_COLOR },
  { "CURVE_COLOR", CURVE_COLOR },
  { "CURVE_CURSOR_COLOR", CURVE_CURSOR_COLOR },
  { "TITLE_BGCOLOR", TITLE_BGCOLOR },
  { "TRIM_BGCOLOR", TRIM_BGCOLOR },
  { "TRIM_SHADOW_COLOR", TRIM_SHADOW_COLOR },
  { "MAINVIEW_PANES_COLOR", MAINVIEW_PANES_COLOR },
  { "MAINVIEW_GRAPHICS_COLOR", MAINVIEW_GRAPHICS_COLOR },
  { "HEADER_BGCOLOR", HEADER_BGCOLOR },
  { "HEADER_ICON_BGCOLOR", HEADER_ICON_BGCOLOR },
  { "HEADER_CURRENT_BGCOLOR", HEADER_CURRENT_BGCOLOR },
  { "OVERLAY_COLOR", OVERLAY_COLOR },
  { "MENU_HEADER_HEIGHT", MENU_HEADER_HEIGHT },
  { "WHITE", (double)WHITE },
  { "GREY", (double)GREY },
  { "DARKGREY", (double)DARKGREY },
  { "BLACK", (double)BLACK },
  { "YELLOW", (double)YELLOW },
  { "BLUE", (double)BLUE },
  { "LIGHTGREY", (double)LIGHTGREY },
  { "RED", (double)RED },
  { "DARKRED", (double)DARKRED },
#else
  { "FIXEDWIDTH", FIXEDWIDTH },
#endif
#if defined(PCBHORUS)
  { "EVT_PAGEUP_FIRST",  EVT_KEY_FIRST(KEY_PGUP) },
  { "EVT_PAGEDN_FIRST",  EVT_KEY_FIRST(KEY_PGDN) },
  { "EVT_TELEM_FIRST",  EVT_KEY_FIRST(KEY_TELEM) },
  { "EVT_MODEL_FIRST",  EVT_KEY_FIRST(KEY_MODEL) },
  { "EVT_SYS_FIRST",  EVT_KEY_FIRST(KEY_RADIO) },
  { "EVT_RTN_FIRST",  EVT_KEY_FIRST(KEY_EXIT) },
#elif defined(PCBXLITE)
  { "EVT_DOWN_FIRST", EVT_KEY_FIRST(KEY_DOWN) },
  { "EVT_UP_FIRST", EVT_KEY_FIRST(KEY_UP) },
  { "EVT_LEFT_FIRST", EVT_KEY_FIRST(KEY_LEFT) },
  { "EVT_RIGHT_FIRST", EVT_KEY_FIRST(KEY_RIGHT) },
  { "EVT_SHIFT_FIRST", EVT_KEY_FIRST(KEY_SHIFT) },
  { "EVT_DOWN_BREAK", EVT_KEY_BREAK(KEY_DOWN) },
  { "EVT_UP_BREAK", EVT_KEY_BREAK(KEY_UP) },
  { "EVT_LEFT_BREAK", EVT_KEY_BREAK(KEY_LEFT) },
  { "EVT_RIGHT_BREAK", EVT_KEY_BREAK(KEY_RIGHT) },
  { "EVT_SHIFT_BREAK", EVT_KEY_BREAK(KEY_SHIFT) },
  { "EVT_DOWN_LONG", EVT_KEY_LONG(KEY_DOWN) },
  { "EVT_UP_LONG", EVT_KEY_LONG(KEY_UP) },
  { "EVT_LEFT_LONG", EVT_KEY_LONG(KEY_LEFT) },
  { "EVT_RIGHT_LONG", EVT_KEY_LONG(KEY_RIGHT) },
  { "EVT_SHIFT_LONG", EVT_KEY_LONG(KEY_SHIFT) },
  { "EVT_DOWN_REPT", EVT_KEY_REPT(KEY_DOWN) },
  { "EVT_UP_REPT", EVT_KEY_REPT(KEY_UP) },
  { "EVT_LEFT_REPT", EVT_KEY_REPT(KEY_LEFT) },
  { "EVT_RIGHT_REPT", EVT_KEY_REPT(KEY_RIGHT) },
  { "FORCE", FORCE },
  { "ERASE", ERASE },
  { "ROUND", ROUND },
#elif defined(PCBTARANIS)
  { "EVT_MENU_BREAK", EVT_KEY_BREAK(KEY_MENU) },
  { "EVT_MENU_LONG", EVT_KEY_LONG(KEY_MENU) },
  { "EVT_PAGE_BREAK", EVT_KEY_BREAK(KEY_PAGE) },
  { "EVT_PAGE_LONG", EVT_KEY_LONG(KEY_PAGE) },
  { "EVT_PLUS_BREAK", EVT_KEY_BREAK(KEY_PLUS) },
  { "EVT_MINUS_BREAK", EVT_KEY_BREAK(KEY_MINUS) },
  { "EVT_PLUS_FIRST", EVT_KEY_FIRST(KEY_PLUS) },
  { "EVT_MINUS_FIRST", EVT_KEY_FIRST(KEY_MINUS) },
  { "EVT_PLUS_REPT", EVT_KEY_REPT(KEY_PLUS) },
  { "EVT_MINUS_REPT", EVT_KEY_REPT(KEY_MINUS) },
#if LCD_DEPTH > 1
  { "FILL_WHITE", FILL_WHITE },
  { "GREY_DEFAULT", GREY_DEFAULT },
#endif
  { "FORCE", FORCE },
  { "ERASE", ERASE },
  { "ROUND", ROUND },
#endif
  { "EVT_ENTER_BREAK", EVT_KEY_BREAK(KEY_ENTER) },
  { "EVT_ENTER_LONG", EVT_KEY_LONG(KEY_ENTER) },
  { "EVT_EXIT_BREAK", EVT_KEY_BREAK(KEY_EXIT) },
#if defined(ROTARY_ENCODER_NAVIGATION)
  { "EVT_ROT_BREAK", EVT_KEY_BREAK(KEY_ENTER) },
  { "EVT_ROT_LONG", EVT_KEY_LONG(KEY_ENTER) },
  { "EVT_ROT_LEFT", EVT_ROTARY_LEFT },
  { "EVT_ROT_RIGHT", EVT_ROTARY_RIGHT },
#endif
  { "SOLID", SOLID },
  { "DOTTED", DOTTED },
  { "LCD_W", LCD_W },
  { "LCD_H", LCD_H },
  { "PLAY_NOW", PLAY_NOW },
  { "PLAY_BACKGROUND", PLAY_BACKGROUND },
  { "TIMEHOUR", TIMEHOUR },

#if defined(PCBHORUS)
  // Adding the unit consts for the set Telemetry function adds about 1k of flash usage
  {"UNIT_RAW", UNIT_RAW },
  {"UNIT_VOLTS", UNIT_VOLTS },
  {"UNIT_AMPS", UNIT_AMPS },
  {"UNIT_MILLIAMPS", UNIT_MILLIAMPS },
  {"UNIT_KTS", UNIT_KTS },
  {"UNIT_METERS_PER_SECOND", UNIT_METERS_PER_SECOND },
  {"UNIT_FEET_PER_SECOND", UNIT_FEET_PER_SECOND },
  {"UNIT_KMH", UNIT_KMH },
  {"UNIT_MPH", UNIT_MPH },
  {"UNIT_METERS", UNIT_METERS },
  {"UNIT_FEET", UNIT_FEET },
  {"UNIT_CELSIUS", UNIT_CELSIUS },
  {"UNIT_FAHRENHEIT", UNIT_FAHRENHEIT },
  {"UNIT_PERCENT", UNIT_PERCENT },
  {"UNIT_MAH", UNIT_MAH },
  {"UNIT_WATTS", UNIT_WATTS },
  {"UNIT_MILLIWATTS", UNIT_MILLIWATTS },
  {"UNIT_DB", UNIT_DB },
  {"UNIT_RPMS", UNIT_RPMS },
  {"UNIT_G", UNIT_G },
  {"UNIT_DEGREE", UNIT_DEGREE },
  {"UNIT_RADIANS", UNIT_RADIANS },
  {"UNIT_MILLILITERS", UNIT_MILLILITERS },
  {"UNIT_FLOZ", UNIT_FLOZ },
  {"UNIT_HOURS", UNIT_HOURS },
  {"UNIT_MINUTES", UNIT_MINUTES },
  {"UNIT_SECONDS", UNIT_SECONDS },
  {"UNIT_CELLS", UNIT_CELLS},
  {"UNIT_DATETIME", UNIT_DATETIME},
  {"UNIT_GPS", UNIT_GPS},
  {"UNIT_BITFIELD", UNIT_BITFIELD},
  {"UNIT_TEXT", UNIT_TEXT},
#endif
  { NULL, 0 }  /* sentinel */
};
