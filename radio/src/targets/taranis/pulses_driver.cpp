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

static void intmodulePxxStart( void ) ;
static void intmodulePxxStop( void ) ;
#if defined(TARANIS_INTERNAL_PPM)
  static void intmodulePpmStart( void ) ;
  static void intmodulePpmStop( void ) ;
#endif
static void extmodulePxxStart( void ) ;
static void extmodulePxxStop( void ) ;
#if defined(DSM2) || defined(MULTIMODULE)
static void extmoduleDsm2Start( void ) ;
static void extmoduleDsm2Stop( void ) ;
#endif
static void extmodulePpmStart( void ) ;
static void extmodulePpmStop( void ) ;
static void intmoduleNoneStart( void ) ;
static void intmoduleNoneStop( void ) ;
static void extmoduleNoneStart( void ) ;
static void extmoduleNoneStop( void ) ;
static void extmoduleCrossfireStart( void ) ;
static void extmoduleCrossfireStop( void ) ;

void init_pxx(uint32_t port)
{
  if (port == INTERNAL_MODULE)
    intmodulePxxStart() ;
  else
    extmodulePxxStart() ;
}

void disable_pxx(uint32_t port)
{
  if (port == INTERNAL_MODULE)
    intmodulePxxStop() ;
  else
    extmodulePxxStop() ;
}

#if defined(DSM2) || defined(MULTIMODULE)
void init_dsm2(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmoduleDsm2Start();
  }
}

void disable_dsm2(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmoduleDsm2Stop();
  }
}
#endif

void init_ppm(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmodulePpmStart();
  }
#if defined(TARANIS_INTERNAL_PPM)
  else if (port == INTERNAL_MODULE) {
    intmodulePpmStart();
  }
#endif
}

void disable_ppm(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmodulePpmStop();
  }
#if defined(TARANIS_INTERNAL_PPM)
  else if (port == INTERNAL_MODULE) {
    intmodulePpmStop();
  }
#endif
}

void set_external_ppm_parameters(uint32_t idleTime, uint32_t delay, uint32_t positive)
{
  EXTMODULE_TIMER->CCR2 = idleTime;
  EXTMODULE_TIMER->CCR1 = delay;
  // we are using complementary output so logic has to be reversed here
  EXTMODULE_TIMER->CCER = TIM_CCER_CC1NE | (positive ? 0 : TIM_CCER_CC1NP);
}

#if defined(TARANIS_INTERNAL_PPM)
void set_internal_ppm_parameters(uint32_t idleTime, uint32_t delay, uint32_t positive)
{
  INTMODULE_TIMER->CCR2 = idleTime;
  INTMODULE_TIMER->CCR3 = delay;
  INTMODULE_TIMER->CCER = TIM_CCER_CC3E | (positive ? TIM_CCER_CC3P : 0);
}
#endif

void init_no_pulses(uint32_t port)
{
  if (port == INTERNAL_MODULE)
    intmoduleNoneStart();
  else
    extmoduleNoneStart();
}

void disable_no_pulses(uint32_t port)
{
  if (port == INTERNAL_MODULE)
    intmoduleNoneStop();
  else
    extmoduleNoneStop();
}

void init_crossfire(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmoduleCrossfireStart();
  }
}

void disable_crossfire(uint32_t port)
{
  if (port == EXTERNAL_MODULE)
    extmoduleCrossfireStop();
}

static void intmoduleNoneStart()
{
  INTERNAL_MODULE_OFF();

  // Timer1, channel 3
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = INTMODULE_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(INTMODULE_GPIO, &GPIO_InitStructure);
  
  GPIO_SetBits(INTMODULE_GPIO, INTMODULE_GPIO_PIN) ; // Set high
  
  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  INTMODULE_TIMER->ARR = 36000 ;             // 18mS
  INTMODULE_TIMER->CCR2 = 32000 ;            // Update time
  INTMODULE_TIMER->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 2000000 - 1 ;               // 0.5uS from 30MHz
  
  INTMODULE_TIMER->CCER = TIM_CCER_CC3E ;
  
  INTMODULE_TIMER->CCMR2 = 0 ;
  INTMODULE_TIMER->EGR = 1 ;                                                         // Restart

  INTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0 ;                     // Toggle CC1 o/p
  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  INTMODULE_TIMER->CR1 |= TIM_CR1_CEN ;
  NVIC_EnableIRQ(TIM1_CC_IRQn) ;
  NVIC_SetPriority(TIM1_CC_IRQn, 7);
}

