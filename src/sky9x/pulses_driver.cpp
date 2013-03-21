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

void init_main_ppm( uint32_t period, uint32_t out_enable )
{
  register Pio *pioptr ;
  register Pwm *pwmptr ;

  setupPulsesPPM(0) ;

  if ( out_enable )
  {
    pioptr = PIOA ;
    pioptr->PIO_ABCDSR[0] &= ~PIO_PA17 ;            // Peripheral C
    pioptr->PIO_ABCDSR[1] |= PIO_PA17 ;             // Peripheral C
    pioptr->PIO_PDR = PIO_PA17 ;                    // Disable bit A17 Assign to peripheral
  }

  pwmptr = PWM ;
  // PWM3 for PPM output
  pwmptr->PWM_CH_NUM[3].PWM_CMR = 0x0000000B ;                  // CLKA
  if (g_model.pulsePol)
    pwmptr->PWM_CH_NUM[3].PWM_CMR |= 0x00000200 ;               // CPOL
  pwmptr->PWM_CH_NUM[3].PWM_CPDR = period ;                     // Period in half uS
  pwmptr->PWM_CH_NUM[3].PWM_CPDRUPD = period ;                  // Period in half uS
  pwmptr->PWM_CH_NUM[3].PWM_CDTY = g_model.ppmDelay*100+600;    // Duty in half uS
  pwmptr->PWM_CH_NUM[3].PWM_CDTYUPD = g_model.ppmDelay*100+600; // Duty in half uS
  pwmptr->PWM_ENA = PWM_ENA_CHID3 ;                             // Enable channel 3
  pwmptr->PWM_IER1 = PWM_IER1_CHID3 ;

#if !defined(REVA)
  configure_pins( PIO_PC15, PIN_PERIPHERAL | PIN_INPUT | PIN_PER_B | PIN_PORTC | PIN_NO_PULLUP ) ;
#endif

#if !defined(REVA)
  // PWM1 for PPM2
  pwmptr->PWM_CH_NUM[1].PWM_CMR = 0x0000000B ;    // CLKB
  if (g_model.pulsePol)
    pwmptr->PWM_CH_NUM[1].PWM_CMR |= 0x00000200 ;   // CPOL
  pwmptr->PWM_CH_NUM[1].PWM_CPDR = period ;                       // Period
  pwmptr->PWM_CH_NUM[1].PWM_CPDRUPD = period ;            // Period
  pwmptr->PWM_CH_NUM[1].PWM_CDTY = g_model.ppmDelay*100+600 ;                             // Duty
  pwmptr->PWM_CH_NUM[1].PWM_CDTYUPD = g_model.ppmDelay*100+600 ;          // Duty
  pwmptr->PWM_ENA = PWM_ENA_CHID1 ;                                               // Enable channel 1
#endif

  pwmptr->PWM_IER1 = PWM_IER1_CHID1 ;
  NVIC_EnableIRQ(PWM_IRQn) ;
}

void disable_main_ppm()
{
  register Pio *pioptr ;

  pioptr = PIOA ;
  pioptr->PIO_PER = PIO_PA17 ;                                            // Assign A17 to PIO

  PWM->PWM_IDR1 = PWM_IDR1_CHID3 ;
}

void disable_ppm2()
{
  register Pio *pioptr ;

  pioptr = PIOC ;
  pioptr->PIO_PER = PIO_PC17 ;                                            // Assign A17 to PIO

  PWM->PWM_IDR1 = PWM_IDR1_CHID1 ;
  NVIC_DisableIRQ(PWM_IRQn) ;
}

// Initialise the SSC to allow PXX output.
// TD is on PA17, peripheral A
void init_ssc()
{
  register Ssc *sscptr ;

  PMC->PMC_PCER0 |= 0x00400000L ;               // Enable peripheral clock to SSC

  configure_pins( PIO_PA17, PIN_PERIPHERAL | PIN_INPUT | PIN_PER_A | PIN_PORTA | PIN_NO_PULLUP ) ;

  sscptr = SSC ;
  sscptr->SSC_CMR = Master_frequency / (125000*2) ;               // 8uS per bit
  sscptr->SSC_TCMR = 0 ;          //  0000 0000 0000 0000 0000 0000 0000 0000
  sscptr->SSC_TFMR = 0x00000027 ;         //  0000 0000 0000 0000 0000 0000 1010 0111 (8 bit data, lsb)
  sscptr->SSC_CR = SSC_CR_TXEN ;
}

void disable_ssc()
{
  register Pio *pioptr ;
  register Ssc *sscptr ;

  // Revert back to pwm output
  pioptr = PIOA ;
  pioptr->PIO_PER = PIO_PA17 ;                                         // Assign A17 to PIO

  sscptr = SSC ;
  sscptr->SSC_CR = SSC_CR_TXDIS ;
}
