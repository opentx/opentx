/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011, Atmel Corporation
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


/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include "board.h"
#include "board_lowlevel.h"
typedef void( *IntFunc )( void );
/*----------------------------------------------------------------------------
 *        Prototypes
 *----------------------------------------------------------------------------*/
/** Stack top */
extern unsigned long _sstack;

/** Initialize segments*/
extern unsigned int _sidata;
extern unsigned int _sdata, _edata;
extern unsigned int _szero, _ezero;

/** Default link entry */
extern int main(int argc, char **argv);

/** Reset entry */
void ResetException(void) ;//__attribute__((__interrupt__));

/** Exception Table */
__attribute__((section(".isr_vector")))
//IntFunc exception_table[] = {
//__attribute__ ((section(".vectors"), used))
void (* const gVectors[])(void) =
{ 
     /* Configure Initial Stack Pointer, using linker-generated symbols */
    (void (*)(void))((unsigned long)&_sstack),
//    /*(IntFunc)&*/_sstack,
    ResetException,  /* Initial PC, set to entry point  */
    (unsigned int) 0 /*NMIException*/,
    (unsigned int) 0 /*HardFaultException*/,
    (unsigned int) 0 /*MemManageException*/,
    (unsigned int) 0 /*BusFaultException*/,
    (unsigned int) 0 /*UsageFaultException*/,
    0, 0, 0, 0,             /* Reserved */
    (unsigned int) 0 /*SVCHandler*/,
    (unsigned int) 0 /*DebugMonitor*/,
    0,                      /* Reserved */
    (unsigned int) 0 /*PendSVC*/,
    (unsigned int) 0 /*SysTickHandler*/,
    /*
    :
    */
};

/** Applet Mailbox and initialization flag */
#define MAILBOX_SIZE 32
__attribute__((section(".mailbox")))
unsigned int mailbox[MAILBOX_SIZE];
volatile unsigned int isInitialized = 0;

/*----------------------------------------------------------------------------
 *         Reset Handler
 *----------------------------------------------------------------------------*/

/**
 * \brief  This is the code that gets called on processor reset. To initialize the
 * device. And call the main() routine.
 *
 */
__attribute__ ((section(".text"), used))
void ResetException(void)
{
    unsigned int *pSrc, *pDest;
//    { asm volatile ("cpsid i"); }

	// Initialize data
    // Zero fill bss
    if (isInitialized == 0) {
        pSrc = &_sidata;
        for(pDest = &_sdata; pDest < &_edata;) {

            *(pDest ++) = *(pSrc ++);
        }

        for (pDest = &_szero; pDest < &_ezero;) {

            *(pDest ++) = 0;
        }
        isInitialized = 1;
    }

    /* LowLevelInit(); */

    /* Save original SP. */
    __asm("mov r4, sp");
    /* Change SP to current stack point. */
    __asm("ldr r5, =_sstack");
    __asm(" mov sp, r5");
    main(1, (char**)mailbox);
    /* Restore previous SP point. */
    __asm("mov sp, r4");
}

