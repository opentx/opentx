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

#include "opentx.h"
#include "stamp.h"
#include "boot.h"

#define APP_START_ADDRESS (uint32_t)(FIRMWARE_ADDRESS + BOOTLOADER_SIZE)

#if defined(EEPROM)
#define MAIN_MENU_LEN 3
#else
#define MAIN_MENU_LEN 2
#endif

typedef void (*voidFunction)(void);

#define jumpTo(addr) {                                          \
        SCB->VTOR = addr;                                       \
        __set_MSP(*(__IO uint32_t*)addr);                       \
        uint32_t     jumpAddress = *(uint32_t*)(addr + 4);      \
        voidFunction jumpFn = (voidFunction)jumpAddress;        \
        jumpFn();                                               \
    }

const uint8_t bootloaderVersion[] __attribute__ ((section(".version"), used)) =
{
  'B', 'O', 'O', 'T', '1', '0'
};

#if defined(ROTARY_ENCODER_NAVIGATION)
volatile rotenc_t rotencValue[1] = {0};
#endif

/*----------------------------------------------------------------------------
 *        Local variables
 *----------------------------------------------------------------------------*/

uint32_t FirmwareSize;
uint32_t firmwareAddress = FIRMWARE_ADDRESS;
uint32_t firmwareWritten = 0;

#if defined(EEPROM)
uint32_t eepromAddress = 0;
uint32_t eepromWritten = 0;
#endif

TCHAR backupFilename[_MAX_LFN+1];

uint32_t Master_frequency;
volatile uint8_t Tenms = 1;

FIL FlashFile;
DIR Dj;
FILINFO Finfo;

TCHAR Filenames[20][_MAX_LFN + 1];
uint32_t FileSize[20];
uint32_t Valid;

#define BLOCK_LEN 4096
uint8_t Block_buffer[BLOCK_LEN];
UINT BlockCount;

uint32_t memoryType;
uint32_t unlocked = 0;

void interrupt10ms(void)
{
  Tenms |= 1;     // 10 mS has passed

  uint8_t index = 0;
  uint8_t in = readKeys();
  for (uint8_t i = 1; i != uint8_t(1 << TRM_BASE); i <<= 1) {
    uint8_t value = (in & i);
    keys[index].input(value);
    ++index;
  }

#if defined(ROTARY_ENCODER_NAVIGATION)
  checkRotaryEncoder();
  static rotenc_t rePreviousValue;
  rotenc_t reNewValue = (rotencValue[0] / 2);
  int8_t scrollRE = reNewValue - rePreviousValue;
  if (scrollRE) {
    rePreviousValue = reNewValue;
    if (scrollRE < 0) {
        putEvent(EVT_KEY_FIRST(KEY_UP)); //EVT_ROTARY_LEFT
    }
    else {
        putEvent(EVT_KEY_FIRST(KEY_DOWN)); //EVT_ROTARY_RIGHT
    }
  }
#endif
}

void init10msTimer()
{
  INTERRUPT_xMS_TIMER->ARR = 9999;  // 10mS in uS
  INTERRUPT_xMS_TIMER->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 1000000 - 1; // 1uS
  INTERRUPT_xMS_TIMER->CCER = 0;
  INTERRUPT_xMS_TIMER->CCMR1 = 0;
  INTERRUPT_xMS_TIMER->EGR = 0;
  INTERRUPT_xMS_TIMER->CR1 = 5;
  INTERRUPT_xMS_TIMER->DIER |= 1;
  NVIC_EnableIRQ(INTERRUPT_xMS_IRQn);
}

extern "C" void INTERRUPT_xMS_IRQHandler()
{
  INTERRUPT_xMS_TIMER->SR &= ~TIM_SR_UIF;
  interrupt10ms();
}

FRESULT readBinDir(DIR *dj, FILINFO *fno)
{
  FRESULT fr;
  do {
    fr = f_readdir(dj, fno);    // First entry

    if (fr != FR_OK || fno->fname[0] == 0)
      break;

    int32_t len = strlen(fno->fname) - 4;
    if (len < 0)
        continue;

    if (fno->fname[len] != '.')
        continue;
    
    if ((fno->fname[len + 1] != 'b') && (fno->fname[len + 1] != 'B'))
        continue;

    if ((fno->fname[len + 2] != 'i') && (fno->fname[len + 2] != 'I'))
        continue;

    if ((fno->fname[len + 3] != 'n') && (fno->fname[len + 3] != 'N'))
        continue;

    // match!
    break;

  } while (1);

  return fr;
}

