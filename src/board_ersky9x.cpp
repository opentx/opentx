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

#include "open9x.h"

uint32_t Master_frequency ;
volatile uint32_t Tenms ;

/** Console baudrate 9600. */
#define CONSOLE_BAUDRATE    9600
/** Usart Hw interface used by the console (UART0). */
#define CONSOLE_USART       UART0
/** Usart Hw ID used by the console (UART0). */
#define CONSOLE_ID          ID_UART0
/** Pins description corresponding to Rxd,Txd, (UART pins) */
#define CONSOLE_PINS        {PINS_UART}

/** Second serial baudrate 9600. */
#define SECOND_BAUDRATE    9600
/** Usart Hw interface used by the console (UART0). */
#define SECOND_USART       USART0
/** Usart Hw ID used by the console (UART0). */
#define SECOND_ID          ID_USART0
/** Pins description corresponding to Rxd,Txd, (UART pins) */
#define SECOND_PINS        {PINS_USART0}

#define BT_USART       UART1
#define BT_ID          ID_UART1

#define CPU_INT         int32_t
#define CPU_UINT        uint32_t

#ifdef REVB
inline void init_soft_power()
{
  register Pio *pioptr ;

  pioptr = PIOC ;
  // Configure RF_power (PC17) but not PPM-jack-in (PC22), neither need pullups
  pioptr->PIO_PER = PIO_PC17 ;            // Enable bit C17
  pioptr->PIO_ODR = PIO_PC17 ;            // Set bit C17 as input

  pioptr = PIOA ;
  pioptr->PIO_PER = PIO_PA8 ;             // Enable bit A8 (Soft Power)
  pioptr->PIO_ODR = PIO_PA8 ;             // Set bit A8 as input
  pioptr->PIO_PUER = PIO_PA8 ;    // Enable PA8 pullup
}
#endif

// Prototype
// Free pins (PA16 is stock buzzer)
// PA23, PA24, PA25, PB7, PB13
// PC20, PC21(labelled 17), PC22, PC24
// REVB
// PA25, use for stock buzzer
// PB14, PB6
// PC21, PC19, PC15 (PPM2 output)
inline void config_free_pins()
{
  register Pio *pioptr ;

#ifdef REVB
  pioptr = PIOB ;
  pioptr->PIO_PER = 0x00004040L ;         // Enable bits B14, 6
  pioptr->PIO_ODR = 0x00004040L ;         // Set as input
  pioptr->PIO_PUER = 0x00004040L ;        // Enable pullups

  pioptr = PIOC ;
  pioptr->PIO_PER = 0x00280000L ;         // Enable bits C21, 19
  pioptr->PIO_ODR = 0x00280000L ;         // Set as input
  pioptr->PIO_PUER = 0x00280000L ;        // Enable pullups
#else
  pioptr = PIOA ;
  pioptr->PIO_PER = 0x03800000L ;         // Enable bits A25,24,23
  pioptr->PIO_ODR = 0x03800000L ;         // Set as input
  pioptr->PIO_PUER = 0x03800000L ;        // Enable pullups

  pioptr = PIOB ;
  pioptr->PIO_PER = 0x00002080L ;         // Enable bits B13, 7
  pioptr->PIO_ODR = 0x00002080L ;         // Set as input
  pioptr->PIO_PUER = 0x00002080L ;        // Enable pullups

  pioptr = PIOC ;
  pioptr->PIO_PER = 0x01700000L ;         // Enable bits C24,22,21,20
  pioptr->PIO_ODR = 0x01700000L ;         // Set as input
  pioptr->PIO_PUER = 0x01700000L ;        // Enable pullups
#endif
}

