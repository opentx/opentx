/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
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

#if defined(HELI) || defined(FRSKY_HUB)
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
getvalue_t div10_and_round(getvalue_t value)
{
  if (value >= 0 ) {
    value += 5;
  }
  else {
    value -= 5;
  }
  return value/10;
}


#if defined(FRSKY)
uint16_t getChannelRatio(uint8_t channel)
{
  return (uint16_t)g_model.frsky.channels[channel].ratio << g_model.frsky.channels[channel].multiplier;
}

lcdint_t applyChannelRatio(uint8_t channel, lcdint_t val)
{
#if defined(CPUARM)
  // A3 and A4 don't have ratio settings
  if (channel > 1)
    return val;
#endif
  return ((int32_t)val+g_model.frsky.channels[channel].offset) * getChannelRatio(channel) * 2 / 51;
}
#endif

#if defined(PCBTARANIS)
double gpsToDouble(bool neg, int16_t bp, int16_t ap)
{
  double result = ap;
  result /= 10000;
  result += (bp % 100);
  result /= 60;
  result += (bp / 100);
  return neg?-result:result;
}
#endif

#if defined(FRSKY_HUB)
void extractLatitudeLongitude(uint32_t * latitude, uint32_t * longitude)
{
  div_t qr = div(frskyData.hub.gpsLatitude_bp, 100);
  *latitude = ((uint32_t)(qr.quot) * 1000000) + (((uint32_t)(qr.rem) * 10000 + frskyData.hub.gpsLatitude_ap) * 5) / 3;

  qr = div(frskyData.hub.gpsLongitude_bp, 100);
  *longitude = ((uint32_t)(qr.quot) * 1000000) + (((uint32_t)(qr.rem) * 10000 + frskyData.hub.gpsLongitude_ap) * 5) / 3;
}

#if defined(PCBTARANIS)
double pilotLatitude;
double pilotLongitude;
#endif

void getGpsPilotPosition()
{
#if defined(PCBTARANIS)
  pilotLatitude = gpsToDouble(frskyData.hub.gpsLatitudeNS=='S', frskyData.hub.gpsLatitude_bp, frskyData.hub.gpsLatitude_ap);
  pilotLongitude = gpsToDouble(frskyData.hub.gpsLongitudeEW=='W', frskyData.hub.gpsLongitude_bp, frskyData.hub.gpsLongitude_ap);
#endif
  extractLatitudeLongitude(&frskyData.hub.pilotLatitude, &frskyData.hub.pilotLongitude);
  uint32_t lat = frskyData.hub.pilotLatitude / 10000;
  uint32_t angle2 = (lat*lat) / 10000;
  uint32_t angle4 = angle2 * angle2;
  frskyData.hub.distFromEarthAxis = 139*(((uint32_t)10000000-((angle2*(uint32_t)123370)/81)+(angle4/25))/12500);
  // printf("frskyData.hub.distFromEarthAxis=%d\n", frskyData.hub.distFromEarthAxis); fflush(stdout);
}

void getGpsDistance()
{
  uint32_t lat, lng;

  extractLatitudeLongitude(&lat, &lng);

  // printf("lat=%d (%d), long=%d (%d)\n", lat, abs(lat - frskyData.hub.pilotLatitude), lng, abs(lng - frskyData.hub.pilotLongitude));

  uint32_t angle = (lat > frskyData.hub.pilotLatitude) ? lat - frskyData.hub.pilotLatitude : frskyData.hub.pilotLatitude - lat;
  uint32_t dist = EARTH_RADIUS * angle / 1000000;
  uint32_t result = dist*dist;

  angle = (lng > frskyData.hub.pilotLongitude) ? lng - frskyData.hub.pilotLongitude : frskyData.hub.pilotLongitude - lng;
  dist = frskyData.hub.distFromEarthAxis * angle / 1000000;
  result += dist*dist;

  dist = abs(TELEMETRY_BARO_ALT_AVAILABLE() ? TELEMETRY_RELATIVE_BARO_ALT_BP : TELEMETRY_RELATIVE_GPS_ALT_BP);
  result += dist*dist;

  frskyData.hub.gpsDistance = isqrt32(result);
  if (frskyData.hub.gpsDistance > frskyData.hub.maxGpsDistance)
    frskyData.hub.maxGpsDistance = frskyData.hub.gpsDistance;
}
#endif

#if defined(CPUARM) && defined(VARIO)

