/**
  ******************************************************************************
  * @file    Project/spi/spi.h
  * @author  FrSky Application Team
  * @Hardware version V0.2
  * @date    11-July-2012
  * @brief   spi.c' Header file.
  * *
  ******************************************************************************
*/
#ifndef _ASPI_H_
#define _ASPI_H_

#if defined(__cplusplus) && !defined(SIMU)
extern "C" {
#endif

#define __no_operation     __NOP

#define LCD_NCS_HIGH()    LCD_GPIO_NCS->BSRRL = LCD_GPIO_PIN_NCS
#define LCD_NCS_LOW()     LCD_GPIO_NCS->BSRRH = LCD_GPIO_PIN_NCS

#define LCD_A0_HIGH()     LCD_GPIO_SPI->BSRRL = LCD_GPIO_PIN_A0
#define LCD_A0_LOW()      LCD_GPIO_SPI->BSRRH = LCD_GPIO_PIN_A0

#define LCD_RST_HIGH()    LCD_GPIO_RST->BSRRL = LCD_GPIO_PIN_RST
#define LCD_RST_LOW()     LCD_GPIO_RST->BSRRH = LCD_GPIO_PIN_RST

#define LCD_CLK_HIGH()    LCD_GPIO_SPI->BSRRL = LCD_GPIO_PIN_CLK
#define LCD_CLK_LOW()     LCD_GPIO_SPI->BSRRH = LCD_GPIO_PIN_CLK

#define LCD_MOSI_HIGH()   LCD_GPIO_SPI->BSRRL = LCD_GPIO_PIN_MOSI
#define LCD_MOSI_LOW()    LCD_GPIO_SPI->BSRRH = LCD_GPIO_PIN_MOSI

void AspiCmd(u8 Command_Byte);
void AspiData(u8 Para_data);

#if defined(__cplusplus) && !defined(SIMU)
}
#endif

#endif