// Assumes PMC has already enabled clocks to ports
inline void setup_switches()
{
  register Pio *pioptr ;

  pioptr = PIOA ;
#ifdef REVB
  pioptr->PIO_PER = 0x01808087 ;          // Enable bits
  pioptr->PIO_ODR = 0x01808087 ;          // Set bits input
  pioptr->PIO_PUER = 0x01808087 ;         // Set bits with pullups
#else
  pioptr->PIO_PER = 0xF8008184 ;          // Enable bits
  pioptr->PIO_ODR = 0xF8008184 ;          // Set bits input
  pioptr->PIO_PUER = 0xF8008184 ;         // Set bits with pullups
#endif
  pioptr = PIOB ;
#ifdef REVB
  pioptr->PIO_PER = 0x00000030 ;          // Enable bits
  pioptr->PIO_ODR = 0x00000030 ;          // Set bits input
  pioptr->PIO_PUER = 0x00000030 ;         // Set bits with pullups
#else
  pioptr->PIO_PER = 0x00000010 ;          // Enable bits
  pioptr->PIO_ODR = 0x00000010 ;          // Set bits input
  pioptr->PIO_PUER = 0x00000010 ;         // Set bits with pullups
#endif

  pioptr = PIOC ;
#ifdef REVB
  pioptr->PIO_PER = 0x91114900 ;          // Enable bits
  pioptr->PIO_ODR = 0x91114900 ;          // Set bits input
  pioptr->PIO_PUER = 0x91114900 ;         // Set bits with pullups
#else
  pioptr->PIO_PER = 0x10014900 ;          // Enable bits
  pioptr->PIO_ODR = 0x10014900 ;          // Set bits input
  pioptr->PIO_PUER = 0x10014900 ;         // Set bits with pullups
#endif
}

#ifndef SIMU

/**
 * Configures a UART peripheral with the specified parameters.
 *
 * baudrate  Baudrate at which the UART should operate (in Hz).
 * masterClock  Frequency of the system master clock (in Hz).
 * uses PA9 and PA10, RXD2 and TXD2
 */
inline void UART_Configure( uint32_t baudrate, uint32_t masterClock)
{
//    const Pin pPins[] = CONSOLE_PINS;
  register Uart *pUart = CONSOLE_USART;
  register Pio *pioptr ;

  /* Configure PIO */
  pioptr = PIOA ;
  pioptr->PIO_ABCDSR[0] &= ~0x00000600 ;        // Peripheral A
  pioptr->PIO_ABCDSR[1] &= ~0x00000600 ;        // Peripheral A
  pioptr->PIO_PDR = 0x00000600 ;                                        // Assign to peripheral

  /* Configure PMC */
  PMC->PMC_PCER0 = 1 << CONSOLE_ID;

  /* Reset and disable receiver & transmitter */
  pUart->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX
                 | UART_CR_RXDIS | UART_CR_TXDIS;

  /* Configure mode */
  pUart->UART_MR =  0x800 ;  // NORMAL, No Parity

  /* Configure baudrate */
  /* Asynchronous, no oversampling */
  pUart->UART_BRGR = (masterClock / baudrate) / 16;

  /* Disable PDC channel */
  pUart->UART_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS;

  /* Enable receiver and transmitter */
  pUart->UART_CR = UART_CR_RXEN | UART_CR_TXEN;
}

inline void UART3_Configure( uint32_t baudrate, uint32_t masterClock)
{
//    const Pin pPins[] = CONSOLE_PINS;
  register Uart *pUart = BT_USART;
  register Pio *pioptr ;

  /* Configure PIO */
  pioptr = PIOB ;
  pioptr->PIO_ABCDSR[0] &= ~(PIO_PB2 | PIO_PB3) ;       // Peripheral A
  pioptr->PIO_ABCDSR[1] &= ~(PIO_PB2 | PIO_PB3) ;       // Peripheral A
  pioptr->PIO_PDR = (PIO_PB2 | PIO_PB3) ;                                       // Assign to peripheral

  /* Configure PMC */
  PMC->PMC_PCER0 = 1 << BT_ID;

  /* Reset and disable receiver & transmitter */
  pUart->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX
                 | UART_CR_RXDIS | UART_CR_TXDIS;

  /* Configure mode */
  pUart->UART_MR =  0x800 ;  // NORMAL, No Parity

  /* Configure baudrate */
  /* Asynchronous, no oversampling */
  pUart->UART_BRGR = (masterClock / baudrate) / 16;

//      baudrate = (masterClock * 8 / baudrate) / 16 ;
//  pUart->UART_BRGR = ( baudrate / 8 ) || ( ( baudrate & 7 ) << 16 ) ; // Fractional part to allow 152000 baud
//
  /* Disable PDC channel */
  pUart->UART_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS;

  /* Enable receiver and transmitter */
  pUart->UART_CR = UART_CR_RXEN | UART_CR_TXEN;
}

