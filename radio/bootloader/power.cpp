/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
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
 */

#include <stdint.h>
#ifdef PCBSKY 
#include "AT91SAM3S4.h"
#endif


#ifdef PCBX9D
#include "stm32f2xx_gpio.h"
#include "stm32f2xx_rcc.h"
#include "hal.h"
#endif

#include "radio.h"
#include "logicio.h"

#ifdef PCBSKY 


void init_soft_power()
{
	// Configure RF_power (PC17)
	configure_pins( PIO_PC17, PIN_ENABLE | PIN_INPUT | PIN_PORTC | PIN_NO_PULLUP | PIN_PULLDOWN ) ;
	
	configure_pins( PIO_PA8, PIN_ENABLE | PIN_INPUT | PIN_PORTA | PIN_PULLUP ) ;
}


// Returns zero if power is switched off
//  1 if power switch is on
//  2 if power switch off, trainer power on
uint32_t check_soft_power()
{
	if ( PIOC->PIO_PDSR & PIO_PC17 )		// Power on
	{
		return POWER_ON ;
	}
	return POWER_OFF ;	
}


// turn off soft power
void soft_power_off()
{
	configure_pins( PIO_PA8, PIN_ENABLE | PIN_OUTPUT | PIN_LOW | PIN_PORTA | PIN_NO_PULLUP ) ;
}

#endif

#ifdef PCBX9D

void soft_power_off()
{
  GPIO_ResetBits(GPIOPWR,PIN_MCU_PWR);
}

uint32_t check_soft_power()
{
  if (GPIO_ReadInputDataBit(GPIOPWR, PIN_PWR_STATUS) == Bit_RESET)
    return POWER_ON;
  else
    return POWER_OFF;
}

void init_soft_power()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN ; 		// Enable portD clock
	GPIO_ResetBits(GPIOPWR, PIN_INT_RF_PWR | PIN_EXT_RF_PWR);

  /* GPIO  Configuration*/
	configure_pins( PIN_INT_RF_PWR | PIN_EXT_RF_PWR | PIN_MCU_PWR, PIN_OUTPUT | PIN_PUSHPULL | PIN_OS25 | PIN_PORTD ) ;

	configure_pins( PIN_PWR_STATUS, PIN_INPUT | PIN_PULLUP | PIN_PORTD ) ;
	
	configure_pins( PIN_TRNDET, PIN_INPUT | PIN_PULLUP | PIN_PORTA ) ;
  
  // Soft power ON
	
	GPIO_SetBits(GPIOPWR,PIN_MCU_PWR);

}

#endif

