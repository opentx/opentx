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

void module_output_active()
{
  register Pio *pioptr = PIOA ;
  pioptr->PIO_ABCDSR[0] &= ~PIO_PA17 ;            // Peripheral C
  pioptr->PIO_ABCDSR[1] |= PIO_PA17 ;             // Peripheral C
  pioptr->PIO_PDR = PIO_PA17 ;                    // Disable bit A17 Assign to peripheral
#if defined(REVX)
  if (g_model.moduleData[EXTERNAL_MODULE].ppmOutputType) {
    pioptr->PIO_MDDR = PIO_PA17 ;                 // Push Pull O/p in A17
  }
  else {
    pioptr->PIO_MDER = PIO_PA17 ;                 // Open Drain O/p in A17
  }
#else
  pioptr->PIO_MDDR = PIO_PA17 ;                                           // Push Pull O/p in A17
#endif
  pioptr->PIO_PUER = PIO_PA17 ;                   // With pull up
}

void init_main_ppm(uint32_t period, uint32_t out_enable)
{
  register Pwm *pwmptr ;

  setupPulsesPPM(EXTERNAL_MODULE) ;

  if (out_enable) {
    module_output_active();
  }

  pwmptr = PWM ;
  // PWM3 for PPM output
  pwmptr->PWM_CH_NUM[3].PWM_CMR = 0x0004000B ;  // CLKA
  if (!g_model.moduleData[EXTERNAL_MODULE].ppmPulsePol) {
    pwmptr->PWM_CH_NUM[3].PWM_CMR |= 0x00000200 ;               // CPOL
  }
  pwmptr->PWM_CH_NUM[3].PWM_CPDR = period ;                     // Period in half uS
  pwmptr->PWM_CH_NUM[3].PWM_CPDRUPD = period ;                  // Period in half uS
  pwmptr->PWM_CH_NUM[3].PWM_CDTY = g_model.moduleData[EXTERNAL_MODULE].ppmDelay*100+600;    // Duty in half uS
  pwmptr->PWM_CH_NUM[3].PWM_CDTYUPD = g_model.moduleData[EXTERNAL_MODULE].ppmDelay*100+600; // Duty in half uS
  pwmptr->PWM_ENA = PWM_ENA_CHID3 ;                             // Enable channel 3
  pwmptr->PWM_IER1 = PWM_IER1_CHID3 ;

  NVIC_SetPriority(PWM_IRQn, 3 ) ;
  NVIC_EnableIRQ(PWM_IRQn) ;
}

void disable_main_ppm()
{
  register Pio * pioptr = PIOA;
  pioptr->PIO_PER = PIO_PA17;                                            // Assign A17 to PIO
  PWM->PWM_IDR1 = PWM_IDR1_CHID3;
}

void init_second_ppm(uint32_t period)
{
#if !defined(REVA)
  // PWM1 for PPM2
  register Pwm * pwmptr = PWM;
  configure_pins(PIO_PC15, PIN_PERIPHERAL | PIN_INPUT | PIN_PER_B | PIN_PORTC | PIN_NO_PULLUP ) ; 
  pwmptr->PWM_CH_NUM[1].PWM_CMR = 0x0000000B ;    // CLKB
  if (!g_model.moduleData[EXTRA_MODULE].ppmPulsePol) {
    pwmptr->PWM_CH_NUM[1].PWM_CMR |= 0x00000200 ;   // CPOL
  }
  pwmptr->PWM_CH_NUM[1].PWM_CPDR = period ;                       // Period
  pwmptr->PWM_CH_NUM[1].PWM_CPDRUPD = period ;            // Period
  pwmptr->PWM_CH_NUM[1].PWM_CDTY = g_model.moduleData[EXTRA_MODULE].ppmDelay*100+600 ;                             // Duty
  pwmptr->PWM_CH_NUM[1].PWM_CDTYUPD = g_model.moduleData[EXTRA_MODULE].ppmDelay*100+600 ;          // Duty  pwmptr->PWM_ENA = PWM_ENA_CHID1 ;                                               // Enable channel 1
  pwmptr->PWM_IER1 = PWM_IER1_CHID1 ; 
#endif
}

void disable_second_ppm()
{
#if !defined(REVA)
  register Pio * pioptr = PIOC;
  pioptr->PIO_PER = PIO_PC15 ;                                            // Assign C17 to PIO
  PWM->PWM_IDR1 = PWM_IDR1_CHID1 ;
#endif
}

void init_no_pulses(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    init_main_ppm(3000, 0);
  }
  else {
    // TODO
  }
}

void disable_no_pulses(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    disable_ppm(EXTERNAL_MODULE);
  }
  else {
    // TODO
  }
}

void init_ppm(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    init_main_ppm(3000, 1);
  }
  else {
    init_second_ppm(3000);
  }
}

void disable_ppm(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    disable_main_ppm();
  }
  else {
    disable_second_ppm();
  }
}

