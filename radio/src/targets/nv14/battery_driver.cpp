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

#define  __BATTERY_DRIVER_C__


void init_battery_charge()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = PWR_CHARGE_FINISHED_GPIO_PIN | PWR_CHARGING_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(PWR_CHARGING_GPIO, &GPIO_InitStructure);
  GPIO_SetBits(PWR_CHARGING_GPIO, PWR_CHARGE_FINISHED_GPIO_PIN | PWR_CHARGING_GPIO_PIN);
}

uint16_t get_battery_charge_state()
{
  if (!READ_CHARGE_FINISHED_STATE())
  {
      return CHARGE_FINISHED;
  }
  else if (!READ_CHARGING_STATE())
  {
      return CHARGE_STARTED;
  }

  return CHARGE_NONE;
}


uint16_t getBatteryVoltage()
{
#if defined (SIMU)
  return 350 + g_eeGeneral.txVoltageCalibration;
#else
  int32_t t = getAnalogValue(TX_VOLTAGE) + g_eeGeneral.txVoltageCalibration;

  t *= ( 3300 * 20);
  t /= ( 4095 * 10);
  t = ( t * 100  + 500 ) / 1000;

  return t;
#endif
}



















