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

#include "../../opentx.h"

void setupPulses(unsigned int port);
void setupPulsesPPM(unsigned int port);
void setupPulsesPXX(unsigned int port);

static void intmodulePxxStart( void ) ;
static void intmodulePxxStop( void ) ;
static void extmodulePxxStart( void ) ;
static void extmodulePxxStop( void ) ;
#if defined(DSM2)
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

#if defined(DSM2)
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
}

void disable_ppm(uint32_t port)
{
  if (port == EXTERNAL_MODULE) {
    extmodulePpmStop();
  }
}

void set_external_ppm_parameters(uint32_t idleTime, uint32_t delay, uint32_t positive)
{
  EXTMODULE_TIMER->CCR2 = idleTime;
  EXTMODULE_TIMER->CCR1 = delay;
  EXTMODULE_TIMER->CCER = TIM_CCER_CC1E | (positive ? 0 : TIM_CCER_CC1P);
}

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
  GPIO_InitStructure.GPIO_Pin = INTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(INTMODULE_TX_GPIO, &GPIO_InitStructure);
  GPIO_SetBits(INTMODULE_TX_GPIO, INTMODULE_TX_GPIO_PIN) ; // Set high

  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  INTMODULE_TIMER->ARR = 36000 ;             // 18mS
  INTMODULE_TIMER->CCR2 = 32000 ;            // Update time
  INTMODULE_TIMER->PSC = INTMODULE_TIMER_FREQ / 2000000 - 1 ; // 0.5uS (2Mhz)

  INTMODULE_TIMER->CCER = TIM_CCER_CC3E ;

  INTMODULE_TIMER->CCMR2 = 0 ;
  INTMODULE_TIMER->EGR = 1 ;                                                         // Restart

  INTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0 ;                     // Toggle CC1 o/p
  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  INTMODULE_TIMER->CR1 |= TIM_CR1_CEN ;
  NVIC_EnableIRQ(INTMODULE_TIMER_IRQn) ;
  NVIC_SetPriority(INTMODULE_TIMER_IRQn, 7);
}

static void intmoduleNoneStop()
{
  NVIC_DisableIRQ(INTMODULE_TIMER_IRQn) ;
  INTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE ;
  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
}

#if !defined(SIMU)
extern "C" void DMA2_Stream7_IRQHandler(void)
{
  if(DMA_GetITStatus(INTMODULE_DMA_STREAM, DMA_IT_TCIF7)) {
    // TODO we could send the 8 next channels here (when needed)
    DMA_ClearITPendingBit(INTMODULE_DMA_STREAM, DMA_IT_TCIF7);
  }
}

extern "C" void TIM1_CC_IRQHandler()
{
  INTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE;       // stop this interrupt
  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF;           // clear flag

  setupPulses(INTERNAL_MODULE);

  if (s_current_protocol[INTERNAL_MODULE] == PROTO_PXX) {
    DMA_InitTypeDef DMA_InitStructure;
    DMA_DeInit(INTMODULE_DMA_STREAM);
    DMA_InitStructure.DMA_Channel = INTMODULE_DMA_CHANNEL;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&INTMODULE_USART->DR);
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)modulePulsesData[INTERNAL_MODULE].pxx.pulses;
    DMA_InitStructure.DMA_BufferSize = (uint8_t *)modulePulsesData[INTERNAL_MODULE].pxx.ptr - (uint8_t *)modulePulsesData[INTERNAL_MODULE].pxx.pulses;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(INTMODULE_DMA_STREAM, &DMA_InitStructure);

    DMA_Cmd(INTMODULE_DMA_STREAM, ENABLE);
    USART_DMACmd(INTMODULE_USART, USART_DMAReq_Tx, ENABLE);
    // DMA_ITConfig(INTMODULE_DMA_STREAM, DMA_IT_TC, ENABLE);
  }

  INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE;
}
#endif

