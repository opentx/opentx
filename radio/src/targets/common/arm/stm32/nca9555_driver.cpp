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

constexpr uint8_t NCA9555_REG_CMD_INPUT0 = 0x00;
constexpr uint8_t NCA9555_REG_CMD_INPUT1 = 0x01;
constexpr uint8_t NCA9555_REG_CMD_OUTPUT0 = 0x02;
constexpr uint8_t NCA9555_REG_CMD_OUTPUT1 = 0x03;
constexpr uint8_t NCA9555_REG_CMD_CONFIGURATION0 = 0x06;
constexpr uint8_t NCA9555_REG_CMD_CONFIGURATION1 = 0x07;

uint16_t inputValues[2] = {0xFFFF, 0xFFFF};

constexpr uint8_t I2C_TIMEOUT = 100; // 10ms

enum {
  INPUT1_FS_4,
  INPUT1_FS_3,
  INPUT1_FS_2,
  INPUT1_FS_1,
  INPUT1_TRIM_RHR,
  INPUT1_TRIM_RHL,
  INPUT1_SWL_L,
  INPUT1_SPARE,
  INPUT1_KEY_RTN,
  INPUT1_KEY_PAGE,
  INPUT1_KEY_SYS,
  INPUT1_KEY_DISP,
  INPUT1_KEY_MDL,
  INPUT1_SWK_L,
  INPUT1_TRIM_LHL,
  INPUT1_TRIM_LHR
};

enum {
  INPUT2_SWC_H,
  INPUT2_SWC_L,
  INPUT2_SWH_L,
  INPUT2_SWD_L,
  INPUT2_SWD_H,
  INPUT2_TRIM_RVU,
  INPUT2_TRIM_RVD,
  INPUT2_SWJ_L,
  INPUT2_SWI_L,
  INPUT2_TRIM_LVD,
  INPUT2_TRIM_LVU,
  INPUT2_SWA_L,
  INPUT2_SWA_H,
  INPUT2_SWF_L,
  INPUT2_SWB_L,
  INPUT2_SWB_H,
};

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
  GPIO_InitStructure.GPIO_Pin = I2CX_SCL_GPIO_PIN | I2CX_SDA_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(I2CX_SCL_GPIO, &GPIO_InitStructure);
}

void i2c2Start()
{
  I2CX->CR1 |= I2C_CR1_START;
}

void i2c2Stop()
{
  I2CX->CR1 |= I2C_CR1_STOP;
}

void i2c2Abort()
{
  I2CX->CR1 = 0;
  I2CX->CR1 |= I2C_CR1_PE;
}

bool i2c2WaitIdle()
{
  uint32_t elapsed = 0;
  while (I2CX->SR2 & I2C_SR2_BUSY) {
    if (elapsed++ >= I2C_TIMEOUT) {
      TRACE("I2CX waitIdle() failed");
      i2c2Abort();
      return false;
    }
    delay_us(100);
  }
  return true;
}

bool i2c2WaitEvent(uint32_t event)
{
  uint32_t elapsed = 0;
  while (!I2C_CheckEvent(I2CX, event)) {
    if (elapsed++ >= I2C_TIMEOUT) {
      TRACE("I2CX waitEvent(%02X) failed", event);
      i2c2Abort();
      return false;
    }
    delay_us(100);
  }
  return true;
}

