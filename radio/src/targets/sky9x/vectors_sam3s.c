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

#define __VECTORS_SAM3S_C__

#include "debug.h"

/*=========================================================================*/
/*  DEFINE: All extern Data                                                */
/*=========================================================================*/
extern unsigned long _estack;

/*=========================================================================*/
/*  DEFINE: Prototypes                                                     */
/*=========================================================================*/
void ResetHandler(void);

void NMI_Handler (void) __attribute__((weak));
void HardFault_Handler (void) __attribute__((weak));
void MemManage_Handler (void) __attribute__((weak));
void BusFault_Handler (void) __attribute__((weak));
void UsageFault_Handler (void) __attribute__((weak));
void SVC_Handler (void) __attribute__((weak));
void DebugMon_Handler (void) __attribute__((weak));
void PendSV_Handler(void) __attribute__((weak));
void SysTick_Handler (void) __attribute__((weak));
void SUPC_IRQHandler (void) __attribute__((weak));
void RSTC_IRQHandler (void) __attribute__((weak));
void RTC_IRQHandler (void) __attribute__((weak));
void RTT_IRQHandler (void) __attribute__((weak));
void WDT_IRQHandler (void) __attribute__((weak));
void PMC_IRQHandler (void) __attribute__((weak));
void EEFC_IRQHandler (void) __attribute__((weak));
void UART0_IRQHandler (void) __attribute__((weak));
void UART1_IRQHandler (void) __attribute__((weak));
void SMC_IRQHandler (void) __attribute__((weak));
void PIOA_IRQHandler (void) __attribute__((weak));
void PIOB_IRQHandler (void) __attribute__((weak));
void PIOC_IRQHandler (void) __attribute__((weak));
void USART0_IRQHandler (void) __attribute__((weak));
void USART1_IRQHandler (void) __attribute__((weak));
void MCI_IRQHandler (void) __attribute__((weak));
void TWI0_IRQHandler (void) __attribute__((weak));
void TWI1_IRQHandler (void) __attribute__((weak));
void SPI_IRQHandler (void) __attribute__((weak));
void SSC_IRQHandler (void) __attribute__((weak));
void TC0_IRQHandler (void) __attribute__((weak));
void TC1_IRQHandler (void) __attribute__((weak));
void TC2_IRQHandler (void) __attribute__((weak));
void TC3_IRQHandler (void) __attribute__((weak));
void TC4_IRQHandler (void) __attribute__((weak));
void TC5_IRQHandler (void) __attribute__((weak));
void ADC_IRQHandler (void) __attribute__((weak));
void DAC_IRQHandler (void) __attribute__((weak));
void PWM_IRQHandler (void) __attribute__((weak));
void CRCCU_IRQHandler (void) __attribute__((weak));
void ACC_IRQHandler (void) __attribute__((weak));
void USBD_IrqHandler (void) __attribute__((weak));

/*=========================================================================*/
/*  DEFINE: All code exported                                              */
/*=========================================================================*/
/*
 * This is our vector table.
 */
__attribute__ ((section(".vectors"), used))
void (* const gVectors[])(void) = 
{
   (void (*)(void))((unsigned long)&_estack),
   ResetHandler,
   NMI_Handler,
   HardFault_Handler,
   MemManage_Handler,
   BusFault_Handler,
   UsageFault_Handler,
   0, 0, 0, 0,
   SVC_Handler,
   DebugMon_Handler,
   0,
   PendSV_Handler,
   SysTick_Handler,
   
   SUPC_IRQHandler,     /*  0  SUPPLY CONTROLLER */
   RSTC_IRQHandler,     /*  1  RESET CONTROLLER */
   RTC_IRQHandler,      /*  2  REAL TIME CLOCK */
   RTT_IRQHandler,      /*  3  REAL TIME TIMER */
   WDT_IRQHandler,      /*  4  WATCHDOG TIMER */
   PMC_IRQHandler,      /*  5  PMC */
   EEFC_IRQHandler,     /*  6  EEFC */
   0,                   /*  7  Reserved */
   UART0_IRQHandler,    /*  8  UART0 */
   UART1_IRQHandler,    /*  9  UART1 */
   SMC_IRQHandler,      /*  10 SMC */
   PIOA_IRQHandler,     /*  11 Parallel IO Controller A */
   PIOB_IRQHandler,     /*  12 Parallel IO Controller B */
   PIOC_IRQHandler,     /*  13 Parallel IO Controller C */
   USART0_IRQHandler,   /*  14 USART 0 */
   USART1_IRQHandler,   /*  15 USART 1 */
   0,                   /*  16 Reserved */
   0,                   /*  17 Reserved */
   MCI_IRQHandler,      /*  18 MCI */
   TWI0_IRQHandler,     /*  19 TWI 0 */
   TWI1_IRQHandler,     /*  20 TWI 1 */
   SPI_IRQHandler,      /*  21 SPI */
   SSC_IRQHandler,      /*  22 SSC */
   TC0_IRQHandler,      /*  23 Timer Counter 0 */
   TC1_IRQHandler,      /*  24 Timer Counter 1 */
   TC2_IRQHandler,      /*  25 Timer Counter 2 */
   TC3_IRQHandler,      /*  26 Timer Counter 3 */
   TC4_IRQHandler,      /*  27 Timer Counter 4 */
   TC5_IRQHandler,      /*  28 Timer Counter 5 */
   ADC_IRQHandler,      /*  29 ADC controller */
   DAC_IRQHandler,      /*  30 DAC controller */
   PWM_IRQHandler,      /*  31 PWM */
   CRCCU_IRQHandler,    /*  32 CRC Calculation Unit */
   ACC_IRQHandler,      /*  33 Analog Comparator */
   USBD_IrqHandler,     /*  34 USB Device Port */
   0                    /*  35 not used */
}; /* gVectors */

/*
 * And here are the weak interrupt handlers.
 */
void NMI_Handler (void) { while(1); }
void HardFault_Handler (void) { while(1); }
void MemManage_Handler (void) { while(1); }
void BusFault_Handler (void) { while(1); }
void UsageFault_Handler (void) { while(1); }
void SVC_Handler (void) { while(1); }
void DebugMon_Handler (void) { while(1); }
void SUPC_IRQHandler (void) { while(1); }
void RSTC_IRQHandler (void) { while(1); }
void RTC_IRQHandler (void) { while(1); }
void RTT_IRQHandler (void) { while(1); }
void WDT_IRQHandler (void) { while(1); }
void PMC_IRQHandler (void) { while(1); }
void EEFC_IRQHandler (void) { while(1); }
void UART1_IRQHandler (void) { while(1); }
void SMC_IRQHandler (void) { while(1); }
void PIOA_IRQHandler (void) { while(1); }
void PIOB_IRQHandler (void) { while(1); }
void PIOC_IRQHandler (void) { while(1); }
void USART0_IRQHandler (void) { while(1); }
void USART1_IRQHandler (void) { while(1); }
void MCI_IRQHandler (void) { while(1); }
void TWI1_IRQHandler (void) { while(1); }
void SSC_IRQHandler (void) { while(1); }
void TC0_IRQHandler (void) { while(1); }
void TC1_IRQHandler (void) { while(1); }
void TC4_IRQHandler (void) { while(1); }
void TC5_IRQHandler (void) { while(1); }
void ADC_IRQHandler (void) { while(1); }
void CRCCU_IRQHandler (void) { while(1); }
void ACC_IRQHandler (void) { while(1); }

/*** EOF ***/
