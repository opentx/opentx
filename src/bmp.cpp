/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
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

#include "open9x.h"

const pm_char * bmpLoad(uint8_t *dest, const char *filename)
{
  FIL bmpFile;
  UINT read;
  uint8_t bmpBuf[64]; /* maximum with 64px for 1-bit bitmaps */
  uint8_t *buf = &bmpBuf[0];

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

  uint32_t len = limit((uint32_t)0, (uint32_t)(hsize-14), (uint32_t)32);
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

  uint32_t width, height;

  switch (ihsize){
    case  40: // windib
    case  56: // windib v3
    case  64: // OS/2 v2
    case 108: // windib v4
    case 124: // windib v5
      width  = *((uint32_t *)&buf[4]);
      height = *((uint32_t *)&buf[8]);
      buf += 12;
      break;
    case  12: // OS/2 v1
      width  = *((uint16_t *)&buf[4]);
      height = *((uint16_t *)&buf[6]);
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

  if (width > MODEL_BITMAP_WIDTH || height > MODEL_BITMAP_HEIGHT) {
    f_close(&bmpFile);
    return STR_INCOMPATIBLE;
  }

  uint16_t depth = *((uint16_t *)&buf[2]);

  if (f_lseek(&bmpFile, hsize) != FR_OK) {
    f_close(&bmpFile);
    return SDCARD_ERROR(result);
  }

  *dest++ = width;
  *dest++ = height;

  uint32_t n;
  buf = &bmpBuf[0];

  switch (depth) {
    case 1:
      n = width/8;
      for (uint32_t i=0; i<height; i+=8) {
        result = f_read(&bmpFile, buf, n*8, &read);
        if (result != FR_OK || read != n*8) {
          f_close(&bmpFile);
          return SDCARD_ERROR(result);
        }
        uint8_t * dst = dest + ((height-i-8)/8 * width);
        for (uint32_t j=0; j<n; j++) {
          *dst++ = ~(((buf[j+0*n] >> 7) << 7) + ((buf[j+1*n] >> 7) << 6) + ((buf[j+2*n] >> 7) << 5) + ((buf[j+3*n] >> 7) << 4) + ((buf[j+4*n] >> 7) << 3) + ((buf[j+5*n] >> 7) << 2) + ((buf[j+6*n] >> 7) << 1) + ((buf[j+7*n] >> 7) << 0));
          *dst++ = ~((((buf[j+0*n] >> 6) & 1) << 7) + (((buf[j+1*n] >> 6) & 1) << 6) + (((buf[j+2*n] >> 6) & 1) << 5) + (((buf[j+3*n] >> 6) & 1) << 4) + (((buf[j+4*n] >> 6) & 1) << 3) + (((buf[j+5*n] >> 6) & 1) << 2) + (((buf[j+6*n] >> 6) & 1) << 1) + (((buf[j+7*n] >> 6) & 1) << 0));
          *dst++ = ~((((buf[j+0*n] >> 5) & 1) << 7) + (((buf[j+1*n] >> 5) & 1) << 6) + (((buf[j+2*n] >> 5) & 1) << 5) + (((buf[j+3*n] >> 5) & 1) << 4) + (((buf[j+4*n] >> 5) & 1) << 3) + (((buf[j+5*n] >> 5) & 1) << 2) + (((buf[j+6*n] >> 5) & 1) << 1) + (((buf[j+7*n] >> 5) & 1) << 0));
          *dst++ = ~((((buf[j+0*n] >> 4) & 1) << 7) + (((buf[j+1*n] >> 4) & 1) << 6) + (((buf[j+2*n] >> 4) & 1) << 5) + (((buf[j+3*n] >> 4) & 1) << 4) + (((buf[j+4*n] >> 4) & 1) << 3) + (((buf[j+5*n] >> 4) & 1) << 2) + (((buf[j+6*n] >> 4) & 1) << 1) + (((buf[j+7*n] >> 4) & 1) << 0));
          *dst++ = ~((((buf[j+0*n] >> 3) & 1) << 7) + (((buf[j+1*n] >> 3) & 1) << 6) + (((buf[j+2*n] >> 3) & 1) << 5) + (((buf[j+3*n] >> 3) & 1) << 4) + (((buf[j+4*n] >> 3) & 1) << 3) + (((buf[j+5*n] >> 3) & 1) << 2) + (((buf[j+6*n] >> 3) & 1) << 1) + (((buf[j+7*n] >> 3) & 1) << 0));
          *dst++ = ~((((buf[j+0*n] >> 2) & 1) << 7) + (((buf[j+1*n] >> 2) & 1) << 6) + (((buf[j+2*n] >> 2) & 1) << 5) + (((buf[j+3*n] >> 2) & 1) << 4) + (((buf[j+4*n] >> 2) & 1) << 3) + (((buf[j+5*n] >> 2) & 1) << 2) + (((buf[j+6*n] >> 2) & 1) << 1) + (((buf[j+7*n] >> 2) & 1) << 0));
          *dst++ = ~((((buf[j+0*n] >> 1) & 1) << 7) + (((buf[j+1*n] >> 1) & 1) << 6) + (((buf[j+2*n] >> 1) & 1) << 5) + (((buf[j+3*n] >> 1) & 1) << 4) + (((buf[j+4*n] >> 1) & 1) << 3) + (((buf[j+5*n] >> 1) & 1) << 2) + (((buf[j+6*n] >> 1) & 1) << 1) + (((buf[j+7*n] >> 1) & 1) << 0));
          *dst++ = ~((((buf[j+0*n] >> 0) & 1) << 7) + (((buf[j+1*n] >> 0) & 1) << 6) + (((buf[j+2*n] >> 0) & 1) << 5) + (((buf[j+3*n] >> 0) & 1) << 4) + (((buf[j+4*n] >> 0) & 1) << 3) + (((buf[j+5*n] >> 0) & 1) << 2) + (((buf[j+6*n] >> 0) & 1) << 1) + (((buf[j+7*n] >> 0) & 1) << 0));
        }
      }
      break;

    /* case 4:
      for (uint32_t i=0; i<height; i++) {
        int j;
        for (j = 0; j < n; j++){
          ptr[j*2+0] = (buf[j] >> 4) & 0xF;
          ptr[j*2+1] = buf[j] & 0xF;
        }
        buf += n;
        ptr += linesize;
      }
      break;
    */

    default:
      f_close(&bmpFile);
      return STR_INCOMPATIBLE;
  }

  f_close(&bmpFile);
  return 0;
}
