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

#if defined(FRSKY) || defined(CPUARM)
uint16_t getChannelRatio(uint8_t channel)
{
  return (uint16_t)g_model.frsky.channels[channel].ratio << g_model.frsky.channels[channel].multiplier;
}

lcdint_t applyChannelRatio(uint8_t channel, lcdint_t val)
{
  if (TELEMETRY_STREAMING())
    return ((int32_t)val+g_model.frsky.channels[channel].offset) * getChannelRatio(channel) * 2 / 51;
  else
    return 0;
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

void getGpsPilotPosition()
{
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

  dist = abs(frskyData.hub.baroAltitudeOffset ? TELEMETRY_ALT_BP : TELEMETRY_GPS_ALT_BP);
  result += dist*dist;

  frskyData.hub.gpsDistance = isqrt32(result);
  if (frskyData.hub.gpsDistance > frskyData.hub.maxGpsDistance)
    frskyData.hub.maxGpsDistance = frskyData.hub.gpsDistance;
}
#endif

#if defined(FRSKY) && defined(VARIO)
void varioWakeup()
{
  static tmr10ms_t s_varioTmr;
  tmr10ms_t tmr10ms = get_tmr10ms();
  
  if (isFunctionActive(FUNC_VARIO)) {
#if defined(AUDIO)
    // conversion in cm/s
    cli();
    int16_t verticalSpeed = frskyData.hub.varioSpeed;
    sei();
    
    int16_t varioCenterMax = (int16_t)g_model.frsky.varioCenterMax * 10 + 50;
    if (verticalSpeed >= varioCenterMax) {
      verticalSpeed = verticalSpeed - varioCenterMax;
      int16_t varioMax = (10+(int16_t)g_model.frsky.varioMax) * 100;
      if (verticalSpeed > varioMax) verticalSpeed = varioMax;
      verticalSpeed = (verticalSpeed * 10) / ((varioMax-varioCenterMax) / 100);
    }
    else {
      int16_t varioCenterMin = (int16_t)g_model.frsky.varioCenterMin * 10 - 50;
      if (verticalSpeed <= varioCenterMin) {
        verticalSpeed = verticalSpeed - varioCenterMin;
        int16_t varioMin = (-10+(int16_t)g_model.frsky.varioMin) * 100;
        if (verticalSpeed < varioMin) verticalSpeed = varioMin;
        verticalSpeed = (verticalSpeed * 10) / ((varioCenterMin-varioMin) / 100);
      }
      else {
        return;
      }
    }

    if (verticalSpeed < 0 || (int16_t)(s_varioTmr-tmr10ms) < 0) {
#if defined(CPUARM)
     uint8_t SoundVarioBeepTime;
     uint8_t SoundVarioBeepFreq;
      if (verticalSpeed > 0) {
        SoundVarioBeepTime = (8000 - verticalSpeed * 5) / 100;
        SoundVarioBeepFreq = (verticalSpeed * 4 + 8000) >> 7;
      }
      else {
        SoundVarioBeepTime = 20;
        SoundVarioBeepFreq = (verticalSpeed * 3 + 8000) >> 7;
      }
      s_varioTmr = tmr10ms + (SoundVarioBeepTime/2);
#else
      uint8_t SoundVarioBeepTime = (1600 - verticalSpeed) / 100;
      uint8_t SoundVarioBeepFreq = (verticalSpeed * 10 + 16000) >> 8;
      s_varioTmr = tmr10ms + (SoundVarioBeepTime*2);
#endif
      AUDIO_VARIO(SoundVarioBeepFreq, SoundVarioBeepTime);
    }

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
