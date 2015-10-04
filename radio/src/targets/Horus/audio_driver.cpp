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

#include "../../opentx.h"

const int8_t volumeScale[VOLUME_LEVEL_MAX+1] =
{
    0,  1,  2,  3,  5,  9,  13,  17,  22,  27,  33,  40,
    64, 82, 96, 105, 112, 117, 120, 122, 124, 125, 126, 127
};

#if !defined(SIMU)
bool dacIdle = true;

void setSampleRate(uint32_t frequency)
{
  register uint32_t timer = (PERI1_FREQUENCY * TIMER_MULT_APB1) / frequency - 1 ;         // MCK/8 and 100 000 Hz

  TIM6->CR1 &= ~TIM_CR1_CEN ;
  TIM6->CNT = 0 ;
  TIM6->ARR = limit<uint32_t>(2, timer, 65535) ;
  TIM6->CR1 |= TIM_CR1_CEN ;
}

// Start TIMER6 at 100000Hz, used for DAC trigger
void dacTimerInit()
{
  // Now for timer 6
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN ;            // Enable clock

  TIM6->PSC = 0 ;                                                                                                 // Max speed
  TIM6->ARR = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 100000 - 1 ;        // 10 uS, 100 kHz
  TIM6->CR2 = 0 ;
  TIM6->CR2 = 0x20 ;
  TIM6->CR1 = TIM_CR1_CEN ;
}

// Configure DAC0 (or DAC1 for REVA)
// Not sure why PB14 has not be allocated to the DAC, although it is an EXTRA function
// So maybe it is automatically done
void dacInit()
{
  dacTimerInit();

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN ;           // Enable portA clock
  configure_pins( GPIO_Pin_4, PIN_ANALOG | PIN_PORTA ) ;
  RCC->APB1ENR |= RCC_APB1ENR_DACEN ;                             // Enable clock
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN ;                    // Enable DMA1 clock

  // Chan 7, 16-bit wide, Medium priority, memory increments
  DMA1_Stream5->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  DMA1->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5 ; // Write ones to clear bits
  DMA1_Stream5->CR = DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_2 | DMA_SxCR_PL_0 |
                     DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_CIRC ;
  DMA1_Stream5->PAR = CONVERT_PTR_UINT(&DAC->DHR12R1);
  // DMA1_Stream5->M0AR = CONVERT_PTR_UINT(Sine_values);
  DMA1_Stream5->FCR = 0x05 ; //DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 ;
  // DMA1_Stream5->NDTR = 100 ;

  DAC->DHR12R1 = 2010 ;
  DAC->SR = DAC_SR_DMAUDR1 ;              // Write 1 to clear flag
  DAC->CR = DAC_CR_TEN1 | DAC_CR_EN1 ;                    // Enable DAC
  NVIC_EnableIRQ(TIM6_DAC_IRQn); // TODO needed?
  NVIC_SetPriority(TIM6_DAC_IRQn, 7);
  NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  NVIC_SetPriority(DMA1_Stream5_IRQn, 7);
}

bool dacQueue(AudioBuffer *buffer)
{
  if (dacIdle) {
    dacIdle = false;
    DMA1_Stream5->CR &= ~DMA_SxCR_EN ;                              // Disable DMA channel
    DMA1->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5 ; // Write ones to clear bits
    DMA1_Stream5->M0AR = CONVERT_PTR_UINT(buffer->data);
    DMA1_Stream5->NDTR = buffer->size;
    DMA1_Stream5->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE ;               // Enable DMA channel and interrupt
    DAC->SR = DAC_SR_DMAUDR1 ;                      // Write 1 to clear flag
    DAC->CR |= DAC_CR_EN1 | DAC_CR_DMAEN1 ;                 // Enable DAC
    return true;
  }
  else {
    return false;
  }
}

void dacStart()
{
  DMA1->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5 ; // Write ones to clear bits
  DMA1_Stream5->CR |= DMA_SxCR_CIRC | DMA_SxCR_EN ;                               // Enable DMA channel
  DAC->SR = DAC_SR_DMAUDR1 ;                      // Write 1 to clear flag
  DAC->CR |= DAC_CR_EN1 | DAC_CR_DMAEN1 ;                 // Enable DAC
}

void dacStop()
{
  DMA1_Stream5->CR &= ~DMA_SxCR_CIRC ;
}

// Sound routines
void audioInit()
{
  dacInit();
}

void audioEnd()
{
  DAC->CR = 0 ;
  TIM6->CR1 = 0 ;
  // Also need to turn off any possible interrupts
  NVIC_DisableIRQ(TIM6_DAC_IRQn) ;
  NVIC_DisableIRQ(DMA1_Stream5_IRQn) ;
}

extern "C" void TIM6_DAC_IRQHandler()
{
  DAC->CR &= ~DAC_CR_DMAEN1 ;     // Stop DMA requests
  DAC->SR = DAC_SR_DMAUDR1 ;      // Write 1 to clear flag
}

extern "C" void DMA1_Stream5_IRQHandler()
{
  DMA1_Stream5->CR &= ~DMA_SxCR_TCIE ;            // Stop interrupt
  DMA1->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5 ; // Write ones to clear flags
  DMA1_Stream5->CR &= ~DMA_SxCR_EN ;                              // Disable DMA channel

  AudioBuffer *nextBuffer = audioQueue.getNextFilledBuffer();
  if (nextBuffer) {
    DMA1_Stream5->M0AR = CONVERT_PTR_UINT(nextBuffer->data);
    DMA1_Stream5->NDTR = nextBuffer->size;
    DMA1->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5 ; // Write ones to clear bits
    DMA1_Stream5->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE ;       // Enable DMA channel
    DAC->SR = DAC_SR_DMAUDR1;                      // Write 1 to clear flag
  }
  else {
    dacIdle = true;
  }
}
#endif  // #if !defined(SIMU)
