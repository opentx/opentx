/**
  ******************************************************************************
  * @file    RTC/HW_Calendar/main.c 
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
#include "main.h"
#include <stdio.h>

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup HW_Calendar
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
USART_InitTypeDef USART_InitStructure;
RTC_TimeTypeDef RTC_TimeStructure;
RTC_InitTypeDef RTC_InitStructure;
RTC_AlarmTypeDef  RTC_AlarmStructure;

__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;

/* Private function prototypes -----------------------------------------------*/
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
  NVIC_InitTypeDef  NVIC_InitStructure;
  EXTI_InitTypeDef  EXTI_InitStructure;
       
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);
 
  /* Output a message on Hyperterminal using printf function */
  printf("\n\r  *********************** RTC Hardware Calendar Example ***********************\n\r");
  
  if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)
  {  
    /* RTC configuration  */
    RTC_Config();

    /* Configure the RTC data register and RTC prescaler */
    RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
    RTC_InitStructure.RTC_SynchPrediv = SynchPrediv;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
   
    /* Check on RTC init */
    if (RTC_Init(&RTC_InitStructure) == ERROR)
    {
      printf("\n\r        /!\\***** RTC Prescaler Config failed ********/!\\ \n\r");
    }

    /* Configure the time register */
    RTC_TimeRegulate(); 
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      printf("\r\n Power On Reset occurred....\n\r");
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      printf("\r\n External Reset occurred....\n\r");
    }

    printf("\n\r No need to configure RTC....\n\r");
    
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();

    /* Clear the RTC Alarm Flag */
    RTC_ClearFlag(RTC_FLAG_ALRAF);

    /* Clear the EXTI Line 17 Pending bit (Connected internally to RTC Alarm) */
    EXTI_ClearITPendingBit(EXTI_Line17);

    /* Display the RTC Time and Alarm */
    RTC_TimeShow();
    RTC_AlarmShow();
  }
   
  /* Configure the external interrupt "WAKEUP", "KEY" and "TAMPER" buttons */
  STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_EXTI); 
  STM_EVAL_PBInit(BUTTON_TAMPER , BUTTON_MODE_EXTI);
  STM_EVAL_PBInit(BUTTON_WAKEUP , BUTTON_MODE_EXTI);

  /* Configure LEDx */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);

  /* Turn LED1 ON */
  STM_EVAL_LEDOn(LED2);

  /* RTC Alarm A Interrupt Configuration */
  /* EXTI configuration *********************************************************/
  EXTI_ClearITPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

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
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);
    
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

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
}

/**
  * @brief  Returns the time entered by user, using Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_TimeRegulate(void)
{
  uint32_t tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;

  printf("\n\r==============Time Settings=====================================\n\r");
  RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
  printf("  Please Set Hours:\n\r");
  while (tmp_hh == 0xFF)
  {
    tmp_hh = USART_Scanf(23);
    RTC_TimeStructure.RTC_Hours = tmp_hh;
  }
  printf("  %0.2d\n\r", tmp_hh);
  
  printf("  Please Set Minutes:\n\r");
  while (tmp_mm == 0xFF)
  {
    tmp_mm = USART_Scanf(59);
    RTC_TimeStructure.RTC_Minutes = tmp_mm;
  }
  printf("  %0.2d\n\r", tmp_mm);
  
  printf("  Please Set Seconds:\n\r");
  while (tmp_ss == 0xFF)
  {
    tmp_ss = USART_Scanf(59);
    RTC_TimeStructure.RTC_Seconds = tmp_ss;
  }
  printf("  %0.2d\n\r", tmp_ss);

  /* Configure the RTC time register */
  if(RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure) == ERROR)
  {
    printf("\n\r>> !! RTC Set Time failed. !! <<\n\r");
  } 
  else
  {
    printf("\n\r>> !! RTC Set Time success. !! <<\n\r");
    RTC_TimeShow();
    /* Indicator for the RTC configuration */
    RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
  }

  tmp_hh = 0xFF;
  tmp_mm = 0xFF;
  tmp_ss = 0xFF;

  /* Disable the Alarm A */
  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

  printf("\n\r==============Alarm A Settings=====================================\n\r");
  RTC_AlarmStructure.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
  printf("  Please Set Alarm Hours:\n\r");
  while (tmp_hh == 0xFF)
  {
    tmp_hh = USART_Scanf(23);
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours = tmp_hh;
  }
  printf("  %0.2d\n\r", tmp_hh);
  
  printf("  Please Set Alarm Minutes:\n\r");
  while (tmp_mm == 0xFF)
  {
    tmp_mm = USART_Scanf(59);
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = tmp_mm;
  }
  printf("  %0.2d\n\r", tmp_mm);
  
  printf("  Please Set Alarm Seconds:\n\r");
  while (tmp_ss == 0xFF)
  {
    tmp_ss = USART_Scanf(59);
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = tmp_ss;
  }
  printf("  %0.2d", tmp_ss);

  /* Set the Alarm A */
  RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
  RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
  RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

  /* Configure the RTC Alarm A register */
  RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
  printf("\n\r>> !! RTC Set Alarm success. !! <<\n\r");
  RTC_AlarmShow();

  /* Enable the RTC Alarm A Interrupt */
  RTC_ITConfig(RTC_IT_ALRA, ENABLE);
   
  /* Enable the alarm  A */
  RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
}

/**
  * @brief  Display the current time on the Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_TimeShow(void)
{
  /* Get the current Time */
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
  printf("\n\r  The current time is :  %0.2d:%0.2d:%0.2d \n\r", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
}

/**
  * @brief  Display the current time on the Hyperterminal.
  * @param  None
  * @retval None
  */
void RTC_AlarmShow(void)
{
  /* Get the current Alarm */
  RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
  printf("\n\r  The current alarm is :  %0.2d:%0.2d:%0.2d \n\r", RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours, RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes, RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds);
}


/**
  * @brief  Gets numeric values from the hyperterminal.
  * @param  None
  * @retval None
  */
uint8_t USART_Scanf(uint32_t value)
{
  uint32_t index = 0;
  uint32_t tmp[2] = {0, 0};

  while (index < 2)
  {
    /* Loop until RXNE = 1 */
    while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_RXNE) == RESET)
    {}
    tmp[index++] = (USART_ReceiveData(EVAL_COM1));
    if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))
    {
      printf("\n\r Please enter valid number between 0 and 9 \n\r");
      index--;
    }
  }
  /* Calculate the Corresponding value */
  index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
  /* Checks */
  if (index > value)
  {
    printf("\n\r Please enter valid number between 0 and %d \n\r", value);
    return 0xFF;
  }
  return index;
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(EVAL_COM1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET)
  {}

  return ch;
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
