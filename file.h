#ifndef file_h
/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 *
 * gruvin9x is based on code named er9x by
 * Author - Erez Raviv <erezraviv@gmail.com>, which is in turn
 * was based on the original (and ongoing) project by Thomas Husterer,
 * th9x -- http://code.google.com/p/th9x/
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

#define file_h

#include <inttypes.h>

//
// bs=16  128 blocks    verlust link:128  16files:16*8  128     sum 256
// bs=32   64 blocks    verlust link: 64  16files:16*16 256     sum 320
//
#if defined(PCBV3)
#define EESIZE   4096
#define BS       32
#else
#define EESIZE   2048
#define BS       16
#endif

/// fileId of general file
#define FILE_GENERAL   0
/// convert model number 0..MAX_MODELS-1  int fileId
#define FILE_MODEL(n) (1+(n))
#define FILE_TMP      (1+16)

bool EeFsOpen();
int8_t EeFsck();
void EeFsFormat();
uint16_t EeFsGetFree();

#ifdef EEPROM_ASYNC_WRITE
extern volatile int8_t eeprom_buffer_size;
#endif

class EFile
{
  public:

    ///remove contents of given file
    static void rm(uint8_t i_fileId);

    ///swap contents of file1 with them of file2
    static void swap(uint8_t i_fileId1,uint8_t i_fileId2);

    ///return true if the file with given fileid exists
    static bool exists(uint8_t i_fileId);

    ///return size of compressed file without block overhead
    uint16_t size(); // TODO static ?

    ///open file for reading, no close necessary
    void openRd(uint8_t i_fileId);

    uint8_t read(uint8_t*buf, uint16_t i_len);

  protected:

    uint8_t  m_fileId;    //index of file in directory = filename
    uint16_t m_pos;       //over all filepos
    uint8_t  m_currBlk;   //current block.id
    uint8_t  m_ofs;       //offset inside of the current block
};

#define ERR_NONE 0
#define ERR_FULL 1
#ifndef EEPROM_ASYNC_WRITE
#define ERR_TMO  2
#endif
extern uint8_t  s_write_err;    // error reasons
extern uint8_t  s_sync_write;

///deliver current errno, this is reset in open
inline uint8_t errno() { return s_write_err; }

class RlcFile: public EFile
{
  uint8_t  m_bRlc;      // control byte for run length decoder
  uint8_t  m_zeroes;

#ifdef EEPROM_ASYNC_WRITE
  uint8_t m_flags;
#define WRITE_FIRST_LINK  0x01
#define WRITE_NEXT_LINK_1 0x02
#define WRITE_NEXT_LINK_2 0x03
#define WRITE_START_STEP               0x10
#define WRITE_FREE_UNUSED_BLOCKS_STEP1 0x20
#define WRITE_FREE_UNUSED_BLOCKS_STEP2 0x30
#define WRITE_FLUSH_STEP               0x40
#define WRITE_SWAP_STEP                0x50
  uint8_t m_write_step;

  uint16_t m_rlc_len;
  uint8_t * m_rlc_buf;
  uint8_t m_cur_rlc_len;
  uint8_t m_write1_byte;
  uint8_t m_write_len;
  uint8_t * m_write_buf;
#else
  uint16_t m_stopTime10ms; // maximum point of time for writing
#endif

public:

  void openRlc(uint8_t i_fileId);


#ifdef EEPROM_ASYNC_WRITE
  void create(uint8_t i_fileId, uint8_t typ, uint8_t sync_write);
  inline bool isWriting() { return m_write_step != 0; }
  void write(uint8_t*buf, uint8_t i_len);
  void write1(uint8_t b);
  void nextWriteStep();
  void nextRlcWriteStep();
  void writeRlc(uint8_t i_fileId, uint8_t typ, uint8_t *buf, uint16_t i_len, uint8_t sync_write);
  void flush();
#else
  /// create a new file with given fileId,
  /// !!! if this file already exists, then all blocks are reused
  /// and all contents will be overwritten.
  /// after writing close has to be called

  void create(uint8_t i_fileId, uint8_t typ, uint16_t maxTme10ms);

  uint8_t write(uint8_t*buf, uint8_t i_len);
  uint8_t write1(uint8_t b);
  ///open file, write to file and close it.
  ///If file existed before, then contents is overwritten.
  ///If file was larger before, then unused blocks are freed
  uint16_t writeRlc(uint8_t i_fileId, uint8_t typ,uint8_t*buf,uint16_t i_len, uint8_t maxTme10ms);
#endif

  /*
   * close file and truncate the blockchain if to long.
   */
  void close();

  ///read from opened file and decode rlc-coded data
#ifdef TRANSLATIONS
  uint16_t readRlc12(uint8_t*buf,uint16_t i_len,bool rlc2);
  inline uint16_t readRlc1(uint8_t*buf,uint16_t i_len)
  {
    return readRlc12(buf,i_len,false);
  }
  inline uint16_t readRlc(uint8_t*buf, uint16_t i_len)
  {
    return readRlc12(buf,i_len,true);
  }
#else
  uint16_t readRlc(uint8_t*buf, uint16_t i_len); // TODO should be like writeRlc?
#endif

};

#endif
/*eof*/
