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

void setupPulses(unsigned int port);
void setupPulsesPPM(unsigned int port);
void setupPulsesPXX(unsigned int port);

void set_external_ppm_parameters(uint32_t idleTime, uint32_t delay, uint32_t positive)
{
  TIM1->CCR2 = idleTime;
  TIM1->CCR1 = delay;
  // we are using complementary output so logic has to be reversed here
  TIM1->CCER = TIM_CCER_CC1NE | (positive ? 0 : TIM_CCER_CC1NP);
}

void init_no_pulses(uint32_t port)
{
  EXTERNAL_MODULE_OFF();

  // Timer1, channel 1
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(EXTMODULE_RCC_AHB1Periph_GPIO, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_GPIO, &GPIO_InitStructure);

  GPIO_SetBits(EXTMODULE_GPIO, EXTMODULE_GPIO_PIN) ; // Set high
  
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN ;            // Enable clock

  TIM1->CR1 &= ~TIM_CR1_CEN ;
  TIM1->ARR = 36000 ;             // 18mS
  TIM1->CCR2 = 32000 ;            // Update time
  TIM1->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 2000000 - 1 ;               // 0.5uS from 30MHz

  TIM1->CCER = TIM_CCER_CC3E ;

  TIM1->CCMR2 = 0 ;
  TIM1->EGR = 1 ;                                                         // Restart

  TIM1->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0 ;                     // Toggle CC1 o/p
  TIM1->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  TIM1->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  TIM1->CR1 |= TIM_CR1_CEN ;
  NVIC_EnableIRQ(TIM1_CC_IRQn) ;
  NVIC_SetPriority(TIM1_CC_IRQn, 7);
}

void disable_no_pulses(uint32_t port)
{
  NVIC_DisableIRQ(TIM1_CC_IRQn) ;
  TIM1->DIER &= ~TIM_DIER_CC2IE ;
  TIM1->CR1 &= ~TIM_CR1_CEN ;
}

void init_pxx(uint32_t port)
{
  EXTERNAL_MODULE_ON();

  // Timer1, channel1
  setupPulsesPXX(EXTERNAL_MODULE);

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN ;           // Enable portA clock
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(EXTMODULE_RCC_AHB1Periph_GPIO, ENABLE);
  GPIO_PinAFConfig(EXTMODULE_GPIO, EXTMODULE_GPIO_PinSource, GPIO_AF_TIM1);
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_GPIO, &GPIO_InitStructure);

  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN ;            // Enable clock
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN ;            // Enable DMA2 clock

  TIM1->CR1 &= ~TIM_CR1_CEN ;
  TIM1->ARR = 18000 ;                     // 9mS
  TIM1->CCR2 = 15000 ;            // Update time
  TIM1->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 2000000 - 1 ;               // 0.5uS from 30MHz
  TIM1->CCER = TIM_CCER_CC1E ;
  TIM1->CR2 = TIM_CR2_OIS1 ;              // O/P idle high
  TIM1->BDTR = TIM_BDTR_MOE ;             // Enable outputs
  TIM1->CCR1 = modulePulsesData[EXTERNAL_MODULE].pxx.pulses[0];
  TIM1->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0 ;                     // Force O/P high
  TIM1->EGR = 1 ;                                                         // Restart
  TIM1->DIER |= TIM_DIER_CC1DE ;                        // Enable DMA on CC3 match
  TIM1->DCR = 15 ;    // TODO check                        // DMA to CC1

  // Enable the DMA channel here, DMA2 stream 2, channel 7
  DMA2_Stream2->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  DMA2->LIFCR = DMA_LIFCR_CTCIF2 | DMA_LIFCR_CHTIF2 | DMA_LIFCR_CTEIF2 | DMA_LIFCR_CDMEIF2 | DMA_LIFCR_CFEIF2 ; // Write ones to clear bits
  DMA2_Stream2->CR = DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_2 | DMA_SxCR_PL_0 | DMA_SxCR_MSIZE_0
                                                         | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_PFCTRL ;
  DMA2_Stream2->PAR = CONVERT_PTR_UINT(&TIM1->DMAR);
  DMA2_Stream2->M0AR = CONVERT_PTR_UINT(&modulePulsesData[EXTERNAL_MODULE].pxx.pulses[1]);
//      DMA2_Stream2->FCR = 0x05 ; //DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 ;
//      DMA2_Stream2->NDTR = 100 ;
  DMA2_Stream2->CR |= DMA_SxCR_EN ;               // Enable DMA

  TIM1->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0 ;                     // Toggle CC1 o/p
  TIM1->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  TIM1->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  TIM1->CR1 |= TIM_CR1_CEN ;
  NVIC_EnableIRQ(TIM1_CC_IRQn);
  NVIC_SetPriority(TIM1_CC_IRQn, 7);
}

