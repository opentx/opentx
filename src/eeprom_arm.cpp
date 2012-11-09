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
#include "inttypes.h"
#include "string.h"

volatile uint32_t Spi_complete ;
uint8_t  s_eeDirtyMsk;
uint16_t s_eeDirtyTime10ms;

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

char ModelNames[MAX_MODELS][sizeof(g_model.name)] ;		// Allow for general

// TODO check everything here
uint8_t	Eeprom32_process_state ;
uint8_t	Eeprom32_state_after_erase ;
uint8_t	Eeprom32_write_pending ;
uint8_t Eeprom32_file_index ;
uint8_t *Eeprom32_buffer_address ;
uint8_t *Eeprom32_source_address ;
uint32_t Eeprom32_address ;
uint32_t Eeprom32_data_size ;

#define EE_NOWAIT	1

void eeDirty(uint8_t msk)
{
  s_eeDirtyMsk |= msk;
  s_eeDirtyTime10ms = get_tmr10ms() ;
}

void handle_serial( void ) ;

uint32_t get_current_block_number( uint32_t block_no, uint16_t *p_size, uint32_t *p_seq ) ;
void write32_eeprom_block( uint32_t eeAddress, register uint8_t *buffer, uint32_t size, uint32_t immediate=0 ) ;
void ee32_read_model_names( void ) ;
void ee32LoadModelName(uint8_t id, char *buf, uint8_t len) ;

// New file system

// Start with 16 model memories, initial test uses 34 and 35 for these
// Blocks 0 and 1, general
// Blocks 2 and 3, model 1
// Blocks 4 and 5, model 2 etc
// Blocks 32 and 33, model 16

uint8_t Current_general_block ;		// 0 or 1 is active block
uint8_t Other_general_block_blank ;

struct t_eeprom_buffer
{
    struct t_eeprom_header header ;
    union t_eeprom_data
    {
        EEGeneral general_data ;
        ModelData model_data ;
        uint8_t   bytes[4096-sizeof(t_eeprom_header)];
    } data ;
} Eeprom_buffer ;

void eeDeleteModel(uint8_t id)
{
  eeCheck(true);

  memset(ModelNames[id], 0, sizeof(g_model.name));

  Eeprom32_source_address = (uint8_t *)&g_model ;   // Get data from here
  Eeprom32_data_size = 0 ;                          // This much
  Eeprom32_file_index = id + 1 ;                    // This file system entry
  Eeprom32_process_state = E32_BLANKCHECK ;
  eeWaitFinished();
}

bool eeCopyModel(uint8_t dst, uint8_t src)
{
  // eeCheck(true) should have been called before entering here

  uint16_t size = File_system[src+1].size ;
  read32_eeprom_data( (File_system[src+1].block_no << 12) + sizeof( struct t_eeprom_header), ( uint8_t *)&Eeprom_buffer.data.model_data, size) ;

  if (size > sizeof(g_model.name))
    memcpy(ModelNames[dst], Eeprom_buffer.data.model_data.name, sizeof(g_model.name));
  else
    memset(ModelNames[dst], 0, sizeof(g_model.name));

  Eeprom32_source_address = (uint8_t *)&Eeprom_buffer.data.model_data;    // Get data from here
  Eeprom32_data_size = sizeof(g_model) ;                                  // This much
  Eeprom32_file_index = dst + 1 ;                                         // This file system entry
  Eeprom32_process_state = E32_BLANKCHECK ;
  eeWaitFinished();
  return true;
}

