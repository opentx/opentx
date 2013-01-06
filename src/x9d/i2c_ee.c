/**
  ******************************************************************************
  * @file    Project/ee_drivers/i2c_ee.c
  * @author  X9D Application Team
  * @version V 0.2
  * @date    12-JULY-2012
  * @brief   This file provides a set of functions needed to manage the
  *          communication between I2C peripheral and I2C M24CXX EEPROM.
             CAT5137 added,share the I2C.
  ******************************************************************************
*/

#include "../open9x.h"

/**
  * @brief  Configure the used I/O ports pin
  * @param  None
  * @retval None
  */
static void I2C_GPIO_Configuration(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  GPIOB->BSRRH =I2C_EE_WP;	//PB9
  GPIO_InitStructure.GPIO_Pin = I2C_EE_WP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(I2C_EE_WP_GPIO, &GPIO_InitStructure);

  /* Configure I2C_EE pins: SCL and SDA */
  GPIO_InitStructure.GPIO_Pin =  I2C_EE_SCL | I2C_EE_SDA;//PE0,PE1
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(I2C_EE_GPIO, &GPIO_InitStructure);
}

short I2C_START()
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

void I2C_STOP()
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

void I2C_ACK()
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
void I2C_NO_ACK()
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

short I2C_WAIT_ACK()
{
  short i=50;
  SCL_L;
  I2C_delay();
  SDA_H;
  I2C_delay();
  SCL_H;
  I2C_delay();
  while(i) {
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
    I2C_delay();
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

char I2C_READ()
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

void I2C_EE_Init()
{
  /* GPIO Periph clock enable */
  RCC_AHB1PeriphClockCmd(I2C_EE_GPIO_CLK, ENABLE);

  /* GPIO configuration */
  I2C_GPIO_Configuration();
}

/**
  * @brief  Writes one byte to the I2C EEPROM.
  * @param  pBuffer : pointer to the buffer  containing the data to be
  *   written to the EEPROM.
  * @param  WriteAddr : EEPROM's internal address to write to.
  * @retval None
  */
void I2C_EE_ByteWrite(uint8_t* pBuffer, uint16_t WriteAddr)
{
  I2C_START();
  I2C_SEND_DATA(I2C_EEPROM_ADDRESS|EE_CMD_WRITE);
  I2C_WAIT_ACK();
#ifdef EE_M24C08
  I2C_SEND_DATA(WriteAddr);
  I2C_WAIT_ACK();
#else
  I2C_SEND_DATA((uint8_t)((WriteAddr&0xFF00)>>8) );
  I2C_WAIT_ACK();
  I2C_SEND_DATA((uint8_t)(WriteAddr&0xFF));
  I2C_WAIT_ACK();
#endif
  I2C_SEND_DATA(*pBuffer);
  I2C_WAIT_ACK();
  I2C_STOP();
}

/**
  * @brief  Reads a block of data from the EEPROM.
  * @param  pBuffer : pointer to the buffer that receives the data read
  *   from the EEPROM.
  * @param  ReadAddr : EEPROM's internal address to read from.
  * @param  NumByteToRead : number of bytes to read from the EEPROM.
  * @retval None
  */
void I2C_EE_BufferRead(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{
  I2C_START();
  I2C_SEND_DATA(I2C_EEPROM_ADDRESS|EE_CMD_WRITE);
  I2C_WAIT_ACK();
#ifdef EE_M24C08
  I2C_SEND_DATA(ReadAddr);
  I2C_WAIT_ACK();
#else
  I2C_SEND_DATA((uint8_t)((ReadAddr & 0xFF00) >> 8));
  I2C_WAIT_ACK();
  I2C_SEND_DATA((uint8_t)(ReadAddr & 0x00FF));
  I2C_WAIT_ACK();
#endif
  I2C_STOP();
	
  I2C_START();
  I2C_SEND_DATA(I2C_EEPROM_ADDRESS|EE_CMD_READ);
  I2C_WAIT_ACK();
  while (NumByteToRead) {
    if (NumByteToRead == 1) {
      *pBuffer =I2C_READ();
      I2C_NO_ACK();
      I2C_STOP();
    }
    else {
      *pBuffer =I2C_READ();
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
void I2C_EE_BufferWrite(uint8_t* pBuffer, uint16_t WriteAddr, uint16_t NumByteToWrite)
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

      lcd_clear();
      lcd_putsAtt(100, 10, "WaitStdby", GREY1);
      lcd_putsAtt(100, 20, "WaitStdby", GREY2);
      lcd_putsAtt(100, 30, "WaitStdby", 0);
      lcdRefresh();

      I2C_EE_WaitEepromStandbyState();

      lcd_clear();
      lcd_putsAtt(100, 10, "Fin WaitStdby", GREY2);
      lcdRefresh();

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
  I2C_SEND_DATA(I2C_EEPROM_ADDRESS|EE_CMD_WRITE);
  I2C_WAIT_ACK();

#ifdef EE_M24C08
  I2C_SEND_DATA(WriteAddr);
  I2C_WAIT_ACK();
#else
  I2C_SEND_DATA((uint8_t)((WriteAddr & 0xFF00) >> 8));
  I2C_WAIT_ACK();
  I2C_SEND_DATA((uint8_t)(WriteAddr & 0x00FF));
  I2C_WAIT_ACK();
#endif /* EE_M24C08 */

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
    I2C_SEND_DATA(I2C_EEPROM_ADDRESS|EE_CMD_WRITE);
  } while (0 == I2C_WAIT_ACK());

  I2C_STOP();
}
