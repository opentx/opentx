#include "opentx.h"
#include "boot.h"
#include "bin_files.h"

BinFileInfo binFiles[MAX_BIN_FILES];
static DIR dir;

FRESULT openBinDir(MemoryType mt)
{
    FRESULT fr = f_chdir(getBinaryPath(mt));
    if (fr != FR_OK) return fr;
    
    return f_opendir(&dir, ".");
}

static FRESULT findNextBinFile(FILINFO* fno)
{
  FRESULT fr;

  do {
    fr = f_readdir(&dir, fno);

    if (fr != FR_OK || fno->fname[0] == 0)
      break;

    int32_t len = strlen(fno->fname) - 4;
    if (len < 0)
        continue;

    if (fno->fname[len] != '.')
        continue;
    
    if ((fno->fname[len + 1] != 'b') && (fno->fname[len + 1] != 'B'))
        continue;

    if ((fno->fname[len + 2] != 'i') && (fno->fname[len + 2] != 'I'))
        continue;

    if ((fno->fname[len + 3] != 'n') && (fno->fname[len + 3] != 'N'))
        continue;

    // match!
    break;

  } while (1);

  return fr;
}

unsigned int fetchBinFiles(unsigned int index)
{
  FILINFO file_info;

  // rewind
  if (f_readdir(&dir, NULL) != FR_OK)
      return 0;

  // skip 'index' .bin files
  for (unsigned int i = 0; i <= index; i++) {
      
      if (findNextBinFile(&file_info) != FR_OK /*|| file_info.fname[0] == 0*/)
          return 0;
  }

  strAppend(binFiles[0].name, file_info.fname);
  binFiles[0].size = file_info.fsize;

  unsigned int i = 1;
  for (; i < MAX_NAMES_ON_SCREEN+1; i++) {

      if (findNextBinFile(&file_info) != FR_OK || file_info.fname[0] == 0)
          return i;
  
      strAppend(binFiles[i].name, file_info.fname);
      binFiles[i].size = file_info.fsize;
  }

  return i;
}