// USART0 configuration
// Work in Progress, UNTESTED
// Uses PA5 and PA6 (RXD and TXD)
inline void UART2_Configure( uint32_t baudrate, uint32_t masterClock)
{
////    const Pin pPins[] = CONSOLE_PINS;
  register Usart *pUsart = SECOND_USART;
  register Pio *pioptr ;

  /* Configure PIO */
  pioptr = PIOA ;
  pioptr->PIO_ABCDSR[0] &= ~0x00000060 ;        // Peripheral A
  pioptr->PIO_ABCDSR[1] &= ~0x00000060 ;        // Peripheral A
  pioptr->PIO_PDR = 0x00000060 ;                                        // Assign to peripheral

//  /* Configure PMC */
  PMC->PMC_PCER0 = 1 << SECOND_ID;

//  /* Reset and disable receiver & transmitter */
  pUsart->US_CR = US_CR_RSTRX | US_CR_RSTTX
                         | US_CR_RXDIS | US_CR_TXDIS;

//  /* Configure mode */
  pUsart->US_MR =  0x000008C0 ;  // NORMAL, No Parity

//  /* Configure baudrate */
//  /* Asynchronous, no oversampling */
  pUsart->US_BRGR = (masterClock / baudrate) / 16;

//  /* Disable PDC channel */
  pUsart->US_PTCR = US_PTCR_RXTDIS | US_PTCR_TXTDIS;

//  /* Enable receiver and transmitter */
  pUsart->US_CR = US_CR_RXEN | US_CR_TXEN;
}

// Starts TIMER2 at 100Hz,  commentd out drive of TIOA2 (A26, EXT2) out
inline void start_timer2()
{
  register Pio *pioptr ;
  register Tc *ptc ;
  register uint32_t timer ;

  // Enable peripheral clock TC0 = bit 23 thru TC5 = bit 28
  PMC->PMC_PCER0 |= 0x02000000L ;               // Enable peripheral clock to TC2

  timer = Master_frequency / 12800 ;              // MCK/128 and 100 Hz

  ptc = TC0 ;           // Tc block 0 (TC0-2)
  ptc->TC_BCR = 0 ;                       // No sync
  ptc->TC_BMR = 0 ;
  ptc->TC_CHANNEL[2].TC_CMR = 0x00008000 ;        // Waveform mode
  ptc->TC_CHANNEL[2].TC_RC = timer ;                      // 10 Hz
  ptc->TC_CHANNEL[2].TC_RA = timer >> 1 ;
  ptc->TC_CHANNEL[2].TC_CMR = 0x0009C003 ;        // 0000 0000 0000 1001 1100 0000 0000 0011
                                                                                                                                                                                // MCK/128, set @ RA, Clear @ RC waveform

  pioptr = PIOC ;
  pioptr->PIO_PER = 0x00080000L ;         // Enable bits C19
  pioptr->PIO_OER = 0x00080000L ;         // Set as output
  ptc->TC_CHANNEL[2].TC_CCR = 5 ;         // Enable clock and trigger it (may only need trigger)

  NVIC_EnableIRQ(TC2_IRQn) ;
  TC0->TC_CHANNEL[2].TC_IER = TC_IER0_CPCS ;
}

// Test, starts TIMER0 at full speed (MCK/2) for delay timing
inline void start_timer0()
{
  register Tc *ptc ;

  // Enable peripheral clock TC0 = bit 23 thru TC5 = bit 28
  PMC->PMC_PCER0 |= 0x00800000L ;               // Enable peripheral clock to TC0

  ptc = TC0 ;           // Tc block 0 (TC0-2)
  ptc->TC_BCR = 0 ;                       // No sync
  ptc->TC_BMR = 2 ;
  ptc->TC_CHANNEL[0].TC_CMR = 0x00008000 ;        // Waveform mode
  ptc->TC_CHANNEL[0].TC_RC = 0xFFF0 ;
  ptc->TC_CHANNEL[0].TC_RA = 0 ;
  ptc->TC_CHANNEL[0].TC_CMR = 0x00008040 ;        // 0000 0000 0000 0000 1000 0000 0100 0000, stop at regC
  ptc->TC_CHANNEL[0].TC_CCR = 5 ;         // Enable clock and trigger it (may only need trigger)
}

