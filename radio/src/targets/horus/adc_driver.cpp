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

#define ADC_CS_HIGH()                  (ADC_SPI_GPIO->BSRRL = ADC_SPI_PIN_CS)
#define ADC_CS_LOW()                   (ADC_SPI_GPIO->BSRRH = ADC_SPI_PIN_CS)

#define RESETCMD                       0x4000
#define MANUAL_MODE                    0x1000 // manual mode channel 0

#define SAMPTIME                       2 // sample time = 28 cycles

uint16_t adcValues[NUMBER_ANALOG] __DMA;

static u16 SPIx_ReadWriteByte(uint16_t value)
{
  while(SPI_I2S_GetFlagStatus(ADC_SPI,SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(ADC_SPI,value);

  while(SPI_I2S_GetFlagStatus(ADC_SPI,SPI_I2S_FLAG_RXNE) ==RESET);
  return SPI_I2S_ReceiveData(ADC_SPI);
}

static void ADS7952_Init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;

  GPIO_InitStructure.GPIO_Pin = ADC_SPI_PIN_MISO|ADC_SPI_PIN_SCK|ADC_SPI_PIN_MOSI;
  GPIO_InitStructure.GPIO_Speed =GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(ADC_SPI_GPIO, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = ADC_SPI_PIN_CS;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(ADC_SPI_GPIO, &GPIO_InitStructure);

  GPIO_PinAFConfig(ADC_SPI_GPIO, ADC_SPI_PinSource_SCK, ADC_GPIO_AF);
  GPIO_PinAFConfig(ADC_SPI_GPIO, ADC_SPI_PinSource_MISO, ADC_GPIO_AF);
  GPIO_PinAFConfig(ADC_SPI_GPIO, ADC_SPI_PinSource_MOSI, ADC_GPIO_AF);

  SPI_I2S_DeInit(ADC_SPI);

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(ADC_SPI,&SPI_InitStructure);
  SPI_Cmd(ADC_SPI,ENABLE);
  SPI_I2S_ITConfig(ADC_SPI,SPI_I2S_IT_TXE,DISABLE);
  SPI_I2S_ITConfig(ADC_SPI,SPI_I2S_IT_RXNE,DISABLE);

  ADC_CS_HIGH();
  delay_01us(1);
  ADC_CS_LOW();
  SPIx_ReadWriteByte(RESETCMD);
  ADC_CS_HIGH();
  delay_01us(1);
  ADC_CS_LOW();
  SPIx_ReadWriteByte(MANUAL_MODE);
  ADC_CS_HIGH();
    
// SPIx_ReadWriteByte(ProgramReg_Auto2 );
// ADC_CS_HIGH();

// asm("nop");
// ADC_CS_LOW();
// SPIx_ReadWriteByte(AutoMode_2);
// ADC_CS_HIGH();

// asm("nop");
// ADC_CS_LOW();
// SPIx_ReadWriteByte(ContinuedSelectMode);
// ADC_CS_HIGH();
// asm("nop");
}

void adcInit()
{
  ADS7952_Init();

  configure_pins( ADC_GPIO_PIN_MOUSE1 | ADC_GPIO_PIN_MOUSE2, PIN_ANALOG | PIN_PORTF );
	
  ADC3->CR1 = ADC_CR1_SCAN;
  ADC3->CR2 = ADC_CR2_ADON | ADC_CR2_DMA | ADC_CR2_DDS;
  ADC3->SQR1 = (2-1) << 20; // NUMBER_ANALOG Channels
  ADC3->SQR3 = ADC_IN_MOUSE1 + (ADC_IN_MOUSE2<<5);
  ADC3->SMPR1 = SAMPTIME + (SAMPTIME<<3) + (SAMPTIME<<6) + (SAMPTIME<<9) + (SAMPTIME<<12)
								+ (SAMPTIME<<15) + (SAMPTIME<<18) + (SAMPTIME<<21) + (SAMPTIME<<24);
  ADC3->SMPR2 = SAMPTIME + (SAMPTIME<<3) + (SAMPTIME<<6) + (SAMPTIME<<9) + (SAMPTIME<<12)
								+ (SAMPTIME<<15) + (SAMPTIME<<18) + (SAMPTIME<<21) + (SAMPTIME<<24) + (SAMPTIME<<27);
  ADC->CCR = 0; // ADC_CCR_ADCPRE_0; Clock div 2
	
  // Enable the DMA channel here, DMA2 stream 1, channel 2
  DMA2_Stream1->CR = DMA_SxCR_PL | DMA_SxCR_CHSEL_1 | DMA_SxCR_MSIZE_0 | DMA_SxCR_PSIZE_0 | DMA_SxCR_MINC;
  DMA2_Stream1->PAR = CONVERT_PTR_UINT(&ADC3->DR);
  DMA2_Stream1->M0AR = CONVERT_PTR_UINT(&adcValues[MOUSE1]);
  DMA2_Stream1->FCR = DMA_SxFCR_DMDIS | DMA_SxFCR_FTH_0;
}

#define SPI_STICK1                     0
#define SPI_STICK2                     1
#define SPI_STICK3                     2
#define SPI_STICK4                     3
#define SPI_S1                         4
#define SPI_6POS                       5
#define SPI_S2                         6
#define SPI_S3                         7
#define SPI_S4                         8
#define SPI_TX_VOLTAGE                 9
#define SPI_RS                         10
#define SPI_LS                         11

const uint16_t adcCommands[MOUSE1+2] =
{
  MANUAL_MODE | ( SPI_STICK1     << 7 ),
  MANUAL_MODE | ( SPI_STICK2     << 7 ),
  MANUAL_MODE | ( SPI_STICK3     << 7 ),
  MANUAL_MODE | ( SPI_STICK4     << 7 ),
  MANUAL_MODE | ( SPI_S1         << 7 ),
  MANUAL_MODE | ( SPI_6POS       << 7 ),
  MANUAL_MODE | ( SPI_S2         << 7 ),
  MANUAL_MODE | ( SPI_LS         << 7 ),
  MANUAL_MODE | ( SPI_RS         << 7 ),
  MANUAL_MODE | ( SPI_S3         << 7 ),
  MANUAL_MODE | ( SPI_S4         << 7 ),
  MANUAL_MODE | ( SPI_TX_VOLTAGE << 7 ),
  MANUAL_MODE | ( 0 << 7 ),
  MANUAL_MODE | ( 0 << 7 )
};

void adcRead()
{
  const uint16_t * command = adcCommands;

  // Start on chip ADC read
  DMA2_Stream1->CR &= ~DMA_SxCR_EN;		// Disable DMA
  ADC3->SR &= ~(uint32_t) ( ADC_SR_EOC | ADC_SR_STRT | ADC_SR_OVR );
  DMA2->LIFCR = DMA_LIFCR_CTCIF1 | DMA_LIFCR_CHTIF1 |DMA_LIFCR_CTEIF1 | DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CFEIF1; // Write ones to clear bits
  DMA2_Stream1->M0AR = CONVERT_PTR_UINT(&adcValues[MOUSE1]);
  DMA2_Stream1->NDTR = 2;
  DMA2_Stream1->CR |= DMA_SxCR_EN;		// Enable DMA
  ADC3->CR2 |= (uint32_t)ADC_CR2_SWSTART;

  ADC_CS_LOW();
  delay_01us(1);
  SPIx_ReadWriteByte(*command++);	// Discard
  ADC_CS_HIGH();
  delay_01us(1);
  
  ADC_CS_LOW();
  delay_01us(1);
  SPIx_ReadWriteByte(*command++);	// Discard
  ADC_CS_HIGH();
  delay_01us(1);

  for (uint32_t adcIndex=0; adcIndex<MOUSE1; adcIndex++) {
    ADC_CS_LOW();
    delay_01us(1);
    adcValues[adcIndex] = (0x0fff & SPIx_ReadWriteByte(*command++));
    ADC_CS_HIGH();
    delay_01us(1);
  }

  for (uint32_t i=0; i<20000; i++) {
    if (DMA2->LISR & DMA_LISR_TCIF1) {
      break;
    }
  }

  // On chip ADC read should have finished
}

const int8_t ana_direction[NUMBER_ANALOG] = {1,-1,1,-1,  -1,1,-1,  -1,-1,  -1,1, 0,0,0};

uint16_t getAnalogValue(uint8_t index)
{
  if (ana_direction[index] < 0)
    return 4096 - adcValues[index];
  else
    return adcValues[index];
}
