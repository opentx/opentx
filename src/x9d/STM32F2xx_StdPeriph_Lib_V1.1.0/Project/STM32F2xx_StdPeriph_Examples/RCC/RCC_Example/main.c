/**
  ******************************************************************************
  * @file    RCC/RCC_Example/main.c 
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

/** @addtogroup RCC_Example
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/ 
/* Private function prototypes -----------------------------------------------*/
void Delay (uint32_t nCount);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
int main(void)
{
 GPIO_InitTypeDef GPIO_InitStructure;
 NVIC_InitTypeDef NVIC_InitStructure;
 RCC_ClocksTypeDef RCC_ClockFreq;

  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f2xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f2xx.c file
     */     

  /* Initialize LEDs mounted on STM322xG-EVAL board ***************************/
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);

  /* Turn on LED1 and LED3 */
  STM_EVAL_LEDOn(LED1);
  STM_EVAL_LEDOn(LED3);
  
  /* This function fills the RCC_ClockFreq structure with the current
     frequencies of different on chip clocks (for debug purpose) **************/
  RCC_GetClocksFreq(&RCC_ClockFreq);
  
  /* Enable Clock Security System(CSS): this will generate an NMI exception
     when HSE clock fails *****************************************************/
  RCC_ClockSecuritySystemCmd(ENABLE);
 
  /* Enable and configure RCC global IRQ channel, will be used to manage HSE ready 
     and PLL ready interrupts. 
     These interrupts are enabled in stm32f2xx_it.c file **********************/
  NVIC_InitStructure.NVIC_IRQChannel = RCC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Output HSE clock on MCO1 pin(PA8) ****************************************/ 
  /* Enable the GPIOA peripheral */ 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  /* Configure MCO1 pin(PA8) in alternate function */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
    
  /* HSE clock selected to output on MCO1 pin(PA8)*/
  RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);

  while (1)
  {
    /* Toggle LED2 and LED4 */
    STM_EVAL_LEDToggle(LED2);
    STM_EVAL_LEDToggle(LED4);

    /* Insert a delay */
    Delay(0x7FFFF);

    /* Toggle LED1 and LED3 */
    STM_EVAL_LEDToggle(LED1);
    STM_EVAL_LEDToggle(LED3);

    /* Insert a delay */
    Delay(0x7FFFF);    
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
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
