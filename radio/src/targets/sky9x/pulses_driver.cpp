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

void setExternalModulePolarity()
{
  Pwm * pwmptr = PWM;
  pwmptr->PWM_CH_NUM[3].PWM_CDTYUPD = GET_PPM_DELAY(EXTERNAL_MODULE) * 2; // Duty in half uS
  if (GET_PPM_POLARITY(EXTERNAL_MODULE))
    pwmptr->PWM_CH_NUM[3].PWM_CMR &= ~0x00000200;   // CPOL
  else
    pwmptr->PWM_CH_NUM[3].PWM_CMR |= 0x00000200;    // CPOL
}

void setExtraModulePolarity()
{
  Pwm * pwmptr = PWM;
  pwmptr->PWM_CH_NUM[1].PWM_CDTYUPD = GET_PPM_DELAY(EXTRA_MODULE) * 2; // Duty in half uS
  if (GET_PPM_POLARITY(EXTRA_MODULE))
    pwmptr->PWM_CH_NUM[1].PWM_CMR &= ~0x00000200;   // CPOL
  else
    pwmptr->PWM_CH_NUM[1].PWM_CMR |= 0x00000200;    // CPOL
}

void module_output_active()
{
  Pio *pioptr = PIOA;
  pioptr->PIO_ABCDSR[0] &= ~PIO_PA17;            // Peripheral C
  pioptr->PIO_ABCDSR[1] |= PIO_PA17;             // Peripheral C
  pioptr->PIO_PDR = PIO_PA17;                    // Disable bit A17 Assign to peripheral
#if defined(REVX)
  if (g_model.moduleData[EXTERNAL_MODULE].ppm.outputType) {
    pioptr->PIO_MDDR = PIO_PA17;                 // Push Pull O/p in A17
  }
  else {
    pioptr->PIO_MDER = PIO_PA17;                 // Open Drain O/p in A17
  }
#else
  pioptr->PIO_MDDR = PIO_PA17;                   // Push Pull O/p in A17
#endif
  pioptr->PIO_PUER = PIO_PA17;                   // With pull up
}

void init_main_ppm(uint32_t period, uint32_t out_enable)
{
  Pwm * pwmptr;

  setupPulsesPPMModule(EXTERNAL_MODULE);

  if (out_enable) {
    module_output_active();
  }

  pwmptr = PWM;
  // PWM3 for PPM output
  pwmptr->PWM_CH_NUM[3].PWM_CMR = 0x0004000B;                          // CLKA
  pwmptr->PWM_CH_NUM[3].PWM_CPDR = period;                             // Period in half uS
  pwmptr->PWM_CH_NUM[3].PWM_CPDRUPD = period;                          // Period in half uS
  pwmptr->PWM_CH_NUM[3].PWM_CDTY = GET_PPM_DELAY(EXTERNAL_MODULE) * 2; // Duty in half uS
  pwmptr->PWM_ENA = PWM_ENA_CHID3;                                     // Enable channel 3
  pwmptr->PWM_IER1 = PWM_IER1_CHID3;

  setExternalModulePolarity();

  NVIC_SetPriority(PWM_IRQn, 3);
  NVIC_EnableIRQ(PWM_IRQn);
}

void disable_main_ppm()
{
  Pio * pioptr = PIOA;
  pioptr->PIO_PER = PIO_PA17;                                          // Assign A17 to PIO
  PWM->PWM_IDR1 = PWM_IDR1_CHID3;
}

void init_second_ppm(uint32_t period)
{
#if !defined(REVA)
  // PWM1 for PPM2
  Pwm * pwmptr = PWM;
  configure_pins(PIO_PC15, PIN_PERIPHERAL | PIN_INPUT | PIN_PER_B | PIN_PORTC | PIN_NO_PULLUP);
  pwmptr->PWM_CH_NUM[1].PWM_CMR = 0x0000000B;                          // CLKB
  if (!GET_PPM_POLARITY(EXTRA_MODULE)) {
    pwmptr->PWM_CH_NUM[1].PWM_CMR |= 0x00000200;                       // CPOL
  }
  pwmptr->PWM_CH_NUM[1].PWM_CPDR = period;                             // Period
  pwmptr->PWM_CH_NUM[1].PWM_CPDRUPD = period;                          // Period
  pwmptr->PWM_CH_NUM[1].PWM_CDTY = GET_PPM_DELAY(EXTRA_MODULE)*2;      // Duty
  pwmptr->PWM_CH_NUM[1].PWM_CDTYUPD = GET_PPM_DELAY(EXTRA_MODULE)*2;   // Duty
  pwmptr->PWM_ENA = PWM_ENA_CHID1;                                     // Enable channel 1
  pwmptr->PWM_IER1 = PWM_IER1_CHID1;
#endif
}

