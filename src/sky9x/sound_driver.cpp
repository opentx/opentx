/****************************************************************************
*  Copyright (c) 2011 by Michael Blandford. All rights reserved.
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
*  History:
*
****************************************************************************/

#include "../open9x.h"

volatile uint8_t Buzzer_count ;

// Must NOT be in flash, PDC needs a RAM source.
// Amplitude reduced to 30% to allow for voice volume
uint16_t Sine_values[] =
{
  2048,2085,2123,2160,2197,2233,2268,2303,2336,2369,
  2400,2430,2458,2485,2510,2533,2554,2573,2590,2605,
  2618,2629,2637,2643,2646,2648,2646,2643,2637,2629,
  2618,2605,2590,2573,2554,2533,2510,2485,2458,2430,
  2400,2369,2336,2303,2268,2233,2197,2160,2123,2085,
  2048,2010,1972,1935,1898,1862,1826,1792,1758,1726,
  1695,1665,1637,1610,1585,1562,1541,1522,1505,1490,
  1477,1466,1458,1452,1448,1448,1448,1452,1458,1466,
  1477,1490,1505,1522,1541,1562,1585,1610,1637,1665,
  1695,1726,1758,1792,1826,1862,1898,1935,1972,2010
};

// Sound routines
void startSound()
{
  register Pio *pioptr ;

  start_timer1() ;
  initDac() ;
  init_twi() ;

  pioptr = PIOA ;
#ifdef REVB
  pioptr->PIO_CODR = 0x02000000L ;	// Set bit A25 OFF
  pioptr->PIO_PER = 0x02000000L ;		// Enable bit A25 (Stock buzzer)
  pioptr->PIO_OER = 0x02000000L ;		// Set bit A25 as output
#else
  pioptr->PIO_CODR = 0x00010000L ;	// Set bit A16 OFF
  pioptr->PIO_PER = 0x00010000L ;		// Enable bit A16 (Stock buzzer)
  pioptr->PIO_OER = 0x00010000L ;		// Set bit A16 as output
#endif
}

#ifdef REVB
void buzzer_on()
{
  PIOA->PIO_SODR = 0x02000000L ;	// Set bit A25 ON
}

void buzzer_off()
{
  PIOA->PIO_CODR = 0x02000000L ;	// Set bit A25 ON
}
#else
void buzzer_on()
{
  PIOA->PIO_SODR = 0x00010000L ;	// Set bit A16 ON
}

void buzzer_off()
{
  PIOA->PIO_CODR = 0x00010000L ;	// Set bit A16 ON
}
#endif

void buzzer_sound( uint8_t time )
{
  buzzer_on() ;
  Buzzer_count = time ;
}

void setFrequency( uint32_t frequency )
{
  register Tc *ptc ;
  register uint32_t timer ;

  timer = Master_frequency / (8 * frequency) ;		// MCK/8 and 100 000 Hz
  if (timer > 65535)
    timer = 65535 ;
  if (timer < 2)
    timer = 2 ;

  ptc = TC0 ;		// Tc block 0 (TC0-2)
  ptc->TC_CHANNEL[1].TC_CCR = TC_CCR0_CLKDIS ;		// Stop clock
  ptc->TC_CHANNEL[1].TC_RC = timer ;			// 100 000 Hz
  ptc->TC_CHANNEL[1].TC_RA = timer >> 1 ;
  ptc->TC_CHANNEL[1].TC_CCR = 5 ;		// Enable clock and trigger it (may only need trigger)
}

// Start TIMER1 at 100000Hz, used for DACC trigger
void start_timer1()
{
  register Tc *ptc ;
  register uint32_t timer ;

	// Enable peripheral clock TC0 = bit 23 thru TC5 = bit 28
  PMC->PMC_PCER0 |= 0x01000000L ;		// Enable peripheral clock to TC1
  
  timer = Master_frequency / 800000 ;		// MCK/8 and 100 000 Hz
  ptc = TC0 ;		// Tc block 0 (TC0-2)
  ptc->TC_BCR = 0 ;			// No sync
  ptc->TC_BMR = 0 ;
  ptc->TC_CHANNEL[1].TC_CMR = 0x00008000 ;	// Waveform mode
  ptc->TC_CHANNEL[1].TC_RC = timer ;			// 100 000 Hz
  ptc->TC_CHANNEL[1].TC_RA = timer >> 1 ;
  ptc->TC_CHANNEL[1].TC_CMR = 0x0009C001 ;	// 0000 0000 0000 1001 1100 0000 0000 0001
																						// MCK/8, set @ RA, Clear @ RC waveform
  ptc->TC_CHANNEL[1].TC_CCR = 5 ;		// Enable clock and trigger it (may only need trigger)
  // Sound_g.Frequency = 1000 ;
}


