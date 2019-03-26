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

void i2c2Init()
{
  I2C_DeInit(I2CX);

  I2C_InitTypeDef I2C_InitStructure;
  I2C_InitStructure.I2C_ClockSpeed = I2CX_SPEED;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2CX, &I2C_InitStructure);
  I2C_Cmd(I2CX, ENABLE);

  GPIO_PinAFConfig(I2CX_SCL_GPIO, I2CX_SCL_GPIO_PinSource, I2CX_GPIO_AF);
  GPIO_PinAFConfig(I2CX_SDA_GPIO, I2CX_SDA_GPIO_PinSource, I2CX_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = I2CX_SCL_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(I2CX_SCL_GPIO, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = I2CX_SDA_GPIO_PIN;
  GPIO_Init(I2CX_SDA_GPIO, &GPIO_InitStructure);
}

#define I2C_TIMEOUT_MAX 1000
bool I2C_WaitEvent(uint32_t event)
{
  uint32_t timeout = I2C_TIMEOUT_MAX;
  while (!I2C_CheckEvent(I2CX, event)) {
    if ((timeout--) == 0) return false;
  }
  return true;
}

bool I2C2_WaitEventCleared(uint32_t event)
{
  uint32_t timeout = I2C_TIMEOUT_MAX;
  while (I2C_CheckEvent(I2CX, event)) {
    if ((timeout--) == 0) return false;
  }
  return true;
}

void setVolume(uint8_t volume)
{
  if (!I2C2_WaitEventCleared(I2C_FLAG_BUSY))
    return;

  I2C_GenerateSTART(I2CX, ENABLE);
  if (!I2C2_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
    return;

  I2C_Send7bitAddress(I2C, I2C_ADDRESS_VOLUME, I2C_Direction_Transmitter);
  if (!I2C2_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    return;

  I2C_SendData(I2CX, 0);
  if (!I2C2_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING))
    return;
  I2C_SendData(I2CX, volume);
  if (!I2C2_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    return;

  I2C_GenerateSTOP(I2CX, ENABLE);
}

int32_t getVolume()
{
  return -1; // TODO
}
#endif
