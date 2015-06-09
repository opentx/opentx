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

#define PIN_ANALOG  0x0003
#define PIN_PORTA   0x0000
#define PIN_PORTB   0x0100
#define PIN_PORTC   0x0200

#define STICK_RV    0
#define STICK_RH    1
#define STICK_LH    2
#define STICK_LV    3
#define POT_L       6
#define POT_R       8
#define POT_XTRA    9
#define SLIDE_L     14
#define SLIDE_R     15
#define BATTERY     10

#if defined(REV9E)
#define SLIDER_L2   8   // ADC_GPIO_PIN_POT3 (SLIDER3) = ANC3_IN8 
#define SLIDER_R2   7   // ADC_GPIO_PIN_POT4 (SLIDER4) = ADC3_IN7
#define POT_4       6   // ADC_GPIO_PIN_POT5 (POT4)    = ADC3_IN6
#endif

// Sample time should exceed 1uS
#define SAMPTIME    2   // sample time = 28 cycles

#if defined(REV9E)
  const int8_t ana_direction[NUMBER_ANALOG] = {1,-1,1,-1,  -1,1,-1,  -1,1,  1,  -1,-1,1};
#elif defined(REVPLUS)
  const int8_t ana_direction[NUMBER_ANALOG] = {1,-1,1,-1,  -1,1,-1,  -1,1,  1};
#elif defined(REV4a)
  const int8_t ana_direction[NUMBER_ANALOG] = {1,-1,1,-1,  -1,-1,0,  -1,1,  1};
#else
  const int8_t ana_direction[NUMBER_ANALOG] = {1,-1,1,-1,  -1,1,0,   -1,1,  1};
#endif

#if defined(REV9E)
    #define NUMBER_ANALOG_ADC1      10
    #define NUMBER_ANALOG_ADC3      3
    // mapping from Analog_values order to enum Analogs
    const uint8_t ana_mapping[NUMBER_ANALOG] = { 0 /*STICK1*/, 1 /*STICK2*/, 2 /*STICK3*/, 3 /*STICK4*/, 
                                                 4 /*POT1*/, 5 /*POT2*/, 6 /*POT3*/, 12 /*POT4*/, 
                                                 7 /*SLIDER1*/, 8 /*SLIDER2*/, 10 /*SLIDER3*/, 11 /*SLIDER4*/,
                                                 9 /*TX_VOLTAGE*/ };
#else
    #define NUMBER_ANALOG_ADC1      10
#endif

uint16_t Analog_values[NUMBER_ANALOG];

