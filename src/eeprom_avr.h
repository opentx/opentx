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

#ifndef eeprom_avr_h
#define eeprom_avr_h

#include <inttypes.h>

#if defined(PCBV4) && !defined(REV0)
#define WRITE_DELAY_10MS 500
extern uint16_t s_eeDirtyTime10ms;
#endif

//
// bs=16  128 blocks    verlust link:128  16files:16*8  128     sum 256
// bs=32   64 blocks    verlust link: 64  16files:16*16 256     sum 320
//
#if defined(PCBV4)
#define EESIZE     4096
#define EEFS_VERS  5
#define MAXFILES   36
#else
#define EESIZE     2048
#define EEFS_VERS  4
#define MAXFILES   20
#endif

#define BS       16

PACK(struct DirEnt{
  uint8_t  startBlk;
  uint16_t size:12;
  uint16_t typ:4;
});

PACK(struct EeFs{
  uint8_t  version;
  uint8_t  mySize;
  uint8_t  freeList;
  uint8_t  bs;
  DirEnt   files[MAXFILES];
});

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

/// fileId of general file
#define FILE_GENERAL   0
/// convert model number 0..MAX_MODELS-1  int fileId
#define FILE_MODEL(n) (1+(n))
#define FILE_TMP      (1+MAX_MODELS)

#define RESV     sizeof(EeFs)  //reserv for eeprom header with directory (eeFs)

#if defined(PCBV4)
#define FIRSTBLK      1
#define BLOCKS        (1+(EESIZE-RESV)/BS)
#define BLOCKS_OFFSET (RESV-BS)
#else
#define FIRSTBLK      (RESV/BS)
#define BLOCKS        (EESIZE/BS)
#define BLOCKS_OFFSET 0
#endif

bool EeFsOpen();
int8_t EeFsck();
void EeFsFormat();
uint16_t EeFsGetFree();

extern volatile int8_t eeprom_buffer_size;

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

//  protected:

    uint8_t  m_fileId;    //index of file in directory = filename
    uint16_t m_pos;       //over all filepos
    uint8_t  m_currBlk;   //current block.id
    uint8_t  m_ofs;       //offset inside of the current block
};

#define ERR_NONE 0
#define ERR_FULL 1
extern uint8_t  s_write_err;    // error reasons
extern uint8_t  s_sync_write;

///deliver current errno, this is reset in open
inline uint8_t write_errno() { return s_write_err; }

class RlcFile: public EFile
{
  uint8_t  m_bRlc;      // control byte for run length decoder
  uint8_t  m_zeroes;

  uint8_t m_flags;
#define WRITE_FIRST_LINK               0x01
#define WRITE_NEXT_LINK_1              0x02
#define WRITE_NEXT_LINK_2              0x03
#define WRITE_START_STEP               0x10
#define WRITE_FREE_UNUSED_BLOCKS_STEP1 0x20
#define WRITE_FREE_UNUSED_BLOCKS_STEP2 0x30
#define WRITE_FINAL_DIRENT_STEP        0x40
#define WRITE_TMP_DIRENT_STEP          0x50
  uint8_t m_write_step;
  uint16_t m_rlc_len;
  uint8_t * m_rlc_buf;
  uint8_t m_cur_rlc_len;
  uint8_t m_write1_byte;
  uint8_t m_write_len;
  uint8_t * m_write_buf;
#if defined (EEPROM_PROGRESS_BAR)
  uint8_t m_ratio;
#endif

public:

  void openRlc(uint8_t i_fileId);

  void create(uint8_t i_fileId, uint8_t typ, uint8_t sync_write);

  ///copy contents of i_fileSrc to i_fileDst
  bool copy(uint8_t i_fileDst, uint8_t i_fileSrc);

  inline bool isWriting() { return m_write_step != 0; }
  void write(uint8_t*buf, uint8_t i_len);
  void write1(uint8_t b);
  void nextWriteStep();
  void nextRlcWriteStep();
  void writeRlc(uint8_t i_fileId, uint8_t typ, uint8_t *buf, uint16_t i_len, uint8_t sync_write);
  void flush();

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

#if defined (EEPROM_PROGRESS_BAR)
  void DisplayProgressBar(uint8_t x);
#endif
};

extern RlcFile theFile;  //used for any file operation

inline void eeFlush() { theFile.flush(); }

#if defined (EEPROM_PROGRESS_BAR)
#define DISPLAY_PROGRESS_BAR(x) theFile.DisplayProgressBar(x)
#else
#define DISPLAY_PROGRESS_BAR(x)
#endif

uint16_t evalChkSum();

#define eeDeleteModel(x) EFile::rm(FILE_MODEL(x))
#define eeCopyModel(dst, src) theFile.copy(FILE_MODEL(dst), FILE_MODEL(src))
#define eeSwapModels(id1, id2) EFile::swap(FILE_MODEL(id1), FILE_MODEL(id2))

#ifdef SDCARD
const pm_char * eeArchiveModel(uint8_t i_fileSrc);
const pm_char * eeRestoreModel(uint8_t i_fileDst, char *model_name);
#endif

#endif
/*eof*/
