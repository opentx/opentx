#include "board_taranis.h"

void eepromPageWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint8_t NumByteToWrite);
void eepromWaitEepromStandbyState(void);

void i2cInit()
{
  I2C_DeInit(I2C);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = I2C_GPIO_PIN_WP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(I2C_GPIO, &GPIO_InitStructure);
  GPIO_ResetBits(I2C_GPIO, I2C_GPIO_PIN_WP);

  I2C_InitTypeDef I2C_InitStructure;
  I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2C, &I2C_InitStructure);
  I2C_Cmd(I2C, ENABLE);

  GPIO_PinAFConfig(I2C_GPIO, I2C_GPIO_PinSource_SCL, I2C_GPIO_AF);
  GPIO_PinAFConfig(I2C_GPIO, I2C_GPIO_PinSource_SDA, I2C_GPIO_AF);

  GPIO_InitStructure.GPIO_Pin = I2C_GPIO_PIN_SCL | I2C_GPIO_PIN_SDA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(I2C_GPIO, &GPIO_InitStructure);
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

/**
  * @brief  Reads a block of data from the EEPROM.
  * @param  pBuffer : pointer to the buffer that receives the data read
  *   from the EEPROM.
  * @param  ReadAddr : EEPROM's internal address to read from.
  * @param  NumByteToRead : number of bytes to read from the EEPROM.
  * @retval None
  */
bool I2C_EE_ReadBlock(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
  if (!I2C_WaitEventCleared(I2C_FLAG_BUSY))
    return false;

  I2C_GenerateSTART(I2C, ENABLE);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
    return false;

  I2C_Send7bitAddress(I2C, I2C_ADDRESS_EEPROM, I2C_Direction_Transmitter);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    return false;

  I2C_SendData(I2C, (uint8_t)((ReadAddr & 0xFF00) >> 8));
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING))
    return false;
  I2C_SendData(I2C, (uint8_t)(ReadAddr & 0x00FF));
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    return false;

  I2C_GenerateSTART(I2C, ENABLE);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
    return false;

  I2C_Send7bitAddress(I2C, I2C_ADDRESS_EEPROM, I2C_Direction_Receiver);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    return false;

  if (NumByteToRead > 1) {
    I2C_AcknowledgeConfig(I2C, ENABLE);
  }

  while (NumByteToRead) {
    if (NumByteToRead == 1) {
      I2C_AcknowledgeConfig(I2C, DISABLE);
    }
    if (!I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_RECEIVED))
      return false;
    *pBuffer++ = I2C_ReceiveData(I2C);
    NumByteToRead--;
  }

  I2C_GenerateSTOP(I2C, ENABLE);
  return true;
}

void eepromReadBlock(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
  while (!I2C_EE_ReadBlock(pBuffer, ReadAddr, NumByteToRead)) {
    i2cInit();
  }
}

/**
  * @brief  Writes buffer of data to the I2C EEPROM.
  * @param  pBuffer : pointer to the buffer  containing the data to be
  *   written to the EEPROM.
  * @param  WriteAddr : EEPROM's internal address to write to.
  * @param  NumByteToWrite : number of bytes to write to the EEPROM.
  * @retval None
  */
void eepromWriteBlock(uint8_t* pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
  uint8_t offset = WriteAddr % I2C_FLASH_PAGESIZE;
  uint8_t count = I2C_FLASH_PAGESIZE - offset;
  if (NumByteToWrite < count) {
    count = NumByteToWrite;
  }
  while (count > 0) {
    eepromPageWrite(pBuffer, WriteAddr, count);
    eepromWaitEepromStandbyState();
    WriteAddr += count;
    pBuffer += count;
    NumByteToWrite -= count;
    count = I2C_FLASH_PAGESIZE;
    if (NumByteToWrite < I2C_FLASH_PAGESIZE) {
      count = NumByteToWrite;
    }
  }
}

/**
  * @brief  Writes more than one byte to the EEPROM with a single WRITE cycle.
  * @note   The number of byte can't exceed the EEPROM page size.
  * @param  pBuffer : pointer to the buffer containing the data to be
  *   written to the EEPROM.
  * @param  WriteAddr : EEPROM's internal address to write to.
  * @param  NumByteToWrite : number of bytes to write to the EEPROM.
  * @retval None
  */
bool I2C_EE_PageWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint8_t NumByteToWrite)
{
  if (!I2C_WaitEventCleared(I2C_FLAG_BUSY))
    return false;

  I2C_GenerateSTART(I2C, ENABLE);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
    return false;

  I2C_Send7bitAddress(I2C, I2C_ADDRESS_EEPROM, I2C_Direction_Transmitter);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    return false;

  I2C_SendData(I2C, (uint8_t)((WriteAddr & 0xFF00) >> 8));
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING))
    return false;
  I2C_SendData(I2C, (uint8_t)(WriteAddr & 0x00FF));
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING))
    return false;

  /* While there is data to be written */
  while (NumByteToWrite--) {
    I2C_SendData(I2C, *pBuffer);
    if (!I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING))
      return false;
    pBuffer++;
  }

  if (!I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    return false;

  I2C_GenerateSTOP(I2C, ENABLE);
  return true;
}

void eepromPageWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint8_t NumByteToWrite)
{
  while (!I2C_EE_PageWrite(pBuffer, WriteAddr, NumByteToWrite)) {
    i2cInit();
  }
}

/**
  * @brief  Wait for EEPROM Standby state
  * @param  None
  * @retval None
  */
bool I2C_EE_WaitEepromStandbyState(void)
{
  do {
    I2C_GenerateSTART(I2C, ENABLE);
    if (!I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
      return false;

    I2C_Send7bitAddress(I2C, I2C_ADDRESS_EEPROM, I2C_Direction_Transmitter);
  } while (!I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

  I2C_GenerateSTOP(I2C, ENABLE);
  return true;
}

void eepromWaitEepromStandbyState(void)
{
  while (!I2C_EE_WaitEepromStandbyState()) {
    i2cInit();
  }
}

#if !defined(BOOT)
void setVolume(uint8_t volume)
{
  if (volume > VOLUME_LEVEL_MAX) {
    volume = VOLUME_LEVEL_MAX;
  }

  if (!I2C_WaitEventCleared(I2C_FLAG_BUSY))
    return;

  I2C_GenerateSTART(I2C, ENABLE);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
    return;

  I2C_Send7bitAddress(I2C, I2C_ADDRESS_CAT5137, I2C_Direction_Transmitter);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    return;

  I2C_SendData(I2C, 0);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTING))
    return;
  I2C_SendData(I2C, volumeScale[volume]);
  if (!I2C_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    return;

  I2C_GenerateSTOP(I2C, ENABLE);
}
#endif

#if 0
uint8_t I2C_read_volume()
{
  uint8_t volume ;
  I2C_START();
  I2C_SEND_DATA(I2C_ADDRESS_CAT5137|EE_CMD_WRITE);
  I2C_WAIT_ACK();
  I2C_SEND_DATA(0);
  I2C_WAIT_ACK();
  I2C_START();
  I2C_SEND_DATA(I2C_ADDRESS_CAT5137|EE_CMD_READ);
  I2C_WAIT_ACK();
  volume = I2C_READ();
  I2C_NO_ACK();
  I2C_STOP();
  return volume ;
}
#endif
