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
uint16_t Sine_values[] =
{
2048,2173,2298,2422,2545,2666,2784,2899,3011,3119,
3223,3322,3417,3505,3589,3666,3736,3800,3857,3907,
3950,3985,4012,4032,4044,4048,4044,4032,4012,3985,
3950,3907,3857,3800,3736,3666,3589,3505,3417,3322,
3223,3119,3011,2899,2784,2666,2545,2422,2298,2173,
2048,1922,1797,1673,1550,1429,1311,1196,1084, 976,
 872, 773, 678, 590, 506, 429, 359, 295, 238, 188,
 145, 110,  83,  63,  51,  48,  51,  63,  83, 110,
 145, 188, 238, 295, 359, 429, 506, 590, 678, 773,
 872, 976,1084,1196,1311,1429,1550,1673,1797,1922
} ;

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
      CoEnterISR(); // Enter the interrupt
      CoSetFlag(audioFlag);
      CoExitISR(); // Exit the interrupt
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
  setVolume(2) ;
}

static int16_t volumeRequiredIrq ;
static const uint8_t Volume_scale[NUM_VOL_LEVELS] = 
{
    0,  2,  4,   6,   8,  10,  13,  17,  22,  27,  33,  40,
    64, 82, 96, 105, 112, 117, 120, 122, 124, 125, 126, 127
} ;

void setVolume( register uint8_t volume )
{
//	PMC->PMC_PCER0 |= 0x00080000L ;		// Enable peripheral clock to TWI0
	
  if (volume >= NUM_VOL_LEVELS) {
    volume = NUM_VOL_LEVELS - 1 ;
  }

  volume = Volume_scale[volume] ;

  __disable_irq() ;
  if ( TWI0->TWI_IMR & TWI_IMR_TXCOMP ) {
    volumeRequiredIrq = volume ;
  }
  else {
    TWI0->TWI_THR = volume ;		// Send data
    TWI0->TWI_CR = TWI_CR_STOP ;	// Stop Tx
    TWI0->TWI_IER = TWI_IER_TXCOMP ;
  }
  __enable_irq() ;
}

extern "C" void TWI0_IRQHandler()
{
  if ( volumeRequiredIrq >= 0 ) {
    TWI0->TWI_THR = volumeRequiredIrq ;		// Send data
    volumeRequiredIrq = -1 ;
    TWI0->TWI_CR = TWI_CR_STOP ;		// Stop Tx
  }
  else {
    TWI0->TWI_IDR = TWI_IDR_TXCOMP ;
  }
}


