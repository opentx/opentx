/****************************************************************************
*  Copyright (c) 2014 by Michael Blandford. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*  1. Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of its contributors may
*     be used to endorse or promote products derived from this software
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
*  SUCH DAMAGE.
*
****************************************************************************
* Other Authors:
 * - Andre Bernet
 * - Bertrand Songis
 * - Bryan J. Rentoul (Gruvin)
 * - Cameron Weeks
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini
 * - Thomas Husterer
*
****************************************************************************/


/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "AT91SAM3S4.h"


uint8_t Eblock_buffer[4096] ;
uint8_t Eblock_current[4096] ;		// For erased checking
int32_t EblockAddress ;

uint8_t Spi_tx_buf[24] ;
uint8_t Spi_rx_buf[24] ;
uint32_t Spi_init_done = 0 ;

uint32_t EepromBlocked = 1 ;
extern uint8_t EE_timer ;

// Test for EEPROM file
uint32_t isEepromStart( uint8_t *p )
{
	uint32_t csum ;
	uint32_t size = 7 ;

	csum = 0 ;
	while( size )
	{
		csum += *p++ ;
		size -= 1 ;
	}
	if ( csum == *p )
	{
		return 1 ;
	}
	return 0 ;	
}


uint32_t  eeprom_write_one( uint8_t byte, uint8_t count ) ;
uint32_t eeprom_read_status( void ) ;
void eeprom_write_enable( void ) ;
void eeprom_wait_busy( void ) ;
uint32_t spi_operation( register uint8_t *tx, register uint8_t *rx, register uint32_t count ) ;
void init_spi( void ) ;
uint32_t unprotect_eeprom( void ) ;
uint32_t spi_PDC_action( register uint8_t *command, register uint8_t *tx, register uint8_t *rx, register uint32_t comlen, register uint32_t count ) ;
void AT25D_Read( uint8_t *BufferAddr, uint32_t size, uint32_t memoryOffset) ;
void AT25D_Write( uint8_t *BufferAddr, uint32_t size, uint32_t memoryOffset ) ;
uint32_t AT25D_EraseBlock( uint32_t memoryOffset ) ;
void writeBlock( void ) ;


uint32_t eeprom_write_one( uint8_t byte, uint8_t count )
{
	register Spi *spiptr ;
	register uint32_t result ;
	
	spiptr = SPI ;
	spiptr->SPI_CR = 1 ;								// Enable
	(void) spiptr->SPI_RDR ;		// Dump any rx data
	
	spiptr->SPI_TDR = byte ;

	result = 0 ; 
	while( ( spiptr->SPI_SR & SPI_SR_RDRF ) == 0 )
	{
		// wait for received
		if ( ++result > 10000 )
		{
			break ;				
		}
	}
	if ( count == 0 )
	{
		spiptr->SPI_CR = 2 ;								// Disable
		return spiptr->SPI_RDR ;
	}
	(void) spiptr->SPI_RDR ;		// Dump the rx data
	spiptr->SPI_TDR = 0 ;
	result = 0 ; 
	while( ( spiptr->SPI_SR & SPI_SR_RDRF ) == 0 )
	{
		// wait for received
		if ( ++result > 10000 )
		{
			break ;				
		}
	}
	spiptr->SPI_CR = 2 ;								// Disable
	return spiptr->SPI_RDR ;
}


uint32_t eeprom_read_status()
{
	return eeprom_write_one( 5, 1 ) ;
}


void eeprom_write_enable()
{
	eeprom_write_one( 6, 0 ) ;
}

void eeprom_wait_busy()
{
	register uint32_t x ;
	register uint32_t y ;
	
	y = 0 ;
	do
	{
		y += 1 ;
		if ( y > 1000000 )
		{
			break ;			
		}
		x = eeprom_read_status() ;
	} while ( x & 1 ) ;
  
}

