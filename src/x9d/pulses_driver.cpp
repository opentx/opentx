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

// Robert@FrSky code
#if 0
#include "../x9d/hal.h"
#include "../x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/CMSIS/Device/ST/STM32F2xx/Include/Stm32f2xx.h"
#include "../x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/Stm32f2xx_dma.h"
#include "../x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/Stm32f2xx_spi.h"
#include "../x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/Misc.h"
#include "../x9d/STM32F2xx_StdPeriph_Lib_V1.1.0/Libraries/STM32F2xx_StdPeriph_Driver/inc/Stm32f2xx_tim.h"
#endif

// TODO not here!
void setupPulses();
void setupPulsesPPM();
void setupPulsesPXX();

uint16_t *ppmStreamPtr;
extern uint16_t ppmStream[20];
extern uint16_t pxxStream[400] ;

void init_pxx()
{
  // Timer1
  setupPulsesPXX() ; // TODO not here!

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN ;           // Enable portA clock
#if defined(REV3)
  configure_pins( 0x0100, PIN_PERIPHERAL | PIN_PORTA | PIN_PER_1 | PIN_OS25 | PIN_PUSHPULL ) ;
#else
  configure_pins( 0x0080, PIN_PERIPHERAL | PIN_PORTA | PIN_PER_1 | PIN_OS25 | PIN_PUSHPULL ) ;
#endif
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN ;            // Enable clock
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN ;                    // Enable DMA1 clock

  TIM1->CR1 &= ~TIM_CR1_CEN ;
  TIM1->ARR = 18000 ;                     // 9mS
  TIM1->CCR2 = 15000 ;            // Update time
  TIM1->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 2000000 - 1 ;               // 0.5uS from 30MHz
#if defined(REV3)
  TIM1->CCER = TIM_CCER_CC1E | TIM_CCER_CC1P ;
#else
  TIM1->CCER = TIM_CCER_CC1NE ;
#endif
  TIM1->CR2 = TIM_CR2_OIS1 ;                      // O/P idle high
  TIM1->BDTR = TIM_BDTR_MOE ;             // Enable outputs
  TIM1->CCR1 = pxxStream[0] ;
  TIM1->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_0 ;                     // Force O/P high
  TIM1->EGR = 1 ;                                                         // Restart

//      TIM1->SR &= ~TIM_SR_UIF ;                               // Clear flag
//      TIM1->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  TIM1->DIER |= TIM_DIER_CC1DE ;          // Enable DMA on CC1 match
  TIM1->DCR = 13 ;                                                                // DMA to CC1

//      TIM1->CR1 = TIM_CR1_OPM ;                               // Just run once
        // Enable the DMA channel here, DMA2 stream 1, channel 6
  DMA2_Stream1->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  DMA2->LIFCR = DMA_LIFCR_CTCIF1 | DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTEIF1 | DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CFEIF1 ; // Write ones to clear bits
  DMA2_Stream1->CR = DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_2 | DMA_SxCR_PL_0 | DMA_SxCR_MSIZE_0
                                                                                 | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_PFCTRL ;
  DMA2_Stream1->PAR = CONVERT_PTR(&TIM1->DMAR);
  DMA2_Stream1->M0AR = CONVERT_PTR(&pxxStream[1]);
//      DMA2_Stream1->FCR = 0x05 ; //DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 ;
//      DMA2_Stream1->NDTR = 100 ;
  DMA2_Stream1->CR |= DMA_SxCR_EN ;               // Enable DMA

  TIM1->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0 ;                     // Toggle CC1 o/p
  TIM1->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  TIM1->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  TIM1->CR1 |= TIM_CR1_CEN ;
  NVIC_EnableIRQ(TIM1_CC_IRQn) ;
}

void disable_pxx()
{
  NVIC_DisableIRQ(TIM1_CC_IRQn) ;
  TIM1->DIER &= ~TIM_DIER_CC2IE ;
  TIM1->CR1 &= ~TIM_CR1_CEN ;
}

// Robert@FrSky code
#if 0
static DMA_InitTypeDef DMA_InitStructure;

/**
* Configure PA7 AS SPI1_MOSI, transfer by DMA2 ch3/stream3
*/
void init_pxx()
{
  setupPulsesPXX(); // TODO later ... not here!

  // SPI1
  SPI_InitTypeDef SPI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN ;
  GPIO_InitStructure.GPIO_Pin = PIN_CPPM_OUT;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA, 7, GPIO_AF_SPI1);

  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Enable clock

  SPI_DeInit(SPI1);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; // 1us resolution
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;

  SPI_Init(SPI1, &SPI_InitStructure);
  SPI_CalculateCRC(SPI1, DISABLE);

  RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN ;
  DMA_DeInit(DMA_Stream_SPI1_TX);

  DMA_InitStructure.DMA_Channel = DMA_Channel_SPI1_TX;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (DWORD)(&(SPI1->DR));
  DMA_InitStructure.DMA_Memory0BaseAddr = (DWORD)pxxStream;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_BufferSize = sizeof(pxxStream)/2;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  DMA_Init(DMA_Stream_SPI1_TX, &DMA_InitStructure);
  DMA_ITConfig(DMA_Stream_SPI1_TX, DMA_IT_TC, ENABLE);
  DMA_Cmd(DMA_Stream_SPI1_TX, ENABLE);

  SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
  SPI_Cmd(SPI1, ENABLE);
}

