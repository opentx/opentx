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

#include "board_taranis.h"
#include "../../pwr.h"

extern volatile uint32_t g_tmr10ms;
#define get_tmr10ms() g_tmr10ms

void pwrInit()
{
  // if any changes are done to the PWR PIN or pwrOn() function
  // then the same changes must be done in _bootStart()

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = PWR_GPIO_PIN_ON;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(PWR_GPIO, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = PWR_GPIO_PIN_SWITCH;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(PWR_GPIO, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = PWR_GPIO_PIN_LED;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(PWR_GPIO_LED, &GPIO_InitStructure);

  GPIO_ResetBits(INTMODULE_GPIO_PWR, INTMODULE_GPIO_PIN_PWR);
  GPIO_InitStructure.GPIO_Pin = INTMODULE_GPIO_PIN_PWR;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(INTMODULE_GPIO_PWR, &GPIO_InitStructure);
  
  GPIO_ResetBits(EXTMODULE_GPIO_PWR, EXTMODULE_GPIO_PIN_PWR);
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_GPIO_PIN_PWR;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(EXTMODULE_GPIO_PWR, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = TRAINER_GPIO_PIN_DETECT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_Init(TRAINER_GPIO_DETECT, &GPIO_InitStructure);

  pwrOn();
}

void pwrOn()
{
  GPIO_SetBits(PWR_GPIO, PWR_GPIO_PIN_ON);
}

void pwrOff()
{
  GPIO_ResetBits(PWR_GPIO, PWR_GPIO_PIN_ON);

#if defined(REV9E)
  // 9E needs watchdog reset because CPU is still running while the power
  // key is held pressed by the user
  while (1) {
    wdt_reset();
#if 0
    // It doesn't work correctly, if we press long on the pwr button, the radio restarts when the button is released
    PWR->CR |= PWR_CR_CWUF;
    /* Select STANDBY mode */
    PWR->CR |= PWR_CR_PDDS;
    /* Set SLEEPDEEP bit of Cortex System Control Register */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    /* Request Wait For Event */
    __WFE();
#endif
  }
#endif
}

#if defined(REV9E)
uint32_t pwrPressed()
{
  return GPIO_ReadInputDataBit(PWR_GPIO, PWR_GPIO_PIN_SWITCH) == Bit_RESET;
}
#endif

#if !defined(REV9E)
uint32_t pwrCheck()
{
#if defined(SIMU)
  return e_power_on;
#else
  if (GPIO_ReadInputDataBit(PWR_GPIO, PWR_GPIO_PIN_SWITCH) == Bit_RESET)
    return e_power_on;
  else if (usbPlugged())
    return e_power_usb;
  else
    return e_power_off;
#endif
}
#endif
