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

// Sample time should exceed 1uS
#define SAMPTIME    2   // sample time = 28 cycles

#if defined(REV9E)
  const int8_t ana_direction[NUMBER_ANALOG] = {1,1,-1,-1,  -1,-1,-1,1, -1,1,1,1,  -1};
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
                                                 10 /*POT1*/, 4 /*POT2*/, 5 /*POT3*/, 6 /*POT4*/,
                                                 11 /*SLIDER1*/, 12 /*SLIDER2*/, 7 /*SLIDER3*/, 8 /*SLIDER4*/,
                                                 9 /*TX_VOLTAGE*/ };
#else
    #define NUMBER_ANALOG_ADC1      10
#endif

uint16_t Analog_values[NUMBER_ANALOG] __DMA;

void adcInit()
{
#if defined(REV9E)
  configure_pins(ADC_GPIO_PIN_STICK_RV | ADC_GPIO_PIN_STICK_RH | ADC_GPIO_PIN_STICK_LH | ADC_GPIO_PIN_STICK_LV | ADC_GPIO_PIN_SLIDER3, PIN_ANALOG | PIN_PORTA);
  configure_pins(ADC_GPIO_PIN_POT2 | ADC_GPIO_PIN_SLIDER4, PIN_ANALOG | PIN_PORTB);
  configure_pins(ADC_GPIO_PIN_POT3 | ADC_GPIO_PIN_POT4 | ADC_GPIO_PIN_SLIDER1 | ADC_GPIO_PIN_SLIDER2 | ADC_GPIO_PIN_BATT, PIN_ANALOG | PIN_PORTC);
  configure_pins(ADC_GPIO_PIN_POT1 | ADC_GPIO_PIN_SLIDER1 | ADC_GPIO_PIN_SLIDER2, PIN_ANALOG | PIN_PORTF);
#elif defined(REVPLUS)
  configure_pins(ADC_GPIO_PIN_STICK_RV | ADC_GPIO_PIN_STICK_RH | ADC_GPIO_PIN_STICK_LH | ADC_GPIO_PIN_STICK_LV | ADC_GPIO_PIN_POT1, PIN_ANALOG | PIN_PORTA);
  configure_pins(ADC_GPIO_PIN_POT2 | ADC_GPIO_PIN_POT3, PIN_ANALOG | PIN_PORTB);
  configure_pins(ADC_GPIO_PIN_SLIDER1 | ADC_GPIO_PIN_SLIDER2 | ADC_GPIO_PIN_BATT, PIN_ANALOG | PIN_PORTC);
#else
  configure_pins(ADC_GPIO_PIN_STICK_RV | ADC_GPIO_PIN_STICK_RH | ADC_GPIO_PIN_STICK_LH | ADC_GPIO_PIN_STICK_LV | ADC_GPIO_PIN_POT1, PIN_ANALOG | PIN_PORTA);
  configure_pins(ADC_GPIO_PIN_POT2, PIN_ANALOG|PIN_PORTB);
  configure_pins(ADC_GPIO_PIN_SLIDER1 | ADC_GPIO_PIN_SLIDER2 | ADC_GPIO_PIN_BATT, PIN_ANALOG | PIN_PORTC);
#endif

  ADC1->CR1 = ADC_CR1_SCAN;
  ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_DMA | ADC_CR2_DDS;
  ADC1->SQR1 = (NUMBER_ANALOG_ADC1-1) << 20 ; // bits 23:20 = number of conversions
#if defined(REV9E)
  ADC1->SQR2 = (ADC_CHANNEL_POT4<<0) + (ADC_CHANNEL_SLIDER3<<5) + (ADC_CHANNEL_SLIDER4<<10) + (ADC_CHANNEL_BATT<<15); // conversions 7 and more
  ADC1->SQR3 = (ADC_CHANNEL_STICK_LH<<0) + (ADC_CHANNEL_STICK_LV<<5) + (ADC_CHANNEL_STICK_RV<<10) + (ADC_CHANNEL_STICK_RH<<15) + (ADC_CHANNEL_POT2<<20) + (ADC_CHANNEL_POT3<<25); // conversions 1 to 6
#else
  ADC1->SQR2 = (ADC_CHANNEL_POT3<<0) + (ADC_CHANNEL_SLIDER1<<5) + (ADC_CHANNEL_SLIDER2<<10) + (ADC_CHANNEL_BATT<<15); // conversions 7 and more
  ADC1->SQR3 = (ADC_CHANNEL_STICK_LH<<0) + (ADC_CHANNEL_STICK_LV<<5) + (ADC_CHANNEL_STICK_RV<<10) + (ADC_CHANNEL_STICK_RH<<15) + (ADC_CHANNEL_POT1<<20) + (ADC_CHANNEL_POT2<<25); // conversions 1 to 6
#endif
  ADC1->SMPR1 = SAMPTIME + (SAMPTIME<<3) + (SAMPTIME<<6) + (SAMPTIME<<9) + (SAMPTIME<<12) + (SAMPTIME<<15) + (SAMPTIME<<18) + (SAMPTIME<<21) + (SAMPTIME<<24);
  ADC1->SMPR2 = SAMPTIME + (SAMPTIME<<3) + (SAMPTIME<<6) + (SAMPTIME<<9) + (SAMPTIME<<12) + (SAMPTIME<<15) + (SAMPTIME<<18) + (SAMPTIME<<21) + (SAMPTIME<<24) + (SAMPTIME<<27) ;

  ADC->CCR = 0 ; //ADC_CCR_ADCPRE_0 ;             // Clock div 2

  DMA2_Stream0->CR = DMA_SxCR_PL | DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC;
  DMA2_Stream0->PAR = CONVERT_PTR_UINT(&ADC1->DR);
  DMA2_Stream0->M0AR = CONVERT_PTR_UINT(Analog_values);
  DMA2_Stream0->NDTR = NUMBER_ANALOG_ADC1;
  DMA2_Stream0->FCR = DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 ;

#if defined(REV9E)
  ADC3->CR1 = ADC_CR1_SCAN ;
  ADC3->CR2 = ADC_CR2_ADON | ADC_CR2_DMA | ADC_CR2_DDS ;
  ADC3->SQR1 = (NUMBER_ANALOG_ADC3-1) << 20 ;   // NUMBER_ANALOG Channels
  ADC3->SQR2 = 0; 
  ADC3->SQR3 = (ADC_CHANNEL_POT1<<0) + (ADC_CHANNEL_SLIDER1<<5) + (ADC_CHANNEL_SLIDER2<<10) ; // conversions 1 to 3
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
  if (IS_POT(index) && !IS_POT_AVAILABLE(index)) {
    // Use fixed analog value for non-existing and/or non-connected pots.
    // Non-connected analog inputs will slightly follow the adjacent connected analog inputs, 
    // which produces ghost readings on these inputs.
    return 0;
  }
#if defined(REV9E)
  index = ana_mapping[index];
#endif
  if (ana_direction[index] < 0)
    return 4096 - Analog_values[index];
  else
    return Analog_values[index];
}
