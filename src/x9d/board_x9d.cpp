/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
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

#include "../open9x.h"

uint16_t Analog_values[NUMBER_ANALOG] ;
uint8_t temperature = 0;          // Raw temp reading
uint8_t maxTemperature = 0 ;       // Raw temp reading
volatile uint32_t Tenms ; // TODO to remove everywhere / use a #define

// TODO why not here the Peri1_frequency ... (to avoid modifications in system_stm32f2xx.c)
uint32_t Master_frequency ;
extern uint32_t Peri1_frequency ;
extern uint32_t Peri2_frequency ;

void keysInit();
void pwrInit();
void eepromInit(); // TODO check it's not in another include
void delaysInit();

uint8_t getTemperature()
{
  return temperature + g_eeGeneral.temperatureCalib;
}

// TODO change this function!
void read_9_adc()
{
}

void start_ppm_capture()
{
}

void usbBootloader()
{
}

void usbMassStorage()
{
}

// Starts TIMER at 200Hz, 5mS period
void init5msTimer()
{
  // Timer14
  RCC->APB1ENR |= RCC_APB1ENR_TIM14EN ;           // Enable clock
  TIM14->ARR = 4999 ;     // 5mS
  TIM14->PSC = Peri1_frequency / 1000000 ;                // 1uS from 30MHz
  TIM14->CCER = 0 ;
  TIM14->CCMR1 = 0 ;
  TIM14->EGR = 0 ;
  TIM14->CR1 = 5 ;
  TIM14->DIER |= 1 ;
  NVIC_EnableIRQ(TIM8_TRG_COM_TIM14_IRQn) ;
}

void stop5msTimer( void )
{
  TIM14->CR1 = 0 ;        // stop timer
  NVIC_DisableIRQ(TIM8_TRG_COM_TIM14_IRQn) ;
  RCC->APB1ENR &= ~RCC_APB1ENR_TIM14EN ;          // Disable clock
}

// TODO use the same than board_sky9x.cpp
void interrupt5ms()
{
  static uint32_t pre_scale ;       // Used to get 10 Hz counter

  // HAPTIC_HEARTBEAT();
  // AUDIO_HEARTBEAT();

  if ( ++pre_scale >= 2 ) {
    Tenms |= 1 ;                    // 10 mS has passed
    /* if ( Buzzer_count ) {
      if ( --Buzzer_count == 0 )
        buzzer_off() ;
    } */
    pre_scale = 0 ;
    per10ms();
  }
}

extern "C" void TIM8_TRG_COM_TIM14_IRQHandler()
{
  TIM14->SR &= ~TIM_SR_UIF ;
  interrupt5ms() ;
}

#if !defined(SIMU)
void boardInit()
{
  pwrInit();
  keysInit();
  init5msTimer();

  __enable_irq() ;

  eepromInit();
  delaysInit();
}
#endif


