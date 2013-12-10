/*
 * Author - Bertrand Songis <bsongis@gmail.com>
 *
 * Based on th9x -> http://code.google.com/p/th9x/
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


#include "stdio.h"
#include "inttypes.h"
#include "string.h"
#include "file.h"
#include <assert.h>
#include <algorithm>
#include "eeprominterface.h"

EFile::EFile():
eeprom(NULL),
eeprom_size(0),
eeFs(NULL),
eeFsArm(NULL)
{
}

void EFile::EeFsCreate(uint8_t *eeprom, int size, BoardEnum board)
{
  this->eeprom = eeprom;
  this->eeprom_size = size;
  this->board = board;

  if (board == BOARD_SKY9X) {
    memset(eeprom, 0xFF, size);
  }
  else if (IS_TARANIS(board)) {
    eeFsArm = (EeFsArm *)eeprom;
    eeFsVersion = 5;
    eeFsSize = 8+4*62;
    eeFsBlockSize = 64;
    eeFsFirstBlock = 1;
    eeFsBlocksOffset = eeFsSize - eeFsBlockSize;
    if (board == BOARD_TARANIS_REV4a)
      eeFsBlocksMax = 1 + (EESIZE_TARANIS_REV4a-eeFsSize)/eeFsBlockSize;
    else
      eeFsBlocksMax = 1 + (EESIZE_TARANIS-eeFsSize)/eeFsBlockSize;
    eeFsLinkSize = sizeof(int16_t);
    memset(eeprom, 0, size);
    eeFsArm->version  = eeFsVersion;
    eeFsArm->mySize   = eeFsSize;
    eeFsArm->freeList = 0;
    eeFsArm->bs       = 64;
    for (unsigned int i=eeFsFirstBlock; i<eeFsBlocksMax-1; i++)
      EeFsSetLink(i, i+1);
    EeFsSetLink(eeFsBlocksMax-1, 0);
    eeFsArm->freeList = eeFsFirstBlock;
    // EeFsFlush();
  }
  else {
    eeFs = (EeFs *)eeprom;
    eeFsVersion = (board==BOARD_GRUVIN9X || board==BOARD_M128) ? 5 : 4;
    eeFsBlockSize = 16;
    eeFsLinkSize = 1;

    if (eeFsVersion == 5) {
      eeFsSize = 4+3*36;
      eeFsFirstBlock = 1;
      eeFsBlocksOffset = 112 - 16;
      eeFsBlocksMax = 1 + (4096-112)/16;
    }
    else {
      eeFsSize = 4+3*20;
      eeFsFirstBlock = 4;
      eeFsBlocksOffset = 0;
      eeFsBlocksMax = 2048/16;
    }

    memset(eeprom, 0, size);
    eeFs->version  = eeFsVersion;
    eeFs->mySize   = eeFsSize;
    eeFs->freeList = 0;
    eeFs->bs       = eeFsBlockSize;
    for (unsigned int i=eeFsFirstBlock; i<eeFsBlocksMax-1; i++)
      EeFsSetLink(i, i+1);
    EeFsSetLink(eeFsBlocksMax-1, 0);
    eeFs->freeList = eeFsFirstBlock;
    // EeFsFlush();
  }
}

bool EFile::EeFsOpen(uint8_t *eeprom, int size, BoardEnum board)
{
  this->eeprom = eeprom;
  this->eeprom_size = size;
  this->board = board;

  if (board == BOARD_SKY9X) {
    return 1;
  }
  else if (IS_TARANIS(board)) {
    eeFsArm = (EeFsArm *)eeprom;
    eeFsVersion = eeFsArm->version;
    eeFsSize = 8+4*62;
    eeFsBlockSize = 64;
    eeFsLinkSize = sizeof(int16_t);
    eeFsFirstBlock = 1;
    eeFsBlocksOffset = eeFsSize - eeFsBlockSize;
    if (board == BOARD_TARANIS_REV4a)
      eeFsBlocksMax = 1 + (EESIZE_TARANIS_REV4a-eeFsSize)/eeFsBlockSize;
    else
      eeFsBlocksMax = 1 + (EESIZE_TARANIS-eeFsSize)/eeFsBlockSize;
    return eeFsArm->mySize == eeFsSize;
  }
  else {
    eeFs = (EeFs *)eeprom;
    eeFsVersion = eeFs->version;
    eeFsBlockSize = 16;
    eeFsLinkSize = 1;
    if (eeFsVersion == 5) {
      eeFsSize = 4+3*36;
      eeFsFirstBlock = 1;
      eeFsBlocksOffset = 112 - 16;
      eeFsBlocksMax = 1 + (4096-112)/16;
    }
    else if (eeFsVersion == 4) {
      eeFsSize = 4+3*20;
      eeFsFirstBlock = 4;
      eeFsBlocksOffset = 0;
      eeFsBlocksMax = 2048/16;
    }
    else {
      return 0;
    }

    return eeFs->mySize == eeFsSize;
  }
}

void EFile::eeprom_read_block (void *pointer_ram, unsigned int pointer_eeprom, size_t size)
{
  memcpy(pointer_ram, &eeprom[pointer_eeprom], size);
}

void EFile::eeprom_write_block(const void *pointer_ram, unsigned int pointer_eeprom, size_t size)
{
  memcpy(&eeprom[pointer_eeprom], pointer_ram, size);
}

uint8_t EFile::EeFsRead(unsigned int blk, unsigned int ofs)
{
  uint8_t ret;
  eeprom_read_block(&ret, blk*eeFsBlockSize+ofs+eeFsBlocksOffset, 1);
  return ret;
}

void EFile::EeFsWrite(unsigned int blk, unsigned int ofs, uint8_t val)
{
  eeprom_write_block(&val, blk*eeFsBlockSize+ofs+eeFsBlocksOffset, 1);
}

unsigned int EFile::EeFsGetLink(unsigned int blk)
{
  if (IS_ARM(board)) {
    int16_t ret;
    eeprom_read_block((uint8_t *)&ret, blk*eeFsBlockSize+eeFsBlocksOffset, eeFsLinkSize);
    return ret;
  }
  else {
    return EeFsRead(blk, 0);
  }
}

void EFile::EeFsSetLink(unsigned int blk, unsigned int val)
{
  if (IS_ARM(board)) {
    int16_t s_link = val;
    eeprom_write_block((uint8_t *)&s_link, (blk*eeFsBlockSize)+eeFsBlocksOffset, eeFsLinkSize);
  }
  else {
    EeFsWrite(blk, 0, val);
  }
}

uint8_t EFile::EeFsGetDat(unsigned int blk, unsigned int ofs)
{
  return EeFsRead(blk, ofs+eeFsLinkSize);
}

void EFile::EeFsSetDat(unsigned int blk, unsigned int ofs, const uint8_t *buf, unsigned int len)
{
  eeprom_write_block(buf, blk*eeFsBlockSize+ofs+eeFsLinkSize+eeFsBlocksOffset, len);
}

unsigned int EFile::EeFsGetFree()
{
  unsigned int ret = 0;
  unsigned int i;
  if (IS_ARM(board))
    i = eeFsArm->freeList;
  else
    i = eeFs->freeList;

  while (i) {
    ret += eeFsBlockSize-eeFsLinkSize;
    i = EeFsGetLink(i);
  }
  return ret;
}

/*
 * free one or more blocks
 */
