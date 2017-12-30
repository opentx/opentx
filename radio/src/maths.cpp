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

#include "opentx.h"

#if !defined(CPUARM)
// #define CORRECT_NEGATIVE_SHIFTS
// open.20.fsguruh; shift right operations do the rounding different for negative values compared to positive values
// so all negative divisions round always further down, which give absolute values bigger compared to a usual division
// this is noticable on the display, because instead of -100.0 -99.9 is shown; While in praxis I doublt somebody will notice a
// difference this is more a mental thing. Maybe people are distracted, because the easy calculations are obviously wrong
// this define would correct this, but costs 34 bytes code for stock version

// currently we set this to active always, because it might cause a fault about 1% compared positive and negative values
// is done now in makefile

int16_t calc100to256_16Bits(int16_t x) // return x*2.56
{
    // y = 2*x + x/2 +x/16-x/512-x/2048
    // 512 and 2048 are out of scope from int8 input --> forget it
#ifdef CORRECT_NEGATIVE_SHIFTS
    int16_t res=(int16_t)x<<1;
    //int8_t  sign=(uint8_t) x>>7;
    int8_t sign=(x<0?1:0);

    x-=sign;
    res+=(x>>1);
    res+=sign;
    res+=(x>>4);
    res+=sign;
    return res;
#else
    return ((int16_t)x<<1)+(x>>1)+(x>>4);
#endif
}

int16_t calc100to256(int8_t x) // return x*2.56
{
  return calc100to256_16Bits(x);
}

int16_t calc100toRESX_16Bits(int16_t x) // return x*10.24
{
#ifdef CORRECT_NEGATIVE_SHIFTS
  int16_t res= ((int16_t)x*41)>>2;
  int8_t sign=(x<0?1:0);
  //int8_t  sign=(uint8_t) x>>7;
  x-=sign;
  res-=(x>>6);
  res-=sign;
  return res;
#else
  // return (int16_t)x*10 + x/4 - x/64;
  return ((x*41)>>2) - (x>>6);
#endif
}

int16_t calc100toRESX(int8_t x) // return x*10.24
{
  return calc100toRESX_16Bits(x);
}

// return x*1.024
int16_t calc1000toRESX(int16_t x) // improve calc time by Pat MacKenzie
{
  // return x + x/32 - x/128 + x/512;
  int16_t y = x>>5;
  x+=y;
  y=y>>2;
  x-=y;
  return x+(y>>2);
}

int16_t calcRESXto1000(int16_t x)  // return x/1.024
{
// *1000/1024 = x - x/32 + x/128
  return (x - (x>>5) + (x>>7));
}

int8_t calcRESXto100(int16_t x)
{
  return (x*25) >> 8;
}
#endif
#if defined(HELI) || defined(FRSKY_HUB) || defined(CPUARM)
uint16_t isqrt32(uint32_t n)
{
  uint16_t c = 0x8000;
  uint16_t g = 0x8000;

  for (;;) {
    if ((uint32_t)g*g > n)
      g ^= c;
    c >>= 1;
    if(c == 0)
      return g;
    g |= c;
  }
}
#endif

/*
  Division by 10 and rounding or fixed point arithmetic values

  Examples: 
    value -> result
    105 ->  11
    104 ->  10
   -205 -> -21
   -204 -> -20 
*/

#if defined(FRSKY_HUB) && !defined(CPUARM)
// convert latitude and longitude to 1/10^6 degrees
void extractLatitudeLongitude(uint32_t * latitude, uint32_t * longitude)
{
  div_t qr = div(telemetryData.hub.gpsLatitude_bp, 100);
  *latitude = ((uint32_t)(qr.quot) * 1000000) + (((uint32_t)(qr.rem) * 10000 + telemetryData.hub.gpsLatitude_ap) * 5) / 3;

  qr = div(telemetryData.hub.gpsLongitude_bp, 100);
  *longitude = ((uint32_t)(qr.quot) * 1000000) + (((uint32_t)(qr.rem) * 10000 + telemetryData.hub.gpsLongitude_ap) * 5) / 3;
}

#if __clang__
// clang does not like packed member access at all. Since mavlink is a 3rd party library, ignore the errors
#pragma clang diagnostic push
#pragma clang diagnostic warning "-Waddress-of-packed-member"
#endif
void getGpsPilotPosition()
{
  extractLatitudeLongitude(&telemetryData.hub.pilotLatitude, &telemetryData.hub.pilotLongitude);
  // distFromEarthAxis = cos(lat) * EARTH_RADIUS
  // 1 - x2/2 + x4/24
  uint32_t lat = telemetryData.hub.pilotLatitude / 10000;
  uint32_t angle2 = (lat*lat) / 10000;
  uint32_t angle4 = angle2 * angle2;
  telemetryData.hub.distFromEarthAxis = 139 * (((uint32_t)10000000-((angle2*(uint32_t)123370)/81)+(angle4/25)) / 12500);
  // TRACE("telemetryData.hub.distFromEarthAxis=%d", telemetryData.hub.distFromEarthAxis);
}

void getGpsDistance()
{
  uint32_t lat, lng;

  extractLatitudeLongitude(&lat, &lng);

  // printf("lat=%d (%d), long=%d (%d)\n", lat, abs(lat - telemetryData.hub.pilotLatitude), lng, abs(lng - telemetryData.hub.pilotLongitude));

  uint32_t angle = (lat > telemetryData.hub.pilotLatitude) ? lat - telemetryData.hub.pilotLatitude : telemetryData.hub.pilotLatitude - lat;
  uint32_t dist = EARTH_RADIUS * angle / 1000000;
  uint32_t result = dist*dist;

  angle = (lng > telemetryData.hub.pilotLongitude) ? lng - telemetryData.hub.pilotLongitude : telemetryData.hub.pilotLongitude - lng;
  dist = telemetryData.hub.distFromEarthAxis * angle / 1000000;
  result += dist*dist;

  dist = abs(TELEMETRY_BARO_ALT_AVAILABLE() ? TELEMETRY_RELATIVE_BARO_ALT_BP : TELEMETRY_RELATIVE_GPS_ALT_BP);
  result += dist*dist;

  telemetryData.hub.gpsDistance = isqrt32(result);
  if (telemetryData.hub.gpsDistance > telemetryData.hub.maxGpsDistance)
    telemetryData.hub.maxGpsDistance = telemetryData.hub.gpsDistance;
}
#endif

#if defined(CPUARM)
// djb2 hash algorithm
uint32_t hash(const void * ptr, uint32_t size)
{
  const uint8_t * data = (const uint8_t *)ptr;
  uint32_t hash = 5381;
  for (uint32_t i=0; i<size; i++) {
    hash = ((hash << 5) + hash) + data[i]; /* hash * 33 + c */
  }
  return hash;
}
#endif
