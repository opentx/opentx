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

#endif // _ASPI_H_

