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

#define STICK_LV    3
#define STICK_LH    2
#define STICK_RV    0
#define STICK_RH    1
#define POT_L       13
#define POT_R       5
#define SLIDE_L     9
#define SLIDE_R     8
#define SWITCHES1   11
#define SWITCHES2   7
#define SWITCHES3   10
#define SWITCHES4   6
#define BATTERY     14

// Sample time should exceed 1uS
#define SAMPTIME    2   // sample time = 28 cycles

uint16_t Analog_values[NUMBER_ANALOG] __DMA;

#define NUMBER_ANALOG_ADC1      13

#if 0
const int8_t ana_direction[NUMBER_ANALOG] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
#endif

void adcInit()
{
  // Enable clocks
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
  RCC->AHB1ENR |= ADC_RCC_AHB1Periph_GPIO | RCC_AHB1ENR_DMA2EN;

  configure_pins(ADC_GPIO_PIN_STICK_LV | ADC_GPIO_PIN_STICK_LH | ADC_GPIO_PIN_STICK_RV | ADC_GPIO_PIN_STICK_RH |
                 ADC_GPIO_PIN_POT_R1 | ADC_GPIO_SWITCHES_PIN_R3 | ADC_GPIO_SWITCHES_PIN_R4, PIN_ANALOG | PIN_PORTA);

  configure_pins(ADC_GPIO_PIN_POT_R2 | ADC_GPIO_PIN_POT_L2, PIN_ANALOG | PIN_PORTB);

  configure_pins(ADC_GPIO_PIN_POT_L1 | ADC_GPIO_SWITCHES_PIN_L3 | ADC_GPIO_SWITCHES_PIN_L4 | ADC_GPIO_PIN_BATT, PIN_ANALOG | PIN_PORTC);

  ADC1->CR1 = ADC_CR1_SCAN;
  ADC1->CR2 = ADC_CR2_ADON | ADC_CR2_DMA | ADC_CR2_DDS;
  ADC1->SQR1 = (BATTERY<<0) + ((NUMBER_ANALOG_ADC1-1) << 20); // bits 23:20 = number of conversions
  ADC1->SQR2 = (SLIDE_L<<0) + (SLIDE_R<<5) + (SWITCHES1<<10) + (SWITCHES2<<15) + (SWITCHES3<<20) + (SWITCHES4<<25); // conversions 7 to 12
  ADC1->SQR3 = (STICK_LH<<0) + (STICK_LV<<5) + (STICK_RV<<10) + (STICK_RH<<15) + (POT_L<<20) + (POT_R<<25); // conversions 1 to 6
  ADC1->SMPR1 = SAMPTIME + (SAMPTIME<<3) + (SAMPTIME<<6) + (SAMPTIME<<9) + (SAMPTIME<<12) + (SAMPTIME<<15) + (SAMPTIME<<18) + (SAMPTIME<<21) + (SAMPTIME<<24);
  ADC1->SMPR2 = SAMPTIME + (SAMPTIME<<3) + (SAMPTIME<<6) + (SAMPTIME<<9) + (SAMPTIME<<12) + (SAMPTIME<<15) + (SAMPTIME<<18) + (SAMPTIME<<21) + (SAMPTIME<<24) + (SAMPTIME<<27) ;

  ADC->CCR = 0 ; //ADC_CCR_ADCPRE_0 ;             // Clock div 2

  DMA2_Stream0->CR = DMA_SxCR_PL | DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC;
  DMA2_Stream0->PAR = CONVERT_PTR_UINT(&ADC1->DR);
  DMA2_Stream0->M0AR = CONVERT_PTR_UINT(Analog_values);
  DMA2_Stream0->NDTR = NUMBER_ANALOG_ADC1;
  DMA2_Stream0->FCR = DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 ;
}

void adcRead()
{
  DMA2_Stream0->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  ADC1->SR &= ~(uint32_t) ( ADC_SR_EOC | ADC_SR_STRT | ADC_SR_OVR ) ;
  DMA2->LIFCR = DMA_LIFCR_CTCIF0 | DMA_LIFCR_CHTIF0 |DMA_LIFCR_CTEIF0 | DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CFEIF0 ; // Write ones to clear bits
  DMA2_Stream0->CR |= DMA_SxCR_EN ;               // Enable DMA
  ADC1->CR2 |= (uint32_t)ADC_CR2_SWSTART ;
  for (unsigned int i=0; i<10000; i++) {
    if (DMA2->LISR & DMA_LISR_TCIF0) {
      break;
    }
  }
  DMA2_Stream0->CR &= ~DMA_SxCR_EN ;              // Disable DMA
}

// TODO
void adcStop()
{
}

uint16_t getAnalogValue(uint32_t value)
{
  // return Analog_values[ana_mapping[value]];
  return 0; // Analog_values[value];
}
