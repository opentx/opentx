/**
  ******************************************************************************
  * @file    Project/Audio/Audio.c
  * @author  X9D Application Team
  * @version V 0.2
  * @date    12-JULY-2012
  * @brief   This file provides drivers foe Audio module.I2S AND GPIO Included.
  ******************************************************************************
*/

#include "stm32f2xx.h"
#include "Audio.h"
#include "hal.h"


static 
void Audio_GPIO_Init()
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

void I2S2_Init()
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