void EFile::EeFsFree(unsigned int blk)
{
  unsigned int i = blk;
  while (EeFsGetLink(i)) i = EeFsGetLink(i);
  if (IS_ARM(board)) {
    EeFsSetLink(i, eeFsArm->freeList);
    eeFsArm->freeList = blk; //chain in front
  }
  else {
    EeFsSetLink(i, eeFs->freeList);
    eeFs->freeList = blk; //chain in front
  }
}

/*
 * alloc one block from freelist
 */
unsigned int EFile::EeFsAlloc()
{
  unsigned int ret = (IS_ARM(board) ? eeFsArm->freeList : eeFs->freeList);
  if (ret) {
    if (IS_ARM(board))
      eeFsArm->freeList = EeFsGetLink(ret);
    else
      eeFs->freeList = EeFsGetLink(ret);
    EeFsSetLink(ret, 0);
  }
  return ret;
}

unsigned int EFile::size(unsigned int id)
{
  return IS_ARM(board) ? eeFsArm->files[id].size : eeFs->files[id].size;
}

unsigned int EFile::openRd(unsigned int i_fileId)
{
  if (board == BOARD_SKY9X) {
    m_fileId = get_current_block_number(i_fileId * 2, &m_size);
    m_pos = sizeof(t_eeprom_header);
    return 1;
  }
  else {
    m_fileId   = i_fileId;
    m_pos      = 0;
    m_currBlk  = (IS_ARM(board) ? eeFsArm->files[m_fileId].startBlk : eeFs->files[m_fileId].startBlk);
    m_ofs      = 0;
    m_zeroes   = 0;
    m_bRlc     = 0;
    m_err      = ERR_NONE;       //error reasons
    if (IS_ARM(board))
      return eeFsArm->files[m_fileId].typ;
    else
      return eeFs->files[m_fileId].typ;
  }
}

