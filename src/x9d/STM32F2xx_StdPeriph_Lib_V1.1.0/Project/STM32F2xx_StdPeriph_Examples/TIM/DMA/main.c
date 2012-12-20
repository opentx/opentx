/**
  ******************************************************************************
  * @file    TIM/DMA/main.c 
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

/** @addtogroup TIM_DMA
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIM1_CCR3_ADDRESS    0x4001003C

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
uint16_t SRC_Buffer[3] = {0, 0, 0};
uint16_t TimerPeriod = 0;

/* Private function prototypes -----------------------------------------------*/
void TIM_Config(void);

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

  /* TIM Configuration */
  TIM_Config();

  /* TIM1 DMA Transfer example -------------------------------------------------
  
  TIM1 input clock (TIM1CLK) is set to 2 * APB2 clock (PCLK2), since APB2 
  prescaler is different from 1.   
    TIM1CLK = 2 * PCLK2  
    PCLK2 = HCLK / 2 
    => TIM1CLK = 2 * (HCLK / 2) = HCLK = SystemCoreClock
  
  TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
  SystemCoreClock is set to 120 MHz for STM32F2xx devices.

  The objective is to configure TIM1 channel 3 to generate complementary PWM
  signal with a frequency equal to 17.57 KHz:
     - TIM1_Period = (SystemCoreClock / 17570) - 1
  and a variable duty cycle that is changed by the DMA after a specific number of
  Update DMA request.

  The number of this repetitive requests is defined by the TIM1 Repetion counter,
  each 3 Update Requests, the TIM1 Channel 3 Duty Cycle changes to the next new 
  value defined by the SRC_Buffer . 
  
  Note: 
    SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
    Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
    function to update SystemCoreClock variable value. Otherwise, any configuration
    based on this variable will be incorrect.  
  -----------------------------------------------------------------------------*/
  /* Compute the value to be set in ARR regiter to generate signal frequency at 17.57 Khz */
  TimerPeriod = (SystemCoreClock / 17570 ) - 1;
  /* Compute CCR1 value to generate a duty cycle at 50% */
  SRC_Buffer[0] = (uint16_t) (((uint32_t) 5 * (TimerPeriod - 1)) / 10);
  /* Compute CCR1 value to generate a duty cycle at 37.5% */
  SRC_Buffer[1] = (uint16_t) (((uint32_t) 375 * (TimerPeriod - 1)) / 1000);
  /* Compute CCR1 value to generate a duty cycle at 25% */
  SRC_Buffer[2] = (uint16_t) (((uint32_t) 25 * (TimerPeriod - 1)) / 100);

  /* TIM1 Peripheral Configuration -------------------------------------------*/
  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 3;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* Channel 3 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = SRC_Buffer[0];
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC3Init(TIM1, &TIM_OCInitStructure);

  /* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);
  
  /* DMA enable*/
  DMA_Cmd(DMA2_Stream6, ENABLE);
  
  /* TIM1 Update DMA Request enable */
  TIM_DMACmd(TIM1, TIM_DMA_CC3, ENABLE);

  /* Main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);

  while (1)
  {}
}

/**
  * @brief  Configure the TIM1 Pins.
  * @param  None
  * @retval None
  */
void TIM_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
  
  /* GPIOA and GPIOB clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);

  /* GPIOA Configuration: Channel 3 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_TIM1);

  /* GPIOB Configuration: Channel 3N as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_TIM1);

  /* DMA clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 , ENABLE);

  DMA_DeInit(DMA2_Stream6);
  DMA_InitStructure.DMA_Channel = DMA_Channel_6;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(TIM1_CCR3_ADDRESS) ;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)SRC_Buffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStructure.DMA_BufferSize = 3;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

  DMA_Init(DMA2_Stream6, &DMA_InitStructure);
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

  while (1)
  {}
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
