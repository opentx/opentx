/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
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

#include "open9x.h"
#include "stdio.h"
#include "inttypes.h"
#include "string.h"

uint8_t  s_write_err = 0;    // error reasons
uint8_t  s_sync_write = false;
uint8_t  s_eeDirtyMsk;
#if defined(PCBV4) && !defined(REV0)
uint16_t s_eeDirtyTime10ms;
#endif

RlcFile theFile;  //used for any file operation
EeFs eeFs;

void eeDirty(uint8_t msk)
{
  s_eeDirtyMsk |= msk;
#if defined(PCBV4) && !defined(REV0)
  s_eeDirtyTime10ms = get_tmr10ms();
#endif
}

uint16_t eeprom_pointer;
const char* eeprom_buffer_data;
volatile int8_t eeprom_buffer_size = 0;

#if !defined(SIMU)

inline void eeprom_write_byte()
{
  EEAR = eeprom_pointer;
  EEDR = *eeprom_buffer_data;
#if defined (PCBV4)
  EECR |= 1<<EEMPE;
  EECR |= 1<<EEPE;
#else
  EECR |= 1<<EEMWE;
  EECR |= 1<<EEWE;
#endif
  eeprom_pointer++;
  eeprom_buffer_data++;
}

ISR(EE_READY_vect)
{
  if (--eeprom_buffer_size > 0) {
    eeprom_write_byte();
  }
  else {
#if defined (PCBV4)
    EECR &= ~(1<<EERIE);
#else
    EECR &= ~(1<<EERIE);
#endif
  }
}

#endif

void eeWriteBlockCmp(const void *i_pointer_ram, uint16_t i_pointer_eeprom, size_t size)
{
  assert(!eeprom_buffer_size);

  eeprom_pointer = i_pointer_eeprom;
  eeprom_buffer_data = (const char*)i_pointer_ram;
  eeprom_buffer_size = size+1;

#ifdef SIMU
  sem_post(eeprom_write_sem);
#elif defined (PCBARM)

#elif defined (PCBV4)
  EECR |= (1<<EERIE);
#else
  EECR |= (1<<EERIE);
#endif

  if (s_sync_write) {
    while (eeprom_buffer_size > 0) wdt_reset();
  }
}

static uint8_t EeFsRead(uint8_t blk, uint8_t ofs)
{
  uint8_t ret;
#ifdef SIMU
  eeprom_read_block(&ret, (const void*)(uint64_t)(blk*BS+ofs+BLOCKS_OFFSET), 1);
#else
  eeprom_read_block(&ret, (const void*)(blk*BS+ofs+BLOCKS_OFFSET), 1);
#endif
  return ret;
}

static uint8_t EeFsGetLink(uint8_t blk)
{
  return EeFsRead(blk, 0);
}

static void EeFsSetLink(uint8_t blk, uint8_t val)
{
  static uint8_t s_link; // we write asynchronously, then nothing on the stack!
  s_link = val;
  eeWriteBlockCmp(&s_link, (blk*BS)+BLOCKS_OFFSET, 1);
}

static uint8_t EeFsGetDat(uint8_t blk,uint8_t ofs)
{
  return EeFsRead(blk, ofs+1);
}

static void EeFsSetDat(uint8_t blk,uint8_t ofs,uint8_t*buf,uint8_t len)
{
  eeWriteBlockCmp(buf, blk*BS+ofs+1+BLOCKS_OFFSET, len);
}

static void EeFsFlushFreelist()
{
  eeWriteBlockCmp(&eeFs.freeList, offsetof(EeFs, freeList), sizeof(eeFs.freeList));
}

static void EeFsFlushDirEnt(uint8_t i_fileId)
{
  eeWriteBlockCmp(&eeFs.files[i_fileId], offsetof(EeFs, files) + sizeof(DirEnt)*i_fileId, sizeof(DirEnt));
}

static void EeFsFlush()
{
  eeWriteBlockCmp(&eeFs, 0, sizeof(eeFs));
}

uint16_t EeFsGetFree()
{
  uint16_t  ret = 0;
  uint8_t i = eeFs.freeList;
  while( i ){
    ret += BS-1;
    i = EeFsGetLink(i);
  }
  return ret;
}

