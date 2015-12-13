#include "../../opentx.h"

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

void lockFlash()
{
  while (!FLASH->SR & FLASH_SR_BSY);
  FLASH->CR |= FLASH_CR_LOCK;
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

void writeFlash(uint32_t *address, uint32_t *buffer) // page size is 256 bytes
{
  if ((uint32_t) address == 0x08000000) {
    eraseSector(0);
  }
  else if ((uint32_t) address == 0x08004000) {
    eraseSector(1);
  }
  else if ((uint32_t) address == 0x08008000) {
    eraseSector(2);
  }
  else if ((uint32_t) address == 0x0800C000) {
    eraseSector(3);
  }
  else if ((uint32_t) address == 0x08010000) {
    eraseSector(4);
  }
  else if ((uint32_t) address == 0x08020000) {
    eraseSector(5);
  }
  else if ((uint32_t) address == 0x08040000) {
    eraseSector(6);
  }
  else if ((uint32_t) address == 0x08060000) {
    eraseSector(7);
  }

  for (uint32_t i=0; i<FLASH_PAGESIZE/4; i++) {
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

uint32_t isFirmwareStart(const void * buffer)
{
  const uint32_t * block = (const uint32_t *)buffer;

#if defined(REV9E)
  if ((block[0] & 0xFFFC0000) != 0x10000000 && (block[0] & 0xFFFC0000) != 0x20000000) {
    return 0;
  }
  if ((block[1] & 0xFFF00000) != 0x08000000) {
    return 0;
  }
  if ((block[2] & 0xFFF00000) != 0x08000000) {
    return 0;
  }
#else
  if ((block[0] & 0xFFFC0000) != 0x20000000) {
    return 0;
  }
  if ((block[1] & 0xFFF00000) != 0x08000000) {
    return 0;
  }
  if ((block[2] & 0xFFF00000) != 0x08000000) {
    return 0;
  }
#endif
  return 1;
}

uint32_t isBootloaderStart(const void * buffer)
{
  const uint32_t * block = (const uint32_t *)buffer;

  for (int i=0; i<256; i++) {
    if (block[i] == 0x544F4F42/*BOOT*/) {
      return 1;
    }
  }
  return 0;
}
