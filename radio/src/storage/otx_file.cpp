#include "opentx.h"
#include "otx_file.h"

#define MINIZ_NO_STDIO
#include "thirdparty/miniz/miniz.c"

#define OTX_IO_BUF_SIZE (4*1024)

static FIL            otxFile;
static mz_zip_archive zipArchive;
static unsigned char  ioBuffer[OTX_IO_BUF_SIZE];

//#define FORCE_COMPRESSED_OTX

static bool zipAddFile(mz_zip_archive *pZip, const char *pArchive_name,
                       unsigned long uncomp_size, FIL* srcFile)
{
  unsigned char local_dir_header[MZ_ZIP_LOCAL_DIR_HEADER_SIZE];
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
  
  if (!mz_zip_writer_write_zeros(pZip, cur_archive_file_ofs, sizeof(local_dir_header)))
    return MZ_FALSE;

  cur_archive_file_ofs += sizeof(local_dir_header);

  MZ_CLEAR_OBJ(local_dir_header);
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
 
  tdefl_compressor *pComp = (tdefl_compressor *)pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, sizeof(tdefl_compressor));
  if (!pComp) {
      return MZ_FALSE;
  }

  mz_zip_writer_add_state state;
  state.m_pZip = pZip;
  state.m_cur_archive_file_ofs = cur_archive_file_ofs;
  state.m_comp_size = 0;

  if (tdefl_init(pComp, mz_zip_writer_add_put_buf_callback, &state,
                 tdefl_create_comp_flags_from_zip_params(MZ_DEFAULT_LEVEL, -15,
                                                         MZ_DEFAULT_STRATEGY))
      != TDEFL_STATUS_OKAY) {

      pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
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

  pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);

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
                     local_dir_header, sizeof(local_dir_header)) != sizeof(local_dir_header))
      return false;

  if (!mz_zip_writer_add_to_central_dir(pZip, pArchive_name, (mz_uint16)archive_name_size,
                                        NULL, 0, 0, 0, uncomp_size,
                                        comp_size, uncomp_crc32, method, 0, 0,
                                        0, local_dir_header_ofs, ext_attributes))
      return false;

  pZip->m_total_files++;
  pZip->m_archive_size = cur_archive_file_ofs;

  return false;
}

static size_t zipFileWrite(void *pOpaque, mz_uint64 file_ofs, const void *pBuf, size_t n)
{
    if (file_ofs != otxFile.fptr) {
        if (f_lseek(&otxFile, file_ofs) != FR_OK)
            return 0;
    }
    TRACE("zipFileWrite(%lu,%p,%lu)",file_ofs,pBuf,n);

    unsigned int written = 0;
    if (f_write(&otxFile, pBuf, n, &written) != FR_OK)
        return 0;

    return written;
}

bool initOtxWriter(const char* path)
{
    // Open and truncate file if necessary
    FRESULT fr = f_open(&otxFile, path, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK)
        return false;

    memset(&zipArchive, 0, sizeof(zipArchive));
    zipArchive.m_pWrite = zipFileWrite;

    if (!mz_zip_writer_init(&zipArchive, 0))
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

    bool result = zipAddFile(&zipArchive, path, fno.fsize, &tmpFile);
    f_close(&tmpFile);

    return result;
}

void closeOtxFile()
{
    mz_zip_writer_finalize_archive(&zipArchive);
    mz_zip_writer_end(&zipArchive);
    f_close(&otxFile);
}
