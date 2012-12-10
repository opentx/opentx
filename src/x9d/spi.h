/**
  ******************************************************************************
  * @file    Project/spi/spi.h 
  * @author  X9D Application Team
  * @Hardware version V0.2
  * @date    11-July-2012
  * @brief   spi.c' Header file.
  * *
  ******************************************************************************
*/
#ifndef _SPI_H_
#define _SPI_H_

#include "stm32f2xx.h"

#define	LCD_NCS_HIGH()		    do{GPIO_SetBits(GPIO_LCD, PIN_LCD_NCS);}while(0)
#define	LCD_NCS_LOW()		    do{GPIO_ResetBits(GPIO_LCD, PIN_LCD_NCS);}while(0)

#define LCD_A0_HIGH()           do{GPIO_SetBits(GPIO_LCD, PIN_LCD_A0);}while(0)
#define LCD_A0_LOW()            do{GPIO_ResetBits(GPIO_LCD, PIN_LCD_A0);}while(0)

#define LCD_RST_HIGH()		    do{GPIO_SetBits(GPIO_LCD, PIN_LCD_RST);}while(0)
#define LCD_RST_LOW()		    do{GPIO_ResetBits(GPIO_LCD, PIN_LCD_RST);}while(0)

#define LCD_CLK_HIGH()		    do{GPIO_SetBits(GPIO_LCD, PIN_LCD_CLK);}while(0)
#define LCD_CLK_LOW()		    do{GPIO_ResetBits(GPIO_LCD, PIN_LCD_CLK);}while(0)

#define LCD_MOSI_HIGH()		    do{GPIO_SetBits(GPIO_LCD, PIN_LCD_MOSI);}while(0)
#define LCD_MOSI_LOW()		    do{GPIO_ResetBits(GPIO_LCD, PIN_LCD_MOSI);}while(0)

void AspiCmd(u8 Command_Byte);
void AspiData(u8 Para_data);
//u16 spiRegAccess(u8 addrByte, u8 writeValue);

#endif