static void extmoduleNoneStart()
{
  if (!IS_TRAINER_EXTERNAL_MODULE()) {
    EXTERNAL_MODULE_OFF();
  }

  GPIO_PinAFConfig(EXTMODULE_PPM_GPIO, EXTMODULE_PPM_GPIO_PinSource, 0); // TODO : check if needed
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_PPM_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_PPM_GPIO, &GPIO_InitStructure);
  GPIO_SetBits(EXTMODULE_PPM_GPIO, EXTMODULE_PPM_GPIO_PIN) ; // Set high

  TIM_DeInit(EXTMODULE_TIMER);

  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Prescaler = EXTMODULE_TIMER_FREQ / 2000000 - 1; // 0.5uS (2Mhz)
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 36000;             // 18mS
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(EXTMODULE_TIMER, &TIM_TimeBaseStructure);

  // set CC2 compare to fire slightly before the Update
  TIM_SetCompare2(EXTMODULE_TIMER, 32000);    // 16ms

  // enable CC2 interrupt
  TIM_ClearFlag(EXTMODULE_TIMER, TIM_FLAG_CC2);
  TIM_ITConfig(EXTMODULE_TIMER, TIM_IT_CC2, ENABLE);

  // start timer
  TIM_Cmd(EXTMODULE_TIMER, ENABLE);

  // enable interrupt and set it's priority
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn) ;
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}

static void extmoduleNoneStop()
{
  NVIC_DisableIRQ(EXTMODULE_TIMER_IRQn) ;
  TIM_DeInit(EXTMODULE_TIMER);
}

static void extmoduleCrossfireStart()
{
  EXTERNAL_MODULE_ON();

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_PPM_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_PPM_GPIO, &GPIO_InitStructure);
  GPIO_SetBits(EXTMODULE_PPM_GPIO, EXTMODULE_PPM_GPIO_PIN); // Set high

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  EXTMODULE_TIMER->ARR = 5000 ;             // 2.5mS
  EXTMODULE_TIMER->CCR2 = 32000 ;            // Update time
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1 ; // 0.5uS (2Mhz)

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

void intmodulePxxStart()
{
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  INTERNAL_MODULE_ON();

  setupPulsesPXX(INTERNAL_MODULE) ; // TODO not here!

  NVIC_InitStructure.NVIC_IRQChannel = INTMODULE_DMA_STREAM_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; /* Not used as 4 bits are used for the pre-emption priority. */;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

#if 0
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = TIM_RF_PRIO;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; /* Not used as 4 bits are used for the pre-emption priority. */;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init( &NVIC_InitStructure );

  // open heartbit ---------------------
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Connect EXTI Line12 to PD12 pin */
  EXTI_InitTypeDef EXTI_InitStructure;
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource12);
  EXTI_InitStructure.EXTI_Line = EXTI_Line12;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;///////
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  EXTI15_10_Interrupt_Handler = EXTI15_10_IRQHandler_pxx;
#endif

  // TX Pin
  GPIO_PinAFConfig(INTMODULE_TX_GPIO, INTMODULE_TX_GPIO_PinSource, INTMODULE_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = INTMODULE_TX_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(INTMODULE_TX_GPIO, &GPIO_InitStructure);

  // RX Pin
  GPIO_PinAFConfig(INTMODULE_RX_GPIO, INTMODULE_RX_GPIO_PinSource, INTMODULE_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = INTMODULE_RX_GPIO_PIN;
  GPIO_Init(INTMODULE_RX_GPIO, &GPIO_InitStructure);

  // UART config
  USART_DeInit(INTMODULE_USART);
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(INTMODULE_USART, &USART_InitStructure);
  USART_Cmd(INTMODULE_USART, ENABLE);

  // open DMA------------------------------
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);	// Enable DMA1 clock

  // Timer
  INTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  INTMODULE_TIMER->ARR = 18000 ;                     // 9mS
  INTMODULE_TIMER->CCR2 = 15000 ;            // Update time
  INTMODULE_TIMER->PSC = INTMODULE_TIMER_FREQ / 2000000 - 1 ; // 0.5uS (2Mhz)
  INTMODULE_TIMER->CCER = TIM_CCER_CC3E ;
  INTMODULE_TIMER->CCER = TIM_CCER_CC3E ;

  INTMODULE_TIMER->CCMR2 = 0 ;
  INTMODULE_TIMER->EGR = 1 ;                                                         // Restart

  INTMODULE_TIMER->CCMR2 = TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_0 ;                     // Toggle CC1 o/p
  INTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  INTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  INTMODULE_TIMER->CR1 |= TIM_CR1_CEN ;
  NVIC_EnableIRQ(INTMODULE_TIMER_IRQn) ;
  NVIC_SetPriority(INTMODULE_TIMER_IRQn, 7);
}

