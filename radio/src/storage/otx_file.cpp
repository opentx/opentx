#include "opentx.h"
#include "otx_file.h"

#define MINIZ_NO_STDIO
#include "thirdparty/miniz/miniz.c"

// use deflated files instead of 0-compression
//#define FORCE_COMPRESSED_OTX

#define OTX_IO_BUF_SIZE (4*1024)

static FIL                      otxFile;
static mz_zip_archive_file_stat zipStat;

static mz_zip_archive* zipArchive = NULL;
static unsigned char*  ioBuffer = NULL;
static unsigned char*  local_dir_header = NULL;

#if defined(FORCE_COMPRESSED_OTX)
static tdefl_compressor* otxComp = NULL;
#endif

static bool zipAddFile(mz_zip_archive *pZip, const char *pArchive_name,
                       unsigned long uncomp_size, FIL* srcFile)
{
  unsigned int uncomp_crc32 = MZ_CRC32_INIT;
  unsigned short method = 0, ext_attributes = 0;

  if ((!pZip) || (!pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_WRITING)
      || (!pArchive_name))
    return MZ_FALSE;

  size_t archive_name_size = strlen(pArchive_name);
  if (!archive_name_size)
      return MZ_FALSE;

  // skip leading '/'
  if (pArchive_name[0] == '/') {
      pArchive_name++;
      archive_name_size--;
  }

  unsigned long local_dir_header_ofs = pZip->m_archive_size;
  unsigned long cur_archive_file_ofs = pZip->m_archive_size;
  
  if (!mz_zip_writer_write_zeros(pZip, cur_archive_file_ofs, MZ_ZIP_LOCAL_DIR_HEADER_SIZE))
    return MZ_FALSE;

  cur_archive_file_ofs += MZ_ZIP_LOCAL_DIR_HEADER_SIZE;

  memset(local_dir_header, 0, MZ_ZIP_LOCAL_DIR_HEADER_SIZE);
  if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs,
                     pArchive_name, archive_name_size) != archive_name_size)
    return MZ_FALSE;

  cur_archive_file_ofs += archive_name_size;

  unsigned long uncomp_remaining = uncomp_size;
  unsigned long comp_size = 0;

#if !defined(FORCE_COMPRESSED_OTX)

  while (uncomp_remaining) {

      mz_uint n = (mz_uint)MZ_MIN(OTX_IO_BUF_SIZE, uncomp_remaining);

      unsigned int bytes_read = 0;
      if ((f_read(srcFile, ioBuffer, n, &bytes_read) != FR_OK) ||
          (bytes_read != n))
          return false;

      if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, ioBuffer, n) != n)
          return false;

      uncomp_crc32 = (mz_uint32)mz_crc32(uncomp_crc32, (const mz_uint8 *)ioBuffer, n);
      uncomp_remaining -= n;
      cur_archive_file_ofs += n;
  }
  comp_size = uncomp_size;

#else

  tdefl_compressor* pComp = otxComp;
  // tdefl_compressor *pComp = (tdefl_compressor *)pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, sizeof(tdefl_compressor));
  // if (!pComp) {
  //     return MZ_FALSE;
  // }

  mz_zip_writer_add_state state;
  state.m_pZip = pZip;
  state.m_cur_archive_file_ofs = cur_archive_file_ofs;
  state.m_comp_size = 0;

  if (tdefl_init(pComp, mz_zip_writer_add_put_buf_callback, &state,
                 tdefl_create_comp_flags_from_zip_params(MZ_DEFAULT_LEVEL, -15,
                                                         MZ_DEFAULT_STRATEGY))
      != TDEFL_STATUS_OKAY) {

      // pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
      return MZ_FALSE;
  }

  mz_bool result = MZ_FALSE;
  for ( ; ; ) {

      size_t in_buf_size = (mz_uint32)MZ_MIN(uncomp_remaining, OTX_IO_BUF_SIZE);
      tdefl_status status;

      unsigned int bytes_read = 0;
      if (f_read(srcFile, ioBuffer, in_buf_size, &bytes_read) != FR_OK)
          break;

      if(in_buf_size != bytes_read)
          break;

      uncomp_crc32 = (mz_uint32)mz_crc32(uncomp_crc32, ioBuffer, in_buf_size);
      uncomp_remaining -= in_buf_size;

      status = tdefl_compress_buffer(pComp, ioBuffer, in_buf_size,
                                     uncomp_remaining ? TDEFL_NO_FLUSH : TDEFL_FINISH);

      if (status == TDEFL_STATUS_DONE) {
          result = MZ_TRUE;
          break;
      }
      else if (status != TDEFL_STATUS_OKAY)
          break;
  }

  // pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);

  if (!result)
      return MZ_FALSE;

  comp_size = state.m_comp_size;
  cur_archive_file_ofs = state.m_cur_archive_file_ofs;
  method = MZ_DEFLATED;

#endif

  if (!mz_zip_writer_create_local_dir_header(pZip, local_dir_header,
                                             (mz_uint16)archive_name_size, 0, uncomp_size,
                                             comp_size, uncomp_crc32, method, 0, 0, 0))
      return false;

  if (pZip->m_pWrite(pZip->m_pIO_opaque, local_dir_header_ofs,
                     local_dir_header, MZ_ZIP_LOCAL_DIR_HEADER_SIZE) != MZ_ZIP_LOCAL_DIR_HEADER_SIZE)
      return false;

  if (!mz_zip_writer_add_to_central_dir(pZip, pArchive_name, (mz_uint16)archive_name_size,
                                        NULL, 0, 0, 0, uncomp_size,
                                        comp_size, uncomp_crc32, method, 0, 0,
                                        0, local_dir_header_ofs, ext_attributes))
      return false;

  pZip->m_total_files++;
  pZip->m_archive_size = cur_archive_file_ofs;

  return true;
}