/// free one or more blocks
static void EeFsFree(uint8_t blk)
{
  uint8_t i = blk;
  while( EeFsGetLink(i)) i = EeFsGetLink(i);
  EeFsSetLink(i,eeFs.freeList);
  eeFs.freeList = blk; //chain in front
  EeFsFlushFreelist();
}

int8_t EeFsck()
{
  s_sync_write = true;

  uint8_t *bufp = reusableBuffer.eefs_buffer;
  memset(bufp, 0, BLOCKS);
  uint8_t blk ;
  int8_t ret = 0;

  for(uint8_t i = 0; i <= MAXFILES; i++) {
    uint8_t *startP = (i==MAXFILES ? &eeFs.freeList : &eeFs.files[i].startBlk);
    uint8_t lastBlk = 0;
    blk = *startP;
    while (blk) {
      if (blk < FIRSTBLK || // bad blk index
          blk >= BLOCKS  || // bad blk index
          bufp[blk])        // blk double usage
      {
        if (lastBlk) {
          EeFsSetLink(lastBlk, 0);
        }
        else {
          *startP = 0; // interrupt chain at startpos
          EeFsFlush();
        }
        blk = 0; // abort
      }
      else {
        bufp[blk] = i+1;
        lastBlk   = blk;
        blk       = EeFsGetLink(blk);
      }
    }
  }
  for (blk=FIRSTBLK; blk<BLOCKS; blk++) {
    if (!bufp[blk]) { // unused block
      EeFsSetLink(blk, eeFs.freeList);
      eeFs.freeList = blk; // chain in front
      EeFsFlushFreelist();
    }
  }

  s_sync_write = false;

  return ret;
}

void EeFsFormat()
{
  s_sync_write = true;

  memset(&eeFs,0, sizeof(eeFs));
  eeFs.version  = EEFS_VERS;
  eeFs.mySize   = sizeof(eeFs);
  eeFs.freeList = 0;
  eeFs.bs       = BS;
  for(uint8_t i = FIRSTBLK; i < BLOCKS-1; i++) EeFsSetLink(i,i+1);
  EeFsSetLink(BLOCKS-1, 0);
  eeFs.freeList = FIRSTBLK;
  EeFsFlush();

  s_sync_write = false;
}

bool EeFsOpen() // TODO inline?
{
  eeprom_read_block(&eeFs, 0, sizeof(eeFs));

#ifdef SIMU
  if (eeFs.version != EEFS_VERS) {
    printf("bad eeFs.version (%d instead of %d)\n", eeFs.version, EEFS_VERS);
    fflush(stdout);
  }
  if (eeFs.mySize != sizeof(eeFs)) {
    printf("bad eeFs.mySize (%u instead of %u)\n", eeFs.mySize, sizeof(eeFs));
    fflush(stdout);
  }
#endif  

  return eeFs.version == EEFS_VERS && eeFs.mySize == sizeof(eeFs);
}

bool EFile::exists(uint8_t i_fileId)
{
  return eeFs.files[i_fileId].startBlk;
}

/*
 * Swap two files in eeprom
 */
void EFile::swap(uint8_t i_fileId1, uint8_t i_fileId2)
{

#if ((__GNUC__ > 4 || __GNUC__ == 4) && (__GNUC_MINOR__ > 7 || __GNUC_MINOR__ == 7))
  DirEnt tmp;
  __builtin_memcpy(&tmp, __builtin_assume_aligned(&eeFs.files[i_fileId1], sizeof(DirEnt)), sizeof(DirEnt));
  __builtin_memcpy(&eeFs.files[i_fileId1], __builtin_assume_aligned(&eeFs.files[i_fileId2], sizeof(DirEnt)), sizeof(DirEnt));
  __builtin_memcpy(&eeFs.files[i_fileId2], __builtin_assume_aligned(&tmp, sizeof(DirEnt)), sizeof(DirEnt));
#else
  DirEnt            tmp = eeFs.files[i_fileId1];
  eeFs.files[i_fileId1] = eeFs.files[i_fileId2];
  eeFs.files[i_fileId2] = tmp;
#endif
  s_sync_write = true;
  EeFsFlushDirEnt(i_fileId1);
  EeFsFlushDirEnt(i_fileId2);
  s_sync_write = false;
}

