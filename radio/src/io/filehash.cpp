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
#include "../thirdparty/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/stm32f2xx_hash.h"

#define SHA1BUSY_TIMEOUT    ((uint32_t) 0x00010000)



ErrorStatus HASH_SHA1(uint8_t *Input, uint32_t Ilen, uint8_t Output[20])
{
  HASH_InitTypeDef SHA1_HASH_InitStructure;
  HASH_MsgDigest SHA1_MessageDigest;
  __IO uint16_t nbvalidbitsdata = 0;
  uint32_t i = 0;
  __IO uint32_t counter = 0;
  uint32_t busystatus = 0;
  ErrorStatus status = SUCCESS;
  uint32_t inputaddr  = (uint32_t)Input;
  uint32_t outputaddr = (uint32_t)Output;

  /* Number of valid bits in last word of the Input data */
  nbvalidbitsdata = 8 * (Ilen % 4);

  /* HASH peripheral initialization */
  HASH_DeInit();

  /* HASH Configuration */
  SHA1_HASH_InitStructure.HASH_AlgoSelection = HASH_AlgoSelection_SHA1;
  SHA1_HASH_InitStructure.HASH_AlgoMode = HASH_AlgoMode_HASH;
  SHA1_HASH_InitStructure.HASH_DataType = HASH_DataType_8b;
  HASH_Init(&SHA1_HASH_InitStructure);

  /* Configure the number of valid bits in last word of the data */
  HASH_SetLastWordValidBitsNbr(nbvalidbitsdata);

  /* Write the Input block in the IN FIFO */
  for(i=0; i<Ilen; i+=4)
  {
    HASH_DataIn(*(uint32_t*)inputaddr);
    inputaddr+=4;
  }

  /* Start the HASH processor */
  HASH_StartDigest();

  /* wait until the Busy flag is RESET */
  do
  {
    busystatus = HASH_GetFlagStatus(HASH_FLAG_BUSY);
    counter++;
  }while ((counter != SHA1BUSY_TIMEOUT) && (busystatus != RESET));

  if (busystatus != RESET)
  {
    status = ERROR;
  }
  else
  {
    /* Read the message digest */
    HASH_GetDigest(&SHA1_MessageDigest);
    *(uint32_t*)(outputaddr)  = __REV(SHA1_MessageDigest.Data[0]);
    outputaddr+=4;
    *(uint32_t*)(outputaddr)  = __REV(SHA1_MessageDigest.Data[1]);
    outputaddr+=4;
    *(uint32_t*)(outputaddr)  = __REV(SHA1_MessageDigest.Data[2]);
    outputaddr+=4;
    *(uint32_t*)(outputaddr)  = __REV(SHA1_MessageDigest.Data[3]);
    outputaddr+=4;
    *(uint32_t*)(outputaddr)  = __REV(SHA1_MessageDigest.Data[4]);
  }
  return status;
}


// Adapted from stm32f2xx_hash_sha1.c

void calculateSha1File(const char *filename)
{
  FIL file;
  f_open(&file, filename, FA_READ);
  uint8_t buffer[1024];
  UINT count;
  FSIZE_t size = f_size(&file);

  /* Number of valid bits in last word of the Input data */
  int nbvalidbitsdata = 8 * (size % 4);

  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_HASH, ENABLE);

  /* HASH peripheral initialization */
  HASH_DeInit();

  /* HASH Configuration */
  HASH_InitTypeDef SHA1_HASH_InitStructure;
  SHA1_HASH_InitStructure.HASH_AlgoSelection = HASH_AlgoSelection_SHA1;
  SHA1_HASH_InitStructure.HASH_AlgoMode = HASH_AlgoMode_HASH;
  SHA1_HASH_InitStructure.HASH_DataType = HASH_DataType_8b;
  HASH_Init(&SHA1_HASH_InitStructure);

  /* Configure the number of valid bits in last word of the data */
  HASH_SetLastWordValidBitsNbr(nbvalidbitsdata);

  FSIZE_t bytesRead=0;
  while (true) {
    //watchdogSuspend(100/*1s*/);
    if (f_read(&file, buffer, 1024, &count) != FR_OK) {
      POPUP_WARNING(STR_SDCARD_ERROR);
      break;
    }
    bytesRead+=count;

    TRACE("count %d, bytesRead %d, size %d\r\n", count, bytesRead, size);

    /* File completely read */
    if (count == 0) {
      /* Start the HASH processor */
      HASH_StartDigest();

      /* wait until the Busy flag is RESET */
      uint32_t busystatus;
      uint32_t counter=0;

      do {
        busystatus = HASH_GetFlagStatus(HASH_FLAG_BUSY);
        counter++;
      } while ((counter != SHA1BUSY_TIMEOUT) && (busystatus != RESET));

      if (busystatus != RESET) {
        POPUP_WARNING("SHA1 Hash engine failure");
        break;
      }
      HASH_MsgDigest digest;
      HASH_GetDigest(&digest);

      char w[40];

      // 20 byte hex
      for (int i=0; i < 160/4;i++) {
        uint8_t b = (digest.Data[i / 8] >> (i % 8*4)) & 0xf;
        w[i] = b>9 ? b+'A'-10 : b+'0';

      }

      POPUP_CONFIRMATION("Hash");
      SET_WARNING_INFO(w, strlen(w), 0);

      break;
    }

    for (unsigned int j = 0; j < count; j += 4) {
      /* Write the Input block in the IN FIFO */
      HASH_DataIn(*(uint32_t *) (buffer+j));
    }
    drawProgressBar(STR_CALCULATING, bytesRead, size);
  }

  f_close(&file);
}