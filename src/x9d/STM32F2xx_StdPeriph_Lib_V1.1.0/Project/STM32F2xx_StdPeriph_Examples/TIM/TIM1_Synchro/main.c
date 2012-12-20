/**
  ******************************************************************************
  * @file    TIM/TIM1_Synchro/main.c 
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

/** @addtogroup TIM_TIM1_Synchro
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
TIM_BDTRInitTypeDef TIM_BDTRInitStructure;

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
       
  /* TIM1 Configuration */
  TIM_Config();
  
  /* ---------------------------------------------------------------------------
    TIM1 and Timers(TIM3 and TIM4) synchronisation in parallel mode.
     1/TIM1 is configured as Master Timer:
         - PWM Mode is used
         - The TIM1 Update event is used as Trigger Output
    
     2/TIM3 and TIM4 are slaves for TIM1,
         - PWM Mode is used
         - The ITR0(TIM1) is used as input trigger for both slaves
         - Gated mode is used, so starts and stops of slaves counters
           are controlled by the Master trigger output signal(update event).
    
    In this example TIM1 input clock (TIM1CLK) is set to 2 * APB2 clock (PCLK2), 
    since APB2 prescaler is different from 1.   
      TIM1CLK = 2 * PCLK2  
      PCLK2 = HCLK / 2 
      => TIM1CLK = HCLK = SystemCoreClock
          
    The TIM1 counter clock is equal to SystemCoreClock = 120 Mhz.
                                                               
    The Master Timer TIM1 is running at:
    TIM1 frequency = TIM1 counter clock / (TIM1_Period + 1) = 468.750 KHz
    TIM1_Period = (TIM1 counter clock / TIM1 frequency) - 1 = 255
    and the duty cycle is equal to: TIM1_CCR1/(TIM1_ARR + 1) = 50%

    The TIM3 is running at: 
    (TIM1 frequency)/ ((TIM3 period +1)* (Repetition_Counter+1)) = 31.250 KHz and
    a duty cycle equal to TIM3_CCR1/(TIM3_ARR + 1) = 33.3%

    The TIM4 is running at:
    (TIM1 frequency)/ ((TIM4 period +1)* (Repetition_Counter+1)) = 46.875 KHz and
    a duty cycle equal to TIM4_CCR1/(TIM4_ARR + 1) = 50%

    Note: 
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f2xx.c file.
     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
     function to update SystemCoreClock variable value. Otherwise, any configuration
     based on this variable will be incorrect.    
  --------------------------------------------------------------------------- */   


  /* TIM3 Peripheral Configuration ----------------------------------------*/
  /* TIM3 Slave Configuration: PWM1 Mode */
  TIM_TimeBaseStructure.TIM_Period = 2;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 1;

  TIM_OC1Init(TIM3, &TIM_OCInitStructure);

  /* Slave Mode selection: TIM3 */
  TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Gated);
  TIM_SelectInputTrigger(TIM3, TIM_TS_ITR0);
  
  /* TIM4 Peripheral Configuration ----------------------------------------*/
  /* TIM4 Slave Configuration: PWM1 Mode */
  TIM_TimeBaseStructure.TIM_Period = 1;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 1;

  TIM_OC1Init(TIM4, &TIM_OCInitStructure);

  /* Slave Mode selection: TIM4 */
  TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Gated);
  TIM_SelectInputTrigger(TIM4, TIM_TS_ITR0);
  
  /* TIM1 Peripheral Configuration ----------------------------------------*/
  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 255;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 4;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* Channel 1 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 127;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);

  /* Automatic Output enable, Break, dead time and lock configuration*/
  TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
  TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
  TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
  TIM_BDTRInitStructure.TIM_DeadTime = 5;
  TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
  TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
  TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;

  TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);

  /* Master Mode selection */
  TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);

  /* Select the Master Slave Mode */
  TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);
  
  /* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);

  /* TIM enable counter */
  TIM_Cmd(TIM3, ENABLE);
  TIM_Cmd(TIM4, ENABLE);

  /* Main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);

  while (1)
  {}
}

/**
  * @brief  Configures TIM1, TIM3 and TIM4 Pins.
  * @param  None
  * @retval None
  */
void TIM_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOA, GPIOB and GPIOC clocks enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC, ENABLE);
  
  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
                         
  /* TIM3 and TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);

  /* GPIOA Configuration: TIM1 Channel1 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* GPIOC Configuration: TIM3 Channel1 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* GPIOB Configuration: TIM4 Channel1 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Connect TIM pins to AF1 */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
  
  /* Connect TIM pins to AF2 */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3);
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
