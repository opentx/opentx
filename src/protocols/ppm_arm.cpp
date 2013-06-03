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

#define PPM_STREAM_INIT  { 2000, 2200, 2400, 2600, 2800, 3000, 3200, 3400, 9000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
#if defined(PCBTARANIS)
  uint16_t ppmStream[NUM_MODULES+1][20]  = { PPM_STREAM_INIT, PPM_STREAM_INIT, PPM_STREAM_INIT };
#else
  uint16_t ppmStream[NUM_MODULES][20]  = { MODULES_INIT(PPM_STREAM_INIT) };
#endif

void setupPulsesPPM(unsigned int port)                   // Don't enable interrupts through here
{
  int16_t PPM_range = g_model.extendedLimits ? 640 * 2 : 512 * 2; //range of 0.7..1.7msec

  // Total frame length = 22.5msec
  // each pulse is 0.7..1.7ms long with a 0.3ms stop tail
  // The pulse ISR is 2mhz that's why everything is multiplied by 2

  uint32_t firstCh = g_model.moduleData[port].channelsStart;
  uint32_t lastCh = min<unsigned int>(NUM_CHNOUT, firstCh + 8 + g_model.moduleData[port].channelsCount);

#if defined(PCBSKY9X)
  register Pwm *pwmptr = PWM;
  uint32_t pwmCh = (port == 0 ? 3 : 1);
  pwmptr->PWM_CH_NUM[pwmCh].PWM_CDTYUPD = (g_model.moduleData[port].ppmDelay * 50 + 300) * 2; //Stoplen *2
  if (g_model.moduleData[port].ppmPulsePol)
    pwmptr->PWM_CH_NUM[pwmCh].PWM_CMR |= 0x00000200 ;   // CPOL
  else
    pwmptr->PWM_CH_NUM[pwmCh].PWM_CMR &= ~0x00000200 ;  // CPOL
#endif

  uint16_t * ptr = ppmStream[port];
  uint32_t rest = 22500u * 2; //Minimum Framelen=22.5 ms
  rest += (int32_t(g_model.moduleData[port].ppmFrameLength)) * 1000;
  for (uint32_t i=firstCh; i<lastCh; i++) {
    int16_t v = limit((int16_t)-PPM_range, channelOutputs[i], (int16_t)PPM_range) + 2*PPM_CH_CENTER(i);
    rest -= v;
    *ptr++ = v; /* as Pat MacKenzie suggests */
  }
  rest = (rest > 65535) ? 65535 : rest;
  *ptr = rest;
  *(ptr + 1) = 0;

#if defined(PCBTARANIS)
  if (port == TRAINER_MODULE) {
    TIM3->CCR2 = rest - 1000 ;             // Update time
    TIM3->CCR4 = (g_model.moduleData[port].ppmDelay*50+300)*2;
    if(!g_model.moduleData[TRAINER_MODULE].ppmPulsePol)
      TIM3->CCER |= TIM_CCER_CC4P;
    else
      TIM3->CCER &= ~TIM_CCER_CC4P;
  }
  else if (port == EXTERNAL_MODULE) {
    TIM8->CCR2 = rest - 1000;             // Update time
    TIM8->CCR1 = (g_model.moduleData[port].ppmDelay*50+300)*2;
    if(!g_model.moduleData[EXTERNAL_MODULE].ppmPulsePol)
      TIM8->CCER |= TIM_CCER_CC1NP;
    else
      TIM8->CCER &= ~TIM_CCER_CC1NP;
  }
  else {
    TIM1->CCR2 = rest - 1000;             // Update time
    TIM1->CCR3 = (g_model.moduleData[port].ppmDelay*50+300)*2;
  }
#endif
}
