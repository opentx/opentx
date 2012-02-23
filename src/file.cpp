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

#define PROGRESS_VERTICAL_BAR

#include "open9x.h"
#include "stdio.h"
#include "inttypes.h"
#include "string.h"

uint8_t  s_write_err = 0;    // error reasons
uint8_t  s_sync_write = false;

#define EEFS_VERS 4
PACK(struct DirEnt{
  uint8_t  startBlk;
  uint16_t size:12;
  uint16_t typ:4;
});
#define MAXFILES (1+MAX_MODELS+3)
PACK(struct EeFs{
  uint8_t  version;
  uint8_t  mySize;
  uint8_t  freeList;
  uint8_t  bs;
  DirEnt   files[MAXFILES];
}) eeFs;


static uint8_t EeFsRead(uint8_t blk, uint8_t ofs)
{
  uint8_t ret;
  eeprom_read_block(&ret, (const void*)(blk*BS+ofs), 1);
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
  eeWriteBlockCmp(&s_link, (blk*BS), 1);
}

static uint8_t EeFsGetDat(uint8_t blk,uint8_t ofs)
{
  return EeFsRead(blk, ofs+1);
}

static void EeFsSetDat(uint8_t blk,uint8_t ofs,uint8_t*buf,uint8_t len)
{
  eeWriteBlockCmp(buf, blk*BS+ofs+1, len);
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

static void EeFsFree(uint8_t blk){///free one or more blocks
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
  memset(bufp,0,BLOCKS);
  uint8_t blk ;
  int8_t ret=0;

  for(uint8_t i = 0; i <= MAXFILES; i++){
    uint8_t *startP = i==MAXFILES ? &eeFs.freeList : &eeFs.files[i].startBlk;
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
  for(uint8_t i = FIRSTBLK; i < BLOCKS; i++) EeFsSetLink(i,i+1);
  EeFsSetLink(BLOCKS-1, 0);
  eeFs.freeList = FIRSTBLK;
  EeFsFlush();

  s_sync_write = false;
}

bool EeFsOpen()
{
  eeprom_read_block(&eeFs,0,sizeof(eeFs));

#ifdef SIMU
  if(eeFs.version != EEFS_VERS)    perror("bad eeFs.version\n");
  if(eeFs.mySize  != sizeof(eeFs)) perror("bad eeFs.mySize\n");
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
  DirEnt            tmp = eeFs.files[i_fileId1];
  eeFs.files[i_fileId1] = eeFs.files[i_fileId2];
  eeFs.files[i_fileId2] = tmp;
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
    alert(STR_EEPROMOVERFLOW);
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

void RlcFile::writeRlc(uint8_t i_fileId, uint8_t typ, uint8_t*buf, uint16_t i_len, uint8_t sync_write)
{
  create(i_fileId, typ, sync_write);

  m_write_step = WRITE_START_STEP;
  m_rlc_buf = buf;
  m_rlc_len = i_len;
  m_cur_rlc_len = 0;
#if defined (PROGRESS_CIRCLE)
  m_ratio = (typ == FILE_TYP_MODEL ? 60 : 6);
#elif defined (PROGRESS_VERTICAL_BAR)
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

void RlcFile::DisplayProgressBar(uint8_t x)
{
  if (s_eeDirtyMsk || isWriting() || eeprom_buffer_size) {
#if defined (PROGRESS_BAR)
    uint8_t len = (s_eeDirtyMsk ? 123 : min((uint8_t)123, (uint8_t)((m_rlc_len) / 5 + eeprom_buffer_size)));
    lcd_filled_rect(2, 1, 125, 5, SOLID, WHITE);
    lcd_filled_rect(3, 2, 123-len, 3);
#elif defined (PROGRESS_CIRCLE)
    lcd_filled_rect(x-1, 0, 7, 7, SOLID, WHITE);
    uint8_t len = s_eeDirtyMsk ? 1 : limit(1, 12 - (uint8_t)(m_rlc_len/m_ratio), 12);
    lcd_hline(x+1, 1, min((uint8_t)3, len));
    if (len >= 3) {
      lcd_vline(x+4, 2, min(3, len-3));
      if (len >= 6) {
        lcd_hline(x+4, 1+4, -min(3, len-6));
        if (len >= 9) {
          lcd_vline(x, 5, -min(3, len-9));
        }
      }
    }
#elif defined (PROGRESS_VERTICAL_BAR)
    uint8_t len = s_eeDirtyMsk ? 1 : limit((uint8_t)1, (uint8_t)(7 - (m_rlc_len/m_ratio)), (uint8_t)7);
    lcd_filled_rect(x+1, 0, 5, FH, SOLID, WHITE);
    lcd_filled_rect(x+2, 7-len, 3, len);
#elif defined (PROGRESS_FIXED_CIRCLE)
    lcd_square(x, 1, 5);
#endif
  }
}