void adcInit()
{
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;            // Enable clock
  RCC->AHB1ENR |= ADC_AHB1Periph_GPIO;        // Enable ports A&C clocks
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;            // Enable DMA2 clock

  configure_pins(ADC_GPIO_PIN_STICK_RV | ADC_GPIO_PIN_STICK_RH | ADC_GPIO_PIN_STICK_LH | ADC_GPIO_PIN_STICK_LV |
                 ADC_GPIO_PIN_POT1, PIN_ANALOG | PIN_PORTA);

#if defined(REV9E)
  configure_pins(ADC_GPIO_PIN_POT2 | ADC_GPIO_PIN_POT6, PIN_ANALOG|PIN_PORTB);
#elif defined(REVPLUS)
  configure_pins(ADC_GPIO_PIN_POT2 | ADC_GPIO_PIN_POT3, PIN_ANALOG|PIN_PORTB);
#else
  configure_pins(ADC_GPIO_PIN_POT2, PIN_ANALOG|PIN_PORTB);
#endif

  configure_pins(ADC_GPIO_PIN_SLIDER1 | ADC_GPIO_PIN_SLIDER2 | ADC_GPIO_PIN_BATT, PIN_ANALOG | PIN_PORTC);

  ADC1->CR1 = ADC_CR1_SCAN;
  ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_DMA | ADC_CR2_DDS;
  ADC1->SQR1 = (NUMBER_ANALOG_ADC1-1) << 20 ; // bits 23:20 = number of conversions
  ADC1->SQR2 = (POT_XTRA<<0) + (SLIDE_L<<5) + (SLIDE_R<<10) + (BATTERY<<15); // conversions 7 and more
  ADC1->SQR3 = (STICK_LH<<0) + (STICK_LV<<5) + (STICK_RV<<10) + (STICK_RH<<15) + (POT_L<<20) + (POT_R<<25); // conversions 1 to 6
  ADC1->SMPR1 = SAMPTIME + (SAMPTIME<<3) + (SAMPTIME<<6) + (SAMPTIME<<9) + (SAMPTIME<<12) + (SAMPTIME<<15) + (SAMPTIME<<18) + (SAMPTIME<<21) + (SAMPTIME<<24);
  ADC1->SMPR2 = SAMPTIME + (SAMPTIME<<3) + (SAMPTIME<<6) + (SAMPTIME<<9) + (SAMPTIME<<12) + (SAMPTIME<<15) + (SAMPTIME<<18) + (SAMPTIME<<21) + (SAMPTIME<<24) + (SAMPTIME<<27) ;

  ADC->CCR = 0 ; //ADC_CCR_ADCPRE_0 ;             // Clock div 2

  DMA2_Stream0->CR = DMA_SxCR_PL | DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC;
  DMA2_Stream0->PAR = CONVERT_PTR_UINT(&ADC1->DR);
  DMA2_Stream0->M0AR = CONVERT_PTR_UINT(Analog_values);
  DMA2_Stream0->NDTR = NUMBER_ANALOG_ADC1;
  DMA2_Stream0->FCR = DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 ;

#if defined(REV9E)
  RCC->APB2ENR |= RCC_APB2ENR_ADC3EN ;      // Enable clock
  configure_pins( ADC_GPIO_PIN_POT3 | ADC_GPIO_PIN_POT4 | ADC_GPIO_PIN_POT5, PIN_ANALOG | PIN_PORTF ) ;

  ADC3->CR1 = ADC_CR1_SCAN ;
  ADC3->CR2 = ADC_CR2_ADON | ADC_CR2_DMA | ADC_CR2_DDS ;
  ADC3->SQR1 = (NUMBER_ANALOG_ADC3-1) << 20 ;   // NUMBER_ANALOG Channels
  ADC3->SQR2 = 0; 
  ADC3->SQR3 = (SLIDER_L2<<0) + (SLIDER_R2<<5) + (POT_4<<10) ; // conversions 1 to 3
  ADC3->SMPR1 = SAMPTIME + (SAMPTIME<<3) + (SAMPTIME<<6);
  ADC3->SMPR2 = 0;
  
  // Enable the DMA channel here, DMA2 stream 1, channel 2
  DMA2_Stream1->CR = DMA_SxCR_PL | DMA_SxCR_CHSEL_1 | DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC;
  DMA2_Stream1->PAR = CONVERT_PTR_UINT(&ADC3->DR);
  DMA2_Stream1->M0AR = CONVERT_PTR_UINT(Analog_values + NUMBER_ANALOG_ADC1);
  DMA2_Stream1->NDTR = NUMBER_ANALOG_ADC3;
  DMA2_Stream1->FCR = DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 ;
#endif
}

void adcRead()
{
  DMA2_Stream0->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  ADC1->SR &= ~(uint32_t) ( ADC_SR_EOC | ADC_SR_STRT | ADC_SR_OVR ) ;
  DMA2->LIFCR = DMA_LIFCR_CTCIF0 | DMA_LIFCR_CHTIF0 |DMA_LIFCR_CTEIF0 | DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CFEIF0 ; // Write ones to clear bits
  DMA2_Stream0->CR |= DMA_SxCR_EN ;               // Enable DMA
  ADC1->CR2 |= (uint32_t)ADC_CR2_SWSTART ;

#if defined(REV9E)
  DMA2_Stream1->CR &= ~DMA_SxCR_EN ;    // Disable DMA
  ADC3->SR &= ~(uint32_t) ( ADC_SR_EOC | ADC_SR_STRT | ADC_SR_OVR ) ;
  DMA2->LIFCR = DMA_LIFCR_CTCIF1 | DMA_LIFCR_CHTIF1 |DMA_LIFCR_CTEIF1 | DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CFEIF1 ; // Write ones to clear bits
  DMA2_Stream1->CR |= DMA_SxCR_EN ;   // Enable DMA
  ADC3->CR2 |= (uint32_t)ADC_CR2_SWSTART ;
#endif  // #if defined(REV9E)

#if defined(REV9E)
  for (unsigned int i=0; i<10000; i++) {
    if ((DMA2->LISR & DMA_LISR_TCIF0) && (DMA2->LISR & DMA_LISR_TCIF1)) {
      break;
    }
  }
  DMA2_Stream0->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  DMA2_Stream1->CR &= ~DMA_SxCR_EN ;              // Disable DMA
#else
  for (unsigned int i=0; i<10000; i++) {
    if (DMA2->LISR & DMA_LISR_TCIF0) {
      break;
    }
  }
  DMA2_Stream0->CR &= ~DMA_SxCR_EN ;              // Disable DMA
#endif
}

// TODO
void adcStop()
{
}

uint16_t getAnalogValue(uint32_t index)
{
#if defined(REV9E)
  index = ana_mapping[index];
#endif
  if (ana_direction[index] < 0)
    return 4096 - Analog_values[index];
  else
    return Analog_values[index];
}
