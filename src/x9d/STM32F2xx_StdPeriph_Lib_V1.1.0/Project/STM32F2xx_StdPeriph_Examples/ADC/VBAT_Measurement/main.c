/**
  ******************************************************************************
  * @file    ADC/VBAT_Measurement/main.c 
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

/** @addtogroup ADC_VBAT_Measurement
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* used to display the ADC converted value on LCD */
#define PRINT_ON_LCD
  /* if you are not using the LCD, you can monitor the converted value by adding
     the variable "ADCConvertedValue" to the debugger watch window */

#define ADC1_DR_ADDRESS    ((uint32_t)0x4001204C)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint16_t ADCConvertedValue = 0;
__IO uint32_t VBATVoltage = 0;

/* Private function prototypes -----------------------------------------------*/
void ADC1_ChVbat_DMA_Config(void);

#ifdef PRINT_ON_LCD
void Display_Init(void);
void Display(void);
static void delay(__IO uint32_t nCount);
#endif /* PRINT_ON_LCD */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
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

  /* ADC1 Channel Vbat configuration */
  ADC1_ChVbat_DMA_Config();
 
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConv(ADC1);

  while (1)
  {
#ifdef PRINT_ON_LCD
  /* Display ADC converted value on LCD */
    Display();
#endif /* PRINT_ON_LCD */
  }
}

/**
  * @brief  ADC1 Channel Vbat configuration (DMA, ADC, CLK)
  * @param  None
  * @retval None
  */
void ADC1_ChVbat_DMA_Config(void)
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
    
  /* Enable peripheral clocks *************************************************/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* DMA2_Stream0 channel0 configuration **************************************/
  DMA_DeInit(DMA2_Stream0);
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCConvertedValue;
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
    
  /* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC1 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;  
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);
  
  /* ADC1 regular channel18 (VBAT) configuration ******************************/
  ADC_RegularChannelConfig(ADC1, ADC_Channel_Vbat, 1, ADC_SampleTime_15Cycles);

  /* Enable VBAT channel */
  ADC_VBATCmd(ENABLE); 

  /* Enable DMA request after last transfer (Single-ADC mode) */
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

  /* Enable ADC1 **************************************************************/
  ADC_Cmd(ADC1, ENABLE);
}

#ifdef PRINT_ON_LCD
/**
  * @brief  Display ADC converted value on LCD
  * @param  None
  * @retval None
  */
void Display(void)
{
   uint32_t vbatV=0,vbatmV=0;
   uint8_t text[50];

  /*The VBAT pin is internally connected to a bridge divider by 2 */
  VBATVoltage = (ADCConvertedValue*2)*3300/0xFFF;
  vbatV=VBATVoltage/1000;
  vbatmV = (VBATVoltage%1000)/100;
  sprintf((char*)text,"       %d,%d V   ",vbatV,vbatmV);
  LCD_DisplayStringLine(LINE(6),text);

  delay(100);
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

  LCD_DisplayStringLine(LINE(0x13), " STM32F2xx ADC1 VBAT Measurement example");

  /* Set the LCD Text size */
  LCD_SetFont(&Font16x24);

  LCD_DisplayStringLine(LINE(0), "**VBAT Measurement**");

  /* Set the LCD Back Color and Text Color*/
  LCD_SetBackColor(White);
  LCD_SetTextColor(Blue);

  LCD_DisplayStringLine(LINE(2)," Eval Board Instant ");
  LCD_DisplayStringLine(LINE(4),"  Battery Voltage   ");
}

/**
  * @brief  Inserts a delay time
  * @param  nCount: specifies the delay time length
  * @retval None
  */
static void delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
  for(index = (100000 * nCount); index != 0; index--)
  {
  }
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
