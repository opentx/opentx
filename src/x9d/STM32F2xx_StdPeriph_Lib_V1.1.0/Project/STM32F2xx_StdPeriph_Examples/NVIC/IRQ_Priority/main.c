/**
  ******************************************************************************
  * @file    NVIC/IRQ_Priority/main.c 
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

/** @addtogroup IRQ_Priority
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint8_t PreemptionOccured = 0; 
__IO uint8_t PreemptionPriorityValue = 0; 

/* Private function prototypes -----------------------------------------------*/
void NVIC_Config(void);
void Delay(__IO uint32_t nCount);

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
  
  /* NVIC configuration ------------------------------------------------------*/
  NVIC_Config();     
  
  /* Initialize LEDs and push-button mounted on STM322xG-EVAL board */       
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_EXTI);
  STM_EVAL_PBInit(BUTTON_WAKEUP, BUTTON_MODE_EXTI); 

  /* Configure the SysTick Handler Priority: Preemption priority and subpriority */
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), !PreemptionPriorityValue, 0));

  while (1)
  {
    if(PreemptionOccured != 0)
    {
      /* Toggel The lED1 */
      STM_EVAL_LEDToggle(LED1);
      
      /* Insert delay Time */
      Delay(0x5FFFF);
      
      STM_EVAL_LEDToggle(LED2);
      
      Delay(0x5FFFF);
      
      STM_EVAL_LEDToggle(LED3);
      
      Delay(0x5FFFF);
      
      STM_EVAL_LEDToggle(LED4);
      
      Delay(0x5FFFF); 
    }
  }
}

/**
  * @brief  Configures the NVIC interrupts.
  * @param  None
  * @retval None
  */
void NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the preemption priority and subpriority:
     - 1 bits for pre-emption priority: possible value are 0 or 1 
     - 3 bits for subpriority: possible value are 0..7
     - Lower values gives higher priority  
   */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* Enable the WAKEUP_BUTTON_EXTI_IRQn Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = WAKEUP_BUTTON_EXTI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriorityValue;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the KEY_BUTTON_EXTI_IRQn Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = KEY_BUTTON_EXTI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
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