uint32_t fillNames(uint32_t index)
{
  uint32_t i;
  FRESULT fr;
  fr = f_readdir(&Dj, 0);         // rewind
  for (i = 0; i <= index; ++i) {
    fr = readBinDir(&Dj, &Finfo);   // First entry
    if (fr == FR_NO_FILE) {
      return 0;
    }
  }
  strAppend(Filenames[0], Finfo.fname);
  FileSize[0] = Finfo.fsize;
  for (i = 1; i < 7; i += 1) {
    fr = readBinDir(&Dj, &Finfo);
    if (fr != FR_OK || Finfo.fname[0] == 0) {
      break;
    }
    strAppend(Filenames[i], Finfo.fname);
    FileSize[i] = Finfo.fsize;
  }
  return i;
}

const char *getBinaryPath()
{
    if (memoryType == MEM_FLASH)
        return FIRMWARES_PATH;
    else
        return EEPROMS_PATH;
}

FRESULT openBinaryFile(uint32_t index)
{
  TCHAR filename[_MAX_LFN+1];
  FRESULT fr;
  memset(Block_buffer, 0, sizeof(Block_buffer));
  strAppend(strAppend(strAppend(filename, getBinaryPath()), "/"), Filenames[index]);
  if ((fr = f_open(&FlashFile, filename, FA_READ)) != FR_OK) {
    return fr;
  }
  if (memoryType == MEM_FLASH) {
    if ((fr = f_lseek(&FlashFile, BOOTLOADER_SIZE)) != FR_OK) {
      return fr;
    }
  }
  fr = f_read(&FlashFile, Block_buffer, BLOCK_LEN, &BlockCount);

  if (BlockCount == BLOCK_LEN)
    return fr;
  else
    return FR_INVALID_OBJECT;
}

uint32_t isValidBufferStart(const uint8_t * buffer)
{
#if defined(EEPROM)
  if (memoryType == MEM_FLASH)
    return isFirmwareStart(buffer);
  else
    return isEepromStart(buffer);
#else
  return isFirmwareStart(buffer);
#endif
}

int menuFlashFile(uint32_t index, event_t event)
{
  FRESULT fr;

  if (Valid == 0) {
    // Validate file here
    if ((fr = openBinaryFile(index))) {
      Valid = 2;
    }
    else {
      if ((fr = f_close(&FlashFile))) {
        Valid = 2;
      }
      else {
        Valid = 1;
      }
      if (!isValidBufferStart(Block_buffer)) {
        Valid = 2;
      }
    }
  }

  if (Valid == 2) {
    if (memoryType == MEM_FLASH)
        bootloaderDrawMessage(STR_INVALID_FIRMWARE, 2, false);
    else
        bootloaderDrawMessage(STR_INVALID_EEPROM, 2, false);

    if (event == EVT_KEY_BREAK(KEY_EXIT) || event == EVT_KEY_BREAK(KEY_ENTER)) {
      return 0;
    }
    return -1;
  }

  bootloaderDrawMessage(STR_HOLD_ENTER_TO_START, 2, false);

  if (event == EVT_KEY_LONG(KEY_ENTER)) {
    fr = openBinaryFile(index);
    return (fr == FR_OK && isValidBufferStart(Block_buffer));
  }
  else if (event == EVT_KEY_FIRST(KEY_EXIT)) {
    return 0;
  }

  return -1;
}

extern Key keys[];

static uint32_t PowerUpDelay;

void flashWriteBlock()
{
  uint32_t blockOffset = 0;
  while (BlockCount) {
    flashWrite((uint32_t *)firmwareAddress, (uint32_t *)&Block_buffer[blockOffset]);
    blockOffset += FLASH_PAGESIZE;
    firmwareAddress += FLASH_PAGESIZE;
    if (BlockCount > FLASH_PAGESIZE) {
      BlockCount -= FLASH_PAGESIZE;
    }
    else {
      BlockCount = 0;
    }
  }
}

#if defined(EEPROM)
void writeEepromBlock()
{
  eepromWriteBlock(Block_buffer, eepromAddress, BlockCount);
  eepromAddress += BlockCount;
}
#endif

