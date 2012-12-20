/**
  ******************************************************************************
  * @file    ADC/DualADC_RegulSimu_DMAmode1/main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx.h"

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup ADC_DualADC_RegulSimu_DMAmode1
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC_CCR_ADDRESS    ((uint32_t)0x40012308)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint16_t ADCDualConvertedValue[4];

/* Private function prototypes -----------------------------------------------*/
void DMA_Config(void);
void GPIO_Config(void);
void ADC1_CH10_CH11_Config(void);
void ADC2_CH11_CH12_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f2xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f2xx.c file
     */

  /* Enable peripheral clocks *************************************************/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_ADC2, ENABLE);

  /* DMA2 Stream0 channel0 configuration **************************************/
  DMA_Config();
  
  /* ADCs configuration ------------------------------------------------------*/
  /* Configure ADC Channel10, 11, 12 pin as analog input */
  GPIO_Config();

  /* ADC Common Init */
  ADC_CommonInitStructure.ADC_Mode = ADC_DualMode_RegSimult;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC1 regular channels 10, 11 configuration */
  ADC1_CH10_CH11_Config();

  /* ADC2 regular channels 11, 12 configuration */
  ADC2_CH11_CH12_Config();

  /* Enable DMA request after last transfer (Multi-ADC mode)  */
  ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC2 */
  ADC_Cmd(ADC2, ENABLE);

  /* Start ADC1 Software Conversion */
  ADC_SoftwareStartConv(ADC1);

  while (1)
  {
  }
}

/**
  * @brief  ADC1 regular channels 10 and 11 configuration
  * @param  None
  * @retval None
  */
void ADC1_CH10_CH11_Config(void)
{
  ADC_InitTypeDef ADC_InitStructure;

  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;  
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 2;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channels 10, 11 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_3Cycles);
}

/**
  * @brief  ADC2 regular channels 11, 12 configuration
  * @param  None
  * @retval None
  */
void ADC2_CH11_CH12_Config(void)
{
  ADC_InitTypeDef ADC_InitStructure;

  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 2;
  ADC_Init(ADC2, &ADC_InitStructure);

  /* ADC2 regular channels 11, 12 configuration */ 
  ADC_RegularChannelConfig(ADC2, ADC_Channel_11, 1, ADC_SampleTime_3Cycles);
  ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 2, ADC_SampleTime_3Cycles);
}

/**
  * @brief  DMA Configuration
  * @param  None
  * @retval None
  */
void DMA_Config(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  DMA_InitStructure.DMA_Channel = DMA_Channel_0; 
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCDualConvertedValue;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC_CCR_ADDRESS;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 4;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);

  /* DMA2_Stream0 enable */
  DMA_Cmd(DMA2_Stream0, ENABLE);
}

/**
  * @brief Configure ADC Channels 10, 11, 12 pins as analog inputs
  * @param  None
  * @retval None
  */
void GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* ADC Channel 10 -> PC0
     ADC Channel 11 -> PC1
     ADC Channel 12 -> PC2
  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