// Configure DAC1 (or DAC0 for REVB)
// Not sure why PB14 has not be allocated to the DAC, although it is an EXTRA function
// So maybe it is automatically done
void initDac()
{
  register Dacc *dacptr ;

  PMC->PMC_PCER0 |= 0x40000000L ;		// Enable peripheral clock to DAC
  dacptr = DACC ;
#ifdef REVB
  dacptr->DACC_MR = 0x0B000215L ;			// 0000 1011 0000 0001 0000 0010 0001 0101
#else
  dacptr->DACC_MR = 0x0B010215L ;			// 0000 1011 0000 0001 0000 0010 0001 0101
#endif
#ifdef REVB
  dacptr->DACC_CHER	= 1 ;							// Enable channel 0
#else
  dacptr->DACC_CHER	= 2 ;							// Enable channel 1
#endif
  dacptr->DACC_CDR = 2048 ;						// Half amplitude
// Data for PDC must NOT be in flash, PDC needs a RAM source.
  dacptr->DACC_TPR = CONVERT_PTR(Sine_values);
  dacptr->DACC_TNPR = CONVERT_PTR(Sine_values);
  dacptr->DACC_TCR = 50 ;       // words, 100 16 bit values
  dacptr->DACC_TNCR = 50 ;	// words, 100 16 bit values
  dacptr->DACC_PTCR = DACC_PTCR_TXTEN ;
  NVIC_EnableIRQ(DACC_IRQn) ;
}

extern "C" void DAC_IRQHandler()
{
// Data for PDC must NOT be in flash, PDC needs a RAM source.
  if (audioQueue.state == AUDIO_PLAYING_WAV) {
    if (nextAudioData) {
      DACC->DACC_TNPR = CONVERT_PTR(nextAudioData);
      DACC->DACC_TNCR = nextAudioSize;
      nextAudioData = NULL;
    }
    else {
      toneStop();
    }
  }
  else {
    DACC->DACC_TNPR = CONVERT_PTR(Sine_values);
    DACC->DACC_TNCR = 50 ;	// words, 100 16 bit values
  }
}

void end_sound()
{
  DACC->DACC_IDR = DACC_IDR_ENDTX ;
  DACC->DACC_IDR = DACC_IDR_TXBUFE ;
  NVIC_DisableIRQ(DACC_IRQn) ;
  TWI0->TWI_IDR = TWI_IDR_TXCOMP ;
  NVIC_DisableIRQ(TWI0_IRQn) ;
  PMC->PMC_PCER0 &= ~0x00080000L ;		// Disable peripheral clock to TWI0
  PMC->PMC_PCER0 &= ~0x40000000L ;		// Disable peripheral clock to DAC
}

// Set up for volume control (TWI0)
// Need PA3 and PA4 set to peripheral A
void init_twi()
{
  register Pio *pioptr ;
  register uint32_t timing ;
  
  PMC->PMC_PCER0 |= 0x00080000L ;		// Enable peripheral clock to TWI0
	
	/* Configure PIO */
  pioptr = PIOA ;
  pioptr->PIO_ABCDSR[0] &= ~0x00000018 ;	// Peripheral A
  pioptr->PIO_ABCDSR[1] &= ~0x00000018 ;	// Peripheral A
  pioptr->PIO_PDR = 0x00000018 ;					// Assign to peripheral
	
  timing = Master_frequency * 5 / 1000000 ;		// 5uS high and low
  timing += 15 - 4 ;
  timing /= 16 ;
  timing |= timing << 8 ;

  TWI0->TWI_CWGR = 0x00040000 | timing ;			// TWI clock set
  TWI0->TWI_CR = TWI_CR_MSEN | TWI_CR_SVDIS ;		// Master mode enable
  TWI0->TWI_MMR = 0x002F0000 ;		// Device 5E (>>1) and master is writing
  NVIC_EnableIRQ(TWI0_IRQn) ;
}

static int8_t Volume_required ;
static uint8_t Volume_read_pending ;
static uint8_t CoProc_read_pending ;
static uint8_t CoProc_write_pending ;
static uint8_t CoProc_appgo_pending ;
uint8_t Volume_read ;
uint8_t Coproc_read ;
int8_t Coproc_valid ;
static uint8_t *Twi_read_address ;
static uint8_t TwiOperation ;

