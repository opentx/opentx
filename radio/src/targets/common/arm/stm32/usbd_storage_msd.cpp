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

/* Includes ------------------------------------------------------------------*/
#include "opentx.h"
#include "FatFs/diskio.h"

#if defined(__cplusplus) && !defined(SIMU)
extern "C" {
#endif

#include "usbd_msc_mem.h"
#include "usb_conf.h"

enum MassstorageLuns {
  STORAGE_SDCARD_LUN,
  STORAGE_EEPROM_LUN,
  STORAGE_LUN_NBR
};

/* USB Mass storage Standard Inquiry Data */
const unsigned char STORAGE_Inquirydata[] = { //36
  /* LUN 0 */
  0x00,		
  0x80,		
  0x02,		
  0x02,
  (USBD_STD_INQUIRY_LENGTH - 5),
  0x00,
  0x00,	
  0x00,
  USB_MANUFACTURER,                        /* Manufacturer : 8 bytes */
  USB_PRODUCT,                             /* Product      : 16 Bytes */
  'R', 'a', 'd', 'i', 'o', ' ', ' ', ' ',
  '1', '.', '0', '0',                      /* Version      : 4 Bytes */
  /* LUN 1 */
  0x00,		
  0x80,		
  0x02,		
  0x02,
  (USBD_STD_INQUIRY_LENGTH - 5),
  0x00,
  0x00,	
  0x00,
  USB_MANUFACTURER,                        /* Manufacturer : 8 bytes */
  USB_PRODUCT,                             /* Product      : 16 Bytes */
  'R', 'a', 'd', 'i', 'o', ' ', ' ', ' ',
  '1', '.', '0' ,'0',                      /* Version      : 4 Bytes */
};

int32_t fat12Write(const uint8_t * buffer, uint16_t sector, uint16_t count);
int32_t fat12Read(uint8_t * buffer, uint16_t sector, uint16_t count );

int8_t STORAGE_Init (uint8_t lun);

int8_t STORAGE_GetCapacity (uint8_t lun, 
                           uint32_t *block_num, 
                           uint32_t *block_size);

int8_t  STORAGE_IsReady (uint8_t lun);

int8_t  STORAGE_IsWriteProtected (uint8_t lun);

int8_t STORAGE_Read (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,
                        uint16_t blk_len);

int8_t STORAGE_Write (uint8_t lun, 
                        uint8_t *buf, 
                        uint32_t blk_addr,
                        uint16_t blk_len);

int8_t STORAGE_GetMaxLun (void);

const USBD_STORAGE_cb_TypeDef USBD_MICRO_SDIO_fops =    // modified my OpenTX
{
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  (int8_t *)STORAGE_Inquirydata,
};

const USBD_STORAGE_cb_TypeDef  * const USBD_STORAGE_fops = &USBD_MICRO_SDIO_fops;    // modified my OpenTX

#if defined(__cplusplus) && !defined(SIMU)
}
#endif

int8_t STORAGE_Init (uint8_t lun)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

/* TODO if no SD ... if( SD_Init() != 0)
  {
    return (-1); 
  } 
*/
  return (0);
}

/**
  * @brief  return medium capacity and block size
  * @param  lun : logical unit number
  * @param  block_num :  number of physical block
  * @param  block_size : size of a physical block
  * @retval Status
  */
int8_t STORAGE_GetCapacity (uint8_t lun, uint32_t *block_num, uint32_t *block_size)
{
  if (lun == STORAGE_EEPROM_LUN) {
    *block_size = BLOCK_SIZE;
#if defined(EEPROM)
    *block_num  = 3 + EEPROM_SIZE/BLOCK_SIZE + FLASHSIZE/BLOCK_SIZE;
#else
    *block_num  = 3 + FLASHSIZE/BLOCK_SIZE;
#endif
    return 0;
  }

  if (!SD_CARD_PRESENT())
    return -1;
  
  *block_size = BLOCK_SIZE;

  static DWORD sector_count = 0;
  if (sector_count == 0) {
    if (disk_ioctl(0, GET_SECTOR_COUNT, &sector_count) != RES_OK) {
      sector_count = 0;
      return -1;
    }
  }

  *block_num  = sector_count;

  return 0;
}

