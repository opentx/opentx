/**
  ******************************************************************************
  * @file    PWR/STANDBY/main.c 
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

/** @addtogroup PWR_STANDBY
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Uncomment the corresponding line to select the RTC Clock source */
#define RTC_CLOCK_SOURCE_LSE   /* LSE used as RTC source clock */
/* #define RTC_CLOCK_SOURCE_LSI */ /* LSI used as RTC source clock. The RTC Clock
                                      may varies due to LSI frequency dispersion. */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RTC_InitTypeDef RTC_InitStructure;
RTC_TimeTypeDef RTC_TimeStructure;
__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;

/* Private function prototypes -----------------------------------------------*/
void RTC_Config(void);
void SysTick_Configuration(void);

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

  /* Initialize LEDs and Key Button mounted on STM322xG-EVAL board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_EXTI);

  /* RTC configuration */
  RTC_Config();

  /* Turn on LED1 */
  STM_EVAL_LEDOn(LED1);

  /* Enable WKUP pin  */
  PWR_WakeUpPinCmd(ENABLE);

  /* Configure the SysTick to generate an interrupt each 250 ms */
  SysTick_Configuration();

  while (1)
  {
  }
}

/**
  * @brief  Configures the RTC.
  * @param  None
  * @retval None
  */
void RTC_Config(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Clear Wakeup flag */
  PWR_ClearFlag(PWR_FLAG_WU);

  /* Check if the StandBy flag is set */
  if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)
  {
    /* Clear StandBy flag */
    PWR_ClearFlag(PWR_FLAG_SB);

    /* Wait for RTC APB registers synchronisation (needed after start-up from Reset)*/
    RTC_WaitForSynchro();
    /* No need to configure the RTC as the RTC config(clock source, enable,
       prescaler,...) are kept after wake-up from STANDBY */
  }
  else
  {
    /* RTC Configuration ******************************************************/
    /* Reset Backup Domain */
    RCC_BackupResetCmd(ENABLE);
    RCC_BackupResetCmd(DISABLE);

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
/* The RTC Clock may varies due to LSI frequency dispersion. */   
    /* Enable the LSI OSC */ 
    RCC_LSICmd(ENABLE);

    /* Wait till LSI is ready */  
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
    {
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
  
    SynchPrediv = 0xFF;
    AsynchPrediv = 0x7F;

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
    /* Enable the LSE OSC */
    RCC_LSEConfig(RCC_LSE_ON);

    /* Wait till LSE is ready */  
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  
    SynchPrediv = 0xFF;
    AsynchPrediv = 0x7F;

#else
  #error Please select the RTC Clock source inside the main.c file
#endif /* RTC_CLOCK_SOURCE_LSI */

    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation (needed after start-up from Reset)*/
    RTC_WaitForSynchro();

    /* Set the RTC time base to 1s */
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    RTC_InitStructure.RTC_SynchPrediv = 0x00FF;

    if (RTC_Init(&RTC_InitStructure) == ERROR)
    {
      /* Turn on LED2 */
      STM_EVAL_LEDOn(LED2);
      
      /* User can add here some code to deal with this error */
      while(1);
    }

    /* Set the time to 01h 00mn 00s AM */
    RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = 0x01;
    RTC_TimeStructure.RTC_Minutes = 0x00;
    RTC_TimeStructure.RTC_Seconds = 0x00;  
  
    if(RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure) == ERROR)
    {
      /* Turn on LED2 */
      STM_EVAL_LEDOn(LED2);
      
      /* User can add here some code to deal with this error */
      while(1);
    }
  }

  /* Clear RTC Alarm Flag */ 
  RTC_ClearFlag(RTC_FLAG_ALRAF);
}

/**
  * @brief  Configures the SysTick to generate an interrupt each 250 ms.
  * @param  None
  * @retval None
  */
void SysTick_Configuration(void)
{
  /* SysTick interrupt each 250 ms */
  if (SysTick_Config((SystemCoreClock/8) / 4))
  { 
    /* Capture error */ 
    while (1);
  }

  /* Select AHB clock(HCLK) divided by 8 as SysTick clock source */
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);

  /* Set SysTick Preemption Priority to 1 */
  NVIC_SetPriority(SysTick_IRQn, 0x04);
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