#define TWI_NONE          0
#define TWI_READ_VOL      1
#define TWI_WRITE_VOL     2
#define TWI_READ_COPROC   3
#define TWI_COPROC_APPGO  4
#define TWI_WAIT_STOP     5
#define TWI_WRITE_COPROC  6

// Commands to the coprocessor bootloader/application
#define TWI_CMD_PAGEUPDATE          0x01  // TWI Command to program a flash page
#define TWI_CMD_EXECUTEAPP          0x02  // TWI Command to jump to the application program
#define TWI_CMD_SETREAD_ADDRESS     0x03  // TWI Command to set address to read from
#define TWI_CMD_WRITE_DATA          0x04  // TWI Command send data to the application
#define COPROC_RX_BUXSIZE   22
uint8_t Co_proc_status[COPROC_RX_BUXSIZE] ;
uint8_t *Co_proc_write_ptr ;
uint32_t Co_proc_write_count ;


// This is called from an interrupt routine, or
// interrupts must be disabled while it is called
// from elsewhere.
void i2c_check_for_request()
{
  if ( TWI0->TWI_IMR & TWI_IMR_TXCOMP )
  {
    return ;    // Busy
  }
  
  if ( Volume_required >= 0 )       // Set volume to this value
  {
    TWI0->TWI_MMR = 0x002F0000 ;    // Device 5E (>>1) and master is writing
    TwiOperation = TWI_WRITE_VOL ;
    TWI0->TWI_THR = Volume_required ;   // Send data
    Volume_required = -1 ;
    TWI0->TWI_IER = TWI_IER_TXCOMP ;
    TWI0->TWI_CR = TWI_CR_STOP ;    // Stop Tx
  }
  else if ( CoProc_read_pending )
  {
    Coproc_valid = 0 ;
    CoProc_read_pending = 0 ;
    TWI0->TWI_MMR = 0x00351000 ;    // Device 35 and master is reading
    TwiOperation = TWI_READ_COPROC ;
#ifndef SIMU
    TWI0->TWI_RPR = (uint32_t)&Co_proc_status[0] ;
#endif
    TWI0->TWI_RCR = COPROC_RX_BUXSIZE - 1 ;
    if ( TWI0->TWI_SR & TWI_SR_RXRDY )
    {
      (void) TWI0->TWI_RHR ;
    }
    TWI0->TWI_PTCR = TWI_PTCR_RXTEN ; // Start transfers
    TWI0->TWI_CR = TWI_CR_START ;   // Start Rx
    TWI0->TWI_IER = TWI_IER_RXBUFF | TWI_IER_TXCOMP ;
  }
  else if ( Volume_read_pending )
  {
    Volume_read_pending = 0 ;
    TWI0->TWI_MMR = 0x002F1000 ;    // Device 5E (>>1) and master is reading
    TwiOperation = TWI_READ_VOL ;
    Twi_read_address = &Volume_read ;
    TWI0->TWI_CR = TWI_CR_START | TWI_CR_STOP ;   // Start and stop Tx
    TWI0->TWI_IER = TWI_IER_TXCOMP ;
  }
  else if ( CoProc_appgo_pending )
  {
    CoProc_appgo_pending = 0 ;
    TWI0->TWI_MMR = 0x00350000 ;    // Device 35 and master is writing
    TwiOperation = TWI_COPROC_APPGO ;
    TWI0->TWI_THR = TWI_CMD_EXECUTEAPP ;  // Send appgo command
    TWI0->TWI_IER = TWI_IER_TXCOMP ;
    TWI0->TWI_CR = TWI_CR_STOP ;    // Stop Tx
  }
  else if ( CoProc_write_pending )
  {
    CoProc_write_pending = 0 ;
    TWI0->TWI_MMR = 0x00350000 ;    // Device 35 and master is writing
    TwiOperation = TWI_WRITE_COPROC ;
#ifndef SIMU
    TWI0->TWI_TPR = (uint32_t)Co_proc_write_ptr ;
#endif
    TWI0->TWI_TCR = Co_proc_write_count ;
    TWI0->TWI_THR = TWI_CMD_WRITE_DATA ;  // Send write command
    TWI0->TWI_PTCR = TWI_PTCR_TXTEN ; // Start data transfer
    TWI0->TWI_IER = TWI_IER_TXBUFE | TWI_IER_TXCOMP ;
  }
}

