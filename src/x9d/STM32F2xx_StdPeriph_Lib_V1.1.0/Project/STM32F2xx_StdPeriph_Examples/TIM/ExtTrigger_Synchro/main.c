/**
  ******************************************************************************
  * @file    TIM/ExtTrigger_Synchro/main.c 
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

/** @addtogroup TIM_ExtTrigger_Synchro
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_ICInitTypeDef  TIM_ICInitStructure;
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

  /* Timers synchronisation in cascade mode with an external trigger -----
    1/TIM1 is configured as Master Timer:
       - Toggle Mode is used
       - The TIM1 Enable event is used as Trigger Output 

    2/TIM1 is configured as Slave Timer for an external Trigger connected
      to TIM1 TI2 pin (TIM1 CH2 configured as input pin):
       - The TIM1 TI2FP2 is used as Trigger Input
       - Rising edge is used to start and stop the TIM1: Gated Mode.

    3/TIM3 is slave for TIM1 and Master for TIM4,
       - Toggle Mode is used
       - The ITR1(TIM1) is used as input trigger 
       - Gated mode is used, so start and stop of slave counter
         are controlled by the Master trigger output signal(TIM1 enable event).
       - The TIM3 enable event is used as Trigger Output. 

    4/TIM4 is slave for TIM3,
       - Toggle Mode is used
       - The ITR2(TIM3) is used as input trigger
       - Gated mode is used, so start and stop of slave counter
         are controlled by the Master trigger output signal(TIM3 enable event).
    
    TIM1 input clock (TIM1CLK) is set to 2 * APB2 clock (PCLK2), 
    since APB2 prescaler is different from 1.   
      TIM1CLK = 2 * PCLK2  
      PCLK2 = HCLK / 2 
      => TIM1CLK = 2 * (HCLK / 2) = HCLK = SystemCoreClock 
    
    TIM3/TIM4 input clock (TIM3CLK/TIM4CLK) is set to 2 * APB1 clock (PCLK1), 
    since APB1 prescaler is different from 1.   
      TIM3CLK/TIM4CLK = 2 * PCLK1  
      PCLK1 = HCLK / 4 
      => TIM3CLK/TIM4CLK = HCLK / 2 = SystemCoreClock /2
  
    The TIM1CLK is fixed to 120 MHZ, the Prescaler is equal to 4 so the TIMx clock 
    counter is equal to 24 MHz.
    The TIM3CLK  and TIM4CLK are fixed to 60 MHZ, the Prescaler is equal to 4 
    so the TIMx clock counter is equal to 12 MHz.      
    The Three Timers are running at: 
    TIMx frequency = TIMx clock counter/ 2*(TIMx_Period + 1) = 162.1 KHz.

    The starts and stops of the TIM1 counters are controlled by the 
    external trigger.
    The TIM3 starts and stops are controlled by the TIM1, and the TIM4 
    starts and stops are controlled by the TIM3.  
    
    Note: 
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f2xx.c file.
     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
     function to update SystemCoreClock variable value. Otherwise, any configuration
     based on this variable will be incorrect.  
  -------------------------------------------------------------------- */

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 73;
  TIM_TimeBaseStructure.TIM_Prescaler = 4;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  TIM_TimeBaseStructure.TIM_Period = 36;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  TIM_TimeBaseStructure.TIM_Period = 36;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  /* Master Configuration in Toggle Mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 64;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM1, &TIM_OCInitStructure);

  /* TIM1 Input Capture Configuration */
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0;

  TIM_ICInit(TIM1, &TIM_ICInitStructure);

  /* TIM1 Input trigger configuration: External Trigger connected to TI2 */
  TIM_SelectInputTrigger(TIM1, TIM_TS_TI2FP2);
  TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Gated);

  /* Select the Master Slave Mode */
  TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);

  /* Master Mode selection: TIM1 */
  TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Enable);

  /* Slaves Configuration: Toggle Mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 10;
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);

  TIM_OC1Init(TIM4, &TIM_OCInitStructure);

  /* Slave Mode selection: TIM3 */
  TIM_SelectInputTrigger(TIM3, TIM_TS_ITR0);
  TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Gated);

  /* Select the Master Slave Mode */
  TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);

  /* Master Mode selection: TIM3 */
  TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Enable);

  /* Slave Mode selection: TIM4 */
  TIM_SelectInputTrigger(TIM4, TIM_TS_ITR2);
  TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Gated);
  
  /* TIM1 Main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);

  /* TIM enable counter */
  TIM_Cmd(TIM1, ENABLE);
  TIM_Cmd(TIM3, ENABLE);
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

  /* TIM3 and TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);

  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
                           
  /* GPIOA and GPIOB clocks enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE, ENABLE);

  /* GPIOA Configuration: PA.08(TIM1 CH1) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
    /* GPIOE Configuration: PE.11(TIM1 CH2) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* Connect TIM pins to AF1 */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);  
  
  /* GPIOB Configuration: PB.06(TIM4 CH1) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
     /* Connect TIM pins to AF2 */  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);
  
  /* GPIOC Configuration: PC.06(TIM3 CH1) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
   /* Connect TIM pins to AF2 */  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3); 
  
  
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
