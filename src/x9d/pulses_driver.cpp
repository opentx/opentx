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

#include "../open9x.h"

// TODO not here!
void setupPulses();
void setupPulsesPPM();
void setupPulsesPXX();

uint16_t *ppmStreamPtr;
extern uint16_t ppmStream[20];
extern uint16_t pxxStream[400] ;

void init_pxx()
{
  // Timer1
  setupPulsesPXX() ; // TODO not here!

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN ;           // Enable portA clock
  configure_pins( 0x0100, PIN_PERIPHERAL | PIN_PORTA | PIN_PER_1 | PIN_OS25 | PIN_PUSHPULL ) ;
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN ;            // Enable clock
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN ;                    // Enable DMA1 clock

  TIM1->CR1 &= ~TIM_CR1_CEN ;
  TIM1->ARR = 18000 ;                     // 9mS
  TIM1->CCR2 = 15000 ;            // Update time
  TIM1->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 2000000 - 1 ;               // 0.5uS from 30MHz
  TIM1->CCER = TIM_CCER_CC1E | TIM_CCER_CC1P ;
  TIM1->CR2 = TIM_CR2_OIS1 ;                      // O/P idle high
  TIM1->BDTR = TIM_BDTR_MOE ;             // Enable outputs
  TIM1->CCR1 = pxxStream[0] ;
  TIM1->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0 ;                     // Force O/P high
  TIM1->EGR = 1 ;                                                         // Restart

//      TIM1->SR &= ~TIM_SR_UIF ;                               // Clear flag
//      TIM1->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  TIM1->DIER |= TIM_DIER_CC1DE ;          // Enable DMA on CC1 match
  TIM1->DCR = 13 ;                                                                // DMA to CC1

//      TIM1->CR1 = TIM_CR1_OPM ;                               // Just run once
        // Enable the DMA channel here, DMA2 stream 1, channel 6
  DMA2_Stream1->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  DMA2->LIFCR = DMA_LIFCR_CTCIF1 | DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTEIF1 | DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CFEIF1 ; // Write ones to clear bits
  DMA2_Stream1->CR = DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_2 | DMA_SxCR_PL_0 | DMA_SxCR_MSIZE_0
                                                                                 | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_PFCTRL ;
  DMA2_Stream1->PAR = (uint32_t) &TIM1->DMAR ;
  DMA2_Stream1->M0AR = CONVERT_PTR(&pxxStream[1]);
//      DMA2_Stream1->FCR = 0x05 ; //DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 ;
//      DMA2_Stream1->NDTR = 100 ;
  DMA2_Stream1->CR |= DMA_SxCR_EN ;               // Enable DMA

  TIM1->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0 ;                     // Toggle CC1 o/p
  TIM1->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  TIM1->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  TIM1->CR1 |= TIM_CR1_CEN ;
  NVIC_EnableIRQ(TIM1_CC_IRQn) ;
}

void disable_pxx()
{
  NVIC_DisableIRQ(TIM1_CC_IRQn) ;
  TIM1->DIER &= ~TIM_DIER_CC2IE ;
  TIM1->CR1 &= ~TIM_CR1_CEN ;
}


// PPM output
// Timer 1, channel 1 on PA8 for prototype
// Pin is AF1 function for timer 1
void init_main_ppm()
{
  // Timer1
  setupPulsesPPM() ;
  ppmStreamPtr = ppmStream ;

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN ;           // Enable portA clock
  configure_pins( 0x0100, PIN_PERIPHERAL | PIN_PORTA | PIN_PER_1 | PIN_OS25 | PIN_PUSHPULL ) ;
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN ;            // Enable clock

  TIM1->ARR = *ppmStreamPtr++ ;
  TIM1->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 2000000 - 1 ;               // 0.5uS from 30MHz
  TIM1->CCER = TIM_CCER_CC1E ;
  TIM1->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC2PE ;                   // PWM mode 1
  TIM1->CCR1 = 600 ;              // 300 uS pulse
  TIM1->BDTR = TIM_BDTR_MOE ;
  TIM1->EGR = 1 ;
  TIM1->DIER = TIM_DIER_UDE ;

  TIM1->SR &= ~TIM_SR_UIF ;                               // Clear flag
  TIM1->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  TIM1->DIER |= TIM_DIER_CC2IE ;
  TIM1->DIER |= TIM_DIER_UIE ;

  TIM1->CR1 = TIM_CR1_CEN ;
  NVIC_EnableIRQ(TIM1_CC_IRQn) ;
  NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn) ;
}

void disable_main_ppm()
{
  NVIC_DisableIRQ(TIM1_CC_IRQn) ;
  NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn) ;
  TIM1->DIER &= ~TIM_DIER_CC2IE & ~TIM_DIER_UIE ;
  TIM1->CR1 &= ~TIM_CR1_CEN ;
}

extern "C" void TIM1_CC_IRQHandler()
{
  TIM1->DIER &= ~TIM_DIER_CC2IE ;         // stop this interrupt
  TIM1->SR &= ~TIM_SR_CC2IF ;                             // Clear flag

  setupPulses() ;

  if (s_current_protocol == PROTO_PPM)
  {
    ppmStreamPtr = ppmStream ;

    TIM1->DIER |= TIM_DIER_UDE ;

    TIM1->SR &= ~TIM_SR_UIF ;                                       // Clear this flag
    TIM1->DIER |= TIM_DIER_UIE ;                            // Enable this interrupt
  }
  else if (s_current_protocol == PROTO_PXX)
  {
    DMA2_Stream1->CR &= ~DMA_SxCR_EN ;              // Disable DMA
    DMA2->LIFCR = DMA_LIFCR_CTCIF1 | DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTEIF1 | DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CFEIF1 ; // Write ones to clear bits
    DMA2_Stream1->M0AR = CONVERT_PTR(&pxxStream[1]);
    DMA2_Stream1->CR |= DMA_SxCR_EN ;               // Enable DMA
    TIM1->CCR1 = pxxStream[0] ;
    TIM1->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  }
}

extern "C" void TIM1_UP_TIM10_IRQHandler()
{
  TIM1->SR &= ~TIM_SR_UIF ;                               // Clear flag

  TIM1->ARR = *ppmStreamPtr++ ;
  if ( *ppmStreamPtr == 0 )
  {
    TIM1->SR &= ~TIM_SR_CC2IF ;                     // Clear this flag
    TIM1->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  }
}
