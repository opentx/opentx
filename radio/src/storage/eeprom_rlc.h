/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x 
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _EEPROM_RLC_H_
#define _EEPROM_RLC_H_

#include "definitions.h"

#if defined(CPUARM)
  #define blkid_t    uint16_t
  #define EEFS_VERS  5
  #define MAXFILES   62
  #define BS         64
#elif defined(CPUM2560) || defined(CPUM2561) || defined(CPUM128)
  #define blkid_t    uint8_t
  #define EEFS_VERS  5
  #define MAXFILES   36
  #define BS         16
#else
  #define blkid_t    uint8_t
  #define EEFS_VERS  4
  #define MAXFILES   20
  #define BS         16
#endif

PACK(struct DirEnt {
  blkid_t  startBlk;
  uint16_t size:12;
  uint16_t typ:4;
});

#if defined(CPUARM)
  #define EEFS_EXTRA_FIELDS uint8_t  spare[2];
#else
  #define EEFS_EXTRA_FIELDS
#endif

PACK(struct EeFs {
  uint8_t  version;
  blkid_t  mySize;
  blkid_t  freeList;
  uint8_t  bs;
  EEFS_EXTRA_FIELDS
  DirEnt   files[MAXFILES];
});

extern EeFs eeFs;

#define FILE_TYP_GENERAL 1
#define FILE_TYP_MODEL   2

/// fileId of general file
#define FILE_GENERAL   0
/// convert model number 0..MAX_MODELS-1  int fileId
#define FILE_MODEL(n) (1+(n))
#define FILE_TMP      (1+MAX_MODELS)

#define RESV          sizeof(EeFs)  //reserv for eeprom header with directory (eeFs)

#if defined(CPUM64)
#define FIRSTBLK      (RESV/BS)
#define BLOCKS        (EEPROM_SIZE/BS)
#define BLOCKS_OFFSET 0
#else
#define FIRSTBLK      1
#define BLOCKS        (1+(EEPROM_SIZE-RESV)/BS)
#define BLOCKS_OFFSET (RESV-BS)
#endif

uint16_t EeFsGetFree();

class EFile
{
  public:

    ///remove contents of given file
    static void rm(uint8_t i_fileId);

    ///swap contents of file1 with them of file2
    static void swap(uint8_t i_fileId1, uint8_t i_fileId2);

    ///return true if the file with given fileid exists
    static bool exists(uint8_t i_fileId);

    ///open file for reading, no close necessary
    void openRd(uint8_t i_fileId);

    uint8_t read(uint8_t *buf, uint8_t len);

//  protected:

    uint8_t  m_fileId;    //index of file in directory = filename
    uint16_t m_pos;       //over all filepos
    blkid_t  m_currBlk;   //current block.id
    uint8_t  m_ofs;       //offset inside of the current block
};

#define eeFileSize(f)   eeFs.files[f].size
#define eeModelSize(id) eeFileSize(FILE_MODEL(id))

#define ERR_NONE 0
#define ERR_FULL 1
extern uint8_t s_write_err; // error reasons
inline uint8_t write_errno() { return s_write_err; }

extern uint8_t s_sync_write;
#define ENABLE_SYNC_WRITE(val)         s_sync_write = val;
#define IS_SYNC_WRITE_ENABLE()         s_sync_write

class RlcFile: public EFile
{
    uint8_t  m_bRlc;      // control byte for run length decoder
    uint8_t  m_zeroes;

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

    /// copy contents of i_fileSrc to i_fileDst
    bool copy(uint8_t i_fileDst, uint8_t i_fileSrc);

    inline bool isWriting() { return m_write_step != 0; }
    void write(uint8_t *buf, uint8_t i_len);
    void write1(uint8_t b);
    void nextWriteStep();
    void nextRlcWriteStep();
    void writeRlc(uint8_t i_fileId, uint8_t typ, uint8_t *buf, uint16_t i_len, uint8_t sync_write);

    // flush the current write operation if any
    void flush();

    // read from opened file and decode rlc-coded data
    uint16_t readRlc(uint8_t *buf, uint16_t i_len);

#if defined (EEPROM_PROGRESS_BAR)
    void drawProgressBar(uint8_t x);
#endif
};

extern RlcFile theFile;  //used for any file operation

inline void eeFlush()
{
  theFile.flush();
}

void eepromWriteBlock(uint8_t * buffer, size_t address, size_t size);

inline bool eepromIsWriting()
{
  return theFile.isWriting();
}

inline void eepromWriteProcess()
{
  theFile.nextWriteStep();
}

#if defined (EEPROM_PROGRESS_BAR)
#define DISPLAY_PROGRESS_BAR(x) theFile.drawProgressBar(x)
#else
#define DISPLAY_PROGRESS_BAR(x)
#endif

#if defined(CPUARM)
bool eeCopyModel(uint8_t dst, uint8_t src);
void eeSwapModels(uint8_t id1, uint8_t id2);
void eeDeleteModel(uint8_t idx);
#else
#define eeCopyModel(dst, src) theFile.copy(FILE_MODEL(dst), FILE_MODEL(src))
#define eeSwapModels(id1, id2) EFile::swap(FILE_MODEL(id1), FILE_MODEL(id2))
#define eeDeleteModel(idx) EFile::rm(FILE_MODEL(idx))
#endif

#if defined(SDCARD)
const pm_char * eeBackupModel(uint8_t i_fileSrc);
const pm_char * eeRestoreModel(uint8_t i_fileDst, char *model_name);
#endif

// For conversions
#if defined(CPUARM)
void loadRadioSettingsSettings();
void loadModel(int index, bool alarms=true);
#endif

bool eepromOpen();
void eeLoadModelName(uint8_t id, char *name);
bool eeLoadGeneral();

// For EEPROM backup/restore
#if defined(CPUARM)
inline bool isEepromStart(const void * buffer)
{
  // OpenTX EEPROM
  {
    const EeFs * eeprom = (const EeFs *)buffer;
    if (eeprom->version==EEFS_VERS && eeprom->mySize==sizeof(eeFs) && eeprom->bs==BS)
      return true;
  }

  // ersky9x EEPROM
  {
    const uint8_t * eeprom = (const uint8_t *)buffer;
    uint8_t size = eeprom[1] ;
    uint8_t bs = eeprom[3] ;
    if (size==0x80 && bs==0x80)
      return true;
  }

  return false;
}

void eepromBackup();
#endif

#endif // _EEPROM_RLC_H_
