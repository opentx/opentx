/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/**
 * \file
 *
 * \par Purpose
 *
 * Provides the low-level initialization function that gets called on chip startup.
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "board.h"

/*----------------------------------------------------------------------------
 *        Local definitions
 *----------------------------------------------------------------------------*/
/** Define clock timeout */
#define CLOCK_TIMEOUT         5000
/** Bit 29 must always be set to 1 when programming the CKGR_PLLAR register.*/
#define PMC_PLLA_SET_BIT29    (0x1 << 29)
/** Specifies the number of Slow Clock cycles x8 before the LOCKA bit is set in PMC_SR after CKGR_PLLAR is written.*/
#define PMC_PLLA_COUNT        (0xBF << 8)
/** The PLLA Clock frequency is the PLLA input frequency multiplied by 8*/
#define PMC_PLLA_MULTIPIER    (0x07 << 16)
/** PLLA Divider is bypassed.*/
#define PMC_PLLA_DIV          (0x01)

void revert_osc( void ) ;


/*----------------------------------------------------------------------------
 *        Local functions
 * returns master_frequency
 *----------------------------------------------------------------------------*/

// *** WARNING, this is called BEFORE .data and .bss have been initialised
// *** GLOBAL variables are NOT useable

static uint32_t BOARD_ConfigurePmc(void)
{
/* TODO: configurate PLL in new way */
//#if 0
//    uint32_t timeout = 0;
//    /** 1: Checking the Main Oscillator Frequency (Optional) and Initialize main oscillator */
//    /** 1.1 Initialize main oscillator */
//    // TODO:

//    /** 2. Setting PLL and Divider*/
//    /** 2.1 Initialize PLLA*/
//    PMC->CKGR_PLLAR = ( PMC_PLLA_SET_BIT29 |
//                        PMC_PLLA_MULTIPIER |
//                        PMC_PLLA_COUNT |
//                        PMC_PLLA_DIV
//                       );
//    timeout = 0;
//    /** 2.2 Wait for the LOCKA bit to be set by polling the status register.*/
//    while (!(PMC->PMC_SR & PMC_SR_LOCKA) && (timeout++ < CLOCK_TIMEOUT));
//    /** 2.3 wait for the MCKRDY bit to be set by polling the status register*/
//    timeout = 0;
//    while (!(PMC->PMC_SR & PMC_SR_MCKRDY) && (timeout++ < CLOCK_TIMEOUT));

//    /** 3. Selection of Master Clock and Processor Clock*/
//    PMC->PMC_MCKR |= PMC_MCKR_PRES_CLK_2;

//    /** 3.1 wait for the MCKRDY bit to be set by polling the status register*/
//    timeout = 0;
//    while (!(PMC->PMC_SR & PMC_SR_MCKRDY) && (timeout++ < CLOCK_TIMEOUT));

//    /** 3.2 PLLA Clock is selected */
//    PMC->PMC_MCKR = PMC_MCKR_PRES_CLK_2 | PMC_MCKR_CSS_PLLA_CLK;

//    /** 3.3 wait for the MCKRDY bit to be set by polling the status register*/
//    timeout = 0;
//    while (!(PMC->PMC_SR & PMC_SR_MCKRDY) && (timeout++ < CLOCK_TIMEOUT));
//#endif

#if 1 // old as sam3u
#define AT91C_CKGR_MUL_SHIFT         16
//#define AT91C_CKGR_OUT_SHIFT         14
#define AT91C_CKGR_PLLCOUNT_SHIFT     8
#define AT91C_CKGR_DIV_SHIFT          0

// Settings at 72MHz/2 = 36MHz
#define BOARD_OSCOUNT         (CKGR_MOR_MOSCXTST & (0x8 << 8))
#define BOARD_PLLR ((1 << 29) | (0x5 << AT91C_CKGR_MUL_SHIFT) \
         | (0x1 << AT91C_CKGR_PLLCOUNT_SHIFT) | (0x2 << AT91C_CKGR_DIV_SHIFT))
#define BOARD_MCKR (PMC_MCKR_CSS_PLLA_CLK)

// Define clock timeout
#undef CLOCK_TIMEOUT
#define CLOCK_TIMEOUT           0xFFFFFFFF

    register uint32_t timeout = 0 ;
		register Pmc *pmcptr ;

		pmcptr = PMC ;

    /* Enable NRST reset
     ************************************/
    //AT91C_BASE_RSTC->RSTC_RMR |= AT91C_RSTC_URSTEN;

    /* Select external slow clock
     ****************************/