static void intmodulePxxStop()
{
  INTERNAL_MODULE_OFF();
}

void extmodulePxxStart()
{
  EXTERNAL_MODULE_ON();

  setupPulsesPXX(EXTERNAL_MODULE);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_PinAFConfig(EXTMODULE_PPM_GPIO, EXTMODULE_PPM_GPIO_PinSource, EXTMODULE_PPM_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_PPM_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_PPM_GPIO, &GPIO_InitStructure);

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  EXTMODULE_TIMER->ARR = 18000 ;                     // 9mS
  EXTMODULE_TIMER->CCR2 = 15000 ;            // Update time
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1 ; // 0.5uS (2Mhz)
  EXTMODULE_TIMER->CCER = TIM_CCER_CC1NE ;
  EXTMODULE_TIMER->CR2 = TIM_CR2_OIS1 ;              // O/P idle high
  EXTMODULE_TIMER->BDTR = TIM_BDTR_MOE ;             // Enable outputs
  EXTMODULE_TIMER->CCR1 = modulePulsesData[EXTERNAL_MODULE].pxx.pulses[0];
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0 ;                     // Force O/P high
  EXTMODULE_TIMER->EGR = 1 ; // Restart

  EXTMODULE_TIMER->DIER |= TIM_DIER_CC1DE ;                        // Enable DMA on CC1 match
  EXTMODULE_TIMER->DCR = 13;

  // Enable the DMA channel here, DMA1 stream 5, channel 3 (TIM2_CH1)
  DMA1_Stream5->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  DMA1->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5 ; // Write ones to clear bits
  DMA1_Stream5->CR = DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_1 | DMA_SxCR_PL_0 | DMA_SxCR_MSIZE_0
                                                         | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_PFCTRL ;
  DMA1_Stream5->PAR = CONVERT_PTR_UINT(&EXTMODULE_TIMER->DMAR);
  DMA1_Stream5->M0AR = CONVERT_PTR_UINT(&modulePulsesData[EXTERNAL_MODULE].pxx.pulses[1]);
//      DMA1_Stream5->FCR = 0x05 ; //DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 ;
//      DMA1_Stream5->NDTR = 100 ;
  DMA1_Stream5->CR |= DMA_SxCR_EN ;               // Enable DMA

  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0 ;                     // Toggle CC1 o/p
  EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN ;
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn);
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}

static void extmodulePxxStop()
{
  DMA1_Stream5->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  NVIC_DisableIRQ(EXTMODULE_TIMER_IRQn) ;
  EXTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE ;
  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  if (!IS_TRAINER_EXTERNAL_MODULE()) {
    EXTERNAL_MODULE_OFF();
  }
}