unsigned int EFile::read(uint8_t *buf, unsigned int i_len)
{
  unsigned int len = IS_ARM(board) ? eeFsArm->files[m_fileId].size : eeFs->files[m_fileId].size;
  len -= m_pos;
  if (i_len > len) i_len = len;
  len = i_len;
  while(len) {
    if (!m_currBlk) break;
    *buf++ = EeFsGetDat(m_currBlk, m_ofs++);
    if (m_ofs >= (eeFsBlockSize-(IS_ARM(board)? 2 : 1))) {
      m_ofs = 0;
      m_currBlk = EeFsGetLink(m_currBlk);
    }
    len--;
  }
  m_pos += i_len - len;
  return i_len - len;
}

// G: Read runlength (RLE) compressed bytes into buf.
unsigned int EFile::readRlc12(uint8_t *buf, unsigned int i_len, bool rlc2)
{
  memset(buf, 0, i_len);

  if (board == BOARD_SKY9X) {
    int len = std::min((int)i_len, (int)m_size + (int)sizeof(t_eeprom_header) - (int)m_pos);
    if (len > 0) {
      eeprom_read_block(buf, (m_fileId << 12) + m_pos, len);
      m_pos += len;
    }
    return len;
  }
  else {
    unsigned int i=0;
    for( ; 1; ) {
      uint8_t ln = std::min<uint16_t>(m_zeroes, i_len-i);
      memset(&buf[i], 0, ln);
      i        += ln;
      m_zeroes -= ln;
      if(m_zeroes) break;

      ln = std::min<uint16_t>(m_bRlc, i_len-i);
      uint8_t lr = read(&buf[i], ln);
      i        += lr ;
      m_bRlc   -= lr;
      if(m_bRlc) break;

      if (read(&m_bRlc, 1) !=1) break; //read how many bytes to read

      assert(m_bRlc & 0x7f);
      
      if (rlc2) {
        if(m_bRlc&0x80){ // if contains high byte
          m_zeroes  =(m_bRlc>>4) & 0x7;
          m_bRlc    = m_bRlc & 0x0f;
        }
        else if(m_bRlc&0x40){
          m_zeroes  = m_bRlc & 0x3f;
          m_bRlc    = 0;
        }
        //else   m_bRlc
      }
      else {
        if(m_bRlc&0x80){ // if contains high byte
          m_zeroes  = m_bRlc & 0x7f;
          m_bRlc    = 0;
        }
      }
    }
    return i;
  }
}