bool i2c2Read(uint8_t addr, uint8_t * data, uint8_t len)
{
  if (!i2c2WaitIdle())
    return false;

  i2c2Start();
  if (!i2c2WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
    return false;

  I2C_Send7bitAddress(I2CX, addr, I2C_Direction_Receiver);
  I2C_AcknowledgeConfig(I2CX, ENABLE);

  for (uint8_t i = 0; i < len; i++) {
    if (i == len - 1)
      I2C_AcknowledgeConfig(I2CX, DISABLE);
    if (!i2c2WaitEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
      return false;

    data[i] = I2C_ReceiveData(I2CX);
  }

  i2c2Stop();
  return true;
}

bool i2c2Write(uint8_t addr, const uint8_t * data, uint8_t len)
{
  if (!i2c2WaitIdle()) {
    return false;
  }

  i2c2Start();
  if (!i2c2WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
    return false;

  I2C_Send7bitAddress(I2CX, addr, I2C_Direction_Transmitter);
  I2C_AcknowledgeConfig(I2CX, ENABLE);
  if (!i2c2WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    return false;

  for (uint8_t i = 0; i < len; i++) {
    I2C_SendData(I2CX, data[i]);
    if (!i2c2WaitEvent(i == len - 1 ? I2C_EVENT_MASTER_BYTE_TRANSMITTED : I2C_EVENT_MASTER_BYTE_TRANSMITTING))
      return false;
  }
  
  i2c2Stop();
  return true;
}

uint16_t nca9555ReadRegister(uint8_t addr, uint8_t cmd)
{
  uint8_t data[2] = {};
  if (!i2c2Write(addr, &cmd, 1) || !i2c2Read(addr, data, 2)) {
    TRACE("nca9555ReadRegister(addr=%02X, cmd=%02X) failed", addr, cmd);
  }
  return *((uint16_t *)data);
}

void nca9555WriteRegister(uint8_t addr, uint8_t cmd, uint8_t port0, uint8_t port1)
{
  uint8_t data[3] = {cmd, port0, port1};
  if (!i2c2Write(addr, data, 3)) {
    TRACE("nca9555WriteRegister(addr=%02X, cmd=%02X) failed", addr, cmd);
  }
}

void nca9555Read()
{
  inputValues[0] = nca9555ReadRegister(NCA9555_I2C_ADDR, NCA9555_REG_CMD_INPUT0);
#if defined(NCA9555_I2C_ADDR2)
  inputValues[1] = nca9555ReadRegister(NCA9555_I2C_ADDR2, NCA9555_REG_CMD_INPUT0);
#endif
}

void nca9555Init()
{
  nca9555WriteRegister(NCA9555_I2C_ADDR, NCA9555_REG_CMD_CONFIGURATION0, 0b11111111, 0b11111111);
#if defined(NCA9555_I2C_ADDR2)
  nca9555WriteRegister(NCA9555_I2C_ADDR2, NCA9555_REG_CMD_CONFIGURATION0, 0b11111111, 0b11111111);
#endif
  nca9555Read();
}

bool isKeyPressed(uint8_t key)
{
  uint32_t value = inputValues[0];
  switch (key) {
    case KEY_EXIT:
      return !(value & (1 << INPUT1_KEY_RTN));

    case KEY_PAGE:
      return !(value & (1 << INPUT1_KEY_PAGE));

    case KEY_SYS:
      return !(value & (1 << INPUT1_KEY_SYS));

    case KEY_TELE:
      return !(value & (1 << INPUT1_KEY_DISP));

    case KEY_MODEL:
      return !(value & (1 << INPUT1_KEY_MDL));

    default:
      return false;
  }
}

uint32_t readTrims()
{
  uint32_t result = 0;

  uint32_t value1 = inputValues[0];
  uint32_t value2 = inputValues[1];

  if (!(value1 & (1 << INPUT1_TRIM_LHL)))
    result |= 0x01u;
  if (!(value1 & (1 << INPUT1_TRIM_LHR)))
    result |= 0x02u;
  if (!(value1 & (1 << INPUT1_TRIM_RHL)))
    result |= 0x40u;
  if (!(value1 & (1 << INPUT1_TRIM_RHR)))
    result |= 0x80u;

  if (!(value2 & (1 << INPUT2_TRIM_LVD)))
    result |= 0x04u;
  if (!(value2 & (1 << INPUT2_TRIM_LVU)))
    result |= 0x08u;
  if (!(value2 & (1 << INPUT2_TRIM_RVD)))
    result |= 0x10u;
  if (!(value2 & (1 << INPUT2_TRIM_RVU)))
    result |= 0x20u;

  return result;
}

uint8_t getFunctionSwitchesStates()
{
  //uint32_t value2 = inputValues[1];
  uint8_t result = 0;
  /*if (!(value2 & (1 << INPUT2_FS_1)))
    result |= 1u << 0u;
  if (!(value2 & (1 << INPUT2_FS_2)))
    result |= 1u << 1u;
  if (!(value2 & (1 << INPUT2_FS_3)))
    result |= 1u << 2u;
  if (!(value2 & (1 << INPUT2_FS_4)))
    result |= 1u << 3u;*/
  return result;
}

constexpr uint8_t SWITCH_POSITION(uint8_t swtch, uint8_t position)
{
  return swtch * 3 + position;
}

#define ADD_2POS_CASES(x) \
  case SWITCH_POSITION(SW_S ## x, 2): \
    xxx = !((1 << INPUT2_SW ## x ## _L) & inputValue); \
    break; \
  case SWITCH_POSITION(SW_S ## x, 0): \
    xxx = (1 << INPUT2_SW ## x ## _L) & inputValue; \
    break

#define ADD_3POS_CASES(x) \
  case SWITCH_POSITION(SW_S ## x, 2): \
    xxx = (1 << INPUT2_SW ## x ## _H) & inputValue; \
    xxx = xxx && !(1 << INPUT2_SW ## x ## _L & inputValue); \
    break; \
  case SWITCH_POSITION(SW_S ## x, 1): \
    xxx = (1 << INPUT2_SW ## x ## _H & inputValue) && (1 << INPUT2_SW ## x ## _L & inputValue); \
    break; \
  case SWITCH_POSITION(SW_S ## x, 0): \
    xxx = (1 << INPUT2_SW ## x ## _L) & inputValue; \
    xxx = xxx && !(1 << INPUT2_SW ## x ## _H & inputValue); \
    break

uint32_t switchStateFromExternalChip(uint8_t index)
{
  bool xxx = false;
  uint32_t inputValue = inputValues[1];

  switch (index) {
    ADD_3POS_CASES(A);
    ADD_3POS_CASES(B);
    ADD_3POS_CASES(C);
    ADD_3POS_CASES(D);
    ADD_2POS_CASES(F);
    ADD_2POS_CASES(H);
    ADD_2POS_CASES(I);
    ADD_2POS_CASES(J);
  }

  return xxx;
}