uint32_t spi_operation( register uint8_t *tx, register uint8_t *rx, register uint32_t count )
{
	register Spi *spiptr ;
	register uint32_t result ;

//  PMC->PMC_PCER0 |= 0x00200000L ;		// Enable peripheral clock to SPI

	result = 0 ; 
	spiptr = SPI ;
	spiptr->SPI_CR = 1 ;								// Enable
	(void) spiptr->SPI_RDR ;		// Dump any rx data
	while( count )
	{
		result = 0 ;
		while( ( spiptr->SPI_SR & SPI_SR_TXEMPTY ) == 0 )
		{
			// wait
			if ( ++result > 10000 )
			{
				result = 0xFFFF ;
				break ;				
			}
		}
		if ( result > 10000 )
		{
			break ;
		}
//		if ( count == 1 )
//		{
//			spiptr->SPI_CR = SPI_CR_LASTXFER ;		// LastXfer bit
//		}
		spiptr->SPI_TDR = *tx++ ;
		result = 0 ;
		while( ( spiptr->SPI_SR & SPI_SR_RDRF ) == 0 )
		{
			// wait for received
			if ( ++result > 10000 )
			{
				result = 0x2FFFF ;
				break ;				
			}
		}
		if ( result > 10000 )
		{
			break ;
		}
		*rx++ = spiptr->SPI_RDR ;
		count -= 1 ;
	}
	if ( result <= 10000 )
	{
		result = 0 ;
	}
	spiptr->SPI_CR = 2 ;								// Disable

// Power save
//  PMC->PMC_PCER0 &= ~0x00200000L ;		// Disable peripheral clock to SPI

	return result ;
}

uint32_t spi_PDC_action( register uint8_t *command, register uint8_t *tx, register uint8_t *rx, register uint32_t comlen, register uint32_t count )
{
	register Spi *spiptr ;
//	register uint32_t result ;
	register uint32_t condition ;
	static uint8_t discard_rx_command[4] ;

//  PMC->PMC_PCER0 |= 0x00200000L ;		// Enable peripheral clock to SPI

//	Spi_complete = 0 ;
	if ( comlen > 4 )
	{
//		Spi_complete = 1 ;
		return 0x4FFFF ;		
	}
	condition = SPI_SR_TXEMPTY ;
	spiptr = SPI ;
	spiptr->SPI_CR = 1 ;				// Enable
	(void) spiptr->SPI_RDR ;		// Dump any rx data
	(void) spiptr->SPI_SR ;			// Clear error flags
	spiptr->SPI_RPR = (uint32_t)discard_rx_command ;
	spiptr->SPI_RCR = comlen ;
	if ( rx )
	{
		spiptr->SPI_RNPR = (uint32_t)rx ;
		spiptr->SPI_RNCR = count ;
		condition = SPI_SR_RXBUFF ;
	}
	spiptr->SPI_TPR = (uint32_t)command ;
	spiptr->SPI_TCR = comlen ;
	if ( tx )
	{
		spiptr->SPI_TNPR = (uint32_t)tx ;
	}
	else
	{
		spiptr->SPI_TNPR = (uint32_t)rx ;
	}
	spiptr->SPI_TNCR = count ;

	spiptr->SPI_PTCR = SPI_PTCR_RXTEN | SPI_PTCR_TXTEN ;	// Start transfers

	// Wait for things to get started, avoids early interrupt
	for ( count = 0 ; count < 1000 ; count += 1 )
	{
		if ( ( spiptr->SPI_SR & SPI_SR_TXEMPTY ) == 0 )
		{
			break ;			
		}
	}
	
	count = 0 ;
	while( ( spiptr->SPI_SR & condition ) == 0 )
	{
		if ( ++count > 1000000 )
		{
			break ;			
		}
	}
	
	spiptr->SPI_CR = 2 ;				// Disable
	(void) spiptr->SPI_RDR ;		// Dump any rx data
	(void) spiptr->SPI_SR ;			// Clear error flags
	spiptr->SPI_PTCR = SPI_PTCR_RXTDIS | SPI_PTCR_TXTDIS ;	// Stop tramsfers

	if ( count > 1000000 )
	{
		return 1 ;
	}
	return 0 ;
}


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






