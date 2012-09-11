#ifndef _FILE_FS_
#define _FILE_FS_

#include "fat/fatfs/src/ff.h"

#if _FS_TINY == 0
#  define STR_ROOT_DIRECTORY "0:"
#else
#  define STR_ROOT_DIRECTORY ""
#endif


#endif // _FILE_FS_