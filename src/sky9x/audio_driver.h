/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
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

#ifndef audio_driver_h
#define audio_driver_h

#define NUM_VOL_LEVELS	24

extern void startSound( void ) ;
extern void buzzer_on( void ) ;
extern void buzzer_off( void ) ;
extern void buzzer_sound( uint8_t time ) ;
extern void setFrequency( uint32_t frequency ) ;
extern uint32_t getFrequency();
extern void start_timer1( void ) ;
extern void initDac( void ) ;
extern void end_sound( void ) ;
extern void write_coprocessor( uint8_t *ptr, uint32_t count ) ;
extern void read_coprocessor( bool onlytemp=false ) ;
extern uint8_t Coproc_read ;
extern int8_t Coproc_valid ;
extern int8_t Coproc_temp ;
extern int8_t Coproc_maxtemp ;

extern uint16_t *nextAudioData;
extern uint16_t nextAudioSize;

inline void toneStart()
{
  PMC->PMC_PCER0 |= 0x40000000L ; // Enable peripheral clock to DAC
  DACC->DACC_IER = DACC_IER_ENDTX ;
}

inline void toneStop()
{
  DACC->DACC_IDR = DACC_IDR_ENDTX ; // Disable interrupt
}

extern volatile uint8_t Buzzer_count ;

extern void init_twi( void ) ;
extern void setVolume( register uint8_t volume ) ;
extern "C" void TWI0_IRQHandler (void) ;

#endif
