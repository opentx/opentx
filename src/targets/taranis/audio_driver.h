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

#ifndef audio_driver_h
#define audio_driver_h

extern void audioInit( void ) ;
extern void audioEnd( void ) ;

extern void setFrequency( uint32_t frequency ) ;
extern uint32_t getFrequency();

extern bool dacIdle;
extern uint16_t *nextAudioData;
extern uint16_t nextAudioSize;

inline void dacStart()
{
  DMA1->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5 ; // Write ones to clear bits
  DMA1_Stream5->CR |= DMA_SxCR_CIRC | DMA_SxCR_EN ;                               // Enable DMA channel
  DAC->SR = DAC_SR_DMAUDR1 ;                      // Write 1 to clear flag
  DAC->CR |= DAC_CR_EN1 | DAC_CR_DMAEN1 ;                 // Enable DAC
}

inline void dacStop()
{
  DMA1_Stream5->CR &= ~DMA_SxCR_CIRC ;
}

inline void dacFill(uint16_t *data, uint16_t size)
{
  DMA1_Stream5->CR &= ~DMA_SxCR_EN;                            // Disable DMA channel
  DMA1_Stream5->M0AR = CONVERT_PTR(data);
  DMA1_Stream5->NDTR = size*2;
}

inline uint16_t dacQueue(uint16_t *data, uint16_t size)
{
  if (dacIdle) {
    dacIdle = false;
    DMA1_Stream5->CR &= ~DMA_SxCR_EN ;                              // Disable DMA channel
    DMA1->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5 ; // Write ones to clear bits
    DMA1_Stream5->M0AR = CONVERT_PTR(data);
    DMA1_Stream5->NDTR = size*2;
    DMA1_Stream5->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE ;               // Enable DMA channel and interrupt
    DAC->SR = DAC_SR_DMAUDR1 ;                      // Write 1 to clear flag
    DAC->CR |= DAC_CR_EN1 | DAC_CR_DMAEN1 ;                 // Enable DAC
    return size;
  }
  else {
    return 0;
  }
}

#define VOLUME_LEVEL_MAX  23
#define VOLUME_LEVEL_DEF  12
static const int8_t volumeScale[VOLUME_LEVEL_MAX+1] =
{
    0,  1,  2,  3,  5,  9,  13,  17,  22,  27,  33,  40,
    64, 82, 96, 105, 112, 117, 120, 122, 124, 125, 126, 127
} ;

#if !defined(SIMU)
#define setVolume(v) I2C_set_volume(volumeScale[min<uint8_t>(v, VOLUME_LEVEL_MAX)])
#else
#define setVolume(v)
#endif

#endif