// Initialise the SSC to allow PXX output.
// TD is on PA17, peripheral A
void init_ssc()
{
  register Ssc *sscptr ;

  PMC->PMC_PCER0 |= 0x00400000L ;               // Enable peripheral clock to SSC

  configure_pins( PIO_PA17, PIN_PERIPHERAL | PIN_INPUT | PIN_PER_A | PIN_PORTA ) ;

  sscptr = SSC ;
  sscptr->SSC_THR = 0xFF ;    // Make the output high.
  sscptr->SSC_TFMR = 0x00000027 ;         //  0000 0000 0000 0000 0000 0000 1010 0111 (8 bit data, lsb)
  sscptr->SSC_CMR = Master_frequency / (125000*2) ;               // 8uS per bit
  sscptr->SSC_TCMR = 0 ;          //  0000 0000 0000 0000 0000 0000 0000 0000
  sscptr->SSC_CR = SSC_CR_TXEN ;

//BF:20161102 Commenting this line solve issu #3628
//#if defined(REVX)
//  PIOA->PIO_MDER = PIO_PA17;            // Open Drain O/p in A17
//#else
//  PIOA->PIO_MDDR = PIO_PA17;            // Push Pull O/p in A17
//#endif
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

void init_pxx(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    init_main_ppm(2500 * 2, 0);
    init_ssc();
  }
  else {
    // TODO
  }
}

void disable_pxx(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    disable_ssc();
    disable_ppm(EXTERNAL_MODULE);
  }
  else {
    // TODO
  }
}

void init_dsm2(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    init_main_ppm(2500 * 2, 0);
    init_ssc();
  }
  else {
    // TODO
  }
}

void disable_dsm2(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    disable_ssc();
    disable_ppm(EXTERNAL_MODULE);
  }
  else {
    // TODO
  }
}

#if !defined(SIMU)
extern "C" void PWM_IRQHandler(void)
{
  register Pwm * pwmptr = PWM;
  uint32_t reason = pwmptr->PWM_ISR1;
  uint32_t period;

  if (reason & PWM_ISR1_CHID3) {
    // Use the current protocol, don't switch until set_up_pulses
    switch (s_current_protocol[EXTERNAL_MODULE]) {
      case PROTO_PXX:
        // Alternate periods of 6.5mS and 2.5 mS
        period = pwmptr->PWM_CH_NUM[3].PWM_CPDR;
        if (period == 2500 * 2) {
          period = 6500 * 2;
        }
        else {
          period = 2500 * 2;
        }
        pwmptr->PWM_CH_NUM[3].PWM_CPDRUPD = period; // Period in half uS
        if (period != 2500 * 2) {
          setupPulses(EXTERNAL_MODULE);
        }
        else {
          // Kick off serial output here
          register Ssc * sscptr = SSC;
          sscptr->SSC_TPR = CONVERT_PTR_UINT(modulePulsesData[EXTERNAL_MODULE].pxx.pulses);
          sscptr->SSC_TCR = (uint8_t *)modulePulsesData[EXTERNAL_MODULE].pxx.ptr - (uint8_t *)modulePulsesData[EXTERNAL_MODULE].pxx.pulses;
          sscptr->SSC_PTCR = SSC_PTCR_TXTEN; // Start transfers
        }
        break;

      case PROTO_DSM2_LP45:
      case PROTO_DSM2_DSM2:
      case PROTO_DSM2_DSMX:
        // Alternate periods of 19.5mS and 2.5 mS
        period = pwmptr->PWM_CH_NUM[3].PWM_CPDR;
        if (period == 2500 * 2) {
          period = 19500 * 2;
        }
        else {
          period = 2500 * 2;
        }
        pwmptr->PWM_CH_NUM[3].PWM_CPDRUPD = period; // Period in half uS
        if (period != 2500 * 2) {
          setupPulses(EXTERNAL_MODULE);
        }
        else {
          // Kick off serial output here
          register Ssc * sscptr = SSC;
          sscptr->SSC_TPR = CONVERT_PTR_UINT(modulePulsesData[EXTERNAL_MODULE].dsm2.pulses);
          sscptr->SSC_TCR = (uint8_t *)modulePulsesData[EXTERNAL_MODULE].dsm2.ptr - (uint8_t *)modulePulsesData[EXTERNAL_MODULE].dsm2.pulses;
          sscptr->SSC_PTCR = SSC_PTCR_TXTEN; // Start transfers
        }
        break;

      default:
        pwmptr->PWM_CH_NUM[3].PWM_CPDRUPD = modulePulsesData[EXTERNAL_MODULE].ppm.pulses[modulePulsesData[EXTERNAL_MODULE].ppm.index++]; // Period in half uS
        if (modulePulsesData[EXTERNAL_MODULE].ppm.pulses[modulePulsesData[EXTERNAL_MODULE].ppm.index] == 0) {
          modulePulsesData[EXTERNAL_MODULE].ppm.index = 0;
          setupPulses(EXTERNAL_MODULE);
        }
        break;

    }
  }

#if !defined(REVA)
  if (reason & PWM_ISR1_CHID1) {
    pwmptr->PWM_CH_NUM[1].PWM_CPDRUPD = modulePulsesData[EXTRA_MODULE].ppm.pulses[modulePulsesData[EXTRA_MODULE].ppm.index++] ;  // Period in half uS
    if (modulePulsesData[EXTRA_MODULE].ppm.pulses[modulePulsesData[EXTRA_MODULE].ppm.index] == 0) {
      modulePulsesData[EXTRA_MODULE].ppm.index = 0;
      setupPulsesPPM(EXTRA_MODULE);
    }
  }
#endif
}
#endif