void init_spi()
{
	register Pio *pioptr ;
	register Spi *spiptr ;
	register uint32_t timer ;
	register uint8_t *p ;
	uint8_t spi_buf[4] ;

	if ( !Spi_init_done)
	{
  	PMC->PMC_PCER0 |= 0x00200000L ;		// Enable peripheral clock to SPI
  	/* Configure PIO */
		pioptr = PIOA ;
  	pioptr->PIO_ABCDSR[0] &= ~0x00007800 ;	// Peripheral A bits 14,13,12,11
  	pioptr->PIO_ABCDSR[1] &= ~0x00007800 ;	// Peripheral A
  	pioptr->PIO_PDR = 0x00007800 ;					// Assign to peripheral
	
		spiptr = SPI ;
		timer = ( 64000000 / 3000000 ) << 8 ;
		spiptr->SPI_MR = 0x14000011 ;				// 0001 0100 0000 0000 0000 0000 0001 0001 Master
		spiptr->SPI_CSR[0] = 0x01180009 | timer ;		// 0000 0001 0001 1000 xxxx xxxx 0000 1001
	//	NVIC_EnableIRQ(SPI_IRQn) ;

		p = spi_buf ;
		
	//	*p = 0x39 ;		// Unprotect sector command
	//	*(p+1) = 0 ;
	//	*(p+2) = 0 ;
	//	*(p+3) = 0 ;		// 3 bytes address

	//	spi_operation( p, spi_buf, 4 ) ;
	
		eeprom_write_enable() ;

		*p = 1 ;		// Write status register command
		*(p+1) = 0 ;
		spi_operation( p, spi_buf, 2 ) ;
		Spi_init_done = 1 ;
	}

}


void AT25D_Read( uint8_t *BufferAddr, uint32_t size, uint32_t memoryOffset)
{
	register uint8_t *p ;
	
	p = Spi_tx_buf ;
	*p = 3 ;		// Read command
	*(p+1) = memoryOffset >> 16 ;
	*(p+2) = memoryOffset >> 8 ;
	*(p+3) = memoryOffset ;		// 3 bytes address
	
	spi_PDC_action( p, 0, BufferAddr, 4, size ) ;
}


uint32_t ee32_read_512( uint32_t sector, uint8_t *buffer )
{
	AT25D_Read( buffer, 512, sector * 512 ) ;
	return 1 ;
}

void AT25D_Write( uint8_t *BufferAddr, uint32_t size, uint32_t memoryOffset )
{
	register uint8_t *p ;
	
	eeprom_write_enable() ;
	
	p = Spi_tx_buf ;
	*p = 2 ;		// Write command
	*(p+1) = memoryOffset >> 16 ;
	*(p+2) = memoryOffset >> 8 ;
	*(p+3) = memoryOffset ;		// 3 bytes address
		 
	spi_PDC_action( p, BufferAddr, 0, 4, size ) ;

	eeprom_wait_busy() ;

}

uint32_t eeprom_block_erased( register uint8_t *p)
{
	register uint32_t x ;
	register uint32_t result ;

	result = 1 ;

	for ( x = 0 ; x < 4096 ; x += 1 )
	{
		if ( *p++ != 0xFF )
		{
			result = 0 ;			
			break ;
		}		
	}
	return result ;
}

uint32_t eeprom_page_erased( register uint8_t *p)
{
	register uint32_t x ;
	register uint32_t result ;

	result = 1 ;

	for ( x = 0 ; x < 256 ; x += 1 )
	{
		if ( *p++ != 0xFF )
		{
			result = 0 ;			
			break ;
		}		
	}
	return result ;
}

