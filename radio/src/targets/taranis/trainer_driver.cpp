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

uint16_t * TrainerPulsePtr;
extern uint16_t ppmStream[NUM_MODULES+1][20];
extern Fifo<32> sbusFifo;

#define setupTrainerPulses() setupPulsesPPM(TRAINER_MODULE)

// Trainer PPM oputput PC9, Timer 3 channel 4, (Alternate Function 2)
void init_trainer_ppm()
{
  TrainerPulsePtr = ppmStream[TRAINER_MODULE];

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN ;           // Enable portC clock
  configure_pins( PIN_TR_PPM_OUT, PIN_PERIPHERAL | PIN_PORTC | PIN_PER_2 | PIN_OS25) ;
  configure_pins( PIN_TR_PPM_IN, PIN_PORTA | PIN_INPUT ) ;
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN ;            // Enable clock
  TIM3->CR1 &= ~TIM_CR1_CEN ;

  // setupTrainerPulses() is also configuring registers,
  // so it has to be called after the peripheral is enabled
  setupTrainerPulses() ;

  TIM3->ARR = *TrainerPulsePtr++ ;
  TIM3->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 2000000 - 1 ;               // 0.5uS
  TIM3->CCMR2 = TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4PE ;                   // PWM mode 1
  TIM3->BDTR = TIM_BDTR_MOE ;
  TIM3->EGR = 1 ;

  TIM3->SR &= ~TIM_SR_UIF ;                               // Clear flag
  TIM3->SR &= ~TIM_SR_CC1IF ;                             // Clear flag
  TIM3->DIER |= TIM_DIER_CC1IE ;
  TIM3->DIER |= TIM_DIER_UIE ;

  TIM3->CR1 = TIM_CR1_CEN ;
  NVIC_EnableIRQ(TIM3_IRQn) ;
  NVIC_SetPriority(TIM3_IRQn, 7);
}

// TODO - testing
void stop_trainer_ppm()
{
  configure_pins( PIN_TR_PPM_OUT, PIN_INPUT | PIN_PORTC ) ; // Pin as input
  TIM3->DIER = 0 ;                                      // Stop Interrupt
  TIM3->CR1 &= ~TIM_CR1_CEN ;                             // Stop counter
  NVIC_DisableIRQ(TIM3_IRQn) ;                         // Stop Interrupt
}

void set_trainer_ppm_parameters(uint32_t idleTime, uint32_t delay, uint32_t positive)
{
  TIM3->CCR1 = idleTime;
  TIM3->CCR4 = delay;
  TIM3->CCER = TIM_CCER_CC4E | (positive ? TIM_CCER_CC4P : 0);
}

// Trainer capture, PC8, Timer 3 channel 3
void init_trainer_capture()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = PIN_TR_PPM_IN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_TR_INOUT, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIO_TR_INOUT, GPIO_PinSource8, GPIO_AF_TIM3);

  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN ;
  TIM3->ARR = 0xFFFF ;
  TIM3->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 2000000 - 1 ;               // 0.5uS
  TIM3->CR2 = 0 ;
  TIM3->CCMR2 = TIM_CCMR2_IC3F_0 | TIM_CCMR2_IC3F_1 | TIM_CCMR2_CC3S_0 ;
  TIM3->CCER = TIM_CCER_CC3E ;
  TIM3->SR &= ~TIM_SR_CC3IF & ~TIM_SR_CC2IF & ~TIM_SR_UIF ;  // Clear flags
  TIM3->DIER |= TIM_DIER_CC3IE ;
  TIM3->CR1 = TIM_CR1_CEN ;
  NVIC_EnableIRQ(TIM3_IRQn) ;
  NVIC_SetPriority(TIM3_IRQn, 7);
}

void stop_trainer_capture()
{
  TIM3->CR1 &= ~TIM_CR1_CEN ;                          // Stop counter
  TIM3->DIER = 0;                      // Stop Interrupt
  NVIC_DisableIRQ(TIM3_IRQn) ;                         // Stop Interrupt
}

