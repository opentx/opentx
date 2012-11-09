/*
 * Author - Erez Raviv <erezraviv@gmail.com>
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

#ifndef eeprom_arm_h
#define eeprom_arm_h

#include <inttypes.h>
#include <stdint.h>

#ifdef REV0
#define WRITE_DELAY_10MS 100
#else
#define WRITE_DELAY_10MS 500
#endif

extern uint8_t  s_eeDirtyMsk;
extern uint16_t s_eeDirtyTime10ms;

// States in Eeprom32_process_state
#define E32_IDLE                                                        1
#define E32_ERASESENDING                        2
#define E32_ERASEWAITING                        3
#define E32_WRITESENDING                        4
#define E32_WRITEWAITING                        5
#define E32_READSENDING                         6
#define E32_READWAITING                         7
#define E32_BLANKCHECK                          8
#define E32_WRITESTART                          9
extern uint8_t Eeprom32_process_state ;
extern uint8_t *Eeprom32_source_address ;
extern uint8_t Eeprom32_file_index ;
extern uint32_t Eeprom32_data_size ;

extern void eeprom_init( void ) ;
extern void end_spi(); // TODO not public
extern void ee32_process( void ) ;
extern bool eeLoadGeneral( void ) ;
extern bool eeConvert();
extern void eeWaitFinished();

extern void eeDeleteModel( uint8_t id ) ;
extern bool eeModelExists(uint8_t id) ;
extern bool eeCopyModel(uint8_t dst, uint8_t src);
extern void eeSwapModels(uint8_t id1, uint8_t id2);

#define DISPLAY_PROGRESS_BAR(x)

struct t_file_entry
{
    uint32_t block_no ;
    uint32_t sequence_no ;
    uint16_t size ;
    uint8_t flags ;
} ;

struct t_eeprom_header
{
    uint32_t sequence_no ;              // sequence # to decide which block is most recent
    uint16_t data_size ;                        // # bytes in data area
    uint8_t flags ;
    uint8_t hcsum ;
};

extern struct t_file_entry File_system[] ;
extern char ModelNames[][sizeof(g_model.name)] ;		// Allow for general

extern EEGeneral  g_eeGeneral;
extern ModelData  g_model;

extern uint8_t Spi_tx_buf[] ;
extern uint8_t Spi_rx_buf[] ;

void read32_eeprom_data(uint32_t eeAddress, register uint8_t *buffer, uint32_t size, uint32_t immediate=0);

#if defined(SDCARD)
const pm_char * eeBackupModel(uint8_t i_fileSrc);
const pm_char * eeRestoreModel(uint8_t i_fileDst, char *model_name);
#endif

#endif