void varioWakeup()
{
  if (isFunctionActive(FUNCTION_VARIO)) {
    int varioFreq, varioDuration, varioPause=0;
    uint8_t varioFlags;

    int verticalSpeed = frskyData.hub.varioSpeed;

#if 0
    if (g_model.frsky.varioSource == VARIO_SOURCE_DTE) {
      #warning "Ele stick for vario tests"
      verticalSpeed = getValue(MIXSRC_Ele);
    }
#endif

    int varioCenterMin = (int)g_model.frsky.varioCenterMin * 10 - 50;
    int varioCenterMax = (int)g_model.frsky.varioCenterMax * 10 + 50;
    int varioMax = (10+(int)g_model.frsky.varioMax) * 100;
    int varioMin = (-10+(int)g_model.frsky.varioMin) * 100;

    if (verticalSpeed > varioMax)
      verticalSpeed = varioMax;
    else if (verticalSpeed < varioMin)
      verticalSpeed = varioMin;

    if (verticalSpeed > varioCenterMin) {
      varioFreq = VARIO_FREQUENCY_ZERO + (g_eeGeneral.varioPitch*10) + (((VARIO_FREQUENCY_RANGE+(g_eeGeneral.varioRange*10)) * (verticalSpeed-varioCenterMin)) / varioMax);
      int varioPeriod = VARIO_REPEAT_MAX + ((VARIO_REPEAT_ZERO+(g_eeGeneral.varioRepeat*10)-VARIO_REPEAT_MAX) * (varioMax-verticalSpeed) * (varioMax-verticalSpeed)) / ((varioMax-varioCenterMin) * (varioMax-varioCenterMin));
      if (verticalSpeed >= varioCenterMax || varioCenterMin == varioCenterMax)
        varioDuration = varioPeriod / 5;
      else
        varioDuration = varioPeriod * (85 - (((verticalSpeed-varioCenterMin) * 25) / (varioCenterMax-varioCenterMin))) / 100;
      varioPause = varioPeriod - varioDuration;
      varioFlags = PLAY_BACKGROUND;
    }
    else {
      varioFreq = VARIO_FREQUENCY_ZERO + (g_eeGeneral.varioPitch*10) - (((VARIO_FREQUENCY_ZERO+(g_eeGeneral.varioPitch*10)-((VARIO_FREQUENCY_ZERO + (g_eeGeneral.varioPitch*10))/2)) * (verticalSpeed-varioCenterMin)) / varioMin);
      varioDuration = 80; // continuous beep: we will enter again here before the tone ends
      varioFlags = PLAY_BACKGROUND|PLAY_NOW;
    }

    AUDIO_VARIO(varioFreq, varioDuration, varioPause, varioFlags);
  }
}

#elif defined(FRSKY) && defined(VARIO)

void varioWakeup()
{
  static tmr10ms_t s_varioTmr;
  tmr10ms_t tmr10ms = get_tmr10ms();
  
  if (isFunctionActive(FUNCTION_VARIO)) {
#if defined(AUDIO)
    cli();
    int16_t verticalSpeed = frskyData.hub.varioSpeed;
    sei();
    
#if defined(PCBSTD)
    int16_t varioCenterMax = (int16_t)g_model.frsky.varioCenterMax * 10 + 50;
    if (verticalSpeed >= varioCenterMax) {
      verticalSpeed = verticalSpeed - varioCenterMax;
      int16_t varioMax = (10+(int16_t)g_model.frsky.varioMax) * 100;
      if (verticalSpeed > varioMax) verticalSpeed = varioMax;
      verticalSpeed = (verticalSpeed * 10) / ((varioMax-varioCenterMax) / 100);

      if ((int16_t)(s_varioTmr-tmr10ms) < 0) {
        uint8_t varioFreq = (verticalSpeed * 10 + 16000) >> 8;
        uint8_t varioDuration = (1600 - verticalSpeed) / 100;
        s_varioTmr = tmr10ms + (varioDuration*2);
        AUDIO_VARIO(varioFreq, varioDuration);
      }
    }
#else
    int varioCenterMin = (int)g_model.frsky.varioCenterMin * 10 - 50;
    int varioCenterMax = (int)g_model.frsky.varioCenterMax * 10 + 50;
    int varioMax = (10+(int)g_model.frsky.varioMax) * 100;
    int varioMin = (-10+(int)g_model.frsky.varioMin) * 100;

    if (verticalSpeed < varioCenterMin || (verticalSpeed > varioCenterMax && (int16_t)(s_varioTmr - tmr10ms) < 0)) {
      if (verticalSpeed > varioMax)
        verticalSpeed = varioMax;
      else if (verticalSpeed < varioMin)
        verticalSpeed = varioMin;

      uint8_t varioFreq, varioDuration;
      if (verticalSpeed > 0) {
        varioFreq = (verticalSpeed * 4 + 8000) >> 7;
        varioDuration = (8000 - verticalSpeed * 5) / 100;
      }
      else {
        varioFreq = (verticalSpeed * 3 + 8000) >> 7;
        varioDuration = 20;
      }
      s_varioTmr = tmr10ms + (varioDuration/2);
      AUDIO_VARIO(varioFreq, varioDuration);
    }
#endif

#else // defined(AUDIO)

    int8_t verticalSpeed = limit((int16_t)-100, (int16_t)(frskyData.hub.varioSpeed/10), (int16_t)+100);

    uint16_t interval;
    if (verticalSpeed == 0) {
      interval = 300;
    }
    else {
      if (verticalSpeed < 0) {
        verticalSpeed = -verticalSpeed;
        warble = 1;
      }
      interval = (uint8_t)200 / verticalSpeed;
    }
    if (g_tmr10ms - s_varioTmr > interval) {
      s_varioTmr = g_tmr10ms;
      if (warble)
        AUDIO_VARIO_DOWN();
      else
        AUDIO_VARIO_UP();
    }
#endif
  }
  else {
    s_varioTmr = tmr10ms;
  }
}

#endif
