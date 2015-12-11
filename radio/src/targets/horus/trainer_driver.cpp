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

extern Fifo<32> sbusFifo;

#define setupTrainerPulses() setupPulsesPPM(TRAINER_MODULE)

// Trainer PPM output PB4, Timer 3 channel 1
void init_trainer_ppm()
{
  trainerPulsesData.ppm.ptr = trainerPulsesData.ppm.pulses;

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN ;
  configure_pins( TRAINER_GPIO_PIN_OUT, PIN_PERIPHERAL | PIN_PORTB | PIN_PER_2 | PIN_OS25 | PIN_PUSHPULL ) ;
  configure_pins( TRAINER_GPIO_PIN_IN, PIN_PORTB | PIN_INPUT ) ;
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN ;
  TRAINER_TIMER->CR1 &= ~TIM_CR1_CEN ;

  // setupTrainerPulses() is also configuring registers,
  // so it has to be called after the peripheral is enabled
  setupTrainerPulses() ;

  TRAINER_TIMER->ARR = *trainerPulsesData.ppm.ptr++ ;
  TRAINER_TIMER->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 2000000 - 1 ;               // 0.5uS
  TRAINER_TIMER->CCMR2 = TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4PE ;                   // PWM mode 1
  TRAINER_TIMER->BDTR = TIM_BDTR_MOE ;
  TRAINER_TIMER->EGR = 1 ;

  TRAINER_TIMER->SR &= ~TIM_SR_UIF ;                               // Clear flag
  TRAINER_TIMER->SR &= ~TIM_SR_CC1IF ;                             // Clear flag
  TRAINER_TIMER->DIER |= TIM_DIER_CC1IE ;
  TRAINER_TIMER->DIER |= TIM_DIER_UIE ;

  TRAINER_TIMER->CR1 = TIM_CR1_CEN ;
  NVIC_EnableIRQ(TRAINER_TIMER_IRQn) ;
  NVIC_SetPriority(TRAINER_TIMER_IRQn, 7);
}

// TODO - testing
void stop_trainer_ppm()
{
  configure_pins( TRAINER_GPIO_PIN_OUT, PIN_INPUT | PIN_PORTB ) ; // Pin as input
  TRAINER_TIMER->DIER = 0 ;                                     // Stop Interrupt
  TRAINER_TIMER->CR1 &= ~TIM_CR1_CEN ;                          // Stop counter
  NVIC_DisableIRQ(TRAINER_TIMER_IRQn) ;                         // Stop Interrupt
}

void set_trainer_ppm_parameters(uint32_t idleTime, uint32_t delay, uint32_t positive)
{
  TRAINER_TIMER->CCR1 = idleTime;
  TRAINER_TIMER->CCR4 = delay;
  TRAINER_TIMER->CCER = TIM_CCER_CC4E | (positive ? TIM_CCER_CC4P : 0);
}

// Trainer capture, PC8, Timer 3 channel 3
void init_trainer_capture()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  GPIO_InitStructure.GPIO_Pin = TRAINER_GPIO_PIN_IN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(TRAINER_GPIO, &GPIO_InitStructure);
  GPIO_PinAFConfig(TRAINER_GPIO, TRAINER_GPIO_PinSource_IN, TRAINER_GPIO_AF);

  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN ;
  TRAINER_TIMER->ARR = 0xFFFF ;
  TRAINER_TIMER->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 2000000 - 1 ;               // 0.5uS
  TRAINER_TIMER->CR2 = 0 ;
  TRAINER_TIMER->CCMR2 = TIM_CCMR2_IC3F_0 | TIM_CCMR2_IC3F_1 | TIM_CCMR2_CC3S_0 ;
  TRAINER_TIMER->CCER = TIM_CCER_CC3E ;
  TRAINER_TIMER->SR &= ~TIM_SR_CC3IF & ~TIM_SR_CC2IF & ~TIM_SR_UIF ;  // Clear flags
  TRAINER_TIMER->DIER |= TIM_DIER_CC3IE ;
  TRAINER_TIMER->CR1 = TIM_CR1_CEN ;
  NVIC_EnableIRQ(TRAINER_TIMER_IRQn) ;
  NVIC_SetPriority(TRAINER_TIMER_IRQn, 7);
}

void stop_trainer_capture()
{
  TRAINER_TIMER->CR1 &= ~TIM_CR1_CEN ;                          // Stop counter
  TRAINER_TIMER->DIER = 0;                      // Stop Interrupt
  NVIC_DisableIRQ(TRAINER_TIMER_IRQn) ;                         // Stop Interrupt
}

#if !defined(SIMU)
extern "C" void TIM3_IRQHandler()
{
  uint16_t capture = 0;
  bool doCapture = false ;

  // What mode? in or out?
  if ( (TRAINER_TIMER->DIER & TIM_DIER_CC3IE ) && ( TRAINER_TIMER->SR & TIM_SR_CC3IF ) ) {
    // capture mode on trainer jack
    capture = TRAINER_TIMER->CCR3 ;
    doCapture = true;
  }

  if ( ( TRAINER_TIMER->DIER & TIM_DIER_CC2IE ) && ( TRAINER_TIMER->SR & TIM_SR_CC2IF ) ) {
    // capture mode on heartbeat pin (external module)
    capture = TRAINER_TIMER->CCR2 ;
    doCapture = true ;
  }

  if (doCapture) {
    captureTrainerPulses(capture);
  }

  // PPM out compare interrupt
  if ( ( TRAINER_TIMER->DIER & TIM_DIER_CC1IE ) && ( TRAINER_TIMER->SR & TIM_SR_CC1IF ) ) {
    // compare interrupt
    TRAINER_TIMER->DIER &= ~TIM_DIER_CC1IE ;         // stop this interrupt
    TRAINER_TIMER->SR &= ~TIM_SR_CC1IF ;                             // Clear flag

    setupTrainerPulses() ;

    trainerPulsesData.ppm.ptr = trainerPulsesData.ppm.pulses;
    TRAINER_TIMER->DIER |= TIM_DIER_UDE ;
    TRAINER_TIMER->SR &= ~TIM_SR_UIF ;                                       // Clear this flag
    TRAINER_TIMER->DIER |= TIM_DIER_UIE ;                            // Enable this interrupt
  }

  // PPM out update interrupt
  if ( (TRAINER_TIMER->DIER & TIM_DIER_UIE) && ( TRAINER_TIMER->SR & TIM_SR_UIF ) ) {
    TRAINER_TIMER->SR &= ~TIM_SR_UIF ;                               // Clear flag
    TRAINER_TIMER->ARR = *trainerPulsesData.ppm.ptr++ ;
    if ( *trainerPulsesData.ppm.ptr == 0 ) {
      TRAINER_TIMER->SR &= ~TIM_SR_CC1IF ;                     // Clear this flag
      TRAINER_TIMER->DIER |= TIM_DIER_CC1IE ;  // Enable this interrupt
    }
  }
}
#endif
