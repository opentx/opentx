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
#ifndef _ASPI_H_
#define _ASPI_H_

#include "./STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Include/stm32f2xx.h"

#define __no_operation     __NOP

#define	LCD_NCS_HIGH()		GPIO_LCD->BSRRL = PIN_LCD_NCS
#define	LCD_NCS_LOW()		GPIO_LCD->BSRRH = PIN_LCD_NCS

#define LCD_A0_HIGH()           GPIO_LCD->BSRRL = PIN_LCD_A0
#define LCD_A0_LOW()            GPIO_LCD->BSRRH = PIN_LCD_A0

#define LCD_RST_HIGH()		GPIO_LCD->BSRRL = PIN_LCD_RST
#define LCD_RST_LOW()		GPIO_LCD->BSRRH = PIN_LCD_RST

#define LCD_CLK_HIGH()		GPIO_LCD->BSRRL = PIN_LCD_CLK
#define LCD_CLK_LOW()		GPIO_LCD->BSRRH = PIN_LCD_CLK

#define LCD_MOSI_HIGH()		GPIO_LCD->BSRRL = PIN_LCD_MOSI
#define LCD_MOSI_LOW()		GPIO_LCD->BSRRH = PIN_LCD_MOSI

void AspiCmd(u8 Command_Byte);
void AspiData(u8 Para_data);

#endif

