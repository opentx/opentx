/**
  ******************************************************************************
  * @file    Project/spi/spi.c 
  * @author  FrSky Application Team
  * @Hardware version V0.2
  * @date    11-July-2012
  * @brief   This file provides spi Init(Analog spi),read and write.
             driver for LCD.
  * *
  ******************************************************************************
*/

#include "../opentx.h"

/**
**********send command to lcd**************
*/
//hardware spi
/*u16 spiCmd(u8 addr)
{
	u8 readValue;
	
	SPI_RS_LOW();
    SPI_NCS_LOW();
	
	SPI_WRITE_BYTE(addr);
	SPI_WAIT_DONE();

	readValue=SPI_READ_BYTE();
	SPI_NCS_HIGH();
    return	readValue;
}*/

//Anolog spi
void AspiCmd(u8 Command_Byte)
{
    int i=8; 
    LCD_A0_LOW();

    LCD_CLK_HIGH();
    LCD_NCS_LOW();  
 
    while (i--) {
      LCD_CLK_LOW();

      if (Command_Byte&0x80)
        LCD_MOSI_HIGH();
      else
        LCD_MOSI_LOW();

      Command_Byte <<= 1;

      LCD_CLK_HIGH();
    }

    LCD_NCS_HIGH();  
    LCD_A0_HIGH();
}

/**

********send data to lcd**************

*/

//hardware spi
/*
u16 spiData(u8 addr)
{
	u16 readValue;
      	OLED_RS_HIGH();
	SPI_NCS_LOW();
	
	SPI_WRITE_BYTE(addr);
	SPI_WAIT_DONE();

	readValue=SPI_READ_BYTE();
	SPI_NCS_HIGH();
      	return	readValue;
}
*/

//Anolog spi
void AspiData(u8 Para_data)
{
    int i=8;
    LCD_CLK_HIGH();
    LCD_A0_HIGH();
    LCD_NCS_LOW();
    while (i--) {
        if (Para_data&0x80)
          LCD_MOSI_HIGH();
        else
          LCD_MOSI_LOW();
        Para_data <<= 1;
        LCD_CLK_LOW();
        __no_operation();
        LCD_CLK_HIGH();
        __no_operation();
    }
    LCD_NCS_HIGH();
    LCD_A0_HIGH();  
}

/***
**********read one byte in a register*******
*/

/*
u16 spiRegAccess(u8 addrByte, u8 writeValue)
{
	u8 readValue;

	SPI_NCS_LOW();
	
	SPI_WRITE_BYTE(addrByte);
	SPI_WAIT_DONE();

	SPI_WRITE_BYTE(writeValue);
	SPI_WAIT_DONE();

	readValue=SPI_READ_BYTE();
	SPI_NCS_HIGH();
	return	readValue;
}

*/


