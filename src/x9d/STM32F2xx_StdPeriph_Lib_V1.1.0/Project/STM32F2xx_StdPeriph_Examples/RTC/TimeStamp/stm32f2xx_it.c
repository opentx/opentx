/**
  ******************************************************************************
  * @file    RTC/TimeStamp/stm32f2xx_it.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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
#include "stm32f2xx_it.h"
#include "main.h"

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup RTC_TimeStamp
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F2xx Peripherals Interrupt Handlers                   */
/******************************************************************************/

/**
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
  /* Clear the TimeStamp registers */
  if(EXTI_GetITStatus(WAKEUP_BUTTON_EXTI_LINE) != RESET)
  {
    /* Turn LED1 ON and LED2 OFF */
    STM_EVAL_LEDOn(LED1);
    STM_EVAL_LEDOff(LED2);

    /* Clear The TSF Flag (Clear TimeStamp Registers) */
    RTC_ClearFlag(RTC_FLAG_TSF);
    printf("\r\n******************** TimeStamp Event Cleared ********************");
    /* Clear the Wakeup Button EXTI line pending bit */
    EXTI_ClearITPendingBit(WAKEUP_BUTTON_EXTI_LINE);
  }
}
 
/**
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  /* TimeStamp Event detected */
  if(EXTI_GetITStatus(TAMPER_BUTTON_EXTI_LINE) != RESET)
  {
    /* Turn LED4 ON */
    STM_EVAL_LEDOn(LED4);
    printf("\r\n******************** TimeStamp Event Occurred ********************");
    /* Clear the TAMPER Button EXTI line pending bit */
    EXTI_ClearITPendingBit(TAMPER_BUTTON_EXTI_LINE); 
  }

  /* To display Current RTC Time and Date Calendar */
  if(EXTI_GetITStatus(KEY_BUTTON_EXTI_LINE) != RESET)
  {
    /* Turn LED4 ON */
    STM_EVAL_LEDOn(LED4);

    /* Clear the KEY Button EXTI line pending bit */
    EXTI_ClearITPendingBit(KEY_BUTTON_EXTI_LINE); 

    /* Display the Time */
    RTC_TimeShow();
    /* Display the date */
    RTC_DateShow();
    /* Display the TimeStamp */
    RTC_TimeStampShow();
  }
}

/******************************************************************************/
/*                 STM32F2xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f2xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
