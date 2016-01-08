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

#include "../../pwr.h"
#include "../horus/board_horus.h"

void pwrInit()
{
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

  // Init Module PWR
  GPIO_ResetBits(EXTMODULE_GPIO_PWR, EXTMODULE_GPIO_PIN_PWR);
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_GPIO_PIN_PWR;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_Init(EXTMODULE_GPIO_PWR, &GPIO_InitStructure);

  pwrOn();
}

void pwrOn()
{
  GPIO_SetBits(PWR_GPIO, PWR_GPIO_PIN_ON);
}

void pwrOff()
{
  GPIO_ResetBits(PWR_GPIO, PWR_GPIO_PIN_ON);
}

uint32_t pwrPressed()
{
  return GPIO_ReadInputDataBit(PWR_GPIO, PWR_GPIO_PIN_SWITCH) == Bit_RESET;
}