extern "C" void TC2_IRQHandler()
{
  register uint32_t dummy;
        static uint32_t pre_scale ;             // Used to get 10 Hz counter

  /* Clear status bit to acknowledge interrupt */
  dummy = TC0->TC_CHANNEL[2].TC_SR;
        (void) dummy ;          // Discard value - prevents compiler warning

        /* TODO if ( Buzzer_count )
        {
                if ( --Buzzer_count == 0 )
                {
                        buzzer_off() ;
                }
        } */

        if ( ++pre_scale >= 10 )
        {
                // TODO needed? Timer2_count += 1 ;
                pre_scale = 0 ;
        }
  per10ms();

//      heartbeat |= HEART_TIMER10ms;
}

// Settings for mode register ADC_MR
// USEQ off - silicon problem, doesn't work
// TRANSFER = 1
// TRACKTIM = 4 (5 clock periods)
// ANACH = 0
// SETTLING = 1 (not used if ANACH = 0)
// STARTUP = 1 (8 clock periods)
// PRESCAL = 3.6 MHz clock (between 1 and 20MHz)
// FREERUN = 0
// FWUP = 0
// SLEEP = 0
// LOWRES = 0
// TRGSEL = 0
// TRGEN = 0 (software trigger only)
inline void init_adc()
{
  register Adc *padc ;
  register uint32_t timer ;

  timer = ( Master_frequency / (3600000*2) ) << 8 ;
  // Enable peripheral clock ADC = bit 29
  PMC->PMC_PCER0 |= 0x20000000L ;               // Enable peripheral clock to ADC
  padc = ADC ;
  padc->ADC_MR = 0x14110000 | timer ;  // 0001 0100 0001 0001 xxxx xxxx 0000 0000
#ifdef REVB
  padc->ADC_CHER = 0x0000633E ;  // channels 1,2,3,4,5,8,9,13,14
#else
  padc->ADC_CHER = 0x0000623E ;  // channels 1,2,3,4,5,9,13,14
#endif
  padc->ADC_CGR = 0 ;  // Gain = 1, all channels
  padc->ADC_COR = 0 ;  // Single ended, 0 offset, all channels
}

// PWM used for PPM generation, and LED Backlight
// Output pin PB5 not used, PA17 used as PWMH3 peripheral C
// PWM peripheral ID = 31 (0x80000000)
// Ensure PB5 is three state/input, used on REVB for MENU KEY

