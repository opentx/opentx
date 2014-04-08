/****************************************************************************
 *  Copyright (c) 2014 by Michael Blandford. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the author nor the names of its contributors may
 *     be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 *  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 *  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 *  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *
 ****************************************************************************
 * Other Authors:
 * - Andre Bernet
 * - Bertrand Songis
 * - Bryan J. Rentoul (Gruvin)
 * - Cameron Weeks
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini
 * - Thomas Husterer
 *
 ****************************************************************************/

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(PCBTARANIS)
#include "stm32f2xx_flash.h"
#endif

#include "board_taranis.h"
#include "../pwr.h"
#include "../lcd.h"
#include "../keys.h"
#include "../sdcard.h"
#include "../FatFs/ff.h"
#include "../FatFs/diskio.h"
#include "../translations/en.h"

#if defined(PCBTARANIS)
  #define BOOTLOADER_TITLE      "Taranis Boot Loader - v1.0"
  #define BOOT_KEY_UP		KEY_PLUS
  #define BOOT_KEY_DOWN		KEY_MINUS
  #define BOOT_KEY_LEFT		KEY_MENU
  #define BOOT_KEY_RIGHT	KEY_PAGE
  #define BOOT_KEY_MENU		KEY_ENTER
  #define BOOT_KEY_EXIT		KEY_EXIT
  #define DISPLAY_CHAR_WIDTH	35
#elif defined(PCBSKY9X)
  #define BOOTLOADER_TITLE      "Sky9x Boot Loader - v1.0"
#endif

