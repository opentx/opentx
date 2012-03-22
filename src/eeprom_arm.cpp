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


#include <stdint.h>
#include "open9x.h"
#include "stdio.h"
#include "inttypes.h"
#include "string.h"

extern PROGMEM s9xsplash[] ;



// Logic for storing to EERPOM/loading from EEPROM
// If main needs to wait for the eeprom, call mainsequence without actioning menus
// General configuration
// 'main' set flag STORE_GENERAL
// 'eeCheck' sees flag STORE_GENERAL, copies data, starts 2 second timer, clears flag STORE_GENERAL
// 'eeCheck' sees 2 second timer expire, locks copy, initiates write, enters WRITE_GENERAL mode
// 'eeCheck' completes write, unlocks copy, exits WRITE_GENERAL mode

// 'main' set flag STORE_MODEL(n)

// 'main' set flag STORE_MODEL_TRIM(n)


// 'main' needs to load a model





// These may not be needed, or might just be smaller
uint8_t Spi_tx_buf[24] ;
uint8_t Spi_rx_buf[24] ;


struct t_file_entry File_system[MAX_MODELS+1] ;

unsigned char ModelNames[MAX_MODELS+1][sizeof(g_model.name)] ;		// Allow for general


//static uint32_t Eeprom_image_updated ;		// Ram image changed
//static uint32_t Eeprom_sequence_no ;			// Ram image changed
//static uint8_t Current_eeprom_block ;		// 0 or 1 is active block
//static uint8_t Other_eeprom_block_blank ;
//static uint8_t Eeprom_process_state ;
//static uint8_t Eeprom_process_sub_no ;		// Used to manage writes
//static uint8_t	Eeprom_write_pending ;
//static uint8_t	Eeprom_writing_block_no ;


uint8_t	Eeprom32_process_state ;
uint8_t	Eeprom32_state_after_erase ;
uint8_t	Eeprom32_write_pending ;
uint8_t Eeprom32_file_index ;
uint8_t *Eeprom32_buffer_address ;
uint8_t *Eeprom32_source_address ;
uint32_t Eeprom32_address ;
uint32_t Eeprom32_data_size ;


uint16_t General_timer ;
uint16_t Model_timer ;
uint32_t Update_timer ;
uint8_t General_dirty ;
uint8_t Model_dirty ;

uint8_t Ee32_general_write_pending ;
uint8_t Ee32_model_write_pending ;
uint8_t Ee32_model_delete_pending ;



// States in Eeprom_process_state
#define E_IDLE							1
//#define E_ERASESENDING			2
//#define E_ERASEWAITING			3
//#define E_WRITESENDING			4
//#define E_WRITEWAITING			5
//#define E_32ACTIVE					6

#define EE_WAIT			0
#define EE_NO_WAIT	1


// States in Eeprom32_process_state
#define E32_IDLE							1
#define E32_ERASESENDING			2
#define E32_ERASEWAITING			3
#define E32_WRITESENDING			4
#define E32_WRITEWAITING			5
#define E32_READSENDING				6
#define E32_READWAITING				7
#define E32_BLANKCHECK				8
#define E32_WRITESTART				9

//#define E32_READING						10		// Set elsewhere as a lock


void handle_serial( void ) ;
//void hello( void ) ;
//void dbl9x( void ) ;
//uint32_t read_switch( enum EnumKeys enuk ) ;
//uint32_t read_eeprom_block( uint32_t block_no, uint32_t immediate ) ;
//uint32_t write_eeprom_block( uint32_t block_no, uint32_t sub_no, uint32_t size, uint32_t immediate ) ;
//uint32_t eeprom_image_blank( uint32_t image_index ) ;

bool ee32ModelExists(uint8_t id) ;
uint32_t get_current_block_number( uint32_t block_no, uint16_t *p_size, uint32_t *p_seq ) ;
uint32_t read32_eeprom_data( uint32_t eeAddress, register uint8_t *buffer, uint32_t size, uint32_t immediate ) ;
uint32_t write32_eeprom_block( uint32_t eeAddress, register uint8_t *buffer, uint32_t size, uint32_t immediate ) ;
void ee32_read_model_names( void ) ;
void ee32LoadModelName(uint8_t id, unsigned char*buf,uint8_t len) ;
void ee32_update_name( uint32_t id, uint8_t *source ) ;



void eeprom_process( void ) ;
uint32_t ee32_process( void ) ;

// New file system

// Start with 16 model memories, initial test uses 34 and 35 for these
// Blocks 0 and 1, general
// Blocks 2 and 3, model 1
// Blocks 4 and 5, model 2 etc
// Blocks 32 and 33, model 16

uint8_t Current_general_block ;		// 0 or 1 is active block
uint8_t Other_general_block_blank ;

struct t_eeprom_header
{
	uint32_t sequence_no ;		// sequence # to decide which block is most recent
	uint16_t data_size ;			// # bytes in data area
	uint8_t flags ;
	uint8_t hcsum ;
} ;

// Structure of data in a block
struct t_eeprom_block
{
	struct t_eeprom_header header ;
	union
	{
		uint8_t bytes[4088] ;
		uint32_t words[1022] ;
	} data ;
} ;

//struct t_gen_buf
//{
//	struct t_eeprom_header header ;
//	EEGeneral data ;
//} ;

//struct t_model_buf
//{
//	struct t_eeprom_header header ;
//	ModelData data ;
//} ;

#define EEPROM_BUFFER_SIZE ((sizeof(ModelData) + sizeof( struct t_eeprom_header ) + 3)/4)


struct t_eeprom_buffer
{
	struct t_eeprom_header header ;
	union t_eeprom_data
	{
		EEGeneral general_data ;
		ModelData model_data ;
		uint32_t words[ EEPROM_BUFFER_SIZE ] ;
	} data ;	
} Eeprom_buffer ;


