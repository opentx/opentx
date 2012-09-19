/* System Config */

/* 
	PLL
	
   - Main clock F_OSC=12,0MHz @ Keil MCB2130) [limits: 10 MHz to 25 MHz]
   - System should run at max. Frequency (60MHz) [limit: max 60 MHz]
   - Choose multiplier M=5 
     so cclk = M * F_OSC= 5 * 12000000 = 60000000 Hz
   - MSEL-Bit in PLLCFG (bits 0-4) MSEL = M-1
   - F_CCO must be inbetween the limits 156 MHz to 320 MHz
     datasheet: F_CCO = F_OSC * M * 2 * P
   - choose devider P=2 => F_CCO = 12000000Hz * 5 * 2 * 2
     = 240 MHz
   - PSEL0 (Bit5 in PLLCFG) = 1, PSEL1 (Bit6) = 0 (0b01)
*/
#define FOSC		12000000
#define PLL_M		5	
#define MSEL		(PLL_M-1)
#define PSEL0 		5
#define PSEL1 		6

#define PLLE		0
#define PLLC		1

#define PLOCK		10

#define PLL_FEED1	0xAA
#define PLL_FEED2	0x55


/*
	MAM(Memory Accelerator Module)
	- choosen: MAM fully enabled = MAM-Mode 2
	- System-Clock cclk=60MHz -> 3 CCLKs are proposed as fetch timing
*/
#define MAM_MODE 	2
#define MAM_FETCH   3
	
/*
	VPB (V... Pheriphal Bus)
	- choosen: VPB should run at full speed -> devider VPBDIV=1
	=> pclk = cclk = 60MHz
*/
#define VPBDIV_VAL	1

/* 
	SCB
*/
#define MEMMAP_BOOT_LOADER_MODE   0       // Interrupt vectors are re-mapped to Boot Block.
#define MEMMAP_USER_FLASH_MODE    (1<<0)  // Interrupt vectors are not re-mapped and reside in Flash.
#define MEMMAP_USER_RAM_MODE      (1<<1)  // Interrupt vectors are re-mapped to Static RAM.
