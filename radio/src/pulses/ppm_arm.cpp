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

#include "../opentx.h"

void setupPulsesPPM(unsigned int port)                   // Don't enable interrupts through here
{
  int16_t PPM_range = g_model.extendedLimits ? (512*LIMIT_EXT_PERCENT/100) * 2 : 512 * 2; //range of 0.7..1.7msec

  // Total frame length = 22.5msec
  // each pulse is 0.7..1.7ms long with a 0.3ms stop tail
  // The pulse ISR is 2mhz that's why everything is multiplied by 2

  uint32_t firstCh = g_model.moduleData[port].channelsStart;
  uint32_t lastCh = min<unsigned int>(NUM_CHNOUT, firstCh + 8 + g_model.moduleData[port].channelsCount);

#if defined(PCBSKY9X)
  // TODO move register stuff to driver
  register Pwm *pwmptr = PWM;
  uint32_t pwmCh = (port == EXTERNAL_MODULE ? 3 : 1);
  pwmptr->PWM_CH_NUM[pwmCh].PWM_CDTYUPD = (g_model.moduleData[port].ppmDelay * 50 + 300) * 2; //Stoplen *2
  if (g_model.moduleData[port].ppmPulsePol)
    pwmptr->PWM_CH_NUM[pwmCh].PWM_CMR &= ~0x00000200 ;  // CPOL
  else
    pwmptr->PWM_CH_NUM[pwmCh].PWM_CMR |= 0x00000200 ;   // CPOL
#endif

  PpmPulsesData * ppmPulsesData = (port == TRAINER_MODULE ? &trainerPulsesData.ppm : &modulePulsesData[port].ppm);
  uint16_t * ptr = ppmPulsesData->pulses;

#if defined(PPM_PIN_HW_SERIAL)
  ppmPulsesData->index = 0;
#else
  ppmPulsesData->ptr = ptr;
#endif

  int32_t rest = 22500u * 2;
  rest += (int32_t(g_model.moduleData[port].ppmFrameLength)) * 1000;
  for (uint32_t i=firstCh; i<lastCh; i++) {
    int16_t v = limit((int16_t)-PPM_range, channelOutputs[i], (int16_t)PPM_range) + 2*PPM_CH_CENTER(i);
    rest -= v;
    *ptr++ = v; /* as Pat MacKenzie suggests */
  }
  if (rest > 65535) rest = 65535; /* prevents overflows */
  if (rest < 9000)  rest = 9000;  /* avoids that CCR2 is bigger than ARR which would cause reboot */
  *ptr = rest;
  *(ptr + 1) = 0;

#if !defined(PCBSKY9X)
  rest -= 1000;
  uint32_t ppmDelay = (g_model.moduleData[port].ppmDelay * 50 + 300) * 2;
  // set idle time, ppm delay and ppm polarity
  if (port == TRAINER_MODULE) {
    set_trainer_ppm_parameters(rest, ppmDelay, !g_model.moduleData[TRAINER_MODULE].ppmPulsePol); // ppmPulsePol: 0 - positive, 1 - negative
  }
  else if (port == EXTERNAL_MODULE) {
    set_external_ppm_parameters(rest, ppmDelay, !g_model.moduleData[EXTERNAL_MODULE].ppmPulsePol);
  }
#endif

#if defined(TARANIS_INTERNAL_PPM)
  else if (port == INTERNAL_MODULE) {
    set_internal_ppm_parameters(rest, ppmDelay, !g_model.moduleData[INTERNAL_MODULE].ppmPulsePol);
  }
#endif // #if defined(TARANIS_INTERNAL_PPM)
}
