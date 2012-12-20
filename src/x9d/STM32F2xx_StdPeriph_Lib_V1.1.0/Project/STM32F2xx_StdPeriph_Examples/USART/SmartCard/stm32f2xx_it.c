/**
  ******************************************************************************
  * @file    USART/Smartcard/stm32f2xx_it.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
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
#include "smartcard.h"
#include "stm322xg_eval.h"

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_SmartCard
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern __IO uint32_t CardInserted;
extern __IO uint32_t TimingDelay;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
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
  * @brief  This function handles PendSV_Handler exception.
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
  /* Decrement the TimingDelay variable */
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}

/******************************************************************************/
/*            STM32F2xx Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles SC_USART global interrupt request.
  * @param  None
  * @retval None
  */
void SC_USART_IRQHandler(void)
{
  /* If a Frame error is signaled by the card */
  if(USART_GetITStatus(SC_USART, USART_IT_FE) != RESET)
  {
    USART_ReceiveData(SC_USART);

    /* Resend the byte that failed to be received (by the Smartcard) correctly */
    SC_ParityErrorHandler();
  }
  
  /* If the SC_USART detects a parity error */
  if(USART_GetITStatus(SC_USART, USART_IT_PE) != RESET)
  {
    /* Enable SC_USART RXNE Interrupt (until receiving the corrupted byte) */
    USART_ITConfig(SC_USART, USART_IT_RXNE, ENABLE);
    /* Flush the SC_USART DR register */
    USART_ReceiveData(SC_USART);
  }
  
  if(USART_GetITStatus(SC_USART, USART_IT_RXNE) != RESET)
  {
    /* Disable SC_USART RXNE Interrupt */
    USART_ITConfig(SC_USART, USART_IT_RXNE, DISABLE);
    USART_ReceiveData(SC_USART);
  }
  
  /* If a Overrun error is signaled by the card */
  if(USART_GetITStatus(SC_USART, USART_IT_ORE) != RESET)
  {
    USART_ReceiveData(SC_USART);
  }
  /* If a Noise error is signaled by the card */
  if(USART_GetITStatus(SC_USART, USART_IT_NE) != RESET)
  {
    USART_ReceiveData(SC_USART);
  }
}

/**
  * @brief  This function handles the smartcard detection interrupt request.
  * @param  None
  * @retval None
  */
void SC_OFF_EXTI_IRQHandler(void)
{
  if(EXTI_GetITStatus(SC_OFF_EXTI_LINE) != RESET)
  {
    /* Clear EXTI Line Pending Bit */
    EXTI_ClearITPendingBit(SC_OFF_EXTI_LINE);

    /* Toggle LED1..4 */
    STM_EVAL_LEDToggle(LED1);
    STM_EVAL_LEDToggle(LED2);
    STM_EVAL_LEDToggle(LED3);
    STM_EVAL_LEDToggle(LED4);
    
    /* Smartcard detected */
    CardInserted = 1;

    /* Enable CMDVCC */
    SC_PowerCmd(ENABLE);

    /* Reset the card */
    SC_Reset(Bit_RESET);
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