static void intmoduleNoneStop()
{
  NVIC_DisableIRQ(TIM1_CC_IRQn) ;
  INTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE ;
  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
}

static void extmoduleNoneStart()
{
  if (!IS_TRAINER_EXTERNAL_MODULE()) {
    EXTERNAL_MODULE_OFF();
  }

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_GPIO, &GPIO_InitStructure);
  GPIO_SetBits(EXTMODULE_GPIO, EXTMODULE_GPIO_PIN); // Set high
  
  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  EXTMODULE_TIMER->ARR = 36000 ;             // 18mS
  EXTMODULE_TIMER->CCR2 = 32000 ;            // Update time
  EXTMODULE_TIMER->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 2000000 - 1 ;               // 0.5uS from 30MHz
  
  EXTMODULE_TIMER->CCMR2 = 0 ;
  EXTMODULE_TIMER->EGR = 1 ;                                                         // Restart

  EXTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0 ;                     // Toggle CC1 o/p
  EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN ;
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn) ;
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}

static void extmoduleNoneStop()
{
  NVIC_DisableIRQ(EXTMODULE_TIMER_IRQn) ;
  EXTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE ;
  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
}

static void extmoduleCrossfireStart()
{
  EXTERNAL_MODULE_ON();

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_GPIO, &GPIO_InitStructure);
  GPIO_SetBits(EXTMODULE_GPIO, EXTMODULE_GPIO_PIN); // Set high

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  EXTMODULE_TIMER->ARR = 5000 ;             // 2.5mS
  EXTMODULE_TIMER->CCR2 = 32000 ;            // Update time
  EXTMODULE_TIMER->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 2000000 - 1 ;               // 0.5uS from 30MHz

  EXTMODULE_TIMER->CCMR2 = 0 ;
  EXTMODULE_TIMER->EGR = 1 ;                                                         // Restart

  EXTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0 ;                     // Toggle CC1 o/p
  EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN ;
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn) ;
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}

static void extmoduleCrossfireStop()
{
  NVIC_DisableIRQ(EXTMODULE_TIMER_IRQn) ;
  EXTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE ;
  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;

  if (!IS_TRAINER_EXTERNAL_MODULE()) {
    EXTERNAL_MODULE_OFF();
  }
}

