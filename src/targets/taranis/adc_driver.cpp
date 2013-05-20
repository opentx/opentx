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

#include "../opentx.h"

#define PIN_ANALOG  0x0003
#define PIN_PORTA   0x0000
#define PIN_PORTB   0x0100
#define PIN_PORTC   0x0200

#define STICK_LV    3
#define STICK_LH    2
#define STICK_RV    0
#define STICK_RH    1
#define POT_L       6
#if defined(REV3)
  #define POT_R     7
#else
  #define POT_R     8
#endif
#define SLIDE_L    14
#define SLIDE_R    15
#define BATTERY    10

// Sample time should exceed 1uS
#define SAMPTIME    2   // sample time = 15 cycles

volatile uint16_t Analog_values[NUMBER_ANALOG];
#if defined(REV4a)
const char ana_direction[NUMBER_ANALOG] = {0,1,0,1,  1,1,1,0,  0};
#elif !defined(REV3)
const char ana_direction[NUMBER_ANALOG] = {0,1,0,1,  1,0,1,0,  0};
#endif

void adcInit()
{
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN ;                    // Enable clock
  RCC->AHB1ENR |= RCC_AHB1Periph_GPIOADC ;        // Enable ports A&C clocks
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN ;            // Enable DMA2 clock

#if defined(REV3)
  configure_pins(PIN_STK_J1 | PIN_STK_J2 | PIN_STK_J3 | PIN_STK_J4 |
                 PIN_FLP_J1 | PIN_FLP_J2, PIN_ANALOG | PIN_PORTA) ;
#else
  configure_pins(PIN_STK_J1 | PIN_STK_J2 | PIN_STK_J3 | PIN_STK_J4 |
                 PIN_FLP_J1, PIN_ANALOG | PIN_PORTA) ;
#endif

#if !defined(REV3)
  configure_pins(PIN_FLP_J2, PIN_ANALOG|PIN_PORTB);
#endif

  configure_pins(PIN_SLD_J1 | PIN_SLD_J2 | PIN_MVOLT, PIN_ANALOG | PIN_PORTC) ;

  ADC1->CR1 = ADC_CR1_SCAN ;
  ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_DMA | ADC_CR2_DDS ;
  ADC1->SQR1 = (NUMBER_ANALOG-1) << 20 ;             // NUMBER_ANALOG Channels
  ADC1->SQR2 = SLIDE_L + (SLIDE_R<<5) + (BATTERY<<10) ;
  ADC1->SQR3 = STICK_LH + (STICK_LV<<5) + (STICK_RV<<10) + (STICK_RH<<15) + (POT_L<<20) + (POT_R<<25) ;
  ADC1->SMPR1 = SAMPTIME + (SAMPTIME<<3) + (SAMPTIME<<6) + (SAMPTIME<<9) + (SAMPTIME<<12)
                                                          + (SAMPTIME<<15) + (SAMPTIME<<18) + (SAMPTIME<<21) + (SAMPTIME<<24) ;
  ADC1->SMPR2 = SAMPTIME + (SAMPTIME<<3) + (SAMPTIME<<6) + (SAMPTIME<<9) + (SAMPTIME<<12)
                                                          + (SAMPTIME<<15) + (SAMPTIME<<18) + (SAMPTIME<<21) + (SAMPTIME<<24) + (SAMPTIME<<27) ;

  ADC->CCR = 0 ; //ADC_CCR_ADCPRE_0 ;             // Clock div 2

  DMA2_Stream0->CR = DMA_SxCR_PL | DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC ;
  DMA2_Stream0->PAR = CONVERT_PTR(&ADC1->DR);
  DMA2_Stream0->M0AR = CONVERT_PTR(Analog_values);
  DMA2_Stream0->FCR = DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 ;
}

void adcRead()
{
  uint32_t i ;

  DMA2_Stream0->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  ADC1->SR &= ~(uint32_t) ( ADC_SR_EOC | ADC_SR_STRT | ADC_SR_OVR ) ;
  DMA2->LIFCR = DMA_LIFCR_CTCIF0 | DMA_LIFCR_CHTIF0 |DMA_LIFCR_CTEIF0 | DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CFEIF0 ; // Write ones to clear bits
  DMA2_Stream0->M0AR = CONVERT_PTR(Analog_values);
  DMA2_Stream0->NDTR = NUMBER_ANALOG ;
  DMA2_Stream0->CR |= DMA_SxCR_EN ;               // Enable DMA
  ADC1->CR2 |= (uint32_t)ADC_CR2_SWSTART ;
  for (i=0; i<10000; i++) {
    if (DMA2->LISR & DMA_LISR_TCIF0) {
      break ;
    }
  }
  DMA2_Stream0->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  // return ( i < 10000 ) ? 1 : 0 ;

#if !defined(REV3)
  // adc direction correct
  for (i=0; i<NUMBER_ANALOG; i++) {
    if (ana_direction[i]) {
      Analog_values[i] = 4096-Analog_values[i];
    }
  }
#endif
}

// TODO
void adcStop()
{
}