#if defined(DSM2)
static void extmoduleDsm2Start()
{
  EXTERNAL_MODULE_ON();

  setupPulsesDSM2(EXTERNAL_MODULE);

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_PinAFConfig(EXTMODULE_PPM_GPIO, EXTMODULE_PPM_GPIO_PinSource, EXTMODULE_PPM_GPIO_AF);
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_PPM_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_PPM_GPIO, &GPIO_InitStructure);

  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  EXTMODULE_TIMER->ARR = 44000 ;                     // 22mS
  EXTMODULE_TIMER->CCR2 = 40000 ;            // Update time
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1 ; // 0.5uS (2Mhz)
  EXTMODULE_TIMER->CCER = TIM_CCER_CC1NE  | TIM_CCER_CC1NP ;
  EXTMODULE_TIMER->CR2 = TIM_CR2_OIS1 ;                      // O/P idle high
  EXTMODULE_TIMER->BDTR = TIM_BDTR_MOE ;             // Enable outputs
  EXTMODULE_TIMER->CCR1 = modulePulsesData[EXTERNAL_MODULE].dsm2.pulses[0];
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0 ;                     // Force O/P high
  EXTMODULE_TIMER->EGR = 1 ;                                                         // Restart

  EXTMODULE_TIMER->DIER |= TIM_DIER_CC1DE ;          // Enable DMA on CC1 match
  EXTMODULE_TIMER->DCR = 13 ;                                                                // DMA to CC1

  // Enable the DMA channel here, DMA1 stream 5, channel 3 (TIM2_CH1)
  DMA1_Stream5->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  DMA1->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5 ; // Write ones to clear bits
  DMA1_Stream5->CR = DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_1 | DMA_SxCR_PL_0 | DMA_SxCR_MSIZE_0
                                                         | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_PFCTRL ;
  DMA1_Stream5->PAR = CONVERT_PTR_UINT(&EXTMODULE_TIMER->DMAR);
  DMA1_Stream5->M0AR = CONVERT_PTR_UINT(&modulePulsesData[EXTERNAL_MODULE].dsm2.pulses[1]);
//      DMA1_Stream5->FCR = 0x05 ; //DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 ;
//      DMA1_Stream5->NDTR = 100 ;
  DMA1_Stream5->CR |= DMA_SxCR_EN ;               // Enable DMA

  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0 ;                     // Toggle CC1 o/p
  EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN ;
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn) ;
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}

static void extmoduleDsm2Stop()
{
  DMA1_Stream5->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  NVIC_DisableIRQ(EXTMODULE_TIMER_IRQn) ;
  EXTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE ;
  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  if (!IS_TRAINER_EXTERNAL_MODULE()) {
    EXTERNAL_MODULE_OFF();
  }
}
#endif

static void extmodulePpmStart()
{
  EXTERNAL_MODULE_ON();

  GPIO_PinAFConfig(EXTMODULE_PPM_GPIO, EXTMODULE_PPM_GPIO_PinSource, EXTMODULE_PPM_GPIO_AF);
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = EXTMODULE_PPM_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(EXTMODULE_PPM_GPIO, &GPIO_InitStructure);

  // PPM generation principle:
  //
  // Hardware timer in PWM mode is used for PPM generation
  // Output is OFF if CNT<CCR1(delay) and ON if bigger
  // CCR1 register defines duration of pulse length and is constant
  // AAR register defines duration of each pulse, it is 
  // updated after every pulse in Update interrupt handler.
  // CCR2 register defines duration of no pulses (time between two pulse trains)
  // it is calculated every round to have PPM period constant.
  // CC2 interrupt is then used to setup new PPM values for the
  // next PPM pulses train.
  // stop timer
  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;

  // setupPulsesPPM() is also configuring registers,
  // so it has to be called after the peripheral is enabled
  setupPulsesPPM(EXTERNAL_MODULE) ;

  EXTMODULE_TIMER->ARR = *modulePulsesData[EXTERNAL_MODULE].ppm.ptr++ ;
  EXTMODULE_TIMER->PSC = EXTMODULE_TIMER_FREQ / 2000000 - 1 ; // 0.5uS (2Mhz)

  // CH1 PWM mode 1, CH1 out is OFF if CNT<CCR1(delay) and ON if bigger (up to AAR,which is pulse length)
  EXTMODULE_TIMER->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC2PE;        // PWM mode 1

  // Reloads register vales now
  EXTMODULE_TIMER->EGR = 1 ;

  // Update DMA request enabled. WHY?????
  EXTMODULE_TIMER->DIER = TIM_DIER_UDE ;

  // clear interrupt flags and enable Update and
  // CC2 interrupts (this one is enabled now to trigger pulse setup)
  EXTMODULE_TIMER->SR &= ~TIM_SR_UIF ;                               // Clear flag
  EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;
  EXTMODULE_TIMER->DIER |= TIM_DIER_UIE ;

  // enable timer and global interrupts
  EXTMODULE_TIMER->CR1 |= TIM_CR1_CEN ;
  NVIC_EnableIRQ(EXTMODULE_TIMER_IRQn) ;
  NVIC_SetPriority(EXTMODULE_TIMER_IRQn, 7);
}