static void intmodulePxxStart()
{
  INTERNAL_MODULE_ON();

  // Timer1, channel 3
  setupPulsesPXX(INTERNAL_MODULE) ; // TODO not here!

  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_PinAFConfig(INTMODULE_GPIO, INTMODULE_GPIO_PinSource, INTMODULE_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = INTMODULE_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(INTMODULE_GPIO, &GPIO_InitStructure);
  
  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  INTMODULE_TIMER->ARR = 18000 ;                     // 9mS
  INTMODULE_TIMER->CCR2 = 15000 ;            // Update time
  INTMODULE_TIMER->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 2000000 - 1 ;               // 0.5uS from 30MHz
  INTMODULE_TIMER->CCER = TIM_CCER_CC3E ;
  INTMODULE_TIMER->CR2 = TIM_CR2_OIS3 ;              // O/P idle high
  INTMODULE_TIMER->BDTR = TIM_BDTR_MOE ;             // Enable outputs
  INTMODULE_TIMER->CCR3 = modulePulsesData[INTERNAL_MODULE].pxx.pulses[0];
  INTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_0 ;                     // Force O/P high
  INTMODULE_TIMER->EGR = 1 ;                                                         // Restart
  INTMODULE_TIMER->DIER |= TIM_DIER_CC3DE ;          		// Enable DMA on CC3 match
  INTMODULE_TIMER->DCR = 15 ;                            // DMA to CC1

  // Enable the DMA channel here, DMA2 stream 6, channel 6
  DMA2_Stream6->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  DMA2->HIFCR = DMA_HIFCR_CTCIF6 | DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTEIF6 | DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CFEIF6 ; // Write ones to clear bits
  DMA2_Stream6->CR = DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_2 | DMA_SxCR_PL_0 | DMA_SxCR_MSIZE_0
                                                         | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_PFCTRL ;
  DMA2_Stream6->PAR = CONVERT_PTR_UINT(&INTMODULE_TIMER->DMAR);
  DMA2_Stream6->M0AR = CONVERT_PTR_UINT(&modulePulsesData[INTERNAL_MODULE].pxx.pulses[1]);
  DMA2_Stream6->CR |= DMA_SxCR_EN ;               // Enable DMA

  INTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0 ;                     // Toggle CC1 o/p
  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  INTMODULE_TIMER->CR1 |= TIM_CR1_CEN ;
  NVIC_EnableIRQ(TIM1_CC_IRQn);
  NVIC_SetPriority(TIM1_CC_IRQn, 7);
}

static void intmodulePxxStop()
{
  DMA2_Stream6->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  NVIC_DisableIRQ(TIM1_CC_IRQn) ;
  INTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE ;
  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  INTERNAL_MODULE_OFF();
}

#if defined(TARANIS_INTERNAL_PPM)
// PPM output
// Timer 1, channel 1 on PA8 for prototype
// Pin is AF1 function for timer 1
static void intmodulePpmStart()
{
  INTERNAL_MODULE_ON();

  // Timer1
  configure_pins(INTMODULE_GPIO_PIN, PIN_PERIPHERAL | PIN_PORTA | PIN_PER_1 | PIN_OS25);
  
  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;

  // setupPulsesPPM() is also configuring registers,
  // so it has to be called after the peripheral is enabled
  setupPulsesPPM(INTERNAL_MODULE) ;

  INTMODULE_TIMER->ARR = *modulePulsesData[INTERNAL_MODULE].ppm.ptr++ ;
  INTMODULE_TIMER->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 2000000 - 1 ;               // 0.5uS from 30MHz
  
  INTMODULE_TIMER->CCER = TIM_CCER_CC3E ;
  
  INTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2 ;     // PWM mode 1
  INTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC2PE ;                   			// PWM mode 1
  INTMODULE_TIMER->BDTR = TIM_BDTR_MOE ;
  INTMODULE_TIMER->EGR = 1 ;
  INTMODULE_TIMER->DIER = TIM_DIER_UDE ;

  INTMODULE_TIMER->SR &= ~TIM_SR_UIF ;                               // Clear flag
  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;
  INTMODULE_TIMER->DIER |= TIM_DIER_UIE ;

  INTMODULE_TIMER->CR1 = TIM_CR1_CEN ;
  NVIC_EnableIRQ(TIM1_CC_IRQn) ;
  NVIC_SetPriority(TIM1_CC_IRQn, 7);
  NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn) ;
  NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 7);
}

static void intmodulePpmStop()
{
  NVIC_DisableIRQ(TIM1_CC_IRQn) ;
  NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn) ;
  INTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE & ~TIM_DIER_UIE ;
  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;

  INTERNAL_MODULE_OFF();
}
#endif  // #if defined(TARANIS_INTERNAL_PPM)

