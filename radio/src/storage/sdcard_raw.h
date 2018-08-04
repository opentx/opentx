#ifndef _SDCARD_RAW_H_
#define _SDCARD_RAW_H_

#include "FatFs/ff.h"

const char * openFile(const char * fullpath, FIL* file, uint16_t* size);

#endif
