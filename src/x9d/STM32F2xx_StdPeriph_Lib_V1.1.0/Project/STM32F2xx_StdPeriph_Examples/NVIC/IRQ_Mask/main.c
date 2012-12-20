/**
  ******************************************************************************
  * @file    NVIC/IRQ_Mask/main.c 
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

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup IRQ_Mask
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
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

  /* Initialize Leds mounted on STM322xG-EVAL board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);

  /* Initialize the KEY and SEL buttons mounted on STM322xG-EVAL board */
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_GPIO);
  STM_EVAL_PBInit(BUTTON_TAMPER, BUTTON_MODE_EXTI);
    
  /* TIM configuration -------------------------------------------------------*/
  TIM_Config();

  while (1)
  { 
    /* Wait until KEY button is pressed. */
    while(STM_EVAL_PBGetState(BUTTON_KEY) == RESET)
    {
    }
    while(STM_EVAL_PBGetState(BUTTON_KEY) != RESET)
    {
    }

    /* This instruction raises the execution priority to 0. This prevents all 
       exceptions with configurable priority from activating, other than through 
       the HardFault fault escalation mechanism. */
    __disable_irq();

    /* Turn LED4 ON */
    STM_EVAL_LEDOn(LED4);

    /* Wait until KEY button is pressed. */
    while(STM_EVAL_PBGetState(BUTTON_KEY) == RESET)
    {
    }
    while(STM_EVAL_PBGetState(BUTTON_KEY) != RESET)
    {
    }

    /* This instruction will allow all exceptions with configurable priority to 
       be activated. */
    __enable_irq();

    /* Turn LED4 OFF */
    STM_EVAL_LEDOff(LED4);
  }
}

/**
  * @brief  Configures the used Timers.
  * @param  None
  * @retval None
  */
void TIM_Config(void)
{ 
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;

  /* Enable TIM2, TIM3 and TIM4 clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 |
                         RCC_APB1Periph_TIM4, ENABLE);

  /* TIM2 configuration */
  TIM_TimeBaseStructure.TIM_Period = 0x4AF;          
  TIM_TimeBaseStructure.TIM_Prescaler = ((SystemCoreClock/1200) - 1);
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_OCStructInit(&TIM_OCInitStructure);
  
  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_Pulse = 0x0;  
  TIM_OC1Init(TIM2, &TIM_OCInitStructure);
  
  /* TIM3 configuration */
  TIM_TimeBaseStructure.TIM_Period = 0x95F;    
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  
  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  
  /* TIM4 configuration */
  TIM_TimeBaseStructure.TIM_Period = 0xE0F;  
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  
  /* Output Compare Timing Mode configuration: Channel1 */
  TIM_OC1Init(TIM4, &TIM_OCInitStructure);

  /* Immediate load of TIM2,TIM3 and TIM4 Precaler values */
  TIM_PrescalerConfig(TIM2, ((SystemCoreClock/1200) - 1), TIM_PSCReloadMode_Immediate);
  TIM_PrescalerConfig(TIM3, ((SystemCoreClock/1200) - 1), TIM_PSCReloadMode_Immediate);
  TIM_PrescalerConfig(TIM4, ((SystemCoreClock/1200) - 1), TIM_PSCReloadMode_Immediate);

  /* Clear TIM2, TIM3 and TIM4 update pending flags */
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);
  TIM_ClearFlag(TIM3, TIM_FLAG_Update);
  TIM_ClearFlag(TIM4, TIM_FLAG_Update);

  /* Configure two bits for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

  /* Enable the TIM2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the TIM3 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TIM4 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable TIM2, TIM3 and TIM4 Update interrupts */
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

  /* TIM2, TIM3 and TIM4 enable counters */
  TIM_Cmd(TIM2, ENABLE);
  TIM_Cmd(TIM3, ENABLE);
  TIM_Cmd(TIM4, ENABLE);
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