// Configure PWM3 as PPM drive,
// PWM0 is LED backlight PWM on PWMH0
// This is PC18 peripheral B, Also enable PC22 peripheral B, this is PPM-JACK (PWML3)
//
// REVB board:
// PWML2, output as peripheral C on PA16, is for HAPTIC
// For testing, just drive it out with PWM
// PWML1 for PPM2 output as peripheral B on PC15
// For testing, just drive it out with PWM
void init_pwm()
{
  register Pio *pioptr ;
  register Pwm *pwmptr ;
  register uint32_t timer ;

  PMC->PMC_PCER0 |= ( 1 << ID_PWM ) ;           // Enable peripheral clock to PWM

  MATRIX->CCFG_SYSIO |= 0x00000020L ;                             // Disable TDO let PB5 work!

  /* Configure PIO */
#ifndef REVB
  pioptr = PIOB ;
  pioptr->PIO_PER = 0x00000020L ;         // Enable bit B5
  pioptr->PIO_ODR = 0x00000020L ;         // set as input
#endif

#ifdef REVB
  pioptr = PIOB ;
  pioptr->PIO_ABCDSR[0] &= ~PIO_PA16 ;          // Peripheral C
  pioptr->PIO_ABCDSR[1] |= PIO_PA16 ;                   // Peripheral C
  pioptr->PIO_PDR = PIO_PA16 ;                                            // Disable bit A16 Assign to peripheral
#endif

  pioptr = PIOC ;
  pioptr->PIO_ABCDSR[0] |= PIO_PC18 ;                   // Peripheral B
  pioptr->PIO_ABCDSR[1] &= ~PIO_PC18 ;          // Peripheral B
  pioptr->PIO_PDR = PIO_PC18 ;                                            // Disable bit C18 Assign to peripheral

#ifdef REVB
  pioptr->PIO_ABCDSR[0] |= PIO_PC15 ;                   // Peripheral B
  pioptr->PIO_ABCDSR[1] &= ~PIO_PC15 ;          // Peripheral B
  pioptr->PIO_PDR = PIO_PC15 ;                                            // Disable bit C15 Assign to peripheral
#endif

#ifdef REVB
  pioptr->PIO_ABCDSR[0] |= PIO_PC22 ;                   // Peripheral B
  pioptr->PIO_ABCDSR[1] &= ~PIO_PC22 ;          // Peripheral B
  pioptr->PIO_PDR = PIO_PC22 ;                                            // Disable bit C22 Assign to peripheral
#endif

  // Configure clock - depends on MCK frequency
  timer = Master_frequency / 2000000 ;
  timer |= ( Master_frequency / ( 32* 10000 ) ) << 16 ;
  timer &= 0x00FF00FF ;

  pwmptr = PWM ;
  pwmptr->PWM_CLK = 0x05000000 | timer ;  // MCK for DIVA, DIVA = 18 gives 0.5uS clock period @35MHz                                                                                                                                              // MCK/32 / timer = 10000Hz for CLKB

  // PWM0 for LED backlight
  pwmptr->PWM_CH_NUM[0].PWM_CMR = 0x0000000C ;    // CLKB
  pwmptr->PWM_CH_NUM[0].PWM_CPDR = 100 ;                  // Period
  pwmptr->PWM_CH_NUM[0].PWM_CPDRUPD = 100 ;               // Period
  pwmptr->PWM_CH_NUM[0].PWM_CDTY = 40 ;                           // Duty
  pwmptr->PWM_CH_NUM[0].PWM_CDTYUPD = 40 ;                // Duty
  pwmptr->PWM_ENA = PWM_ENA_CHID0 ;                                               // Enable channel 0

#ifdef REVB
  // PWM1 for PPM2 output 100Hz test
  pwmptr->PWM_CH_NUM[1].PWM_CMR = 0x0000000C ;    // CLKB
  pwmptr->PWM_CH_NUM[1].PWM_CPDR = 100 ;                  // Period
  pwmptr->PWM_CH_NUM[1].PWM_CPDRUPD = 100 ;               // Period
  pwmptr->PWM_CH_NUM[1].PWM_CDTY = 40 ;                           // Duty
  pwmptr->PWM_CH_NUM[1].PWM_CDTYUPD = 40 ;                // Duty
  pwmptr->PWM_ENA = PWM_ENA_CHID1 ;                                               // Enable channel 1
#endif

#ifdef REVB
  // PWM2 for HAPTIC drive 100Hz test
  pwmptr->PWM_CH_NUM[2].PWM_CMR = 0x0000000C ;    // CLKB
  pwmptr->PWM_CH_NUM[2].PWM_CPDR = 100 ;                  // Period
  pwmptr->PWM_CH_NUM[2].PWM_CPDRUPD = 100 ;               // Period
  pwmptr->PWM_CH_NUM[2].PWM_CDTY = 40 ;                           // Duty
  pwmptr->PWM_CH_NUM[2].PWM_CDTYUPD = 40 ;                // Duty
  pwmptr->PWM_ENA = PWM_ENA_CHID2 ;                                               // Enable channel 2
#endif
}

// LCD i/o pins
// LCD_RES     PC27
// LCD_CS1     PC26
// LCD_E       PC12
// LCD_RnW     PC13
// LCD_A0      PC15
// LCD_D0      PC0
// LCD_D1      PC7
// LCD_D2      PC6
// LCD_D3      PC5
// LCD_D4      PC4
// LCD_D5      PC3
// LCD_D6      PC2
// LCD_D7      PC1

#define LCD_DATA        0x000000FFL
#ifdef REVB
#define LCD_A0    0x00000080L
#else
#define LCD_A0    0x00008000L
#endif
#define LCD_RnW   0x00002000L
#define LCD_E     0x00001000L
#define LCD_CS1   0x04000000L
#define LCD_RES   0x08000000L

extern void start_sound(); // TODO elsewhere