void disable_pxx(uint32_t port)
{
  DMA2_Stream2->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  NVIC_DisableIRQ(TIM1_CC_IRQn) ;
  TIM1->DIER &= ~TIM_DIER_CC2IE ;
  TIM1->CR1 &= ~TIM_CR1_CEN ;
  EXTERNAL_MODULE_OFF();
}

#if defined(DSM2)
void init_dsm2(uint32_t port)
{
  EXTERNAL_MODULE_ON();

  // Timer8
  setupPulsesDSM2(EXTERNAL_MODULE);

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN ;           // Enable portA clock
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(EXTMODULE_RCC_AHB1Periph_GPIO, ENABLE);
  GPIO_PinAFConfig(EXTMODULE_GPIO, EXTMODULE_GPIO_PinSource, GPIO_AF_TIM1);
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_GPIO, &GPIO_InitStructure);
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN ;            // Enable clock
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN ;            // Enable DMA2 clock

  TIM1->CR1 &= ~TIM_CR1_CEN ;
  TIM1->ARR = 44000 ;                     // 22mS
  TIM1->CCR2 = 40000 ;            // Update time
  TIM1->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 2000000 - 1 ;               // 0.5uS from 30MHz
  TIM1->CCER = TIM_CCER_CC1NE  | TIM_CCER_CC1NP ;
  TIM1->CR2 = TIM_CR2_OIS1 ;                      // O/P idle high
  TIM1->BDTR = TIM_BDTR_MOE ;             // Enable outputs
  TIM1->CCR1 = modulePulsesData[EXTERNAL_MODULE].dsm2.pulses[0];
  TIM1->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0 ;                     // Force O/P high
  TIM1->EGR = 1 ;                                                         // Restart

//      TIM1->SR &= ~TIM_SR_UIF ;                               // Clear flag
//      TIM1->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  TIM1->DIER |= TIM_DIER_CC1DE ;          // Enable DMA on CC1 match
  TIM1->DCR = 13 ;                                                                // DMA to CC1

//      TIM1->CR1 = TIM_CR1_OPM ;                               // Just run once
  // Enable the DMA channel here, DMA2 stream 2, channel 7
  DMA2_Stream2->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  DMA2->LIFCR = DMA_LIFCR_CTCIF2 | DMA_LIFCR_CHTIF2 | DMA_LIFCR_CTEIF2 | DMA_LIFCR_CDMEIF2 | DMA_LIFCR_CFEIF2 ; // Write ones to clear bits
  DMA2_Stream2->CR = DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_2 | DMA_SxCR_PL_0 | DMA_SxCR_MSIZE_0
                                                         | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_PFCTRL ;
  DMA2_Stream2->PAR = CONVERT_PTR_UINT(&TIM1->DMAR);
  DMA2_Stream2->M0AR = CONVERT_PTR_UINT(&modulePulsesData[EXTERNAL_MODULE].dsm2.pulses[1]);
//      DMA2_Stream2->FCR = 0x05 ; //DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 ;
//      DMA2_Stream2->NDTR = 100 ;
  DMA2_Stream2->CR |= DMA_SxCR_EN ;               // Enable DMA

  TIM1->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0 ;                     // Toggle CC1 o/p
  TIM1->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  TIM1->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  TIM1->CR1 |= TIM_CR1_CEN ;
  NVIC_EnableIRQ(TIM1_CC_IRQn) ;
  NVIC_SetPriority(TIM1_CC_IRQn, 7);
}

void disable_dsm2(uint32_t port)
{
  DMA2_Stream2->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  NVIC_DisableIRQ(TIM1_CC_IRQn) ;
  TIM1->DIER &= ~TIM_DIER_CC2IE ;
  TIM1->CR1 &= ~TIM_CR1_CEN ;
  EXTERNAL_MODULE_OFF();
}
#endif

