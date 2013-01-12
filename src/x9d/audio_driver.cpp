/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
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

volatile uint8_t Buzzer_count ;

// Must NOT be in flash, PDC needs a RAM source.
// Amplitude reduced to 30% to allow for voice volume
uint16_t Sine_values[] =
{
  2048,2085,2123,2160,2197,2233,2268,2303,2336,2369,
  2400,2430,2458,2485,2510,2533,2554,2573,2590,2605,
  2618,2629,2637,2643,2646,2648,2646,2643,2637,2629,
  2618,2605,2590,2573,2554,2533,2510,2485,2458,2430,
  2400,2369,2336,2303,2268,2233,2197,2160,2123,2085,
  2048,2010,1972,1935,1898,1862,1826,1792,1758,1726,
  1695,1665,1637,1610,1585,1562,1541,1522,1505,1490,
  1477,1466,1458,1452,1448,1448,1448,1452,1458,1466,
  1477,1490,1505,1522,1541,1562,1585,1610,1637,1665,
  1695,1726,1758,1792,1826,1862,1898,1935,1972,2010
};

// Sound routines
void startSound()
{
}

uint32_t currentFrequency = 0;

uint32_t getFrequency()
{
  return currentFrequency;
}

void setFrequency(uint32_t frequency)
{
  if (currentFrequency != frequency) {
    currentFrequency = frequency;
  }
}

// Start TIMER1 at 100000Hz, used for DACC trigger
void start_timer1()
{
}


// Configure DAC0 (or DAC1 for REVA)
// Not sure why PB14 has not be allocated to the DAC, although it is an EXTRA function
// So maybe it is automatically done
void initDac()
{
}

void end_sound()
{
}

// Set up for volume control (TWI0)
// Need PA3 and PA4 set to peripheral A
void init_twi()
{
}

void setVolume( unsigned char volume )
{
}
/*
void read_volume()
{
} */


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
    I2S_Cmd(CODEC_I2S,ENABLE);
    //interrupt
#ifdef CODEC_USE_INT
    SPI_I2S_ITConfig(CODEC_I2S,SPI_I2S_IT_TXE,ENABLE);
#elif defined(CODEC_USE_DMA)
#error "DMA is not initialized"
#endif
}
