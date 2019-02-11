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
#include "opentx.h"

enum ENUM_TOUCH_STATE
{
    TOUCH_NONE,
    TOUCH_SLIDE,
    TOUCH_CLICK
};

#define TOUCH_DELAY() TouchDelay()

void GPIO_SetDirection( GPIO_TypeDef *GPIOx, unsigned char Pin, unsigned char IsInput )
{
  unsigned int Mask;
  unsigned int Position;
  unsigned int Register;


  Position = Pin << 1;
  Mask = ~( 0x03UL << Position );

  //EnterCritical();
  Register = GPIOx->OSPEEDR & Mask;
  Register |= IO_SPEED_HIGH << Position;
  GPIOx->OSPEEDR = Register;
  //ExitCritical();

  //EnterCritical();
  Register = GPIOx->MODER & Mask;
  if( !IsInput )
  {
      Register |= IO_MODE_OUTPUT << Position;
  }

  GPIOx->MODER = Register;
  //ExitCritical();
}

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
void TouchInit( void )
{
  SET_TOUCH_RESET_OUT();
  CLR_TOUCH_RESET();
  SET_TOUCH_SCL_OUT();
  SET_TOUCH_SDA_OUT();
  SET_TOUCH_SCL();
  SET_TOUCH_SDA();
  SET_TOUCH_INT_IN();
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
static void TouchConditionDelay( void )
{
  volatile unsigned int i;

  for( i = 0; i < 40; i++ )
  {
      ;
  }
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
static void TouchDelay( void )
{
  volatile unsigned int i;

  for( i = 0; i < 3; i++ )
  {
      ;
  }
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
static void TouchStart( void )
{
  SET_TOUCH_SCL_OUT();
  SET_TOUCH_SDA_OUT();
  SET_TOUCH_SCL();
  SET_TOUCH_SDA();
  TouchConditionDelay();
  CLR_TOUCH_SDA();
  TouchConditionDelay();
  CLR_TOUCH_SCL();
  TOUCH_DELAY();
}

/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
static void TouchStop( void )
{
  CLR_TOUCH_SCL();
  TOUCH_DELAY();
  CLR_TOUCH_SDA();
  SET_TOUCH_SCL();
  TouchConditionDelay();
  SET_TOUCH_SDA();
  TouchConditionDelay();
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
static void  SendAckToTouch( unsigned char Ack )
{
  if( Ack )
  {
      SET_TOUCH_SDA();
  }
  else
  {
      CLR_TOUCH_SDA();
  }

  SET_TOUCH_SCL();
  TOUCH_DELAY();
  CLR_TOUCH_SCL();
  TOUCH_DELAY();
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
static unsigned char GetTouchAck( void )
{
  unsigned char i;

  SET_TOUCH_SDA();
  SET_TOUCH_SDA_IN();
  SET_TOUCH_SCL();
  TOUCH_DELAY();

  if( READ_TOUCH_SDA() )
  {
      i = 1;
  }
  else
  {
      i = 0;
  }

  CLR_TOUCH_SCL();
  SET_TOUCH_SDA_OUT();
  return( i );
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
unsigned char  ReceiveByteFromTouch( void )
{
  unsigned char i;
  unsigned char ucReceive = 0;

  SET_TOUCH_SDA_IN();

  for( i = 0; i < 8; i++ )
  {
      CLR_TOUCH_SCL();
      TOUCH_DELAY();
      ucReceive <<= 1;
      SET_TOUCH_SCL();
      TOUCH_DELAY();

      if( READ_TOUCH_SDA() )
      {
          ucReceive |= 0x01;
      }
  }

  CLR_TOUCH_SCL();
  SET_TOUCH_SDA_OUT();
  TOUCH_DELAY();
  return( ucReceive );
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
static void SendByteToTouch( unsigned char ucSendData )
{
  unsigned char i;

  for( i = 0; i < 8; i++ )
  {
      CLR_TOUCH_SCL();
      TOUCH_DELAY();
      if( ucSendData & 0x80 )
      {
          SET_TOUCH_SDA();
      }
      else
      {
          CLR_TOUCH_SDA();
      }

      SET_TOUCH_SCL();
      ucSendData <<= 1;
      TOUCH_DELAY();
  }

  CLR_TOUCH_SCL();
  TOUCH_DELAY();
}
/*==================================================================================================
Name:
Function:
Input:
Output:
==================================================================================================*/
unsigned char ReadTouchData( unsigned char *pData, unsigned char Length )
{
  unsigned char ucTryCount;
#if 1
  ucTryCount = 0;
  while( ucTryCount < 3 )
  {
      do
      {
          TouchStart();
          SendByteToTouch( ( TOUCH_PANEL_ADDRESS << 1 )|0x00 );
          if( GetTouchAck() )
          {
              break;
          }

          SendByteToTouch( 0x00 );
          if( GetTouchAck() )
          {
              break;
          }

          TouchStart();
          SendByteToTouch( ( TOUCH_PANEL_ADDRESS << 1 )|0x01 );
          if( GetTouchAck() )
          {
              break;
          }

          while( Length )
          {
              *pData = ReceiveByteFromTouch();
              Length--;
              pData++;

              if( 0 != Length )
              {
                  SendAckToTouch( 0 );
              }
              else
              {
                  SendAckToTouch( 1 );
              }
          }

          TouchStop();
          return( !0 );

      }while( 0 );

      ucTryCount++;
      TouchStop();
  }
#endif
  return( 0 );
}


signed int x;
signed int y;
unsigned char  TouchEvent;
unsigned char  TouchBuffer[7];
unsigned short Tx;
unsigned short Ty;
extern STRUCT_TOUCH touchState; //definition in mainwindow.cpp
static unsigned char  TouchCycle = 0;
static unsigned char  TouchState = TOUCH_NONE;
static unsigned short Txold;
static unsigned short Tyold;
static unsigned short TouchStartX;
static unsigned short TouchStartY;

void TouchDriver( void )
{
  SET_TOUCH_RESET();

  TouchCycle++;

  if( TouchCycle >= 1 )
  {
      TouchCycle = 0;
      if( ReadTouchData( TouchBuffer, sizeof( TouchBuffer ) ) )
      {
          TouchEvent = TouchBuffer[3] >> 6;
          Tx = TouchBuffer[3] & 0x0f;
          Tx <<= 8;
          Tx |= TouchBuffer[4];
          Ty = TouchBuffer[5] & 0x0f;
          Ty <<= 8;
          Ty |= TouchBuffer[6];

          #if defined( LCD_DIRECTION ) && ( LCD_DIRECTION == LCD_VERTICAL )
          Tx=LCD_WIDTH-Tx;
          Ty=LCD_HEIGHT-Ty;
          #else
          x = ( LCD_WIDTH - 1 ) - Ty;
          Ty = Tx;
          Tx = x;
          #endif
          if( 2 == TouchEvent )
          {
              touchState.X = Tx;
              touchState.Y = Ty;

              if( TOUCH_NONE == TouchState )
              {
                  touchState.Event = TE_NONE;
                  TouchState = TOUCH_CLICK;

                  TouchStartX = Tx;
                  TouchStartY = Ty;

                  if (g_eeGeneral.backlightMode & e_backlight_mode_keys)
                    backlightOn(); // TODO is that the best place ?
              }
              else
              {

                  if ( TOUCH_CLICK == TouchState )
                  {
                    x = Tx - Txold;
                    y = Ty - Tyold;

                    if( ( x >= SLIDE_RANGE ) || ( x <= -SLIDE_RANGE ) || ( y >= SLIDE_RANGE ) || ( y <= -SLIDE_RANGE ) )
                    {
                        TouchState = TOUCH_SLIDE;
                        touchState.Event = TE_SLIDE;
                    }
                    else
                    {
                        touchState.Event = TE_DOWN;
                        touchState.startX = touchState.lastX = touchState.X;
                        touchState.startY = touchState.lastY = touchState.Y;
                    }
                  }
                  else if ( TOUCH_SLIDE == TouchState )
                  {
                      touchState.Event = TE_SLIDE;
                  }
                  else
                  {
                      TouchState = TOUCH_NONE;
                  }
              }
              Txold = Tx;
              Tyold = Ty;
          }
          else
          {
              if ( TOUCH_CLICK == TouchState )
              {
                  touchState.X = Txold;
                  touchState.Y = Tyold;
                  touchState.Event = TE_UP;
                  touchState.Time = get_tmr10ms();
              }
              else if (get_tmr10ms() > touchState.Time + 50)
              {
                  touchState.X = LCD_WIDTH;
                  touchState.Y = LCD_HEIGHT;
                  touchState.Event = TE_NONE;
              }

              TouchState = TOUCH_NONE;
          }
      }
      else
      {
          touchState.X = LCD_WIDTH;
          touchState.Y = LCD_HEIGHT;
          touchState.Event = TE_NONE;
          TouchState = TOUCH_NONE;
      }
  }
}
