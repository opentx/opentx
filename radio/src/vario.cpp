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


#if defined(CPUARM)

void varioWakeup()
{
  if (isFunctionActive(FUNCTION_VARIO)) {
    int varioFreq, varioDuration, varioPause=0;
    uint8_t varioFlags;

    int verticalSpeed = 0;
    if (g_model.frsky.varioSource) {
      uint8_t item = g_model.frsky.varioSource-1;
      if (item < MAX_TELEMETRY_SENSORS) {
        verticalSpeed = telemetryItems[item].value * g_model.telemetrySensors[item].getPrecMultiplier();
      }
    }

    int varioCenterMin = (int)g_model.frsky.varioCenterMin * 10 - 50;
    int varioCenterMax = (int)g_model.frsky.varioCenterMax * 10 + 50;
    int varioMax = (10+(int)g_model.frsky.varioMax) * 100;
    int varioMin = (-10+(int)g_model.frsky.varioMin) * 100;

    if (verticalSpeed > varioMax)
      verticalSpeed = varioMax;
    else if (verticalSpeed < varioMin)
      verticalSpeed = varioMin;

    if (verticalSpeed <= varioCenterMin) {
      varioFreq = VARIO_FREQUENCY_ZERO + (g_eeGeneral.varioPitch*10) - (((VARIO_FREQUENCY_ZERO+(g_eeGeneral.varioPitch*10)-((VARIO_FREQUENCY_ZERO + (g_eeGeneral.varioPitch*10))/2)) * (verticalSpeed-varioCenterMin)) / varioMin);
      varioDuration = 80; // continuous beep: we will enter again here before the tone ends
      varioFlags = PLAY_BACKGROUND|PLAY_NOW;
    }
    else if (verticalSpeed >= varioCenterMax || !g_model.frsky.varioCenterSilent) {
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
      return;
    }

    AUDIO_VARIO(varioFreq, varioDuration, varioPause, varioFlags);
  }
}

#elif defined(TELEMETRY_FRSKY)

void varioWakeup()
{
  static tmr10ms_t s_varioTmr;
  tmr10ms_t tmr10ms = get_tmr10ms();

  if (isFunctionActive(FUNCTION_VARIO)) {
#if defined(AUDIO)
    cli();
    int16_t verticalSpeed = telemetryData.hub.varioSpeed;
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

#elif defined(BUZZER) // && !defined(AUDIO)

    int8_t verticalSpeed = limit((int16_t)-100, (int16_t)(telemetryData.hub.varioSpeed/10), (int16_t)+100);

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
