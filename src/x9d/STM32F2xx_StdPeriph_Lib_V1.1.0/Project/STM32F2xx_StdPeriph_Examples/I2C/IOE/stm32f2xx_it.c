/**
  ******************************************************************************
  * @file    I2C/IOE/stm32f2xx_it.c 
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

/** @addtogroup I2C_IOE
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
  TimingDelay_Decrement();
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
  * @brief  This function handles External line 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI0_IRQHandler(void)
{
  if(EXTI_GetITStatus(WAKEUP_BUTTON_EXTI_LINE) != RESET)
  {
    /* Toggle LD3 */
    STM_EVAL_LEDToggle(LED3);

    LCD_DisplayStringLine(Line4, (uint8_t *)"IT:  WAKEUP Pressed ");

    EXTI_ClearITPendingBit(WAKEUP_BUTTON_EXTI_LINE);
  }
}

/**
  * @brief  This function handles External line 2 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI2_IRQHandler(void)
{
  if(EXTI_GetITStatus(IOE_IT_EXTI_LINE) != RESET)
  {
    
#ifdef IOE_INTERRUPT_MODE   
    static JOY_State_TypeDef JoyState = JOY_NONE;
    static TS_STATE* TS_State;
    
    /* Check if the interrupt source is the Touch Screen */
    if (IOE_GetGITStatus(IOE_1_ADDR, IOE_TS_IT) & IOE_TS_IT)
    {
      /* Update the structure with the current position */
      TS_State = IOE_TS_GetState();  
      
      if ((TS_State->TouchDetected) && (TS_State->Y < 220) && (TS_State->Y > 180))
      {
        if ((TS_State->X > 10) && (TS_State->X < 70))
        {
          LCD_DisplayStringLine(Line6, (uint8_t *)" LD4                ");
          STM_EVAL_LEDOn(LED4);
        }
        else if ((TS_State->X > 90) && (TS_State->X < 150))
        {
          LCD_DisplayStringLine(Line6, (uint8_t *)"      LD3           ");
          STM_EVAL_LEDOn(LED3);
        }
        else if ((TS_State->X > 170) && (TS_State->X < 230))
        {
          LCD_DisplayStringLine(Line6, (uint8_t *)"           LD2      ");
          STM_EVAL_LEDOn(LED2);
        }     
        else if ((TS_State->X > 250) && (TS_State->X < 310))
        {
          LCD_DisplayStringLine(Line6, (uint8_t *)"                LD1 ");
          STM_EVAL_LEDOn(LED1);
        }
      }
      else
      {
        STM_EVAL_LEDOff(LED1);
        STM_EVAL_LEDOff(LED2);
        STM_EVAL_LEDOff(LED3);
        STM_EVAL_LEDOff(LED4);
      }    
      
      /* Clear the interrupt pending bits */    
      IOE_ClearGITPending(IOE_1_ADDR, IOE_TS_IT);      
    }
    else if (IOE_GetGITStatus(IOE_2_ADDR, IOE_GIT_GPIO))
    {
      /* Get the Joytick State */
      JoyState = IOE_JoyStickGetState();
      
      switch (JoyState)
      {
      case JOY_NONE:
        LCD_DisplayStringLine(Line5, (uint8_t *)"JOY: IT  ----        ");
        break;
      case JOY_UP:
        LCD_DisplayStringLine(Line5, (uint8_t *)"JOY: IT  UP         ");
        break;     
      case JOY_DOWN:
        LCD_DisplayStringLine(Line5, (uint8_t *)"JOY: IT DOWN        ");
        break;          
      case JOY_LEFT:
        LCD_DisplayStringLine(Line5, (uint8_t *)"JOY: IT LEFT        ");
        break;         
      case JOY_RIGHT:
        LCD_DisplayStringLine(Line5, (uint8_t *)"JOY: IT  RIGHT        ");
        break;                 
      case JOY_CENTER:
        LCD_DisplayStringLine(Line5, (uint8_t *)"JOY: IT CENTER       ");
        break; 
      default:
        LCD_DisplayStringLine(Line5, (uint8_t *)"JOY: IT ERROR      ");
        break;         
      }   
      
      /* Clear the interrupt pending bits */    
      IOE_ClearGITPending(IOE_2_ADDR, IOE_GIT_GPIO);
      IOE_ClearIOITPending(IOE_2_ADDR, IOE_JOY_IT);     
    }
    else
    {
      IOE_ClearGITPending(IOE_1_ADDR, ALL_IT);
      IOE_ClearGITPending(IOE_2_ADDR, ALL_IT);
    }
#endif /* IOE_INTERRUPT_MODE */
    
    EXTI_ClearITPendingBit(IOE_IT_EXTI_LINE);
  }  
}

/**
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  if(EXTI_GetITStatus(KEY_BUTTON_EXTI_LINE) != RESET)
  {
    /* Toggle LD1 */
    STM_EVAL_LEDToggle(LED1);

    LCD_DisplayStringLine(Line4, (uint8_t *)"IT:   KEY Pressed   ");
	
    EXTI_ClearITPendingBit(KEY_BUTTON_EXTI_LINE);
  }

  if(EXTI_GetITStatus(TAMPER_BUTTON_EXTI_LINE) != RESET)
  {
    /* Toggle LD2 */
    STM_EVAL_LEDToggle(LED2);

    LCD_DisplayStringLine(Line4, (uint8_t *)"IT: TAMPER Pressed  ");
   
    EXTI_ClearITPendingBit(TAMPER_BUTTON_EXTI_LINE);
  }
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