// PPM output
// Timer 1, channel 1 on PA8 for prototype
// Pin is AF1 function for timer 1
void init_ppm(uint32_t port)
{
  EXTERNAL_MODULE_ON();

  // Timer1
  modulePulsesData[EXTERNAL_MODULE].ppm.ptr = modulePulsesData[EXTERNAL_MODULE].ppm.pulses;

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN ;           // Enable portA clock
  configure_pins( EXTMODULE_GPIO_PIN, PIN_PERIPHERAL | PIN_PORTA | PIN_PER_3 | PIN_OS25 | PIN_PUSHPULL ) ;
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN ;            // Enable clock
  TIM1->CR1 &= ~TIM_CR1_CEN ;

  // setupPulsesPPM() is also configuring registers,
  // so it has to be called after the peripheral is enabled
  setupPulsesPPM(EXTERNAL_MODULE) ;

  TIM1->ARR = *modulePulsesData[EXTERNAL_MODULE].ppm.ptr++ ;
  TIM1->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 2000000 - 1 ;               // 0.5uS from 30MHz

  TIM1->CCER = TIM_CCER_CC3E ;

  TIM1->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC2PE;        // PWM mode 1
  TIM1->BDTR = TIM_BDTR_MOE ;
  TIM1->EGR = 1 ;
  TIM1->DIER = TIM_DIER_UDE ;

  TIM1->SR &= ~TIM_SR_UIF ;                               // Clear flag
  TIM1->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  TIM1->DIER |= TIM_DIER_CC2IE ;
  TIM1->DIER |= TIM_DIER_UIE ;

  TIM1->CR1 = TIM_CR1_CEN ;
  NVIC_EnableIRQ(TIM1_CC_IRQn) ;
  NVIC_SetPriority(TIM1_CC_IRQn, 7);
  NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn) ;
  NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 7);
}

void disable_ppm(uint32_t port)
{
  NVIC_DisableIRQ(TIM1_CC_IRQn) ;
  NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn) ;
  TIM1->DIER &= ~TIM_DIER_CC2IE & ~TIM_DIER_UIE ;
  TIM1->CR1 &= ~TIM_CR1_CEN ;
  EXTERNAL_MODULE_OFF();
}

#if !defined(SIMU)
extern "C" void TIM1_CC_IRQHandler()
{
  TIM1->DIER &= ~TIM_DIER_CC2IE ;         // stop this interrupt
  TIM1->SR &= ~TIM_SR_CC2IF ;                             // Clear flag

  setupPulses(EXTERNAL_MODULE) ;

  if (s_current_protocol[EXTERNAL_MODULE] == PROTO_PXX) {
    DMA2_Stream2->CR &= ~DMA_SxCR_EN ;              // Disable DMA
    DMA2->LIFCR = DMA_LIFCR_CTCIF2 | DMA_LIFCR_CHTIF2 | DMA_LIFCR_CTEIF2 | DMA_LIFCR_CDMEIF2 | DMA_LIFCR_CFEIF2 ; // Write ones to clear bits
    DMA2_Stream2->M0AR = CONVERT_PTR_UINT(&modulePulsesData[EXTERNAL_MODULE].pxx.pulses[1]);
    DMA2_Stream2->CR |= DMA_SxCR_EN ;               // Enable DMA
    TIM1->CCR1 = modulePulsesData[EXTERNAL_MODULE].pxx.pulses[0];
    TIM1->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  }
#if defined(DSM2)
  else if (s_current_protocol[EXTERNAL_MODULE] >= PROTO_DSM2_LP45 && s_current_protocol[EXTERNAL_MODULE] <= PROTO_DSM2_DSMX) {
    DMA2_Stream2->CR &= ~DMA_SxCR_EN ;              // Disable DMA
    DMA2->LIFCR = DMA_LIFCR_CTCIF2 | DMA_LIFCR_CHTIF2 | DMA_LIFCR_CTEIF2 | DMA_LIFCR_CDMEIF2 | DMA_LIFCR_CFEIF2 ; // Write ones to clear bits
    DMA2_Stream2->M0AR = CONVERT_PTR_UINT(&modulePulsesData[EXTERNAL_MODULE].dsm2.pulses[1]);
    DMA2_Stream2->CR |= DMA_SxCR_EN ;               // Enable DMA
    TIM1->CCR1 = modulePulsesData[EXTERNAL_MODULE].dsm2.pulses[0];
    TIM1->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  }
#endif
  else if (s_current_protocol[EXTERNAL_MODULE] == PROTO_PPM) {
    modulePulsesData[EXTERNAL_MODULE].ppm.ptr = modulePulsesData[EXTERNAL_MODULE].ppm.pulses;
    TIM1->DIER |= TIM_DIER_UDE ;
    TIM1->SR &= ~TIM_SR_UIF ;                                       // Clear this flag
    TIM1->DIER |= TIM_DIER_UIE ;                            // Enable this interrupt
  }
  else {
    TIM1->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  }
}

extern "C" void TIM1_UP_TIM10_IRQHandler()
{
  TIM1->SR &= ~TIM_SR_UIF ;                               // Clear flag

  TIM1->ARR = *modulePulsesData[EXTERNAL_MODULE].ppm.ptr++ ;
  if (*modulePulsesData[EXTERNAL_MODULE].ppm.ptr == 0) {
    TIM1->SR &= ~TIM_SR_CC2IF ;                     // Clear this flag
    TIM1->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  }
}
#endif