void EFile::rm(uint8_t i_fileId)
{
  uint8_t i = eeFs.files[i_fileId].startBlk;
  memset(&eeFs.files[i_fileId], 0, sizeof(eeFs.files[i_fileId]));
  s_sync_write = true;
  EeFsFlushDirEnt(i_fileId);
  if (i) EeFsFree(i); //chain in
  s_sync_write = false;
}

uint16_t EFile::size()
{
  return eeFs.files[m_fileId].size;
}

/*
 * Open file i_fileId for reading.
 * Return the file's type
 */
void EFile::openRd(uint8_t i_fileId)
{
  m_fileId = i_fileId;
  m_pos      = 0;
  m_currBlk  = eeFs.files[m_fileId].startBlk;
  m_ofs      = 0;
  s_write_err = ERR_NONE;       // error reasons */
}

void RlcFile::openRlc(uint8_t i_fileId)
{
  EFile::openRd(i_fileId);
  m_zeroes   = 0;
  m_bRlc     = 0;
}

uint8_t EFile::read(uint8_t*buf,uint16_t i_len)
{
  uint16_t len = eeFs.files[m_fileId].size - m_pos;
  if(len < i_len) i_len = len;
  len = i_len;
  while(len)
  {
    if(!m_currBlk) break;
    *buf++ = EeFsGetDat(m_currBlk, m_ofs++);
    if(m_ofs>=(BS-1)){
      m_ofs=0;
      m_currBlk=EeFsGetLink(m_currBlk);
    }
    len--;
  }
  m_pos += i_len - len;
  return i_len - len;
}

/*
 * Read runlength (RLE) compressed bytes into buf.
 */
#ifdef TRANSLATIONS
uint16_t RlcFile::readRlc12(uint8_t*buf,uint16_t i_len, bool rlc2)
#else
uint16_t RlcFile::readRlc(uint8_t*buf,uint16_t i_len)
#endif
{
  uint16_t i=0;
  for( ; 1; ){
    uint8_t l=min<uint16_t>(m_zeroes,i_len-i);
    memset(&buf[i],0,l);
    i        += l;
    m_zeroes -= l;
    if(m_zeroes) break;

    l=min<uint16_t>(m_bRlc,i_len-i);
    uint8_t lr = read(&buf[i],l); 
    i        += lr ;
    m_bRlc   -= lr;
    if(m_bRlc) break;

    if(read(&m_bRlc,1)!=1) break; //read how many bytes to read

    assert(m_bRlc & 0x7f);
#ifdef TRANSLATIONS
    if (rlc2) {
#endif
      if(m_bRlc&0x80){ // if contains high byte
        m_zeroes  =(m_bRlc>>4) & 0x7;
        m_bRlc    = m_bRlc & 0x0f;
      }
      else if(m_bRlc&0x40){
        m_zeroes  = m_bRlc & 0x3f;
        m_bRlc    = 0;
      }
      //else   m_bRlc
#ifdef TRANSLATIONS
    }
    else {
      if(m_bRlc&0x80){ // if contains high byte
        m_zeroes  = m_bRlc & 0x7f;
        m_bRlc    = 0;
      }
    }
#endif
  }
  return i;
}

void RlcFile::write1(uint8_t b)
{
  m_write1_byte = b;
  write(&m_write1_byte, 1);
}

void RlcFile::write(uint8_t *buf, uint8_t i_len)
{
  m_write_len = i_len;
  m_write_buf = buf;

  do {
    nextWriteStep();
  } while (s_sync_write && m_write_len && !s_write_err);
}

