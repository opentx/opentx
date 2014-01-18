/****************************************************************************
*  Copyright (c) 2012 by Michael Blandford. All rights reserved.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*
****************************************************************************/

#ifdef PCBX9D
#define PIN_MODE_MASK		0x0003
#define PIN_INPUT				0x0000
#define PIN_OUTPUT			0x0001
#define PIN_PERIPHERAL	0x0002
#define PIN_ANALOG			0x0003
#define PIN_PULL_MASK		0x000C
#define PIN_PULLUP			0x0004
#define PIN_NO_PULLUP		0x0000
#define PIN_PULLDOWN		0x0008
#define PIN_NO_PULLDOWN	0x0000
#define PIN_NO_PULL			0x0000
#define PIN_PERI_MASK		0x00F0
//#define PIN_PERI_MASK_H	0x0020
#define PIN_PER_0				0x0000
#define PIN_PER_1				0x0010
#define PIN_PER_2				0x0020
#define PIN_PER_3				0x0030
#define PIN_PER_4				0x0040
#define PIN_PER_5				0x0050
#define PIN_PER_6				0x0060
#define PIN_PER_7				0x0070
#define PIN_PER_8				0x0080
#define PIN_PER_9				0x0090
#define PIN_PER_10			0x00A0
#define PIN_PER_11			0x00B0
#define PIN_PER_12			0x00C0
#define PIN_PER_13			0x00D0
#define PIN_PER_14			0x00E0
#define PIN_PER_15			0x00F0
#define PIN_PORT_MASK		0x0700
#define PIN_PORTA				0x0000
#define PIN_PORTB				0x0100
#define PIN_PORTC				0x0200
#define PIN_PORTD				0x0300
#define PIN_PORTE				0x0400
#define PIN_LOW					0x0000
#define PIN_HIGH				0x1000
#define PIN_SPEED_MASK	0x6000
#define	PIN_OS2					0x0000
#define	PIN_OS25				0x2000
#define	PIN_OS50				0x4000
#define	PIN_OS100				0x6000
#define	PIN_PUSHPULL		0x0000
#define	PIN_ODRAIN			0x8000

#define GPIO_Pin_0                 ((uint16_t)0x0001)  /* Pin 0 selected */
#define GPIO_Pin_1                 ((uint16_t)0x0002)  /* Pin 1 selected */
#define GPIO_Pin_2                 ((uint16_t)0x0004)  /* Pin 2 selected */
#define GPIO_Pin_3                 ((uint16_t)0x0008)  /* Pin 3 selected */
#define GPIO_Pin_4                 ((uint16_t)0x0010)  /* Pin 4 selected */
#define GPIO_Pin_5                 ((uint16_t)0x0020)  /* Pin 5 selected */
#define GPIO_Pin_6                 ((uint16_t)0x0040)  /* Pin 6 selected */
#define GPIO_Pin_7                 ((uint16_t)0x0080)  /* Pin 7 selected */
#define GPIO_Pin_8                 ((uint16_t)0x0100)  /* Pin 8 selected */
#define GPIO_Pin_9                 ((uint16_t)0x0200)  /* Pin 9 selected */
#define GPIO_Pin_10                ((uint16_t)0x0400)  /* Pin 10 selected */
#define GPIO_Pin_11                ((uint16_t)0x0800)  /* Pin 11 selected */
#define GPIO_Pin_12                ((uint16_t)0x1000)  /* Pin 12 selected */
#define GPIO_Pin_13                ((uint16_t)0x2000)  /* Pin 13 selected */
#define GPIO_Pin_14                ((uint16_t)0x4000)  /* Pin 14 selected */
#define GPIO_Pin_15                ((uint16_t)0x8000)  /* Pin 15 selected */
#define GPIO_Pin_All               ((uint16_t)0xFFFF)  /* All pins selected */

#endif


#ifdef PCBSKY
#define PIN_ENABLE			0x001
#define PIN_PERIPHERAL	0x000
#define PIN_INPUT				0x002
#define PIN_OUTPUT			0x000
#define PIN_PULLUP			0x004
#define PIN_NO_PULLUP		0x000
#define PIN_PULLDOWN		0x008
#define PIN_NO_PULLDOWN	0x000
#define PIN_PERI_MASK_L	0x010
#define PIN_PERI_MASK_H	0x020
#define PIN_PER_A				0x000
#define PIN_PER_B				0x010
#define PIN_PER_C				0x020
#define PIN_PER_D				0x030
#define PIN_PORT_MASK		0x0C0
#define PIN_PORTA				0x000
#define PIN_PORTB				0x040
#define PIN_PORTC				0x080
#define PIN_LOW					0x000
#define PIN_HIGH				0x100
#endif


extern void configure_pins( uint32_t pins, uint16_t config ) ;
extern void init_keys( void ) ;
extern uint32_t read_keys( void ) ;
extern uint32_t read_trims( void ) ;
extern uint32_t keyState( enum EnumKeys enuk) ;
extern void init_trims( void ) ;
extern void setup_switches( void ) ;
extern void config_free_pins( void ) ;

#define GPIO_ResetBits( port, bits ) (port->BSRRH = bits)
#define GPIO_SetBits( port, bits ) (port->BSRRL = bits)
#define GPIO_ReadInputDataBit( port, bit) ( (port->IDR & bit) ? Bit_SET : Bit_RESET)

#ifdef PCBX9D
uint32_t switchPosition( uint32_t swtch ) ;
#endif