static void extmodulePpmStop()
{
  NVIC_DisableIRQ(EXTMODULE_TIMER_IRQn) ;
  EXTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE & ~TIM_DIER_UIE ;
  EXTMODULE_TIMER->CR1 &= ~TIM_CR1_CEN ;
  EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF & ~TIM_SR_UIF;            // Clear all flags
  if (!IS_TRAINER_EXTERNAL_MODULE()) {
    EXTERNAL_MODULE_OFF();
  }
}

extern "C" void TIM2_IRQHandler()
{
  //determine if its CC or UP interrupt
  uint16_t sr = EXTMODULE_TIMER->SR;
  uint16_t dier = EXTMODULE_TIMER->DIER;

  if (sr & TIM_SR_CC2IF && dier & TIM_DIER_CC2IE) {    // Capture/Compare 2 interrupt enabled and fired
    EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;             // Clear flag

    setupPulses(EXTERNAL_MODULE);

    if (s_current_protocol[EXTERNAL_MODULE] == PROTO_PXX) {
      DMA1_Stream5->CR &= ~DMA_SxCR_EN ;              // Disable DMA
      DMA1->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5 ; // Write ones to clear bits
      DMA1_Stream5->M0AR = CONVERT_PTR_UINT(&modulePulsesData[EXTERNAL_MODULE].pxx.pulses[1]);
      DMA1_Stream5->CR |= DMA_SxCR_EN ;               // Enable DMA
      EXTMODULE_TIMER->CCR1 = modulePulsesData[EXTERNAL_MODULE].pxx.pulses[0];
    }
#if defined(DSM2)
    else if (s_current_protocol[EXTERNAL_MODULE] >= PROTO_DSM2_LP45 && s_current_protocol[EXTERNAL_MODULE] <= PROTO_DSM2_DSMX) {
      DMA1_Stream5->CR &= ~DMA_SxCR_EN ;              // Disable DMA
      DMA1->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5 ; // Write ones to clear bits
      DMA1_Stream5->M0AR = CONVERT_PTR_UINT(&modulePulsesData[EXTERNAL_MODULE].dsm2.pulses[1]);
      DMA1_Stream5->CR |= DMA_SxCR_EN ;               // Enable DMA
      EXTMODULE_TIMER->CCR1 = modulePulsesData[EXTERNAL_MODULE].dsm2.pulses[0];
    }
#endif
    else if (s_current_protocol[EXTERNAL_MODULE] == PROTO_PPM) {
      // Stop CC2 interrupt, the pulse train period ended
      // the new PPM values were set, we begin new pulse train
      // generation with Update interrupt
      EXTMODULE_TIMER->DIER &= ~TIM_DIER_CC2IE ; // stop CC2 interrupt
      EXTMODULE_TIMER->DIER |= TIM_DIER_UDE ;
      EXTMODULE_TIMER->SR &= ~TIM_SR_UIF ;     // Clear Update interrupt flag
      EXTMODULE_TIMER->DIER |= TIM_DIER_UIE ;  // Enable Update interrupt
    }
  }

  if (sr & TIM_SR_UIF && dier & TIM_DIER_UIE) {      // Update interrupt enabled and fired
    EXTMODULE_TIMER->SR &= ~TIM_SR_UIF ;                               // Clear flag
    if (s_current_protocol[EXTERNAL_MODULE] == PROTO_PPM) {
      EXTMODULE_TIMER->ARR = *modulePulsesData[EXTERNAL_MODULE].ppm.ptr++ ;
      if (*modulePulsesData[EXTERNAL_MODULE].ppm.ptr == 0) {
        // we reached the end of PPM pulses
        // enable CC2 interrupt (which comes before Update in any case) 
        // to start new PPM cycle and setup pulses
        EXTMODULE_TIMER->SR &= ~TIM_SR_CC2IF ;    // Clear CC1 flag
        EXTMODULE_TIMER->DIER |= TIM_DIER_CC2IE ;  // Enable CC1 interrupt
      }
    }
  }
}