void board_init()
{
  // register uint32_t goto_usb ;
  register Pio *pioptr ;
  // Debug variable
  // uint32_t both_on ;

  WDT->WDT_MR = 0x3FFFAFFF ;                      // Disable watchdog

  MATRIX->CCFG_SYSIO |= 0x000000F0L ;             // Disable syspins, enable B4,5,6,7

  PMC->PMC_PCER0 = (1<<ID_PIOC)|(1<<ID_PIOB)|(1<<ID_PIOA)|(1<<ID_UART0) ;                               // Enable clocks to PIOB and PIOA and PIOC and UART0
  pioptr = PIOA ;
#ifdef REVB
  init_soft_power() ;
#else
  // On REVB, PA21 is used as AD8, and measures current consumption.
  pioptr->PIO_PER = PIO_PA21 ;            // Enable bit A21 (EXT3)
  pioptr->PIO_OER = PIO_PA21 ;            // Set bit A21 as output
  pioptr->PIO_SODR = PIO_PA21 ;   // Set bit A21 ON
#endif

  // pioptr->PIO_PUER = 0x80000000 ;         // Enable pullup on bit A31 (EXIT)
  // pioptr->PIO_PER = 0x80000000 ;          // Enable bit A31

  pioptr = PIOC ;
  pioptr->PIO_PER = PIO_PC25 ;            // Enable bit C25 (USB-detect)
  // pioptr->PIO_OER = 0x80000000L ;         // Set bit C31 as output
  // pioptr->PIO_SODR = 0x80000000L ;        // Set bit C31

#ifndef REVB
  // Configure RF_power (PC17) and PPM-jack-in (PC19), neither need pullups
  pioptr->PIO_PER = 0x000A0000L ;         // Enable bit C19, C17
  pioptr->PIO_ODR = 0x000A0000L ;         // Set bits C19 and C17 as input
#endif

  config_free_pins() ;

  // Next section configures the key inputs on the LCD data
#ifdef REVB
  pioptr->PIO_PER = 0x0000003BL ;         // Enable bits 1,3,4,5, 0
  pioptr->PIO_OER = PIO_PC0 ;             // Set bit 0 output
  pioptr->PIO_ODR = 0x0000003AL ;         // Set bits 1, 3, 4, 5 input
  pioptr->PIO_PUER = 0x0000003AL ;                // Set bits 1, 3, 4, 5 with pullups
#else
  pioptr->PIO_PER = 0x0000003DL ;         // Enable bits 2,3,4,5, 0
  pioptr->PIO_OER = PIO_PC0 ;             // Set bit 0 output
  pioptr->PIO_ODR = 0x0000003CL ;         // Set bits 2, 3, 4, 5 input
  pioptr->PIO_PUER = 0x0000003CL ;                // Set bits 2, 3, 4, 5 with pullups
#endif

  pioptr = PIOB ;
#ifdef REVB
  pioptr->PIO_PUER = PIO_PB5 ;                                    // Enable pullup on bit B5 (MENU)
  pioptr->PIO_PER = PIO_PB5 ;                                     // Enable bit B5
#else
  pioptr->PIO_PUER = PIO_PB6 ;                                    // Enable pullup on bit B6 (MENU)
  pioptr->PIO_PER = PIO_PB6 ;                                     // Enable bit B6
#endif

  setup_switches() ;

  // Enable PCK2 on PB3, This is for testing of Timer 2 working
  // It will be used as serial data to the Bluetooth module
  pioptr->PIO_ABCDSR[0] |=  PIO_PB3 ;     // Peripheral B
  pioptr->PIO_ABCDSR[1] &= ~PIO_PB3 ;   // Peripheral B
  pioptr->PIO_PDR = PIO_PB3 ;                                   // Assign to peripheral
  PMC->PMC_SCER |= 0x0400 ;                                                               // PCK2 enabled
  PMC->PMC_PCK[2] = 2 ;                                                                           // PCK2 is PLLA

  UART_Configure( 9600, Master_frequency ) ;
  UART2_Configure( 9600, Master_frequency ) ;             // Testing
  UART3_Configure( 9600, Master_frequency ) ;             // Testing

  start_timer2() ;
  start_timer0() ;
  init_adc() ;
  init_pwm() ;

  __enable_irq() ;

  start_sound() ;

  eeprom_init();
}


#endif

uint16_t getTmr2MHz()
{
  return TC1->TC_CHANNEL[0].TC_CV ;
}