//union t_eeprom_buffer
//{
//	struct t_gen_buf eeGeneral_buffer ;
//	struct t_model_buf eeModel_buffer ;
//	uint32_t words[ EEPROM_BUFFER_SIZE ] ;
//} Eeprom_buffer ;



//struct t_eeprom_block E_images[2] ;

// Check all 4096 bytes of an image to see if they are blank
//uint32_t eeprom_image_blank( uint32_t image_index )
//{
//	register uint32_t x ;
//	register uint32_t *p ;

//	p = &E_images[image_index].sequence_no ;

//	for ( x = 0 ; x < 1024 ; x += 1 )
//	{
//		if ( *p++ != 0xFFFFFFFF )
//		{
//			return 0 ;
//		}		
//	}
//	return 1 ;
//}

// genaral data needs to be written to EEPROM
void ee32StoreGeneral()
{
	General_dirty = 1 ;
	General_timer = 500 ;		// 5 seconds timeout before writing
}


// Store model to EEPROM, trim is non-zero if this is the result of a trim change
void ee32StoreModel( uint8_t modelNumber, uint8_t trim )
{
	Model_dirty = modelNumber + 1 ;
	Model_timer = 500 ;	
	ee32_update_name( Model_dirty, (uint8_t *)&g_model ) ;		// In case it's changed
}

void ee32_delete_model( uint8_t id )
{
	uint8_t buffer[sizeof(g_model.name)] ;
  memset( buffer, ' ', sizeof(g_model.name) ) ;
	ee32_update_name( id + 1, buffer ) ;
	Ee32_model_delete_pending = id + 1 ;
}

void ee32_read_model_names()
{
	uint32_t i ;

	for ( i = 1 ; i <= MAX_MODELS ; i += 1 )
	{
		ee32LoadModelName( i, ModelNames[i], sizeof(g_model.name) ) ;
	}
}

void ee32_update_name( uint32_t id, uint8_t *source )
{
	uint8_t * p ;
	uint32_t i ;

	p = ModelNames[id] ;
	for ( i = 0 ; i < sizeof(g_model.name) ; i += 1 )
	{
		*p++ = *source++ ;
	}
}


// Read eeprom data starting at random address
uint32_t read32_eeprom_data( uint32_t eeAddress, register uint8_t *buffer, uint32_t size, uint32_t immediate )
{
	register uint8_t *p ;
	register uint32_t x ;

	p = Spi_tx_buf ;
	*p = 3 ;		// Read command
	*(p+1) = eeAddress >> 16 ;
	*(p+2) = eeAddress >> 8 ;
	*(p+3) = eeAddress ;		// 3 bytes address
	spi_PDC_action( p, 0, buffer, 4, size ) ;

	if ( immediate )
	{
		return 0 ;		
	}
	for ( x = 0 ; x < 100000 ; x += 1  )
	{
		if ( Spi_complete )
		{
			break ;				
		}        			
	}
	return x ; 
}


uint32_t write32_eeprom_block( uint32_t eeAddress, register uint8_t *buffer, uint32_t size, uint32_t immediate )
{
	register uint8_t *p ;
	register uint32_t x ;

	eeprom_write_enable() ;

	p = Spi_tx_buf ;
	*p = 2 ;		// Write command
	*(p+1) = eeAddress >> 16 ;
	*(p+2) = eeAddress >> 8 ;
	*(p+3) = eeAddress ;		// 3 bytes address
	spi_PDC_action( p, buffer, 0, 4, size ) ;

	if ( immediate )
	{
		return 0 ;		
	}
	for ( x = 0 ; x < 100000 ; x += 1  )
	{
		if ( Spi_complete )
		{
			break ;				
		}        			
	}
	return x ; 
}

uint8_t byte_checksum( uint8_t *p, uint32_t size )
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

