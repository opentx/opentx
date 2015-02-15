/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include "opentx.h"

const pm_char * bmpLoad(uint8_t *bmp, const char *filename, const unsigned int width, const unsigned int height)
{
  FIL bmpFile;
  UINT read;
  uint8_t palette[16];
  uint8_t bmpBuf[LCD_W]; /* maximum with LCD_W */
  uint8_t *buf = &bmpBuf[0];

  if (width > LCD_W) {
    return STR_INCOMPATIBLE;
  }

  FRESULT result = f_open(&bmpFile, filename, FA_OPEN_EXISTING | FA_READ);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  if (f_size(&bmpFile) < 14) {
    f_close(&bmpFile);
    return STR_INCOMPATIBLE;
  }

  result = f_read(&bmpFile, buf, 14, &read);
  if (result != FR_OK || read != 14) {
    f_close(&bmpFile);
    return SDCARD_ERROR(result);
  }

  if (buf[0] != 'B' || buf[1] != 'M') {
    f_close(&bmpFile);
    return STR_INCOMPATIBLE;
  }

  uint32_t fsize  = *((uint32_t *)&buf[2]);
  uint32_t hsize  = *((uint32_t *)&buf[10]); /* header size */

  uint32_t len = limit((uint32_t)4, (uint32_t)(hsize-14), (uint32_t)32);
  result = f_read(&bmpFile, buf, len, &read);
  if (result != FR_OK || read != len) {
    f_close(&bmpFile);
    return SDCARD_ERROR(result);
  }

  uint32_t ihsize = *((uint32_t *)&buf[0]); /* more header size */

  /* invalid header size */
  if (ihsize + 14 > hsize) {
    f_close(&bmpFile);
    return STR_INCOMPATIBLE;
  }

  /* sometimes file size is set to some headers size, set a real size in that case */
  if (fsize == 14 || fsize == ihsize + 14)
    fsize = f_size(&bmpFile) - 2;

  /* declared file size less than header size */
  if (fsize <= hsize) {
    f_close(&bmpFile);
    return STR_INCOMPATIBLE;
  }

  uint32_t w, h;

  switch (ihsize){
    case  40: // windib
    case  56: // windib v3
    case  64: // OS/2 v2
    case 108: // windib v4
    case 124: // windib v5
      w  = *((uint32_t *)&buf[4]);
      h = *((uint32_t *)&buf[8]);
      buf += 12;
      break;
    case  12: // OS/2 v1
      w  = *((uint16_t *)&buf[4]);
      h = *((uint16_t *)&buf[6]);
      buf += 8;
      break;
    default:
      f_close(&bmpFile);
      return STR_INCOMPATIBLE;
  }

  if (*((uint16_t *)&buf[0]) != 1) { /* planes */
    f_close(&bmpFile);
    return STR_INCOMPATIBLE;
  }

  if (w > width || h > height) {
    f_close(&bmpFile);
    return STR_INCOMPATIBLE;
  }

  uint16_t depth = *((uint16_t *)&buf[2]);

  buf = &bmpBuf[0];

  if (depth == 4) {
    if (f_lseek(&bmpFile, hsize-64) != FR_OK || f_read(&bmpFile, buf, 64, &read) != FR_OK || read != 64) {
      f_close(&bmpFile);
      return SDCARD_ERROR(result);
    }
    for (uint8_t i=0; i<16; i++) {
      palette[i] = buf[4*i] >> 4;
    }
  }
  else {
    if (f_lseek(&bmpFile, hsize) != FR_OK) {
      f_close(&bmpFile);
      return SDCARD_ERROR(result);
    }
  }

  uint8_t *dest = bmp;

  *dest++ = w;
  *dest++ = h;

  memset(dest, 0, BITMAP_BUFFER_SIZE(w, h) - 2);

  uint32_t rowSize;

  switch (depth) {
    case 1:
      rowSize = ((w+31)/32)*4;
      for (uint32_t i=0; i<h; i+=2) {
        result = f_read(&bmpFile, buf, rowSize*2, &read);
        if (result != FR_OK || read != rowSize*2) {
          f_close(&bmpFile);
          return SDCARD_ERROR(result);
        }

        for (uint32_t j=0; j<w; j++) {
          uint8_t * dst = dest + (h-i-2)/2 * w + j;
          if (!(buf[j/8] & (1<<(7-(j%8)))))
            *dst |= 0xF0;
          if (!(buf[rowSize+j/8] & (1<<(7-(j%8)))))
            *dst |= 0x0F;
        }
      }
      break;

    case 4:
      rowSize = ((4*w+31)/32)*4;
      for (int32_t i=h-1; i>=0; i--) {
        result = f_read(&bmpFile, buf, rowSize, &read);
        if (result != FR_OK || read != rowSize) {
          f_close(&bmpFile);
          return SDCARD_ERROR(result);
        }
        uint8_t * dst = dest + (i/2)*w;
        for (uint32_t j=0; j<w; j++) {
          uint8_t index = (buf[j/2] >> ((j & 1) ? 0 : 4)) & 0x0F;
          uint8_t val = palette[index] << ((i & 1) ? 4 : 0);
          *dst++ |= val ^ ((i & 1) ? 0xF0 : 0x0F);
        }
      }
      break;

    default:
      f_close(&bmpFile);
      return STR_INCOMPATIBLE;
  }

  f_close(&bmpFile);
  return 0;
}