//    if ((SUPC->SUPC_SR & SUPC_SR_OSCSEL) != SUPC_SR_OSCSEL_CRYST) {
//        SUPC->SUPC_CR = SUPC_CR_XTALSEL_CRYSTAL_SEL | ((uint32_t)0xA5 << 24);
//        timeout = 0;
//        while (!(SUPC->SUPC_SR & SUPC_SR_OSCSEL_CRYST) );
//    }


    /* Initialize main oscillator
     ****************************/
    if(!(pmcptr->CKGR_MOR & CKGR_MOR_MOSCSEL))
    {

      pmcptr->CKGR_MOR = (0x37 << 16) | BOARD_OSCOUNT | CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTEN;
      timeout = 0;
      while (!(pmcptr->PMC_SR & PMC_SR_MOSCXTS) && (++timeout < CLOCK_TIMEOUT));

    }

    /* Switch to 3-20MHz Xtal oscillator */
    pmcptr->CKGR_MOR = (0x37 << 16) | BOARD_OSCOUNT | CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTEN | CKGR_MOR_MOSCSEL;
    timeout = 0;
    while (!(pmcptr->PMC_SR & PMC_SR_MOSCSELS) && (++timeout < CLOCK_TIMEOUT));

    pmcptr->PMC_MCKR = (PMC->PMC_MCKR & ~(uint32_t)PMC_MCKR_CSS) | PMC_MCKR_CSS_MAIN_CLK;
    timeout = 0;
    while (!(pmcptr->PMC_SR & PMC_SR_MCKRDY) && (++timeout < CLOCK_TIMEOUT));

    /* Initialize PLLA 72 MHz */
    pmcptr->CKGR_PLLAR = BOARD_PLLR;
    timeout = 0;
    while (!(pmcptr->PMC_SR & PMC_SR_LOCKA) && (++timeout < CLOCK_TIMEOUT));


    /* Initialize UTMI for USB usage, can be disabled if not using USB for the sake of saving power*/
    //AT91C_BASE_CKGR->CKGR_UCKR |= (AT91C_CKGR_UPLLCOUNT & (3 << 20)) | AT91C_CKGR_UPLLEN;
    //timeout = 0;
    //while (!(PMC->PMC_SR & AT91C_PMC_LOCKU) && (timeout++ < CLOCK_TIMEOUT));

    /* Switch to fast clock
     **********************/
//    PMC->PMC_MCKR = (BOARD_MCKR & ~PMC_MCKR_CSS) | PMC_MCKR_CSS_MAIN_CLK;
//    timeout = 0;
//    while (!(PMC->PMC_SR & PMC_SR_MCKRDY) && (timeout++ < CLOCK_TIMEOUT));

		// Switch to PLLA as main clock 36MHz
  PMC->PMC_MCKR = BOARD_MCKR;
  timeout = 0;
  while (!(PMC->PMC_SR & PMC_SR_MCKRDY) && (timeout++ < CLOCK_TIMEOUT));

#endif
	return 36000000L ;		// Master_frequency
}

void revert_osc()
{
  register uint32_t timeout = 0 ;
	register Pmc *pmcptr ;

	pmcptr = PMC ;


	// Switch back to the internal oscillator
  pmcptr->CKGR_MOR = (0x37 << 16) | BOARD_OSCOUNT | CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCXTEN ;
  timeout = 0;
  while (!(pmcptr->PMC_SR & PMC_SR_MOSCSELS) && (++timeout < CLOCK_TIMEOUT));

  pmcptr->PMC_MCKR = (PMC->PMC_MCKR & ~(uint32_t)PMC_MCKR_CSS) | PMC_MCKR_CSS_SLOW_CLK;
  timeout = 0;
  while (!(pmcptr->PMC_SR & PMC_SR_MCKRDY) && (++timeout < CLOCK_TIMEOUT));

    
	pmcptr->CKGR_PLLAR = 1 << 29 ;		// Stop PLLA

}

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/**
 * \brief Performs the low-level initialization of the chip. This includes EFC,
 * master clock and watchdog configuration.
 */
/*----------------------------------------------------------------------------*/
uint32_t SystemInit (void)
{
    /** Set 2 cycle (1 WS) for Embedded Flash Access */
		// Max clock is 38 MHz (1.8V VVDCORE)
   EFC->EEFC_FMR = (1 << 8) ;

   /** Configure PMC */
  return BOARD_ConfigurePmc();
}