#if !defined(SIMU)
extern "C" void TIM3_IRQHandler()
{
  uint16_t capture = 0;
  static uint16_t lastCapt ;
  uint16_t val ;
  bool doCapture = false ;

  // What mode? in or out?
  if ( (TIM3->DIER & TIM_DIER_CC3IE ) && ( TIM3->SR & TIM_SR_CC3IF ) ) {
    // capture mode on trainer jack
    capture = TIM3->CCR3 ;
    doCapture = true;
  }

  if ( ( TIM3->DIER & TIM_DIER_CC2IE ) && ( TIM3->SR & TIM_SR_CC2IF ) ) {
    // capture mode on heartbeat pin (external module)
    capture = TIM3->CCR2 ;
    doCapture = true ;
  }

  if (doCapture) {
    val = (uint16_t)(capture - lastCapt) / 2 ;
    lastCapt = capture;

    // We process g_ppmInsright here to make servo movement as smooth as possible
    //    while under trainee control
    if (val>4000 && val<19000) { // G: Prioritize reset pulse. (Needed when less than 16 incoming pulses)
      ppmInState = 1; // triggered
    }
    else {
      if (ppmInState>0 && ppmInState<=16) {
        if (val>800 && val<2200) {
          ppmInValid = PPM_IN_VALID_TIMEOUT;
          g_ppmIns[ppmInState++ - 1] = (int16_t)(val - 1500)*(g_eeGeneral.PPM_Multiplier+10)/10; //+-500 != 512, but close enough.
        }
        else {
          ppmInState = 0; // not triggered
        }
      }
    }
  }

  // PPM out compare interrupt
  if ( ( TIM3->DIER & TIM_DIER_CC1IE ) && ( TIM3->SR & TIM_SR_CC1IF ) ) {
    // compare interrupt
    TIM3->DIER &= ~TIM_DIER_CC1IE ;         // stop this interrupt
    TIM3->SR &= ~TIM_SR_CC1IF ;                             // Clear flag

    setupTrainerPulses() ;

    TrainerPulsePtr = ppmStream[TRAINER_MODULE];
    TIM3->DIER |= TIM_DIER_UDE ;
    TIM3->SR &= ~TIM_SR_UIF ;                                       // Clear this flag
    TIM3->DIER |= TIM_DIER_UIE ;                            // Enable this interrupt
  }

  // PPM out update interrupt
  if ( (TIM3->DIER & TIM_DIER_UIE) && ( TIM3->SR & TIM_SR_UIF ) ) {
    TIM3->SR &= ~TIM_SR_UIF ;                               // Clear flag
    TIM3->ARR = *TrainerPulsePtr++ ;
    if ( *TrainerPulsePtr == 0 ) {
      TIM3->SR &= ~TIM_SR_CC1IF ;                     // Clear this flag
      TIM3->DIER |= TIM_DIER_CC1IE ;  // Enable this interrupt
    }
  }
}
#endif

void init_cppm_on_heartbeat_capture(void)
{
  EXTERNAL_MODULE_ON();

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN ;           // Enable portC clock
  configure_pins( 0x0080, PIN_PERIPHERAL | PIN_PORTC | PIN_PER_2 ) ;
  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN ;            // Enable clock

  TIM3->ARR = 0xFFFF ;
  TIM3->PSC = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 2000000 - 1 ;               // 0.5uS
  TIM3->CR2 = 0 ;
  TIM3->CCMR1 = TIM_CCMR1_IC2F_0 | TIM_CCMR1_IC2F_1 | TIM_CCMR1_CC2S_0 ;
  TIM3->CCER = TIM_CCER_CC2E ;
  TIM3->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  TIM3->DIER |= TIM_DIER_CC2IE ;
  TIM3->CR1 = TIM_CR1_CEN ;
  NVIC_SetPriority(TIM3_IRQn, 7);
  NVIC_EnableIRQ(TIM3_IRQn) ;
}

void stop_cppm_on_heartbeat_capture(void)
{
  TIM3->DIER = 0 ;
  TIM3->CR1 &= ~TIM_CR1_CEN ;                             // Stop counter
  NVIC_DisableIRQ(TIM3_IRQn) ;                            // Stop Interrupt

  if (!IS_PULSES_EXTERNAL_MODULE()) {
    EXTERNAL_MODULE_OFF();
  }
}

void init_sbus_on_heartbeat_capture()
{
  EXTERNAL_MODULE_ON();

  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

  USART_InitStructure.USART_BaudRate = 100000;
  USART_InitStructure.USART_WordLength = USART_WordLength_9b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_Even;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx;

  USART_Init(USART6, &USART_InitStructure);
  USART_Cmd(USART6, ENABLE);
  USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);

  NVIC_SetPriority(USART6_IRQn, 6);
  NVIC_EnableIRQ(USART6_IRQn);
}

void stop_sbus_on_heartbeat_capture(void)
{
  configure_pins( 0x0080, PIN_INPUT | PIN_PORTC ) ;
  NVIC_DisableIRQ(USART6_IRQn) ;

  if (!IS_PULSES_EXTERNAL_MODULE()) {
    EXTERNAL_MODULE_OFF();
  }
}

#if !defined(SIMU) && !defined(REV9E)
extern "C" void USART6_IRQHandler()
{
  uint32_t status;
  uint8_t data;

  status = USART6->SR;

  while (status & (USART_FLAG_RXNE | USART_FLAG_ERRORS)) {
    data = USART6->DR;

    if (!(status & USART_FLAG_ERRORS))
      sbusFifo.push(data);

    status = USART6->SR;
  }
}
#endif