int main()
{
  BootloaderState state = ST_START;
  uint32_t vpos = 0;
  uint8_t index = 0;
  FRESULT fr;
  uint32_t nameCount = 0;

  wdt_reset();
  RCC_AHB1PeriphClockCmd(PWR_RCC_AHB1Periph | KEYS_RCC_AHB1Periph |
                         LCD_RCC_AHB1Periph | BACKLIGHT_RCC_AHB1Periph |
                         SERIAL_RCC_AHB1Periph | I2C_RCC_AHB1Periph |
                         SD_RCC_AHB1Periph, ENABLE);

  RCC_APB1PeriphClockCmd(LCD_RCC_APB1Periph | BACKLIGHT_RCC_APB1Periph |
                         INTERRUPT_xMS_RCC_APB1Periph | I2C_RCC_APB1Periph |
                         SERIAL_RCC_APB1Periph |
                         SD_RCC_APB1Periph, ENABLE);

  RCC_APB2PeriphClockCmd(LCD_RCC_APB2Periph | BACKLIGHT_RCC_APB2Periph, ENABLE);

  keysInit();

  // wait for inputs to stabilize
  for (uint32_t i = 0; i < 50000; i += 1) {
    wdt_reset();
  }
  
  if ((readTrims() & 0x42) == 0) { // LHR & RHL trims not pressed
      // Start main application
      jumpTo(APP_START_ADDRESS);
  }

  pwrInit();
  delaysInit(); // needed for lcdInit()

#if defined(DEBUG)
  serial2Init(UART_MODE_DEBUG, 0); // default serial mode (None if DEBUG not defined)
#endif

  __enable_irq();
  TRACE("\nHorus bootloader started :)");

  lcdInit();
  backlightInit();

#if defined(PCBTARANIS)
  i2cInit();
#endif

  // we need to initialize g_FATFS_Obj here, because it is in .ram section (because of DMA access) 
  // and this section is un-initialized
  //memset(&g_FATFS_Obj, 0, sizeof(g_FATFS_Obj)); // probably not needed, f_mount does the init...
  
  init10msTimer();

  // SD card detect pin
  sdInit();
  usbInit();

  // init screen
  bootloaderInitScreen();

#if defined(PWR_PRESS_BUTTON) or defined(PCBHORUS)
  // wait until power button is released
  while(pwrPressed());
#endif
  
  for (;;) {
    wdt_reset();

    if (Tenms) {
      Tenms = 0;

      if (state != ST_USB) {
        if (usbPlugged()) {
          state = ST_USB;
          if (!unlocked) {
            unlocked = 1;
            unlockFlash();
          }
          usbStart();
          usbPluggedIn();
        }
      }

      lcdRefreshWait();
      event_t event = getEvent();

      if (state == ST_START) {

          bootloaderDrawScreen(state, vpos);

          if (event == EVT_KEY_FIRST(KEY_DOWN)) {
              vpos = (vpos + 1) % MAIN_MENU_LEN;
          }
          else if (event == EVT_KEY_FIRST(KEY_UP)) {
              vpos = (vpos + MAIN_MENU_LEN - 1) % MAIN_MENU_LEN;
          }
          else if (event == EVT_KEY_BREAK(KEY_ENTER)) {
              switch (vpos) {
              case 0:
                  state = ST_FLASH_MENU;
                  break;
#if defined(EEPROM)
              case 1:
                  state = ST_RESTORE_MENU;
                  break;
#endif
              default:
                  state = ST_REBOOT;
                  break;
              }
          }
      }

      if (state == ST_USB) {
          if (usbPlugged() == 0) {
              vpos = 0;
              usbStop();
              if (unlocked) {
                  lockFlash();
                  unlocked = 0;
              }
              state = ST_START;
          }
          bootloaderDrawScreen(state, 0);
      }

      if (state == ST_FLASH_MENU || state == ST_RESTORE_MENU) {
          // rco: why? has been initialised at the beginning already..
          //sdInit();
          memoryType = (state == ST_RESTORE_MENU ? MEM_EEPROM : MEM_FLASH);
          state = ST_DIR_CHECK;
      }
      else if (state == ST_DIR_CHECK) {

          fr = f_chdir(getBinaryPath());
          if (fr == FR_OK) {
              index = 0;
              fr = f_opendir(&Dj, ".");
              if (fr == FR_OK) {
                  state = ST_FILE_LIST;
                  nameCount = fillNames(0);
                  vpos = 0;
              }
              else {
                  bootloaderDrawScreen(state, 0);
                  bootloaderDrawMessage(INDENT "Directory is empty!", 0, false);
              }
          }
          else {
              bootloaderDrawScreen(state, 0);
              bootloaderDrawMessage(INDENT "Directory is missing!", 0, false);
              bootloaderDrawMessage(getBinaryPath(), 1, false);
          }

          if (fr != FR_OK) {
              if (event == EVT_KEY_BREAK(KEY_EXIT) || event == EVT_KEY_BREAK(KEY_ENTER)) {
                  vpos = 0;
                  state = ST_START;
              }
          }
      }

      if (state == ST_FILE_LIST) {
        uint32_t limit = 6;
        if (nameCount < limit) {
          limit = nameCount;
        }

        if (event == EVT_KEY_REPT(KEY_DOWN) || event == EVT_KEY_FIRST(KEY_DOWN)) {
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
        else if (event == EVT_KEY_REPT(KEY_UP) || event == EVT_KEY_FIRST(KEY_UP)) {
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
        else if (event == EVT_KEY_BREAK(KEY_ENTER)) {
          // Select file to flash
          state = ST_FLASH_CHECK;
          Valid = 0;
        }
        else if (event == EVT_KEY_FIRST(KEY_EXIT)) {
          state = ST_START;
          vpos = 0;
        }

        bootloaderDrawScreen(state, 0);
        for (uint32_t i=0; i<limit; i++) {
            bootloaderDrawMessage(Filenames[i], i, (vpos == i));
        }
      }
      else if (state == ST_FLASH_CHECK) {

          bootloaderDrawScreen(state, 0);

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

#if defined(EEPROM)
              eepromAddress = 0;
              eepromWritten = 0;
#endif
              state = ST_FLASHING;
          }
      }

      else if (state == ST_FLASHING) {
        // commit to flashing
        if (!unlocked && (memoryType == MEM_FLASH)) {
          unlocked = 1;
          unlockFlash();
        }

        int progress = 0;
        if (memoryType == MEM_FLASH) {
          flashWriteBlock();
          firmwareWritten += sizeof(Block_buffer);
          progress = ((LCD_W-12)*firmwareWritten) / FirmwareSize;
        }
#if defined(EEPROM)
        else {
          writeEepromBlock();
          eepromWritten += sizeof(Block_buffer);
          progress = ((LCD_W-12)*eepromWritten) / EEPROM_SIZE;
        }
#endif

        bootloaderDrawScreen(state, progress);

        fr = f_read(&FlashFile, Block_buffer, sizeof(Block_buffer), &BlockCount);
        if (BlockCount == 0) {
          state = ST_FLASH_DONE; // EOF
        }
        if (firmwareWritten >= FLASHSIZE - BOOTLOADER_SIZE) {
          state = ST_FLASH_DONE; // Backstop
        }
#if defined(EEPROM)
        if (eepromWritten >= EEPROM_SIZE) {
          state = ST_FLASH_DONE; // Backstop
        }
#endif
      }

      if (state == ST_FLASH_DONE) {
        if (unlocked) {
          lockFlash();
          unlocked = 0;
        }

        if (event == EVT_KEY_FIRST(KEY_EXIT) || event == EVT_KEY_BREAK(KEY_ENTER)) {
          state = ST_START;
          vpos = 0;
        }

        bootloaderDrawScreen(state, 0);
      }

      if (event == EVT_KEY_LONG(KEY_EXIT)) {
          // Start the main application

          state = ST_REBOOT;
      }

      lcdRefresh();

      if (PowerUpDelay < 20) {  // 200 mS
        PowerUpDelay += 1;
      }
      else {
        sdPoll10ms();
      }
    }

    if (state != ST_FLASHING && state != ST_USB) {
      if (pwrOffPressed()) {
        lcdClear();
        lcdOff(); // this drains LCD caps
        pwrOff();
        for (;;) {
          // Wait for power to go off
        }
      }
    }

    if (state == ST_REBOOT) {
        // Jump to proper application address
        jumpTo(APP_START_ADDRESS);
    }
  }

  return 0;
}

#if defined(PCBHORUS)
void *__dso_handle = 0;
#endif