void writeBlock()
{
	uint32_t x ;
	uint32_t address ;
	uint32_t i ;
	uint8_t *s ;
	
	x = eeprom_block_erased( Eblock_current ) ;		// EEPROM block blanked?
	if ( x == 0 )
	{
		AT25D_EraseBlock( EblockAddress ) ;
    memset( Eblock_current, 0xFF, 4096 ) ;		// Now erased
	}
			
	s = Eblock_buffer ;
	address = EblockAddress ;
	for ( i = 0 ; i < 16 ; i += 1 )		// pages in block
	{
		x = eeprom_page_erased( s ) ;
		if ( x == 0 )				// Not blank
		{
//EeWriteCount += 1 ;
   		AT25D_Write( s, 256, address ) ;
		}						
		s += 256 ;
		address += 256 ;
	}
	EblockAddress = -1 ;
	EE_timer = 0 ;
}


void readBlock( uint32_t block_address )
{
  AT25D_Read( Eblock_buffer, 4096, block_address ) ;	// read block to write to
  memcpy( Eblock_current, Eblock_buffer, 4096 ) ;			// Copy for erase checking

	EblockAddress = block_address ;
}
			 
uint32_t ee32_write( const uint8_t *buffer, uint32_t sector, uint32_t count )
{
	// EEPROM write
	uint32_t startMemoryOffset ;
	uint32_t memoryOffset ;
	uint32_t bytesToWrite ;
  uint8_t *pBuffer ;
	int32_t block_address ;

	if ( sector == 0 )
	{
		if ( isEepromStart( (uint8_t *) buffer ) )
		{
			EepromBlocked = 0 ;
		}
		else
		{
			EepromBlocked = 1 ;
		}
	}

	if ( EepromBlocked )
	{
		return 1 ;
	}

	startMemoryOffset = sector ;
	startMemoryOffset *= 512 ;		// Byte address into EEPROM
  memoryOffset      = startMemoryOffset;

	bytesToWrite = count * 512 ;
				
	unprotect_eeprom() ;

  pBuffer = (uint8_t *) buffer ;

	block_address = memoryOffset &0xFFFFF000 ;		// 4k boundary

	if ( EblockAddress != -1 )
	{
		// Ram copy is dirty
		if ( EblockAddress != block_address )
		{
			// flush buffer
			writeBlock() ;
		}
	}

	// Now check for pre-read
	if ( EblockAddress != block_address )
	{
		readBlock( block_address ) ;
		
//		// Check to see if it is blank
//		x = eeprom_block_erased( Eblock_buffer ) ;
//		if ( x == 0 )
//		{
//			AT25D_EraseBlock( block_address ) ;
//		}
	}

	while (bytesToWrite)
	{
		uint32_t bytes_to_copy ;
		uint32_t i ;
		uint8_t *s ;
		uint8_t *dest ;

		if ( EblockAddress != block_address )
		{
			readBlock( block_address ) ;
		}

		dest = Eblock_buffer + (memoryOffset & 0x0FFF ) ;
		s = pBuffer ;
		bytes_to_copy = 4096 - ( memoryOffset - block_address ) ;
		if ( bytes_to_copy > bytesToWrite )
		{
			bytes_to_copy = bytesToWrite ;
		}
		for ( i = 0 ; i < bytes_to_copy ; i += 1 )
		{
			*dest++ = *s++ ;						
		}
		memoryOffset += bytes_to_copy ;
		bytesToWrite -= bytes_to_copy ;
		 
		if ( dest > &Eblock_buffer[4095] )
		{
			// copied data past end
			writeBlock() ;
		}
		else
		{
			EE_timer = 30 ;		// Write dirty block in 0.3 secs
		}
	}
	return 1 ;
}

uint32_t AT25D_EraseBlock( uint32_t memoryOffset )
{
	register uint8_t *p ;
	register uint32_t x ;

//	EeEraseCount += 1 ;
	 
	eeprom_write_enable() ;
	p = Spi_tx_buf ;
	*p = 0x20 ;		// Block Erase command
	*(p+1) = memoryOffset >> 16 ;
	*(p+2) = memoryOffset >> 8 ;
	*(p+3) = memoryOffset ;		// 3 bytes address
	x = spi_operation( p, Spi_rx_buf, 4 ) ;

	eeprom_wait_busy() ;
	return x ;
}


