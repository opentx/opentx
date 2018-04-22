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

#ifndef _SDCARD_RAW_H_
#define _SDCARD_RAW_H_

#include "ff.h"

#define DEFAULT_CATEGORY         "Models"
#define DEFAULT_MODEL_FILENAME   "model1.bin"

// opens radio.bin or model file
const char * openFile(const char * fullpath, FIL* file, uint16_t* size);

void getModelPath(char * path, const char * filename);

const char * readModel(const char * filename, uint8_t * buffer, uint32_t size);
const char * loadModel(const char * filename, bool alarms=true);
const char * createModel();

PACK(struct RamBackup {
  uint16_t size;
  uint8_t data[4094];
});

extern RamBackup * ramBackup;

#endif // _SDCARD_RAW_H_
