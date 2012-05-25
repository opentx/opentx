#ifndef _LIB_ATSAM3S_
#define _LIB_ATSAM3S_

/*
 * Peripherals registers definitions
 */
#if defined atsam3s4
    #include <include/ATSAM3S4.h>
#elif defined atsam3s2
    #include <include/ATSAM3S2.h>
#elif defined atsam3s1
    #include <include/ATSAM3S1.h>
#else
    #warning Library does not support the specified chip, specifying atsam3s4.
    #define atsam3s4
    #include <include/ATSAM3S4.h>
#endif


/* Define attribute */
#if defined   ( __CC_ARM   ) /* Keil µVision 4 */
    #define WEAK __attribute__ ((weak))
#elif defined ( __ICCARM__ ) /* IAR Ewarm 5.41+ */
    #define WEAK __weak
#elif defined (  __GNUC__  ) /* GCC CS3 2009q3-68 */
    #define WEAK __attribute__ ((weak))
#endif

/* Define NO_INIT attribute */
#if defined   ( __CC_ARM   )
    #define NO_INIT
#elif defined ( __ICCARM__ )
    #define NO_INIT __no_init
#elif defined (  __GNUC__  )
    #define NO_INIT
#endif

/*
 * Core
 */

/*
 * Peripherals
 */
//#include <include/async.h>
//#include <include/efc.h>
//#include <include/flashd.h>
//#include <include/pio.h>
//#include <include/pmc.h>
//#include <include/spi.h>
//#include <include/spi_pdc.h>
//#include <include/twi.h>
//#include <include/twid.h>
//#include <include/usart.h>

/*
#include <USBD_Config.h>
*/

//#include <include/trace.h>
//#include <include/wdt.h>

#endif /* _LIB_ATSAM3S_ */