uint8_t lunReady[STORAGE_LUN_NBR];

void usbPluggedIn()
{
  lunReady[STORAGE_SDCARD_LUN] = 1;
  lunReady[STORAGE_EEPROM_LUN] = 1;
}

/**
  * @brief  check whether the medium is ready
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t  STORAGE_IsReady (uint8_t lun)
{ 
#if defined(EEPROM)
  if (lun == STORAGE_EEPROM_LUN) {
    return (lunReady[STORAGE_EEPROM_LUN] != 0) ? 0 : -1;
  }
#endif

  return (lunReady[STORAGE_SDCARD_LUN] != 0 && SD_CARD_PRESENT()) ? 0 : -1;
}

/**
  * @brief  check whether the medium is write-protected
  * @param  lun : logical unit number
  * @retval Status
  */
int8_t  STORAGE_IsWriteProtected (uint8_t lun)
{
  return  0;
}

/**
  * @brief  Read data from the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to save data
  * @param  blk_addr :  address of 1st block to be read
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */

int8_t STORAGE_Read (uint8_t lun, 
                 uint8_t *buf, 
                 uint32_t blk_addr,                       
                 uint16_t blk_len)
{
  WATCHDOG_SUSPEND(100/*1s*/);
  
  if (lun == STORAGE_EEPROM_LUN) {
    return (fat12Read(buf, blk_addr, blk_len) == 0) ? 0 : -1;
  }

  // read without cache
  return (__disk_read(0, buf, blk_addr, blk_len) == RES_OK) ? 0 : -1;
}
/**
  * @brief  Write data to the medium
  * @param  lun : logical unit number
  * @param  buf : Pointer to the buffer to write from
  * @param  blk_addr :  address of 1st block to be written
  * @param  blk_len : nmber of blocks to be read
  * @retval Status
  */

int8_t STORAGE_Write (uint8_t lun, 
                  uint8_t *buf, 
                  uint32_t blk_addr,
                  uint16_t blk_len)
{
  WATCHDOG_SUSPEND(100/*1s*/);
  
  if (lun == STORAGE_EEPROM_LUN)	{
    return (fat12Write(buf, blk_addr, blk_len) == 0) ? 0 : -1;
  }

  // write without cache
  return (__disk_write(0, buf, blk_addr, blk_len) == RES_OK) ? 0 : -1;
}

/**
  * @brief  Return number of supported logical unit
  * @param  None
  * @retval number of logical unit
  */

int8_t STORAGE_GetMaxLun (void)
{
  return STORAGE_LUN_NBR - 1;
}

//------------------------------------------------------------------------------
/**
 * FAT12 boot sector partition.
 */
#if defined(EEPROM)
#define TOTALSECTORS  (3+ (EEPROM_SIZE/BLOCK_SIZE) +  (FLASHSIZE/BLOCK_SIZE))
#else
#define TOTALSECTORS  (3 + (FLASHSIZE/BLOCK_SIZE))
#endif
const char g_FATboot[BLOCK_SIZE] =
{
  0xeb, 0x3c, 0x90, // Jump instruction.
  0x39, 0x58, 0x20, 0x54, 0x45, 0x41, 0x4D, 0x00, // OEM Name
  0x00, 0x02, // Bytes per sector
  0x08, // Sectors per FS cluster.
  0x01, 0x00, // Reserved sector count

  0x01, // Number of FATs
  0x10, 0x00, // Number of root directory entries
  TOTALSECTORS & 0x00ff,  (TOTALSECTORS & 0xff00) >> 8, // Total sectors
  0xf8, // Media descriptor
  0x01, 0x00, // Sectors per FAT table
  0x20, 0x00, // Sectors per track
  0x40, 0x00, // Number of heads
  0x00, 0x00, 0x00, 0x00, // Number of hidden sectors

  0x00, 0x00, 0x00, 0x00, // Large number of sectors.
  0x00, // Physical drive number
  0x00, // Reserved
  0x29, // Extended boot signature
  0xCE, 0xFA, 0x5C, 0xD1, // Disk ID (serial number)
  0x56, 0x49, 0x52, 0x54, 0x46, 0x53, 0x20, 0x20, 0x20, 0x20, 0x20, // Volume label
  0x46, 0x41, 0x54, 0x31, 0x32, 0x20, 0x20, 0x20, // FAT file system type
  0x00, 0x00, // OS boot code

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xaa
};


