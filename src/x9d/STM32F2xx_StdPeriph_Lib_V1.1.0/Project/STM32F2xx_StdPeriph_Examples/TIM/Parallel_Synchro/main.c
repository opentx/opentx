/**
  ******************************************************************************
  * @file    TIM/Parallel_Synchro/main.c 
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

/** @addtogroup TIM_Parallel_Synchro
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;

/* Private function prototypes -----------------------------------------------*/
void TIM_Config(void);

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
       
  /* TIM Configuration */
  TIM_Config();

  /* ---------------------------------------------------------------------------
     Timers synchronisation in parallel mode
     1/TIM2 is configured as Master Timer:
         - PWM Mode is used
         - The TIM2 Update event is used as Trigger Output  
     2/TIM3 and TIM4 are slaves for TIM2,
         - PWM Mode is used
         - The ITR1(TIM2) is used as input trigger for both slaves
         - Gated mode is used, so starts and stops of slaves counters
           are controlled by the Master trigger output signal(update event).
  
    TIM2 input clock (TIM2CLK) is set to 2 * APB1 clock (PCLK1), 
    since APB1 prescaler is different from 1.   
      TIM2CLK = 2 * PCLK1  
      PCLK1 = HCLK / 4 
      => TIM2CLK = HCLK / 2 = SystemCoreClock /2
     
    To get Master Timer TIM2 output clock at 234.375 KHz running at  and the 
    duty cycle is equal to 25% :
    
    The period (TIM2_ARR) is computed as follows:
    ARR = (TIM2 counter clock / TIM2 output clock) - 1
        = 255

    The dutu cycle (TIM2_CCR1) is computed as follows:
    CCR1 = ((TIM2_ARR + 1) * 25)/100
         = 64
     
    The TIM3 is running:
    - At (TIM2 frequency)/ (TIM3 period + 1) = 23.437 KHz and a duty cycle
      equal to TIM3_CCR1/(TIM3_ARR + 1) = 30%
     
    The TIM4 is running:
    - At (TIM2 frequency)/ (TIM4 period + 1) = 46.875 KHz and a duty cycle
      equal to TIM4_CCR1/(TIM4_ARR + 1) = 60%
      
    Note: 
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f2xx.c file.
     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
     function to update SystemCoreClock variable value. Otherwise, any configuration
     based on this variable will be incorrect.    
     
  ----------------------------------------------------------------------------*/

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 255;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_TimeBaseStructure.TIM_Period = 9;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_TimeBaseStructure.TIM_Period = 4;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  /* Master Configuration in PWM1 Mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 64;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM2, &TIM_OCInitStructure);

  /* Select the Master Slave Mode */
  TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);

  /* Master Mode selection */
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

  /* Slaves Configuration: PWM1 Mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 3;

  TIM_OC1Init(TIM3, &TIM_OCInitStructure);

  TIM_OC1Init(TIM4, &TIM_OCInitStructure);

  /* Slave Mode selection: TIM3 */
  TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Gated);
  TIM_SelectInputTrigger(TIM3, TIM_TS_ITR1);
  
  /* Slave Mode selection: TIM4 */
  TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Gated);
  TIM_SelectInputTrigger(TIM4, TIM_TS_ITR1);
  
  /* TIM enable counter */
  TIM_Cmd(TIM3, ENABLE);
  TIM_Cmd(TIM2, ENABLE);
  TIM_Cmd(TIM4, ENABLE);

  while (1)
  {}
}

/**
  * @brief  Configure the TIM Pins.
  * @param  None
  * @retval None
  */
void TIM_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* TIM2, TIM3 and TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 |
                         RCC_APB1Periph_TIM4, ENABLE);

  /* GPIOA, GPIOB and GPIOC clocks enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
                         
  /* GPIOC Configuration: PC6(TIM3 CH1) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* GPIOA Configuration: PA0(TIM2 CH1) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* GPIOB Configuration: PB6(TIM4 CH1) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Connect TIM pins to AF2 */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);     
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
