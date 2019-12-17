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

#include "board.h"
#include "storage/rtc_backup.h"

void pwrInit()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  // Internal module PWR
  GPIO_ResetBits(INTMODULE_PWR_GPIO, INTMODULE_PWR_GPIO_PIN);
  GPIO_InitStructure.GPIO_Pin = INTMODULE_PWR_GPIO_PIN;
  GPIO_Init(INTMODULE_PWR_GPIO, &GPIO_InitStructure);

  // External module PWR
  GPIO_ResetBits(EXTMODULE_PWR_GPIO, EXTMODULE_PWR_GPIO_PIN);
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_PWR_GPIO_PIN;
  GPIO_Init(EXTMODULE_PWR_GPIO, &GPIO_InitStructure);

#if defined(INTMODULE_BOOTCMD_GPIO)
  GPIO_ResetBits(INTMODULE_BOOTCMD_GPIO, INTMODULE_BOOTCMD_GPIO_PIN);
  GPIO_InitStructure.GPIO_Pin = INTMODULE_BOOTCMD_GPIO_PIN;
  GPIO_Init(INTMODULE_BOOTCMD_GPIO, &GPIO_InitStructure);
#endif

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;

  // PWR SWITCH
  GPIO_InitStructure.GPIO_Pin = PWR_SWITCH_GPIO_PIN;
  GPIO_Init(PWR_SWITCH_GPIO, &GPIO_InitStructure);

#if defined(PCBREV_GPIO_PIN)
  // PCBREV
  GPIO_InitStructure.GPIO_Pin = PCBREV_GPIO_PIN;
  GPIO_Init(PCBREV_GPIO, &GPIO_InitStructure);
  hardwareOptions.pcbrev = PCBREV_VALUE();
#endif

  // SD-DETECT PIN
  GPIO_ResetBits(SD_PRESENT_GPIO, SD_PRESENT_GPIO_PIN);
  GPIO_InitStructure.GPIO_Pin = SD_PRESENT_GPIO_PIN;
  GPIO_Init(SD_PRESENT_GPIO, &GPIO_InitStructure);

  // TRAINER DETECT PIN
  GPIO_InitStructure.GPIO_Pin = TRAINER_DETECT_GPIO_PIN;
  GPIO_Init(TRAINER_DETECT_GPIO, &GPIO_InitStructure);
}

void pwrOn()
{
  // we keep the init of the PIN to have pwrOn as quick as possible

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = PWR_ON_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(PWR_ON_GPIO, &GPIO_InitStructure);

  GPIO_SetBits(PWR_ON_GPIO, PWR_ON_GPIO_PIN);
}

void pwrOff()
{
  GPIO_ResetBits(PWR_ON_GPIO, PWR_ON_GPIO_PIN);
}

bool pwrPressed()
{
  return GPIO_ReadInputDataBit(PWR_ON_GPIO, PWR_SWITCH_GPIO_PIN) == Bit_RESET;
}

void pwrResetHandler()
{
  RCC->AHB1ENR |= PWR_RCC_AHB1Periph;

  // these two NOPs are needed (see STM32F errata sheet) before the peripheral
  // register can be written after the peripheral clock was enabled
  __ASM volatile ("nop");
  __ASM volatile ("nop");

  if (WAS_RESET_BY_WATCHDOG_OR_SOFTWARE()) {
    pwrOn();
  }
}
