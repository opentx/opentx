/**
  ******************************************************************************
  * @file    ADC/DualADC_Interleaved_DMAmode3/main.c
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
#include "stm322xg_eval.h"
#include "stm322xg_eval_lcd.h"
#include <stdio.h>

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup ADC_DualADC_Interleaved_DMAmode3
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* used to display the ADC converted value on LCD */
#define PRINT_ON_LCD
  /* if you are not using the LCD, you can monitor the converted values by adding
     the variable "ADCDualConvertedValue" to the debugger watch window */

#define ADC_CDR_ADDRESS    ((uint32_t)0x40012308)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint16_t ADCDualConvertedValue;

ADC_InitTypeDef ADC_InitStructure;
ADC_CommonInitTypeDef ADC_CommonInitStructure;
DMA_InitTypeDef DMA_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;

/* Private function prototypes -----------------------------------------------*/
#ifdef PRINT_ON_LCD
void Display_Init(void);
void Display(void);
#endif /* PRINT_ON_LCD */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f2xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f2xx.c file
     */
#ifdef PRINT_ON_LCD
  /* LCD Display init  */
  Display_Init();
#endif /* PRINT_ON_LCD */

/******************************************************************************/
/*               ADCs interface clock, pin and DMA configuration              */
/******************************************************************************/

  /* Enable peripheral clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2, ENABLE);
  
  /* Configure ADC Channel 12 pin as analog input */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* DMA2 Stream0 channel0 configuration */
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC_CDR_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCDualConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);

  /* DMA2_Stream0 enable */
  DMA_Cmd(DMA2_Stream0, ENABLE);

/******************************************************************************/
/*  ADCs configuration: double interleaved with 6cycles delay to reach 5Msps  */
/******************************************************************************/

  /* ADC Common configuration *************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_DualMode_Interl;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_6Cycles;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_3;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInit(&ADC_CommonInitStructure);  
  
  /* DMA mode 3 is used in interleaved mode in 6-bit or 8-bit resolutions */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_8b;

  /* ADC1 regular channel 12 configuration ************************************/
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;  
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  /* ADC1 regular channel12 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_3Cycles);

  /* ADC2 regular channel 12 configuration ************************************/
  ADC_Init(ADC2, &ADC_InitStructure);
  /* ADC2 regular channel12 configuration */
  ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 1, ADC_SampleTime_3Cycles);

  /* Enable DMA request after last transfer (multi-ADC mode) ******************/
  ADC_MultiModeDMARequestAfterLastTransferCmd(ENABLE);

  /* Enable ADC1 **************************************************************/
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC2 **************************************************************/
  ADC_Cmd(ADC2, ENABLE);

  /* Start ADC1 Software Conversion */
  ADC_SoftwareStartConv(ADC1);

  while (1)
  {
#ifdef PRINT_ON_LCD
  /* Display ADCs converted values on LCD */
    Display();
#endif /* PRINT_ON_LCD */
  }
}

#ifdef PRINT_ON_LCD
/**
  * @brief  Display ADCs converted values on LCD
  * @param  None
  * @retval None
  */
void Display(void)
{
  uint32_t Voltage =0, mVoltage=0;
  uint8_t text[50];
  __IO uint32_t ADC1ConvertedVoltage;
  __IO uint32_t ADC2ConvertedVoltage;
  __IO uint8_t ADC1ConvertedValue;
  __IO uint8_t ADC2ConvertedValue;

  ADC1ConvertedValue = (ADCDualConvertedValue & 0x00FF);
  ADC1ConvertedVoltage = ADC1ConvertedValue *3300/0xFF;

  Voltage = (ADC1ConvertedVoltage)/1000;
  mVoltage = (ADC1ConvertedVoltage%1000)/100;
  sprintf((char*)text,"   ADC1 = %d,%d V   ",Voltage,mVoltage);
  LCD_DisplayStringLine(LINE(6),text);


  ADC2ConvertedValue = (ADCDualConvertedValue >>8);
  ADC2ConvertedVoltage = ADC2ConvertedValue *3300/0xFF;

  Voltage=ADC2ConvertedVoltage/1000;
  mVoltage = (ADC2ConvertedVoltage%1000)/100;
  sprintf((char*)text,"   ADC2 = %d,%d V   ",Voltage,mVoltage);
  LCD_DisplayStringLine(LINE(7),text);
}

/**
  * @brief  Display Init (LCD)
  * @param  None
  * @retval None
  */
void Display_Init(void)
{

/* Initialize the LCD */
  STM322xG_LCD_Init();
  
  /* Clear the LCD */ 
  LCD_Clear(White);

  /* Set the LCD Text size */
  LCD_SetFont(&Font8x12);

  /* Set the LCD Back Color and Text Color*/
  LCD_SetBackColor(Blue);
  LCD_SetTextColor(White);

  LCD_DisplayStringLine(LINE(0x13), " Dual ADC Interleaved DMA mode3 example ");


  /* Set the LCD Text size */
  LCD_SetFont(&Font16x24);

  LCD_DisplayStringLine(LINE(0), "ADC Ch12 Conv 5Msps ");


  /* Set the LCD Back Color and Text Color*/
  LCD_SetBackColor(White);
  LCD_SetTextColor(Blue); 

  LCD_DisplayStringLine(LINE(2)," Connect voltage to ");
  LCD_DisplayStringLine(LINE(3),"  ADC Ch12 (PC.02)  ");
}
#endif /* PRINT_ON_LCD */


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