void RlcFile::nextWriteStep()
{
  if (!m_currBlk && m_pos==0) {
    eeFs.files[FILE_TMP].startBlk = m_currBlk = eeFs.freeList;
    if (m_currBlk) {
      eeFs.freeList = EeFsGetLink(m_currBlk);
      m_write_step |= WRITE_FIRST_LINK;
      EeFsFlushFreelist();
      return;
    }
  }

  if ((m_write_step & 0x0f) == WRITE_FIRST_LINK) {
    m_write_step -= WRITE_FIRST_LINK;
    EeFsSetLink(m_currBlk, 0);
    return;
  }

  while (m_write_len) {
    if (!m_currBlk) {
      s_write_err = ERR_FULL;
      break;
    }
    if (m_ofs >= (BS-1)) {
      m_ofs = 0;
      uint8_t nextBlk = EeFsGetLink(m_currBlk);
      if (!nextBlk) {
        if (!eeFs.freeList) {
          s_write_err = ERR_FULL;
          break;
        }
        m_write_step += WRITE_NEXT_LINK_1; // TODO review all these names
        EeFsSetLink(m_currBlk, eeFs.freeList);
        return;
      }
      m_currBlk = nextBlk;
    }
    switch (m_write_step & 0x0f) {
      case WRITE_NEXT_LINK_1:
        m_currBlk = eeFs.freeList;
        eeFs.freeList = EeFsGetLink(eeFs.freeList);
        m_write_step += 1;
        EeFsFlushFreelist();
        return;
      case WRITE_NEXT_LINK_2:
        m_write_step -= WRITE_NEXT_LINK_2;
        EeFsSetLink(m_currBlk, 0); // TODO needed?
        return;
    }
    if (!m_currBlk) { // TODO needed?
      s_write_err = ERR_FULL;
      break;
    }
    uint8_t tmp = BS-1-m_ofs; if(tmp>m_write_len) tmp=m_write_len;
    m_write_buf += tmp;
    m_write_len -= tmp;
    m_ofs += tmp;
    m_pos += tmp;
    EeFsSetDat(m_currBlk, m_ofs-tmp, m_write_buf-tmp, tmp);
    return;
  }

  if (s_write_err == ERR_FULL) {
    alert(STR_EEPROMWARN, STR_EEPROMOVERFLOW);
    m_write_step = 0;
    m_write_len = 0;
  }

  if (!s_sync_write)
    nextRlcWriteStep();
}

void RlcFile::create(uint8_t i_fileId, uint8_t typ, uint8_t sync_write)
{
  // all write operations will be executed on FILE_TMP
  openRlc(FILE_TMP); // internal use
  eeFs.files[FILE_TMP].typ      = typ;
  eeFs.files[FILE_TMP].size     = 0;
  m_fileId = i_fileId;
  s_sync_write = sync_write;
}

/*
 * Copy file src to dst
 */
bool RlcFile::copy(uint8_t i_fileDst, uint8_t i_fileSrc)
{
  EFile theFile2;
  theFile2.openRd(i_fileSrc);

  create(i_fileDst, FILE_TYP_MODEL/*optimization, only model files are copied. should be eeFs.files[i_fileSrc].typ*/, true);
  uint8_t buf[15];
  uint8_t len;
  while ((len=theFile2.read(buf, 15)))
  {
    write(buf, len);
    if (write_errno() != 0) {
      s_sync_write = false;
      return false;
    }
  }

  uint8_t fri=0;
  if (m_currBlk && (fri=EeFsGetLink(m_currBlk)))
    EeFsSetLink(m_currBlk, 0);

  if (fri) EeFsFree(fri);  //chain in

  eeFs.files[FILE_TMP].size = m_pos;
  EFile::swap(m_fileId, FILE_TMP);

  assert(!m_write_step);

  // s_sync_write is set to false in swap();
  return true;
}

#if defined(SDCARD)
#define FILENAME_MAXLEN 8
const pm_char * eeBackupModel(uint8_t i_fileSrc)
{
  char *buf = reusableBuffer.models.mainname;
  FIL archiveFile;
  DIR archiveFolder;
  UINT written;

#ifndef SIMU
  FRESULT result = f_mount(0, &g_FATFS_Obj);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }
#endif

  // check and create folder here
  strcpy_P(buf, STR_MODELS_PATH);
#ifndef SIMU
  result = f_opendir(&archiveFolder, buf);
  if (result != FR_OK) {
    result = f_mkdir(buf);
    if (result != FR_OK)
      return SDCARD_ERROR(result);
  }