#if !defined(SIMU)
extern "C" void TIM1_CC_IRQHandler()
{
  INTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE;       // stop this interrupt
  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF;           // clear flag
  DMA2_Stream6->CR &= ~DMA_SxCR_EN;    // disable DMA, it will have the whole of the execution time of setupPulses() to actually stop

  setupPulses(INTERNAL_MODULE);

  if (s_current_protocol[INTERNAL_MODULE] == PROTO_PXX) {
    DMA2->HIFCR = DMA_HIFCR_CTCIF6 | DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTEIF6 | DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CFEIF6;
    DMA2_Stream6->M0AR = CONVERT_PTR_UINT(&modulePulsesData[INTERNAL_MODULE].pxx.pulses[1]);
    DMA2_Stream6->CR |= DMA_SxCR_EN;   // enable DMA
    INTMODULE_TIMER->CCR3 = modulePulsesData[INTERNAL_MODULE].pxx.pulses[0];
    INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE;      // enable this interrupt
  }
#if defined(TARANIS_INTERNAL_PPM)
  else if (s_current_protocol[INTERNAL_MODULE] == PROTO_PPM) {
    INTMODULE_TIMER->DIER |= TIM_DIER_UDE;
    INTMODULE_TIMER->SR &= ~TIM_SR_UIF;
    INTMODULE_TIMER->DIER |= TIM_DIER_UIE;
  }
#endif
  else {
    INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE;
  }
}

extern "C" void TIM1_UP_TIM10_IRQHandler()
{
  INTMODULE_TIMER->SR &= ~TIM_SR_UIF ;                               // Clear flag

  INTMODULE_TIMER->ARR = *modulePulsesData[INTERNAL_MODULE].ppm.ptr++ ;
  if (*modulePulsesData[INTERNAL_MODULE].ppm.ptr == 0) {
    INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;                     // Clear this flag
    INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  }
}
#endif

void extmodulePxxStart()
{
  EXTERNAL_MODULE_ON();

  // Timer8
  setupPulsesPXX(EXTERNAL_MODULE);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_PinAFConfig(EXTMODULE_GPIO, EXTMODULE_GPIO_PinSource, EXTMODULE_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_GPIO, &GPIO_InitStructure);

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  EXTMODULE_TIMER->ARR = 18000 ;                     // 9mS
  EXTMODULE_TIMER->CCR2 = 15000 ;            // Update time
  EXTMODULE_TIMER->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 2000000 - 1 ;               // 0.5uS from 30MHz
  EXTMODULE_TIMER->CCER = TIM_CCER_CC1NE ;
  EXTMODULE_TIMER->CR2 = TIM_CR2_OIS1 ;                      // O/P idle high
  EXTMODULE_TIMER->BDTR = TIM_BDTR_MOE ;             // Enable outputs
  EXTMODULE_TIMER->CCR1 = modulePulsesData[EXTERNAL_MODULE].pxx.pulses[0];
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0 ;                     // Force O/P high
  EXTMODULE_TIMER->EGR = 1 ;                                                         // Restart

  EXTMODULE_TIMER->DIER |= TIM_DIER_CC1DE ;          // Enable DMA on CC1 match
  EXTMODULE_TIMER->DCR = 13 ;                                                                // DMA to CC1

  // Enable the DMA channel here, DMA2 stream 2, channel 7
  DMA2_Stream2->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  DMA2->LIFCR = DMA_LIFCR_CTCIF2 | DMA_LIFCR_CHTIF2 | DMA_LIFCR_CTEIF2 | DMA_LIFCR_CDMEIF2 | DMA_LIFCR_CFEIF2 ; // Write ones to clear bits
  DMA2_Stream2->CR = DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_2 | DMA_SxCR_PL_0 | DMA_SxCR_MSIZE_0
                                                         | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_PFCTRL ;
  DMA2_Stream2->PAR = CONVERT_PTR_UINT(&EXTMODULE_TIMER->DMAR);
  DMA2_Stream2->M0AR = CONVERT_PTR_UINT(&modulePulsesData[EXTERNAL_MODULE].pxx.pulses[1]);
//      DMA2_Stream2->FCR = 0x05 ; //DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 ;
//      DMA2_Stream2->NDTR = 100 ;
  DMA2_Stream2->CR |= DMA_SxCR_EN ;               // Enable DMA

  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0 ;                     // Toggle CC1 o/p
  EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN ;
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn) ;
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}

static void extmodulePxxStop()
{
  DMA2_Stream2->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  NVIC_DisableIRQ(EXTMODULE_TIMER_IRQn) ;
  EXTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE ;
  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  if (!IS_TRAINER_EXTERNAL_MODULE()) {
    EXTERNAL_MODULE_OFF();
  }
}