void disable_pxx()
{
  NVIC_DisableIRQ(DMA2_Stream3_IRQn);
  DMA_ITConfig(DMA_Stream_SPI1_TX, DMA_IT_TC, DISABLE);
  SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, DISABLE);
}

extern "C" void DMA2_Stream3_IRQHandler()
{
  if (DMA_GetFlagStatus(DMA_Stream_SPI1_TX, DMA_IT_TCIF3)) {
    DMA_ClearFlag(DMA_Stream_SPI1_TX, DMA_IT_TCIF3);

    setupPulses() ;
    //memset(pxxStream, 0x5555, 5); //test pxx signal on PA.07

    DMA_InitStructure.DMA_Memory0BaseAddr = (DWORD)pxxStream;
    DMA_InitStructure.DMA_BufferSize = sizeof(pxxStream)/2;
    DMA_Init(DMA_Stream_SPI1_TX, &DMA_InitStructure);
    DMA_Cmd(DMA_Stream_SPI1_TX, ENABLE);

    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
  }
}
#endif

// PPM output
// Timer 1, channel 1 on PA8 for prototype
// Pin is AF1 function for timer 1
void init_main_ppm()
{
  // Timer1
  setupPulsesPPM() ;
  ppmStreamPtr = ppmStream ;

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN ;           // Enable portA clock
#if defined(REV3)
  configure_pins( 0x0100, PIN_PERIPHERAL | PIN_PORTA | PIN_PER_1 | PIN_OS25 | PIN_PUSHPULL ) ;
#else
  configure_pins( 0x0080, PIN_PERIPHERAL | PIN_PORTA | PIN_PER_1 | PIN_OS25 | PIN_PUSHPULL ) ;
#endif
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN ;            // Enable clock

  TIM1->ARR = *ppmStreamPtr++ ;
  TIM1->PSC = (PERI2_FREQUENCY * TIMER_MULT_APB2) / 2000000 - 1 ;               // 0.5uS from 30MHz
#if defined(REV3)
  TIM1->CCER = TIM_CCER_CC1E ;
#else
  TIM1->CCER = TIM_CCER_CC1NE | TIM_CCER_CC1NP ;
#endif
  TIM1->CCMR1 = TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC2PE ;                   // PWM mode 1
  TIM1->CCR1 = 600 ;              // 300 uS pulse
  TIM1->BDTR = TIM_BDTR_MOE ;
  TIM1->EGR = 1 ;
  TIM1->DIER = TIM_DIER_UDE ;

  TIM1->SR &= ~TIM_SR_UIF ;                               // Clear flag
  TIM1->SR &= ~TIM_SR_CC2IF ;                             // Clear flag
  TIM1->DIER |= TIM_DIER_CC2IE ;
  TIM1->DIER |= TIM_DIER_UIE ;

  TIM1->CR1 = TIM_CR1_CEN ;
  NVIC_EnableIRQ(TIM1_CC_IRQn) ;
  NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn) ;
}

void disable_main_ppm()
{
  NVIC_DisableIRQ(TIM1_CC_IRQn) ;
  NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn) ;
  TIM1->DIER &= ~TIM_DIER_CC2IE & ~TIM_DIER_UIE ;
  TIM1->CR1 &= ~TIM_CR1_CEN ;
}

extern "C" void TIM1_CC_IRQHandler()
{
  TIM1->DIER &= ~TIM_DIER_CC2IE ;         // stop this interrupt
  TIM1->SR &= ~TIM_SR_CC2IF ;                             // Clear flag

  setupPulses() ;

  if (s_current_protocol == PROTO_PXX) {
    DMA2_Stream1->CR &= ~DMA_SxCR_EN ;              // Disable DMA
    DMA2->LIFCR = DMA_LIFCR_CTCIF1 | DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTEIF1 | DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CFEIF1 ; // Write ones to clear bits
    DMA2_Stream1->M0AR = CONVERT_PTR(&pxxStream[1]);
    DMA2_Stream1->CR |= DMA_SxCR_EN ;               // Enable DMA
    TIM1->CCR1 = pxxStream[0] ;
    TIM1->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  }
  else {
    ppmStreamPtr = ppmStream ;

    TIM1->DIER |= TIM_DIER_UDE ;
    TIM1->SR &= ~TIM_SR_UIF ;                                       // Clear this flag
    TIM1->DIER |= TIM_DIER_UIE ;                            // Enable this interrupt
  }
}

extern "C" void TIM1_UP_TIM10_IRQHandler()
{
  TIM1->SR &= ~TIM_SR_UIF ;                               // Clear flag

  TIM1->ARR = *ppmStreamPtr++ ;
  if ( *ppmStreamPtr == 0 )
  {
    TIM1->SR &= ~TIM_SR_CC2IF ;                     // Clear this flag
    TIM1->DIER |= TIM_DIER_CC2IE ;  // Enable this interrupt
  }
}