static const uint8_t Volume_scale[NUM_VOL_LEVELS] = 
{
    0,  2,  4,   6,   8,  10,  13,  17,  22,  27,  33,  40,
    64, 82, 96, 105, 112, 117, 120, 122, 124, 125, 126, 127
} ;

void setVolume( unsigned char volume )
{
//  PMC->PMC_PCER0 |= 0x00080000L ;   // Enable peripheral clock to TWI0
  
  if ( volume >= NUM_VOL_LEVELS )
  {
    volume = NUM_VOL_LEVELS - 1 ;   
  }
  volume = Volume_scale[volume] ;
  Volume_required = volume ;
  __disable_irq() ;
  i2c_check_for_request() ;
  __enable_irq() ;
}

void read_volume()
{
  Volume_read_pending = 1 ;
  __disable_irq() ;
  i2c_check_for_request() ;
  __enable_irq() ;
}

void read_coprocessor()
{
  CoProc_read_pending = 1 ;
  __disable_irq() ;
  i2c_check_for_request() ;
  __enable_irq() ;
} 

void write_coprocessor( uint8_t *ptr, uint32_t count )
{
  Co_proc_write_ptr = ptr ;
  Co_proc_write_count = count ;
  CoProc_write_pending = 1 ;
  __disable_irq() ;
  i2c_check_for_request() ;
  __enable_irq() ;
} 

void appgo_coprocessor()
{
  CoProc_appgo_pending = 1 ;
  __disable_irq() ;
  i2c_check_for_request() ;
  __enable_irq() ;
} 

#if !defined(SIMU)
extern "C" void TWI0_IRQHandler()
{
  if ( TwiOperation == TWI_READ_VOL )
  {
    if ( TWI0->TWI_SR & TWI_SR_RXRDY )
    {
      *Twi_read_address = TWI0->TWI_RHR ;   // Read data
    }
  }

  if ( TwiOperation == TWI_READ_COPROC )
  {
    if ( TWI0->TWI_SR & TWI_SR_RXBUFF )
    {
      TWI0->TWI_IDR = TWI_IDR_RXBUFF ;
      TwiOperation = TWI_WAIT_STOP ;
      TWI0->TWI_CR = TWI_CR_STOP ;  // Stop Rx
      TWI0->TWI_RCR = 1 ;           // Last byte
      return ;
    }
    else
    {
      Coproc_valid = -1 ;     
    }
  }   
      
  if ( TwiOperation == TWI_WAIT_STOP )
  {
    Coproc_valid = 1 ;
    Coproc_read = Co_proc_status[0] ;
    if ( Coproc_read & 0x80 ) {
      // Bootloader
      CoProc_appgo_pending = 1 ;  // Action application
    }
    else {
      // Got data from tiny app
      // Set the date and time
      struct gtm utm;
      utm.tm_sec = Co_proc_status[1] ;
      utm.tm_min = Co_proc_status[2] ;
      utm.tm_hour = Co_proc_status[3] ;
      utm.tm_mday = Co_proc_status[4] ;
      utm.tm_mon = Co_proc_status[5] - 1;
      utm.tm_year = (Co_proc_status[6] + ( Co_proc_status[7] << 8 )) - 1900;
      g_rtcTime = gmktime(&utm);
    }
    TWI0->TWI_PTCR = TWI_PTCR_RXTDIS ;  // Stop transfers
    if ( TWI0->TWI_SR & TWI_SR_RXRDY ) {
      (void) TWI0->TWI_RHR ;      // Discard any rubbish data
    }
  }

//  if ( TwiOperation == TWI_WRITE_VOL )
//  {
    
//  }

  if ( TwiOperation == TWI_WRITE_COPROC )
  {
    if ( TWI0->TWI_SR & TWI_SR_TXBUFE )
    {
      TWI0->TWI_IDR = TWI_IDR_TXBUFE ;
      TWI0->TWI_CR = TWI_CR_STOP ;    // Stop Tx
      TWI0->TWI_PTCR = TWI_PTCR_TXTDIS ;  // Stop transfers
      TwiOperation = TWI_NONE ;
      return ;
    }
  }
   
  TWI0->TWI_IDR = TWI_IDR_TXCOMP | TWI_IDR_TXBUFE | TWI_PTCR_TXTDIS ;
  if ( TWI0->TWI_SR & TWI_SR_NACK )
  {
  }
  TwiOperation = TWI_NONE ; 
  i2c_check_for_request() ;
}
#endif