// TODO: Rewrite FAT to make sense
struct FATEntry_t
{

};

const char g_FAT[BLOCK_SIZE] =
  {
    0xF8, 0xFF, 0xFF, 0x03, 0x40, 0x00, 0x05, 0x60, 0x00, 0x07, 0x80, 0x00, 0x09, 0xF0, 0xFF, 0x0B,
    0xC0, 0x00, 0x0D, 0xE0, 0x00, 0x0F, 0x00, 0x01, 0x11, 0x20, 0x01, 0x13, 0x40, 0x01, 0x15, 0x60,
    0x01, 0x17, 0x80, 0x01, 0x19, 0xA0, 0x01, 0x1B, 0xC0, 0x01, 0x1D, 0xE0, 0x01, 0x1F, 0x00, 0x02,
    0x21, 0x20, 0x02, 0x23, 0x40, 0x02, 0x25, 0x60, 0x02, 0x27, 0x80, 0x02, 0x29, 0xA0, 0x02, 0x2B,
    0xC0, 0x02, 0x2D, 0xE0, 0x02, 0x2F, 0x00, 0x03, 0x31, 0x20, 0x03, 0x33, 0x40, 0x03, 0x35, 0x60,
    0x03, 0x37, 0x80, 0x03, 0x39, 0xA0, 0x03, 0x3B, 0xC0, 0x03, 0x3D, 0xE0, 0x03, 0x3F, 0x00, 0x04,
    0x41, 0x20, 0x04, 0x43, 0x40, 0x04, 0x45, 0x60, 0x04, 0x47, 0x80, 0x04, 0x49, 0xA0, 0x04, 0x4B,
    0xC0, 0x04, 0x4D, 0xE0, 0x04, 0x4F, 0x00, 0x05, 0x51, 0x20, 0x05, 0x53, 0x40, 0x05, 0x55, 0x60,
    0x05, 0x57, 0x80, 0x05, 0x59, 0xA0, 0x05, 0x5B, 0xC0, 0x05, 0x5D, 0xE0, 0x05, 0x5F, 0x00, 0x06,
    0x61, 0x20, 0x06, 0x63, 0x40, 0x06, 0x65, 0x60, 0x06, 0x67, 0x80, 0x06, 0x69, 0xA0, 0x06, 0x6B,
    0xC0, 0x06, 0x6D, 0xE0, 0x06, 0x6F, 0x00, 0x07, 0x71, 0x20, 0x07, 0x73, 0x40, 0x07, 0x75, 0x60,
    0x07, 0x77, 0x80, 0x07, 0x79, 0xA0, 0x07, 0x7B, 0xC0, 0x07, 0x7D, 0xE0, 0x07, 0x7F, 0x00, 0x08,
    0x81, 0x20, 0x08, 0x83, 0x40, 0x08, 0x85, 0x60, 0x08, 0x87, 0x80, 0x08, 0x89, 0xF0, 0xFF, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  };