unsigned int EFile::write1(uint8_t b)
{
  return write(&b, 1);
}

unsigned int EFile::write(const uint8_t *buf, unsigned int i_len)
{
  unsigned int len = i_len;
  if (!m_currBlk && m_pos==0)
  {
    if (IS_ARM(board))
      eeFsArm->files[m_fileId].startBlk = m_currBlk = EeFsAlloc();
    else
      eeFs->files[m_fileId].startBlk = m_currBlk = EeFsAlloc();
  }
  while (len)
  {
    if (!m_currBlk) {
      m_err = ERR_FULL;
      return 0;
    }
    if (m_ofs >= eeFsBlockSize-eeFsLinkSize) {
      m_ofs = 0;
      if (!EeFsGetLink(m_currBlk) ){
        EeFsSetLink(m_currBlk, EeFsAlloc());
      }
      m_currBlk = EeFsGetLink(m_currBlk);
    }
    if (!m_currBlk) {
      m_err = ERR_FULL;
      return 0;
    }
    uint8_t l = eeFsBlockSize-eeFsLinkSize-m_ofs; if(l>len) l=len;
    EeFsSetDat(m_currBlk, m_ofs, buf, l);
    buf   +=l;
    m_ofs +=l;
    len   -=l;
  }
  m_pos += i_len - len;
  return i_len - len;
}

void EFile::create(unsigned int i_fileId, unsigned int typ)
{
  openRd(i_fileId); //internal use
  if (IS_ARM(board)) {
    eeFsArm->files[i_fileId].typ   = typ;
    eeFsArm->files[i_fileId].size  = 0;
  }
  else {
    eeFs->files[i_fileId].typ   = typ;
    eeFs->files[i_fileId].size  = 0;
  }
}

void EFile::closeTrunc()
{
  unsigned int fri=0;
  if (IS_ARM(board))
    eeFsArm->files[m_fileId].size = m_pos;
  else
    eeFs->files[m_fileId].size = m_pos;
  if (m_currBlk && ( fri = EeFsGetLink(m_currBlk))) EeFsSetLink(m_currBlk, 0);
  if(fri) EeFsFree( fri );  //chain in
}

unsigned int EFile::writeRlc1(unsigned int i_fileId, unsigned int typ, const uint8_t *buf, unsigned int i_len)
{
  create(i_fileId, typ);
  bool state0 = true;
  uint8_t cnt = 0;
  uint16_t i;

  //RLE compression:
  //rb = read byte
  //if (rb | 0x80) write rb & 0x7F zeros
  //else write rb bytes
  for (i=0; i<=i_len; i++)
  {
    bool nst0 = (buf[i] == 0);
    if (nst0 && !state0 && buf[i+1]!=0) nst0 = false ;
    if (nst0 != state0 || cnt>=0x7f || i==i_len) {
      if (state0) {
        if(cnt>0) {
          cnt |= 0x80;
          if (write(&cnt,1)!=1)           goto error;
          cnt=0;
        }
      }
      else {
        if (cnt>0) {
          if (write(&cnt,1) !=1)            goto error;
          uint8_t ret=write(&buf[i-cnt],cnt);
          if (ret!=cnt) { cnt-=ret;        goto error;}
          cnt=0;
        }
      }
      state0 = nst0;
    }
    cnt++;
  }
  if(0){
    error:
    i_len = i - (cnt & 0x7f);
  }
  closeTrunc();
  return i_len;
}

/*
 * Write runlength (RLE) compressed bytes
 */