#if defined(DSM2) || defined(MULTIMODULE)
static void extmoduleDsm2Start()
{
  EXTERNAL_MODULE_ON();

  setupPulsesDSM2(EXTERNAL_MODULE);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_PinAFConfig(EXTMODULE_GPIO, EXTMODULE_GPIO_PinSource, EXTMODULE_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_GPIO, &GPIO_InitStructure);

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  EXTMODULE_TIMER->ARR = 44000 ;                     // 22mS
  EXTMODULE_TIMER->CCR2 = 40000 ;            // Update time
  EXTMODULE_TIMER->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 2000000 - 1 ;               // 0.5uS from 30MHz
  EXTMODULE_TIMER->CCER = TIM_CCER_CC1NE  | TIM_CCER_CC1NP ;
  EXTMODULE_TIMER->CR2 = TIM_CR2_OIS1 ;                      // O/P idle high
  EXTMODULE_TIMER->BDTR = TIM_BDTR_MOE ;             // Enable outputs
  EXTMODULE_TIMER->CCR1 = modulePulsesData[EXTERNAL_MODULE].dsm2.pulses[0];
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0 ;                     // Force O/P high
  EXTMODULE_TIMER->EGR = 1 ;                                                         // Restart

  EXTMODULE_TIMER->DIER |= TIM_DIER_CC1DE ;          // Enable DMA on CC1 match
  EXTMODULE_TIMER->DCR = 13 ;                                                                // DMA to CC1

  // Enable the DMA channel here, DMA2 stream 2, channel 7
  DMA2_Stream2->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  DMA2->LIFCR = DMA_LIFCR_CTCIF2 | DMA_LIFCR_CHTIF2 | DMA_LIFCR_CTEIF2 | DMA_LIFCR_CDMEIF2 | DMA_LIFCR_CFEIF2 ; // Write ones to clear bits
  DMA2_Stream2->CR = DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_2 | DMA_SxCR_PL_0 | DMA_SxCR_MSIZE_0
                                                         | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_PFCTRL ;
  DMA2_Stream2->PAR = CONVERT_PTR_UINT(&EXTMODULE_TIMER->DMAR);
  DMA2_Stream2->M0AR = CONVERT_PTR_UINT(&modulePulsesData[EXTERNAL_MODULE].dsm2.pulses[1]);
//      DMA2_Stream2->FCR = 0x05 ; //DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 ;
//      DMA2_Stream2->NDTR = 100 ;
  DMA2_Stream2->CR |= DMA_SxCR_EN ;               // Enable DMA

  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0 ;                     // Toggle CC1 o/p
  EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN ;
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn) ;
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}

static void extmoduleDsm2Stop()
{
  DMA2_Stream2->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  NVIC_DisableIRQ(EXTMODULE_TIMER_IRQn) ;
  EXTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE ;
  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  if (!IS_TRAINER_EXTERNAL_MODULE()) {
    EXTERNAL_MODULE_OFF();
  }
}
#endif

// PPM output
// Timer 1, channel 1 on PA8 for prototype
// Pin is AF1 function for timer 1
static void extmodulePpmStart()
{
  EXTERNAL_MODULE_ON();

  // Timer1
  configure_pins(EXTMODULE_GPIO_PIN, PIN_PERIPHERAL | PIN_PORTA | PIN_PER_3 | PIN_OS25);
  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;

  // setupPulsesPPM() is also configuring registers,
  // so it has to be called after the peripheral is enabled
  setupPulsesPPM(EXTERNAL_MODULE) ;

  EXTMODULE_TIMER->ARR = *modulePulsesData[EXTERNAL_MODULE].ppm.ptr++ ;
  EXTMODULE_TIMER->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 2000000 - 1 ;               // 0.5uS from 30MHz

  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC2PE ;                   // PWM mode 1
  EXTMODULE_TIMER->BDTR = TIM_BDTR_MOE ;
  EXTMODULE_TIMER->EGR = 1 ;
  EXTMODULE_TIMER->DIER = TIM_DIER_UDE ;

  EXTMODULE_TIMER->SR &= ~TIM_SR_UIF ;                               // Clear flag
  EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;
  EXTMODULE_TIMER->DIER |= TIM_DIER_UIE ;

  EXTMODULE_TIMER->CR1 = TIM_CR1_CEN ;
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn) ;
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
  NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn) ;
  NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 7);
}

