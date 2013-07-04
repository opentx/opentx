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

const pm_char * bmpLoad(uint8_t *dest, const char *filename, const xcoord_t width, const uint8_t height)
{
  FIL bmpFile;
  UINT read;
  uint8_t palette[16];
  uint8_t bmpBuf[LCD_W]; /* maximum with LCD_W */
  uint8_t *buf = &bmpBuf[0];

  assert(width <= LCD_W);

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

  *dest++ = w;
  *dest++ = h;

  memset(dest, 0, w*(h/8)*4);

  uint32_t n;

  switch (depth) {
    case 1:
      n = w/8;
      for (uint32_t i=0; i<h; i+=8) {
        result = f_read(&bmpFile, buf, n*8, &read);
        if (result != FR_OK || read != n*8) {
          f_close(&bmpFile);
          return SDCARD_ERROR(result);
        }
        uint8_t * dst = dest + 4*((h-i-8)/8 * w);
        for (uint32_t j=0; j<n; j++) {
#define PUSH_4BYTES(x) *dst = *(dst+1) = *(dst+2) = *(dst+3) = (x); dst += 4
          PUSH_4BYTES(~(((buf[j+0*n] >> 7) << 7) + ((buf[j+1*n] >> 7) << 6) + ((buf[j+2*n] >> 7) << 5) + ((buf[j+3*n] >> 7) << 4) + ((buf[j+4*n] >> 7) << 3) + ((buf[j+5*n] >> 7) << 2) + ((buf[j+6*n] >> 7) << 1) + ((buf[j+7*n] >> 7) << 0)));
          PUSH_4BYTES(~((((buf[j+0*n] >> 6) & 1) << 7) + (((buf[j+1*n] >> 6) & 1) << 6) + (((buf[j+2*n] >> 6) & 1) << 5) + (((buf[j+3*n] >> 6) & 1) << 4) + (((buf[j+4*n] >> 6) & 1) << 3) + (((buf[j+5*n] >> 6) & 1) << 2) + (((buf[j+6*n] >> 6) & 1) << 1) + (((buf[j+7*n] >> 6) & 1) << 0)));
          PUSH_4BYTES(~((((buf[j+0*n] >> 5) & 1) << 7) + (((buf[j+1*n] >> 5) & 1) << 6) + (((buf[j+2*n] >> 5) & 1) << 5) + (((buf[j+3*n] >> 5) & 1) << 4) + (((buf[j+4*n] >> 5) & 1) << 3) + (((buf[j+5*n] >> 5) & 1) << 2) + (((buf[j+6*n] >> 5) & 1) << 1) + (((buf[j+7*n] >> 5) & 1) << 0)));
          PUSH_4BYTES(~((((buf[j+0*n] >> 4) & 1) << 7) + (((buf[j+1*n] >> 4) & 1) << 6) + (((buf[j+2*n] >> 4) & 1) << 5) + (((buf[j+3*n] >> 4) & 1) << 4) + (((buf[j+4*n] >> 4) & 1) << 3) + (((buf[j+5*n] >> 4) & 1) << 2) + (((buf[j+6*n] >> 4) & 1) << 1) + (((buf[j+7*n] >> 4) & 1) << 0)));
          PUSH_4BYTES(~((((buf[j+0*n] >> 3) & 1) << 7) + (((buf[j+1*n] >> 3) & 1) << 6) + (((buf[j+2*n] >> 3) & 1) << 5) + (((buf[j+3*n] >> 3) & 1) << 4) + (((buf[j+4*n] >> 3) & 1) << 3) + (((buf[j+5*n] >> 3) & 1) << 2) + (((buf[j+6*n] >> 3) & 1) << 1) + (((buf[j+7*n] >> 3) & 1) << 0)));
          PUSH_4BYTES(~((((buf[j+0*n] >> 2) & 1) << 7) + (((buf[j+1*n] >> 2) & 1) << 6) + (((buf[j+2*n] >> 2) & 1) << 5) + (((buf[j+3*n] >> 2) & 1) << 4) + (((buf[j+4*n] >> 2) & 1) << 3) + (((buf[j+5*n] >> 2) & 1) << 2) + (((buf[j+6*n] >> 2) & 1) << 1) + (((buf[j+7*n] >> 2) & 1) << 0)));
          PUSH_4BYTES(~((((buf[j+0*n] >> 1) & 1) << 7) + (((buf[j+1*n] >> 1) & 1) << 6) + (((buf[j+2*n] >> 1) & 1) << 5) + (((buf[j+3*n] >> 1) & 1) << 4) + (((buf[j+4*n] >> 1) & 1) << 3) + (((buf[j+5*n] >> 1) & 1) << 2) + (((buf[j+6*n] >> 1) & 1) << 1) + (((buf[j+7*n] >> 1) & 1) << 0)));
          PUSH_4BYTES(~((((buf[j+0*n] >> 0) & 1) << 7) + (((buf[j+1*n] >> 0) & 1) << 6) + (((buf[j+2*n] >> 0) & 1) << 5) + (((buf[j+3*n] >> 0) & 1) << 4) + (((buf[j+4*n] >> 0) & 1) << 3) + (((buf[j+5*n] >> 0) & 1) << 2) + (((buf[j+6*n] >> 0) & 1) << 1) + (((buf[j+7*n] >> 0) & 1) << 0)));
        }
      }
      break;

    case 4:
      n = w/8;
      for (int32_t i=h-1; i>=0; i--) {
        result = f_read(&bmpFile, buf, w/2, &read);
        if (result != FR_OK || read != w/2) {
          f_close(&bmpFile);
          return SDCARD_ERROR(result);
        }
        uint8_t mask = 1 << (i%8);
        for (uint32_t j=0; j<w/2; j++) {
          uint8_t * dst = dest + (i/8)*w*4 + j*8;
          uint8_t val = palette[(buf[j] >> 4) & 0x0F];
          *(dst+0) = ((*(dst+0)) & (~mask)) + ((val & 0x1) ? 0 : mask);
          *(dst+1) = ((*(dst+1)) & (~mask)) + ((val & 0x2) ? 0 : mask);
          *(dst+2) = ((*(dst+2)) & (~mask)) + ((val & 0x4) ? 0 : mask);
          *(dst+3) = ((*(dst+3)) & (~mask)) + ((val & 0x8) ? 0 : mask);
          val = palette[buf[j] & 0x0F];
          *(dst+4) = ((*(dst+4)) & (~mask)) + ((val & 0x1) ? 0 : mask);
          *(dst+5) = ((*(dst+5)) & (~mask)) + ((val & 0x2) ? 0 : mask);
          *(dst+6) = ((*(dst+6)) & (~mask)) + ((val & 0x4) ? 0 : mask);
          *(dst+7) = ((*(dst+7)) & (~mask)) + ((val & 0x8) ? 0 : mask);
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

void lcd_bmp(xcoord_t x, uint8_t y, const pm_uchar * img, uint8_t offset, uint8_t width)
{
  const pm_uchar *q = img;
#if LCD_W >= 260
  xcoord_t w   = pgm_read_byte(q++);
  if (w == 255) w += pgm_read_byte(q++);
#else
  uint8_t w    = pgm_read_byte(q++);
#endif
  if (!width) width = w;
  uint8_t hb   = (pgm_read_byte(q++)+7)/8;
  q += 4*offset;
  for (uint8_t yb = 0; yb < hb; yb++) {
    uint8_t *p = &displayBuf[ (y / 8 + yb) * LCD_W + x ];
    for (xcoord_t i=0; i<width; i++) {
      for (uint8_t plan=0; plan<4; plan++) {
        uint8_t b = pgm_read_byte(q++);
        uint8_t ym8 = (y & 0x07);
        uint8_t *pp = p + plan*DISPLAY_PLAN_SIZE;
        LCD_BYTE_FILTER_PLAN(pp, ~(0xff << ym8), b << ym8);
        if (ym8) {
          uint8_t *r = pp + LCD_W;
          LCD_BYTE_FILTER_PLAN(r, ~(0xff >> (8-ym8)), b >> (8-ym8));
        }
      }
      p++;
    }
  }
}
