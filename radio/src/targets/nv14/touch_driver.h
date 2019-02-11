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
#ifndef __TOUCH_DRIVER_H__
#define __TOUCH_DRIVER_H__

#include "board.h"
#include "touch.h"

#define PORT_TOUCH_SCL              ( GPIOB )
#define PIN_TOUCH_SCL               ( GPIO_Pin_8 )
#define PORT_TOUCH_SDA              ( GPIOB )
#define PIN_TOUCH_SDA               ( GPIO_Pin_7 )
#define PORT_TOUCH_RESET            ( GPIOB )
#define PIN_TOUCH_RESET             ( GPIO_Pin_12 )

#define PORT_TOUCH_INT              ( GPIOB )
#define PIN_TOUCH_INT               ( GPIO_Pin_9 )

#define SET_TOUCH_INT_IN()          GPIO_SetDirection( PORT_TOUCH_INT, 9, 1 )

#define SET_TOUCH_RESET_OUT()       GPIO_SetDirection( PORT_TOUCH_RESET, 12, 0 )
#define SET_TOUCH_RESET()           GPIO_WriteBit( PORT_TOUCH_RESET,PIN_TOUCH_RESET, Bit_SET )
#define CLR_TOUCH_RESET()           GPIO_WriteBit( PORT_TOUCH_RESET,PIN_TOUCH_RESET, Bit_RESET )

#define SET_TOUCH_SCL_OUT()         GPIO_SetDirection( PORT_TOUCH_SCL, 8, 0 )
#define SET_TOUCH_SCL()             GPIO_WriteBit( PORT_TOUCH_SCL,PIN_TOUCH_SCL, Bit_SET )
#define CLR_TOUCH_SCL()             GPIO_WriteBit( PORT_TOUCH_SCL,PIN_TOUCH_SCL, Bit_RESET )

#define SET_TOUCH_SDA_IN()          GPIO_SetDirection( PORT_TOUCH_SDA, 7, 1 )
#define SET_TOUCH_SDA_OUT()         GPIO_SetDirection( PORT_TOUCH_SDA, 7, 0 )

#define SET_TOUCH_SDA()             GPIO_WriteBit(PORT_TOUCH_SDA, PIN_TOUCH_SDA, Bit_SET)
#define CLR_TOUCH_SDA()             GPIO_WriteBit(PORT_TOUCH_SDA, PIN_TOUCH_SDA, Bit_RESET)

#define READ_TOUCH_SDA()            GPIO_ReadInputDataBit( PORT_TOUCH_SDA, PIN_TOUCH_SDA )

#define TOUCH_PANEL_ADDRESS         ( 0x38 )

#define LCD_VERTICAL                ( 0x00 )
#define LCD_HORIZONTAL              ( 0x01 )
#define LCD_DIRECTION               ( LCD_VERTICAL )

#define LCD_WIDTH                         ( 320 )
#define LCD_HEIGHT                        ( 480 )

#define TOUCH_INVALID               ( 250 )

enum ENUM_IO_SPEED
{
    IO_SPEED_LOW,
    IO_SPEED_MID,
    IO_SPEED_QUICK,
    IO_SPEED_HIGH
};

enum ENUM_IO_MODE
{
    IO_MODE_INPUT,
    IO_MODE_OUTPUT,
    IO_MODE_ALTERNATE,
    IO_MODE_ANALOG
};

#define GET_ARRAY_SIZE( ARRAY )              ( ( sizeof( ARRAY ) ) / ( sizeof( ARRAY[0] ) ) )

extern void TouchInit( void );
extern void TouchDriver( void );

#endif /*__TOUCH_DRIVER_H__*/