#endif

  buf[sizeof(MODELS_PATH)-1] = '/';
  eeLoadModelName(i_fileSrc, &buf[sizeof(MODELS_PATH)]);
  buf[sizeof(MODELS_PATH)+FILENAME_MAXLEN] = '\0';

  uint8_t i = sizeof(MODELS_PATH)+FILENAME_MAXLEN-1;
  uint8_t len = 0;
  while (i>sizeof(MODELS_PATH)-1) {
    if (!len && buf[i])
      len = i+1;
    if (len) {
      if (buf[i])
	buf[i] = idx2char(buf[i]);
      else
        buf[i] = '_'; 	
    }
    i--;
  }

  if (len == 0) {
    uint8_t num = i_fileSrc + 1;
    strcpy_P(&buf[sizeof(MODELS_PATH)], STR_MODEL);
    buf[sizeof(MODELS_PATH) + PSIZE(TR_MODEL)] = (char)((num / 10) + '0');
    buf[sizeof(MODELS_PATH) + PSIZE(TR_MODEL) + 1] = (char)((num % 10) + '0');
    len = sizeof(MODELS_PATH) + PSIZE(TR_MODEL) + 2;
  }

  strcpy_P(&buf[len], STR_MODELS_EXT);

#ifdef SIMU
  printf("SD-card backup filename=%s\n", buf); fflush(stdout);
  FRESULT
#endif

  result = f_open(&archiveFile, buf, FA_OPEN_ALWAYS | FA_WRITE);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  result = f_write(&archiveFile, &g_eeGeneral.myVers, 1, &written);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  EFile theFile2;
  theFile2.openRd(FILE_MODEL(i_fileSrc));

  while ((len=theFile2.read((uint8_t *)buf, 15))) {
    result = f_write(&archiveFile, (uint8_t *)buf, len, &written);
    if (result != FR_OK) {
      return SDCARD_ERROR(result);
    }
  }

  f_close(&archiveFile);
  return NULL;
}

const pm_char * eeRestoreModel(uint8_t i_fileDst, char *model_name)
{
  char *buf = reusableBuffer.models.mainname;
  FIL restoreFile;
  UINT read;

  FRESULT result = f_mount(0, &g_FATFS_Obj);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  strcpy_P(buf, STR_MODELS_PATH);
  buf[sizeof(MODELS_PATH)-1] = '/';
  strcpy(&buf[sizeof(MODELS_PATH)], model_name);
  strcpy_P(&buf[strlen(buf)], STR_MODELS_EXT);

  result = f_open(&restoreFile, buf, FA_OPEN_EXISTING | FA_READ);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  result = f_read(&restoreFile, (uint8_t *)buf, 1, &read);
  if (result != FR_OK || read != 1) {
    return SDCARD_ERROR(result);
  }

  if (buf[0] != g_eeGeneral.myVers) {
    // TODO
  }

  if (eeModelExists(i_fileDst)) {
    eeDeleteModel(i_fileDst);
  }

  theFile.create(FILE_MODEL(i_fileDst), FILE_TYP_MODEL, true);

  do {
    result = f_read(&restoreFile, (uint8_t *)buf, 15, &read);
    if (result != FR_OK) {
      s_sync_write = false;
      return SDCARD_ERROR(result);
    }
    if (read > 0) {
      theFile.write((uint8_t *)buf, read);
      if (write_errno() != 0) {
        s_sync_write = false;
        return STR_EEPROMOVERFLOW;
      }
    }
  } while (read == 15);

  uint8_t fri=0;
  if (theFile.m_currBlk && (fri=EeFsGetLink(theFile.m_currBlk)))
    EeFsSetLink(theFile.m_currBlk, 0);

  if (fri) EeFsFree(fri);  //chain in

  eeFs.files[FILE_TMP].size = theFile.m_pos;
  EFile::swap(theFile.m_fileId, FILE_TMP); // s_sync_write is set to false in swap();

  f_close(&restoreFile);
  return NULL;
}
#endif

