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

#include "../../opentx.h"

void hapticOff(void)
{
#if defined(REVPLUS)
  GPIO_ResetBits(GPIO_HAPTIC, PIN_HAPTIC); // Temp, TODO PWM support
  return;
#else
  GPIO_ResetBits(GPIO_HAPTIC, PIN_HAPTIC);
#endif
}

#if defined(REVPLUS)
void hapticOn(uint32_t pwmPercent)
{  // pwmPercent 0-100
  GPIO_SetBits(GPIO_HAPTIC, PIN_HAPTIC); // Temp, TODO PWM support
  return;
}
#else
void hapticOn()
{
  // No PWM before REVPLUS
  GPIO_SetBits(GPIO_HAPTIC, PIN_HAPTIC);
}
#endif

void hapticInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOHAPTIC, ENABLE);

  /* GPIO  Configuration*/
  GPIO_InitStructure.GPIO_Pin = PIN_HAPTIC;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_HAPTIC, &GPIO_InitStructure);
}