static void extmodulePpmStop()
{
  NVIC_DisableIRQ(EXTMODULE_TIMER_IRQn) ;
  NVIC_DisableIRQ(TIM8_UP_TIM13_IRQn) ;
  EXTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE & ~TIM_DIER_UIE ;
  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  if (!IS_TRAINER_EXTERNAL_MODULE()) {
    EXTERNAL_MODULE_OFF();
  }
}

#if !defined(SIMU)
extern "C" void TIM8_CC_IRQHandler()
{
  EXTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE ;         // stop this interrupt
  EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;                             // Clear flag

  setupPulses(EXTERNAL_MODULE) ;

  if (s_current_protocol[EXTERNAL_MODULE] == PROTO_PXX) {
    DMA2_Stream2->CR &= ~DMA_SxCR_EN ;              // Disable DMA
    DMA2->LIFCR = DMA_LIFCR_CTCIF2 | DMA_LIFCR_CHTIF2 | DMA_LIFCR_CTEIF2 | DMA_LIFCR_CDMEIF2 | DMA_LIFCR_CFEIF2 ; // Write ones to clear bits
    DMA2_Stream2->M0AR = CONVERT_PTR_UINT(&modulePulsesData[EXTERNAL_MODULE].pxx.pulses[1]);
    DMA2_Stream2->CR |= DMA_SxCR_EN ;               // Enable DMA
    EXTMODULE_TIMER->CCR1 = modulePulsesData[EXTERNAL_MODULE].pxx.pulses[0];
    EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  }
#if defined(DSM2)
  else if ((s_current_protocol[EXTERNAL_MODULE] >= PROTO_DSM2_LP45 && s_current_protocol[EXTERNAL_MODULE] <= PROTO_DSM2_DSMX) || IS_MULTIMODULE_PROTOCOL (s_current_protocol[EXTERNAL_MODULE]))

{
    DMA2_Stream2->CR &= ~DMA_SxCR_EN ;              // Disable DMA
    DMA2->LIFCR = DMA_LIFCR_CTCIF2 | DMA_LIFCR_CHTIF2 | DMA_LIFCR_CTEIF2 | DMA_LIFCR_CDMEIF2 | DMA_LIFCR_CFEIF2 ; // Write ones to clear bits
    DMA2_Stream2->M0AR = CONVERT_PTR_UINT(&modulePulsesData[EXTERNAL_MODULE].dsm2.pulses[1]);
    DMA2_Stream2->CR |= DMA_SxCR_EN ;               // Enable DMA
    EXTMODULE_TIMER->CCR1 = modulePulsesData[EXTERNAL_MODULE].dsm2.pulses[0];
    EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  }
#endif
  else if (s_current_protocol[EXTERNAL_MODULE] == PROTO_PPM) {
    EXTMODULE_TIMER->DIER |= TIM_DIER_UDE ;
    EXTMODULE_TIMER->SR &= ~TIM_SR_UIF ;                                       // Clear this flag
    EXTMODULE_TIMER->DIER |= TIM_DIER_UIE ;                            // Enable this interrupt
  }
  else {
    EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  }
}

extern "C" void TIM8_UP_TIM13_IRQHandler()
{
  EXTMODULE_TIMER->SR &= ~TIM_SR_UIF ;                               // Clear flag

  EXTMODULE_TIMER->ARR = *modulePulsesData[EXTERNAL_MODULE].ppm.ptr++ ;
  if (*modulePulsesData[EXTERNAL_MODULE].ppm.ptr == 0) {
    EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;                     // Clear this flag
    EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  }
}
#endif
