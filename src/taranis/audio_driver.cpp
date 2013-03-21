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

bool dacIdle = true;
uint32_t currentFrequency = 0;

uint32_t getFrequency()
{
  return currentFrequency;
}

void setFrequency(uint32_t frequency)
{
  if (currentFrequency != frequency) {
    currentFrequency = frequency;

    register uint32_t timer = (PERI1_FREQUENCY * TIMER_MULT_APB1) / frequency - 1 ;         // MCK/8 and 100 000 Hz

    TIM6->CR1 &= ~TIM_CR1_CEN ;
    TIM6->CNT = 0 ;
    TIM6->ARR = limit<uint32_t>(2, timer, 65535) ;
    TIM6->CR1 |= TIM_CR1_CEN ;
  }
}

// Start TIMER6 at 100000Hz, used for DAC trigger
void dacTimerInit()
{
  // Now for timer 6
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN ;            // Enable clock

  TIM6->PSC = 0 ;                                                                                                 // Max speed
  TIM6->ARR = (PERI1_FREQUENCY * TIMER_MULT_APB1) / 100000 - 1 ;        // 10 uS, 100 kHz
  TIM6->CR2 = 0 ;
  TIM6->CR2 = 0x20 ;
  TIM6->CR1 = TIM_CR1_CEN ;
}

// Configure DAC0 (or DAC1 for REVA)
// Not sure why PB14 has not be allocated to the DAC, although it is an EXTRA function
// So maybe it is automatically done
void dacInit()
{
  dacTimerInit();

  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN ;           // Enable portA clock
  configure_pins( 0x0010, PIN_ANALOG | PIN_PORTA ) ;
  RCC->APB1ENR |= RCC_APB1ENR_DACEN ;                             // Enable clock
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN ;                    // Enable DMA1 clock

  // Chan 7, 16-bit wide, Medium priority, memory increments
  DMA1_Stream5->CR &= ~DMA_SxCR_EN ;              // Disable DMA
  DMA1->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5 ; // Write ones to clear bits
  DMA1_Stream5->CR = DMA_SxCR_CHSEL_0 | DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_2 | DMA_SxCR_PL_0 |
                                                                             DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_CIRC ;
  DMA1_Stream5->PAR = CONVERT_PTR(&DAC->DHR12R1);
  DMA1_Stream5->M0AR = CONVERT_PTR(Sine_values);
  DMA1_Stream5->FCR = 0x05 ; //DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0 ;
  DMA1_Stream5->NDTR = 100 ;

  DAC->DHR12R1 = 2010 ;
  DAC->SR = DAC_SR_DMAUDR1 ;              // Write 1 to clear flag
  DAC->CR = DAC_CR_TEN1 | DAC_CR_EN1 ;                    // Enable DAC
  NVIC_SetPriority( DMA1_Stream5_IRQn, 2 ) ; // High priority interrupt
  NVIC_EnableIRQ(TIM6_DAC_IRQn) ;
  NVIC_EnableIRQ(DMA1_Stream5_IRQn) ;
}

// Sound routines
void audioInit()
{
  dacInit() ;
}

void audioEnd()
{
  DAC->CR = 0 ;
  TIM6->CR1 = 0 ;
  // Also need to turn off any possible interrupts
  NVIC_DisableIRQ(TIM6_DAC_IRQn) ;
  NVIC_DisableIRQ(DMA1_Stream5_IRQn) ;
}

#ifndef SIMU
extern "C" void TIM6_DAC_IRQHandler()
{
  DAC->CR &= ~DAC_CR_DMAEN1 ;                     // Stop DMA requests
  DAC->CR &= ~DAC_CR_DMAUDRIE1 ;  // Stop underrun interrupt
  DAC->SR = DAC_SR_DMAUDR1 ;                      // Write 1 to clear flag
}

