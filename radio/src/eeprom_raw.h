/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
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

#ifndef eeprom_arm_h
#define eeprom_arm_h

#include <inttypes.h>
#include <stdint.h>

bool eeLoadGeneral( void ) ;
void eeDeleteModel( uint8_t id ) ;
bool eeCopyModel(uint8_t dst, uint8_t src);
void eeSwapModels(uint8_t id1, uint8_t id2);

#define DISPLAY_PROGRESS_BAR(x)

#if defined(SDCARD)
const pm_char * eeBackupModel(uint8_t i_fileSrc);
const pm_char * eeRestoreModel(uint8_t i_fileDst, char *model_name);
#endif

uint32_t loadGeneralSettings();
uint32_t loadModel(uint32_t index);

enum EepromWriteState {
  EEPROM_IDLE = 0,
  EEPROM_START_WRITE,
  EEPROM_ERASING_FILE_BLOCK1,
  EEPROM_ERASING_FILE_BLOCK1_WAIT,
  EEPROM_ERASE_FILE_BLOCK2,
  EEPROM_ERASING_FILE_BLOCK2,
  EEPROM_ERASING_FILE_BLOCK2_WAIT,
  EEPROM_WRITE_BUFFER,
  EEPROM_WRITING_BUFFER,
  EEPROM_WRITING_BUFFER_WAIT,
  EEPROM_WRITE_NEXT_BUFFER,
  EEPROM_ERASING_FAT_BLOCK,
  EEPROM_ERASING_FAT_BLOCK_WAIT,
  EEPROM_WRITE_NEW_FAT,
  EEPROM_WRITING_NEW_FAT,
  EEPROM_WRITING_NEW_FAT_WAIT,
  EEPROM_END_WRITE
};

extern volatile EepromWriteState eepromWriteState;
inline bool eepromIsWriting()
{
  return (eepromWriteState != EEPROM_IDLE);
}
void eepromWriteProcess();
void eepromWriteWait(EepromWriteState state = EEPROM_IDLE);
bool eepromOpen();

#endif