void eeSwapModels(uint8_t id1, uint8_t id2)
{
  // eeCheck(true) should have been called before entering here

  uint16_t id2_size = File_system[id2+1].size;
  uint32_t id2_block_no = File_system[id2+1].block_no;

  eeCopyModel(id2, id1);

  // block_no(id1) has been shifted now, but we have the size

  // TODO flash saving with function above ...
  if (id2_size > sizeof(g_model.name)) {
    read32_eeprom_data( (id2_block_no << 12) + sizeof( struct t_eeprom_header), ( uint8_t *)&Eeprom_buffer.data.model_data, id2_size);
    memcpy(ModelNames[id1], Eeprom_buffer.data.model_data.name, sizeof(g_model.name));
  }
  else {
    memset(ModelNames[id1], 0, sizeof(g_model.name));
  }

  Eeprom32_source_address = (uint8_t *)&Eeprom_buffer.data.model_data;    // Get data from here
  Eeprom32_data_size = id2_size ;                                         // This much
  Eeprom32_file_index = id1 + 1 ;                                         // This file system entry
  Eeprom32_process_state = E32_BLANKCHECK ;
  eeWaitFinished();
}

uint32_t spi_PDC_action( register uint8_t *command, register uint8_t *tx, register uint8_t *rx, register uint32_t comlen, register uint32_t count )
{
#ifndef SIMU

  register Spi *spiptr ;
  register uint32_t condition ;
  static uint8_t discard_rx_command[4] ;

//  PMC->PMC_PCER0 |= 0x00200000L ;             // Enable peripheral clock to SPI

  Spi_complete = 0 ;
  if ( comlen > 4 )
  {
    Spi_complete = 1 ;
    return 0x4FFFF ;
  }
  condition = SPI_SR_TXEMPTY ;
  spiptr = SPI ;
  spiptr->SPI_CR = 1 ;                    // Enable
  (void) spiptr->SPI_RDR ;                // Dump any rx data
  (void) spiptr->SPI_SR ;                 // Clear error flags
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

  spiptr->SPI_PTCR = SPI_PTCR_RXTEN | SPI_PTCR_TXTEN ;    // Start transfers

  // Wait for things to get started, avoids early interrupt
  for ( count = 0 ; count < 1000 ; count += 1 )
  {
    if ( ( spiptr->SPI_SR & SPI_SR_TXEMPTY ) == 0 )
    {
      break ;
    }
  }
  spiptr->SPI_IER = condition ;

#endif

  return 0 ;
}

uint32_t  eeprom_write_one( uint8_t byte, uint8_t count )
{
#ifndef SIMU
  register Spi *spiptr;
  register uint32_t result;

  spiptr = SPI;
  spiptr->SPI_CR = 1; // Enable
  (void) spiptr->SPI_RDR; // Dump any rx data

  spiptr->SPI_TDR = byte;

  result = 0;
  while ((spiptr->SPI_SR & SPI_SR_RDRF) == 0) {
    // wait for received
    if (++result > 10000) {
      break;
    }
  }
  if (count == 0) {
    spiptr->SPI_CR = 2; // Disable
    return spiptr->SPI_RDR;
  }
  (void) spiptr->SPI_RDR; // Dump the rx data
  spiptr->SPI_TDR = 0;
  result = 0;
  while ((spiptr->SPI_SR & SPI_SR_RDRF) == 0) {
    // wait for received
    if (++result > 10000) {
      break;
    }
  }
  spiptr->SPI_CR = 2; // Disable
  return spiptr->SPI_RDR ;
#else
  return !Spi_complete;
#endif
}

void eeprom_write_enable()
{
  eeprom_write_one( 6, 0 ) ;
}

uint32_t eeprom_read_status()
{
  return eeprom_write_one( 5, 1 ) ;
}

// Read eeprom data starting at random address
void read32_eeprom_data(uint32_t eeAddress, register uint8_t *buffer, uint32_t size, uint32_t immediate)
{
#ifdef SIMU
  assert(size);
  eeprom_pointer = eeAddress;
  eeprom_buffer_data = (char*)buffer;
  eeprom_buffer_size = size;
  eeprom_read_operation = true;
  Spi_complete = false;
  sem_post(eeprom_write_sem);
#else
  register uint8_t *p = Spi_tx_buf ;
  *p = 3 ;                     // Read command
  *(p+1) = eeAddress >> 16 ;
  *(p+2) = eeAddress >> 8 ;
  *(p+3) = eeAddress ;	       // 3 bytes address
  spi_PDC_action( p, 0, buffer, 4, size ) ;
#endif

  if (immediate )
    return ;

  while (!Spi_complete) {
#ifdef SIMU
    sleep(5/*ms*/);
#endif
  }
}