void RlcFile::writeRlc(uint8_t i_fileId, uint8_t typ, uint8_t*buf, uint16_t i_len, uint8_t sync_write)
{
  create(i_fileId, typ, sync_write);

  m_write_step = WRITE_START_STEP;
  m_rlc_buf = buf;
  m_rlc_len = i_len;
  m_cur_rlc_len = 0;
#if defined (EEPROM_PROGRESS_BAR)
  m_ratio = (typ == FILE_TYP_MODEL ? 100 : 10);
#endif

  do {
    nextRlcWriteStep();
  } while (s_sync_write && m_write_step && !s_write_err);
}

void RlcFile::nextRlcWriteStep()
{
  uint8_t cnt    = 1;
  uint8_t cnt0   = 0;
  uint16_t i = 0;

  if (m_cur_rlc_len) {
    uint8_t tmp1 = m_cur_rlc_len;
    uint8_t *tmp2 = m_rlc_buf;
    m_rlc_buf += m_cur_rlc_len;
    m_cur_rlc_len = 0;
    write(tmp2, tmp1);
    return;
  }

  bool    run0   = m_rlc_buf[0] == 0;

  if(m_rlc_len==0) goto close;

  for (i=1; 1; i++) // !! laeuft ein byte zu weit !!
  {
    bool cur0 = m_rlc_buf[i] == 0;
    if (cur0 != run0 || cnt==0x3f || (cnt0 && cnt==0xf)|| i==m_rlc_len){
      if (run0) {
        assert(cnt0==0);
        if (cnt<8 && i!=m_rlc_len)
          cnt0 = cnt; //aufbew fuer spaeter
        else {
          m_rlc_buf+=cnt;
          m_rlc_len-=cnt;
          write1(cnt|0x40);
          return;
        }
      }
      else{
        m_rlc_buf+=cnt0;
        m_rlc_len-=cnt0+cnt;
        m_cur_rlc_len=cnt;
        if(cnt0){
          write1(0x80 | (cnt0<<4) | cnt);
        }
        else{
          write1(cnt);
        }
        return;
      }
      cnt=0;
      if (i==m_rlc_len) break;
      run0 = cur0;
    }
    cnt++;
  }

  close:

   switch(m_write_step) {
     case WRITE_START_STEP:
     {
       uint8_t fri=0;

       if (m_currBlk && ( fri = EeFsGetLink(m_currBlk))) {
         uint8_t prev_freeList = eeFs.freeList;
         eeFs.freeList = fri;
         while( EeFsGetLink(fri)) fri = EeFsGetLink(fri);
         m_write_step = WRITE_FREE_UNUSED_BLOCKS_STEP1;
         EeFsSetLink(fri, prev_freeList);
         return;
       }
     }

     case WRITE_FINAL_DIRENT_STEP:
       m_currBlk = eeFs.files[FILE_TMP].startBlk;
       eeFs.files[FILE_TMP].startBlk = eeFs.files[m_fileId].startBlk;
       eeFs.files[m_fileId].startBlk = m_currBlk;
       eeFs.files[m_fileId].size = m_pos;
       eeFs.files[m_fileId].typ = eeFs.files[FILE_TMP].typ;
       m_write_step = WRITE_TMP_DIRENT_STEP;
       EeFsFlushDirEnt(m_fileId);
       return;

     case WRITE_TMP_DIRENT_STEP:
       m_write_step = 0;
       EeFsFlushDirEnt(FILE_TMP);
       return;

     case WRITE_FREE_UNUSED_BLOCKS_STEP1:
       m_write_step = WRITE_FREE_UNUSED_BLOCKS_STEP2;
       EeFsSetLink(m_currBlk, 0);
       return;

     case WRITE_FREE_UNUSED_BLOCKS_STEP2:
       m_write_step = WRITE_FINAL_DIRENT_STEP;
       EeFsFlushFreelist();
       return;
   }
}

void RlcFile::flush()
{
  while (eeprom_buffer_size > 0) wdt_reset();

  s_sync_write = true;
  while (m_write_len && !s_write_err)
    nextWriteStep();
  while (isWriting() && !s_write_err)
    nextRlcWriteStep();
  s_sync_write = false;
}