// keys:
// KEY_EXIT    PA31 (PC24)
// KEY_MENU    PB6 (PB5)
// KEY_DOWN  LCD5  PC3 (PC5)
// KEY_UP    LCD6  PC2 (PC1)
// KEY_RIGHT LCD4  PC4 (PC4)
// KEY_LEFT  LCD3  PC5 (PC3)
// Reqd. bit 6 LEFT, 5 RIGHT, 4 UP, 3 DOWN 2 EXIT 1 MENU
// LCD pins 5 DOWN, 4 RIGHT, 3 LEFT, 1 UP
uint32_t read_keys()
{
  register uint32_t x;
  register uint32_t y;

  x = PIOC->PIO_PDSR << 1; // 6 LEFT, 5 RIGHT, 4 DOWN, 3 UP ()

#ifdef REVB
  y = x & 0x00000020; // RIGHT
  if ( x & 0x00000004 )
  {
    y |= 0x00000010; // UP
  }
  if ( x & 0x00000010 )
  {
    y |= 0x00000040; // LEFT
  }
  if ( x & 0x00000040 )
  {
    y |= 0x00000008; // DOWN
  }
#else

  y = x & 0x00000060;
  if (x & 0x00000008) {
    y |= 0x00000010;
  }
  if (x & 0x00000010) {
    y |= 0x00000008;
  }
#endif
#ifdef REVB
  if ( PIOC->PIO_PDSR & 0x01000000 )
#else
  if (PIOA->PIO_PDSR & 0x80000000)
#endif
  {
    y |= 4; // EXIT
  }
#ifdef REVB
  if ( PIOB->PIO_PDSR & 0x000000020 )
#else
  if (PIOB->PIO_PDSR & 0x000000040)
#endif
  {
    y |= 2; // MENU
  }

  return y ;
}



uint32_t read_trims()
{
  uint32_t trims = 0;

// TRIM_LH_DOWN    PA7 (PA23)
#ifdef REVB
  if ( ( PIOA->PIO_PDSR & 0x00800000 ) == 0 )
#else
  if ((PIOA->PIO_PDSR & 0x0080) == 0)
#endif
      {
    trims |= 1;
  }

// TRIM_LH_UP PB4
  if ((PIOB->PIO_PDSR & 0x10) == 0) {
    trims |= 2;
  }

// TRIM_LV_DOWN  PA27 (PA24)
#ifdef REVB
  if ( ( PIOA->PIO_PDSR & 0x01000000 ) == 0 )
#else
  if ((PIOA->PIO_PDSR & 0x08000000) == 0)
#endif
      {
    trims |= 4;
  }

// TRIM_LV_UP   PC28
  if ((PIOC->PIO_PDSR & 0x10000000) == 0) {
    trims |= 8;
  }

// TRIM_RV_DOWN   PC10
  if ((PIOC->PIO_PDSR & 0x00000400) == 0) {
    trims |= 0x10;
  }

// TRIM_RV_UP    PA30 (PA1)
#ifdef REVB
  if ( ( PIOA->PIO_PDSR & 0x00000002 ) == 0 )
#else
  if ((PIOA->PIO_PDSR & 0x40000000) == 0)
#endif
      {
    trims |= 0x20;
  }

// TRIM_RH_DOWN    PA29 (PA0)
#ifdef REVB
  if ( ( PIOA->PIO_PDSR & 0x00000001 ) == 0 )
#else
  if ((PIOA->PIO_PDSR & 0x20000000) == 0)
#endif
      {
    trims |= 0x40;
  }

// TRIM_RH_UP   PC9
  if ((PIOC->PIO_PDSR & 0x00000200) == 0) {
    trims |= 0x80;
  }

  return trims ;
}

uint8_t keyDown()
{
  return ~read_keys() & 0x7E ;
}

