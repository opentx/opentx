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

#define __CRT_C__

#include <stdint.h>

/*=========================================================================*/
/*  DEFINE: All extern Data                                                */
/*=========================================================================*/
/*
 * The next data are defined by the linker script.
 */
extern unsigned long _stext;
extern unsigned long _etext;
extern unsigned long _sdata;
extern unsigned long _edata;
extern unsigned long _sbss;
extern unsigned long _ebss;
extern unsigned long _estack;

/* This is the main */
extern int main (void);

/*=========================================================================*/
/*  DEFINE: Prototypes                                                     */
/*=========================================================================*/
extern uint32_t SystemInit (void) ; // __attribute__((weak));

/*=========================================================================*/
/*  DEFINE: All code exported                                              */
/*=========================================================================*/

/***************************************************************************/
/*  SystemInit                                                             */
/*                                                                         */
/*  SystemInit is a function which is provided by the CMSIS interface.     */
/*  If this function is not available, we need a function here to prevent  */
/*  an error of the linker. Therefore this function is declared as weak.   */
/***************************************************************************/
//uint32_t SystemInit (void)
//{
	
//} /* SystemInit */

extern uint32_t Master_frequency ;

/***************************************************************************/
/*  ResetHandler                                                           */
/*                                                                         */
/*  This function is used for the C runtime initialisation,                */
/*  for handling the .data and .bss segments.                              */
/***************************************************************************/
void ResetHandler (void)
{
  {
  	uint32_t *pSrc;
  	uint32_t *pDest;
  	uint32_t m_freq ;
   
  	 /*
  	  * Call the SystemInit code from CMSIS interface if available.
  	  * SystemInit is a week function which can be override
  	  * by an external function.
  	  */
  	m_freq = SystemInit();    
   
  	/*
  	 * Set the "Vector Table Offset Register". From the ARM
  	 * documentation, we got the following information:
  	 *
  	 * Use the Vector Table Offset Register to determine:
  	 *  - if the vector table is in RAM or code memory
  	 *  - the vector table offset.    
  	 */
  	*((uint32_t*)0xE000ED08) = (uint32_t)&_stext;
   
  	/*
  	 * Copy the initialized data of the ".data" segment
  	 * from the flash to the are in the ram.
  	 */
  	pSrc  = &_etext;
  	pDest = &_sdata;
  	 while(pDest < &_edata)
  	{
  	  *pDest++ = *pSrc++;
  	}
   
  	 /*
  	  * Clear the ".bss" segment.
  	  */
  	pDest = &_sbss;
  	while(pDest < &_ebss)
  	{
  	  *pDest++ = 0;
  	}
  	Master_frequency = m_freq ;
  }
   
   /*
    * And now the main function can be called.
    * Scotty, energie...
    */       
  main();    
   
   /*
    * In case there are problems with the
    * "warp drive", stop here.
    */
  while(1) {};    

} /* ResetHandler */

/*** EOF ***/
