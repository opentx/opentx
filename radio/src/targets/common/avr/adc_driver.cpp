/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x 
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"

#define ADC_VREF_TYPE (1 << REFS0) // AVCC with external capacitor at AREF pin

void adcInit()
{
  ADMUX  = ADC_VREF_TYPE;
  ADCSRA = 0x85; // ADC enabled, pre-scaler division=32 (no interrupt, no auto-triggering)
#if defined(CPUM2560)
  ADCSRB = (1 << MUX5);
#endif
}

void adcPrepareBandgap()
{
  // #if structure identical to the one in getADC_bandgap()
#if defined(PCBGRUVIN9X)
  // For PCB V4, use our own 1.2V, external reference (connected to ADC3)
  ADCSRB &= ~(1<<MUX5);
  ADMUX = 0x03|ADC_VREF_TYPE; // Switch MUX to internal reference
#elif defined(PCBMEGA2560)
#else
  ADMUX = 0x1E|ADC_VREF_TYPE; // Switch MUX to internal reference
#endif
}

void getADC()
{
  for (uint8_t adc_input=0; adc_input<8; adc_input++) {
    uint16_t temp_ana;
    ADMUX = adc_input|ADC_VREF_TYPE;
    ADCSRA |= 1 << ADSC; // Start the AD conversion
    while (ADCSRA & (1 << ADSC)); // Wait for the AD conversion to complete
    temp_ana = ADC;
    ADCSRA |= 1 << ADSC; // Start the second AD conversion
    while (ADCSRA & (1 << ADSC)); // Wait for the AD conversion to complete
    temp_ana += ADC;
    s_anaFilt[adc_input] = temp_ana;
  }

#if defined(TELEMETRY_MOD_14051) || defined(TELEMETRY_MOD_14051_SWAPPED)
  processMultiplexAna();
#endif
}

void getADC_bandgap()
{
#if defined(PCBGRUVIN9X)
  static uint8_t s_bgCheck = 0;
  static uint16_t s_bgSum = 0;
  ADCSRA |= (1 << ADSC); // request sample
  s_bgCheck += 32;
  while ((ADCSRA & (1 << ADIF))==0); // wait for sample
  ADCSRA |= (1 << ADIF);
  if (s_bgCheck == 0) { // 8x over-sample (256/32=8)
    BandGap = s_bgSum+ADC;
    s_bgSum = 0;
  }
  else {
    s_bgSum += ADC;
  }
  ADCSRB |= (1 << MUX5);
#elif defined(PCBMEGA2560)
  BandGap = 2000;  
#else
/*
  MCUCR|=0x28;  // enable Sleep (bit5) enable ADC Noise Reduction (bit2)
  asm volatile(" sleep        \n\t");  // if _SLEEP() is not defined use this
  // ADCSRA|=0x40;
  while ((ADCSRA & 0x10)==0);
  ADCSRA|=0x10; // take sample  clear flag?
  BandGap=ADC;    
  MCUCR&=0x08;  // disable sleep  
  */

  ADCSRA |= (1 << ADSC);
  while (ADCSRA & (1 << ADSC));
  BandGap = ADC;
#endif
}