extern "C" void DMA1_Stream5_IRQHandler()
{
  DMA1_Stream5->CR &= ~DMA_SxCR_TCIE ;            // Stop interrupt
  DMA1->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5 ; // Write ones to clear flags
  DMA1_Stream5->CR &= ~DMA_SxCR_EN ;                              // Disable DMA channel
  if (nextAudioData) {
    DMA1_Stream5->M0AR = CONVERT_PTR(nextAudioData);
    DMA1_Stream5->NDTR = nextAudioSize*2;
    DMA1->HIFCR = DMA_HIFCR_CTCIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CFEIF5 ; // Write ones to clear bits
    DMA1_Stream5->CR |= DMA_SxCR_EN | DMA_SxCR_TCIE ;       // Enable DMA channel
    DAC->SR = DAC_SR_DMAUDR1;                      // Write 1 to clear flag
    nextAudioData = NULL;
  }
  else {
    dacIdle = true;
  }
}
#endif

#if 0
static void Audio_GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* EnableI2C GPIO clocks */
    RCC_AHB1PeriphClockCmd(CODEC_I2S_GPIO_CLOCK, ENABLE);//GPIOA C

    /* CODEC_I2S pins configuration: WS, SCK and SD pins -----------------------------*/
    GPIO_InitStructure.GPIO_Pin = CODEC_I2S_SCK_PIN | CODEC_I2S_SD_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(CODEC_I2S_GPIO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = CODEC_I2S_WS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(CODEC_I2S_WS_GPIO, &GPIO_InitStructure);

    /* Connect pins to I2S peripheral  */
    GPIO_PinAFConfig(CODEC_I2S_WS_GPIO, CODEC_I2S_WS_PINSRC, CODEC_I2S_GPIO_AF);
    GPIO_PinAFConfig(CODEC_I2S_GPIO, CODEC_I2S_SCK_PINSRC, CODEC_I2S_GPIO_AF);
    GPIO_PinAFConfig(CODEC_I2S_GPIO, CODEC_I2S_SD_PINSRC, CODEC_I2S_GPIO_AF);

#ifdef CODEC_MCLK_ENABLED
    /* CODEC_I2S pins configuration: MCK pin */
    GPIO_InitStructure.GPIO_Pin = CODEC_I2S_MCK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(CODEC_I2S_MCK_GPIO, &GPIO_InitStructure);
    /* Connect pins to I2S peripheral  */
    GPIO_PinAFConfig(CODEC_I2S_MCK_GPIO, CODEC_I2S_MCK_PINSRC, CODEC_I2S_GPIO_AF);
#endif /* CODEC_MCLK_ENABLED */

/*CODEC_I2S_MUTE & CODEC_I2S_FLT GPIO CONFIG*/
    GPIO_InitStructure.GPIO_Pin = CODEC_I2S_MUTE;// | CODEC_I2S_FLT;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(CODEC_I2S_GPIO, &GPIO_InitStructure);
    GPIO_SetBits(CODEC_I2S_GPIO,CODEC_I2S_MUTE);
   // GPIO_SetBits(CODEC_I2S_CTL_GPIO,CODEC_I2S_FLT);
}

void audioInit()
{
    I2S_InitTypeDef I2S_InitStructure;

    Audio_GPIO_Init();

    /*CONFIG the I2S_RCC ,MUST before enabling the I2S APB clock*/
    //PLLI2SN 302,PLLI2SR 2,I2SDIV 53,I2SODD 1,FS 44.1KHZ,16bit,Error 0.0011%
    RCC_PLLI2SConfig(302,2);
    RCC_PLLI2SCmd(ENABLE);
    RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S);

    /* Enable the CODEC_I2S peripheral clock */
    RCC_APB1PeriphClockCmd(CODEC_I2S_CLK, ENABLE);
    /* CODEC_I2S peripheral configuration */
    SPI_I2S_DeInit(CODEC_I2S);
    I2S_InitStructure.I2S_AudioFreq = AudioFreq;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;//clk 0 when idle state
    I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
#ifdef CODEC_MCLK_ENABLED
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
#elif defined(CODEC_MCLK_DISABLED)
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
#else
    #error "No selection for the MCLK output has been defined !"
#endif /* CODEC_MCLK_ENABLED */

    /* Initialize the I2S peripheral with the structure above */
    I2S_Init(CODEC_I2S, &I2S_InitStructure);
    I2S_Cmd(CODEC_I2S, ENABLE);
    //interrupt
#ifdef CODEC_USE_INT
    SPI_I2S_ITConfig(CODEC_I2S,SPI_I2S_IT_TXE,ENABLE);
#elif defined(CODEC_USE_DMA)
#error "DMA is not initialized"
#endif
}
#endif
