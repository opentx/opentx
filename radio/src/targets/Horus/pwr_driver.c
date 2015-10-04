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

#include "../../pwr.h"
#include "board_horus.h"

extern volatile uint32_t g_tmr10ms;
#define get_tmr10ms() g_tmr10ms

void pwrInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Clock enable */
  RCC_AHB1PeriphClockCmd(PWR_RCC_AHB1Periph_GPIO, ENABLE);

  /* GPIO  Configuration*/
  GPIO_InitStructure.GPIO_Pin = PWR_GPIO_PIN_ON;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(PWR_GPIO, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = PWR_GPIO_PIN_SWITCH;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(PWR_GPIO, &GPIO_InitStructure);

  // Soft power ON
  GPIO_SetBits(PWR_GPIO, PWR_GPIO_PIN_ON);

  // Init Module PWR
  GPIO_ResetBits(EXTMODULE_GPIO_PWR, EXTMODULE_GPIO_PIN_PWR);
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_GPIO_PIN_PWR;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_GPIO_PWR, &GPIO_InitStructure);

  // TODO while (pwrPressed()) ;

}

void pwrOff()
{
  GPIO_ResetBits(PWR_GPIO, PWR_GPIO_PIN_ON);
}

uint32_t pwrPressed()
{
  return GPIO_ReadInputDataBit(PWR_GPIO, PWR_GPIO_PIN_SWITCH) == Bit_RESET;
}

#if !defined(BOOT)
uint32_t pwrPressTime = 0;
uint32_t pwrPressedDuration()
{
  if (pwrPressTime == 0) {
    return 0;
  }
  else {
    return get_tmr10ms() - pwrPressTime;
  }
}
uint32_t pwrCheck()
{
  if (pwrPressed()) {
    if (pwrPressTime == 0) {
      pwrPressTime = get_tmr10ms();
    }
    else {
      if (get_tmr10ms() - pwrPressTime > 250) {
        return e_power_off;
      }
    }
  }
  else {
    pwrPressTime = 0;
  }
  return e_power_on;
}
#endif
