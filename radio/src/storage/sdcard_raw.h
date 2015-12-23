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
#define DEFAULT_MODEL_FILENAME   "model01.bin"

const char * readModel(const char * filename, uint8_t * buffer, uint32_t size);
const char * loadModel(const char * filename);
const char * createModel(int category);


struct StorageModelsList {
  FIL file;
};

const char * storageOpenModelsList(StorageModelsList * storage);
bool storageReadNextModel(StorageModelsList * storage, char * line, int maxlen);
const char * storageInsertModel(const char * name, int category, int position);
const char * storageRemoveModel(int category, int position);
const char * storageRenameModel(const char * name, int category, int position);

const char * storageInsertCategory(const char * name, int position);
const char * storageRenameCategory(int category, const char * name);
const char * storageRemoveCategory(int category);
bool storageReadNextCategory(StorageModelsList * storage, char * line, int maxlen);
bool storageSeekCategory(StorageModelsList * storage, int category);

#endif // _SDCARD_RAW_H_


