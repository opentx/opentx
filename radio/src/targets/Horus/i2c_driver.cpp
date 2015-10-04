#include "board_horus.h"

void i2cInit(void)
{
  RCC_AHB1PeriphClockCmd(I2C_RCC_AHB1Periph_GPIO, ENABLE);
  RCC_APB1PeriphClockCmd(I2C_RCC_APB1Periph_I2C, ENABLE);

  GPIO_PinAFConfig(I2C_GPIO, I2C_GPIO_PinSource_SCL, I2C_GPIO_AF);
  GPIO_PinAFConfig(I2C_GPIO, I2C_GPIO_PinSource_SDA, I2C_GPIO_AF);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = I2C_GPIO_PIN_SCL | I2C_GPIO_PIN_SDA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(I2C_GPIO, &GPIO_InitStructure);

  I2C_InitTypeDef I2C_InitStructure;
  I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2C, &I2C_InitStructure);
  I2C_Cmd(I2C, ENABLE);
}

#define I2C_TIMEOUT_MAX 1000
bool I2C_WaitEvent(uint32_t event)
{
  uint32_t timeout = I2C_TIMEOUT_MAX;
  while (!I2C_CheckEvent(I2C, event)) {
    if ((timeout--) == 0) return false;
  }
  return true;
}

bool I2C_WaitEventCleared(uint32_t event)
{
  uint32_t timeout = I2C_TIMEOUT_MAX;
  while (I2C_CheckEvent(I2C, event)) {
    if ((timeout--) == 0) return false;
  }
  return true;
}

uint8_t i2cReadRegister(uint8_t address, uint8_t index)
{
  if (!I2C_WaitEventCleared(I2C_FLAG_BUSY))
    return 0;

  I2C_GenerateSTART(I2C, ENABLE);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
    return 0;

  I2C_Send7bitAddress(I2C, address, I2C_Direction_Transmitter);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    return 0;

  I2C_SendData(I2C, index);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    return 0;

  I2C_GenerateSTART(I2C, ENABLE);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
    return 0;

  I2C_Send7bitAddress(I2C, address, I2C_Direction_Receiver);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    return 0;

  I2C_AcknowledgeConfig(I2C, DISABLE);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
    return 0;

  uint8_t result = I2C_ReceiveData(I2C);

  I2C_GenerateSTOP(I2C, ENABLE);

  return result;
}

void i2cWriteRegister(uint8_t address, uint8_t index, uint8_t data)
{
  if (!I2C_WaitEventCleared(I2C_FLAG_BUSY))
    return;

  I2C_GenerateSTART(I2C, ENABLE);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
    return;

  I2C_Send7bitAddress(I2C, address, I2C_Direction_Transmitter);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    return;

  I2C_SendData(I2C, index);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    return;

  I2C_SendData(I2C, data);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    return;

  I2C_GenerateSTOP(I2C, ENABLE);
}

void setVolume(uint8_t volume)
{
  if (volume > VOLUME_LEVEL_MAX) {
    volume = VOLUME_LEVEL_MAX;
  }

  i2cWriteRegister(I2C_ADDRESS_CAT5137, 0, volumeScale[volume]);
}
