/**
  ******************************************************************************
  * @file    DAC/DAC_SignalsGeneration/main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Main program body.
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

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup DAC_SignalsGeneration
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DAC_DHR12R2_ADDRESS    0x40007414
#define DAC_DHR8R1_ADDRESS     0x40007410

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
DAC_InitTypeDef  DAC_InitStructure;

const uint16_t Sine12bit[32] = {
                      2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                      3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909, 
                      599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};
const uint8_t Escalator8bit[6] = {0x0, 0x33, 0x66, 0x99, 0xCC, 0xFF};

__IO uint8_t SelectedWavesForm = 1;
__IO uint8_t KeyPressed = SET; 

/* Private function prototypes -----------------------------------------------*/
void TIM6_Config(void);

void DAC_Ch1_EscalatorConfig(void);
void DAC_Ch2_SineWaveConfig(void);

void DAC_Ch1_NoiseConfig(void);
void DAC_Ch2_TriangleConfig(void);

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

  /* Preconfiguration before using DAC----------------------------------------*/
  GPIO_InitTypeDef GPIO_InitStructure;

  /* DMA1 clock and GPIOA clock enable (to be used with DAC) */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1 | RCC_AHB1Periph_GPIOA, ENABLE);

  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

  /* DAC channel 1 & 2 (DAC_OUT1 = PA.4)(DAC_OUT2 = PA.5) configuration */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* TIM6 Configuration ------------------------------------------------------*/
  TIM6_Config();  
  
  /* Configures Key Button */
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_EXTI);
  
  while (1)
  {
    /* If the Key is pressed */
    if (KeyPressed != RESET)
    {            
      DAC_DeInit(); 

      /* select waves forms according to the Key Button status */
      if (SelectedWavesForm == 1)
      {
        /* The sine wave and the escalator wave has been selected */

        /* Escalator Wave generator ------------------------------------------*/
        DAC_Ch1_EscalatorConfig();

        /* Sine Wave generator -----------------------------------------------*/
        DAC_Ch2_SineWaveConfig();
         
      }
      else
      {
        /* The triangle wave and the noise wave has been selected */

        /* Noise Wave generator ----------------------------------------------*/
        DAC_Ch1_NoiseConfig();

        /* Triangle Wave generator -------------------------------------------*/
        DAC_Ch2_TriangleConfig();
      }
      
      KeyPressed = RESET; 
    }
  }
}

/**             
  * @brief  TIM6 Configuration
  * @note   TIM6 configuration is based on APB1 frequency
  * @note   TIM6 Update event occurs each TIM6CLK/256   
  * @param  None
  * @retval None
  */
void TIM6_Config(void)
{
  TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
  /* TIM6 Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  
  /* --------------------------------------------------------
  TIM3 input clock (TIM6CLK) is set to 2 * APB1 clock (PCLK1), 
  since APB1 prescaler is different from 1.   
    TIM6CLK = 2 * PCLK1  
    TIM6CLK = HCLK / 2 = SystemCoreClock /2 
          
  TIM6 Update event occurs each TIM6CLK/256 

  Note: 
   SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f2xx.c file.
   Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
   function to update SystemCoreClock variable value. Otherwise, any configuration
   based on this variable will be incorrect.    

  ----------------------------------------------------------- */
  /* Time base configuration */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Period = 0xFF;          
  TIM_TimeBaseStructure.TIM_Prescaler = 0;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);

  /* TIM6 TRGO selection */
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
  
  /* TIM6 enable counter */
  TIM_Cmd(TIM6, ENABLE);
}

/**
  * @brief  DAC  Channel2 SineWave Configuration
  * @param  None
  * @retval None
  */
void DAC_Ch2_SineWaveConfig(void)
{
  DMA_InitTypeDef DMA_InitStructure;
  
  /* DAC channel2 Configuration */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);

  /* DMA1_Stream6 channel7 configuration **************************************/
  DMA_DeInit(DMA1_Stream6);
  DMA_InitStructure.DMA_Channel = DMA_Channel_7;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)DAC_DHR12R2_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&Sine12bit;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize = 32;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream6, &DMA_InitStructure);

  /* Enable DMA1_Stream6 */
  DMA_Cmd(DMA1_Stream6, ENABLE);

  /* Enable DAC Channel2 */
  DAC_Cmd(DAC_Channel_2, ENABLE);

  /* Enable DMA for DAC Channel2 */
  DAC_DMACmd(DAC_Channel_2, ENABLE);
}

/**
  * @brief  DAC Channel1 Escalator Configuration
  * @param  None
  * @retval None
  */
void DAC_Ch1_EscalatorConfig(void)
{
  DMA_InitTypeDef DMA_InitStructure;

  /* DAC channel1 Configuration */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

  /* DMA1_Stream5 channel7 configuration **************************************/  
  DMA_DeInit(DMA1_Stream5);
  DMA_InitStructure.DMA_Channel = DMA_Channel_7;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_ADDRESS;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&Escalator8bit;
  DMA_InitStructure.DMA_BufferSize = 6;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream5, &DMA_InitStructure);    

  /* Enable DMA1_Stream5 */
  DMA_Cmd(DMA1_Stream5, ENABLE);
  
  /* Enable DAC Channel1 */
  DAC_Cmd(DAC_Channel_1, ENABLE);

  /* Enable DMA for DAC Channel1 */
  DAC_DMACmd(DAC_Channel_1, ENABLE);
}

/**
  * @brief  DAC Channel2 Triangle Configuration
  * @param  None
  * @retval None
  */
void DAC_Ch2_TriangleConfig(void)
{
 /* DAC channel2 Configuration */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Triangle;
  DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_1023;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);

  /* Enable DAC Channel2 */
  DAC_Cmd(DAC_Channel_2, ENABLE);

  /* Set DAC channel2 DHR12RD register */
  DAC_SetChannel2Data(DAC_Align_12b_R, 0x100);
}

/**
  * @brief  DAC  Channel1 Noise Configuration
  * @param  None
  * @retval None
  */
void DAC_Ch1_NoiseConfig(void)
{
 /* DAC channel1 Configuration */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Noise;
  DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits10_0;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

  /* Enable DAC Channel1 */
  DAC_Cmd(DAC_Channel_1, ENABLE);

  /* Set DAC Channel1 DHR12L register */
  DAC_SetChannel1Data(DAC_Align_12b_L, 0x7FF0);
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