const uint8_t bmpHeader[] = {
  0x42, 0x4d, 0xF8, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00,
  0x00, 0x00, 212,  0x00, 0x00, 0x00, 64,   0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0xbc, 0x38, 0x00, 0x00, 0xbc, 0x38, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0xee, 0xee, 0xee, 0x00, 0xdd, 0xdd,
  0xdd, 0x00, 0xcc, 0xcc, 0xcc, 0x00, 0xbb, 0xbb, 0xbb, 0x00, 0xaa, 0xaa, 0xaa, 0x00, 0x99, 0x99,
  0x99, 0x00, 0x88, 0x88, 0x88, 0x00, 0x77, 0x77, 0x77, 0x00, 0x66, 0x66, 0x66, 0x00, 0x55, 0x55,
  0x55, 0x00, 0x44, 0x44, 0x44, 0x00, 0x33, 0x33, 0x33, 0x00, 0x22, 0x22, 0x22, 0x00, 0x11, 0x11,
  0x11, 0x00, 0x00, 0x00, 0x00, 0x00
};

inline display_t getPixel(unsigned int x, unsigned int y)
{
  if (x>=LCD_W || y>=LCD_H)
    return 0;
  display_t * p = &displayBuf[y / 2 * LCD_W + x];
  return (y & 1) ? (*p >> 4) : (*p & 0x0F);
}

const char *writeScreenshot()
{
  FIL bmpFile;
  UINT written;
  char filename[48]; // /SCREENSHOTS/screenshot-2013-01-01-12-35-40.bmp
  DIR folder;

  // check and create folder here
  strcpy_P(filename, SCREENSHOTS_PATH);
  FRESULT result = f_opendir(&folder, filename);
  if (result != FR_OK) {
    if (result == FR_NO_PATH)
      result = f_mkdir(filename);
    if (result != FR_OK)
      return SDCARD_ERROR(result);
  }

  char *tmp = strAppend(&filename[sizeof(SCREENSHOTS_PATH)-1], "/screenshot");
  tmp = strAppendDate(tmp, true);
  strcpy(tmp, BITMAPS_EXT);

  result = f_open(&bmpFile, filename, FA_CREATE_ALWAYS | FA_WRITE);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  result = f_write(&bmpFile, bmpHeader, sizeof(bmpHeader), &written);
  if (result != FR_OK || written != sizeof(bmpHeader)) {
    f_close(&bmpFile);
    return SDCARD_ERROR(result);
  }

  for (int y=LCD_H-1; y>=0; y-=1) {
    for (int x=0; x<8*((LCD_W+7)/8); x+=2) {
      uint8_t byte = getPixel(x+1, y) + (getPixel(x, y) << 4);
      f_write(&bmpFile, &byte, 1, &written);
      if (result != FR_OK || written != 1) {
        f_close(&bmpFile);
        return SDCARD_ERROR(result);
      }
    }
  }

  f_close(&bmpFile);

  return NULL;
}