void write32_eeprom_block( uint32_t eeAddress, register uint8_t *buffer, uint32_t size, uint32_t immediate )
{
#ifdef SIMU
  assert(size);
  eeprom_pointer = eeAddress;
  eeprom_buffer_data = (char*)buffer;
  eeprom_buffer_size = size+1;
  eeprom_read_operation = false;
  Spi_complete = false;
  sem_post(eeprom_write_sem);
#else
  eeprom_write_enable();

  register uint8_t *p = Spi_tx_buf;
  *p = 2; // Write command
  *(p + 1) = eeAddress >> 16;
  *(p + 2) = eeAddress >> 8;
  *(p + 3) = eeAddress; // 3 bytes address
  spi_PDC_action(p, buffer, 0, 4, size);
#endif

  if (immediate)
    return;

  while (!Spi_complete) {
#ifdef SIMU
    sleep(5/*ms*/);
#endif
  }
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
  read32_eeprom_data( block_no << 12, ( uint8_t *)&b0, sizeof(b0) ) ;		// Sequence # 0
  read32_eeprom_data( (block_no+1) << 12, ( uint8_t *)&b1, sizeof(b1) ) ;	// Sequence # 1

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

bool eeLoadGeneral()
{
  uint16_t size = File_system[0].size;

  memset(&g_eeGeneral, 0, sizeof(EEGeneral));

  if (size > sizeof(EEGeneral)) {
    size = sizeof(EEGeneral) ;
  }

  if (size) {
    read32_eeprom_data( ( File_system[0].block_no << 12) + sizeof( struct t_eeprom_header), ( uint8_t *)&g_eeGeneral, size) ;
  }

  if (g_eeGeneral.version != EEPROM_VER) {
    if (!eeConvert())
      return false;
  }

  uint16_t sum = evalChkSum();
  if (g_eeGeneral.chkSum == sum) {
    return true;
  }

#ifdef SIMU
  printf("EEPROM version %d (%d) instead of %d (%d)\n", g_eeGeneral.version, g_eeGeneral.variant, EEPROM_VER, EEPROM_VARIANT);
  fflush(stdout);
#endif

  return false;
}

void eeLoadModel(uint8_t id)
{
  uint16_t size;

  if (id<MAX_MODELS) {

#if defined(SDCARD)
    closeLogs();
#endif

    if (pulsesStarted()) {
      pausePulses();
    }

    pauseMixerCalculations();

    size =  File_system[id+1].size ;

    memset(&g_model, 0, sizeof(g_model));

#ifdef SIMU
    if (size > 0 && size != sizeof(g_model)) {
      printf("Model data read=%d bytes vs %d bytes\n", size, (int)sizeof(ModelData));
    }
#endif

    if (size > sizeof(g_model)) {
      size = sizeof(g_model) ;
    }
			 
    if(size < 256) { // if not loaded a fair amount
      modelDefault(id) ;
      eeCheck(true);
    }
    else {
      read32_eeprom_data( ( File_system[id+1].block_no << 12) + sizeof( struct t_eeprom_header), ( uint8_t *)&g_model, size) ;
    }

    resetAll();

    if (pulsesStarted()) {
      checkTHR();
      checkSwitches();
      resumePulses();
      clearKeyEvents();
    }

    activeFunctions = 0;
    activeFunctionSwitches = 0;

    resetProto();

    for (uint8_t i=0; i<MAX_TIMERS; i++) {
      if (g_model.timers[i].remanent) {
        s_timerVal[i] = g_model.timers[i].value;
      }
    }

    resumeMixerCalculations();
    // TODO pulses should be started after mixer calculations ...
  }
}

bool eeModelExists(uint8_t id)
{
  return ( File_system[id+1].size > 0 ) ;
}

void ee32LoadModelName(uint8_t id, char *buf, uint8_t len)
{
  if (id < MAX_MODELS) {
    id += 1;
    memset(buf, 0, len);
    if (File_system[id].size > sizeof(g_model.name) ) {
      read32_eeprom_data( ( File_system[id].block_no << 12) + 8, ( uint8_t *)buf, sizeof(g_model.name)) ;
    }
  }
}

void eeReadAll()
{
  if (!eeLoadGeneral() )
  {
    generalDefault();

    ALERT(STR_EEPROMWARN, STR_BADEEPROMDATA, AU_BAD_EEPROM);
    MESSAGE(STR_EEPROMWARN, STR_EEPROMFORMATTING, NULL, AU_EEPROM_FORMATTING);

    modelDefault(0);

    if (check_soft_power() < e_power_usb) { // Usb on or power off
      /* we remove all models */
      for (uint32_t i=0; i<MAX_MODELS; i++)
        eeDeleteModel(i);

      STORE_GENERALVARS;
      STORE_MODELVARS;
    }
    else {
      // we don't want to store anything
      s_eeDirtyMsk = 0;
    }
  }
  else
  {
    eeLoadModel(g_eeGeneral.currModel);
    ee32_read_model_names() ;
  }

  stickMode = g_eeGeneral.stickMode; // TODO common!
}

uint32_t spi_operation( register uint8_t *tx, register uint8_t *rx, register uint32_t count )
{
  register Spi *spiptr;
  register uint32_t result;

//  PMC->PMC_PCER0 |= 0x00200000L ;             // Enable peripheral clock to SPI

  result = 0;
  spiptr = SPI;
  spiptr->SPI_CR = 1; // Enable
  (void) spiptr->SPI_RDR; // Dump any rx data
  while (count) {
    result = 0;
    while ((spiptr->SPI_SR & SPI_SR_TXEMPTY) == 0) {
      // wait
      if (++result > 10000) {
        result = 0xFFFF;
        break;
      }
    }
    if (result > 10000) {
      break;
    }
//              if ( count == 1 )
//              {
//                      spiptr->SPI_CR = SPI_CR_LASTXFER ;              // LastXfer bit
//              }
    spiptr->SPI_TDR = *tx++;
    result = 0;
    while ((spiptr->SPI_SR & SPI_SR_RDRF) == 0) {
      // wait for received
      if (++result > 10000) {
        result = 0x2FFFF;
        break;
      }
    }
    if (result > 10000) {
      break;
    }
    *rx++ = spiptr->SPI_RDR;
    count -= 1;
  }
  if (result <= 10000) {
    result = 0;
  }
  spiptr->SPI_CR = 2; // Disable

// Power save
//  PMC->PMC_PCER0 &= ~0x00200000L ;            // Disable peripheral clock to SPI

  return result ;
}

// SPI i/f to EEPROM (4Mb)
// Peripheral ID 21 (0x00200000)
// Connections:
// SS   PA11 (peripheral A)
// MISO PA12 (peripheral A)
// MOSI PA13 (peripheral A)
// SCK  PA14 (peripheral A)
// Set clock to 3 MHz, AT25 device is rated to 70MHz, 18MHz would be better
void init_spi()
{
  register Spi *spiptr ;
  register uint32_t timer ;
  register uint8_t *p ;
  uint8_t spi_buf[4] ;

  PMC->PMC_PCER0 |= 0x00200000L ;               // Enable peripheral clock to SPI
  /* Configure PIO */
  configure_pins( 0x00007800, PIN_PERIPHERAL | PIN_INPUT | PIN_PER_A | PIN_PORTA | PIN_NO_PULLUP ) ;

  spiptr = SPI ;
  timer = ( Master_frequency / 3000000 ) << 8 ;           // Baud rate 3Mb/s
  spiptr->SPI_MR = 0x14000011 ;                           // 0001 0100 0000 0000 0000 0000 0001 0001 Master
  spiptr->SPI_CSR[0] = 0x01180009 | timer ;               // 0000 0001 0001 1000 xxxx xxxx 0000 1001
  NVIC_EnableIRQ(SPI_IRQn) ;

  p = spi_buf ;

//      *p = 0x39 ;             // Unprotect sector command
//      *(p+1) = 0 ;
//      *(p+2) = 0 ;
//      *(p+3) = 0 ;            // 3 bytes address

//      spi_operation( p, spi_buf, 4 ) ;

  eeprom_write_enable() ;

  *p = 1 ;                // Write status register command
  *(p+1) = 0 ;
  spi_operation( p, spi_buf, 2 ) ;
}

#ifndef SIMU
extern "C" void SPI_IRQHandler()
{
  register Spi *spiptr ;

  spiptr = SPI ;
  SPI->SPI_IDR = 0x07FF ;                 // All interrupts off
  spiptr->SPI_CR = 2 ;                    // Disable
  (void) spiptr->SPI_RDR ;                // Dump any rx data
  (void) spiptr->SPI_SR ;                 // Clear error flags
  spiptr->SPI_PTCR = SPI_PTCR_RXTDIS | SPI_PTCR_TXTDIS ;  // Stop tramsfers
  Spi_complete = 1 ;                                      // Indicate completion

// Power save
//  PMC->PMC_PCER0 &= ~0x00200000L ;      // Disable peripheral clock to SPI
}
#endif

void end_spi()
{
  SPI->SPI_CR = 2 ;                                                               // Disable
  SPI->SPI_IDR = 0x07FF ;                                 // All interrupts off
  NVIC_DisableIRQ(SPI_IRQn) ;
}

void eeWaitFinished()
{
  while (Eeprom32_process_state != E32_IDLE) {
    ee32_process();
#ifdef SIMU
    sleep(5/*ms*/);
#endif
  }
}

void eeCheck(bool immediately)
{
  if (immediately) {
    eeWaitFinished();
  }

  if (s_eeDirtyMsk & EE_GENERAL) {
    s_eeDirtyMsk -= EE_GENERAL;
    Eeprom32_source_address = (uint8_t *)&g_eeGeneral ;               // Get data from here
    Eeprom32_data_size = sizeof(g_eeGeneral) ;                        // This much
    Eeprom32_file_index = 0 ;                                         // This file system entry
    Eeprom32_process_state = E32_BLANKCHECK ;
    if (immediately)
      eeWaitFinished();
    else
      return;
  }

  if (s_eeDirtyMsk & EE_MODEL) {
    s_eeDirtyMsk -= EE_MODEL;
    Eeprom32_source_address = (uint8_t *)&g_model ;           // Get data from here
    Eeprom32_data_size = sizeof(g_model) ;                    // This much
    Eeprom32_file_index = g_eeGeneral.currModel + 1 ;         // This file system entry
    Eeprom32_process_state = E32_BLANKCHECK ;
    if (immediately)
      eeWaitFinished();
  }
}

void ee32_process()
{
  register uint8_t *p ;
  register uint8_t *q ;
  register uint32_t x ;
  register uint32_t eeAddress ;

  if ( Eeprom32_process_state == E32_BLANKCHECK ) {
    eeAddress = File_system[Eeprom32_file_index].block_no ^ 1 ;
    eeAddress <<= 12 ;		                                // Block start address
    Eeprom32_address = eeAddress ;				// Where to put new data
#if 0
    x = Eeprom32_data_size + sizeof( struct t_eeprom_header ) ;	// Size needing to be checked
    p = (uint8_t *) &Eeprom_buffer ;
    read32_eeprom_data( eeAddress, p, x, EE_NOWAIT ) ;
#endif
    Eeprom32_process_state = E32_READSENDING ;
  }

  if ( Eeprom32_process_state == E32_READSENDING )
  {
#if 0
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
        Eeprom32_state_after_erase = E32_IDLE ; // TODO really needed?
        Eeprom32_process_state = E32_WRITESTART ;
      }
      else
      {
#endif
#ifdef SIMU
        Eeprom32_process_state = E32_WRITESTART ;
#else
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
#endif
      // }
   // }
  }

  if ( Eeprom32_process_state == E32_WRITESTART )
  {
    uint32_t total_size ;
    p = Eeprom32_source_address;
    q = (uint8_t *) &Eeprom_buffer.data;
    if (p != q) {
      for (x = 0; x < Eeprom32_data_size; x += 1) {
        *q++ = *p++; // Copy the data to temp buffer
      }
    }
    Eeprom_buffer.header.sequence_no = ++File_system[Eeprom32_file_index].sequence_no;
    File_system[Eeprom32_file_index].size = Eeprom_buffer.header.data_size = Eeprom32_data_size;
    Eeprom_buffer.header.flags = 0;
    Eeprom_buffer.header.hcsum = byte_checksum((uint8_t *) &Eeprom_buffer, 7);
    total_size = Eeprom32_data_size + sizeof(struct t_eeprom_header);
    eeAddress = Eeprom32_address; // Block start address
    x = total_size / 256; // # sub blocks
    x <<= 8; // to offset address
    eeAddress += x; // Add it in
    p = (uint8_t *) &Eeprom_buffer;
    p += x; // Add offset
    x = total_size % 256; // Size of last bit
    if (x == 0) // Last bit empty
    {
      x = 256;
      p -= x;
      eeAddress -= x;
    }
    Eeprom32_buffer_address = p;
    Eeprom32_address = eeAddress;
    write32_eeprom_block(eeAddress, p, x, EE_NOWAIT);
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
        write32_eeprom_block( Eeprom32_address, Eeprom32_buffer_address, 256, EE_NOWAIT ) ;
        Eeprom32_process_state = E32_WRITESENDING ;
      }
      else
      {
        File_system[Eeprom32_file_index].block_no ^= 1 ;        // This is now the current block
#if 0
        // now erase the other block
        File_system[Eeprom32_file_index].block_no ^= 1 ;	// This is now the current block
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
#endif
        Eeprom32_process_state = E32_IDLE ;
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
}

void fill_file_index()
{
  for (uint32_t i = 0 ; i < MAX_MODELS + 1 ; i += 1 )
  {
    File_system[i].block_no = get_current_block_number( i * 2, &File_system[i].size, &File_system[i].sequence_no ) ;
  }
}

void ee32_read_model_names()
{
  for (uint32_t i=0; i<MAX_MODELS; i++)
  {
    ee32LoadModelName(i, ModelNames[i], sizeof(g_model.name));
  }
}

void eeprom_init()
{
#ifndef SIMU
  init_spi() ;
#endif
  fill_file_index() ;
  Eeprom32_process_state = E32_IDLE ;
}

uint32_t unprotect_eeprom()
{
  register uint8_t *p;

  eeprom_write_enable();

  p = Spi_tx_buf;
  *p = 0x39; // Unprotect sector command
  *(p + 1) = 0;
  *(p + 2) = 0;
  *(p + 3) = 0; // 3 bytes address

  return spi_operation( p, Spi_rx_buf, 4 ) ;
}

#if defined(SDCARD)
const pm_char * eeBackupModel(uint8_t i_fileSrc)
{
  char *buf = reusableBuffer.models.mainname;
  FIL archiveFile;
  DIR archiveFolder;
  UINT written;

  if (!sd_card_mounted()) {
    return STR_NO_SDCARD;
  }

  // check and create folder here
  strcpy(buf, STR_MODELS_PATH);
  FRESULT result = f_opendir(&archiveFolder, buf);
  if (result != FR_OK) {
    if (result == FR_NO_PATH)
      result = f_mkdir(buf);
    if (result != FR_OK)
      return SDCARD_ERROR(result);
  }

  buf[sizeof(MODELS_PATH)-1] = '/';
  memcpy(&buf[sizeof(MODELS_PATH)], ModelNames[i_fileSrc], sizeof(g_model.name));
  buf[sizeof(MODELS_PATH)+sizeof(g_model.name)] = '\0';

  uint8_t i = sizeof(MODELS_PATH)+sizeof(g_model.name)-1;
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
    strcpy(&buf[sizeof(MODELS_PATH)], STR_MODEL);
    buf[sizeof(MODELS_PATH) + PSIZE(TR_MODEL)] = (char)((num / 10) + '0');
    buf[sizeof(MODELS_PATH) + PSIZE(TR_MODEL) + 1] = (char)((num % 10) + '0');
    len = sizeof(MODELS_PATH) + PSIZE(TR_MODEL) + 2;
  }

  strcpy(&buf[len], STR_MODELS_EXT);

  result = f_open(&archiveFile, buf, FA_CREATE_ALWAYS | FA_WRITE);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  strcpy(statusLineMsg, PSTR("File "));
  strcpy(statusLineMsg+5, &buf[sizeof(MODELS_PATH)]);

  uint16_t size = File_system[i_fileSrc+1].size;

  *(uint32_t*)&buf[0] = O9X_FOURCC;
  buf[4] = g_eeGeneral.version;
  buf[5] = 'M';
  *(uint16_t*)&buf[6] = size;

  result = f_write(&archiveFile, buf, 8, &written);
  if (result != FR_OK || written != 8) {
    f_close(&archiveFile);
    return SDCARD_ERROR(result);
  }

  read32_eeprom_data( (File_system[i_fileSrc+1].block_no << 12) + sizeof( struct t_eeprom_header), ( uint8_t *)&Eeprom_buffer.data.model_data, size) ;
  result = f_write(&archiveFile, (uint8_t *)&Eeprom_buffer.data.model_data, size, &written);
  f_close(&archiveFile);
  if (result != FR_OK || written != size) {
    return SDCARD_ERROR(result);
  }

  showStatusLine();

  return NULL;
}

const pm_char * eeRestoreModel(uint8_t i_fileDst, char *model_name)
{
  char *buf = reusableBuffer.models.mainname;
  FIL restoreFile;
  UINT read;

  if (!sd_card_mounted()) {
    return STR_NO_SDCARD;
  }

  strcpy(buf, STR_MODELS_PATH);
  buf[sizeof(MODELS_PATH)-1] = '/';
  strcpy(&buf[sizeof(MODELS_PATH)], model_name);
  strcpy(&buf[strlen(buf)], STR_MODELS_EXT);

  FRESULT result = f_open(&restoreFile, buf, FA_OPEN_EXISTING | FA_READ);
  if (result != FR_OK) {
    return SDCARD_ERROR(result);
  }

  if (f_size(&restoreFile) < 8) {
    f_close(&restoreFile);
    return STR_INCOMPATIBLE;
  }

  result = f_read(&restoreFile, (uint8_t *)buf, 8, &read);
  if (result != FR_OK || read != 8) {
    f_close(&restoreFile);
    return SDCARD_ERROR(result);
  }

  if (*(uint32_t*)&buf[0] != O9X_FOURCC || (uint8_t)buf[4] != EEPROM_VER || buf[5] != 'M') {
    f_close(&restoreFile);
    return STR_INCOMPATIBLE;
  }

  uint16_t size = max((uint16_t)sizeof(g_model), *(uint16_t*)&buf[6]);

  if (eeModelExists(i_fileDst)) {
    eeDeleteModel(i_fileDst);
  }

  memset((uint8_t *)&Eeprom_buffer.data.model_data, 0, sizeof(g_model));
  result = f_read(&restoreFile, ( uint8_t *)&Eeprom_buffer.data.model_data, size, &read);
  f_close(&restoreFile);

  if (result != FR_OK || read != read) {
    return STR_INCOMPATIBLE;
  }

  // TODO flash saving ...
  if (read > sizeof(g_model.name))
    memcpy(ModelNames[i_fileDst], Eeprom_buffer.data.model_data.name, sizeof(g_model.name));
  else
    memset(ModelNames[i_fileDst], 0, sizeof(g_model.name));

  Eeprom32_source_address = (uint8_t *)&Eeprom_buffer.data.model_data;    // Get data from here
  Eeprom32_data_size = sizeof(g_model);                                   // This much
  Eeprom32_file_index = i_fileDst + 1;                                    // This file system entry
  Eeprom32_process_state = E32_BLANKCHECK;
  eeWaitFinished();

  return NULL;
}
#endif