extern uint32_t keyState(EnumKeys enuk)
{
  CPU_UINT xxx = 0;

  if (enuk < (int) DIM(keys)) return keys[enuk].state() ? 1 : 0;

  switch ((uint8_t) enuk) {
#ifdef REVB
    case SW_ElevDR : xxx = PIOC->PIO_PDSR & 0x80000000; // ELE_DR   PC31
#else
    case SW_ElevDR:
      xxx = PIOA->PIO_PDSR & 0x00000100; // ELE_DR   PA8
#endif
      break;

    case SW_AileDR:
      xxx = PIOA->PIO_PDSR & 0x00000004; // AIL-DR  PA2
      break;

    case SW_RuddDR:
      xxx = PIOA->PIO_PDSR & 0x00008000; // RUN_DR   PA15
      break;
      //     INP_G_ID1 INP_E_ID2
      // id0    0        1
      // id1    1        1
      // id2    1        0
    case SW_ID0:
      xxx = ~PIOC->PIO_PDSR & 0x00004000; // SW_IDL1     PC14
      break;
    case SW_ID1:
      xxx = (PIOC->PIO_PDSR & 0x00004000);
      if (xxx) xxx = (PIOC->PIO_PDSR & 0x00000800);
      break;
    case SW_ID2:
      xxx = ~PIOC->PIO_PDSR & 0x00000800; // SW_IDL2     PC11
      break;

    case SW_Gear:
      xxx = PIOC->PIO_PDSR & 0x00010000; // SW_GEAR     PC16
      break;

#ifdef REVB
      case SW_ThrCt : xxx = PIOC->PIO_PDSR & 0x00100000; // SW_TCUT     PC20
#else
    case SW_ThrCt:
      xxx = PIOA->PIO_PDSR & 0x10000000; // SW_TCUT     PA28
#endif
      break;

    case SW_Trainer:
      xxx = PIOC->PIO_PDSR & 0x00000100; // SW-TRAIN    PC8
      break;

    default:
      break;
  }

  if (xxx) {
    return 1;
  }
  return 0;
}

uint16_t Analog_values[NUMBER_ANALOG] ;

// Read 8 (9 for REVB) ADC channels
// Documented bug, must do them 1 by 1
void read_9_adc()
{
  register Adc *padc;
  register uint32_t y;
  register uint32_t x;

//      PMC->PMC_PCER0 |= 0x20000000L ;         // Enable peripheral clock to ADC

  padc = ADC;
  y = padc->ADC_ISR; // Clear EOC flags
  for (y = NUMBER_ANALOG; --y > 0;) {
    padc->ADC_CR = 2; // Start conversion
    x = 0;
    while ((padc->ADC_ISR & 0x01000000) == 0) {
      // wait for DRDY flag
      if (++x > 1000000) {
        break; // Software timeout
      }
    }
    x = padc->ADC_LCDR; // Clear DRSY flag
  }
  // Next bit may be done using the PDC
  Analog_values[0] = ADC->ADC_CDR1;
  Analog_values[1] = ADC->ADC_CDR2;
  Analog_values[2] = ADC->ADC_CDR3;
  Analog_values[3] = ADC->ADC_CDR4;
  Analog_values[4] = ADC->ADC_CDR5;
  Analog_values[5] = ADC->ADC_CDR9;
  Analog_values[6] = ADC->ADC_CDR13;
  Analog_values[7] = ADC->ADC_CDR14;
#ifdef REVB
  Analog_values[8] = ADC->ADC_CDR8 ;
#endif

// Power save
//  PMC->PMC_PCER0 &= ~0x20000000L ;            // Disable peripheral clock to ADC
}

void readKeysAndTrims()
{
  register uint32_t i;

  if (PIOC->PIO_ODSR & 0x00080000) {
    PIOC->PIO_CODR = 0x00200000L; // Set bit C19 OFF
  }
  else {
    PIOC->PIO_SODR = 0x00200000L; // Set bit C19 ON
  }

  uint8_t enuk = KEY_MENU;
  uint8_t in = ~read_keys();
  for (i = 1; i < 7; i++) {
//INP_B_KEY_MEN 1  .. INP_B_KEY_LFT 6
    keys[enuk].input(in & (1 << i), (EnumKeys) enuk);
    ++enuk;
  }
//  static const uint8_t crossTrim[]={
//    1<<INP_D_TRM_LH_DWN,
//    1<<INP_D_TRM_LH_UP,
//    1<<INP_D_TRM_LV_DWN,
//    1<<INP_D_TRM_LV_UP,
//    1<<INP_D_TRM_RV_DWN,
//    1<<INP_D_TRM_RV_UP,
//    1<<INP_D_TRM_RH_DWN,
//    1<<INP_D_TRM_RH_UP
//  };
  in = read_trims();

  for (i = 1; i < 256; i <<= 1) {
// INP_D_TRM_RH_UP   0 .. INP_D_TRM_LH_UP   7
    keys[enuk].input(in & i, (EnumKeys) enuk);
    ++enuk;
  }
}

void end_ppm_capture()
{
        TC1->TC_CHANNEL[0].TC_IDR = TC_IDR0_LDRAS ;
        NVIC_DisableIRQ(TC3_IRQn) ;
}

