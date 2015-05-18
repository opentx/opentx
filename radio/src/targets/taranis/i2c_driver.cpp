#include "board_taranis.h"

#define EE_CMD_WRITE  (0)
#define EE_CMD_READ   (1)

#define SCL_H         do { I2C_GPIO->BSRRL = I2C_GPIO_PIN_SCL; } while(0)
#define SCL_L         do { I2C_GPIO->BSRRH = I2C_GPIO_PIN_SCL; } while(0)
#define SDA_H         do { I2C_GPIO->BSRRL = I2C_GPIO_PIN_SDA; } while(0)
#define SDA_L         do { I2C_GPIO->BSRRH = I2C_GPIO_PIN_SDA; } while(0)
#define SCL_read      (I2C_GPIO->IDR & I2C_GPIO_PIN_SCL)
#define SDA_read      (I2C_GPIO->IDR & I2C_GPIO_PIN_SDA)

void I2C_EE_PageWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint8_t NumByteToWrite);
void I2C_EE_WaitEepromStandbyState(void);

void I2C_set_volume(register uint8_t volume);
uint8_t I2C_read_volume(void);

#define	I2C_delay()   delay_01us(100);

short I2C_START(void)
{
  SDA_H;
  I2C_delay();
  SCL_H;
  I2C_delay();
  // if (!SDA_read) return 0;
  SDA_L;
  I2C_delay();
  // if (SDA_read) return 0;
  SCL_L;
  I2C_delay();
  return 1;
}

void I2C_STOP(void)
{
  SCL_L;
  I2C_delay();
  SDA_L;
  I2C_delay();
  SCL_H;
  I2C_delay();
  SDA_H;
  I2C_delay();
}

void I2C_ACK(void)
{
  SCL_L;
  I2C_delay();
  SDA_L;
  I2C_delay();
  SCL_H;
  I2C_delay();
  SCL_L;
  I2C_delay();
}

void I2C_NO_ACK(void)
{
  SCL_L;
  I2C_delay();
  SDA_H;
  I2C_delay();
  SCL_H;
  I2C_delay();
  SCL_L;
  I2C_delay();
}

short I2C_WAIT_ACK(void)
{
  short i=50;
  SCL_L;
  I2C_delay();
  SDA_H;
  I2C_delay();
  SCL_H;
  I2C_delay();
  while (i) {
    if(SDA_read) {
      I2C_delay();
      i--;
    }
    else {
      i=2;
      break;
    }
  }
  SCL_L;
  I2C_delay();

  return i;
} 

void I2C_SEND_DATA(char SendByte)
{
  short i=8;
  while (i--) {
    SCL_L;
    // I2C_delay();
    if (SendByte & 0x80)
      SDA_H;
    else
      SDA_L;
    SendByte <<= 1;
    I2C_delay();
    SCL_H;
    I2C_delay();
  }
  SCL_L;
  I2C_delay();
}

char I2C_READ(void)
{ 
  short i=8;
  char ReceiveByte=0;

  SDA_H;
  while (i--) {
    ReceiveByte <<= 1;
    SCL_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    if (SDA_read) {
      ReceiveByte|=0x01;
    }
  }
  SCL_L;
  return ReceiveByte;
} 

