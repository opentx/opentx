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

#include "board_taranis.h"

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

#if defined(REVPLUS)
void AspiCmd(uint8_t Command_Byte)
{
  LCD_A0_LOW() ;
  LCD_NCS_LOW() ;  
  while ( ( SPI3->SR & SPI_SR_TXE ) == 0 ) {
    // wait
  }
  (void)SPI3->DR ;		// Clear receive
  SPI3->DR = Command_Byte ;
  while ( ( SPI3->SR & SPI_SR_RXNE ) == 0 ) {
    // wait
  }
  LCD_NCS_HIGH() ;
}
#else
// Analog SPI
void AspiCmd(u8 Command_Byte)
{
    int i=8; 
    LCD_A0_LOW();

    LCD_CLK_HIGH();
    LCD_CLK_HIGH();
    LCD_NCS_LOW();
 
    while (i--) {
      LCD_CLK_LOW();

      if (Command_Byte&0x80)
        LCD_MOSI_HIGH();
      else
        LCD_MOSI_LOW();

      Command_Byte <<= 1;
      LCD_CLK_LOW(); \
      LCD_CLK_LOW(); \

      LCD_CLK_HIGH();
      LCD_CLK_HIGH();
    }

    LCD_NCS_HIGH();  
    LCD_A0_HIGH();
}
#endif

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
    LCD_CLK_HIGH();
    LCD_A0_HIGH();
    LCD_NCS_LOW();
    while (i--) {
    	LCD_CLK_LOW();
        if (Para_data&0x80)
          LCD_MOSI_HIGH();
        else
          LCD_MOSI_LOW();
        Para_data <<= 1;
        LCD_CLK_LOW();
        LCD_CLK_LOW();

        LCD_CLK_HIGH();
        LCD_CLK_HIGH();
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