uint32_t ee32_check_header( struct t_eeprom_header *hptr )
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
uint32_t get_current_block_number( uint32_t block_no, uint16_t *p_size, uint32_t *p_seq )
{
	struct t_eeprom_header b0 ;
	struct t_eeprom_header b1 ;
  uint32_t sequence_no ;
  uint16_t size ;
	read32_eeprom_data( block_no << 12, ( uint8_t *)&b0, sizeof(b0), EE_WAIT ) ;		// Sequence # 0
	read32_eeprom_data( (block_no+1) << 12, ( uint8_t *)&b1, sizeof(b1), EE_WAIT ) ;	// Sequence # 1

	if ( ee32_check_header( &b0 ) == 0 )
	{
		b0.sequence_no = 0 ;
		b0.data_size = 0 ;
		b0.flags = 0 ;
	}

	size = b0.data_size ;
  sequence_no = b0.sequence_no ;
	if ( ee32_check_header( &b0 ) == 0 )
	{
		if ( ee32_check_header( &b1 ) != 0 )
		{
  		size = b1.data_size ;
		  sequence_no = b1.sequence_no ;
			block_no += 1 ;
		}
		else
		{
			size = 0 ;
			sequence_no = 1 ;
		}
	}
	else
	{
		if ( ee32_check_header( &b1 ) != 0 )
		{
			if ( b1.sequence_no > b0.sequence_no )
			{
	  		size = b1.data_size ;
			  sequence_no = b1.sequence_no ;
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
  if ( sequence_no == 0xFFFFFFFF )
	{  
		sequence_no = 0 ;
	}
  if ( p_seq )
	{
		*p_seq = sequence_no ;
	}
//	Block_needs_erasing = erase ;		
  
	return block_no ;
}

bool ee32LoadGeneral()
{
	uint16_t size ;
	
	size = File_system[0].size ;

  memset(&g_eeGeneral, 0, sizeof(EEGeneral));

	if ( size > sizeof(EEGeneral) )
	{
		size = sizeof(EEGeneral) ;
	}

	if ( size )
	{
		read32_eeprom_data( ( File_system[0].block_no << 12) + sizeof( struct t_eeprom_header), ( uint8_t *)&g_eeGeneral, size, 0 ) ;
	}

  for(uint8_t i=0; i<sizeof(g_eeGeneral.ownerName);i++) // makes sure name is valid
  {
      uint8_t idx = char2idx(g_eeGeneral.ownerName[i]);
      g_eeGeneral.ownerName[i] = idx2char(idx);
  }

  if(g_eeGeneral.myVers<MDVERS)
      sysFlags |= sysFLAG_OLD_EEPROM; // if old EEPROM - Raise flag

  g_eeGeneral.myVers   =  MDVERS; // update myvers

  uint16_t sum=0;
  if(size>(sizeof(EEGeneral)-20)) for(uint8_t i=0; i<12;i++) sum+=g_eeGeneral.calibMid[i];
  return g_eeGeneral.chkSum == sum;
}

void ee32WaitLoadModel(uint8_t id)
{
	// Wait for EEPROM to be idle
	if ( General_timer )
	{
		General_timer = 1 ;		// Make these happen soon
	}
	if ( Model_timer )
	{
		Model_timer = 1 ;
	}

	while( ( Eeprom32_process_state != E32_IDLE )
			|| ( General_timer )
			|| ( Model_timer ) )
	{
		mainSequence( NO_MENU ) ;		// Wait for EERPOM to be IDLE
	}

	ee32LoadModel( id ) ;		// Now get the model
}


void ee32LoadModel(uint8_t id)
{
	uint16_t size ;

    if(id<MAX_MODELS)
    {
			size =  File_system[id+1].size ;

			memset(&g_model, 0, sizeof(g_model));
       
			if ( size > sizeof(g_model) )
			{
				size = sizeof(g_model) ;
			}
			 
      if(size<256) // if not loaded a fair amount
      {
        modelDefault(id) ;
      }
			else
			{
				read32_eeprom_data( ( File_system[id+1].block_no << 12) + sizeof( struct t_eeprom_header), ( uint8_t *)&g_model, size, 0 ) ;
			}

      for(uint8_t i=0; i<sizeof(g_model.name);i++) // makes sure name is valid
      {
          uint8_t idx = char2idx(g_model.name[i]);
          g_model.name[i] = idx2char(idx);
      }
//      g_model.mdVers = MDVERS; //update mdvers

#ifdef FRSKY
  FrskyAlarmSendState |= 0x40 ;		// Get RSSI Alarms
        FRSKY_setModelAlarms();
#endif
    }
}

bool ee32ModelExists(uint8_t id)
{
	return ( File_system[id+1].size > 0 ) ;
}

void ee32LoadModelName( uint8_t id, unsigned char*buf, uint8_t len )
{
	if(id<=MAX_MODELS)
  {
		memset(buf,' ',len);
		if ( File_system[id].size > sizeof(g_model.name) )
		{
			read32_eeprom_data( ( File_system[id].block_no << 12) + 8, ( uint8_t *)buf, sizeof(g_model.name), 0 ) ;
		}
  }
}


void fill_file_index()
{
	uint32_t i ;
	for ( i = 0 ; i < MAX_MODELS + 1 ; i += 1 )
	{
		File_system[i].block_no = get_current_block_number( i * 2, &File_system[i].size, &File_system[i].sequence_no ) ;
	}
}

void init_ee32()
{
	fill_file_index() ;
	ee32_read_model_names() ;
	Eeprom32_process_state = E32_IDLE ;
}

uint32_t ee32_check_finished()
{
	if ( ( Eeprom32_process_state != E32_IDLE )
			|| ( General_timer )
			|| ( Model_timer ) )
	{
		ee32_process() ;
		return 0 ;
	}
	return 1 ;
}


uint32_t ee32_process()
{
	register uint8_t *p ;
	register uint8_t *q ;
	register uint32_t x ;
	register uint32_t eeAddress ;

//	return 0 ;

	if ( General_timer )
	{
		if ( --General_timer == 0 )
		{
			
			// Time to write g_eeGeneral
			Ee32_general_write_pending = 1 ;
		}
	}
	if ( Model_timer )
	{
		if ( --Model_timer == 0 )
		{
			
			// Time to write model
			Ee32_model_write_pending = 1 ;
		}
	}

	if ( Eeprom32_process_state == E32_IDLE )
	{
		if ( Ee32_general_write_pending )
		{
			Ee32_general_write_pending = 0 ;			// clear flag

			// Check we can write, == block is blank

			Eeprom32_source_address = (uint8_t *)&g_eeGeneral ;		// Get data fromm here
			Eeprom32_data_size = sizeof(g_eeGeneral) ;						// This much
			Eeprom32_file_index = 0 ;								// This file system entry
			Eeprom32_process_state = E32_BLANKCHECK ;
//			Writing_model = 0 ;
		}
		else if ( Ee32_model_write_pending )
		{
			Ee32_model_write_pending = 0 ;			// clear flag

			// Check we can write, == block is blank

			Eeprom32_source_address = (uint8_t *)&g_model ;		// Get data from here
			Eeprom32_data_size = sizeof(g_model) ;						// This much
			Eeprom32_file_index = Model_dirty ;								// This file system entry
			Eeprom32_process_state = E32_BLANKCHECK ;
//			Writing_model = Model_dirty ;
		}
		else if ( Ee32_model_delete_pending )
		{
			Eeprom32_source_address = (uint8_t *)&g_model ;		// Get data from here
			Eeprom32_data_size = 0 ;													// This much
			Eeprom32_file_index = Ee32_model_delete_pending ;	// This file system entry
			Ee32_model_delete_pending = 0 ;
			Eeprom32_process_state = E32_BLANKCHECK ;
		}
	}

	if ( Eeprom32_process_state == E32_BLANKCHECK )
	{
		eeAddress = File_system[Eeprom32_file_index].block_no ^ 1 ;
		eeAddress <<= 12 ;		// Block start address
		Eeprom32_address = eeAddress ;						// Where to put new data
		x = Eeprom32_data_size + sizeof( struct t_eeprom_header ) ;	// Size needing to be checked
		p = (uint8_t *) &Eeprom_buffer ;
		read32_eeprom_data( eeAddress, p, x, 1 ) ;
		Eeprom32_process_state = E32_READSENDING ;
	}

	if ( Eeprom32_process_state == E32_READSENDING )
	{
		if ( Spi_complete )
		{
			uint32_t blank = 1 ;
			x = Eeprom32_data_size + sizeof( struct t_eeprom_header ) ;	// Size needing to be checked
			p = (uint8_t *) &Eeprom_buffer ;
			while ( x )
			{
				if ( *p++ != 0xFF )
				{
					blank = 0 ;
					break ;					
				}
				x -= 1 ;
			}
			// If not blank, sort erasing here
			if ( blank )
			{
				Eeprom32_state_after_erase = E32_IDLE ;
				Eeprom32_process_state = E32_WRITESTART ;
			}
			else
			{
				eeAddress = Eeprom32_address ;
				eeprom_write_enable() ;
				p = Spi_tx_buf ;
				*p = 0x20 ;		// Block Erase command
				*(p+1) = eeAddress >> 16 ;
				*(p+2) = eeAddress >> 8 ;
				*(p+3) = eeAddress ;		// 3 bytes address
				spi_PDC_action( p, 0, 0, 4, 0 ) ;
				Eeprom32_process_state = E32_ERASESENDING ;
				Eeprom32_state_after_erase = E32_WRITESTART ;
			}
		}
	}

	if ( Eeprom32_process_state == E32_WRITESTART )
	{
		uint32_t total_size ;
		p = Eeprom32_source_address ;
		q = (uint8_t *)&Eeprom_buffer.data ;
		for ( x = 0 ; x < Eeprom32_data_size ; x += 1 )
		{
			*q++ = *p++ ;			// Copy the data to temp buffer
		}
		Eeprom_buffer.header.sequence_no = ++File_system[Eeprom32_file_index].sequence_no ;
		File_system[Eeprom32_file_index].size = Eeprom_buffer.header.data_size = Eeprom32_data_size ;
		Eeprom_buffer.header.flags = 0 ;
		Eeprom_buffer.header.hcsum = byte_checksum( (uint8_t *)&Eeprom_buffer, 7 ) ;
		total_size = Eeprom32_data_size + sizeof( struct t_eeprom_header ) ;
		eeAddress = Eeprom32_address ;		// Block start address
		x = total_size / 256 ;	// # sub blocks
		x <<= 8 ;						// to offset address
		eeAddress += x ;		// Add it in
		p = (uint8_t *) &Eeprom_buffer ;
		p += x ;						// Add offset
		x = total_size % 256 ;	// Size of last bit
		if ( x == 0 )						// Last bit empty
		{
			x = 256 ;
			p -= x ;
			eeAddress -= x ;
		}
		Eeprom32_buffer_address = p ;
		Eeprom32_address = eeAddress ;
		write32_eeprom_block( eeAddress, p, x, 1 ) ;
		Eeprom32_process_state = E32_WRITESENDING ;
	}

	if ( Eeprom32_process_state == E32_WRITESENDING )
	{
		if ( Spi_complete )
		{
			Eeprom32_process_state = E32_WRITEWAITING ;
		}			
	}		

	if ( Eeprom32_process_state == E32_WRITEWAITING )
	{
		x = eeprom_read_status() ;
		if ( ( x & 1 ) == 0 )
		{
			if ( ( Eeprom32_address & 0x0FFF ) != 0 )		// More to write
			{
				Eeprom32_address -= 256 ;
				Eeprom32_buffer_address -= 256 ;
				write32_eeprom_block( Eeprom32_address, Eeprom32_buffer_address, 256, 1 ) ;
				Eeprom32_process_state = E32_WRITESENDING ;
			}
			else
			{
				// now erase the other block
				File_system[Eeprom32_file_index].block_no ^= 1 ;		// This is now the current block
				eeAddress = Eeprom32_address ^ 0x00001000 ;		// Address of block to erase
				eeprom_write_enable() ;
				p = Spi_tx_buf ;
				*p = 0x20 ;		// Block Erase command
				*(p+1) = eeAddress >> 16 ;
				*(p+2) = eeAddress >> 8 ;
				*(p+3) = eeAddress ;		// 3 bytes address
				spi_PDC_action( p, 0, 0, 4, 0 ) ;
				Eeprom32_process_state = E32_ERASESENDING ;
				Eeprom32_state_after_erase = E32_IDLE ;
			}
		}
	}	

	if ( Eeprom32_process_state == E32_ERASESENDING )
	{
		if ( Spi_complete )
		{
			Eeprom32_process_state = E32_ERASEWAITING ;
		}			
	}	
		
	if ( Eeprom32_process_state == E32_ERASEWAITING )
	{
		x = eeprom_read_status() ;
		if ( ( x & 1 ) == 0 )
		{ // Command finished
			Eeprom32_process_state = Eeprom32_state_after_erase ;
		}			
	}

	if ( Eeprom32_process_state == E32_IDLE )
	{
		return 0 ;			// inactive
	}
	else
	{
		return 1 ;
	}
}


// Temporary eeprom handling - hence it's here

// storage defination
//struct t_eeprom_image
//{
//	uint8_t spi_command[4] ;
//	union
//	{
//		uint8_t bytes[2048] ;
//		uint32_t words[512] ;
//	} image ;
//	uint32_t sequence_no ;	
//	uint32_t filler[3] ;	
//	union
//	{
//		uint8_t bytes[2016] ;
//		uint32_t words[504] ;
//	} imagex ;
//} ;

// storage declaration
//struct t_eeprom_image E_images[2] ;


//void eeprom_process()
//{
//	register uint8_t *p ;
//	register uint8_t *q ;
//	register uint32_t block_no ;
//	register uint32_t x ;
	
//	if ( Eeprom_image_updated )
//	{
//		Eeprom_image_updated = 0 ;
//		Update_timer = 100 ;		// 1 second
//	}
//	if ( Update_timer )
//	{
//		if ( --Update_timer == 0 )
//		{
//			// Changed, but no changes for 2 seconds
//			// Time to write changes to eeprom
//			Eeprom_write_pending = 1 ;
//		}
//	}

//	ee32_process() ;


//	if ( ( Eeprom_process_state == E_32ACTIVE ) || ( Eeprom_process_state == E_IDLE ) )
//	{
//		if ( ee32_process() )
//		{
//			Eeprom_process_state = E_32ACTIVE ;
//			return ;
//		}
//		else
//		{
//			Eeprom_process_state = E_IDLE ;
//		}
//	}


//	if ( Eeprom_process_state == E_IDLE )
//	{
//		if ( Other_eeprom_block_blank == 0 )
//		{
//			// Need to erase it
//			Eeprom_process_state = E_ERASESENDING ;
//			block_no = 0 ;
//			if ( Current_eeprom_block == 0 )
//			{
//				block_no = 1 ;
//			}
//			eeprom_write_enable() ;
//			p = E_images[block_no].spi_command ;
//			*p = 0x20 ;		// Block Erase command
//			*(p+1) = 0 ;
//			*(p+2) = block_no << 4 ;
//			*(p+3) = 0 ;		// 3 bytes address
//			spi_PDC_action( p, 0, 0, 4, 0 ) ;
//		}
//		else if ( Eeprom_write_pending ) 
//		{
//			Eeprom_write_pending = 0 ;
//			block_no = 0 ;
//			if ( Current_eeprom_block == 0 )
//			{
//				block_no = 1 ;
//			}
//			// Copy data from RAM based eeprom image

//			p = E_images[block_no].image.bytes ;
//			q = eeprom ;
//			for ( x = 0 ; x < 2048 ; x += 1 )
//			{
//				*p++ = *q++ ;			
//			}
//			E_images[block_no].sequence_no = Eeprom_sequence_no + 1 ;
//			Eeprom_process_sub_no = 0 ;
//			write_eeprom_block( block_no, Eeprom_process_sub_no, 256, 1 ) ;
//			Eeprom_process_state = E_WRITESENDING ;
//			Eeprom_writing_block_no = block_no ;
//		}
//	}
	
//	if ( Eeprom_process_state == E_ERASESENDING )
//	{
//		if ( Spi_complete )
//		{
//			Eeprom_process_state = E_ERASEWAITING ;
//		}			
//	}	
		
//	if ( Eeprom_process_state == E_ERASEWAITING )
//	{
//		x = eeprom_read_status() ;
//		if ( ( x & 1 ) == 0 )
//		{ // Command finished
//			Eeprom_process_state = E_IDLE ;
//			Other_eeprom_block_blank = 1 ;
//		}			
//	}

//	if ( Eeprom_process_state == E_WRITESENDING )
//	{
//		if ( Spi_complete )
//		{
//			Eeprom_process_state = E_WRITEWAITING ;
//		}			
//	}		
	
//	if ( Eeprom_process_state == E_WRITEWAITING )
//	{
//		x = eeprom_read_status() ;
//		if ( ( x & 1 ) == 0 )
//		{
//			register uint32_t size ;
//			Eeprom_process_sub_no += 1 ;
//			size = 256 ;
//			if ( Eeprom_process_sub_no == 8)
//			{
//				size = 4 ;
//			}
//			if ( Eeprom_process_sub_no > 8)
//			{
//				size = 0 ;
//			}
//			if ( size > 0 )
//			{
//				write_eeprom_block( Eeprom_writing_block_no, Eeprom_process_sub_no, size, 1 ) ;
//				Eeprom_process_state = E_WRITESENDING ;
//			}
//			else
//			{
//				Eeprom_process_state = E_IDLE ;
//				Current_eeprom_block = Eeprom_writing_block_no ;
//				Other_eeprom_block_blank = 0 ;
//			}
//		}
//	}	
//}


void init_eeprom()
{
//	register uint32_t x ;
//	register uint32_t y ;
//	register uint8_t *p ;
//	register uint8_t *q ;
//	register uint32_t valid ;
	
//	read_eeprom_block( 0, 0 ) ;
//	read_eeprom_block( 1, 0 ) ;

	// Here we should find which block is the most recent
//	x = eeprom_image_blank( 0 ) ;
//	y = eeprom_image_blank( 1 ) ;

//	Eeprom_process_state = E_IDLE ;
//	Eeprom_process_sub_no = 0 ;
//	Eeprom_write_pending = 0 ;
//	if ( x )
//	{
//		if ( y )
//		{ // Both blank
//			Other_eeprom_block_blank = 1 ;
//			Current_eeprom_block = 1 ;
//			Eeprom_sequence_no = 0 ;
//		}
//		else
//		{ // Block 1 is active, 0 is blank
//			Other_eeprom_block_blank = 1 ;
//			Current_eeprom_block = 1 ;
//			Eeprom_sequence_no = E_images[1].sequence_no ;
//		}
//	}
//	else
//	{
//		if ( y )
//		{ // Block 0 is active, 1 is blank
//			Other_eeprom_block_blank = 1 ;
//			Current_eeprom_block = 0 ;
//			Eeprom_sequence_no = E_images[0].sequence_no ;
			
//		}
//		else
//		{ // Check sequence number and erase other block
//			Other_eeprom_block_blank = 0 ;
//			x = E_images[0].sequence_no ;
//			y = E_images[1].sequence_no ;
//			if ( y == 0xFFFFFFFF )
//			{
//				y = 0 ;				
//			}
//			if ( x == 0xFFFFFFFF )
//			{
//				x = 0 ;				
//			}
//			if ( x > y )			// Simple test, assumes no 32 bit overflow
//			{
//				Current_eeprom_block = 0 ;
//				Eeprom_sequence_no = x ;
//			}
//			else
//			{
//				Current_eeprom_block = 1 ;
//				Eeprom_sequence_no = y ;
//			}
//		}
//	}

//	// Copy valid block to RAM eeprom image
//	p = E_images[Current_eeprom_block].image.bytes ;
//	q = eeprom ;
//	for ( x = 0 ; x < 2048 ; x += 1 )
//	{
//		*q++ = *p++ ;			
//	}
//	disp_256( (uint32_t)eeprom, 6 ) ;
//	Eeprom_process_state = E_IDLE ;

	init_ee32() ;
}


//uint32_t eeprom_image_blank( uint32_t image_index )
//{
//	register uint32_t x ;
//	register uint32_t *p ;
//	register uint32_t result ;

//	result = 1 ;
//	p = E_images[image_index].image.words ;

//	for ( x = 0 ; x < 512 ; x += 1 )
//	{
//		if ( *p++ != 0xFFFFFFFF )
//		{
//			result = 0 ;			
//		}		
//	}
//	if ( result )
//	{
//		x = E_images[image_index].sequence_no ;
//		if ( x != 0xFFFFFFFF )
//		{
//			result = 0 ;			
//		}
//	}
//	return result ;
//}


//uint32_t read_eeprom_block( uint32_t block_no, uint32_t immediate )
//{
//	register uint8_t *p ;
//	register uint32_t x ;

//	p = E_images[block_no].spi_command ;
//	*p = 3 ;		// Read command
//	*(p+1) = 0 ;
//	*(p+2) = block_no << 4 ;
//	*(p+3) = 0 ;		// 3 bytes address
//	spi_PDC_action( p, 0, E_images[block_no].image.bytes, 4, 2048 + 4 ) ;

//	if ( immediate )
//	{
//		return 0 ;		
//	}
//	for ( x = 0 ; x < 100000 ; x += 1  )
//	{
//		if ( Spi_complete )
//		{
//			break ;				
//		}        			
//	}
//	return x ; 

////	return spi_action( p, E_images[block_no].image, E_images[block_no].image, 4, 2048 + 4 ) ;
//}

//uint32_t write_eeprom_block( uint32_t block_no, uint32_t sub_no, uint32_t size, uint32_t immediate )
//{
//	register uint8_t *p ;
//	register uint32_t x ;

//	eeprom_write_enable() ;
	
//	p = E_images[block_no].spi_command ;
//	*p = 2 ;		// Write command
//	*(p+1) = 0 ;
//	*(p+2) = (block_no << 4) + sub_no ;
//	*(p+3) = 0 ;		// 3 bytes address
//	spi_PDC_action( p, &E_images[block_no].image.bytes[sub_no << 8], 0, 4, size ) ;

//	if ( immediate )
//	{
//		return 0 ;		
//	}
//	for ( x = 0 ; x < 100000 ; x += 1  )
//	{
//		if ( Spi_complete )
//		{
//			break ;				
//		}        			
//	}
//	return x ; 
////	return spi_action( p, E_images[block_no].image[sub_no << 8], 0, 4, size ) ;
//}



//uint32_t write_eeprom_status( uint8_t value )
//{
//	register uint8_t *p ;

//	eeprom_write_enable() ;
		
//	p = Spi_tx_buf ;
//	*p = 1 ;		// Write status register command
//	*(p+1) = value ;

//	return spi_operation( p, Spi_rx_buf, 2 ) ;
//}

uint32_t unprotect_eeprom()
{
 	register uint8_t *p ;

	eeprom_write_enable() ;
		
	p = Spi_tx_buf ;
	*p = 0x39 ;		// Unprotect sector command
	*(p+1) = 0 ;
	*(p+2) = 0 ;
	*(p+3) = 0 ;		// 3 bytes address

	return spi_operation( p, Spi_rx_buf, 4 ) ;
}

//
// bs=16  128 blocks    verlust link:128  16files:16*8  128     sum 256
// bs=32   64 blocks    verlust link: 64  16files:16*16 256     sum 320
//
//#  define EESIZE   2048
//#  define BS       16
//#  define RESV     64  //reserv for eeprom header with directory (eeFs)

//#define FIRSTBLK (RESV/BS)
//#define BLOCKS   (EESIZE/BS)

//#define EEFS_VERS 4
//struct DirEnt{
//  uint8_t  startBlk;
//  uint16_t size:12;
//  uint16_t typ:4;
//}__attribute__((packed));
//#define MAXFILES (1+MAX_MODELS+3)
//struct EeFs{
//  uint8_t  version;
//  uint8_t  mySize;
//  uint8_t  freeList;
//  uint8_t  bs;
//  DirEnt   files[MAXFILES];
//}__attribute__((packed)) eeFs;


//static uint8_t EeFsRead(uint8_t blk,uint8_t ofs){
//  uint8_t ret;
//  eeprom_read_block(&ret,(const void*)(blk*BS+ofs),1);
//  return ret;
//}
//static void EeFsWrite(uint8_t blk,uint8_t ofs,uint8_t val){
//  eeWriteBlockCmp(&val, (void*)(blk*BS+ofs), 1);
//}

//static uint8_t EeFsGetLink(uint8_t blk){
//  return EeFsRead( blk,0);
//}
//static void EeFsSetLink(uint8_t blk,uint8_t val){
//  EeFsWrite( blk,0,val);
//}
//static uint8_t EeFsGetDat(uint8_t blk,uint8_t ofs){
//  return EeFsRead( blk,ofs+1);
//}
//static void EeFsSetDat(uint8_t blk,uint8_t ofs,uint8_t*buf,uint8_t len){
//  //EeFsWrite( blk,ofs+1,val);
//  eeWriteBlockCmp(buf, (void*)(blk*BS+ofs+1), len);
//}
//static void EeFsFlushFreelist()
//{
//  eeWriteBlockCmp(&eeFs.freeList,&((EeFs*)0)->freeList ,sizeof(eeFs.freeList));
//}
//static void EeFsFlush()
//{
//  eeWriteBlockCmp(&eeFs, 0,sizeof(eeFs));
//}

//uint16_t EeFsGetFree()
//{
//  uint16_t  ret = 0;
//  uint8_t i = eeFs.freeList;
//  while( i ){
//    ret += BS-1;
//    i = EeFsGetLink(i);
//  }
//  return ret;
//}
//static void EeFsFree(uint8_t blk){///free one or more blocks
//  uint8_t i = blk;
//  while( EeFsGetLink(i)) i = EeFsGetLink(i);
//  EeFsSetLink(i,eeFs.freeList);
//  eeFs.freeList = blk; //chain in front
//  EeFsFlushFreelist();
//}
//static uint8_t EeFsAlloc(){ ///alloc one block from freelist
//  uint8_t ret=eeFs.freeList;
//  if(ret){
//    eeFs.freeList = EeFsGetLink(ret);
//    EeFsFlushFreelist();
//    EeFsSetLink(ret,0);
//  }
//  return ret;
//}

//int8_t EeFsck()
//{
//  uint8_t *bufp;
//  static uint8_t buffer[BLOCKS];
//  bufp = buffer;
//  memset(bufp,0,BLOCKS);
//  uint8_t blk ;
//  int8_t ret=0;
//  for(uint8_t i = 0; i <= MAXFILES; i++){
//    uint8_t *startP = i==MAXFILES ? &eeFs.freeList : &eeFs.files[i].startBlk;
//    uint8_t lastBlk = 0;
//    blk = *startP;
//      //if(i == MAXFILES) blk = eeFs.freeList;
//      //    else              blk = eeFs.files[i].startBlk;
//    while(blk){
//      //      if(blk <  FIRSTBLK ) goto err_1; //bad blk index
//      //      if(blk >= BLOCKS   ) goto err_2; //bad blk index
//      //      if(bufp[blk])        goto err_3; //blk double usage
//      if( (   blk <  FIRSTBLK ) //goto err_1; //bad blk index
//          || (blk >= BLOCKS   ) //goto err_2; //bad blk index
//          || (bufp[blk]       ))//goto err_3; //blk double usage
//      {
//        if(lastBlk){
//          EeFsSetLink(lastBlk,0);
//        }else{
//          *startP = 0; //interrupt chain at startpos
//          EeFsFlush();
//        }
//        blk=0; //abort
//      }else{
//        bufp[blk] = i+1;
//        lastBlk   = blk;
//        blk       = EeFsGetLink(blk);
//      }
//    }
//  }
//  for(blk = FIRSTBLK; blk < BLOCKS; blk++){
//    if(bufp[blk]==0) {       //goto err_4; //unused block
//      EeFsSetLink(blk,eeFs.freeList);
//      eeFs.freeList = blk; //chain in front
//      EeFsFlushFreelist();
//    }
//  }
//  //  if(0){
//    //err_4: ret--;
//    //err_3: ret--;
//    //    err_2: ret--;
//    //    err_1: ret--;
//  //  }
//  return ret;
//}
//void EeFsFormat()
//{
//  if(sizeof(eeFs) != RESV){
//    extern void eeprom_RESV_mismatch();
//    eeprom_RESV_mismatch();
//  }
//  memset(&eeFs,0, sizeof(eeFs));
//  eeFs.version  = EEFS_VERS;
//  eeFs.mySize   = sizeof(eeFs);
//  eeFs.freeList = 0;
//  eeFs.bs       = BS;
//  for(uint8_t i = FIRSTBLK; i < BLOCKS; i++) EeFsSetLink(i,i+1);
//  EeFsSetLink(BLOCKS-1, 0);
//  eeFs.freeList = FIRSTBLK;
//  EeFsFlush();
//}
//bool EeFsOpen()
//{
//  eeprom_read_block(&eeFs,0,sizeof(eeFs));

//  return eeFs.version == EEFS_VERS && eeFs.mySize  == sizeof(eeFs);
//}

//bool EFile::exists(uint8_t i_fileId)
//{
//  return eeFs.files[i_fileId].startBlk;
//}

//void EFile::swap(uint8_t i_fileId1,uint8_t i_fileId2)
//{
//  DirEnt            tmp = eeFs.files[i_fileId1];
//  eeFs.files[i_fileId1] = eeFs.files[i_fileId2];
//  eeFs.files[i_fileId2] = tmp;;
//  EeFsFlush();
//}

//void EFile::rm(uint8_t i_fileId){
//  uint8_t i = eeFs.files[i_fileId].startBlk;
//  memset(&eeFs.files[i_fileId], 0, sizeof(eeFs.files[i_fileId]));
//  EeFsFlush(); //chained out

//  if(i) EeFsFree( i ); //chain in
//}

//uint16_t EFile::size(){
//  return eeFs.files[m_fileId].size;
//}


//uint8_t EFile::openRd(uint8_t i_fileId){
//  m_fileId = i_fileId;
//  m_pos      = 0;
//  m_currBlk  = eeFs.files[m_fileId].startBlk;
//  m_ofs      = 0;
//  m_bRlc     = 0;
//  m_err      = ERR_NONE;       //error reasons
//  return  eeFs.files[m_fileId].typ;
//}
//uint8_t EFile::read(uint8_t*buf,uint16_t i_len){
//  uint16_t len = eeFs.files[m_fileId].size - m_pos;
//  if(len < i_len) i_len = len;
//  len = i_len;
//  while(len)
//  {
//    if(!m_currBlk) break;
//    *buf++ = EeFsGetDat(m_currBlk, m_ofs++);
//    if(m_ofs>=(BS-1)){
//      m_ofs=0;
//      m_currBlk=EeFsGetLink(m_currBlk);
//    }
//    len--;
//  }
//  m_pos += i_len - len;
//  return i_len - len;
//}
//uint16_t EFile::readRlc(uint8_t*buf,uint16_t i_len){
//  uint16_t i;
//  for( i=0; i<i_len; ){
//    if((m_bRlc&0x7f) == 0) {
//      if(read(&m_bRlc,1)!=1) break; //read how many bytes to read
//    }
//    assert(m_bRlc & 0x7f);
//    uint8_t l=m_bRlc&0x7f;
//    if((uint16_t)l>(i_len-i)) l = (uint8_t)(i_len-i);
//    if(m_bRlc&0x80){       // if contains high byte
//      memset(&buf[i],0,l); // write l zeros
//    }else{
//      uint8_t lr = read(&buf[i],l); // read and write l bytes
//      if(lr!=l) return i+lr;
//    }
//    i    += l;
//    m_bRlc -= l;
//  }
//  return i;
//}
//uint8_t EFile::write(uint8_t*buf,uint8_t i_len){
//  uint8_t len=i_len;
//  if(!m_currBlk && m_pos==0)
//  {
//    eeFs.files[m_fileId].startBlk = m_currBlk = EeFsAlloc();
//  }
//  while(len)
//  {
//    if( (int16_t)(m_stopTime10ms - get_tmr10ms()) < 0)
//    {
//      m_err = ERR_TMO;
//      break;
//    }
//    if(!m_currBlk) {
//      m_err = ERR_FULL;
//      break;
//    }
//    if(m_ofs>=(BS-1)){
//      m_ofs=0;
//      if( ! EeFsGetLink(m_currBlk) ){
//        EeFsSetLink(m_currBlk, EeFsAlloc());
//      }
//      m_currBlk = EeFsGetLink(m_currBlk);
//    }
//    if(!m_currBlk) {
//      m_err = ERR_FULL;
//      break;
//    }
//    uint8_t l = BS-1-m_ofs; if(l>len) l=len;
//    EeFsSetDat(m_currBlk, m_ofs, buf, l);
//    buf   +=l;
//    m_ofs +=l;
//    len   -=l;
//  }
//  m_pos += i_len - len;
//  return   i_len - len;
//}
//void EFile::create(uint8_t i_fileId, uint8_t typ, uint16_t maxTme10ms){
//  openRd(i_fileId); //internal use
//  eeFs.files[i_fileId].typ      = typ;
//  eeFs.files[i_fileId].size     = 0;
//  m_stopTime10ms = get_tmr10ms() + maxTme10ms;
//}
//void EFile::closeTrunc()
//{
//  uint8_t fri=0;
//  eeFs.files[m_fileId].size     = m_pos;
//  if(m_currBlk && ( fri = EeFsGetLink(m_currBlk)))    EeFsSetLink(m_currBlk, 0);
//  EeFsFlush(); //chained out

//  if(fri) EeFsFree( fri );  //chain in
//}

//uint16_t EFile::writeRlc(uint8_t i_fileId, uint8_t typ,uint8_t*buf,uint16_t i_len, uint8_t maxTme10ms){
//  create(i_fileId,typ,maxTme10ms);
//  bool    state0 = true;
//  uint8_t cnt    = 0;
//  uint16_t i;

//  //RLE compression:
//  //rb = read byte
//  //if (rb | 0x80) write rb & 0x7F zeros
//  //else write rb bytes
//  for( i=0; i<=i_len; i++)
//  {
//    bool nst0 = buf[i] == 0;                   
//    if( nst0 && !state0 && buf[i+1]!=0) nst0 = false ;
//    if(nst0 != state0 || cnt>=0x7f || i==i_len){
//      if(state0){  
//        if(cnt>0){
//          cnt|=0x80;
//          if( write(&cnt,1)!=1)           goto error;
//          cnt=0;
//        }
//      }else{
//        if(cnt>0) {
//          if( write(&cnt,1) !=1)            goto error;
//          uint8_t ret=write(&buf[i-cnt],cnt);
//          if( ret !=cnt) { cnt-=ret;        goto error;}
//          cnt=0;
//        }
//      }
//      state0 = nst0;
//    }
//    cnt++;
//  }
//  if(0){
//    error:
//    i_len = i - (cnt & 0x7f);
//  }
//  closeTrunc();
//  return i_len;
//}




