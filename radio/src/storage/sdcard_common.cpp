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
#include "storage.h"
#include "sdcard_common.h"

void getModelPath(char * path, const char * filename)
{
  strcpy(path, STR_MODELS_PATH);
  path[sizeof(MODELS_PATH)-1] = '/';
  strcpy(&path[sizeof(MODELS_PATH)], filename);
}

const char * openFile(const char * fullpath, FIL* file, uint16_t* size, uint8_t * version)
{
  FRESULT result = f_open(file, fullpath, FA_OPEN_EXISTING | FA_READ);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  if (f_size(file) < 8) {
    f_close(file);
    return STR_INCOMPATIBLE;
  }

  UINT read;
  char buf[8];

  result = f_read(file, (uint8_t *)buf, sizeof(buf), &read);
  if ((result != FR_OK) || (read != sizeof(buf))) {
    f_close(file);
    return SDCARD_ERROR(result);
  }

  //TODO: move this code into some checkCompatibleFormat()

  *version = (uint8_t)buf[4];
  if ((*(uint32_t*)&buf[0] != OTX_FOURCC && *(uint32_t*)&buf[0] != O9X_FOURCC) || *version < FIRST_CONV_EEPROM_VER || *version > EEPROM_VER || buf[5] != 'M') {
    f_close(file);
    return STR_INCOMPATIBLE;
  }

  *size = *(uint16_t*)&buf[6];
  return nullptr;
}