// states
enum BootLoaderStates {
  ST_START,
  ST_FLASH_MENU,
  ST_DIR_CHECK,
  ST_OPEN_DIR,
  ST_FILE_LIST,
  ST_FLASH_CHECK,
  ST_FLASHING,
  ST_FLASH_DONE,
  ST_USB,
  ST_REBOOT,

const uint8_t bootloaderVersion[] __attribute__ ((section(".version"))) =
{
  'B', 'O', 'O', 'T', '1', '0'
};

/*----------------------------------------------------------------------------
 *        Local variables
 *----------------------------------------------------------------------------*/

uint32_t FirmwareSize;

uint32_t Master_frequency;
volatile uint8_t Tenms;
uint8_t EE_timer;

TCHAR FlashFilename[60];
FIL FlashFile;
DIR Dj;
FILINFO Finfo;

TCHAR Filenames[20][50];
uint32_t FileSize[20];
uint32_t FnStartIndex;
uint32_t Valid;

uint32_t FlashSize;

uint32_t FlashBlocked = 1;
uint32_t LockBits;

uint32_t Block_buffer[1024];
UINT BlockCount;

#if defined(PCBSKY9X)
  extern int32_t EblockAddress;
#endif

extern uint32_t EepromBlocked;

extern void init_spi(void);
extern void writeBlock(void);
extern void usbPluggedIn();

/*----------------------------------------------------------------------------
 *         Global functions
 *----------------------------------------------------------------------------*/

#if defined(PCBSKY9X)
// Starts TIMER0 at full speed (MCK/2) for delay timing
// @ 36MHz this is 18MHz
// This was 6 MHz, we may need to slow it to TIMER_CLOCK2 (MCK/8=4.5 MHz)
void start_timer0()
{
  register Tc *ptc;

  PMC->PMC_PCER0 |= 0x00800000L;		// Enable peripheral clock to TC0

  ptc = TC0;// Tc block 0 (TC0-2)
  ptc->TC_BCR = 0;// No sync
  ptc->TC_BMR = 2;
  ptc->TC_CHANNEL[0].TC_CMR = 0x00008001;// Waveform mode MCK/8 for 36MHz osc.(Upset be write below)
  ptc->TC_CHANNEL[0].TC_RC = 0xFFF0;
  ptc->TC_CHANNEL[0].TC_RA = 0;
  ptc->TC_CHANNEL[0].TC_CMR = 0x00008040;// 0000 0000 0000 0000 1000 0000 0100 0000, stop at regC, 18MHz
  ptc->TC_CHANNEL[0].TC_CCR = 5;// Enable clock and trigger it (may only need trigger)
}

void stop_timer0( void )
{
  TC0->TC_CHANNEL[0].TC_CCR = TC_CCR0_CLKDIS;		// Disable clock
}

void delay2ms()
{
  TC0->TC_CHANNEL[0].TC_CCR = 5;// Enable clock and trigger it (may only need trigger)
  while ( TC0->TC_CHANNEL[0].TC_CV < 36000 )// 2mS, Value depends on MCK/2 (used 18MHz)
  {
    // Wait
  }
}
#endif

#if defined(PCBTARANIS)
uint32_t isFirmwareStart(uint32_t *block)
{
  if ((block[0] & 0xFFFC0000) != 0x20000000) {
    return 0;
  }
  if ((block[1] & 0xFFF00000) != 0x08000000) {
    return 0;
  }
  if ((block[2] & 0xFFF00000) != 0x08000000) {
    return 0;
  }
  return 1;
}
#elif defined(PCBSKY9X)
uint32_t isFirmwareStart( uint32_t *block )
{
  if ((block[0] & 0xFFFE3000) != 0x20000000 ) {
    return 0;
  }
  if ((block[1] & 0xFFF80000) != 0x00400000) {
    return 0;
  }
  if ((block[2] & 0xFFF80000) != 0x00400000) {
    return 0;
  }
  return 1;
}
#endif

#if defined(PCBSKY9X)

uint32_t (*IAP_Function)(uint32_t, uint32_t);

void writeFlash(uint32_t *address, uint32_t *buffer)	// size is 256 bytes
{
  uint32_t FlashSectorNum;
  uint32_t flash_cmd = 0;

  /* Initialize the function pointer (retrieve function address from NMI vector) */
  if ((uint32_t) address == FIRMWARE_START+BOOTLOADER_SIZE) {
    if (isFirmwareStart(buffer))
      FlashBlocked = 0;
    else
      FlashBlocked = 1;
  }

  if (FlashBlocked) {
    return;
  }

  // Always initialise this here, setting a default doesn't seem to work
  IAP_Function = (uint32_t (*)(uint32_t, uint32_t)) *(( uint32_t *)0x00800008);
  FlashSectorNum = (uint32_t) address;
  FlashSectorNum >>= 8;// page size is 256 bytes
  FlashSectorNum &= 2047;// max page number

  /* Send data to the sector here */
  for (int i=0; i<FLASH_PAGESIZE/4; i++) {
    *address++ = *buffer++;
  }

  /* build the command to send to EEFC */
  flash_cmd = (0x5A << 24) | (FlashSectorNum << 8) | 0x03; //AT91C_MC_FCMD_EWP ;

  __disable_irq();
  /* Call the IAP function with appropriate command */
  i = IAP_Function(0, flash_cmd);
  __enable_irq();
}

uint32_t readLockBits()
{
  // Always initialise this here, setting a default doesn't seem to work
  IAP_Function = (uint32_t (*)(uint32_t, uint32_t)) *(( uint32_t *)0x00800008);

  uint32_t flash_cmd = (0x5A << 24) | 0x0A;//AT91C_MC_FCMD_GLB ;
  __disable_irq();
  (void) IAP_Function( 0, flash_cmd );
  __enable_irq();
  return EFC->EEFC_FRR;
}

void clearLockBits()
{
  uint32_t i;
  uint32_t flash_cmd = 0;

  // Always initialise this here, setting a default doesn't seem to work
  IAP_Function = (uint32_t (*)(uint32_t, uint32_t)) *(( uint32_t *)0x00800008);
  for ( i = 0; i < 16; i += 1 )
  {
    flash_cmd = (0x5A << 24) | ((128*i) << 8) | 0x09; //AT91C_MC_FCMD_CLB ;
    __disable_irq();
    /* Call the IAP function with appropriate command */
    (void) IAP_Function( 0, flash_cmd );
    __enable_irq();
  }
}
#endif

void interrupt10ms(void)
{
  Tenms |= 1;			// 10 mS has passed

  uint8_t enuk = KEY_MENU;
  uint8_t in = ~readKeys();

  for (int i = 1; i < 7; i++) {
    uint8_t value = in & (1 << i);
    keys[enuk].input(value, (EnumKeys) enuk);
    ++enuk;
  }
}

#if defined(PCBSKY9X)
void init10msTimer()
{
  register Tc *ptc;
  register uint32_t timer;

  PMC->PMC_PCER0 |= 0x02000000L;		// Enable peripheral clock to TC2

  timer = Master_frequency / 12800;// MCK/128 and 100 Hz

  ptc = TC0;// Tc block 0 (TC0-2)
  ptc->TC_BCR = 0;// No sync
  ptc->TC_BMR = 0;
  ptc->TC_CHANNEL[2].TC_CMR = 0x00008000;// Waveform mode
  ptc->TC_CHANNEL[2].TC_RC = timer;// 10 Hz
  ptc->TC_CHANNEL[2].TC_RA = timer >> 1;
  ptc->TC_CHANNEL[2].TC_CMR = 0x0009C003;// 0000 0000 0000 1001 1100 0000 0000 0011
                                         // MCK/128, set @ RA, Clear @ RC waveform
  ptc->TC_CHANNEL[2].TC_CCR = 5;// Enable clock and trigger it (may only need trigger)

  NVIC_EnableIRQ(TC2_IRQn);
  TC0->TC_CHANNEL[2].TC_IER = TC_IER0_CPCS;
}

extern "C" void TC2_IRQHandler()
{
  register uint32_t dummy;

  /* Clear status bit to acknowledge interrupt */
  dummy = TC0->TC_CHANNEL[2].TC_SR;
  (void) dummy;		// Discard value - prevents compiler warning

  interrupt10ms();

}
#endif

#if defined(PCBTARANIS)
void init10msTimer()
{
  // Timer14
  RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;		// Enable clock
  TIM14->ARR = 9999;	// 10mS
  TIM14->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 1000000 - 1;	// 1uS from 12MHz
  TIM14->CCER = 0;
  TIM14->CCMR1 = 0;
  TIM14->EGR = 0;
  TIM14->CR1 = 5;
  TIM14->DIER |= 1;
  NVIC_EnableIRQ(TIM8_TRG_COM_TIM14_IRQn);
}

extern "C" void TIM8_TRG_COM_TIM14_IRQHandler()
{
  TIM14->SR &= ~TIM_SR_UIF;
  interrupt10ms();
}

void init_hw_timer()
{
  // Timer13
  RCC->APB1ENR |= RCC_APB1ENR_TIM13EN;		// Enable clock
  TIM13->ARR = 65535;
  TIM13->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 10000000 - 1;// 0.1uS from 12MHz
  TIM13->CCER = 0;
  TIM13->CCMR1 = 0;
  TIM13->EGR = 0;
  TIM13->CR1 = 1;
}

// delay in units of 0.1 uS up to 6.5535 mS
void hw_delay(uint16_t time)
{
  TIM13->CNT = 0;
  TIM13->EGR = 1;		// Re-start counter
  while ( TIM13->CNT < time) {
    // wait
  }
}

//After reset, write is not allowed in the Flash control register (FLASH_CR) to protect the
//Flash memory against possible unwanted operations due, for example, to electric
//disturbances. The following sequence is used to unlock this register:
//1. Write KEY1 = 0x45670123 in the Flash key register (FLASH_KEYR)
//2. Write KEY2 = 0xCDEF89AB in the Flash key register (FLASH_KEYR)
//Any wrong sequence will return a bus error and lock up the FLASH_CR register until the
//next reset.
//The FLASH_CR register can be locked again by software by setting the LOCK bit in the
//FLASH_CR register.
void unlockFlash()
{
  FLASH->KEYR = 0x45670123;
  FLASH->KEYR = 0xCDEF89AB;
}

void waitFlashIdle()
{
  while (FLASH->SR & FLASH_FLAG_BSY) {
    wdt_reset();
  }
}

#define SECTOR_MASK               ((uint32_t)0xFFFFFF07)

void eraseSector(uint32_t sector)
{
  waitFlashIdle();

  FLASH->CR &= CR_PSIZE_MASK;
  FLASH->CR |= FLASH_PSIZE_WORD;
  FLASH->CR &= SECTOR_MASK;
  FLASH->CR |= FLASH_CR_SER | (sector << 3);
  FLASH->CR |= FLASH_CR_STRT;

  /* Wait for operation to be completed */
  waitFlashIdle();

  /* if the erase operation is completed, disable the SER Bit */
  FLASH->CR &= (~FLASH_CR_SER);
  FLASH->CR &= SECTOR_MASK;
}

void writeFlash(uint32_t *address, uint32_t *buffer)	// size is 256 bytes
{
  uint32_t i;

  if ((uint32_t) address == 0x08008000) {
    if (isFirmwareStart(buffer)) {
      FlashBlocked = 0;
    }
    else {
      FlashBlocked = 1;
    }
  }

  if (FlashBlocked) {
    return;
  }

  if ((uint32_t) address == 0x08008000) {
    eraseSector(2);
  }
  if ((uint32_t) address == 0x0800C000) {
    eraseSector(3);
  }
  if ((uint32_t) address == 0x08010000) {
    eraseSector(4);
  }
  if ((uint32_t) address == 0x08020000) {
    eraseSector(5);
  }
  if ((uint32_t) address == 0x08040000) {
    eraseSector(6);
  }
  if ((uint32_t) address == 0x08060000) {
    eraseSector(7);
  }

  // Now program the 256 bytes

  for (i = 0; i < 64; i += 1) {
    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
     be done by word */

    // Wait for last operation to be completed
    waitFlashIdle();

    FLASH->CR &= CR_PSIZE_MASK;
    FLASH->CR |= FLASH_PSIZE_WORD;
    FLASH->CR |= FLASH_CR_PG;

    *address = *buffer;

    /* Wait for operation to be completed */
    waitFlashIdle();
    FLASH->CR &= (~FLASH_CR_PG);

    /* Check the written value */
    if (*address != *buffer) {
      /* Flash content doesn't match SRAM content */
      return;
    }
    /* Increment FLASH destination address */
    address += 1;
    buffer += 1;
  }
}

#endif

uint8_t *cpystr(uint8_t *dest, uint8_t *source)
{
  while ((*dest++ = *source++))
    ;
  return dest - 1;
}

FRESULT readBinDir(DIR *dj, FILINFO *fno)
{
  FRESULT fr;
  uint32_t loop;
  do {
    loop = 0;
    fr = f_readdir(dj, fno);		// First entry

    if (fr != FR_OK || fno->fname[0] == 0) {
      break;
    }
    if (*fno->lfname == 0) {
      cpystr((uint8_t *) fno->lfname, (uint8_t *) fno->fname);	// Copy 8.3 name
    }
    int32_t len = strlen(fno->lfname) - 4;
    if (len < 0) {
      loop = 1;
    }
    if (fno->lfname[len] != '.') {
      loop = 1;
    }
    if ((fno->lfname[len + 1] != 'b') && (fno->lfname[len + 1] != 'B')) {
      loop = 1;
    }
    if ((fno->lfname[len + 2] != 'i') && (fno->lfname[len + 2] != 'I')) {
      loop = 1;
    }
    if ((fno->lfname[len + 3] != 'n') && (fno->lfname[len + 3] != 'N')) {
      loop = 1;
    }

  } while (loop);
  return fr;
}

uint32_t fillNames(uint32_t index)
{
  uint32_t i;
  FRESULT fr;
  Finfo.lfname = Filenames[0];
  Finfo.lfsize = 48;
  fr = f_readdir(&Dj, 0);					// rewind
  fr = f_readdir(&Dj, &Finfo);		// Skip .
  fr = f_readdir(&Dj, &Finfo);		// Skip ..
  i = 0;
  while (i <= index) {
    fr = readBinDir(&Dj, &Finfo);		// First entry
    FileSize[0] = Finfo.fsize;
    i += 1;
    if (fr == FR_NO_FILE) {
      return 0;
    }
  }
  for (i = 1; i < 7; i += 1) {
    Finfo.lfname = Filenames[i];
    fr = readBinDir(&Dj, &Finfo);		// First entry
    FileSize[i] = Finfo.fsize;
    if (fr != FR_OK || Finfo.fname[0] == 0) {
      break;
    }
  }
  return i;
}

FRESULT openFirmwareFile(uint32_t index)
{
  cpystr(cpystr((uint8_t *)FlashFilename, (uint8_t *)FIRMWARES_PATH "/"), (uint8_t *) Filenames[index]);
  f_open(&FlashFile, FlashFilename, FA_READ);
  f_lseek(&FlashFile, BOOTLOADER_SIZE);
  return f_read(&FlashFile, (BYTE *) Block_buffer, 4096, &BlockCount);
}

int menuFlashFile(uint32_t index, uint8_t event)
{
  FRESULT fr;

  lcd_putsLeft(4*FH, "\012Hold [ENT] to start loading" );

  if (Valid == 0) {
    // Validate file here
    // return 3 if invalid
    fr = openFirmwareFile(index);
    fr = f_close(&FlashFile);
    Valid = 1;
    if (isFirmwareStart(Block_buffer) == 0) {
      Valid = 2;
    }
  }

  if (Valid == 2) {
    lcd_putsLeft(4*FH,  "\011No firmware found in the file!");
    if (event == EVT_KEY_FIRST(BOOT_KEY_EXIT) || event == EVT_KEY_FIRST(BOOT_KEY_MENU)) {
      return 0;
    }
    return -1;
  }

  if (event == EVT_KEY_LONG(BOOT_KEY_MENU)) {
    fr = openFirmwareFile(index);
    FirmwareSize = FileSize[index];
    if (fr != FR_OK) {
      return 0;		// File open error
    }
    return 1;
  }
  else if (event == EVT_KEY_FIRST(BOOT_KEY_EXIT)) {
    return 0;
  }

  return -1;
}

extern Key keys[];

static uint32_t PowerUpDelay;

int main()
{
  uint8_t index = 0;
#if defined(PCBTARANIS)
  uint8_t TenCount = 2;
#endif			
  uint8_t maxhsize = DISPLAY_CHAR_WIDTH;
  FRESULT fr;
  uint32_t state = ST_START;
  uint32_t nameCount = 0;
  uint32_t vpos = 0;
  uint32_t hpos = 0;
  uint32_t firmwareAddress = FIRMWARE_ADDRESS;
  uint32_t firmwareWritten = 0;

#if defined(PCBTARANIS)
  wdt_reset();
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; 		// Enable portA clock
#endif

  pwrInit();

#if defined(PCBSKY9X)
  MATRIX->CCFG_SYSIO |= 0x000000F0L;		// Disable syspins, enable B4,5,6,7
#endif

#if defined(PCBSKY9X)
  init_SDcard();
  PIOC->PIO_PER = PIO_PC25;		// Enable bit C25 (USB-detect)
  start_timer0();
#endif

  lcdInit();
#if defined(PCBSKY9X)
  extern uint8_t OptrexDisplay;
  OptrexDisplay = 1;
#endif
  lcd_clear();
  lcd_putsLeft(0, BOOTLOADER_TITLE);
  lcd_invert_line(0);
  lcdRefresh();
#if defined(PCBSKY9X)
  OptrexDisplay = 0;
  lcdRefresh();
#endif

#if defined(PCBTARANIS)
  keysInit();
  I2C_EE_Init();
  init_hw_timer();
#endif

  __enable_irq();
  init10msTimer();

#if defined(PCBSKY9X)
  EblockAddress = -1;
  init_spi();
#endif

#if defined(PCBSKY9X)
  uint32_t chip_id = CHIPID->CHIPID_CIDR;
  FlashSize = ( (chip_id >> 8 ) & 0x000F ) == 9 ? 256 : 512;
#elif defined(PCBTARANIS)
  FlashSize = 512;
#endif

#if defined(PCBSKY9X)
  LockBits = readLockBits();
  if (LockBits) {
    clearLockBits();
  }
#endif

#if defined(PCBTARANIS)
  // SD card detect pin
  sdInit();
  unlockFlash();
  usbInit();
  usbStart();
#endif

  for (;;) {
    wdt_reset();

    if (Tenms) {
      wdt_reset();  // Retrigger hardware watchdog

      if (EE_timer) {
        if (--EE_timer == 0) {
#if defined(PCBSKY9X)
          writeBlock();
#endif
        }
      }

      Tenms = 0;

      lcd_clear();
      lcd_putsLeft(0, BOOTLOADER_TITLE);
      lcd_invert_line(0);

      uint8_t event = getEvent();

      if (state != ST_USB) {
        if (usbPlugged()) {
          state = ST_USB;
          usbPluggedIn();
        }
      }

      if (state == ST_START) {
        lcd_putsLeft(2*FH, "\010Load Firmware");
        lcd_putsLeft(3*FH, "\010Exit");
        lcd_invert_line(2+vpos);
        lcd_putsLeft(6*FH, INDENT "Or plug in a USB cable for mass storage");
        if (event == EVT_KEY_FIRST(BOOT_KEY_DOWN) || event == EVT_KEY_FIRST(BOOT_KEY_UP)) {
          vpos = (vpos+1) & 0x01;
        }
        else if (event == EVT_KEY_BREAK(BOOT_KEY_MENU)) {
          if (vpos == 0)
            state = ST_FLASH_MENU;
          else
            state = ST_REBOOT;
        }
      }

      if (state == ST_USB) {
        lcd_putsLeft(4*FH, "\026USB Connected");
        if (usbPlugged() == 0) {
          vpos = 0;
          state = ST_START;
        }
#if defined(PCBSKY9X)
        usbMassStorage();
        lcd_putc( 0, 6*FH, 'F' );
        lcd_putc( 6, 6*FH, '0' + FlashBlocked );
        lcd_putc( 0, 7*FH, 'E' );
        lcd_putc( 6, 7*FH, '0' + EepromBlocked );
#endif
      }

      if (state == ST_FLASH_MENU) {
        sdInit();
        state = ST_DIR_CHECK;
      }

      else if (state == ST_DIR_CHECK) {
        fr = f_chdir(FIRMWARES_PATH);
        if (fr == FR_OK) {
          state = ST_OPEN_DIR;
        }
        else {
          lcd_putsLeft(2*FH, INDENT "No firmware in " FIRMWARES_PATH " directory");
          if (event == EVT_KEY_FIRST(BOOT_KEY_EXIT) || event == EVT_KEY_FIRST(BOOT_KEY_MENU)) {
            vpos = 0;
            state = ST_START;
          }
        }
      }

      if (state == ST_OPEN_DIR) {
        index = 0;
        fr = f_opendir(&Dj, ".");
        if (fr == FR_OK) {
          state = ST_FILE_LIST;
          nameCount = fillNames(0);
          hpos = 0;
          vpos = 0;
        }
      }

      if (state == ST_FILE_LIST) {
        uint32_t limit = 6;
        if (nameCount < limit) {
          limit = nameCount;
        }
        maxhsize = 0;
        for (uint32_t i=0; i<limit; i++) {
          uint32_t x;
          x = strlen(Filenames[i]);
          if (x > maxhsize) {
            maxhsize = x;
          }
          if (x > DISPLAY_CHAR_WIDTH) {
            if (hpos + DISPLAY_CHAR_WIDTH > x) {
              x = x - DISPLAY_CHAR_WIDTH;
            }
            else {
              x = hpos;
            }
          }
          else {
            x = 0;
          }
          lcd_putsnAtt(INDENT_WIDTH, 16 + FH * i, &Filenames[i][x], DISPLAY_CHAR_WIDTH, 0);
        }

        if (event == EVT_KEY_REPT(BOOT_KEY_DOWN) || event == EVT_KEY_FIRST(BOOT_KEY_DOWN)) {
          if (vpos < limit - 1) {
            vpos += 1;
          }
          else {
            if (nameCount > limit) {
              index += 1;
              nameCount = fillNames(index);
            }
          }
        }
        else if (event == EVT_KEY_REPT(BOOT_KEY_UP) || event == EVT_KEY_FIRST(BOOT_KEY_UP)) {
          if (vpos > 0) {
            vpos -= 1;
          }
          else {
            if (index) {
              index -= 1;
              nameCount = fillNames(index);
            }
          }
        }
#if !defined(PCBTARANIS)
        else if (event == EVT_KEY_REPT(BOOT_KEY_RIGHT) || event == EVT_KEY_FIRST(BOOT_KEY_RIGHT)) {
          if (hpos + DISPLAY_CHAR_WIDTH < maxhsize) {
            hpos += 1;
          }
        }
        else if (event == EVT_KEY_REPT(BOOT_KEY_LEFT) || event == EVT_KEY_FIRST(BOOT_KEY_LEFT)) {
          if (hpos) {
            hpos -= 1;
          }
        }
#endif
        else if (event == EVT_KEY_BREAK(BOOT_KEY_MENU)) {
          // Select file to flash
          state = ST_FLASH_CHECK;
          Valid = 0;
        }
        else if (event == EVT_KEY_FIRST(BOOT_KEY_EXIT)) {
          state = ST_START;
          vpos = 0;
        }

        lcd_invert_line(2 + vpos);
      }

      else if (state == ST_FLASH_CHECK) {
        int result = menuFlashFile(vpos, event);
        FirmwareSize = FileSize[vpos] - BOOTLOADER_SIZE;
        if (result == 0) {
          // canceled
          state = ST_FILE_LIST;
        }
        else if (result == 1) {
          // confirmed
          firmwareAddress = FIRMWARE_ADDRESS + BOOTLOADER_SIZE;
          firmwareWritten = 0;
          state = ST_FLASHING;
        }
      }

      if (state == ST_FLASHING) {
        // Commit to flashing
        uint32_t blockOffset = 0;
        lcd_putsLeft(4*FH, "\032Loading...");
        while (BlockCount) {
          writeFlash((uint32_t *)firmwareAddress, &Block_buffer[blockOffset]);
          blockOffset += FLASH_PAGESIZE/4; // 32-bit words
          firmwareAddress += FLASH_PAGESIZE;
          if (BlockCount > FLASH_PAGESIZE) {
            BlockCount -= FLASH_PAGESIZE;
          }
          else {
            BlockCount = 0;
          }
        }
        firmwareWritten += 4; // 4K blocks

        lcd_rect( 3, 6*FH+4, 204, 7);
        lcd_hline(5, 6*FH+6, (200*firmwareWritten*1024)/FirmwareSize, FORCE);
        lcd_hline(5, 6*FH+7, (200*firmwareWritten*1024)/FirmwareSize, FORCE);
        lcd_hline(5, 6*FH+8, (200*firmwareWritten*1024)/FirmwareSize, FORCE);

        fr = f_read(&FlashFile, (BYTE *)Block_buffer, sizeof(Block_buffer), &BlockCount);
        if (BlockCount == 0) {
          state = ST_FLASH_DONE;
        }
        if (firmwareWritten >= FlashSize - 32) {
          state = ST_FLASH_DONE;				// Backstop
        }
      }

      if (state == ST_FLASH_DONE) {
        lcd_putsLeft(4*FH, "\024Loading Complete");
        if (event == EVT_KEY_FIRST(BOOT_KEY_EXIT) || event == EVT_KEY_BREAK(BOOT_KEY_MENU)) {
          state = ST_START;
          vpos = 0;
        }
      }

      if (event == EVT_KEY_LONG(BOOT_KEY_EXIT)) {
        state = ST_REBOOT;
      }

      if (--TenCount == 0) {
        TenCount = 2;
        lcdRefresh();
      }

      if (PowerUpDelay < 20) {	// 200 mS
        PowerUpDelay += 1;
      }
      else {
        sdPoll10ms();
      }
    }

    if (pwrCheck() == e_power_off && state != ST_FLASHING && state != ST_USB) {
      pwrOff();
      for (;;) {
        // Wait for power to go off
      }
    }

    if (state == ST_REBOOT) {
      if ((~readKeys() & 0x7E) == 0) {
        NVIC_SystemReset();
      }
    }
  }

  return 0;
}

