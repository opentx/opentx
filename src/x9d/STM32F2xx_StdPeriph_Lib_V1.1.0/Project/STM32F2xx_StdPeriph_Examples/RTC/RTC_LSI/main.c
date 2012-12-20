/**
  ******************************************************************************
  * @file    RTC/RTC_LSI/main.c 
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

/** @addtogroup RTC_LSI
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RTC_InitTypeDef   RTC_InitStructure;
__IO uint32_t LsiFreq = 0;
__IO uint32_t CaptureNumber = 0, PeriodValue = 0;

/* Private function prototypes -----------------------------------------------*/
void RTC_Config(void);
uint32_t GetLSIFrequency(void);

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
   
  /* Initialize LEDs mounted on STM322xG-EVAL board --------------------------*/
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_GPIO);
  
  /* RTC Configuration -------------------------------------------------------*/
  RTC_Config();

  /* Wait Until KEY BUTTON is pressed */
  while(STM_EVAL_PBGetState(BUTTON_KEY) != RESET)
  {
  }
  
  /* Get the LSI frequency:  TIM5 is used to measure the LSI frequency */
  LsiFreq = GetLSIFrequency();

  /* Turn on LED2 */
  STM_EVAL_LEDOn(LED2);

  /* Calendar Configuration */
  RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
  RTC_InitStructure.RTC_SynchPrediv	=  (LsiFreq/128) - 1;
  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
  RTC_Init(&RTC_InitStructure);

  while (1)
  {
  }
}

/**
  * @brief  Configure the RTC peripheral by selecting the clock source.
  * @param  None
  * @retval None
  */
void RTC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; 
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);

/* LSI used as RTC source clock */
/* The RTC Clock may varies due to LSI frequency dispersion. */   
  /* Enable the LSI OSC */ 
  RCC_LSICmd(ENABLE);

  /* Wait till LSI is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
   
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();

  /* Calendar Configuration with LSI supposed at 32KHz */
  RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
  RTC_InitStructure.RTC_SynchPrediv	=  0xFF; /* (32KHz / 128) - 1 = 0xFF*/
  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
  RTC_Init(&RTC_InitStructure);  

  /* EXTI configuration *******************************************************/
  EXTI_ClearITPendingBit(EXTI_Line22);
  EXTI_InitStructure.EXTI_Line = EXTI_Line22;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the RTC Wakeup Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  

  /* Configure the RTC WakeUp Clock source: CK_SPRE (1Hz) */
  RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
  RTC_SetWakeUpCounter(0x0);

  /* Enable the RTC Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  
  /* Enable Wakeup Counter */
  RTC_WakeUpCmd(ENABLE);
}

/**
  * @brief  Configures TIM5 to measure the LSI oscillator frequency. 
  * @param  None
  * @retval LSI Frequency
  */
uint32_t GetLSIFrequency(void)
{
  NVIC_InitTypeDef   NVIC_InitStructure;
  TIM_ICInitTypeDef  TIM_ICInitStructure;
  RCC_ClocksTypeDef  RCC_ClockFreq;

  /* Enable the LSI oscillator ************************************************/
  RCC_LSICmd(ENABLE);
  
  /* Wait till LSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}

  /* TIM5 configuration *******************************************************/ 
  /* Enable TIM5 clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
  
  /* Connect internally the TIM5_CH4 Input Capture to the LSI clock output */
  TIM_RemapConfig(TIM5, TIM5_LSI);

  /* Configure TIM5 presclaer */
  TIM_PrescalerConfig(TIM5, 0, TIM_PSCReloadMode_Immediate);
  
  /* TIM5 configuration: Input Capture mode ---------------------
     The LSI oscillator is connected to TIM5 CH4
     The Rising edge is used as active edge,
     The TIM5 CCR4 is used to compute the frequency value 
  ------------------------------------------------------------ */
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
  TIM_ICInitStructure.TIM_ICFilter = 0;
  TIM_ICInit(TIM5, &TIM_ICInitStructure);
  
  /* Enable TIM5 Interrupt channel */
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable TIM5 counter */
  TIM_Cmd(TIM5, ENABLE);

  /* Reset the flags */
  TIM5->SR = 0;
    
  /* Enable the CC4 Interrupt Request */  
  TIM_ITConfig(TIM5, TIM_IT_CC4, ENABLE);


  /* Wait until the TIM5 get 2 LSI edges (refer to TIM5_IRQHandler() in 
    stm32f2xx_it.c file) ******************************************************/
  while(CaptureNumber != 2)
  {
  }
  /* Deinitialize the TIM5 peripheral registers to their default reset values */
  TIM_DeInit(TIM5);


  /* Compute the LSI frequency, depending on TIM5 input clock frequency (PCLK1)*/
  /* Get SYSCLK, HCLK and PCLKx frequency */
  RCC_GetClocksFreq(&RCC_ClockFreq);

  /* Get PCLK1 prescaler */
  if ((RCC->CFGR & RCC_CFGR_PPRE1) == 0)
  { 
    /* PCLK1 prescaler equal to 1 => TIMCLK = PCLK1 */
    return ((RCC_ClockFreq.PCLK1_Frequency / PeriodValue) * 8);
  }
  else
  { /* PCLK1 prescaler different from 1 => TIMCLK = 2 * PCLK1 */
    return (((2 * RCC_ClockFreq.PCLK1_Frequency) / PeriodValue) * 8) ;
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
