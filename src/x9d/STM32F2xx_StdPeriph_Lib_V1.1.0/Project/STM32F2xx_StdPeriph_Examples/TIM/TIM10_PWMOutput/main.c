/**
  ******************************************************************************
  * @file    TIM/TIM10_PWMOutput/main.c 
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

/** @addtogroup TIM10_PWMOutput
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
uint16_t CCR1_Val = 333;
uint16_t PrescalerValue = 0;

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
  
  /* ---------------------------------------------------------------------------
    TIM10 Configuration: generate 1 PWM signal:
    
    In this example TIM10 input clock (TIM10CLK) is set to 2 * APB2 clock (PCLK2), 
    since APB2 prescaler is different from 1.   
      TIM10CLK = 2 * PCLK2  
      PCLK2 = HCLK / 2 
      => TIM10CLK = HCLK = SystemCoreClock
          
    To get TIM10 counter clock at 20 MHz, the prescaler is computed as follows:
       Prescaler = (TIM10CLK / TIM10 counter clock) - 1
       Prescaler = (SystemCoreClock /20 MHz) - 1
                                              
    To get TIM10 output clock at 30 KHz, the period (TIM10_ARR) is computed as follows:
       ARR = (TIM10 counter clock / TIM10 output clock) - 1
           = 665
                  
    TIM10 Channel1 duty cycle = (TIM10_CCR1/ TIM10_ARR)* 100 = 50%

    Note: 
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f2xx.c file.
     Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
     function to update SystemCoreClock variable value. Otherwise, any configuration
     based on this variable will be incorrect.    
  --------------------------------------------------------------------------- */   


  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 20000000) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 665;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM10, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(TIM10, &TIM_OCInitStructure);

  TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TIM10, ENABLE);

  /* TIM10 enable counter */
  TIM_Cmd(TIM10, ENABLE);

  while (1)
  {}
}

/**
  * @brief  Configure the TIM10 Ouput Channels.
  * @param  None
  * @retval None
  */
void TIM_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* TIM10 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);

  /* GPIOF clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  
  /* GPIOF Configuration: TIM10 CH1 (PF6) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOF, &GPIO_InitStructure); 

  /* Connect TIM pins to AF3 */
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource6, GPIO_AF_TIM10);
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
