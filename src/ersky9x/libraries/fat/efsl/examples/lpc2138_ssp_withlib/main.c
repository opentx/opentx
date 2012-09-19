/******************************************************************************
 
 efsl Demo-Application for Philips LPC2138 ARM7TDMI-S
 
 Copyright (c) 2005
 Martin Thomas, Kaiserslautern, Germany <mthomas@rhrk.uni-kl.de>
 
 *****************************************************************************/

#include <string.h>

#include "LPC213x.h"
#include "lpc_config.h"

#include "uart.h"

#include "efs.h"
#include "ls.h"
#include "mkfs.h"
#include "interfaces/lpc2000_dbg_printf.h"

#define rprintf lpc2000_debug_printf

#define BAUD 115200

#define BIT(x) ((unsigned long)1<<x)

// 1st LED on Keil MCB2130
#define LED1PIN  	16
#define LED1BIT     BIT(LED1PIN)
#define LEDDIR      IODIR1
#define LEDSET      IOSET1
#define LEDCLR      IOCLR1

static char LogFileName[] = "logdat9.txt";

static void systemInit(void)
{
	// --- enable and connect the PLL (Phase Locked Loop) ---
	// a. set multiplier and divider
	PLLCFG = MSEL | (1<<PSEL1) | (0<<PSEL0);
	// b. enable PLL
	PLLCON = (1<<PLLE);
	// c. feed sequence
	PLLFEED = PLL_FEED1;
	PLLFEED = PLL_FEED2;
	// d. wait for PLL lock (PLOCK bit is set if locked)
	while (!(PLLSTAT & (1<<PLOCK)));
	// e. connect (and enable) PLL
	PLLCON = (1<<PLLE) | (1<<PLLC);
	// f. feed sequence
	PLLFEED = PLL_FEED1;
	PLLFEED = PLL_FEED2;
	
	// --- setup and enable the MAM (Memory Accelerator Module) ---
	// a. start change by turning of the MAM (redundant)
	MAMCR = 0;	
	// b. set MAM-Fetch cycle to 3 cclk as recommended for >40MHz
	MAMTIM = MAM_FETCH;
	// c. enable MAM 
	MAMCR = MAM_MODE;
	
	// --- set VPB speed ---
	VPBDIV = VPBDIV_VAL;
	
	// --- map INT-vector ---
	#if defined(RAM_RUN)
	  MEMMAP = MEMMAP_USER_RAM_MODE;
	#elif defined(ROM_RUN)
	  MEMMAP = MEMMAP_USER_FLASH_MODE;
	#else
	#error RUN_MODE not defined!
	#endif
}

static void gpioInit(void)
{
	LEDSET  = BIT(LED1PIN);	 // set Bit = LED off (active low)
	LEDDIR |= BIT(LED1PIN); // define LED-Pin as output
}

static void ledToggle(void)
{
	static unsigned char state=0;
	
	state = !state;
	if (state) LEDCLR = BIT(LED1PIN);	// set Bit = LED on
	else LEDSET = BIT(LED1PIN);	// set Bit = LED off (active low)
}

#if 0
static void hang(void)
{
	while(1);
}
#endif


EmbeddedFileSystem efs;
EmbeddedFile filer, filew;
DirList list;
unsigned short e;
unsigned char buf[513];


int main(void)
{
	int ch;
	int8_t res;
	
	systemInit();
	gpioInit();
	
	uart1Init(UART_BAUD(BAUD), UART_8N1, UART_FIFO_8); // setup the UART
		
	uart1Puts("\r\nMMC/SD Card Filesystem Test (P:LPC2138 L:EFSL)\r\n");
	uart1Puts("efsl LPC2000-port and this Demo-Application\r\n");
	uart1Puts("done by Martin Thomas, Kaiserslautern, Germany\r\n");
	
	/* init efsl debug-output */
	lpc2000_debug_devopen(uart1Putch);
		
	ledToggle();
	
	rprintf("CARD init...");
	if ( ( res = efs_init( &efs, 0 ) ) != 0 ) {
		rprintf("failed with %i\n",res);
	}
	else {
		rprintf("ok\n");
		rprintf("Directory of 'root':\n");
		ls_openDir( &list, &(efs.myFs) , "/");
		while ( ls_getNext( &list ) == 0 ) {
			list.currentEntry.FileName[LIST_MAXLENFILENAME-1] = '\0';
			rprintf( "%s ( %li bytes )\n" ,
				list.currentEntry.FileName,
				list.currentEntry.FileSize ) ;
		}
		
		if ( file_fopen( &filer, &efs.myFs , LogFileName , 'r' ) == 0 ) {
			rprintf("File %s open. Content:\n", LogFileName);
			while ( ( e = file_read( &filer, 512, buf ) ) != 0 ) {
				buf[e]='\0';
				uart1Puts((char*)buf);
			}
			rprintf("\n");
			file_fclose( &filer );
		}
		
		if ( file_fopen( &filew, &efs.myFs , LogFileName , 'a' ) == 0 ) {
			rprintf("File %s open for append. Appending...", LogFileName);
			strcpy((char*)buf, "Martin hat's angehaengt\r\n");
			if ( file_write( &filew, strlen((char*)buf), buf ) == strlen((char*)buf) ) {
				rprintf("ok\n");
			}
			else {
				rprintf("fail\n");
			}
			file_fclose( &filew );
		}
		
		if ( file_fopen( &filer, &efs.myFs , LogFileName , 'r' ) == 0 ) {
			rprintf("File %s open. Content:\n", LogFileName);
			while ( ( e = file_read( &filer, 512, buf ) ) != 0 ) {
				buf[e]='\0';
				uart1Puts((char*)buf);
			}
			rprintf("\n");
			file_fclose( &filer );
		}
		
		fs_umount( &efs.myFs ) ;
	}
	
	while(1) {
		if ((ch = uart1Getch()) >= 0) {
			uart1Puts("You pressed : ");
			uart1Putch(ch);
			uart1Puts("\r\n");
			if ( ch == 'M' ) {
				rprintf("Creating FS\n");
				mkfs_makevfat(&efs.myPart);
			}
			ledToggle();
		}
	}
	
	return 0; /* never reached */
}
