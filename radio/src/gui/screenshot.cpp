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

const uint8_t BMP_HEADER[] = {
  0x42, 0x4d, 0xF8, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x28, 0x00,
  0x00, 0x00, LCD_W,  0x00, 0x00, 0x00, LCD_H, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0xbc, 0x38, 0x00, 0x00, 0xbc, 0x38, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0xee, 0xee, 0xee, 0x00, 0xdd, 0xdd,
  0xdd, 0x00, 0xcc, 0xcc, 0xcc, 0x00, 0xbb, 0xbb, 0xbb, 0x00, 0xaa, 0xaa, 0xaa, 0x00, 0x99, 0x99,
  0x99, 0x00, 0x88, 0x88, 0x88, 0x00, 0x77, 0x77, 0x77, 0x00, 0x66, 0x66, 0x66, 0x00, 0x55, 0x55,
  0x55, 0x00, 0x44, 0x44, 0x44, 0x00, 0x33, 0x33, 0x33, 0x00, 0x22, 0x22, 0x22, 0x00, 0x11, 0x11,
  0x11, 0x00, 0x00, 0x00, 0x00, 0x00
};

const char * writeScreenshot()
{
  FIL bmpFile;
  UINT written;
  char filename[42]; // /SCREENSHOTS/screen-2013-01-01-123540.bmp

  // check and create folder here
  strcpy_P(filename, SCREENSHOTS_PATH);
  const char * error = sdCheckAndCreateDirectory(filename);
  if (error) {
    return error;
  }

  char * tmp = strAppend(&filename[sizeof(SCREENSHOTS_PATH)-1], "/screen");
  tmp = strAppendDate(tmp, true);
  strcpy(tmp, BMP_EXT);

  FRESULT result = f_open(&bmpFile, filename, FA_CREATE_ALWAYS | FA_WRITE);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  result = f_write(&bmpFile, BMP_HEADER, sizeof(BMP_HEADER), &written);
  if (result != FR_OK || written != sizeof(BMP_HEADER)) {
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