//	File Attributes
//	0 	0x01 	Read Only.
//	1 	0x02 	Hidden.
//	2 	0x04 	System.
//	3 	0x08 	Volume Label.
//	4 	0x10 	Subdirectory.
//	5 	0x20 	Archive.
//	6 	0x40 	Device.
//	7 	0x80 	Reserved.

typedef struct
{
    uint8_t name[8];
    uint8_t ext[3];
    uint8_t attribute;
    uint8_t reserved;
    uint8_t create_time_ms;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t access_date;
    uint16_t ea_index;
    uint16_t modify_time;
    uint16_t modify_date;
    uint16_t start_cluster;
    uint32_t file_size;
} FATDirEntry_t;

// First 16 FAT root directory entries (1 sector)
const FATDirEntry_t g_DIRroot[16] =
{
    {
        { USB_PRODUCT },
        { ' ', ' ', ' '},
        0x08,		// Volume
        0x00,
        0x00,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x00000000
    },
    {
      { 'F', 'I', 'R', 'M', 'W', 'A', 'R', 'E'},
      { 'B', 'I', 'N'},
#if defined(BOOT)
    0x20,          // Archive
#else
      0x21,          // Readonly+Archive
#endif
      0x00,
      0x3E,
      0xA301,
      0x3D55,
      0x3D55,
      0x0000,
      0xA302,
      0x3D55,
      0x0002,
      FLASHSIZE
  },
#if defined(EEPROM)
    {
        { 'E', 'E', 'P', 'R', 'O', 'M', ' ', ' '},
        { 'B', 'I', 'N'},
        0x20,		// Archive
        0x00,
        0x3E,
        0xA301,
        0x3D55,
        0x3D55,
        0x0000,
        0xA302,
        0x3D55,
        0x0002 + (FLASHSIZE/BLOCK_SIZE)/8,
        EEPROM_SIZE
    },
#else
    {
      { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
      { ' ', ' ', ' '},
      0x00,
      0x00,
      0x00,
      0x0000,
      0x0000,
      0x0000,
      0x0000,
      0x0000,
      0x0000,
      0x0000,
      0x00000000
    },
#endif
    {
        { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        { ' ', ' ', ' '},
        0x00,
        0x00,
        0x00,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x00000000
    },
    {
        { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        { ' ', ' ', ' '},
        0x00,
        0x00,
        0x00,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x00000000
    },
    {
        { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        { ' ', ' ', ' '},
        0x00,
        0x00,
        0x00,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x00000000
    },
    {
        { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        { ' ', ' ', ' '},
        0x00,
        0x00,
        0x00,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x00000000
    },
    {
        { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        { ' ', ' ', ' '},
        0x00,
        0x00,
        0x00,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x00000000
    },
    {
        { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        { ' ', ' ', ' '},
        0x00,
        0x00,
        0x00,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x00000000
    },
    {
        { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        { ' ', ' ', ' '},
        0x00,
        0x00,
        0x00,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x00000000
    },
    {
        { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        { ' ', ' ', ' '},
        0x00,
        0x00,
        0x00,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x00000000
    },
    {
        { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        { ' ', ' ', ' '},
        0x00,
        0x00,
        0x00,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x00000000
    },
    {
        { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        { ' ', ' ', ' '},
        0x00,
        0x00,
        0x00,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x00000000
    },
    {
        { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        { ' ', ' ', ' '},
        0x00,
        0x00,
        0x00,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x00000000
    },
    {
        { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        { ' ', ' ', ' '},
        0x00,
        0x00,
        0x00,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x00000000
    },
    {
        { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        { ' ', ' ', ' '},
        0x00,
        0x00,
        0x00,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x00000000
    },
};

// count is number of 512 byte sectors
int32_t fat12Read(uint8_t * buffer, uint16_t sector, uint16_t count)
{
  while(count) {
    if (sector == 0) {
      memcpy(buffer, g_FATboot, BLOCK_SIZE ) ;
    }
    else if (sector == 1/*Reserved sector count*/) {
      // FAT table.
      memcpy(buffer, g_FAT, BLOCK_SIZE);
    }
    else if (sector == 2) {
      memcpy(buffer, g_DIRroot, BLOCK_SIZE ) ;
    }

    else if (sector < 3 + (FLASHSIZE/BLOCK_SIZE )) {
      uint32_t address;
      address = sector - 3;
      address *= BLOCK_SIZE;
      address += FIRMWARE_ADDRESS;
      memcpy(buffer, (uint8_t *)address, BLOCK_SIZE);
    }
#if defined(EEPROM)
    else if (sector < 3 + (EEPROM_SIZE/BLOCK_SIZE) + (FLASHSIZE/BLOCK_SIZE)) {
      eepromReadBlock(buffer, (sector - 3 - (FLASHSIZE/BLOCK_SIZE))*BLOCK_SIZE, BLOCK_SIZE);
    }
#endif
    buffer += BLOCK_SIZE ;
    sector++ ;
    count-- ;
  }
  return 0 ;
}

int32_t fat12Write(const uint8_t * buffer, uint16_t sector, uint16_t count)
{
  enum FatWriteOperation {
    FATWRITE_NONE,
    FATWRITE_EEPROM,
    FATWRITE_FIRMWARE
  };

  static uint8_t operation = FATWRITE_NONE;

  TRACE("FAT12 Write(sector=%d, count=%d)", sector, count);

  if (sector < 3) {
    // reserved, read-only
  }
  else if (sector < 3 + (FLASHSIZE/BLOCK_SIZE)) {
#if !defined(BOOT) // Don't allow overwrite of running firmware
    return -1;
#else
    // firmware
    uint32_t address;
    address = sector - 3;
    address *= BLOCK_SIZE;
    address += FIRMWARE_ADDRESS;
    while (count) {
      for (uint32_t i=0; i<BLOCK_SIZE/FLASH_PAGESIZE; i++) {
        if (address >= FIRMWARE_ADDRESS+BOOTLOADER_SIZE/*protect bootloader*/ && address <= FIRMWARE_ADDRESS+FLASHSIZE-FLASH_PAGESIZE) {
          if (address == FIRMWARE_ADDRESS+BOOTLOADER_SIZE && isFirmwareStart(buffer)) {
            TRACE("FIRMWARE start found in sector %d", sector);
            operation = FATWRITE_FIRMWARE;
          }
          if (operation == FATWRITE_FIRMWARE) {
            flashWrite((uint32_t *)address, (uint32_t *)buffer);
          }
        }
        address += FLASH_PAGESIZE;
        buffer += FLASH_PAGESIZE;
      }
      sector++;
      count--;
      if (sector-3 >= (FLASHSIZE/BLOCK_SIZE)) {
        TRACE("FIRMWARE end written at sector %d", sector-1);
        operation = FATWRITE_NONE;
      }
    }
#endif
  }
#if defined(EEPROM)
  else if (sector < 3 + (EEPROM_SIZE/BLOCK_SIZE) + (FLASHSIZE/BLOCK_SIZE)) {
    // eeprom
    while (count) {
      if (operation == FATWRITE_NONE && isEepromStart(buffer)) {
        TRACE("EEPROM start found in sector %d", sector);
        operation = FATWRITE_EEPROM;
      }
      if (operation == FATWRITE_EEPROM) {
        eepromWriteBlock((uint8_t *)buffer, (sector-3-(FLASHSIZE/BLOCK_SIZE))*BLOCK_SIZE, BLOCK_SIZE);
      }
      buffer += BLOCK_SIZE;
      sector++;
      count--;
      if (sector-3 >= (EEPROM_SIZE/BLOCK_SIZE)+(FLASHSIZE/BLOCK_SIZE)) {
        TRACE("EEPROM end written at sector %d", sector-1);
        operation = FATWRITE_NONE;
      }
    }
  }
#endif
  return 0 ;
}