void disable_second_ppm()
{
#if !defined(REVA)
  Pio * pioptr = PIOC;
  pioptr->PIO_PER = PIO_PC15;           // Assign C17 to PIO
  PWM->PWM_IDR1 = PWM_IDR1_CHID1;
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
void init_ssc(uint8_t baudrateDiv1000)
{
  Ssc *sscptr;

  PMC->PMC_PCER0 |= 0x00400000L;        // Enable peripheral clock to SSC

  configure_pins(PIO_PA17, PIN_PERIPHERAL | PIN_INPUT | PIN_PER_A | PIN_PORTA);

  sscptr = SSC;
  sscptr->SSC_THR = 0xFF;                    // Make the output high.
  sscptr->SSC_TFMR = 0x00000027;        //  0000 0000 0000 0000 0000 0000 1010 0111 (8 bit data, lsb)
  sscptr->SSC_CMR = Master_frequency / (1000 * baudrateDiv1000 * 2);
  sscptr->SSC_TCMR = 0;
  sscptr->SSC_CR = SSC_CR_TXEN;

#if defined(REVX)
  if (IS_MODULE_MULTIMODULE(EXTERNAL_MODULE)) {
    PIOA->PIO_MDDR = PIO_PA17;                 // Push Pull O/p in A17
  } else {
    PIOA->PIO_MDER = PIO_PA17;						// Open Drain O/p in A17
  }
#else
  PIOA->PIO_MDDR = PIO_PA17;						// Push Pull O/p in A17
#endif
}

void disable_ssc()
{
  Pio *pioptr;
  Ssc *sscptr;

  // Revert back to pwm output
  pioptr = PIOA;
  pioptr->PIO_PER = PIO_PA17;           // Assign A17 to PIO

  sscptr = SSC;
  sscptr->SSC_CR = SSC_CR_TXDIS;
}

void init_pxx(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    init_main_ppm(2500 * 2, 0);
    init_ssc(125);                      // 8us per bit
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

void init_serial(uint32_t port, uint32_t baudrate, uint32_t period_half_us)
{
  if (port == EXTERNAL_MODULE) {
    if (baudrate == 125000) {
      // TODO init_main_ppm could take the period as parameter?
      init_main_ppm(2500 * 2, 0);
      init_ssc(125);
    }
    else {
      init_main_ppm(3500 * 2, 0);
      init_ssc(100);
    }
    }
  else {
    // TODO
  }
}

void disable_serial(uint32_t port)
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
  Pwm * pwmptr = PWM;
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
          setExternalModulePolarity();
        }
        else {
          // Kick off serial output here
          Ssc * sscptr = SSC;
          sscptr->SSC_TPR = CONVERT_PTR_UINT(modulePulsesData[EXTERNAL_MODULE].pxx.pulses);
          sscptr->SSC_TCR = (uint8_t *)modulePulsesData[EXTERNAL_MODULE].pxx.ptr - (uint8_t *)modulePulsesData[EXTERNAL_MODULE].pxx.pulses;
          sscptr->SSC_PTCR = SSC_PTCR_TXTEN;        // Start transfers
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
          Ssc * sscptr = SSC;
          sscptr->SSC_TPR = CONVERT_PTR_UINT(modulePulsesData[EXTERNAL_MODULE].dsm2.pulses);
          sscptr->SSC_TCR = (uint8_t *)modulePulsesData[EXTERNAL_MODULE].dsm2.ptr - (uint8_t *)modulePulsesData[EXTERNAL_MODULE].dsm2.pulses;
          sscptr->SSC_PTCR = SSC_PTCR_TXTEN;        // Start transfers
        }
        break;

#if defined(MULTIMODULE)
      case PROTO_MULTIMODULE:
#endif
      case PROTO_SBUS:
        // Todo: how to do inverted polarity on this platform?
        // Alternate periods of 5.5mS and 3.5 mS
        period = pwmptr->PWM_CH_NUM[3].PWM_CPDR;
        if (period == 3500 * 2) {
          period = 5500 * 2;
        }
        else {
          period = 3500 * 2;
        }
        pwmptr->PWM_CH_NUM[3].PWM_CPDRUPD = period; // Period in half uS
        if (period != 3500 * 2) {
          setupPulses(EXTERNAL_MODULE);
        }
        else {
          // Kick off serial output here
          Ssc * sscptr = SSC;
          sscptr->SSC_TPR = CONVERT_PTR_UINT(modulePulsesData[EXTERNAL_MODULE].dsm2.pulses);
          sscptr->SSC_TCR = (uint8_t *)modulePulsesData[EXTERNAL_MODULE].dsm2.ptr - (uint8_t *)modulePulsesData[EXTERNAL_MODULE].dsm2.pulses;
          sscptr->SSC_PTCR = SSC_PTCR_TXTEN;        // Start transfers
        }
        break;

      default:
        pwmptr->PWM_CH_NUM[3].PWM_CPDRUPD = *modulePulsesData[EXTERNAL_MODULE].ppm.ptr++;
        if (*modulePulsesData[EXTERNAL_MODULE].ppm.ptr == 0) {
          setExternalModulePolarity();
          setupPulses(EXTERNAL_MODULE);
        }
        break;

    }
  }

#if !defined(REVA)
  if (reason & PWM_ISR1_CHID1) {
    pwmptr->PWM_CH_NUM[1].PWM_CPDRUPD = *modulePulsesData[EXTRA_MODULE].ppm.ptr++;
    if (*modulePulsesData[EXTRA_MODULE].ppm.ptr == 0) {
      setupPulsesPPMModule(EXTRA_MODULE);
      setExtraModulePolarity();
    }
  }
#endif
}
#endif
