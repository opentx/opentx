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
#include "lua/lua_api.h"
#include "telemetry/frsky.h"

#if defined(PCBHORUS)
  #include "lua/lua_exports_horus.inc"   // this line must be after lua headers
#elif defined(PCBFLAMENCO)
  #include "lua/lua_exports_flamenco.inc"
#elif defined(PCBX9E)
  #include "lua/lua_exports_taranis_x9e.inc"
#elif defined(PCBTARANIS)
  #include "lua/lua_exports_taranis.inc"
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
 * (string) radio version: `taranisx9e`, `taranisplus` or `taranis`.
If running in simulator the "-simu" is added
 * (number) major version (ie 2 if version 2.1.5)
 * (number) minor version (ie 1 if version 2.1.5)
 * (number) revison number (ie 5 if version 2.1.5)

@status current Introduced in 2.0.0, expanded in 2.1.7

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
  lua_createtable(L, 0, 6);
  lua_pushtableinteger(L, "year", year);
  lua_pushtableinteger(L, "mon", mon);
  lua_pushtableinteger(L, "day", day);
  lua_pushtableinteger(L, "hour", hour);
  lua_pushtableinteger(L, "min", min);
  lua_pushtableinteger(L, "sec", sec);
}

/*luadoc
@function getDateTime()

Return current system date and time that is kept by the RTC unit

@retval table current date and time, table elements:
 * `year` (number) year
 * `mon` (number) month
 * `day` (number) day of month
 * `hour` (number) hours
 * `min` (number) minutes
 * `sec` (number) seconds
*/
static int luaGetDateTime(lua_State * L)
{
  struct gtm utm;
  gettime(&utm);
  luaPushDateTime(L, utm.tm_year + TM_YEAR_BASE, utm.tm_mon + 1, utm.tm_mday, utm.tm_hour, utm.tm_min, utm.tm_sec);
  return 1;
}

static void luaPushLatLon(TelemetrySensor & telemetrySensor, TelemetryItem & telemetryItem)
/* result is lua table containing members ["lat"] and ["lon"] as lua_Number (doubles) in decimal degrees */
{
  lua_createtable(L, 0, 4);
  lua_pushtablenumber(L, "lat", telemetryItem.gps.latitude / 1000000.0);
  lua_pushtablenumber(L, "pilot-lat", telemetryItem.pilotLatitude / 1000000.0);
  lua_pushtablenumber(L, "lon", telemetryItem.gps.longitude / 1000000.0);
  lua_pushtablenumber(L, "pilot-lon", telemetryItem.pilotLongitude / 1000000.0);
}

static void luaPushTelemetryDateTime(TelemetrySensor & telemetrySensor, TelemetryItem & telemetryItem)
{
  luaPushDateTime(L, telemetryItem.datetime.year + 2000, telemetryItem.datetime.month, telemetryItem.datetime.day,
                  telemetryItem.datetime.hour, telemetryItem.datetime.min, telemetryItem.datetime.sec);
}

static void luaPushCells(TelemetrySensor & telemetrySensor, TelemetryItem & telemetryItem)
{
  if (telemetryItem.cells.count == 0)
    lua_pushinteger(L, (int)0); // returns zero if no cells
  else {
    lua_createtable(L, telemetryItem.cells.count, 0);
    for (int i = 0; i < telemetryItem.cells.count; i++) {
      lua_pushnumber(L, i + 1);
      lua_pushnumber(L, telemetryItem.cells.values[i].value / 100.0);
      lua_settable(L, -3);
    }
  }
}