#if defined (EEPROM_PROGRESS_BAR)
void RlcFile::DisplayProgressBar(uint8_t x)
{
  if (s_eeDirtyMsk || isWriting() || eeprom_buffer_size) {
    uint8_t len = s_eeDirtyMsk ? 1 : limit((uint8_t)1, (uint8_t)(7 - (m_rlc_len/m_ratio)), (uint8_t)7);
    lcd_filled_rect(x+1, 0, 5, FH, SOLID, WHITE);
    lcd_filled_rect(x+2, 7-len, 3, len);
  }
}
#endif

bool eeLoadGeneral()
{
  theFile.openRlc(FILE_GENERAL);
  if (theFile.readRlc((uint8_t*)&g_eeGeneral, 1) == 1 && g_eeGeneral.myVers == EEPROM_VER) {
    theFile.openRlc(FILE_GENERAL); // TODO include this openRlc inside readRlc
    if (theFile.readRlc((uint8_t*)&g_eeGeneral, sizeof(g_eeGeneral)) <= sizeof(EEGeneral)) {
      uint16_t sum = evalChkSum();
      if (g_eeGeneral.chkSum == sum) {
        return true;
      }
    }
  }
  return false;
}


uint16_t eeLoadModelName(uint8_t id, char *name)
{
  memset(name, 0, sizeof(g_model.name));
  if (id<MAX_MODELS) {
    theFile.openRlc(FILE_MODEL(id));
    if (theFile.readRlc((uint8_t*)name, sizeof(g_model.name)) == sizeof(g_model.name)) {
      return theFile.size();
    }
  }
  return 0;
}

bool eeModelExists(uint8_t id)
{
    return EFile::exists(FILE_MODEL(id));
}

void eeLoadModel(uint8_t id)
{
  if (id<MAX_MODELS) {

    if (pulsesStarted()) {
      pausePulses();
    }

    theFile.openRlc(FILE_MODEL(id));
    uint16_t sz = theFile.readRlc((uint8_t*)&g_model, sizeof(g_model));

#ifdef SIMU
    if (sz > 0 && sz != sizeof(g_model)) {
      printf("Model data read=%d bytes vs %d bytes\n", sz, (int)sizeof(ModelData));
    }
#endif

    if (sz < 256) {
      modelDefault(id);
      eeCheck(true);
    }

    if (pulsesStarted()) {
      checkTHR();
      checkSwitches();
      resumePulses();
      clearKeyEvents();
    }

    resetProto();
    resetAll();

#ifdef SDCARD
    initLogs();
#endif
  }
}

void eeReadAll()
{
  if(!EeFsOpen() ||
     EeFsck() < 0 ||
     !eeLoadGeneral())
  {
    generalDefault();

    alert(STR_EEPROMWARN, STR_BADEEPROMDATA);
    message(STR_EEPROMWARN, STR_EEPROMFORMATTING, NULL);

    EeFsFormat();

    theFile.writeRlc(FILE_GENERAL, FILE_TYP_GENERAL,(uint8_t*)&g_eeGeneral,sizeof(EEGeneral), true);

    modelDefault(0);
    theFile.writeRlc(FILE_MODEL(0), FILE_TYP_MODEL, (uint8_t*)&g_model, sizeof(g_model), true);
  }

  stickMode = g_eeGeneral.stickMode;
  eeLoadModel(g_eeGeneral.currModel);
}

void eeCheck(bool immediately)
{
  if (immediately) {
    eeFlush();
  }

  if (s_eeDirtyMsk & EE_GENERAL) {
    s_eeDirtyMsk -= EE_GENERAL;
    theFile.writeRlc(FILE_GENERAL, FILE_TYP_GENERAL, (uint8_t*)&g_eeGeneral, sizeof(EEGeneral), immediately);
    if (!immediately) return;
  }

  if (s_eeDirtyMsk & EE_MODEL) {
    s_eeDirtyMsk = 0;
    theFile.writeRlc(FILE_MODEL(g_eeGeneral.currModel), FILE_TYP_MODEL, (uint8_t*)&g_model, sizeof(g_model), immediately);
  }
}

