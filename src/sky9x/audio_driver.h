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

void audioInit( void ) ;
void audioEnd( void ) ;

void setFrequency( uint32_t frequency ) ;
uint32_t getFrequency();

extern uint16_t *nextAudioData;
extern uint16_t nextAudioSize;

inline void dacStart()
{
  PMC->PMC_PCER0 |= 0x40000000L ; // Enable peripheral clock to DAC
  DACC->DACC_IER = DACC_IER_ENDTX ;
}

inline void dacStop()
{
  DACC->DACC_IDR = DACC_IDR_ENDTX ; // Disable interrupt
}

#define VOLUME_LEVEL_MAX  23
#define VOLUME_LEVEL_DEF  12
void setVolume(uint8_t volume);

inline void dacFill(uint16_t *data, uint16_t size)
{
  register Dacc *dacptr = DACC;
  dacptr->DACC_TPR = CONVERT_PTR(data);
  dacptr->DACC_TNPR = CONVERT_PTR(data);
  dacptr->DACC_TCR = size;       // words
  dacptr->DACC_TNCR = size;      // words
}

inline uint16_t dacQueue(uint16_t *data, uint16_t size)
{
  register Dacc *dacptr = DACC;
  if (dacptr->DACC_ISR & DACC_ISR_TXBUFE) {
    dacptr->DACC_TPR = CONVERT_PTR(data);
    dacptr->DACC_TCR = size;
    return size;
  }
  else if (dacptr->DACC_TNCR == 0) {
    dacptr->DACC_TNPR = CONVERT_PTR(data);
    dacptr->DACC_TNCR = size;
    return size;
  }
  else {
    return 0;
  }
}

#endif