void luaGetValueAndPush(int src)
{
  getvalue_t value = getValue(src); // ignored for GPS, DATETIME, and CELLS

  if (src >= MIXSRC_FIRST_TELEM && src <= MIXSRC_LAST_TELEM) {
    div_t qr = div(src-MIXSRC_FIRST_TELEM, 3);
    // telemetry values
    if (TELEMETRY_STREAMING() && telemetryItems[qr.quot].isAvailable()) {
      TelemetrySensor & telemetrySensor = g_model.telemetrySensors[qr.quot];
      switch (telemetrySensor.unit) {
        case UNIT_GPS:
          luaPushLatLon(telemetrySensor, telemetryItems[qr.quot]);
          break;
        case UNIT_DATETIME:
          luaPushTelemetryDateTime(telemetrySensor, telemetryItems[qr.quot]);
          break;
        case UNIT_CELLS:
          if (qr.rem == 0) {
            luaPushCells(telemetrySensor, telemetryItems[qr.quot]);
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
    lua_pushnumber(L, float(value)/10.0);
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

static int luaSportTelemetryPush(lua_State * L)
{
  if (isSportOutputBufferAvailable()) {
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

static int luaCrossfireTelemetryPop(lua_State * L)
{
  if (!luaInputTelemetryFifo) {
    luaInputTelemetryFifo = new Fifo<uint8_t, LUA_TELEMETRY_INPUT_FIFO_SIZE>();
    if (!luaInputTelemetryFifo) {
      return 0;
    }
  }

  uint8_t length, data;
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

static int luaCrossfireTelemetryPush(lua_State * L)
{
  if (isCrossfireOutputBufferAvailable()) {
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
* for OpenTX 2.0.x at http://downloads-20.open-tx.org/firmware/lua_fields.txt
* for OpenTX 2.1.x at http://downloads-21.open-tx.org/firmware/lua_fields.txt (depreciated)
* for OpenTX 2.1.x Taranis and Taranis Plus at http://downloads-21.open-tx.org/firmware/lua_fields_taranis.txt
* for OpenTX 2.1.x Taranis X9E at http://downloads-21.open-tx.org/firmware/lua_fields_taranis_x9e.txt

@param name (string) name of the field

@retval table information about requested field, table elements:
 * `id`   (number) field identifier
 * `name` (string) field name
 * `desc` (string) field description

@retval nil the requested field was not found

@status current Introduced in 2.0.8
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
    return 1;
  }
  return 0;
}

/*luadoc
@function getValue(source)

Returns the value of a source.

The list of valid sources is available:
* for OpenTX 2.0.x at http://downloads-20.open-tx.org/firmware/lua_fields.txt
* for OpenTX 2.1.x at http://downloads-21.open-tx.org/firmware/lua_fields.txt

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
  luaGetValueAndPush(src);
  return 1;
}

/*luadoc
@function getRAS()

Return the RAS value or nil if no valid hardware found

@retval number representing RAS value. Value bellow 0x33 (51 decimal) are all ok, value above 0x33 indicate a hardware antenna issue.
This is just a hardware pass/fail measure and does not represent the quality of the radio link

@status current Introduced in 2.2.0
*/
static int luaGetRAS(lua_State * L)
{
  if (IS_SWR_VALUE_VALID()) { 
    lua_pushinteger(L, telemetryData.swr.value);
  }
  else {
    lua_pushnil(L);  
  }
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

@param unit (number) unit identifier (see table todo)

@param attributes (unsigned number) possible values:
 * `0 or not present` plays integral part of the number (for a number 123 it plays 123)
 * `PREC1` plays a number with one decimal place (for a number 123 it plays 12.3)
 * `PREC2` plays a number with two decimal places (for a number 123 it plays 1.23)

@status current Introduced in 2.0.0

@notice 2.0 Only - automatic conversion of units for distance, speed, and temperature.

OpenTX 2.0:

| Unit  | Sound | File (.wav) | Automatic conversion rules  |
| --- | --- | --- | --- |
| 0 | --- | --- (no unit played) |   |
| 1 | Volts | 116 |   |
| 2 | Amps  | 118 |   |
| 3 | Meters per Second | 120 |   |
| 4 | *missing file*  | 122 |   |
| 5 | Kilometers per Hour / Miles per Hour  | 124 / 142 | Input value is KPH  |
| 6 | Meters / Feet | 126 / 140 | Input value is meters |
| 7 | Degrees | 128 | Input value is celsius, converted to Fahrenheit for Imperial  |
| 8 | Percent | 130 |   |
| 9 | Milliamps | 132 |   |
| 10  | Milliamp Hours  | 134 |   |
| 11  | Watts | 136 |   |
| 12  | DB  | 138 |   |
| 13  | Feet  | 140 |   |
| 14  | Kilometers per Hour / Miles per Hour  | 124 / 142 | Input value is in Knots, converted to KPH or MPH  |
| 15  | Hours | 144 |   |
| 16  | Minutes | 146 |   |
| 17  | Seconds | 148 |   |
| 18  | RPM | 150 |   |
| 19  | Gee | 152 |   |
| 20  | Degrees | 128 |   |


OpenTX 2.1:

| 2.1 Unit  | Sound | Sound File (.wav) |
| --- | --- | --- |
| 0 | --- | --- (no unit played) |   |
| 1 | Volts | 116 |
| 2 | Amps  | 118 |
| 3 | Milliamps | 120 |
| 4 | Knots | 122 |
| 5 | Meters per Second | 124 |
| 6 | Feet per Second | 126 |
| 7 | Kilometers per Hour | 128 |
| 8 | Miles per Hour  | 130 |
| 9 | Meters  | 132 |
| 10  | Feet  | 134 |
| 11  | Degrees Celsius | 136 |
| 12  | Degrees Fahrenheit  | 138 |
| 13  | Percent | 140 |
| 14  | Milliamp Hours  | 142 |
| 15  | Watts | 144 |
| 16  | DB  | 146 |
| 17  | RPM | 148 |
| 18  | Gee | 150 |
| 19  | Degrees | 152 |
| 20  | Milliliters | 154 |
| 21  | Fluid Ounces  | 156 |
| 22  | Hours | 158 |
| 23  | Minutes | 160 |
| 24  | Seconds | 162 |

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
@function luaPlayHaptic(duration, pause [, flags])

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
#endif
  return 0;
}

/*luadoc
@function killEvents(key)

Stops key state machine.

@param key (number) key to be killed, can also include event type (only key part is used)

@status current Introduced in 2.0.0

TODO table of events/masks
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

#if !defined(COLORLCD)
/*luadoc
@function GREY()

Returns gray value which can be used in LCD functions

@retval (number) a value that represents amount of *greyness* (from 0 to 15)

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
 * `battMin` (number) radio battery range - minimum value
 * `battMax` (number) radio battery range - maximum value
 * `imperial` (number) set to a value different from 0 if the radio is set to the
 IMPERIAL units
 * `language` (string) radio language (used for menus)
 * `voice` (string) voice language (used for speech)

@status current Introduced in 2.0.6, `imperial` added in TODO,
`language` and `voice` added int 2.2.0.

*/
static int luaGetGeneralSettings(lua_State * L)
{
  lua_newtable(L);
  lua_pushtablenumber(L, "battMin", double(90+g_eeGeneral.vBatMin)/10);
  lua_pushtablenumber(L, "battMax", double(120+g_eeGeneral.vBatMax)/10);
  lua_pushtableinteger(L, "imperial", g_eeGeneral.imperial);
  lua_pushtablestring(L, "language", TRANSLATIONS);
  lua_pushtablestring(L, "voice", currentLanguagePack->id);
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

const luaL_Reg opentxLib[] = {
  { "getTime", luaGetTime },
  { "getDateTime", luaGetDateTime },
  { "getVersion", luaGetVersion },
  { "getGeneralSettings", luaGetGeneralSettings },
  { "getValue", luaGetValue },
  { "getRAS", luaGetRAS },
  { "getFieldInfo", luaGetFieldInfo },
  { "getFlightMode", luaGetFlightMode },
  { "playFile", luaPlayFile },
  { "playNumber", luaPlayNumber },
  { "playDuration", luaPlayDuration },
  { "playTone", luaPlayTone },
  { "playHaptic", luaPlayHaptic },
  { "popupInput", luaPopupInput },
  { "defaultStick", luaDefaultStick },
  { "defaultChannel", luaDefaultChannel },
  { "killEvents", luaKillEvents },
#if !defined(COLORLCD)
  { "GREY", luaGrey },
#endif
  { "sportTelemetryPop", luaSportTelemetryPop },
  { "sportTelemetryPush", luaSportTelemetryPush },
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
  { "VALUE", 0 }, // TODO reuse ZoneOption::Integer
  { "SOURCE", 1 }, // TODO reuse ZoneOption::Source
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
  { "MIXSRC_SE", MIXSRC_SE },
  { "MIXSRC_SF", MIXSRC_SF },
  { "MIXSRC_SG", MIXSRC_SG },
  { "MIXSRC_SH", MIXSRC_SH },
  { "MIXSRC_CH1", MIXSRC_CH1 },
  { "SWSRC_LAST", SWSRC_LAST_LOGICAL_SWITCH },
#if defined(COLORLCD)
  { "COLOR", ZoneOption::Color },
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
#elif defined(PCBTARANIS)
  { "EVT_MENU_BREAK", EVT_KEY_BREAK(KEY_MENU) },
  { "EVT_PAGE_BREAK", EVT_KEY_BREAK(KEY_PAGE) },
  { "EVT_PAGE_LONG", EVT_KEY_LONG(KEY_PAGE) },
  { "EVT_PLUS_BREAK", EVT_KEY_BREAK(KEY_PLUS) },
  { "EVT_MINUS_BREAK", EVT_KEY_BREAK(KEY_MINUS) },
  { "EVT_PLUS_FIRST", EVT_KEY_FIRST(KEY_PLUS) },
  { "EVT_MINUS_FIRST", EVT_KEY_FIRST(KEY_MINUS) },
  { "EVT_PLUS_REPT", EVT_KEY_REPT(KEY_PLUS) },
  { "EVT_MINUS_REPT", EVT_KEY_REPT(KEY_MINUS) },
  { "FILL_WHITE", FILL_WHITE },
  { "GREY_DEFAULT", GREY_DEFAULT },
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
  { NULL, 0 }  /* sentinel */
};