void eepromInit(void)
{
  RCC_AHB1PeriphClockCmd(I2C_RCC_AHB1Periph_GPIO, ENABLE);
  RCC_APB1PeriphClockCmd(I2C_RCC_APB1Periph_I2C, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = I2C_GPIO_PIN_SCL | I2C_GPIO_PIN_SDA | I2C_GPIO_PIN_WP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(I2C_GPIO, &GPIO_InitStructure);

#if 0
  GPIO_PinAFConfig(I2C_GPIO, I2C_GPIO_PIN_SCL, I2C_GPIO_AF);
  GPIO_PinAFConfig(I2C_GPIO, I2C_GPIO_PIN_SDA, I2C_GPIO_AF);

  I2C_InitTypeDef I2C_InitStructure;
  I2C_InitStructure.I2C_OwnAddress1 = 0x00; // MPU6050 7-bit adress = 0x68, 8-bit adress = 0xD0;
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

  I2C_Init(I2C, &I2C_InitStructure);
  I2C_Cmd(I2C, ENABLE);
#endif

  GPIO_ResetBits(I2C_GPIO, I2C_GPIO_PIN_WP);

  // Set Idle levels
  SDA_H;
  SCL_H;
}

/**
  * @brief  Reads a block of data from the EEPROM.
  * @param  pBuffer : pointer to the buffer that receives the data read
  *   from the EEPROM.
  * @param  ReadAddr : EEPROM's internal address to read from.
  * @param  NumByteToRead : number of bytes to read from the EEPROM.
  * @retval None
  */
void eepromReadBlock(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
  I2C_START();
  I2C_SEND_DATA(I2C_ADDRESS_EEPROM|EE_CMD_WRITE);
  I2C_WAIT_ACK();
  I2C_SEND_DATA((uint8_t)((ReadAddr & 0xFF00) >> 8));
  I2C_WAIT_ACK();
  I2C_SEND_DATA((uint8_t)(ReadAddr & 0x00FF));
  I2C_WAIT_ACK();
	
  I2C_START();
  I2C_SEND_DATA(I2C_ADDRESS_EEPROM|EE_CMD_READ);
  I2C_WAIT_ACK();
  while (NumByteToRead) {
    if (NumByteToRead == 1) {
      *pBuffer = I2C_READ();
      I2C_NO_ACK();
      I2C_STOP();
    }
    else {
      *pBuffer = I2C_READ();
      I2C_ACK();
      pBuffer++;
    }
    NumByteToRead--;
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
void eepromWriteBlock(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, count = 0;
  uint16_t Addr = 0;

  Addr = WriteAddr % I2C_FLASH_PAGESIZE;
  count = I2C_FLASH_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / I2C_FLASH_PAGESIZE;
  NumOfSingle = NumByteToWrite % I2C_FLASH_PAGESIZE;

  /* If WriteAddr is I2C_FLASH_PAGESIZE aligned  */
  if (Addr == 0) {
    /* If NumByteToWrite < I2C_FLASH_PAGESIZE */
    if (NumOfPage == 0) {
      I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      I2C_EE_WaitEepromStandbyState();
    }
    /* If NumByteToWrite > I2C_FLASH_PAGESIZE */
    else {
      while (NumOfPage--) {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_FLASH_PAGESIZE);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr += I2C_FLASH_PAGESIZE;
        pBuffer += I2C_FLASH_PAGESIZE;
      }

      if (NumOfSingle != 0) {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
    }
  }
  /* If WriteAddr is not I2C_FLASH_PAGESIZE aligned  */
  else {
    /* If NumByteToWrite < I2C_FLASH_PAGESIZE */
    if (NumOfPage== 0) {
      /* If the number of data to be written is more than the remaining space
      in the current page: */
      if (NumByteToWrite > count) {
        /* Write the data conained in same page */
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        I2C_EE_WaitEepromStandbyState();

        /* Write the remaining data in the following page */
        I2C_EE_PageWrite((uint8_t*)(pBuffer + count), (WriteAddr + count), (NumByteToWrite - count));
        I2C_EE_WaitEepromStandbyState();
      }
      else {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
    }
    /* If NumByteToWrite > I2C_FLASH_PAGESIZE */
    else {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / I2C_FLASH_PAGESIZE;
      NumOfSingle = NumByteToWrite % I2C_FLASH_PAGESIZE;

      if (count != 0) {
        I2C_EE_PageWrite(pBuffer, WriteAddr, count);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr += count;
        pBuffer += count;
      }

      while (NumOfPage--) {
        I2C_EE_PageWrite(pBuffer, WriteAddr, I2C_FLASH_PAGESIZE);
        I2C_EE_WaitEepromStandbyState();
        WriteAddr +=  I2C_FLASH_PAGESIZE;
        pBuffer += I2C_FLASH_PAGESIZE;
      }
      if (NumOfSingle != 0) {
        I2C_EE_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        I2C_EE_WaitEepromStandbyState();
      }
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
void I2C_EE_PageWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint8_t NumByteToWrite)
{
  I2C_START();
  I2C_SEND_DATA(I2C_ADDRESS_EEPROM|EE_CMD_WRITE);
  I2C_WAIT_ACK();

  I2C_SEND_DATA((uint8_t)((WriteAddr & 0xFF00) >> 8));
  I2C_WAIT_ACK();
  I2C_SEND_DATA((uint8_t)(WriteAddr & 0x00FF));
  I2C_WAIT_ACK();

  /* While there is data to be written */
  while (NumByteToWrite--) {
    I2C_SEND_DATA(*pBuffer);
    I2C_WAIT_ACK();
    pBuffer++;
  }
  I2C_STOP();
}

/**
  * @brief  Wait for EEPROM Standby state
  * @param  None
  * @retval None
  */
void I2C_EE_WaitEepromStandbyState(void)
{
  do {
    I2C_START();
    I2C_SEND_DATA(I2C_ADDRESS_EEPROM|EE_CMD_WRITE);
  } while (0 == I2C_WAIT_ACK());

  I2C_STOP();
}

void setVolume(uint8_t volume)
{
  if (volume > VOLUME_LEVEL_MAX) {
    volume = VOLUME_LEVEL_MAX;
  }

  I2C_START();
  I2C_SEND_DATA(I2C_ADDRESS_CAT5137|EE_CMD_WRITE);
  I2C_WAIT_ACK();
  I2C_SEND_DATA(0);
  I2C_WAIT_ACK();
  I2C_SEND_DATA(volume);
  I2C_WAIT_ACK();
  I2C_STOP();
}

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