unsigned int EFile::writeRlc2(unsigned int i_fileId, unsigned int typ, const uint8_t *buf, unsigned int i_len)
{
  if (board == BOARD_SKY9X) {
    openRd(i_fileId);
    eeprom_write_block(buf, (m_fileId << 12) + m_pos, i_len);
    t_eeprom_header header;
    header.sequence_no = 1;
    header.data_size = i_len;
    header.flags = 0;
    header.hcsum = byte_checksum((uint8_t *) &header, 7);
    eeprom_write_block(&header, (m_fileId << 12), sizeof(header));
    return i_len;
  }
  else {
    create(i_fileId, typ);
    bool    run0   = buf[0] == 0;
    uint8_t cnt    = 1;
    uint8_t cnt0   = 0;
    uint16_t i     = 0;
    if (i_len==0) goto close;

    //RLE compression:
    //rb = read byte
    //if (rb | 0x80) write rb & 0x7F zeros
    //else write rb bytes
    for (i=1; 1; i++) { // !! laeuft ein byte zu weit !!
      bool cur0 = buf[i] == 0;
      if (i==i_len || cur0 != run0 || cnt==0x3f || (cnt0 && cnt==0xf)) {
        if (run0){
          assert(cnt0==0);
          if (cnt<8 && i!=i_len)
            cnt0 = cnt; //aufbew fuer spaeter
          else {
            if (write1(cnt|0x40)!=1)                goto error;//-cnt&0x3f
          }
        }
        else {
          if (cnt0) {
            if (write1(0x80 | (cnt0<<4) | cnt)!=1)  goto error;//-cnt0xx-cnt
            cnt0 = 0;
          }
          else {
            if (write1(cnt)!=1)                    goto error;//-cnt
          }
          uint8_t ret = write(&buf[i-cnt], cnt);
          if (ret != cnt) { cnt-=ret;                goto error;}//-cnt
        }
        cnt=0;
        if (i==i_len) break;
        run0 = cur0;
      }
      cnt++;
    }
    if (0) {
      error:
      i-=cnt+cnt0;
    }

    close:
    closeTrunc();
    return i;
  }
}

uint8_t EFile::byte_checksum( uint8_t *p, unsigned int size )
{
        uint32_t csum ;

        csum = 0 ;
        while( size )
        {
                csum += *p++ ;
                size -= 1 ;
        }
        return csum ;
}

uint32_t EFile::ee32_check_header( struct t_eeprom_header *hptr )
{
        uint8_t csum ;

        csum = byte_checksum( ( uint8_t *) hptr, 7 ) ;
        if ( csum == hptr->hcsum )
        {
                return 1 ;
        }
        return 0 ;
}

// Pass in an even block number, this and the next block will be checked
// to see which is the most recent, the block_no of the most recent
// is returned, with the corresponding data size if required
// and the sequence number if required
uint32_t EFile::get_current_block_number( uint32_t block_no, uint16_t *p_size)
{
  struct t_eeprom_header b0 ;
  struct t_eeprom_header b1 ;
//  uint32_t sequence_no ;
  uint16_t size ;

  eeprom_read_block( ( uint8_t *)&b0, block_no << 12, sizeof(b0) ) ;          // Sequence # 0
  eeprom_read_block( ( uint8_t *)&b1, (block_no+1) << 12, sizeof(b1) ) ;      // Sequence # 1

  if ( ee32_check_header( &b0 ) == 0 )
  {
    b0.sequence_no = 0 ;
    b0.data_size = 0 ;
    b0.flags = 0 ;
  }

  size = b0.data_size ;
  // sequence_no = b0.sequence_no ;
  if ( ee32_check_header( &b0 ) == 0 )
  {
    if ( ee32_check_header( &b1 ) != 0 )
    {
      size = b1.data_size ;
      // sequence_no = b1.sequence_no ;
      block_no += 1 ;
    }
    else
    {
      size = 0 ;
      // sequence_no = 1 ;
    }
  }
  else
  {
    if ( ee32_check_header( &b1 ) != 0 )
    {
      if ( b1.sequence_no > b0.sequence_no )
      {
        size = b1.data_size ;
        // sequence_no = b1.sequence_no ;
        block_no += 1 ;
      }
    }
  }

  if ( size == 0xFFFF )
  {
    size = 0 ;
  }
  if ( p_size )
  {
    *p_size = size ;
  }

  return block_no ;
}