static size_t zipFileWrite(void *pOpaque, mz_uint64 file_ofs, const void *pBuf, size_t n)
{
    FIL* fptr = (FIL*)pOpaque;
    if (file_ofs != fptr->fptr) {
        if (f_lseek(fptr, file_ofs) != FR_OK)
            return 0;
    }
    TRACE("zipFileWrite(%u,%p,%u)",(unsigned int)file_ofs,pBuf,n);

    unsigned int written = 0;
    if (f_write(fptr, pBuf, n, &written) != FR_OK)
        return 0;
    return written;

    return n;
}

static bool alloc_otx_writer()
{
    zipArchive = (mz_zip_archive*)malloc(sizeof(mz_zip_archive));
    ioBuffer = (unsigned char*)malloc(OTX_IO_BUF_SIZE);
    local_dir_header = (unsigned char*)malloc(MZ_ZIP_LOCAL_DIR_HEADER_SIZE);

#if defined(FORCE_COMPRESSED_OTX)
    otxComp = (tdefl_compressor*)malloc(sizeof(tdefl_compressor));
#endif

    return ((zipArchive != NULL)
            && (ioBuffer != NULL)
            && (local_dir_header != NULL)
#if defined(FORCE_COMPRESSED_OTX)
            && (otxComp != NULL)
#endif
            );
}

static void free_otx_writer()
{
    free(zipArchive);
    zipArchive = NULL;

    free(ioBuffer);
    ioBuffer = NULL;

    free(local_dir_header);
    local_dir_header = NULL;

#if defined(FORCE_COMPRESSED_OTX)
    free(otxComp);
    otxComp = NULL;
#endif
}

bool initOtxWriter(const char* path)
{
    if (!alloc_otx_writer())
        return false;
    
    // Open and truncate file if necessary
    FRESULT fr = f_open(&otxFile, path, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK)
        return false;

    memset(zipArchive, 0, sizeof(mz_zip_archive));
    zipArchive->m_pWrite = zipFileWrite;
    zipArchive->m_pIO_opaque = &otxFile;

    if (!mz_zip_writer_init(zipArchive, 0))
        return false;

    return true;
}

bool addFile2Otx(const char* path)
{
    FILINFO fno;
    if (f_stat(path,&fno) != FR_OK)
        return false;
    
    FIL tmpFile;
    if (f_open(&tmpFile, path, FA_READ) != FR_OK)
        return false;

    bool result = zipAddFile(zipArchive, path, fno.fsize, &tmpFile);
    f_close(&tmpFile);

    return result;
}

void closeOtxWriter()
{
    mz_zip_writer_finalize_archive(zipArchive);
    mz_zip_writer_end(zipArchive);
    f_close(&otxFile);
    free_otx_writer();
}

static bool alloc_otx_reader()
{
    zipArchive = (mz_zip_archive*)malloc(sizeof(mz_zip_archive));
    //ioBuffer = (unsigned char*)malloc(OTX_IO_BUF_SIZE);

    return ((zipArchive != NULL) /*&& (ioBuffer != NULL)*/);
}

static void free_otx_reader()
{
    free(zipArchive);
    zipArchive = NULL;

    //free(ioBuffer);
    //ioBuffer = NULL;    
}

static size_t zipFileRead(void *pOpaque, mz_uint64 file_ofs, void *pBuf, size_t n)
{
    FIL* fptr = (FIL*)pOpaque;
    if (file_ofs != fptr->fptr) {
        if (f_lseek(fptr, file_ofs) != FR_OK)
            return 0;
    }
    TRACE("zipFileRead(%u,%p,%u)",(unsigned int)file_ofs,pBuf,n);

    unsigned int nread = 0;
    if (f_read(fptr, pBuf, n, &nread) != FR_OK)
        return 0;

    return nread;
}

int initOtxReader(const char* path)
{
    FILINFO fno;
    if (f_stat(path,&fno) != FR_OK)
        return -1;

    if (!alloc_otx_reader())
        return -1;

    FRESULT fr = f_open(&otxFile, path, FA_READ);
    if (fr != FR_OK)
        return -1;

    memset(zipArchive, 0, sizeof(mz_zip_archive));
    zipArchive->m_pRead = zipFileRead;
    zipArchive->m_pIO_opaque = &otxFile;

    // Checks the archive's central directory
    if (!mz_zip_reader_init(zipArchive, fno.fsize, 0))
        return -1;

    return mz_zip_reader_get_num_files(zipArchive);
}

int locateFileInOtx(const char* path)
{
    if (!path || !(path[0]))
        return -1;

    // skip leading '/'
    if (path[0] == '/') {
        path++;
    }

    return mz_zip_reader_locate_file(zipArchive, path, NULL, 0);
}

bool extractFileFromOtx(unsigned int file_idx)
{
    char filename[60];
    FIL  tmpFile;

    // grab file name
    if (!mz_zip_reader_file_stat(zipArchive, file_idx, &zipStat))
        return false;

    char* tmp = strAppend(filename, "/");
    strAppend(tmp,zipStat.m_filename);

    TRACE("extractFileFromOtx(%u): name='%s'", file_idx, filename);

    FRESULT fr = f_open(&tmpFile, filename, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK)
        return false;

    bool result = mz_zip_reader_extract_to_callback(zipArchive, file_idx, zipFileWrite,
                                                    &tmpFile, 0);
    f_close(&tmpFile);
    return result;
}

void closeOtxReader()
{
    mz_zip_reader_end(zipArchive);
    free_otx_reader();
}
